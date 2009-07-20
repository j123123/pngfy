#ifndef MD5F_H
#define MD5F_h
#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
int md5_file(const char* name, char md5[MD5_DIGEST_LENGTH]);
#endif
