# Programarea Calculatoarelor, seria CC
# Tema2 - 2048

build: 2048

2048: 2048.c
	gcc -Wall 2048.c -o 2048 -lcurses

pack:
	zip -FSr 312CC_Dima_Alexandru.zip README Makefile *.c *.h

clean:
	rm -f 2048

.PHONY:
	pack clean

run:
	./2048
