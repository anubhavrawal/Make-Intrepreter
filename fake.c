#include <errno.h>
#include "fake.h"

//Parses and stores the Rule targets and dependencies
int rule(recipe_t ** pointers_to_recipes, char *buf, int line){
	char *rule_target;
	
	//pointers_to_recipes[line]->deps[0] = malloc(sizeof(char *));

	if (strchr(buf, ':') == NULL){
		return -1;
	}
	
	rule_target = strsep(&buf, ":");

	if (strcmp(rule_target,buf)){
		pointers_to_recipes[line]->target = strdup(rule_target);

		char *deps = strsep(&buf, " ");

		int count =0;
		while (deps){
            deps = strsep(&buf, " ");
			if (deps == NULL){
				break;
			}
			pointers_to_recipes[line]->deps = realloc(pointers_to_recipes[line]->deps, (count+1) *sizeof(char *));
			
			pointers_to_recipes[line]->deps[count] = strdup(deps);
			
			count ++;
		}
		pointers_to_recipes[line]->dep_count = count;

	}
	else
	{
		return -1;
	}
	
	
	return 1;
}

//Displayes all the information parsed from the fakefile
void display(recipe_t ** pointers_to_recipes, int line){
	int i;
	int count;
	for (i=0; i<=line; i++){
		printf("The Targets are: %s \n",pointers_to_recipes[i]->target);

		for(count=0; count < pointers_to_recipes[i]->dep_count; count++){
			printf("The %s's  deps %d is: %s \n",pointers_to_recipes[i]->target, count,pointers_to_recipes[i]->deps[count]);
		}
		
		for(count=0; count < pointers_to_recipes[i]->cmd_count; count++){
			printf("The %s's commands %d is: %s \n", pointers_to_recipes[i]->target, count , pointers_to_recipes[i]->commands[count]);
		}
		printf("\n");
	}
}

//Parses and stores the commands
int  cmds(recipe_t ** pointers_to_recipes, char *buf, int line, int count){
	char * tmp;

	tmp = strsep(&buf, "\t");

	if (strcmp(tmp,buf) == 0){
		return -1;
	}

	if (strchr(buf, ':') != NULL){
		return -1;
	}

	int length_cmd = strlen(buf);
	pointers_to_recipes[line]->commands = realloc(pointers_to_recipes[line]->commands, (count+1) *sizeof(char *));
	pointers_to_recipes[line]->commands[count] = strndup(buf, length_cmd -1);
	
	pointers_to_recipes[line]->commands[count][length_cmd -1] = '\0';

	return 1;

}

int excutecmd(char *command){
	char *args[10];
	int i=0;
	
	//Forking for 2 processes
	pid_t pid1 = fork();

	//Child process
	if (0 == pid1) {
		//Print user feedback on what command is being executed
		printf("%s \n", command);

		//Split the command based on spaces
		args[i] = strtok(command," ");
		while(args[i]!=NULL){
			args[++i] = strtok(NULL," ");
		}
		args[i] = NULL; // Add NULL as last argument for exec

		//Print the command being executed
		//for (int k = 0; k < i; ++k) 
		//	printf("%d, %s\n",k, args[k]);
		//printf("\n");
		
		//Execute the command and handeling child process error
		if ((execvp(args[0],args)  < 0)) {
			perror("execlp error");
			return -1;
			exit(-1);
		}
	}
	
	//Parent Process
	else {
		//wait for child to end
		waitpid(pid1, NULL, 0);
		return 0;
	}
	
	return 0;
}

//Returns the modified time for the "path" file
static time_t getFileCreationTime(char *path) {
    struct stat attr;
    stat(path, &attr);
    return attr.st_mtime;
}

//Delcation of function that will be used later on
int processing(recipe_t ** pointers_to_recipes, int line, int track);

//Search if the given string is avialble as a target and processses it
int target_search(recipe_t ** pointers_to_recipes, int line, char *input){
	int curr_line;
	//Check if the "track"-indexed depencency is depended on anyother avilable targets
	//The check always skips the fist elment on the target list
	for (curr_line = 1; curr_line<line; curr_line++){
		//If the dependancy is on a target file within the fakefile
		if (strcmp(input, pointers_to_recipes[curr_line]->target) ==0){
			//Recurcevely execute that target first
			//Check for the return if '-1' then return -1 for error
			if (processing(pointers_to_recipes, line, curr_line) == -1){
				return -2;
			}
			return 1;
			break; //If FOUND stop searching
		}
	}
	return -1;
}

