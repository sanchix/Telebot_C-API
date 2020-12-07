#
#     Fichero:  tele_bot/Makefile
#       Group:  Grupo 8
#      Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
#       Fecha:  07/dec/2020
#
# Descripcion:  Makefile para compilar tele_bot

LIB = ../lib/
LIB_TELEBOT = $(LIB)telebot_Capi/
LIB_HTTPS = $(LIB)https_lib/
LIB_MBEDTLS = $(LIB_HTTPS)mbedtls/library/

INCLUDES = -I$(LIB) -I$(LIB_HTTPS)mbedtls/include/

LIB_TELEBOT_FILES = $(LIB_TELEBOT)telebot_Capi.o $(LIB_TELEBOT)pooling.o
LIB_HTTPS_FILES = $(LIB_HTTPS)https.o $(LIB_MBEDTLS)libmbedx509.a $(LIB_MBEDTLS)libmbedtls.a $(LIB_MBEDTLS)libmbedcrypto.a

todo:
	make -C $(LIB)
	make tele_bot

tele_bot: tele_bot.o 
	gcc -W -Wall -o tele_bot tele_bot.o $(LIB_TELEBOT_FILES) $(LIB_HTTPS_FILES) -lpthread
	
tele_bot.o: tele_bot.c
	gcc -W -Wall $(INCLUDES) -c tele_bot.c

borra:
	rm -f tele_bot.o
	rm -f tele_bot
	echo > Done cleaning tele_bot
	make -C $(LIB) borra
	
