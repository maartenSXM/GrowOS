#ifndef WIFI_FUNCS_H_
#define WIFI_FUNCS_H_

#include "common.h"

extern const char* loginIndex;
extern const char* serverIndex;

void initialProvision();
void print_auth_mode(int authmode);
void print_cipher_type(int pairwise_cipher, int group_cipher);
void wifi_scan();

#endif