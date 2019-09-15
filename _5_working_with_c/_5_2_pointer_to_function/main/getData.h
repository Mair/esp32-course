#ifndef _GETDATA_H_
#define _GETDATA_H_

struct getDataParams
{
    char url[256];
    void (*deal_with_data_fp)(char * data);
};

void getData(struct getDataParams *params);

#endif