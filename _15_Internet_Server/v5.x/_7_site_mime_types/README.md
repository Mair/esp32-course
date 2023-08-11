```c
 char *ext = strrchr(req->uri,'.');
  if(ext)
  {
    if(strcmp(ext,".css") == 0) httpd_resp_set_type(req,"text/css");
    if(strcmp(ext,".js") == 0) httpd_resp_set_type(req,"text/javascript");
    if(strcmp(ext,".png") == 0) httpd_resp_set_type(req,"image/png");
    if(strcmp(ext,".jpg") == 0) httpd_resp_set_type(req,"image/jpg");
    if(strcmp(ext,".svg") == 0) httpd_resp_set_type(req,"image/svg+xml");
  }
```
