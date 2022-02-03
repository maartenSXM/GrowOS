#include <stdio.h>
#include "string.h"
#include "helperFuncs.h"

void string_to_char(char *dest, String str){
  str.toCharArray(strTemp, str.length());
  strcpy(dest, strTemp);
}

void string_to_char_cat(char *dest, String str){
  str.toCharArray(strTemp, str.length());
  strcat(dest, strTemp);
}

void int_to_char(char *dest, int i){
  int nDigits = floor(log10(abs(i))) + 1;
  char t[nDigits];
  sprintf(t, "%d", i);
  strcpy(dest, t);
}

void int_to_char_cat(char *dest, int i){
  int nDigits = floor(log10(abs(i))) + 1;
  char t[nDigits];
  sprintf(t, "%d", i);
  strcat(dest, t);
}
