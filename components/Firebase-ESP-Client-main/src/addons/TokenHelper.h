#ifndef TOKEN_HElPER_H_
#define TOKEN_HElPER_H_

#include <Arduino.h>
#if defined(ESP32)
#if defined(FIREBASE_ESP32_CLIENT)
#include <FirebaseESP32.h>
#endif
#elif defined(ESP8266)
#if defined(FIREBASE_ESP8266_CLIENT)
#include <FirebaseESP8266.h>
#endif
#endif

#if defined(FIREBASE_ESP_CLIENT)
#include <Firebase_ESP_Client.h>
#endif

String getTokenType(struct token_info_t info);
String getTokenStatus(struct token_info_t info);
String getTokenError(struct token_info_t info);
void tokenStatusCallback(TokenInfo info);

//This header file includes the functions that provide the token generation process info.

#endif  //TOKEN_HElPER_H_