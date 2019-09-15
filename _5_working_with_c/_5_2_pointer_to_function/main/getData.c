#include <stdio.h>
#include "getData.h"

/****************************
* seperate file where code should not change
******************************/ 



void connectToUrl(char * url) 
{
  printf("connected to %s\n", url);
}

void getData(struct getDataParams *params)
{
  //connect to internet

  //create client and connect to url
  connectToUrl(params->url);

  //get some data
  char * data = "this is the data received";

  params->deal_with_data_fp(data);

  // expose data
}
