#include <stdio.h>
#include <string.h>
#include "parse_weather.h"
#include "esp_log.h"
#include "cJSON.h"
#include "esp_heap_caps.h"

static const char * TAG="PARSE WEATHER";

void * json_malloc(size_t size){
  return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
}

void json_free(void* ptr){
  heap_caps_free(ptr);
}

esp_err_t parse_weather(char *weatherStr){

  cJSON_Hooks hooks ={
      .malloc_fn =json_malloc,
      .free_fn = json_free
  };
  cJSON_InitHooks(&hooks);


  cJSON *weather_json =  cJSON_Parse(weatherStr);
  if(weather_json == NULL){
    const char * err = cJSON_GetErrorPtr();
    if(err){
      ESP_LOGE(TAG, "Error parsing json before %s",err);
      return -1;
    }
  }
  cJSON *location = cJSON_GetObjectItemCaseSensitive(weather_json,"location");
  cJSON *name = cJSON_GetObjectItemCaseSensitive(location,"name");
  printf("name %s\n",name->valuestring);

  return ESP_OK;

}