file_search:
	gcc -o file_search file_search.c

all:
	gcc -o file_search file_search.c

clean:
	rm -f file_search
