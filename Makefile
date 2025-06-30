all:
	gcc -Wall -o bubble-csv src/bubble-csv.c
	gcc -Wall -o bubble-csv-teste src/bubble-csv-teste.c

clean:
	rm -f bubble-csv
	rm -f bubble-csv-teste