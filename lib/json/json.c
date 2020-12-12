#include "./jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int analize_list(jsmntok_t *t, char *json);
int analize_object(jsmntok_t *t,char *json);
int jsoneq(const char *json, jsmntok_t *tok, const char *s);
void analize_all(char *json);

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
