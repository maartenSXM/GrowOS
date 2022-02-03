#ifndef WIFI_H_
#define WIFI_H_

#include "common.h"

void print_auth_mode(int authmode);
void print_cipher_type(int pairwise_cipher, int group_cipher);
void wifi_scan();

#endif