#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

int visitDir(char* term, char* directory){
	
	DIR* dirPtr;
	struct dirent *direntPtr;

	//printf("%s\n", directory);
	//printf("visitDir called\n");
		
	dirPtr = opendir(directory);
	
	if(dirPtr == NULL){
		//printf("Cannot open directory\n");
		return 0;
	}

	// Iterate through directories in absolute path specified	
	while((direntPtr = readdir(dirPtr)) != NULL){
		if(direntPtr -> d_type == DT_DIR){
			if(strcmp(".", direntPtr -> d_name)  != 0 && strcmp("..", direntPtr -> d_name) != 0){
				visitDir(term, direntPtr -> d_name);
				if(strstr(direntPtr -> d_name, term) != NULL){
					printf("%s:\n", direntPtr -> d_name);
				}
			}
		}
		else{
			if(strstr(direntPtr -> d_name, term) != NULL){
				printf("%s\n", direntPtr -> d_name);
			}
		}
	}
	
	return 0;

}

int main(int argc, char** argv){
	
	if(argc == 3){
		//printf("Arguments count vaild\n");
		
		//Ensuring that the <starting directory> begins with a '/' and doesn't end with a '/' 
		if (argv[2][0] != '/' || argv[2][strlen(argv[2]) - 1] == '/'){
			printf("Starting Directory Argument is invalid\n");
			return 0;
		}
		
		visitDir(argv[1], argv[2]);

	}
	else{
		printf("Invalid Number of Arguments \n");
	}

	return 0;

}
