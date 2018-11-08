all:
	arm-linux-gnueabi-gcc main.c -Wall -lpthread -lrt -lsqlite3 -o captura

clean:

install:
	mkdir pi@172.28.10.202/home/alumne/Escriptori/Fita1
	scp captura.o pi@172.28.10.202/home/alumne/Escriptori/Fita1

libs:
	+$(MAKE) -C ../lib/mail
mail:
	gcc -Wall -c -o mail.o mail.c -I../include
	gcc -Wall -o mail mail.o -I../include -L../lib/mail -lmail
	cp mail ../bin
	rm mail
	rm mail.o
