#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <pthread.h>

//takes a file/dir as argument, recurses,
// prints name if empty dir or not a dir (leaves)
void recur_file_search(char *file);
void* thread_file_search(void* arg);

//share search term globally (rather than passing recursively)
const char *search_term;

//share four threads globally (rather than passing the same array recursively)
pthread_t threads[4];

//array of strings that would contain the paths (MAXED to 1000)
char* paths[2000];
size_t count = 0;

struct range {
	size_t start;
	size_t end;
};

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("Usage: my_file_search <search_term> <dir>\n");
		printf("Performs recursive file search for files/dirs matching\
				<search_term> starting at <dir>\n");
		exit(1);
	}

	//don't need to bother checking if term/directory are swapped, since we can't
	// know for sure which is which anyway
	search_term = argv[1];

	//open the top-level directory
	DIR *dir = opendir(argv[2]);

	//make sure top-level dir is openable (i.e., exists and is a directory)
	if(dir == NULL)
	{
		perror("opendir failed");
		exit(1);
	}
	
	//start timer for recursive search
	struct timeval start, end;
	gettimeofday(&start, NULL);
	
	//Call the recursive file search to populate paths array and get all of the paths visited
	recur_file_search(argv[2]);

	//Increment used to find bounds for thread logic. If count is less than 4, only one thread
	// Will be used to visit the children
	int increment;
	if(count < 4){
		increment = count % 4;
	}       
	else{
		increment = count / 4;
	}

	//Created range structs to specify each thread's range that they will seek within the array
	struct range rOne;
	rOne.start = 0; //Begins at 0th index
	rOne.end = increment; // Goes up until the increment

	//Past values used while adding increment to set new ranges for additional structs
	struct range rTwo;
	rTwo.start = rOne.end + increment;
	rTwo.end = rTwo.start + increment;

	struct range rThree;
	rThree.start = rTwo.end + increment;
	rThree.end = rThree.start + increment;

	struct range rFour;
	rFour.start = rThree.end + increment;
	rFour.end = count;

	//Pointers used to allow structs to be passed in to the pthread_create function
	struct range* rOnePtr = &rOne;
	struct range* rTwoPtr = &rTwo;
	struct range* rThreePtr = &rThree;
	struct range* rFourPtr = &rFour;

	//Create new threads (probably better to do this in a loop...) that passes
	//in their corresponding struct
	//pthread_create takes in the address of a thread (which would be within 
	//the threads array) and thread attributes (usually NULL), the function
	//pointer to call, and the parameters of the function (as void* args)
	pthread_create(&threads[0], NULL, thread_file_search, rOnePtr);
	pthread_create(&threads[1], NULL, thread_file_search, rTwoPtr);
	pthread_create(&threads[2], NULL, thread_file_search, rThreePtr);
	pthread_create(&threads[3], NULL, thread_file_search, rFourPtr);

	//Join the threads after all threads finished and wait for all to finish
	for(int i = 0; i < 4; i++){
		if(pthread_join(threads[i], NULL)){
			printf("Error in joining thread\n");
		}
	}

	gettimeofday(&end, NULL);
	printf("Time: %ld\n", (end.tv_sec * 1000000 + end.tv_usec)
			- (start.tv_sec * 1000000 + start.tv_usec));

	//Exit the thread
	pthread_exit(NULL);

	//Deallocate memory after array has been used
	for(int i = 0; i < count; i++){
		free(paths[i]);
	}

	return 0;
}

//Function pointer takes in void * and casts arguemnt appropriately
//(expecting a struct range). Called when new thread is created.
void* thread_file_search(void* arg){
	struct range* r = (struct range*)arg;
	//Given appropriate range, iterate through array of paths and
	//print paths that contain a substring of the search_term
	if(r->start < count){
		for(int i = r->start; i < r->end; i++){
			if(i < count){
				if(strstr(paths[i], search_term)){
					printf("%s\n", paths[i]);
				}
			}
		}
	}
}

//This function takes a path to recurse on, searching for mathes to the
// (global) search_term.  The base case for recursion is when *file is
// not a directory.
//Parameters: the starting path for recursion (char *), which could be a
// directory or a regular file (or something else, but we don't need to
// worry about anything else for this assignment).
//Returns: nothing
//Effects: prints the filename if the base case is reached *and* search_term
// is found in the filename; otherwise, prints the directory name if the directory
// matches search_term.
void recur_file_search(char *file)
{
	//check if directory is actually a file
	DIR *d = opendir(file);

	//NULL means not a directory (or another, unlikely error)
	if(d == NULL)
	{
		//opendir SHOULD error with ENOTDIR, but if it did something else,
		// we have a problem (e.g., forgot to close open files, got
		// EMFILE or ENFILE)
		if(errno != ENOTDIR)
		{	
			perror("Something weird happened!");
			fprintf(stderr, "While looking at: %s\n", file);
			exit(1);
		}

		//nothing weird happened, check if the file contains the search term
		// and if so print the file to the screen (with full path)
		if(strstr(file, search_term) != NULL){
			//Add to global array of paths. First allocate mem, then cpy the str
			paths[count] = malloc(sizeof(char)*255);
			strcpy(paths[count], file);
			count++; //increment global count variable
		}

		//no need to close d (we can't, it is NULL!)
		return;
	}

	//call recur_file_search for each file in d
	//readdir "discovers" all the files in d, one by one and we
	// recurse on those until we run out (readdir will return NULL)
	struct dirent *cur_file;
	while((cur_file = readdir(d)) != NULL)
	{
		//make sure we don't recurse on . or ..
		if(strcmp(cur_file->d_name, "..") != 0 &&\
				strcmp(cur_file->d_name, ".") != 0)
		{
			//we need to pass a full path to the recursive function, 
			// so here we append the discovered filename (cur_file->d_name)
			// to the current path (file -- we know file is a directory at
			// this point)
			char *next_file_str = malloc(sizeof(char) * \
					strlen(cur_file->d_name) + \
					strlen(file) + 2);

			strncpy(next_file_str, file, strlen(file));
			strncpy(next_file_str + strlen(file), \
					"/", 1);
			strncpy(next_file_str + strlen(file) + 1, \
					cur_file->d_name, \
					strlen(cur_file->d_name) + 1);

			//Allocate memory of global paths array then copy directory path
			paths[count] = malloc(sizeof(char)*255);
			strcpy(paths[count], next_file_str);
			count++; //increment global count variable

			//recurse on the file
			recur_file_search(next_file_str);

			//free the dynamically-allocated string
			free(next_file_str);
		}
	}

	//close the directory, or we will have too many files opened (bad times)
	closedir(d);
}

