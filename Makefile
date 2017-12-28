all:libconf.a

libconf.a:conf.o stack.o hash.o
	mkdir -p include/conf-c lib 
	cp src/conf.h include/conf-c/
	ar rcs libconf.a conf.o stack.o hash.o
	cp libconf.a lib/

conf.o:src/conf.h src/conf.c 
	gcc -c src/conf.c -O2 -Wall

stack.o:src/stack/stack.h src/stack/stack.c 
	gcc -c src/stack/stack.c -O2 -Wall

hash.o:src/hash/hash.h src/hash/hash.c 
	gcc -c src/hash/hash.c -O2 -Wall

clean:
	rm -rfv include lib *.o *.a

install:
	cp -r include/conf-c /usr/include/ 
	cp lib/libconf.a /usr/lib/

uninstall:
	rm -rfv /usr/include/conf-c/ /usr/lib/libconf.a
