#ifndef _FETCH_H_
#define _FETCH_H_

typedef struct
{
    char *key;
    char *val;
} Header;

typedef enum
{
    Get,
    Post
} HttpMethod;

struct FetchParms
{
    void (*OnGotData)(char *incomingBuffer, char *output);
    char message[300];
    Header header[3];
    int headerCount;
    HttpMethod method;
    char *body;
    int status;
};

void fetch(char *url, struct FetchParms *fetchParms);

#endif