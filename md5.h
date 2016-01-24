/* MD5.H - header file for MD5.CC*/

#ifndef _MY_MD5_H_
#define _MY_MD5_H_ 1

#ifdef __cplusplus
extern "C"{
typedef unsigned char *POINTER;

// UINT2 defines a two byte word 
typedef unsigned short int UINT2;

// UINT4 defines a four byte word
typedef unsigned long int UINT4;
/* MY_MD5 context. */
typedef struct {
	UINT4 state[4];                                   /* state (ABCD) */
	UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

extern void MD5Init (MD5_CTX * context);
extern void MD5Update (MD5_CTX *context, unsigned char *input, unsigned int inputLen);
extern void MD5Final (unsigned char [16], MD5_CTX *);

extern void MD5Transform (UINT4 [4], unsigned char [64]);
extern void Encode (unsigned char *, UINT4 *, unsigned int);
extern void Decode (UINT4 *, unsigned char *, unsigned int);

extern void MD5_memcpy (POINTER, POINTER, unsigned int);
extern void MD5_memset (POINTER, int, unsigned int);
extern void ToMD5(unsigned char [16],  void *, unsigned int);
extern char *MD5Encode(void *src, unsigned int len, char *secret);
};
#endif

#endif
