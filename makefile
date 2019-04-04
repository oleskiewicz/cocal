include config.mk

SRC = cocal.c
OBJ = $(SRC:.c=.o)
BIN = $(SRC:.c=)

all: $(BIN)

fmt:
	indent $(SRC)
	rm -f -- '$(SRC)~'

tags:
	ctags -R .

clean:
	rm -f $(BIN) $(OBJ) tags

%: %.c
	$(CC) $(CFLAGS) -DVERSION="\"$(VERSION)\"" -o $@ $< $(LDFLAGS)
