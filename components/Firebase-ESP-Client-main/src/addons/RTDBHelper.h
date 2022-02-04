#ifndef RTDB_HElPER_H_
#define RTDB_HElPER_H_

#include "FirebaseFS.h"

#ifdef ENABLE_RTDB

#ifndef RTDBHelper_H_
#define RTDBHelper_H_
#include <Arduino.h>

#if defined(FIREBASE_ESP_CLIENT)
#include <Firebase_ESP_Client.h>
#elif defined(FIREBASE_ESP32_CLIENT) || defined(FIREBASE_ESP8266_CLIENT)
#if defined(ESP32)
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <FirebaseESP8266.h>
#endif
#endif

void printResult(FirebaseData &data);
void printResult(FIREBASE_STREAM_CLASS &data);



#endif //RTDBHelper_H_

#endif //ENABLE

#endif //RTDB_HElPER_H_