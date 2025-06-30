all:
	gcc -Wall -o csv-sorter src/bubble-csv.c

clean:
	rm -f csv-sorter