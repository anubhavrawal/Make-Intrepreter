//printf("line: %d \n", line);
        
		/*
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
			
			
		}
		*/