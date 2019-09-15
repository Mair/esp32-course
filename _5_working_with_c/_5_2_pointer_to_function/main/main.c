#include <stdio.h>
#include "getData.h"

void DealWithData(char * data)
{
  printf("got data\n");
  //do some manipulation on specific data for the url
  printf("%s\n", data);
}


void app_main(void)
{
  struct getDataParams params;
  sprintf(params.url, "http://go-somewhere");
  params.deal_with_data_fp = DealWithData;
  getData(&params);
}