//Processing the command
int processing(recipe_t ** pointers_to_recipes, int line, int track){
	int valdity_check = 0;
	int curr_line;
	int dep_update_count = 0;

	//Loop through all the dependencies
	if (pointers_to_recipes[track]->dep_count){
		for(int index= 0; index< pointers_to_recipes[track]->dep_count; index++){
			//Flag for if the dependancy is found as a target
			int found;
			//Call the function to check if there is any target matching the dependancy
			found = target_search(pointers_to_recipes, line, pointers_to_recipes[track]->deps[index]);
			//If error is generated during exectution throw an error
			if (found == -2){
				return -1;
			}

			//Now look for if an actual file exist within the current directory
			if( access( pointers_to_recipes[track]->deps[index] , F_OK ) != -1 ) {
				//Yes, Great
				//Now lets check if the current target is also a file that already exist within this directory
				if( access( pointers_to_recipes[track]->target , F_OK ) != -1 ) {
					//Lets get their modified date-time stamps
					time_t target_time = getFileCreationTime( pointers_to_recipes[track]->target);
					time_t dependancy_time = getFileCreationTime(pointers_to_recipes[track]->deps[index]);
					
					//If the time stamp on the dependancy is newer than target
					if (target_time >= dependancy_time){
						dep_update_count++; // Take a record of it
					}
					
				}
			}

			//Still not found??? Too bad. Raise an error with -1
			else{
				printf("Error dependency file %s cannot be found!! \n",pointers_to_recipes[track]->deps[index]);
				return -1;
			}

		}

		//Remember that we took a record for if dependancy is newer than target
		//If the condition is true for all the all the dependancies then,
		//DO NOT EXCUTE THE COMMAND(S)
		if( (dep_update_count == pointers_to_recipes[track]->dep_count) && (pointers_to_recipes[track]->dep_count > 0) ){
			printf("All the dependancy are upto date for %s \n", pointers_to_recipes[track]->target);
			return 0;
		}

		//If is FALSE for even 1  of dependencies THEN,
		//EXCUTE THE COMMAND(S)
		for(int cmd_count=0; cmd_count< pointers_to_recipes[track]->cmd_count; cmd_count++){
			if (excutecmd(pointers_to_recipes[track]->commands[cmd_count]) == -1){
				printf("Execution error \n");
				return -1;
			}
		}

	}
	//Function end
	return 0;
}

int main(){
    FILE *fake_reader = fopen("Fakefile1","r");
    char *reader = calloc(1,1024);//Allocating memeory for file line reader
    char *buf; // pointer to move within the line
    int line = 0;
	int cmd_track;
	parser state = RULE_START;
	int condition = 1;

	int validation_check = 0;
    
    //Pointers of Pointer for storing all the cards
    recipe_t **pointers_to_recipes = malloc(sizeof(recipe_t*));
    //pointers_to_recipes[0] = malloc(sizeof(recipe_t));

    if (fake_reader == NULL) 
    { 
        printf("Could not open the Fakefile!!! \n"); 
        return 0; 
    }
    
    while( (condition == 1) && (fgets(reader, 1024, fake_reader)) ){

		if (*reader == '#'){
            continue;
        }
		buf = reader;

		switch (state){
			case COMMANDS:
				if (*reader == '\n'){
					line++;
					state = RULE_START;
				}

				else if (*reader == '\t'){
					validation_check = 0;
					validation_check = cmds(pointers_to_recipes, buf, line, cmd_track);

					if (validation_check == -1){
						printf("Invalid format!!!! CMD state\n");
						condition = 0;
						break;
					}
					cmd_track++;
					pointers_to_recipes[line]->cmd_count = cmd_track;
				}

				else{
					printf("Error!!!! in file format!!! \n");
					condition = 0;
					break;
				}

				break;
			
			case RULE_START:
				if ((*reader == '\t') || (*reader =='\n') || (reader == NULL)){
					break;
				}
				pointers_to_recipes = realloc(pointers_to_recipes, (line + 1) *sizeof(recipe_t*));
				pointers_to_recipes[line] = calloc(1, sizeof(recipe_t));
				pointers_to_recipes[line]->deps  = malloc(sizeof(char *));

				//Creating a tmp memory to get rid of \n while reading the reciepe
				//---------------------------------------------
				char *tmp;
				int len_buf = strlen(buf);
				tmp = strndup(buf, len_buf-1); 
				tmp[len_buf-1] = '\0';
				//-----------------------------------------

				validation_check = 0;	
				validation_check = rule(pointers_to_recipes, tmp, line);
				
				free(tmp);// freeing the temporary located memory

				//if -1 falgged by check then,
				if (validation_check == -1){
					line++;
					printf("Invalid format rule state!!!! \n");
					condition = 0; //breaking the while loop
					break;
				}
				

				state = COMMANDS;
				cmd_track = 0;
				pointers_to_recipes[line]->commands = calloc(1,sizeof(char *));
				continue;

				break;

			default:
				break;
		}

    }
	fclose(fake_reader);

	//If there is just extra spaces with no target or commands
	//Then decrement total number of lines as the last incerement was unnecessary.
	if (state == RULE_START){
		line--;
	}

	//Display only if eveything went well
	if (condition){
		//display(pointers_to_recipes, line);
	}

	if (processing(pointers_to_recipes, line, 0) == -1){
		printf("Fakefie fatal execution!!! \n");
		printf("Execution halt!! \n");
	}

	//Freeing all the used memory
	free(reader);
	int i;
	for (i=0; i<=line; i++){
		free(pointers_to_recipes[i]->target);

		int count;
		for(count=0; count < pointers_to_recipes[i]->dep_count; count++){
			free(pointers_to_recipes[i]->deps[count]);
		}
		free(pointers_to_recipes[i]->deps);
		
		for(count=0; count < pointers_to_recipes[i]->cmd_count; count++){
			free(pointers_to_recipes[i]->commands[count]);
		}
		free(pointers_to_recipes[i]->commands);

		free(pointers_to_recipes[i]);
	}
	free(pointers_to_recipes);

}