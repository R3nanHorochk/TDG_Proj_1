/* 
 * REPRESENTACAO DE GRAFOS - Versao 2026-2
 */

#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<memory.h>

typedef enum {
    ESTAGIARIO,
    JUNIOR,
    PLENO,
    SENIOR,
    TECH_LEAD
} NivelCargo;

const char* NOMES_CARGOS[] = {
    "Estagiario",
    "Junior",
    "Pleno",
    "Senior",
    "Tech Lead"
};

/* 
 * Estrutura de dados para representar grafos
 */
typedef struct aresta{ /* Celula de uma lista de arestas */
	int    outroExtremo;
	int    NProjetos;
	int    HorasJuntas;
	struct aresta *prox;
}Aresta;

typedef struct vertice{  /* Cada vertice tem um ponteiro para uma lista de arestas incidentes nele */
	int id;                    
    char nome[50];      
    NivelCargo cargo;   
	Aresta *a;
}Vertice;

/*
 * Declaracoes das funcoes para manipulacao de grafos 
 */
void criarGrafo(Vertice **G, int ordem);
void destruirGrafo(Vertice **G, int ordem);
int  acrescentarAresta(Vertice G[], int ordem, int v1, int v2);
void imprimirGrafo(Vertice G[], int ordem);

 
/*
 * Criacao de um grafo com ordem predefinida (passada como argumento),
 *   e, inicilamente, sem nenhuma aresta 
 */
void criarGrafo(Vertice **G, int ordem){
    int i;
    *G = (Vertice*) malloc(sizeof(Vertice) * ordem);
    
    for(i = 0; i < ordem; i++){
        (*G)[i].id = -1;
        (*G)[i].cargo = 0;       
        strcpy((*G)[i].nome, "");     
        (*G)[i].a = NULL;               
    }
}

/*
 * Cadastra ou atualiza os dados de um trabalhador no vertice indicado pelo 'id'.
 * Retorna 1 em caso de sucesso, ou 0 se o id for invalido.
 */
int cadastrarTrabalhador(Vertice G[], int ordem, int id,char *nome, NivelCargo cargo) {
    if (id < 0 || id >= ordem) {
        printf("[ERRO] ID %d fora dos limites do grafo (0 a %d).\n", id, ordem - 1);
        return 0;
    }
	if (G[id].id != -1) {
    printf("[ERRO] O vertice %d ja esta cadastrado com o trabalhador '%s'.\n", id, G[id].nome);
    return 0;
	}
    G[id].id = id;
    
    /* Copia o nome com limite de seguranca */
    strncpy(G[id].nome, nome, sizeof(G[id].nome) - 1);
    G[id].nome[sizeof(G[id].nome) - 1] = '\0';

    G[id].cargo = cargo;


    return 1;
}
/*
 * Deasaloca a memoria dinamica usada para armazenar um grafo.
 */
void destruirGrafo(Vertice **G, int ordem){
	int i;
    Aresta *p, *n;
    
	for(i=0; i<ordem; i++){ /* Remove lista de adjacencia de cada vertice */
	    p= (*G)[i].a;
        while (p!= NULL){
              n= p->prox;
              free(p);
              p= n;
        }
	}
    free(*G);  /* Remove o vetor de vertices */
}

/*  
 * Acrescenta uma nova aresta em um grafo previamente criado. 
 *   Devem ser passados os extremos v1 e v2 da aresta a ser acrescentada 
 * Como o grafo nao e� orientado, para uma aresta com extremos i e j, quando
 *   i != j, serao criadas, na estrutura de dados, arestas (i,j) e (j,i) .
 */
