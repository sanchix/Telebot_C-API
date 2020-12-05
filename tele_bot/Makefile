#Makefile para compilar tele_bot

SRC = ../src/

tele_bot: tele_bot.o 
	make $(SRC)Makefile 
	gcc -W -Wall -o tele_bot $(SRC)telebot_Capi.o tele_bot.o
	
tele_bot.o: tele_bot.c
	gcc -W -Wall -c tele_bot.c

borra: 
	make $(SRC)Makefile borra
	rm tele_bot.o
