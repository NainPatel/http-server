#ifndef HTTP_PARSE
#define HTTP_PARSE

#include<openssl/ssl.h>

void
processClientRequest(SSL *ssl);

#endif
