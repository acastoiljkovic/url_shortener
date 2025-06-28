#ifndef SERVER_H
#define SERVER_H

int start_http_server(int port);

void set_server_config(int port, const char *base_url, int ttl);

#endif
