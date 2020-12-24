#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "curl.h"


struct memory {
   char *response;
   size_t size;
 };
 
 static size_t cb(void *data, size_t size, size_t nmemb, void *userp)
 {
   size_t realsize = size * nmemb;
   struct memory *mem = (struct memory *)userp;
 
   char *ptr = realloc(mem->response, mem->size + realsize + 1);
   if(ptr == NULL)
     return 0;  /* out of memory! */
 
   mem->response = ptr;
   memcpy(&(mem->response[mem->size]), data, realsize);
   mem->size += realsize;
   mem->response[mem->size] = 0;
 
   return realsize;
 }


int main(){

	CURL *easyhandle;
	CURLcode res;
	curl_version_info_data *data;
	struct memory response;
	
	printf("Hello\n");
	
	// SOLO AL PRINCIPIO DEL PROGRAMA
	curl_global_init(CURL_GLOBAL_ALL);

	printf("Initialized\n");
	
	// Es recomendable checkearlo
	data = curl_version_info(CURLVERSION_NOW);
	if(data->ssl_version != NULL){
		printf("Habemus SSL: %s\n", data->ssl_version);
	}
	
	// Crear uno al arrancar cada hilo (un handle para cada hilo). NO SE DEBEN COMPARTIR ENTRE VARIOS HILOS
	// Para duplucar un handle se puede usar curl_easy_duphandle
	easyhandle = curl_easy_init();
	if(easyhandle){
		
		// Estas opciones son recomendables
		curl_easy_setopt(easyhandle, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(easyhandle, CURLOPT_SSL_VERIFYHOST, 0L);
		
		// Opcion importante para multithread
		curl_easy_setopt(easyhandle, CURLOPT_NOSIGNAL, 1L);
		
		// La URL se almacena en la librería como una opción.
		curl_easy_setopt(easyhandle, CURLOPT_URL, "https://api.telegram.org/bot1384948746:AAHpqgXKIH03JRVvQ-G0yKgxmkN7df2yQEc/getUpdates?offset=-1");
		
		// El manejador se ajusta también con una opción
		curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, cb);
		
		// Pasamos información al manejador
		curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, (void *)&response);
		
		// Realizamos la transacción. Devuelve error si el handle no ha podido procesar todos los datos recibidos.
		// Se puede usar CURLOPT_ERRORBUFFER para ver los errores.
		response.response = NULL;
		response.size = 0;
		printf("pre\n");
		res = curl_easy_perform(easyhandle);
		printf("post\n");
		if(res != CURLE_OK)
			printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		
		printf("Response: %s\n", response.response);
		
		curl_easy_cleanup(easyhandle);
		
	}
	
	// SOLO AL FINAL DEL PROGRAMA
	curl_global_cleanup();

	return 0;

}
