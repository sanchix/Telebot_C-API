#
#     Fichero:  lib/telebot_Capi/Makefile
#       Group:  Grupo 8
#      Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
#       Fecha:  07/dec/2020
#
# Descripcion:  Makefile para compilar la librería telebot_Capi

LIB = ../
LIB_HTTPS = $(LIB)https_lib/
LIB_JANSSON = $(LIB)jansson/
LIB_CURL = $(LIB)curl/
LIB_SSL = $(LIB)ssl/

INCLUDES = -I$(LIB) -I$(LIB_JANSSON)include/ -I$(LIB_HTTPS)mbedtls/include/ -I$(LIB_SSL)include/ -I$(LIB_CURL)include/curl/

todo: telebot_Capi.o tbc_polling.o events.o

telebot_Capi.o: telebot_Capi.c telebot_Capi.h
	gcc -W -Wall $(INCLUDES) -c telebot_Capi.c
	
tbc_polling.o: tbc_polling.c telebot_Capi.h
	gcc -W -Wall $(INCLUDES) -c tbc_polling.c
	
events.o: events.c telebot_Capi.h
	gcc -W -Wall $(INCLUDES) -c events.c

borra: 
	rm -f telebot_Capi.o
	rm -f tbc_polling.o
	rm -f events.o
