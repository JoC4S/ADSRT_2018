all:
	gcc -Wall -fPIC -c -o libmail.o libmail.c -I../../include
	gcc -Wall -shared -fPIC -o libmail.so libmail.o -I../../inlcude
	rm libmail.o

libmailarm:

	arm-linux-gnueabi-gcc -Wall -fPIC -c -o libmail.o libmail.c -I../../include
	arm-linux-gnueabi-gcc -Wall -shared -fPIC -o libmail.so libmail.o -I../inlcude
	rm libmail.o
