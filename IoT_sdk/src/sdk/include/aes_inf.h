/*
aes_inf.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/
#ifndef _AES_INF_H
#define _AES_INF_H

#include "tuya_os_adapter.h"
#include "tuya_cloud_error_code.h"
#include "tuya_cloud_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef VOID (*AES128_ECB_ENC_BUF)(IN CONST BYTE_T *input, IN CONST UINT_T length, IN CONST BYTE_T *key,OUT BYTE_T *output);
typedef VOID (*AES128_ECB_DEC_BUF)(IN CONST BYTE_T *input, IN CONST UINT_T length, IN CONST BYTE_T *key,OUT BYTE_T *output);

typedef VOID (*AES128_CBC_ENC_BUF)(IN CONST BYTE_T *input, IN CONST UINT_T length, IN CONST BYTE_T *key, IN BYTE_T *iv, OUT BYTE_T *output);
typedef VOID (*AES128_CBC_DEC_BUF)(IN CONST BYTE_T *input, IN CONST UINT_T length, IN CONST BYTE_T *key, IN BYTE_T *iv, OUT BYTE_T *output);

typedef struct {
    AES128_ECB_ENC_BUF ecb_enc_128;
    AES128_ECB_DEC_BUF ecb_dec_128;
    AES128_CBC_ENC_BUF cbc_enc_128;
    AES128_CBC_DEC_BUF cbc_dec_128;
}AES_METHOD_REG_S;

#define AES128_ENCRYPT_KEY_LEN 16


typedef enum TUYA_HW_AES_MODE_ {
    TUYA_HW_AES_MODE_ENCRYPT,
    TUYA_HW_AES_MODE_DECRYPT,
} TUYA_HW_AES_MODE_E;


typedef enum TUYA_HW_AES_CRYPT_MODE_ {
    TUYA_HW_AES_CRYPT_MODE_ECB,
    TUYA_HW_AES_CRYPT_MODE_CBC,
    TUYA_HW_AES_CRYPT_MODE_CFB,
    TUYA_HW_AES_CRYPT_MODE_OFB,
} TUYA_HW_AES_CRYPT_MODE_E;


typedef struct TUYA_HW_AES_PARAM_ {
    TUYA_HW_AES_MODE_E        method;
    TUYA_HW_AES_CRYPT_MODE_E  encryptMode;
} TUYA_HW_AES_PARAM_S;


typedef struct TUYA_HW_AES_ {
    int (*aes_create)(void** pphdl, TUYA_HW_AES_PARAM_S* pparam);
    int (*aes_destroy)(void* phdl);
    int (*aes_setkey_enc)(void* phdl, const unsigned char *key, unsigned int keybits);
    int (*aes_setkey_dec)(void* phdl, const unsigned char *key, unsigned int keybits);
    int (*aes_crypt_ecb)(void* phdl, const unsigned char* input, size_t length, unsigned char* output);
    int (*aes_crypt_cbc)(void* phdl, const unsigned char* iv, unsigned int ivbits, const unsigned char *input, size_t length, unsigned char *output);
} TUYA_HW_AES_S;



UINT_T aes_pkcs7padding_buffer(BYTE_T *p_buffer, UINT_T length);

OPERATE_RET aes_method_register(IN CONST AES_METHOD_REG_S *aes, IN CONST TUYA_HW_AES_S* pafunc);

VOID aes_method_unregister(VOID);


OPERATE_RET aes192_cbc_encode(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                            IN CONST BYTE_T *key,IN BYTE_T *iv,\
                            OUT BYTE_T **ec_data,OUT UINT_T *ec_len);
OPERATE_RET aes192_cbc_decode(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                            IN CONST BYTE_T *key,IN BYTE_T *iv,\
                            OUT BYTE_T **dec_data,OUT UINT_T *dec_len);


OPERATE_RET aes128_ecb_encode(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                              OUT BYTE_T **ec_data,OUT UINT_T *ec_len,\
                              IN CONST BYTE_T *key);
OPERATE_RET aes128_ecb_decode(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                              OUT BYTE_T **dec_data,OUT UINT_T *dec_len,\
                              IN CONST BYTE_T *key);
OPERATE_RET aes128_cbc_encode(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                              IN CONST BYTE_T *key,IN BYTE_T *iv,\
                              OUT BYTE_T **ec_data,OUT UINT_T *ec_len);
OPERATE_RET aes128_cbc_decode(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                              IN CONST BYTE_T *key,IN BYTE_T *iv,\
                              OUT BYTE_T **dec_data,OUT UINT_T *dec_len);
OPERATE_RET aes_free_data(IN BYTE_T *data);
INT_T aes_get_actual_length(IN CONST BYTE_T *dec_data,IN CONST UINT_T dec_data_len);


OPERATE_RET aes192_cbc_encode_raw(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                                  IN CONST BYTE_T *key,IN BYTE_T *iv,\
                                  OUT BYTE_T *ec_data);
OPERATE_RET aes192_cbc_decode_raw(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                                  IN CONST BYTE_T *key,IN BYTE_T *iv,\
                                  OUT BYTE_T *dec_data);

OPERATE_RET aes256_cbc_encode_raw(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                                  IN CONST BYTE_T *key,IN BYTE_T *iv,\
                                  OUT BYTE_T *ec_data);


OPERATE_RET aes128_ecb_encode_raw(IN CONST BYTE_T *data, IN CONST UINT_T len,\
                                  OUT BYTE_T *ec_data,IN CONST BYTE_T *key);
OPERATE_RET aes128_ecb_decode_raw(IN CONST BYTE_T *data, IN CONST UINT_T len,\
                                  OUT BYTE_T *dec_data,IN CONST BYTE_T *key);
OPERATE_RET aes128_cbc_encode_raw(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                                  IN CONST BYTE_T *key,IN BYTE_T *iv,\
                                  OUT BYTE_T *ec_data);
OPERATE_RET aes128_cbc_decode_raw(IN CONST BYTE_T *data,IN CONST UINT_T len,\
                                  IN CONST BYTE_T *key,IN BYTE_T *iv,\
                                  OUT BYTE_T *dec_data);


#define aes128_free_data                    aes_free_data
#define aes128_get_data_actual_length       aes_get_actual_length




typedef struct TUYA_HW_AES_HANDLE_ {
    int             init;
    TUYA_HW_AES_S   aesFunc;
    void*           phwHdl;
} TUYA_HW_AES_HANDLE_S;


void aes_method_get_callback_func(TUYA_HW_AES_S* paes);

int tuya_hw_aes_crypt_init(TUYA_HW_AES_HANDLE_S* paesHdl, char* pkey);

int tuya_hw_aes_crypt_uninit(TUYA_HW_AES_HANDLE_S* paesHdl);

int tuya_hw_aes_update_key(TUYA_HW_AES_HANDLE_S* paesHdl, char* pkey);

int tuya_hw_aes_encrypt_cbc(TUYA_HW_AES_HANDLE_S* paesHdl, const unsigned char* iv, unsigned int ivbits,
                            const unsigned char *input, size_t length, unsigned char *output, size_t* poutlen);


#ifdef __cplusplus
}
#endif
#endif

