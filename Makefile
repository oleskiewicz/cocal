VERSION = 0.1

CC      = gcc
CFLAGS  = -std=c99 -O3 -Wall
LDFLAGS = -lm

cocal: cocal.c
	$(CC) $(CFLAGS) $(LDFLAGS) -DVERSION="\"$(VERSION)\"" -o $@ $<

fmt:
	indent cocal.c && rm -f -- 'cocal.c~'

clean:
	rm -f cocal
