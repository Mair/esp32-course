#include <stdio.h>
#include <string.h>

typedef struct Person_struct {
  char firstName[20];
  char lastName[20];
  int age; 
} Person;

void updatePerson(Person  *person) {
  strcpy(person->firstName, "Bob");
  strcpy(person->lastName, "Fisher");
  person->age = 35;
}
void exclamIt(char * phrase){
  strcat(phrase, "!");
}
void app_main(void)
{
  Person person;
  char phrase[20] ={"Hello world"};
  exclamIt(phrase);
  printf("function output: %s\n", phrase);
  updatePerson(&person);
  printf("person: %s %s is %d years old\n", person.firstName, person.lastName, person.age);
}
