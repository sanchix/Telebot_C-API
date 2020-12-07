#Makefile para compilar telebot_Capi.o
#lib/telebot_Capi/Makefile

LIB = ../
HTTP_include = https_lib/

todo: telebot_Capi.o pooling.o
	echo "> Telebot C-API compiled"

telebot_Capi.o: telebot_Capi.c telebot_Capi.h
	gcc -W -Wall -I$(LIB) -I$(LIB)https_lib/mbedtls/include -c telebot_Capi.c
	
pooling.o: pooling.o telebot_Capi.h
	gcc -W -Wall -I$(LIB) -I$(LIB)https_lib/mbedtls/include -c pooling.c

borra: 
	rm telebot_Capi.o
