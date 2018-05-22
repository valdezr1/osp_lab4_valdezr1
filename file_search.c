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

	char* dirPath;

	// Iterate through directories in absolute path specified	
	while((direntPtr = readdir(dirPtr)) != NULL){
		dirPath = malloc(256);
		strcpy(dirPath, directory);

		//Check to see whether the file is a directory
		if(direntPtr -> d_type == DT_DIR){
			if(strcmp(".", direntPtr -> d_name)  != 0 && strcmp("..", direntPtr -> d_name) != 0){
				strcat(dirPath, "/");
				strcat(dirPath, direntPtr -> d_name);
				
				//check if the term is a substring of the name
				if(strstr(direntPtr -> d_name, term) != NULL){
					printf("%s:\n", dirPath);
				}

				//Recursively call function to view inside the directory
				visitDir(term, dirPath);
			}
		}
		else{
			//check if the term is a substring of the name
			if(strstr(direntPtr -> d_name, term) != NULL){
				strcat(dirPath, "/");
				strcat(dirPath, direntPtr -> d_name);
				printf("%s\n", dirPath);
			}
		}
		free(dirPath);
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
