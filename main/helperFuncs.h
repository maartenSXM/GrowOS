#ifndef HELPERFUNCS_H_
#define HELPERFUNCS_H_

#include "globals.h"
#include "pages.h"

void int_to_char_cat(char *dest,int i);
void int_to_char(char *dest,int i);
void string_to_char_cat(char *dest,String str);
void string_to_char(char *dest,String str);
void bit8_hex_array_to_string(char *buffer, unsigned int len, uint8_t *array);

#endif
