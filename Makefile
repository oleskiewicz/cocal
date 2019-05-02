VERSION = 0.1

CC      = gcc
CFLAGS  = -std=c99 -Wall -O2 -march=native
LDFLAGS =
LDLIBS  = -lm

SRC     = cocal.c
BIN     = $(SRC:.c=)

main: $(BIN)

fmt:
	indent $(SRC) && rm -f -- '$(SRC)~'

clean:
	rm -f $(BIN)

$(BIN): $(SRC)
	$(CC) $(LDFLAGS) $(CFLAGS) -DVERSION="\"$(VERSION)\"" -o $@ $< $(LDLIBS)
