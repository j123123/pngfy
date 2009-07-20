#include "md5file.h"
unsigned char hexd[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e','f'};
static void digest_to_string(unsigned char digest[16], char str[33]) {
    int i;
  for (i=0; i<16; i++) {
    str[2*i] = hexd[(digest[i] & 0xf0) >> 4];
    str[2*i+1] = hexd[digest[i] & 0xf];
  }
  str[32] = '\0';
}

int md5_file(const char* name, char md5[MD5_DIGEST_LENGTH]) {
  FILE* fp = fopen(name, "rb");
  if (fp==NULL) return -1;
  MD5_CTX context;
  MD5_Init(&context);

  size_t rb;
  unsigned char buf[1024];

  while ((rb = fread(buf, 1, 1024, fp)) != 0) {
    MD5_Update(&context, buf, rb);
  }
  unsigned char tmd5[MD5_DIGEST_LENGTH];
  MD5_Final(tmd5, &context);
  digest_to_string(tmd5,md5);
  fclose(fp);
  return 0;
}
