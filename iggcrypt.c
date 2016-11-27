#include <stdio.h>  
#include <string.h> 
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "aes.h"
#include "utils.h"
#include "iggcrypt.h"

static void xor_blk_with_char(uint8_t *in, uint8_t *out, uint8_t key, int len)
{
	int i;
	for(i=0; i<len; i++)
		out[i] = in[i] ^ key;
}

static int do_crypt_op(const char *inpath, const char *outpath, uint8_t *key, int op)
{
	AesCtx ctx;
	uint8_t inbuf[16], outbuf[16];
	char iv[]="auqAemaGrauGnaid";
	FILE *infp, *outfp;
	int cnt;
	int R0, R6;

	ctx.Nr = AesGenKeySched(ctx.Ek,  ctx.Dk, key, 16);
	memcpy(ctx.Iv, iv, 16);
	ctx.Mode = CBC;
	infp = fopen(inpath, "rb");
	outfp = fopen(outpath, "wb");
	R6 = 1;
	while(1){
		cnt = fread(inbuf, 1, 16, infp);
		if(16 != cnt)
			break;
		R0 = -1925330167;
		R0 = (((int64_t)R0)*R6>>32)+R6; //SMMLA.W         R0, R0, R6, R6;
		if(R6 == (29*((R0 >> 4) + (R0 >> 31)))){
			xor_blk_with_char(inbuf, outbuf, 0xaa, 16);
		}else{
			if(DO_DECRYPT == op)
				AesDecBlk(&ctx, inbuf, outbuf);
			else if(DO_ENCRYPT == op)
				AesEncBlk(&ctx, inbuf, outbuf);
			else{
				perror("unknown crypt op\n");
			}
		}
		R6++;
		fwrite(outbuf, 1, 16, outfp);
	}
	fclose(infp);
	fclose(outfp);
	return 0;
}

int gate1_crypt_op(const char *inpath, const char *outpath, uint8_t *mac, int op)
{
	uint8_t key[16];

	key[0] = mac[0];
	key[1] = mac[5];
	key[2] = mac[3];
	key[3] = mac[2];
	key[4] = mac[4];
	key[5] = mac[1];
	key[6] = mac[0];
	key[7] = mac[5];
	key[8] = mac[3];
	key[9] = mac[2];
	key[10] = mac[4];
	key[11] = mac[1];
	key[12] = mac[0];
	key[13] = mac[5];
	key[14] = mac[3];
	key[15] = mac[2];
	return do_crypt_op(inpath, outpath, key, op);
}

int gate2_crypt_op(const char *inpath, const char *outpath, uint8_t *mac, uint32_t seconds, int op)
{
	uint8_t key[16];

	key[0] = mac[3] ^ ((seconds>>8)&0xff);
	key[1] = mac[4] ^ ((seconds>>8)&0xff);
	key[2] = mac[2] ^ (seconds&0xff);
	key[3] = mac[0] ^ (seconds&0xff);
	key[4] = mac[1] ^ (seconds&0xff);
	key[5] = mac[5] ^ ((seconds>>8)&0xff);
	key[6] = mac[3] ^ ((seconds>>8)&0xff);
	key[7] = mac[4] ^ ((seconds>>8)&0xff);
	key[8] = mac[2] ^ (seconds&0xff);
	key[9] = mac[0] ^ (seconds&0xff);
	key[10] = mac[1] ^ (seconds&0xff);
	key[11] = mac[5] ^ ((seconds>>8)&0xff);
	key[12] = mac[3] ^ ((seconds>>8)&0xff);
	key[13] = mac[4] ^ ((seconds>>8)&0xff);
	key[14] = mac[2] ^ (seconds&0xff);
	key[15] = mac[0] ^ (seconds&0xff);
	return do_crypt_op(inpath, outpath, key, op);
}

int gate3_crypt_op(const char *inpath, const char *outpath, uint8_t *mac, uint32_t seconds, int op)
{
	uint8_t key[16];

	key[0] = mac[0] ^ (seconds&0xff);
	key[1] = mac[2] ^ (seconds&0xff);
	key[2] = mac[1] ^ (seconds&0xff);
	key[3] = mac[4] ^ ((seconds>>8)&0xff);
	key[4] = mac[3] ^ ((seconds>>8)&0xff);
	key[5] = mac[5] ^ ((seconds>>8)&0xff);
	key[6] = mac[0] ^ (seconds&0xff);
	key[7] = mac[2] ^ (seconds&0xff);
	key[8] = mac[1] ^ (seconds&0xff);
	key[9] = mac[4] ^ ((seconds>>8)&0xff);
	key[10] = mac[3] ^ ((seconds>>8)&0xff);
	key[11] = mac[5] ^ ((seconds>>8)&0xff);
	key[12] = mac[0] ^ (seconds&0xff);
	key[13] = mac[2] ^ (seconds&0xff);
	key[14] = mac[1] ^ (seconds&0xff);
	key[15] = mac[4] ^ ((seconds>>8)&0xff);
	return do_crypt_op(inpath, outpath, key, op);
}

int igg_checksum(const char *fname, uint32_t *checksum)
{
	uint8_t *buf;
	uint32_t sum = 0x1505;
	int i, fsize;
	
	buf = load_file(fname, &fsize);
	if(NULL == buf)
		return -1;
	for(i=0; i<fsize; i++)
		sum = buf[i] + 33*sum;
	*checksum = sum;
	free(buf);
	return 0;
}