/*
Problema da Barbearia utilizando semaforos, variaveis de condicao e threads
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_BARB 3
#define NUM_CX 1
#define VAGAS 20
#define NUM_PAG 50
#define NUM_CLI 50
#define NUM_SOFA 4

typedef struct fila fila;

//Declaração do tipo fila
struct fila{
    unsigned long int inicio,fim,qtd,tam;
    long* vetor;
};

//Função para inicializar fila
fila* create_f(unsigned long int tamanho){
	fila* f = (fila*) malloc(sizeof(fila));
 	if(f!=NULL){
 		f->inicio = 0;
 		f->fim = tamanho-1;
 		f->qtd = 0;
 		f->tam = tamanho;
 		f->vetor = (long*) malloc(sizeof(long)*f->tam);
 		if(f->vetor == NULL) return NULL;
 		else return f;
 	}
 	else return NULL;
}

//Função que retorna 1 se a fila está cheia ou 0 senão
long full_f(fila *f){
	if(f->qtd == f->tam) return 1;
	else return 0;
}

//Função para checar se a fila está vazia
long isEmpty_f(fila *f){
	if(f->qtd == 0) return 1;
	else return 0;
}

//Função para enfileirar elemento
void push_f(fila *f,long x){
	if(full_f(f)==1) printf("FILA CHEIA!");
	else{
		  f->fim = (f->fim+1)%f->tam;
		  f->vetor[f->fim] = x;
		  f->qtd++;
	}
}

//Função para desenfileirar primeiro elemento da fila (NULL se vazia)
long pop_f(fila *f){
 	if(isEmpty_f(f)==1){
    return -1;
  }
 	else{
 		unsigned long int x = f->inicio;
 		f->inicio = (f->inicio+1)%f->tam;
 		f->qtd--;
 		return f->vetor[x];
 	}
}

//Retorna primeiro elemento da fila (NULL - Caso fila vazia)
long front_f(fila*f){
	if(isEmpty_f(f)==1) return 0;
	else return f->vetor[f->inicio];
}

//Retorna tamanho da fila
unsigned long int size_f(fila* f){
  return f->tam;
}

//Função para destuir a fila e todos os nós referentes
void destroy_f(fila* f){
  free(f->vetor);
  free(f);
}


sem_t barbearia, cadeiras, vagas_sofa, sofa, barbeiro, caixa, recibo, atendimento; // semaforos para controle 
pthread_mutex_t mutex_sofa,mutex_pag;	// mutex para acessar buffer
pthread_cond_t pgto_caixa; //Variavel para controle do pagamento
int cont_sofa,cont_pag; 	// contadores de pessoas no sofa e de pagamentos no aguardo de recibo
fila* f;

// corpo de cliente
void *cliBody (void *id)
{
   long tid;
   tid = (long) id;

  sem_wait(&barbearia);
  if(sem_trywait(&cadeiras)==-1){
      sem_wait(&vagas_sofa);
      pthread_mutex_lock(&mutex_sofa);
      cont_sofa++;
      sem_wait(&sofa);
      pthread_mutex_unlock(&mutex_sofa);
  }
  sem_post(&barbeiro);
  sem_wait(&atendimento);
  if(cont_sofa > 0){
      pthread_mutex_lock(&mutex_sofa);
      cont_sofa--;
      sem_post(&sofa);
      sem_post(&vagas_sofa);
      pthread_mutex_unlock(&mutex_sofa);
  }
  else{
      sem_post(&cadeiras);
  }
  pthread_mutex_lock(&mutex_pag);
  push_f(f,tid);
  cont_pag++;
  pthread_cond_signal (&pgto_caixa);
  pthread_mutex_unlock(&mutex_pag);
  sem_wait(&recibo);
  sem_post(&barbearia);

  pthread_exit (NULL) ;

}

// corpo de barbeiro
void *barbBody (void *id)
{
  while(1){
      sem_wait(&barbeiro);
      sem_post(&atendimento);
      sem_wait(&caixa);
  }

  pthread_exit (NULL) ;
}


// corpo de caixa
void *caixaBody (void *id)
{
  while(1){
      pthread_mutex_lock(&mutex_pag);
      while(cont_pag<1) pthread_cond_wait (&pgto_caixa, &mutex_pag);
      printf("Recibo: Cliente %ld, Situação: PAGO\n",pop_f(f));
      cont_pag--;
      pthread_mutex_unlock(&mutex_pag);
      sem_post(&recibo);
      sem_post(&caixa);

  }

  pthread_exit (NULL) ;
}


// programa principal
int main (int argc, char *argv[])
{
  pthread_t clientes[NUM_CLI] ;
  pthread_t barbeiros[NUM_BARB] ;
  pthread_t caixat;
  long i ;

  f = create_f((unsigned long int) NUM_PAG); //Fila de pagamentos

  cont_sofa = 0;
  cont_pag = 0;

  // inicia semaforos
  sem_init (&barbearia, 0, VAGAS) ;
  sem_init (&cadeiras,  0, NUM_BARB) ;
  sem_init (&vagas_sofa,  0, NUM_SOFA) ;
  sem_init (&sofa, 0, 0) ;
  sem_init (&barbeiro, 0, 0);
  sem_init (&caixa, 0, 0);
  sem_init (&recibo, 0, 0);
  sem_init (&atendimento, 0, 0);

  pthread_mutex_init(&mutex_pag,NULL);
  pthread_mutex_init(&mutex_sofa,NULL);

  pthread_cond_init(&pgto_caixa,NULL);


   
   if (pthread_create(&caixat,NULL,caixaBody,(void *)1))
    {
      perror ("pthread_create") ;
      exit (1) ;
    }
  // cria barbeiros
  for (i=0; i<NUM_BARB; i++)
    if (pthread_create (&barbeiros[i], NULL, barbBody, (void *) i))
    {
      perror ("pthread_create") ;
      exit (1) ;
    }

  // cria clientes
  for (i=0; i<NUM_CLI; i++)
    if (pthread_create (&clientes[i], NULL, cliBody, (void *) i))
    {
      perror ("pthread_create") ;
      exit (1) ;
    }
  //destroy_f(f);
  // main encerra aqui
  pthread_exit (NULL) ;
}
