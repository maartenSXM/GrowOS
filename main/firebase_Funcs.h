#ifndef FIREBASE_FUNCS_H_
#define FIREBASE_FUNCS_H_

#include "common.h"
#include "helperFuncs.h"

void initialize_firebase_variables();
void setFirebaseFilePhotoPath(char *buffer, char *src, unsigned int len = 0);

#endif