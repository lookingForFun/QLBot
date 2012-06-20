#pragma once

byte * PadData(const byte * in, size_t length, size_t &totalLength);
byte * UnpadData(const byte * in, size_t length, size_t &finalLength);
int base64_encode(const void *data, int size, char **str);
int base64_decode(const char *str, void *data);

const char * Encrypt(const byte * data, size_t len, byte key[ 32 ], byte iv[ 16 ], size_t &outLen);
byte * Decrypt(const char * b64CipherText, byte key[ 32 ], byte iv[ 16 ], size_t &outLen);

#pragma comment(lib, "CryptLib.lib")
#pragma comment(lib, "cryptolib.lib")