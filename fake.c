#include <errno.h>
#include "fake.h"

void rule(recipe_t ** pointers_to_recipes, char *buf, int line){
	char *rule_target;
	
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
		}	
	}

}

void cmds(recipe_t ** pointers_to_recipes, char *buf, int line, int count){
	char * tmp;
	tmp = strsep(&buf, "\t");

	pointers_to_recipes[line]->commands = realloc(pointers_to_recipes[line]->commands, (count+1) *sizeof(char *));
	pointers_to_recipes[line]->commands[count] = strdup(buf);
	printf("The commands are: %s \n",pointers_to_recipes[line]->commands[count]);

}

int main(){
    FILE *fake_reader = fopen("Fakefile1","r");
    char *reader = calloc(1024,1);//Allocating memeory for file line reader
    char *buf; // pointer to move within the line
    int line = 0;
	int cmd_track;
	parser state = RULE_START;
    
    //Pointers of Pointer for storing all the cards
    recipe_t **pointers_to_recipes; //= malloc(sizeof(recipe_t*));
    //pointers_to_recipes[0] = malloc(sizeof(recipe_t));

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

		if (*reader == '\n'){
            state = RULE_START;
			line++;
            continue;
        }

        buf = reader;
        
        if (state == RULE_START){
			pointers_to_recipes = realloc(pointers_to_recipes, (line + 1) *sizeof(recipe_t*));
			pointers_to_recipes[line] = malloc(sizeof(recipe_t));

            rule(pointers_to_recipes, buf, line);
            state = COMMANDS;
			cmd_track = 0;
			continue;
        }

		/*
		if (state == COMMANDS){
			if (*reader == '\t'){
				cmds(pointers_to_recipes, buf, line, cmd_track);
				cmd_track++;
				continue;
			}

			else{
				printf("Error!!!! in file format!!!");
				break;
			}
			
		}
		*/

    }
    

    fclose(fake_reader); 
}