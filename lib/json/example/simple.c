#include "../jsmn.h"
#include "../json.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */


char *JSON_STRING = "{\"ok\":true,\"result\":[{\"update_id\":596179679,\"message\":{\"message_id\":16,\"from\":{\"id\":150848014,\"is_bot\":false,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"language_code\":\"en\"},\"chat\":{\"id\":150848014,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"type\":\"private\"},\"date\":1607340555,\"text\":\"Parada claro\"}},{\"update_id\":596179680,\"message\":{\"message_id\":17,\"from\":{\"id\":150848014,\"is_bot\":false,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"language_code\":\"en\"},\"chat\":{\"id\":150848014,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"type\":\"private\"},\"date\":1607340661,\"text\":\"Ho\"}}]}";


//static const char *JSON_STRING = "{\"nombre\":\"valor\",\"objeto\":{\"nombre2\":\"valor2\", \"objeto2\":{\"a\":\"1\",\"b\":\"2\"},\"nombre3\":\"valor3\"}}";
//static const char *JSON_STRING = "{\"nombre\":\"valor\",\"lista\":[\"valor2\",\"valor3\"],\"lista2\":[\"valor4\", \"objeto1\":{\"a\":\"1\",\"b\":\"2\"}]}";


/*static const char *JSON_STRING = "{\"nombre\":\"valor\",\"lista\":{\"nombre2\":\"valor2\", \"lista2\":{\"a\":\"1\",\"b\":\"2\"},\"nombre3\":\"valor3\"}}";
 */

/*static const char *JSON_STRING = "{\"nombre\":\"valor\",\"lista\":[\"valor2\",\"valor3\"],\"lista2\":[\"valor4\",{\"a\":\"1\",\"b\":\"2\"},[\"a\",\"b\"]]}";
 */ 


int main() {
  int i;
  int r;
  jsmn_parser p;
  //TODO: Elegir bien
  jsmntok_t t[128]; /* We expect no more than 128 tokens */
  //char buffer[100];

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
		
    printf("%.*s:\n", t[i].end - t[i].start, JSON_STRING + t[i].start);

		if(t[i+1].type == JSMN_STRING){

			printf("%.*s\n", t[i + 1].end - t[i + 1].start,JSON_STRING + t[i + 1].start);
			 i = i + 1;

		}else if(t[i+1].type == JSMN_OBJECT){
		
		  int offset = analize_object(&t[i+1],JSON_STRING);
			i = i + offset;
		
    }else if(t[i+1].type == JSMN_ARRAY){
    
		  int offset = analize_list(&t[i+1],JSON_STRING);
      i = i + offset;
    
    }
	}

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
	int offset = analize_object(&t[i+1],JSON_STRING);
	i = i + offset;
      }
      else if(t[i+1].type == JSMN_ARRAY){
    
	int offset = analize_list(&t[i+1],JSON_STRING);
	i = i + offset;    
      }
      else if(t[i+1].type == JSMN_PRIMITIVE){
    
	printf("%.*s\n", t[i + 1].end - t[i + 1].start, (JSON_STRING + t[i + 1].start));
      }
      
    }

	
  }
  return EXIT_SUCCESS;
}
