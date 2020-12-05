#Makefile para compilar tele_bot
#tele_bot/Makefile

LIB = ../lib/
INCLUDES = -I../lib/

tele_bot: tele_bot.o 
	make -C $(LIB)
	gcc -W -Wall -o tele_bot $(LIB)telebot_Capi.o tele_bot.o
	
tele_bot.o: tele_bot.c
	gcc -W -Wall $(INCLUDES) -c tele_bot.c

borra: 
	make -C $(LIB) borra
	rm tele_bot.o
