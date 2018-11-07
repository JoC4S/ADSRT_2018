all:
	arm-linux-gnueabi-gcc main.c -Wall -lpthread -lrt -lsqlite3 -o captura

clean:
	rm captura

install:
	mkdir pi@172.28.10.202/home/alumne/Escriptori/Fita1
	scp captura.o pi@172.28.10.202/home/alumne/Escriptori/Fita1

libs:
	gcc -Wall -fPIC -c -o mail_lib.o mail_lib.c -I/home/josecs/Escritorio/ADSRT_2018/include
	gcc -Wall -shared -fPIC -o mail_lib.so mail_lib.o -I/home/josecs/Escritorio/ADSRT_2018/include
	cp -v mail_lib.so ../lib
	rm mail_lib.o
	rm mail_lib.so

mail:
	gcc -Wall -c -o mail.o mail.c -I/home/josecs/Escritorio/ADSRT_2018/include
	gcc -Wall -o mail mail.o -L/home/josecs/Escritorio/ADSRT_2018/lib -lmail_lib.so
