all:

captura:
	@echo =======================================================
	@echo Se compila libreria libmail.so para x86
	@echo =======================================================

	+$(MAKE) libmail -C ../lib/

	@echo =======================================================
	@echo Se compila programa captura para x86
	@echo =======================================================
	gcc  -c -o captura.o captura.c -I../include
	gcc  -o captura captura.o -lsqlite3 -lrt -I../include -L../lib/ -lmail
	cp captura ../bin
	rm captura.o captura

informe:
	gcc -c -o informe.o informe.c -I../include
	gcc -o informe informe.o -lsqlite3 -lrt -I../include -L../lib/   -lmail
	cp informe ../bin
	rm informe.o informe

informe_arm:
	@echo =======================================================
	@echo Se compila informe para ARM
	@echo =======================================================
	arm-linux-gnueabi-gcc -c -o informe.o informe.c -I../include
	arm-linux-gnueabi-gcc -o informe informe.o -lrt -I../include -L../lib/arm -lmail -lsqlite3
	cp informe ../bin/arm
	scp ../bin/arm/informe pi@192.168.2.199:/tmp
	rm informe.o informe

captura_arm:
	@echo =======================================================
	@echo Se crean el ejecutable y librerias para ARM
	@echo =======================================================
	+$(MAKE) libmailarm -B ../lib/
	arm-linux-gnueabi-gcc -c -o captura_arm.o captura.c -I../include
	arm-linux-gnueabi-gcc -o captura_arm captura_arm.o -I../include -L./  -lrt -lmail -L../lib/arm/ -lsqlite3
	#scp mail_arm pi@172.28.10.202:/tmp
	@echo =======================================================
	@echo Se copian ejecutable y librerias en Raspberry
	@echo =======================================================
	scp ../lib/arm/libmail.so captura_arm ../lib/arm/libsqlite3.so ../script/scaptura_arm.sh pi@192.168.2.199:/tmp
	rm captura_arm.o captura_arm

install:
	@echo =======================================================
	@echo Se crea libreria libmail.so para ARM sin crossCompiling
	@echo =======================================================

	scp ../lib/libmail.c ../include/libmail.h  pi@192.168.2.199:/tmp
	ssh pi@192.168.2.199 gcc -fPIC -c -o /tmp/libmail.o /tmp/libmail.c
	ssh pi@192.168.2.199 gcc -shared -fPIC -o /tmp/libmail.so /tmp/libmail.o
	ssh pi@192.168.2.199 rm /tmp/libmail.c /tmp/libmail.o /tmp/libmail.h

	@echo ===========================================
	@echo Se compila programa para ARM
	@echo ============================================

	arm-linux-gnueabi-gcc -c -o captura_arm.o captura.c -I../include
	arm-linux-gnueabi-gcc -o captura_arm captura_arm.o -I../include -L./  -lrt -lmail -L../lib/arm/ -lsqlite3

	+$(MAKE) -B informe_arm

	@echo =================================================================
	@echo Se copian ejecutable 'captura', librerias y scropts en Raspberry
	@echo =================================================================

	scp captura_arm ../bin/captura.db ../lib/arm/libsqlite3.so ../script/scaptura_arm.sh ../script/sinforme.sh pi@192.168.2.199:/tmp
	rm captura_arm.o captura_arm

libmail:
	+$(MAKE) -C ../lib/

libmailarm:
	+$(MAKE) libmailarm -C ../lib/
