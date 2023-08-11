```c
 httpd_config_t config = HTTPD_DEFAULT_CONFIG();
 config.uri_match_fn = httpd_uri_match_wildcard;
```

```c

static esp_err_t on_default_url(httpd_req_t *req)
{
  ESP_LOGI(TAG, "URL: %s", req->uri);
  char path[600];
  sprintf(path,"/store%s",req->uri);

  FILE *file = fopen(path,"r");
  if(file == NULL)
  {
    file = fopen("/store/index.html","r");
    if(file == NULL) {
      httpd_resp_send_404(req);
    }

  }
  char buffer[1024];
  int bytes_read = 0;
  while ((bytes_read = fread(buffer,sizeof(char),sizeof(buffer),file)) > 0)
  {
    httpd_resp_send_chunk(req,buffer,bytes_read);
  }
  fclose(file);

  httpd_resp_send_chunk(req,NULL,0);

  return ESP_OK;
}


```
