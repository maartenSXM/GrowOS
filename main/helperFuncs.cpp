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

void bit8_hex_array_to_string(char *buffer, unsigned int len, uint8_t *array)
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}
