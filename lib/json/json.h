/*
**     Fichero:  lib/json/json.h
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  12/dec/2020
**
** Descripcion:  Declaración de funciones y tipos complementarios a la librería jsmn necesarios para trabajar con las cadenas json que se utilizan en la API de telegram.
*/

#include "jsmn.h"

//#define DEBUG 1

typedef struct{
	int id;
	int is_bot;
	char first_name[25];
	char last_name[50];
	char language_code[5];
} from_t;

typedef struct{
	int id;
	char first_name[25];
	char last_name[50];
	char type[20];
} chat_t;

typedef struct{
	int message_id;
	from_t from;
	chat_t chat;
	long date;
	char text[1000];
} message_t;

int json_parse(char *cad, json_parsed_t *parsed);

int json_element_size(jsmntok_t *element);

int json_primeFromObj(char *clave, jsmntok_t *obj, json_parsed_t parsed);

int json_stringFromObj(char *clave, char *string, jsmntok_t *obj, json_parsed_t parsed)

int json_elementFromList(int index, jsmntok_t **element, jsmntok_t *list, json_parsed_t parsed);

int json_elementFromObj(char *clave, jsmntok_t **element, jsmntok_t *obj, json_parsed_t parsed);

int json_smsgFromObj(char *clave, message_t *message, jsmntok_t *obj, json_parsed_t parsed);

int analize_element(jsmntok_t *t, char *json);

int analize_list(jsmntok_t *t, char *json);

int analize_object(jsmntok_t *t,char *json);

int jsoneq(const char *json, jsmntok_t *tok, const char *s);

void analize_all(char *json);