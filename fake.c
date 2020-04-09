#include <errno.h>
#include "fake.h"

recipe_t ** rule(recipe_t ** pointers_to_recipes, char *buf, int line){
	char *rule_target;
	pointers_to_recipes = realloc(pointers_to_recipes, (line + 1) *sizeof(recipe_t*));
	pointers_to_recipes[line] = calloc(1, sizeof(recipe_t));
	pointers_to_recipes[line]->deps  = malloc(sizeof(char *));
	//pointers_to_recipes[line]->deps[0] = malloc(sizeof(char *));
	
	rule_target = strsep(&buf, ":");


	if (strcmp(rule_target,buf)){
		pointers_to_recipes[line]->target = strdup(rule_target);
		
		printf("The Targets are: %s \n",pointers_to_recipes[line]->target);

		char *deps = strsep(&buf, " ");
		int count =0;
		while (deps){
            deps = strsep(&buf, " ");
			if (deps == NULL){
				break;
			}
			pointers_to_recipes[line]->deps = realloc(pointers_to_recipes[line]->deps, (count+1) *sizeof(char *));
			pointers_to_recipes[line]->deps[count] = strdup(deps);
			
			printf("The deps are: %s \n",pointers_to_recipes[line]->deps[count]);
			count ++;
		}
		pointers_to_recipes[line]->dep_count = count;

	}
	return pointers_to_recipes;

}

void cmds(recipe_t ** pointers_to_recipes, char *buf, int line, int count){
	char * tmp;
	tmp = strsep(&buf, "\t");

	pointers_to_recipes[line]->commands = realloc(pointers_to_recipes[line]->commands, (count+2) *sizeof(char *));
	pointers_to_recipes[line]->commands[count] = buf;

	pointers_to_recipes[line]->commands[count+1] = NULL;
	printf("The commands %d are: %s \n", line , pointers_to_recipes[line]->commands[count]);

}

int main(){
    FILE *fake_reader = fopen("Fakefile1","r");
    char *reader = calloc(1,1024);//Allocating memeory for file line reader
    char *buf; // pointer to move within the line
    int line = 0;
	int cmd_track;
	parser state = RULE_START;
    
    //Pointers of Pointer for storing all the cards
    recipe_t **pointers_to_recipes = malloc(sizeof(recipe_t*));
    

    if (fake_reader == NULL) 
    { 
        printf("Could not open the Fakefile!!! \n"); 
        return 0; 
    }
    
    while(1){
		fgets(reader, 1024, fake_reader);
			
		if (feof(fake_reader)){
			break;
		}
        
		//printf("line: %d \n", line);
        if (*reader == '#'){
            continue;
        }

		if ( (*reader == '\n') && (state == COMMANDS)){
            state = RULE_START;
			line++;
			continue;
        }

		if ((*reader == '\n') || (reader == NULL)){
			continue;
		}

        buf = reader;
        
        if (state == RULE_START){
            pointers_to_recipes = rule(pointers_to_recipes, buf, line);
            state = COMMANDS;
			cmd_track = 0;
			pointers_to_recipes[line]->commands = calloc(1,sizeof(char *));
			continue;
        }

		
		if (state == COMMANDS){
			if (*reader == '\t'){
				cmds(pointers_to_recipes, buf, line, cmd_track);
				pointers_to_recipes[line]->cmd_count = cmd_track;
				cmd_track++;
				continue;
			}

			else{
				printf("Error!!!! in file format!!!");
				break;
			}
			
		}
		

    }

	//Freeing all the used memory
	free(reader);
	int i;
	for (i=0; i<line; i++){
		free(pointers_to_recipes[i]->target);

		int count;
		for(count=0; count < pointers_to_recipes[i]->dep_count; count++){
			free(pointers_to_recipes[i]->deps[count]);
		}
		free(pointers_to_recipes[i]->deps);
		
		for(count=0; i < pointers_to_recipes[i]->cmd_count; count++){
			free(pointers_to_recipes[i]->commands[count]);
			count++;
		}
		free(pointers_to_recipes[i]->commands);

		free(pointers_to_recipes[i]);
	}
	free(pointers_to_recipes);
	
    fclose(fake_reader); 

}