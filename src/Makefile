all: mail libs

captura:
	gcc -Wall -c -o captura.o captura.c -I../include
	gcc -Wall -o captura captura.o -I../include -L../lib/mail -lrt -lsqlite3 -lmail
	cp captura ../bin
	rm captura captura.o

captura_arm:
	@echo ===========================================
	@echo Se crean el ejecutable y librerias para ARM 
	@echo ============================================
	arm-linux-gnueabi-gcc -Wall -fPIC -c -o libmail.o ../lib/mail/libmail.c -I../include
	arm-linux-gnueabi-gcc -Wall -shared -fPIC -o libmail.so libmail.o -I../inlcude
	scp libmail.so pi@192.168.2.135:/tmp
	arm-linux-gnueabi-gcc -c -o captura_arm.o captura.c -Wall -I../include
	arm-linux-gnueabi-gcc -o captura_arm captura_arm.o -Wall -I../include -L./  -lrt -lmail -L../lib/sqlite3arm/ -lsqlite3
	scp mail_arm pi@192.168.2.135:/tmp	
	rm captura_arm.o captura_arm libmail.o libmail.so

install:
	mkdir pi@172.28.10.202/home/alumne/Escriptori/Fita1
	scp captura_arm pi@172.28.10.202/home/alumne/Escriptori/Fita1

mail:
	#echo export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/josecs/Escritorio/ADSRT_2018/lib/mail/

	gcc -Wall -c -o mail.o mail.c -I../include
	gcc -Wall -o mail mail.o -I../include -L../lib/mail -lmail
	cp mail ../bin
	rm mail.o
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/josecs/Escritorio/ADSRT_2018/lib/mail/ ./mail

mail_arm:
	@echo ===========================================
	@echo Se crean el ejecutable y librerias para ARM 
	@echo ============================================
	arm-linux-gnueabi-gcc -Wall -fPIC -c -o libmail.o ../lib/mail/libmail.c -I../include
	arm-linux-gnueabi-gcc -Wall -shared -fPIC -o libmail.so libmail.o -I../inlcude
	scp libmail.so pi@192.168.2.135:/tmp
	arm-linux-gnueabi-gcc -c -o mail_arm.o mail.c -Wall -I../include
	arm-linux-gnueabi-gcc -o mail_arm mail_arm.o -Wall -I../include -L./ -lpthread -lrt -lmail
	scp mail_arm pi@192.168.2.135:/tmp	
	rm mail_arm.o mail_arm libmail.o libmail.so

libs:
	+$(MAKE) -C ../lib/mail

Raspberry:
		arm-linux-gnueabi-gcc -c -o captura_arm.o captura.c -Wall -I../include
		arm-linux-gnueabi-gcc -o captura_arm captura_arm.o -Wall -I../include -L../lib/mail -lpthread -lrt -lsqlite3 -lmail
		cp captura_arm ../bin
		rm cptura_arm.o
