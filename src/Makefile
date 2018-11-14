all: mail libs

captura:
	gcc -Wall -c -o captura.o captura.c
	gcc -Wall -o captura captura.o -lrt -lsqlite3
	cp captura ../bin
	rm captura
	rm captura.o

install:
	mkdir pi@172.28.10.202/home/alumne/Escriptori/Fita1
	scp captura.o pi@172.28.10.202/home/alumne/Escriptori/Fita1

mail:
		#echo export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/josecs/Escritorio/ADSRT_2018/lib/mail/

		gcc -Wall -c -o mail.o mail.c -I../include
		gcc -Wall -o mail mail.o -I../include -L../lib/mail -lmail
		cp mail ../bin
		rm mail.o
		LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/josecs/Escritorio/ADSRT_2018/lib/mail/ ./mail

libs:
	+$(MAKE) -C ../lib/mail

Raspberry:
		#arm-linux-gnueabi-gcc captura.c -Wall -lpthread -lrt -lsqlite3 -o captura
