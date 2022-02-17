#include "firebase_Funcs.h"

void initialize_firebase_variables(){
    char blankName[10] = "Un named";
    setFirebaseFilePhotoPath(FIREBASE_FILE_PHOTO_PATH , blankName, 0);
}

void setFirebaseFilePhotoPath(char *buffer, char *src, unsigned int len){
    if(len ==0) bit8_hex_array_to_string(buffer, 6, ESP32_BASE_MAC_ADDRESS);
    else strcpy(buffer,src);
}