int acrescentarAresta(Vertice G[], int ordem, int v1, int v2) {
    Aresta *A1, *A2, *aux;

   

    /* 1. Validacao de limites dos vertices */
    if (v1 < 0 || v1 >= ordem) {
        printf("  -> ERRO: Vertice v%d invalido (fora do intervalo 0 a %d).\n", v1, ordem - 1);
        return 0;
    }
    if (v2 < 0 || v2 >= ordem) {
        printf("  -> ERRO: Vertice v%d invalido (fora do intervalo 0 a %d).\n", v2, ordem - 1);
        return 0;
    }

    /* 2. Em redes de colaboracao, nao ha parceria de uma pessoa consigo mesma */
    if (v1 == v2) {
        printf("  -> AVISO: Um trabalhador nao pode registrar parceria consigo mesmo (v%d == v%d).\n", v1, v2);
        return 0;
    }

    /* 3. VERIFICACAO: Verifica se a parceria ja existe na lista de v1 */
    /* 3. VERIFICACAO: Percorre com while ate chegar em NULL */
    aux = G[v1].a;
    while (aux != NULL) {
        if (aux->outroExtremo == v2) {
            /* Incrementa na lista de v1 */
            aux->NProjetos++;

            /* Incrementa reciprocamente na lista de v2 */
            Aresta *aux2 = G[v2].a;
            while (aux2 != NULL) {
                if (aux2->outroExtremo == v1) {
                    aux2->NProjetos++;
                    break;
                }
                aux2 = aux2->prox;
            }
            return 1;
        }
        aux = aux->prox;
    }

    /* 4. SE NAO EXISTIA: Aloca aresta na lista de v1 */
    A1 = (Aresta *) malloc(sizeof(Aresta));
    A1->outroExtremo = v2;
    A1->NProjetos = 1;         /* Primeiro projeto juntos */
    A1->prox = G[v1].a;
    G[v1].a = A1;

    /* 5. Aloca aresta reciproca na lista de v2 */
    A2 = (Aresta *) malloc(sizeof(Aresta));
    A2->outroExtremo = v1;
    A2->NProjetos = 1;         /* Primeiro projeto juntos */
    A2->prox = G[v2].a;
    G[v2].a = A2;
    return 1;
}


/*  
 * Imprime um grafo com uma notacao similar a uma lista de adjacencia.
 */
void imprimirGrafo(Vertice G[], int ordem) {
    int i;
    Aresta *aux;

    

    for (i = 0; i < ordem; i++) {
        /* Imprime o id, nome e o cargo textual */
        printf("\n[%d] %s (%s)", G[i].id, G[i].nome, NOMES_CARGOS[G[i].cargo]);

        aux = G[i].a;
        if (aux == NULL) {
            printf("\n    └── Sem parcerias registradas.");
        } else {
            printf("\n    Parcerias ativas:");
            while (aux != NULL) {
                int idParceiro = aux->outroExtremo;
                printf("\n      └── com %s (%s) [%d projeto(s) juntos]",
                       G[idParceiro].nome,
                       NOMES_CARGOS[G[idParceiro].cargo],
                       aux->NProjetos);
                aux = aux->prox;
            }
        }
        printf("\n");
    }
}

/*
 * Programa simples para testar a representacao de grafo
 */
int main(int argc, char *argv[]) {
    Vertice *G;
    int ordemG = 8; /* 8 desenvolvedores: IDs de 0 a 7 */

    /* 1. Aloca a estrutura do grafo */
    criarGrafo(&G, ordemG);

    /* 2. Cadastra os dados dos trabalhadores */
    cadastrarTrabalhador(G, ordemG, 0, "Alice Santos",   TECH_LEAD);
    cadastrarTrabalhador(G, ordemG, 1, "Bob Silva",      SENIOR);
    cadastrarTrabalhador(G, ordemG, 2, "Carlos Lima",    PLENO);
    cadastrarTrabalhador(G, ordemG, 3, "Diana Souza",    PLENO);
    cadastrarTrabalhador(G, ordemG, 4, "Eduardo Reis",   JUNIOR);
    cadastrarTrabalhador(G, ordemG, 5, "Fernanda Costa", JUNIOR);
    cadastrarTrabalhador(G, ordemG, 6, "Gabriel Rocha",  ESTAGIARIO);
    cadastrarTrabalhador(G, ordemG, 7, "Helena Ramos",   ESTAGIARIO);

    /* 3. Registra os projetos compartilhados (conexoes entre devs) */
    
    acrescentarAresta(G, ordemG, 0, 1);
    acrescentarAresta(G, ordemG, 0, 2);
    acrescentarAresta(G, ordemG, 0, 7);
    acrescentarAresta(G, ordemG, 2, 4);
    acrescentarAresta(G, ordemG, 2, 5);
    acrescentarAresta(G, ordemG, 2, 5); /* Teste: reforco de parceria (mesmo projeto 2x) */
    acrescentarAresta(G, ordemG, 3, 5);
    acrescentarAresta(G, ordemG, 4, 6);
    acrescentarAresta(G, ordemG, 3, 6);
    acrescentarAresta(G, ordemG, 7, 7); /* Teste: laco (bloqueado pelo aviso) */

    /* 4. Exibe a rede social com nomes, cargos e projetos */
    imprimirGrafo(G, ordemG);

    /* 5. Libera a memoria alocada */
    destruirGrafo(&G, ordemG);

    getchar();
    return 0;
}