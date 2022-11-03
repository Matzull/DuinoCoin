EXECUTABLE = d-cpuminer
FLAGS = -Wall -lcrypto -lpthread -O3

all: main_c

main_c:
	gcc main.c sha1.c $(FLAGS) -o $(EXECUTABLE) 

clean:
	rm -f *.o d-cpuminer
