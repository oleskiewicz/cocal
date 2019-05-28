VERSION = 0.2

CC      = gcc
CFLAGS  = -std=c99 -Wall -O2 -march=native
LDFLAGS =
LDLIBS  = -lm

SRC     = cocal.c
BIN     = $(SRC:.c=)

all: $(BIN)

clean:
	rm -f $(BIN)

%: %.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DVERSION="\"$(VERSION)\"" -o $@ $< $(LDLIBS)
