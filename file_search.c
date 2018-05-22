#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>

// Desc: Function to print any children files/directories of the directory passed in containing the term
void visitDir(char* term, char* directory){
	// DIR pointer to account for return value of opendir
	DIR* dirPtr;
	// Dirent struct pointer to account for readdir return value
	struct dirent *direntPtr;

	// Opens passed in directory and takes in path of specified directory
	// Returns NULL if directory is unable to be opened	
	dirPtr = opendir(directory);
	
	// If directory unable to be opened
	if(dirPtr == NULL){
		printf("Cannot open directory\n");
		return;
	}
	
	char* dirPath;

	// Iterate through directories in absolute path specified
	// readDir takes in opened directory's DIR pointer 
	// Returns value into a struct dirent pointer (direntPtr)
	// If dirent pointer value is NULL, end of directory opened	
	while((direntPtr = readdir(dirPtr)) != NULL){
		//Allocates memory for directory name
		dirPath = malloc(256);
		//Resets dirPath to be the directory passed in the function
		strcpy(dirPath, directory);

		//Check to see whether the file is a directory
		if(direntPtr -> d_type == DT_DIR){
			//Checks to see if the directory is . or .. to ignore and prevent infinite recursion
			if(strcmp(".", direntPtr -> d_name)  != 0 && strcmp("..", direntPtr -> d_name) != 0){
				//Concatinates a '/' and the name of the directory found
				strcat(dirPath, "/");
				strcat(dirPath, direntPtr -> d_name);
				
				//check if the term is a substring of the name
				if(strstr(direntPtr -> d_name, term) != NULL){
					printf("%s:\n", dirPath); //appends a ':' to signify directory type
				}

				//Recursively call function to view inside the directory
				visitDir(term, dirPath);
			}
		}
		else{
			//check if the term is a substring of the name
			if(strstr(direntPtr -> d_name, term) != NULL){
				//Concats a '/' and the file name
				strcat(dirPath, "/");
				strcat(dirPath, direntPtr -> d_name);
				printf("%s\n", dirPath);
			}
		}
		// Frees dynamically allocated memory
		free(dirPath);
	}	
	
	return;

}

int main(int argc, char** argv){	
	if(argc == 3){
		//Ensuring that the <starting directory> begins with a '/' and doesn't end with a '/' 
		if (argv[2][0] != '/' || argv[2][strlen(argv[2]) - 1] == '/'){
			printf("Starting Directory Argument is invalid\n");
			return 0;
		}
		
		// Saves startTime of function (clocks before function has started)
		clock_t startTime = clock();

		//Function call passing in search term (param 1) and starting directory (param 2) from argv
		visitDir(argv[1], argv[2]);

		// Saves endTime of function (clocks when function is finished executing)
		clock_t endTime = clock();
		
		// Start and End time difference saved in elapsedTime as seconds
		double elapsedTime = (double)(endTime - startTime)/ CLOCKS_PER_SEC;

		// Multiplied by 1000 to account for Milliseconds
		elapsedTime = elapsedTime * 1000;
		printf("\nTime: %fms\n", elapsedTime); // Prints as float 

	}
	else{
		//Error with number of arguments (not 3)
		printf("Invalid Number of Arguments \n");
	}

	return 0;
}
