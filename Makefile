VERSION = 0.3

CC      = gcc
CFLAGS  = -std=c99 -Wall -O2 -march=native
LDFLAGS =
LDLIBS  = -lm

SRC     = cosmo.c
BIN     = $(SRC:.c=)

all: $(BIN)

clean:
	rm -f $(BIN)

%: %.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DVERSION="\"$(VERSION)\"" -o $@ $< $(LDLIBS)
