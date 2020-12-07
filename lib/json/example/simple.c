#include "../jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

static const char *JSON_STRING = "{\"ok\":true,\"result\":[{\"update_id\":596179679,\"message\":{\"message_id\":16,\"from\":{\"id\":150848014,\"is_bot\":false,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"language_code\":\"en\"},\"chat\":{\"id\":150848014,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"type\":\"private\"},\"date\":1607340555,\"text\":\"Parada claro\"}},{\"update_id\":596179680,\"message\":{\"message_id\":17,\"from\":{\"id\":150848014,\"is_bot\":false,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"language_code\":\"en\"},\"chat\":{\"id\":150848014,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"type\":\"private\"},\"date\":1607340661,\"text\":\"Ho\"}}]}";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
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
	  
	printf("%i\n", t[i].type);
	if(jsoneq(JSON_STRING, &t[i], "ok") == 0){
		printf(">ok: %.*s\n", t[i + 1].end - t[i + 1].start,
             JSON_STRING + t[i + 1].start);
	}
	if(jsoneq(JSON_STRING, &t[i], "result") == 0){
		printf(">result:\n");
		int offset = 0;
		for(int j = 0; j < t[i+1].size; i++){
			
			if(jsoneq(JSON_STRING, &t[i+3+offset], "update_id") == 0){
				
			}
			
		}
	}
	
  }
  return EXIT_SUCCESS;
}
