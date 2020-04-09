if(cmd_track > 0){
				pointers_to_recipes[line]->commands[cmd_track+1] = NULL;
			}
			else{
				pointers_to_recipes[line]->commands[cmd_track] = NULL;
			}