#ifndef _AD_UNION_AES_WRAPPER_H_
#define _AD_UNION_AES_WRAPPER_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
 
namespace xce {
namespace ad {

#define AES_BLOCK_SIZE 256

class AesWrapper {
public:
  AesWrapper(const unsigned char *key_data, size_t key_data_len) {
    for (unsigned char i = 0; i < 8; ++ i) {
      salt_[i] = i * 2 + 1;
    }
    if (Init(key_data, key_data_len) != 0) {
    }
  }

  ~AesWrapper() {
    EVP_CIPHER_CTX_cleanup(&de_ctx_);
    EVP_CIPHER_CTX_cleanup(&en_ctx_);
  }

  int Init(const unsigned char *key_data, int key_data_len) {
    int i, nrounds = 5;
    unsigned char key[32], iv[32];

    /*
     * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
     * nrounds is the number of times the we hash the material. More rounds are more secure but
     * slower.
     */
    i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt_, key_data, key_data_len, nrounds, key, iv);
    if (i != 32) {
      printf("Key size is %d bits - should be 256 bits\n", i);
      return -1;
    }

    EVP_CIPHER_CTX_init(&en_ctx_);
    EVP_EncryptInit_ex(&en_ctx_, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_CIPHER_CTX_init(&de_ctx_);
    EVP_DecryptInit_ex(&de_ctx_, EVP_aes_256_cbc(), NULL, key, iv);

    return 0;
  }

  unsigned char * Decrypt(unsigned char *ciphertext, int *len)
  {
    /* plaintext will always be equal to or lesser than length of ciphertext*/
    int p_len = *len, f_len = 0;
    unsigned char *plaintext = (unsigned char *)malloc(p_len);

    if(!EVP_DecryptInit_ex(&de_ctx_, NULL, NULL, NULL, NULL)){
      printf("ERROR in EVP_DecryptInit_ex \n");
      return NULL;
    }

    if(!EVP_DecryptUpdate(&de_ctx_, plaintext, &p_len, ciphertext, *len)){
      printf("ERROR in EVP_DecryptUpdate\n");
      return NULL;
    }

    if(!EVP_DecryptFinal_ex(&de_ctx_, plaintext+p_len, &f_len)){
      printf("ERROR in EVP_DecryptFinal_ex\n");
      return NULL;
    }

    *len = p_len + f_len;
    return plaintext;
  }

  unsigned char * Encrypt(unsigned char *plaintext, int *len)
  {
    /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
    int c_len = *len + AES_BLOCK_SIZE - 1, f_len = 0;
    unsigned char *ciphertext = (unsigned char *)malloc(c_len);

    /* allows reusing of 'en_ctx_' for multiple encryption cycles */
    if(!EVP_EncryptInit_ex(&en_ctx_, NULL, NULL, NULL, NULL)){
      printf("ERROR in EVP_EncryptInit_ex \n");
      return NULL;
    }

    /* update ciphertext, c_len is filled with the length of ciphertext generated,
      *len is the size of plaintext in bytes */
    if(!EVP_EncryptUpdate(&en_ctx_, ciphertext, &c_len, plaintext, *len)){
      printf("ERROR in EVP_EncryptUpdate \n");
      return NULL;
    }

    /* update ciphertext with the final remaining bytes */
    if(!EVP_EncryptFinal_ex(&en_ctx_, ciphertext+c_len, &f_len)){
      printf("ERROR in EVP_EncryptFinal_ex \n");
      return NULL;
    }

    *len = c_len + f_len;
    return ciphertext;
  }
private:
  EVP_CIPHER_CTX en_ctx_, de_ctx_;
  // The salt paramter is used as a salt in the derivation: it should point to an 8 byte buffer or NULL if no salt is used.
  unsigned char salt_[8];
};

}
}

#endif // _AD_UNION_AES_WRAPPER_H_

