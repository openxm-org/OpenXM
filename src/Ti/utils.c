/*
** utils.c                                 Birk Huber, 4/99 
** 
**  A place for general global variables and routines accessible 
**  througout the program.
**
** TiGERS,  Toric Groebner Basis Enumeration by Reverse Search 
** copyright (c) 1999  Birk Huber
**
*/
#include<stdio.h>
#include<stdlib.h>

int eatwhite(FILE *is){
 int c;
 while ((c=fgetc(is))!=EOF){
  if (c=='%')  while ((c=fgetc(is))!=EOF && c!='\n');
  if (isspace(c)==0) break;
 } 
 ungetc(c,is);
}
