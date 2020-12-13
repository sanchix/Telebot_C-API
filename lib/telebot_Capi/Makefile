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

INCLUDES = -I$(LIB) -I$(LIB_JANSSON)include/ -I$(LIB_HTTPS)mbedtls/include/

todo: telebot_Capi.o pooling.o

telebot_Capi.o: telebot_Capi.c telebot_Capi.h
	gcc -W -Wall $(INCLUDES) -c telebot_Capi.c
	
pooling.o: pooling.c telebot_Capi.h
	gcc -W -Wall $(INCLUDES) -c pooling.c

borra: 
	rm -f telebot_Capi.o
	rm -f pooling.o
