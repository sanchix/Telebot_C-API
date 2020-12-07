#
#     Fichero:  testbot/Makefile
#       Group:  Grupo 8
#      Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
#       Fecha:  07/dec/2020
#
# Descripcion:  Makefile para compilar testbot

LIB = ../lib/
LIB_TELEBOT = $(LIB)telebot_Capi/
LIB_HTTPS = $(LIB)https_lib/
LIB_MBEDTLS = $(LIB_HTTPS)mbedtls/library/

INCLUDES = -I$(LIB) -I$(LIB_HTTPS)mbedtls/include/

LIB_TELEBOT_FILES = $(LIB_TELEBOT)telebot_Capi.o $(LIB_TELEBOT)pooling.o
LIB_HTTPS_FILES = $(LIB_HTTPS)https.o $(LIB_MBEDTLS)libmbedx509.a $(LIB_MBEDTLS)libmbedtls.a $(LIB_MBEDTLS)libmbedcrypto.a

todo:
	make -C $(LIB)
	make testbot

testbot: testbot.o $(LIB_TELEBOT_FILES) $(LIB_HTTPS_FILES)
	gcc -W -Wall -o testbot testbot.o $(LIB_TELEBOT_FILES) $(LIB_HTTPS_FILES) -lpthread
	
testbot.o: testbot.c
	gcc -W -Wall $(INCLUDES) -c testbot.c

borra:
	rm -f testbot.o
	rm -f testbot
	echo > Done cleaning testbot
	make -C $(LIB) borra
	
