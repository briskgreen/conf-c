all:libconf.so

libconf.so:src/conf.h src/hash/hash.h src/stack/stack.h src/conf.c src/hash/hash.c src/stack/stack.c
	mkdir -p include/conf-c include/conf-c/hash include/conf-c/stack lib && cp src/conf.h include/conf-c/ && cp src/hash/hash.h include/conf-c/hash && cp src/stack/stack.h include/conf-c/stack && cd lib && gcc -shared -o libconf.so ../src/conf.c ../src/hash/hash.c ../src/stack/stack.c && ln -s libconf.so libconf.so.0.1

clean:
	rm -rfv include lib 

install:
	cp -r include/conf-c /usr/include/ && cp lib/libconf.so lib/libconf.so.0.1 /usr/lib/ 

uninstall:
	rm -rfv /usr/include/conf-c/ /usr/lib/libconf.so /usr/lib/libconf.so.0.1
