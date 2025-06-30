all:
	gcc -Wall -o csv-sorter src/csv-sorter.c

clean:
	rm -f csv-sorter