int excutecmd(char *command){
	char *args[10];
	int i=0;
	
	int pipefd[2];
	// need pipe to be shared by both child processes
	//pipe(pipefd);
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
		
		execvp(args[0],args); 
	}
	else {
		//waitpid(pid1, NULL, 0);
		return 0;
	}
	
}