CC = musl-gcc
VERSION ?= 1.0.0
CFLAGS = -Iinclude -I/usr/local/include -Wall -Wextra -static -DAPP_VERSION=\"$(VERSION)\"
LDFLAGS = -L/usr/local/lib -lhiredis -lmicrohttpd


SRC = src/*
OUT = url_shortener

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)
