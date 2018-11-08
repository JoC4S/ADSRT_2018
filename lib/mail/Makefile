all:
	gcc -Wall -fPIC -c -o libmail.o libmail.c -I../../include
	gcc -Wall -shared -fPIC -o libmail.so libmail.o -I../../inlcude
	rm libmail.o
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/josecs/Escritorio/ADSRT_2018/lib/mail/
	export LD_LIBRARY_PATH
