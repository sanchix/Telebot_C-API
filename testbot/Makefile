#
#     Fichero:  testbot/Makefile
#       Group:  Grupo 8
#      Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
#       Fecha:  07/dec/2020
#
# Descripcion:  Makefile para compilar testbot

LIB = ../lib/
LIB_TELEBOT = $(LIB)telebot_Capi/
LIB_JANSSON = $(LIB)jansson/
LIB_CURL = $(LIB)curl/
LIB_SSL = $(LIB)ssl/

INCLUDES = -I$(LIB) -I$(LIB_JANSSON)include/ -I$(LIB_SSL)include/ -I$(LIB_CURL)include/curl/

LIB_TELEBOT_FILES = $(LIB_TELEBOT)telebot_Capi.o $(LIB_TELEBOT)tbc_polling.o $(LIB_TELEBOT)events.o
LIB_JANSSON_FILES = -L$(LIB_JANSSON)lib/ -ljansson
LIB_CURL_FILES = -L$(LIB_SSL)lib -L$(LIB_CURL)lib -lcurl

todo:
	make -C $(LIB)
	make testbot

testbot: testbot.o $(LIB_TELEBOT_FILES) $(LIB_JSON_FILES)
	gcc -W -Wall -o testbot testbot.o $(LIB_TELEBOT_FILES) $(LIB_JANSSON_FILES) $(LIB_CURL_FILES) -lpthread
	
testbot.o: testbot.c
	gcc -W -Wall $(INCLUDES) -c testbot.c

borra:
	rm -f testbot.o
	rm -f testbot
	make -C $(LIB) borra
	
borraclave:
	./borraclave.sh
