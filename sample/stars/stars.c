#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#define ESTRELAS_CSV "estrelas.csv"
#define INITIAL_STRING_SIZE 64

static int toy(int argc, char** argv);
static int stars(int argc, char** argv);
static Estrela *loaddata(char* filename, int* size);
static void freedata(Estrela* estrela);

typedef struct _Estrela {

	char* nome;
	char* classe;
	int velocidade;
} Estrela;


/* Método recebe nome da estrela, mas poderia ser outra
 * coisa! */
size_t Estrela_hash(void* key) {

	int retorno = 0;

	if (key) {
		char *nomeEstrela = key;
		int i;

		/* só pra exemplo! */
		retorno = 33;
		for (i = 0; nomeEstrela[i]; i++)
			retorno = (nomeEstrela[i]) + (retorno << 6) + (retorno << 16) - retorno;
	}

	return retorno;
}


int main(int argc, char** argv) {

	return stars(argc, argv);
}


int stars(int argc, char** argv) {

	return 0;
}


Estrela *loaddata(char* filename, int* size) {

	FILE* f;
	Estrela *r;
	char c;
	char *str;
	int line_count = -1;
	int column_count = 0;
	size_t r_pos = 0;
	size_t str_sz = INITIAL_STRING_SIZE;
	size_t str_pos = 0;

	*size = 0;
	if (filename && size) {

		/* Open file and allocate string buffer */
		f = fopen(filename, "r");
		if (!f) return NULL;

		str = realloc(NULL, sizeof(char) * str_sz);
		if (!str) {
			fclose(f);
			return NULL;
		}
		str[0] = '\0';

		/* Iterate over file's contents */
		while (!feof(f)) {
			c = fgetc(f);

			/* Check if the string buffer is not too small. */
			if (str_pos == str_sz) {
				str_sz *= 2;
				str = realloc(str, str_sz);

				if (!str) {
					fclose(f);
					return NULL;
				}
			}

			switch(c) {
				case '\n': {

					break;
				} case ',': {

					break;
				} default: {
					str[str_pos++] = c;
					str[str_pos] = '\0';
					break;
				}
			}
		}

		free(s);
		fclose(f);
	}

	return r;
}

int toy(int argc, char** argv) {

	int i, sz = 3;
	Estrela *estrela;
	Estrela estrelas[] = {
		{"estrela 1", "estrela", 1}
		, {"estrela 2", "estrela", 2}
		, {"estrela 3", "estrela", 3}
	};

	HashTable *H = HashTable_newWith(sz, sz, Estrela_hash, NULL);

	/* Péssima forma de verificar erros, mas é só um exemplo! */
	if (!H) {
		printf("\n*** Não foi possível criar o hashtable! ***\n");
		return 1;
	}

	/* Inserção */
	for (i = 0; i < sz; i++) {
		if (!HashTable_put(H, estrelas[i].nome, &estrelas[i])) {
			printf("\n*** Erro ao inserir estrela '%s'!", estrelas[i].nome);
			return 1;
		}
	}

	/* Listagem */
	for (i = 0; i < sz; i++) {
		estrela = HashTable_get(H, estrelas[i].nome);
		printf("\n%s: 0x%p == 0x%p", estrelas[i].nome, estrela, &estrelas[i]);

		/*
		printf("\n%s (Classe '%s') = %d km/s (Igual? %s)",
			estrela->nome, estrela->classe, estrela->velocidade,
			estrela == &estrelas[i] ? "Sim" : "Não");
		*/
	}

	/* Finalização */
	HashTable_free(H, NULL, NULL);

	printf("\nFinished.");
	return 0;
}