```c

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
```