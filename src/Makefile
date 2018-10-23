all:
	arm-linux-gnueabi-gcc main.c -Wall -lpthread -lrt -lsqlite3 -o captura

clean:
	rm captura

install:
	mkdir pi@172.28.10.202/home/alumne/Escriptori/Fita1
	scp captura pi@172.28.10.202/home/alumne/Escriptori/Fita1
