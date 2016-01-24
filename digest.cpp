#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "md5.h"

char *GetDigest(const char *username, const char *realm, const char *passwd, const char *method, const char *uri,
		const char *nonce, const char *nc, const char *cnonce, const char *qop, char *response) {
	char buf[1024];

	char a1[256];
	char a2[256];
	char ha1[33];
	char ha2[33];

	printf("username=%s\n", username);
	printf("realm=%s\n", realm);
	printf("passwd=%s\n", passwd);
	printf("method=%s\n", method);
	printf("uri=%s\n", uri);
	printf("nonce=%s\n", nonce);
	printf("nc=%s\n", nc);
	printf("cnonce=%s\n", cnonce);
	printf("qop=%s\n", qop);
	sprintf(a1, "%s:%s:%s", username, realm, passwd);
	MD5Encode(a1, strlen(a1), ha1);

	sprintf(a2, "%s:%s", method, uri);
	MD5Encode(a2, strlen(a2), ha2);
	sprintf(buf, "%s:%s:%s:%s:%s:%s", ha1, nonce, nc, cnonce, qop, ha2);
	printf("buf=%s\n", buf);
	MD5Encode(buf, strlen(buf), response);
	printf("response = %s\n", response);
	return response;
}


