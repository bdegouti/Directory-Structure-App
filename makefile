PROGRAM_NAME = myls

all: myls.c
	gcc -g -Wall -o $(PROGRAM_NAME) myls.c -lm
clean:
	rm $(PROGRAM_NAME)