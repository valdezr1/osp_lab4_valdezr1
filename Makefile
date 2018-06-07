file_search:
	gcc -o threaded_file_search threaded_file_search.c -pthread

all:
	gcc -o threaded_file_search threaded_file_search.c -pthread

clean:
	rm -f threaded_file_search
