#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#define ESTRELAS_CSV "estrelas.csv"
#define INITIAL_STRING_SIZE 64

static int toy(int argc, char** argv);
static int stars(int argc, char** argv);

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

	return toy(argc, argv);
}


int stars(int argc, char** argv) {
	/* TODO */
	return 0;
}

int toy(int argc, char** argv) {

	int i, sz = 3;
	Estrela *estrela;
	Estrela estrelas[] = {
		{"estrela 1", "estrela", 1}
		, {"estrela 2", "estrela", 2}
		, {"estrela 3", "estrela", 3}
	};

	HashTable *H = HashTable_newwith(sz, sz, Estrela_hash, NULL);

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
	HashTable_free(H);

	printf("\nFinished.");
	return 0;
}