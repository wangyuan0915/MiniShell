build:
	gcc shell.c parser.c -o shell -g -Wall

clean:
	rm -f shell
