#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Person_struct
{
  char firstName[20];
  char lastName[20];
  int age;
  void (*DoWork)(char *dataRetrieved)
} Person;

void updatePerson(Person *person, char * dataRetrieved)
{
  strcpy(person->firstName, "data from dataRetrieved");
  strcpy(person->lastName, "data from dataRetrieved");
  person->age = 35;
}

void connectAndGetInfo(char * url,void (*DoWork)(char *dataRetrieved))
{
  //connect to wireless
  // connect to endpoint
  // allocate memory
  char *dataRetrieved = (char *)malloc(1024);
  // fill buffer with data: getRequest(&dataRetrieved);
  // do work and get a person object
  DoWork(dataRetrieved);
  // clean up memory and other resources
  free((void *)dataRetrieved);
}

void DoWorkForPerson(char *dataRetrieved)
{
  Person person;
  updatePerson(&person, dataRetrieved);
  printf("person: %s %s is %d years old\n", person.firstName, person.lastName, person.age);
}

void app_main(void)
{
  connectAndGetInfo("http://getperson.com",DoWorkForPerson);
}
