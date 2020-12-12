#include "../jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */


static const char *JSON_STRING = "{\"ok\":true,\"result\":[{\"update_id\":596179679,\"message\":{\"message_id\":16,\"from\":{\"id\":150848014,\"is_bot\":false,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"language_code\":\"en\"},\"chat\":{\"id\":150848014,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"type\":\"private\"},\"date\":1607340555,\"text\":\"Parada claro\"}},{\"update_id\":596179680,\"message\":{\"message_id\":17,\"from\":{\"id\":150848014,\"is_bot\":false,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"language_code\":\"en\"},\"chat\":{\"id\":150848014,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"type\":\"private\"},\"date\":1607340661,\"text\":\"Ho\"}}]}";


/*static const char *JSON_STRING = "{\"nombre\":\"valor\",\"lista\":{\"nombre2\":\"valor2\", \"lista2\":{\"a\":\"1\",\"b\":\"2\"},\"nombre3\":\"valor3\"}}";
 */

/*static const char *JSON_STRING = "{\"nombre\":\"valor\",\"lista\":[\"valor2\",\"valor3\"],\"lista2\":[\"valor4\",{\"a\":\"1\",\"b\":\"2\"},[\"a\",\"b\"]]}";
 */ 
int analize_list(jsmntok_t *t);

int analize_object(jsmntok_t *t){
	
  int tokensize = 0;
  int offset = 1;
	
  printf("{\n");
	
  for(int i = 0 ; i < t[0].size ; i++){
    printf("%.*s:\n", t[offset].end - t[offset].start,JSON_STRING + t[offset].start);
    
    if(t[offset+1].type == JSMN_STRING){
      printf("%.*s\n", t[offset + 1].end - t[offset + 1].start,JSON_STRING + t[offset + 1].start);
      offset = offset + 2; 
    }
    else if(t[offset+1].type == JSMN_OBJECT){
      offset = offset + 1 + analize_object(&t[offset+1]);
    }
    else if(t[offset+1].type == JSMN_ARRAY){
      offset = offset + 1 + analize_list(&t[offset+1]);
    }
    else if(t[offset+1].type == JSMN_PRIMITIVE){
       printf("%.*s\n", t[offset+1].end - t[offset+1].start,JSON_STRING + t[offset+1].start);
      offset = offset + 2;
    }
  }
	
  printf("}\n");
	
  tokensize = offset;
  return tokensize;
	
}
int analize_list(jsmntok_t *t){
  int tokensize = 0;
  int offset = 1;

  printf("[\n");
  

  
  for(int i = 0 ; i < t[0].size ; i++){
   
    if(t[offset].type == JSMN_STRING){
     
      printf("%.*s\n", t[offset].end - t[offset].start,JSON_STRING + t[offset].start);
      offset = offset + 1;
      
    }
    else if(t[offset].type == JSMN_OBJECT){
      offset = offset + analize_object(&t[offset]);
    }
    else if(t[offset].type == JSMN_ARRAY){
      offset = offset + analize_list(&t[offset]);
    }
    else if(t[offset].type == JSMN_PRIMITIVE){
       printf("%.*s\n", t[offset].end - t[offset].start,JSON_STRING + t[offset].start);
      offset = offset + 1;
    }
  }
  
  printf("]\n");


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

int main() {
  int i;
  int r;
  jsmn_parser p;
  jsmntok_t t[128]; /* We expect no more than 128 tokens */
  char buffer[100];

  jsmn_init(&p);
  r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t,
		 sizeof(t) / sizeof(t[0]));
  if (r < 0) {
    printf("Failed to parse JSON: %d\n", r);
    return 1;
  }

  /* Assume the top-level element is an object */
  if (r < 1 || t[0].type != JSMN_OBJECT) {
    printf("Object expected\n");
    return 1;
  }

  /* Loop over all keys of the root object */
  for (i = 1; i < r; i++) {
	  
    //printf("%i\n", t[i].type);
    if(t[i].type == JSMN_STRING){
      printf("%.*s:\n", t[i].end - t[i].start,
             JSON_STRING + t[i].start);
      if(t[i+1].type == JSMN_STRING){
	printf("%.*s\n", t[i + 1].end - t[i + 1].start,
	       JSON_STRING + t[i + 1].start);
	i = i + 1;
      }
      else if(t[i+1].type == JSMN_OBJECT){
	int offset = analize_object(&t[i+1]);
	i = i + offset;
      }
      else if(t[i+1].type == JSMN_ARRAY){
    
	int offset = analize_list(&t[i+1]);
	i = i + offset;    
      }
      else if(t[i+1].type == JSMN_PRIMITIVE){
    
	printf("%.*s\n", t[i + 1].end - t[i + 1].start, (JSON_STRING + t[i + 1].start));
      }
      
    }
	
  }
  return EXIT_SUCCESS;
}
