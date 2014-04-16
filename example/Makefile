all:read create

read:read.c
	gcc -o read read.c -I ../include/ -L ../lib/ -lconf

create:create.c
	gcc -o create create.c -I ../include/ -L ../lib/ -lconf

clean:
	rm read create
