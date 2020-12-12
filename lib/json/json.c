#include "jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int analize_list(jsmntok_t *t, char *json);
int analize_object(jsmntok_t *t,char *json);
int jsoneq(const char *json, jsmntok_t *tok, const char *s);
void analize_all(char *json);


/*
**   Parámetros:  char *cad: cadena de caracteres a parsear
**				  json_parsed_t *parsed: Puntero a un tipo json_parsed_t, que almacenará información del resultado de parsear la cadena "cad";
**                
**     Devuelve:  int: 0 si termina satisfactoriamente, 1 en caso de error
**
**  Descripción:  Realiza el parseo de una cadena json.
*/
int json_parse(char *cad, json_parsed_t *parsed){
	
	int error = 0;
	parsed->json_string = cad;
	jsmn_parser p;
	jsmn_init(&p);
	parsed->r = jsmn_parse(&p, parsed->json_string, strlen(parsed->json_string), parsed->tokens, sizeof(parsed->tokens) / sizeof((parsed->tokens)[0]));
	
	if(parsed->r < 0){
		printf("json.c: Failed to parse JSON: %d\n", parsed->r);
		error = 1;
	}
	else if(parsed->r < 1 || (parsed->tokens)[0].type != JSMN_OBJECT){
		printf("json.c: Object expected\n");
		error = 1;
	}
	
	return error;
	
}


/*
**   Parámetros:  char *clave: clave del elemento.
**				  obj_token *bot_info: Puntero a un tipo bot_info_t, que almacenará información para la comunicación https con la api de Telegram para el BOT específico.
**                
**     Devuelve:  int: valor del tipo primitivo
**
**  Descripción:  Inicializa las funciones de la librería.
*/
int json_primeFromObj(char *clave, json_parsed_t obj){
  
  //tokens[0] = referido a "{"
  int valor; 
  char aux[20];
  int size_valor=-1;
  //Si size_valor se queda a -1 es que no existe la clave o ha habido un error.

  for (int i = 0; i<obj.tokens[0].size && size_valor==-1; i++)
    if (jsoneq(obj.json_string,&obj.tokens[i],clave) == 0){
      
      size_valor = obj.tokens[i+1].end - obj.tokens[i+1].start;
      sprintf(aux,"%.*s",size_valor,obj.json_string+obj.tokens[i+1].start);
      if (strcmp(aux,"true")==0 || strcmp(aux,"false")==0){
	if (strcmp(aux,"true")==0)
	  valor = 1;
	else
	  valor = 0;
      }
      else{
	valor = atoi (aux);
      }
    }

  if (size_valor == -1)
    return size_valor;
  else
    return valor;
}

/*
**   Parámetro:  json_parsed_t obj: para recibir el token.
**				  
**                
**     Devuelve:  int: tamaño de la lista
**
**  Descripción:  Devuelve el tamaño de una lista json.
*/

int json_list_size (json_parsed_t obj){
  int size;
  size = obj.tokens[0].size;
  return size;
}

/*
**   Parámetros:  char *clave: clave del elemento.
**				  json_parsed_t *obj: Puntero al token referido al objeto
**                
**     Devuelve:  Una variable json_parsed_t que apunta al primer token de la lista
**
**  Descripción:  Devuelve una estructura json_parsed_t apuntando a la lista contenida en el objeto "obj" con clave "clave".
*/
int json_listFromObj(char *clave, json_parsed_t obj){
	
	int offset = 0;
	int ret = -1;
	
	for(int i = 0; i < obj.tokens[0].size && ret == -1; i++){
		
		
		
	}
	
}



int analize_list(jsmntok_t *t,char *json){
	int tokensize = 0;
	int offset = 1;

	printf("[\n");
  

  
	for(int i = 0 ; i < t[0].size ; i++){
   
		if(t[offset].type == JSMN_STRING){
     
			printf("%.*s\n", t[offset].end - t[offset].start,json + t[offset].start);
			offset = offset + 1;
      
		}
		else if(t[offset].type == JSMN_OBJECT){
			offset = offset + analize_object(&t[offset],json);
		}
		else if(t[offset].type == JSMN_ARRAY){
			offset = offset + analize_list(&t[offset],json);
		}
		else if(t[offset].type == JSMN_PRIMITIVE){
			printf("%.*s\n", t[offset].end - t[offset].start,json + t[offset].start);
			offset = offset + 1;
		}
	}
  
	printf("]\n");


	tokensize = offset;
	return tokensize;
}

int analize_object(jsmntok_t *t, char * json){
	
  int tokensize = 0;
  int offset = 1;
	
  printf("{\n");
	
  for(int i = 0 ; i < t[0].size ; i++){
    printf("%.*s:\n", t[offset].end - t[offset].start,json + t[offset].start);
    
    if(t[offset+1].type == JSMN_STRING){
      printf("%.*s\n", t[offset + 1].end - t[offset + 1].start,json + t[offset + 1].start);
      offset = offset + 2; 
    }
    else if(t[offset+1].type == JSMN_OBJECT){
      offset = offset + 1 + analize_object(&t[offset+1],json);
    }
    else if(t[offset+1].type == JSMN_ARRAY){
      offset = offset + 1 + analize_list(&t[offset+1],json);
    }
    else if(t[offset+1].type == JSMN_PRIMITIVE){
       printf("%.*s\n", t[offset+1].end - t[offset+1].start,json + t[offset+1].start);
      offset = offset + 2;
    }
  }
	
  printf("}\n");
	
  tokensize = offset;
  return tokensize;
	
}


int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}
void analize_all(char *json){
  int i;
  int r;
  jsmn_parser p;
  jsmntok_t t[128]; /* We expect no more than 128 tokens */

  jsmn_init(&p);
  r = jsmn_parse(&p, json, strlen(json), t, sizeof(t) / sizeof(t[0]));
  if (r < 0) {
    printf("Failed to parse JSON: %d\n", r);
    
  }

  /* Assume the top-level element is an object */
  else if (r < 1 || t[0].type != JSMN_OBJECT) {
    printf("Object expected\n");
    
  }

  /* Loop over all keys of the root object */
  else
    for (i = 1; i < r; i++) {
	  
      //printf("%i\n", t[i].type);
      if(t[i].type == JSMN_STRING){
	printf("%.*s:\n", t[i].end - t[i].start, json + t[i].start);
	if(t[i+1].type == JSMN_STRING){
	  printf("%.*s\n", t[i + 1].end - t[i + 1].start,json + t[i + 1].start);
	  i = i + 1;
	}
	else if(t[i+1].type == JSMN_OBJECT){
	  int offset = analize_object(&t[i+1],json);
	  i = i + offset;
	}
	else if(t[i+1].type == JSMN_ARRAY){
    
	  int offset = analize_list(&t[i+1],json);
	  i = i + offset;    
	}
	else if(t[i+1].type == JSMN_PRIMITIVE){
    
	  printf("%.*s\n", t[i + 1].end - t[i + 1].start, (json + t[i + 1].start));
	}
      
      }
	
    }
  
}
