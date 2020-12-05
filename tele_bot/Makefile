#Makefile para compilar tele_bot
#tele_bot/Makefile

LIB = ../lib/
INCLUDES = -I../lib/

LIB_HTTPS = "../lib/https_lib/"
LIB_TELEBOT = "../lib/telebot_Capi/"

todo:
	make -C $(LIB)
	make tele_bot

tele_bot: tele_bot.o 
	gcc -W -Wall -o tele_bot tele_bot.o $(LIB_TELEBOT)telebot_Capi.o $(LIB_HTTPS)https.o $(LIB_HTTPS)mbedtls/library/libmbedx509.a $(LIB_HTTPS)mbedtls/library/libmbedtls.a $(LIB_HTTPS)mbedtls/library/libmbedcrypto.a
#	gcc -W -Wall -o tele_bot $(LIB)telebot_Capi/telebot_Capi.o tele_bot.o ../lib/https_lib/https.o
	
tele_bot.o: tele_bot.c
	gcc -W -Wall $(INCLUDES) -c tele_bot.c

borra: 
	rm tele_bot.o
	rm tele_bot
	make -C $(LIB) borra
	
