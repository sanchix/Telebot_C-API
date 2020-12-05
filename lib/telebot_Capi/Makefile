#Makefile para compilar telebot_Capi.o
#lib/telebot_Capi/Makefile

LIB = ../
HTTP_include = https_lib/

telebot_Capi.o: telebot_Capi.c telebot_Capi.h
	gcc -W -Wall -I$(LIB) -I$(LIB)https_lib/mbedtls/include -c telebot_Capi.c

borra: 
	rm telebot_Capi.o
