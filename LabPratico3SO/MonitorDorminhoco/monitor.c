/*
Problema do Monitor Dorminhoco utilizando semafóros e múltiplas threads
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>

#define NUM_ESTUDANTES 50


pthread_mutex_t mutex_cadeira; // mutex para poder atualizar o valor do número de cadeiras livres
pthread_mutex_t mutex_sala; // mutex para controlar quem está na sala
sem_t atendimentoSolicitado; // semáforo para indicar que há estudantes solicitando um atendimento
sem_t atendimentoFornecido; // semáforo para indicar que o monitor está disponível e um aluno pode entrar na sala
sem_t cadeiras_livres;
long int numero_cadeiras;
int atendimentosRealizados = 0;


void *threadMonitor (void *id)
{
  while(1) {
      sem_wait(&atendimentoSolicitado); // escuta o próximo atendimento, dorme enquanto não chegar ninguém
      printf("Monitor tem um atendimento a fazer!\n");
      usleep(1000);
      sem_post(&atendimentoFornecido); // finaliza o atendimento
      printf("Monitor terminou de atender!\n");
      atendimentosRealizados++;
      printf("Total de atendimentos até agora: %d\n", atendimentosRealizados);
      sem_post(&cadeiras_livres); // libera uma das cadeiras que um estudante ocupou
  }

  pthread_exit (NULL) ;
}

void *threadEstudante (void *id)
{
  long tid;
  tid = (long) id;


  while (sem_trywait(&cadeiras_livres) == -1) { // verifica se há cadeiras livres
    printf("Estudante de número %ld volta mais tarde!\n", tid); //estudante vai embora e volta em outro horário
    usleep(3000);
  }

  // sai do laço while caso hajam cadeiras livres

  //verifica se o monitor está atendendo alguém ou se está dormindo

  if (pthread_mutex_trylock(&mutex_sala) != 0) {
  printf("Estudante de número %ld ocupou uma cadeira!\n", tid);//ocupa uma das cadeiras porque a sala está ocupada
  sem_post(&atendimentoSolicitado); // solicita atendimento
  pthread_mutex_lock(&mutex_sala); //ocupa a sala
  sem_wait(&atendimentoFornecido); // aguarda o monitor dizer que ele pode entrar
  pthread_mutex_unlock(&mutex_sala);
  } else { // caso ninguém esteja sendo atendido, apenas entra na sala e acorda o monitor
    sem_post(&cadeiras_livres); // libera uma cadeira porque ele não precisou sentar nela
    sem_post(&atendimentoSolicitado); // entra na sala e solicita atendimento
    printf("Estudante de número %ld está entrando na sala! Acorda, monitor!\n", tid);
    sem_wait(&atendimentoFornecido); // aguarda o monitor terminar de atendê-lo
    pthread_mutex_unlock(&mutex_sala); // libera a sala
  }

  printf("Estudante de número %ld foi atendido!\n", tid);// monitor atendeu o aluno! agora ele pode ir embora!

  pthread_exit (NULL) ;
}

int main (int argc, char *argv[])
{
  pthread_t estudantes[NUM_ESTUDANTES] ;
  pthread_t monitor;
  char entrada[10];

  //Entrada de dados
  fgets (entrada, 10, stdin);
  //Conversao da entrada para long int, retorna 0 se entrada nao inteira
  numero_cadeiras = atol(entrada);

  //cadeiras_livres = numero_cadeiras; //inicia com a quantidade de cadeiras que o usuário informou

  // inicia semaforos
  sem_init (&atendimentoSolicitado,  0, 0) ;
  sem_init (&atendimentoFornecido, 0, 0) ;
  sem_init (&cadeiras_livres, 0, numero_cadeiras) ;

  pthread_mutex_init(&mutex_cadeira,NULL);
  pthread_mutex_init(&mutex_sala,NULL);

  srand(time(NULL));   // Initialization, should only be called once.
  // cria monitor
  
  if (pthread_create (&monitor, NULL, threadMonitor, (void *) 0))
    {
      perror ("pthread_create") ;
      exit (1) ;
    }

  int i;
  // cria estudantes
  for (i=0; i<NUM_ESTUDANTES; i++) {
    if (pthread_create (&estudantes[i], NULL, threadEstudante, (void *) i))
    {
      perror ("pthread_create") ;
      exit (1) ;
    }
    int r = rand();
    usleep(r % 500 * 2); // espera de tempo que vai de 0 a 1 milisegundos para fazer com que dê tempo do monitor fazer o atendimento
  }

  // main encerra aqui
  pthread_exit (NULL) ;
}


