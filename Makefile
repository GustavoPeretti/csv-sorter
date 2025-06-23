all:
	gcc -Wall -o bubble-csv src/bubble-csv.c

clean:
	rm -f bubble-csv