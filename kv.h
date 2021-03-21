#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SEP " ,;"

static inline char *
strtrim(char *in) {
	char *out = strndup(in, strnlen(in, BUFSIZ));
	char *end;

	while (isspace((int)*out))
		out++;

	if (*out == 0)
		return out;

	end = out + strnlen(out, BUFSIZ) - 1;
	while (end > out && isspace((int)*end))
		end--;

	end[1] = '\0';
	return out;
}

static inline bool
atob(const char *str) {
	if (strnlen(str, BUFSIZ) > 0) {
		if ((str[0] == 'T') || (str[0] == 't')) {
			return true;
		}
		else if ((str[0] == 'F') || (str[0] == 'f')) {
			return false;
		}
		else {
			fprintf(stderr, "boolean must be true or false, got \"%s\"\n", str);
			exit(1);
		}
	}
	else {
		fprintf(stderr, "boolean must be non-empty");
		exit(1);
	}
}

static inline char
atoc(char *str) {
	return strnlen(str, BUFSIZ) > 0 ? str[0] : '\0';
}

static inline int *
atoai(char *str) {
	int *a = NULL;
	int i = 0;
	char *tok = strtok(str, SEP);
	do {
		a = realloc(a, i * sizeof(int));
		a[i] = atoi(tok);
		i++;
	} while ((tok = strtok(NULL, SEP)));
	return a;
}

static inline double *
atoaf(char *str) {
	double *a = NULL;
	int i = 0;
	char *tok = strtok(str, SEP);
	do {
		a = realloc(a, i * sizeof(int));
		a[i] = atof(tok);
		i++;
	} while ((tok = strtok(NULL, SEP)));
	return a;
}

#define X(type, name, init) \
	type name;
typedef struct {
	KV
} kv;
#undef X

kv
kv_init() {
	kv c;
#define X(type, name, init) \
	c.name = init;
	KV
#undef X
	return c;
}

void
kv_print(FILE * f, kv c) {
#define X(type, name, init) \
	fprintf( \
		f, \
		_Generic((type){0}, \
			bool: "%s = %d\n", \
			int: "%s = %d\n", \
			int *: "%s = %s\n", \
			double: "%s = %g\n", \
			double *: "%s = %s\n", \
			char: "%s = %c\n", \
			char *: "%s = %s\n" \
		), \
		#name, \
		_Generic((type){0}, \
			int *:  "array of ints", \
			double *: "array of floats", \
			default: c.name \
		) \
	);
	KV
#undef X
}

void
kv_read(FILE * f, kv *c) {
	char *_k = calloc(BUFSIZ, 1);
	char *_v = calloc(BUFSIZ, 1);
#define X(type, name, init) \
	if (!strncmp(k, #name, MAX(strnlen(k, BUFSIZ), strnlen(#name, BUFSIZ)))) \
		c->name = _Generic((type){0}, \
			bool: atob, \
			int: atoi, \
			int *: atoai, \
			double: atof, \
			double *: atoaf, \
			char: atoc, \
			char *: strdup \
		)(v);
	while (fscanf(f, "%256[^=]=%256[^=\n]", _k, _v) == 2) {
		char *k = strtrim(_k), *v = strtrim(_v);
		KV
	}
	free(_k), free(_v);
#undef X
}
