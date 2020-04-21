#include <errno.h>
#include "fake.h"

//Trims the leading annd ending white spaces
//source/credit: https://stackoverflow.com/a/122721
char *trimwhitespace(char *str)
{
	char *end;

	// Trim leading space
	while(isspace((unsigned char)*str)) str++;

	if(*str == 0)  // All spaces?
	return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char)*end)) end--;

	// Write new null terminator character
	end[1] = '\0';

	return str;
}

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

		buf = trimwhitespace(buf);
		char *deps;

		int count =0;
		while (1){
			deps = strsep(&buf, " ");
			if (deps == NULL){
				break;
			}
			//Ignore the unncessary spaces and tabs
			if (*deps == '\0' || *deps == '\t'){
				continue;
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
		printf("The Targets are: |%s| \n",pointers_to_recipes[i]->target);

		for(count=0; count < pointers_to_recipes[i]->dep_count; count++){
			printf("The %s's  deps %d is: |%s| \n",pointers_to_recipes[i]->target, count,pointers_to_recipes[i]->deps[count]);
		}
		
		for(count=0; count < pointers_to_recipes[i]->cmd_count; count++){
			printf("The %s's commands %d is: |%s| \n", pointers_to_recipes[i]->target, count , pointers_to_recipes[i]->commands[count]);
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

/*
 * loop over commands by sharing
 * pipes.
 */
//source/credit: https://gist.github.com/iomonad/a66f6e9cfb935dc12c0244c1e48db5c8
int pipeline(char ***cmd)
{
	int fd[2];
	pid_t pid, wpid;
	int fdd = 0;				/* Backup */
	int status = 0;

	while (*cmd != NULL) {
		pipe(fd);
		if ((pid = fork()) == -1) {
			perror("fork \n");
			exit(1);
		}
		else if (pid == 0) {
			dup2(fdd, 0);
			if (*(cmd + 1) != NULL) {
				dup2(fd[1], 1);
			}
			close(fd[0]);
			execvp((*cmd)[0], *cmd);
			exit(1);
		}
		else {	
			/* Collect childs */
			while ((wpid = wait(&status)) > 0){
				if ( WIFEXITED(status)){
					if(WEXITSTATUS(status) >0 ){
						printf("Child failed \n");
						return -1;
					}
				}
			}

			close(fd[1]);
			fdd = fd[0];
			cmd++;
		}
	}
	return 0;
}

//Handler for multiple pipe case
int multi_pipe_parser(char *command, int j){
	char *command_count = command;
	j = j+2;

	char ***cmd = malloc( j * sizeof(char **));

	int k = 0; 
	char *command_tmp;

	//Split the command based on pipes
	command_tmp = strsep(&command, "|");
	

    command_tmp = trimwhitespace(command_tmp);
	command = trimwhitespace(command);

    while (1){
		char * arg[30];
		int i = 0;
        char *store;
		
		cmd[k] = malloc(sizeof(char *));
		cmd[k][i] = strdup(strsep(&command_tmp," "));
        
		while(command_tmp !=NULL){
			
            if (*command_tmp == '\"'){
                command_tmp++;
				++i;
				cmd[k] = realloc(cmd[k], (i+1) *sizeof(char *));
				
                cmd[k][i] = strdup(strsep(&command_tmp,"\""));
            }
            else
            {
                store = strsep(&command_tmp," ");
				if (*store != '\0'){
					++i;
					cmd[k] = realloc(cmd[k], (i+1) *sizeof(char *));
                	cmd[k][i] = strdup(store);
				}

				
            }
		}
		cmd[k] = realloc(cmd[k], (i+2) *sizeof(char *));
		cmd[k][++i] = NULL; // Add NULL as last argument for exec
		
        if (command == NULL){
            break;
        }

		command_tmp = strsep(&command, "|");
        command_tmp = trimwhitespace(command_tmp);
        k++;
	}
	cmd[++k] = NULL;

	int return_value;
    return_value =  pipeline(cmd) ;
    
	for (int h=0; h<k;h++){
        int m =0;
		while(cmd[h][m] != NULL){
            free(cmd[h][m]);
            m++;
        } 
        free(cmd[h]);
        
    }
	free(cmd);

	return return_value;
}


//Executes the feeded command, is bascially a shell ;) , tried  my best to make it so.
int excutecmd(char *command){
	printf("%s \n", command);

	//Lets see how many pipes we are about to deal with....
	char *pipe_test_store = command; //temporary space to preserve command pointer
	int pipe_num_count; //store the total number
	for (pipe_num_count=0; pipe_test_store[pipe_num_count]; pipe_test_store[pipe_num_count]=='|' ? pipe_num_count++ : *pipe_test_store++);

	//did we find more than 1 pipe???
	if (pipe_num_count>1){
		//Yes sargent!!!
		//Oh boii, we have no work on this function anymore
		//Direct the flow to the multi pipe hadling function and return whatever value they give us back
		return multi_pipe_parser(command, pipe_num_count);
	}

	//No, sargent!!!
	//Well, then private, lets see what we have in our hands.
	char *args[50];
	int i=0;
	char *store;

	int pipefd[2];
	int pipe_check = 0;
	int STD_check  = 0;
	char *command_tmp;

	//Incase of piping
	if (strchr(command, '|') != NULL){
		pipe_check = 1;
	}
	
	if (strchr(command, '>') != NULL){
		STD_check = 1;
	}
	
	else if (strchr(command, '<') != NULL ){
		STD_check = 2;
	}

	else if ((strchr(command, '<')!= NULL) && (strchr(command, '>')!= NULL) ){
		STD_check = 3;
	}

	//printf("The pipe check value at first is: %d\n", pipe_check);

	if (pipe_check == 1){
		pipe(pipefd);
	}

	//Forking for 2 processes
	pid_t pid1 = fork();


	//Child process
	if (0 == pid1) {
		//Print user feedback on what command is being executed

		if ((pipe_check == 1) && (STD_check == 1)){
			//char *command_tmp = strsep(&command, "|");
			//command = command_tmp;//extracting the "ls -la" section
			command = strsep(&command, "|");

			// close the read end of the pipe
			close(pipefd[0]);
			// make the write end the process's standard output
			dup2(pipefd[1],STDOUT_FILENO);

		}

		if ( (pipe_check == 0) && STD_check == 2){
			command_tmp = strsep(&command, "<");
			
			command_tmp = trimwhitespace(command_tmp);
			command = trimwhitespace(command);

			int rfd = open(command, O_RDONLY);
			// make the opened file the process's standard output
			dup2(rfd, STDIN_FILENO);

			command = command_tmp;
		}
		
		else if ( (pipe_check == 0) && STD_check == 1){
			command_tmp = strsep(&command, ">");
			
			command_tmp = trimwhitespace(command_tmp);
			command = trimwhitespace(command);

			// open a file named FILTER to redirect output into
			int rfd = open(command, O_WRONLY|O_CREAT|O_TRUNC, 0644);
			// make the opened file the process's standard output
			dup2(rfd, STDOUT_FILENO);

			command = command_tmp;
		}

		//Split the command_tmp based on spaces or /"
		args[i] = strsep(&command," ");
		while(command !=NULL){
            if (*command == '\"'){
                command++;
                args[++i] = strsep(&command,"\"");
            }
            else{
                store = strsep(&command," ");
				if (*store != '\0'){
					args[++i] = store;
				}
            }
		}
		args[++i] = NULL; // Add NULL as last argument for exec
		
		//Execute the command
		if ((execvp(args[0], args) < 0)) {
			perror("execvp error");
			exit(-1);
		}
	}

	//Parent Process(tmp)
	else {
		pid_t pid2;
		char *args2[50];
		if (pipe_check == 1){
			pid2 = fork();
			
			//Child2
			if (0 == pid2) {
				if ((pipe_check == 1) && (STD_check == 1)){
					// close the write end of the pipe
					close(pipefd[1]);
					// make the read end the process's standard input
					dup2(pipefd[0], STDIN_FILENO);

					//only keeep the filename in command
					command_tmp = strsep(&command, ">");
					
					//Only keep the second portion of command after '|'
					strsep(&command_tmp, "|");

					command_tmp = trimwhitespace(command_tmp);
					command = trimwhitespace(command);

					//Split the command_tmp based on spaces or /"
					i=0;
					args2[i] = strsep(&command_tmp," ");
					while(command_tmp !=NULL){
						if (*command_tmp == '\"'){
							command_tmp++;
							args2[++i] = strsep(&command_tmp,"\"");
						}
						else{
							store = strsep(&command_tmp," ");
							if (*store != '\0'){
								args2[++i] = store;
							}
						}
					}
					args2[++i] = NULL; // Add NULL as last argument for exec
					
					
					// open a file named FILTER to redirect output into
					int rfd = open(command, O_WRONLY|O_CREAT|O_TRUNC, 0644);
					// make the opened file the process's standard output
					dup2(rfd, STDOUT_FILENO);
					
				}
				if ((execvp(args2[0], args2) < 0)) {
					perror("execvp error");
					exit(-1);
				}
			}
		
			//Final parent process for fork with pipe
			else{

				close(pipefd[0]);
				close(pipefd[1]);
				int status1;
				int status2;
				waitpid(pid1, &status1, 0);
				waitpid(pid2, &status2, 0);
				//Handel with the error occured in the child process
				int out_check = 0;

				if ( WIFEXITED(status1)){
					if(WEXITSTATUS(status1) >0 ){
						printf("Child 1 failed \n");
						out_check++;
					}
				}
				if ((WIFEXITED(status2) && pipe_check == 1)){
					if((WEXITSTATUS(status2) && pipe_check == 1)){
						printf("Child 2 failed \n");
						out_check++;
					}
				}
				if (out_check >0){
					return -1;
				}

			}
		}

		//Parent for fork without pipe
		else
		{
			int status;
			//wait for child to end
			waitpid(pid1, &status, 0);

			if ( WIFEXITED(status)){
				if(WEXITSTATUS(status) >0 ){
					printf("Child 1 failed \n");
					return -1;
				}
			}
			return 0;
		}
		

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
	for (curr_line = 0; curr_line<=line; curr_line++){
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

	return -1; //if not found
}

//Checks if the target at index [track] is present within the current directory
int file_presense(recipe_t ** pointers_to_recipes, int track, int index){
	//Now lets check if the current target is also a file that already exist within this directory
	if( access( pointers_to_recipes[track]->target , F_OK ) != -1 ) {
		//Lets get their modified date-time stamps
		time_t target_time = getFileCreationTime( pointers_to_recipes[track]->target);
		time_t dependancy_time = getFileCreationTime(pointers_to_recipes[track]->deps[index]);
		
		//If the time stamp on the dependancy is newer than target
		if (target_time >= dependancy_time){
			return 1; // Take a record of it
		}
		
	}
	return 0;
}

//Processing the command
int processing(recipe_t ** pointers_to_recipes, int line, int track){
	int valdity_check = 0;
	int curr_line;
	int dep_update_count = 0;
	int file_test = 0;

	//Flag for if the dependancy is found as a target
	int found;

	if(pointers_to_recipes[track]->dep_count == 0){
		if( access( pointers_to_recipes[track]->target , F_OK ) != -1 ) {
			printf("fake: %s is upto date \n", pointers_to_recipes[track]->target);
			return 0;
		}
	}

	//Loop through all the dependencies
	if (pointers_to_recipes[track]->dep_count > 0){
		for(int index= 0; index< pointers_to_recipes[track]->dep_count; index++){

			//Lets check if an actual file exist within the current directory
			if( access( pointers_to_recipes[track]->deps[index] , F_OK ) != -1 ) {
				//Yes, Great
				//Call the function for the test and recored its return value
				file_test =file_presense(pointers_to_recipes, track, index);
				dep_update_count = dep_update_count + file_test;

				//Skip this dependecy if file already found
				if (file_test == 1){
					continue; 
				}
			}
		
			//Well if not lets check if a target is avilable with the name
			//Call the function to check if there is any target matching the dependancy
			//If error is generated during exectution throw an error
			found = target_search(pointers_to_recipes, line, pointers_to_recipes[track]->deps[index]);
			if (found == -2 ){
				return -1;
			}

			//Now again look for if an actual file exist within the current directory
			if( access( pointers_to_recipes[track]->deps[index] , F_OK ) != -1 ) {
				dep_update_count = dep_update_count + file_presense(pointers_to_recipes, track, index);
			}
			
			//if it already satisfied the target search test then skip to the next dependancy
			else if (found == 1){
				continue;
			}
			
			//Still not found??? Too bad. Raise an error with -1
			else{
				printf("Error dependency file {%s} cannot be found from target: {%s}. Also the number of dependecies is: %d!! \n",pointers_to_recipes[track]->deps[index], pointers_to_recipes[track]->target, pointers_to_recipes[track]->dep_count);
				return -1;
			}

		}

		//Remember that we took a record for if dependancy is newer than target
		//If the condition is true for all the all the dependancies then,
		//DO NOT EXCUTE THE COMMAND(S)
		if(dep_update_count == pointers_to_recipes[track]->dep_count){
			printf("fake: %s is upto date \n", pointers_to_recipes[track]->target);
			return 0;
		}
	}
	//If is FALSE for even 1  of dependencies THEN,
	//EXCUTE THE COMMAND(S)
	for(int cmd_count=0; cmd_count< pointers_to_recipes[track]->cmd_count; cmd_count++){
		if (excutecmd(pointers_to_recipes[track]->commands[cmd_count]) == -1){
			printf("fake: Execution error \n");
			return -1;
		}
	}

	//Function end
	return 0;
}

int main(int argc, char *argv[]){
	char *filename;
	int file_change_check = 0;
	if ((argc ==3) && (strcmp("-f", argv[1]) ==0) ){
		if( access( argv[2] , F_OK ) != -1 ) {
			filename = argv[2];
			file_change_check = 1;
		}
		else
		{
			printf("fake: Input file not found!!!");
		}
		
		
	}
	else{
		filename = "Fakefile";
	}
	
	

    FILE *fake_reader = fopen(filename,"r");
    char *reader = calloc(1,1024);//Allocating memeory for file line reader
    char *buf; // pointer to move within the line
    int line = 0;
	int cmd_track;
	parser state = RULE_START;
	int condition = 1;

	int validation_check = 0;
    
    //Pointers of Pointer for storing all the cards
    recipe_t **pointers_to_recipes = malloc(sizeof(recipe_t*));

    if (fake_reader == NULL) 
    { 
        printf("Could not open the Fakefile!!! \n"); 
        return -1; 
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
					printf("fake: Error!!!! in file format!!! \n");
					condition = 0;
					exit(-1);
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
					exit(-1);
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

	int specific_check;

	//Display only if eveything went well
	if (condition){
		//display(pointers_to_recipes, line);
	}

	if((file_change_check == 0) && (argc>=2) ){

		for (int arg_index = 1;arg_index< argc; arg_index++){
			specific_check  = target_search(pointers_to_recipes, line, argv[arg_index]);

			if (specific_check == -1) {
				printf("fake: target not found!!! \n");
			}
			else if (specific_check == -2)
			{
				printf("fake: execution error!! \n");
			}
			
		}

	}
	
	else if (processing(pointers_to_recipes, line, 0) == -1){
		printf("fake: Fatal execution!!! \n");
		printf("fake: Execution halt!! \n");
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