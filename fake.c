#include <errno.h>
#include "fake.h"

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
	
	pid_t pid1 = fork();

	if (0 == pid1) {
		args[i] = strtok(command," ");

		while(args[i]!=NULL){
			args[++i] = strtok(NULL," ");
		}
		args[i] = NULL;

		for (int k = 0; k < i; ++k) 
			printf("%d, %s\n",k, args[k]);
		
		printf("\n");
		
		if ((execvp(args[0],args)  < 0)) {
			perror("execlp error");
			return -1;
			exit(-1);
		}
	}
	
	else {
		waitpid(pid1, NULL, 0);
		return 0;
	}
	
	return 0;
}

static time_t getFileCreationTime(char *path) {
    struct stat attr;
    stat(path, &attr);
    return attr.st_mtime;
}

int processing(recipe_t ** pointers_to_recipes, int line, int track){
	int valdity_check = 0;
	int curr_line;
	int dep_update_count = 0;
	if (pointers_to_recipes[track]->dep_count){
		for(int index= 0; index< pointers_to_recipes[track]->dep_count; index++){
			int found = 0;
			
			for (curr_line = 1; curr_line<line; curr_line++){
				//If the dependancy is on a target file within the fakefile
				if (strcmp(pointers_to_recipes[track]->deps[index], pointers_to_recipes[curr_line]->target) ==0){
					if (processing(pointers_to_recipes, line, curr_line) == -1){
						return -1;
					}
					break;
				}
			}
			
			//If the dependency is on a .c or .h files instread of targets within the fakefile
			int dep_len = strlen(pointers_to_recipes[track]->deps[index]);
			char *last_two = &pointers_to_recipes[track]->deps[index][dep_len-2];

			if( access( pointers_to_recipes[track]->deps[index] , F_OK ) != -1 ) { 
				if( access( pointers_to_recipes[track]->target , F_OK ) != -1 ) {
					time_t target_time = getFileCreationTime( pointers_to_recipes[track]->target);
					time_t dependancy_time = getFileCreationTime(pointers_to_recipes[track]->deps[index]);
					if (target_time > dependancy_time){
						dep_update_count++;
					}
					
				}
			}

			else{
				printf("Error dependency file %s cannot be found!! \n",pointers_to_recipes[track]->deps[index]);
				return -1;
			}

		}
		if(dep_update_count == pointers_to_recipes[track]->dep_count){
			printf("All the dependancy are upto date for %s \n", pointers_to_recipes[track]->target);
			return 0;
		}

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