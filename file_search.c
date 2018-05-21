#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(int argc, char** argv){
	
	if(argc == 3){
		printf("Arguments count vaild\n");
		
		//Ensuring that the <starting directory> begins with a '/' and doesn't end with a '/' 
		if (argv[2][0] != '/' || argv[2][strlen(argv[2]) - 1] == '/'){
			printf("Starting Directory Argument is invalid\n");
			return 0;
		}
	}
	else{
		printf("Invalid Number of Arguments \n");
	}

	return 0;

}
