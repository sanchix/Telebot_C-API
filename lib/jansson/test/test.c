#include <stdio.h>
#include "jansson.h"

int main(){

	char json_string[] = "{\"me\":\"Javi\",\"friends\":[\"Paula\",\"Marta\"]}";
	json_t *root;
	json_error_t error;
	
	printf("Test:\n\n");
	
	root = json_loads(json_string, 0, &error);
	
	if(!root){
		printf("Error: on line %d: %s\n", error.line, error.text);
	}
	else if(!json_is_object(root)){
		
		printf("Error: root is not object\n");
		
	}
	else{
		
		json_t *me;
		json_t *friends;
		me = json_object_get(root, "me");
		if(!json_is_string(me)){
			printf("Error: me is not a string\n");
		}
		else{
			printf("Me: %s\n", json_string_value(me));
		}
		
		friends = json_object_get(root, "friends");
		if(!json_is_array(friends)){
			printf("Error: friends is not an array\n");
		}
		else{
			for(size_t i = 0; i < json_array_size(friends); i++){
				json_t *friend;
				friend = json_array_get(friends, i);
				if(!json_is_string(friend)){
					printf("Error: friend is not string\n");
				}
				else{
					printf("Friend %lu: %s\n", i, json_string_value(friend));
				}
			}
		}
		
	}

	return 0;

}
