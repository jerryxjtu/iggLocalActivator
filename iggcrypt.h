
#ifndef __IGG_CRYPT_H__

#define DO_ENCRYPT 1
#define DO_DECRYPT 2

int gate1_crypt_op(const char *inpath, const char *outpath, uint8_t *mac, int op);
int gate2_crypt_op(const char *inpath, const char *outpath, uint8_t *mac, uint32_t seconds, int op);
int gate3_crypt_op(const char *inpath, const char *outpath, uint8_t *mac, uint32_t seconds, int op);
int igg_checksum(const char *fname, uint32_t *checksum);
#endif