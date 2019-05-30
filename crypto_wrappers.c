#include <openssl/sha.h>
#include <openssl/md5.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef int (*fptr)();

char* charTohex(unsigned char c){
  char* ret = (char*)malloc(sizeof(char)*3);
  const char * hex = "0123456789ABCDEF";
  ret[2] = 0;
  ret[0] = hex[(c>>4)&0xF];
  ret[1] = hex[c&0xF];
  return ret;
}

char* strTohex(unsigned char* c, int length){
  char* retVal = malloc(sizeof(char)*length*2 + 1);
  int i;
  for(i = 0; i < length; i++){
    unsigned char tempVal = c[i];
    char* buffer = charTohex(tempVal);
    retVal[i*2] = buffer[0];
    retVal[i*2+1] = buffer[1];
  }
  retVal[length*2] = 0;
  return retVal;
  return c;
}

char* sha1_wrapper(unsigned char* c){
  int length = strlen(c);
  unsigned char hash[20];
  SHA1(c, length, hash);
  return strTohex(hash, 20);
}

char* sha256_wrapper(unsigned char* c){
  int length = strlen(c);
  unsigned char hash[32];
  SHA256(c, length, hash);
  return strTohex(hash, 32);
}

char* md5_wrapper(unsigned char* c){
  int length = strlen(c);
  unsigned char hash[16];
  MD5(c, length, hash);
  return strTohex(hash, 16);
}

fptr pick_hash(char* c){
  if(strcmp(c, "SHA1")==0)
    return sha1_wrapper;
  if(strcmp(c, "SHA256")==0)
    return sha256_wrapper;
  if(strcmp(c, "MD5")==0)
    return md5_wrapper;
  return md5_wrapper;
}

int main(int argc, char** argv){

  if(argc != 3){
    printf("Expected usage: %s hash_function plain_text\n", argv[0]);
    exit(-1);
  }

  char* hash_function = argv[1];
  char* plain_text = argv[2];

  fptr hash = pick_hash(hash_function);

  printf("%s %s->%s\n", hash_function, plain_text, hash(plain_text));
  /*
  printf("%s %s->%s\n", "SHA256", plain_text, sha256_wrapper(plain_text));
  printf("%s %s->%s\n", "SHA1", plain_text, sha1_wrapper(plain_text));
  printf("%s %s->%s\n", "MD5", plain_text, md5_wrapper(plain_text));
  */
}
