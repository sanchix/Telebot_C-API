#
#     Fichero:  lib/Makefile
#       Group:  Grupo 8
#      Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
#       Fecha:  07/dec/2020
#
# Descripcion:  Makefile para compilar todas las librerías:

#Libs:
#	https_lib
#	telebot_Capi

LIB_TELEBOT = telebot_Capi/
LIB_HTTPS = https_lib/
LIB_JSON = json/

todo:	
	make -C $(LIB_HTTPS)
	make -C $(LIB_TELEBOT)
	make -C $(LIB_JSON)

borra:
	make -C $(LIB_TELEBOT) borra
	make -C $(LIB_JSON) clean
