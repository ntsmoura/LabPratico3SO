#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t vagas_b1, vagas_b2, itens_b1, itens_b2;		// semáforos para controle de vagas e itens
sem_t buffer_1, buffer_2;			                  // mutexes para acesso ao buffer

//variáveis de controle da ocupação dos buffers
int qtd_itens_b1, qtd_vagas_b1;                 
int qtd_itens_b2, qtd_vagas_b2;

//Função de espera randômica entre 0 e n+1 segundps
void delayRandom(int n){
  
  sleep(random() % n);
  usleep(random() % 1000000);

}

//Função das threads do tipo X
void *x(void *id){

  long tid = (long)id;
  printf("X%02ld: Olá!\n", tid);

  while(true){
    sem_wait(&vagas_b1);                                            //Aguarda liberação de acesso a vagas do Buffer 1
    sem_wait(&buffer_1);                                            //Aguarda liberação de acesso ao Buffer 1
    qtd_itens_b1++;                                                 //Insere item no Buffer 1
    qtd_vagas_b1--;                                                 //Remove vaga do Buffer 1
    printf("X%02ld: inseriu item em B1 (%d itens, %d vagas)!\n",
            tid, qtd_itens_b1, qtd_vagas_b1);
    sem_post(&buffer_1);                                            //Libera acesso ao Buffer 1
    sem_post(&itens_b1);                                            //Libera acesso aos itens do Buffer 1
    delayRandom(2);                                                 //Delay de tempo aleatório
  }

}

//Função das threads do tipo Y
void *y(void *id){

  long tid = (long)id;
  printf("Y%02ld: Olá!\n", tid);

  while(true){
    sem_wait(&itens_b1);                                            //Aguarda liberação de acesso aos itens do Buffer 1
    sem_wait(&buffer_1);                                            //Aguarda liberação de acesso ao Buffer 1
    qtd_itens_b1--;                                                 //Remove item do Buffer 1
    qtd_vagas_b1++;                                                 //Libera vaga no Buffer 1
    printf("Y%02ld: retirou item de B1 (%d itens, %d vagas)!\n",
            tid, qtd_itens_b1, qtd_vagas_b1);
    sem_post(&buffer_1);                                            //Libera acesso ao Buffer 1
    sem_post(&vagas_b1);                                            //Libera acesso às vagas do Buffer 1
    sem_wait(&vagas_b2);                                            //Aguarda liberação de acesso às vagas do Buffer 2
    sem_wait(&buffer_2);                                            //Aguarda liberação de acesso ao Buffer 2
    qtd_itens_b2++;                                                 //Insere item no Buffer 2
    qtd_vagas_b2--;                                                 //Remove vaga do Buffer 2
    printf("Y%02ld: inseriu item em B2 (%d itens, %d vagas)!\n",
            tid, qtd_itens_b2, qtd_vagas_b2);
    sem_post(&buffer_2);                                            //Libera acesso ao Buffer 2
    sem_post(&itens_b2);                                            //Libera acesso aos itens do Buffer 2
    delayRandom(2);                                                 //Delay de tempo aleatório
  }

}

//Função das threads do tipo Z
void *z(void *id){

  long tid = (long)id;
  printf("Z%02ld: Olá!\n", tid);

  while (true){
    sem_wait(&itens_b2);                                            //Aguarda liberação de acesso aos itens do Buffer 2
    sem_wait(&buffer_2);                                            //Aguarda liberação de acesso ao Buffer 2
    qtd_itens_b2--;                                                 //Remove item do Buffer 2
    qtd_vagas_b2++;                                                 //Libera vaga no Buffer 2
    printf("Z%02ld: retirou item de B2 (%d itens, %d vagas)!\n",
            tid, qtd_itens_b2, qtd_vagas_b2);
    sem_post(&buffer_2);                                            //Libera acesso ao Buffer 2
    sem_post(&vagas_b2);                                            //Libera acesso às vagas do Buffer 2
    delayRandom(2);                                                 //Delay de tempo aleatório
  }

}

//Main
int main(int argc, char *argv[]){ //5 argumentos: qtd x, qtd y, qtd z, vagas b1, vagas b2

  //Variáveis de controle para criação e gerenciamento de threads
  int qtd_threads_x = atoi(argv[1]);
  int qtd_threads_y = atoi(argv[2]);
  int qtd_threads_z = atoi(argv[3]);
  qtd_vagas_b1 = atoi(argv[4]);
  qtd_vagas_b2 = atoi(argv[5]);
  qtd_itens_b1 = 0
  qtd_itens_b2 = 0

  //Array de threads
  pthread_t threads_x[qtd_threads_x];
  pthread_t threads_y[qtd_threads_y];
  pthread_t threads_z[qtd_threads_z];

  //Variável de controle de loop para criação de threads
  long i;

  //Inicialização dos semáforos do Buffer 1
  sem_init(&buffer_1, 0, 1) ;
  sem_init(&vagas_b1,  0, qtd_vagas_b1) ;
  sem_init(&itens_b1,  0, 0) ;

  //Inicialização dos semáforos do Buffer 2
  sem_init(&buffer_2, 0, 1) ;
  sem_init(&vagas_b2,  0, qtd_vagas_b2) ;
  sem_init(&itens_b2,  0, 0) ;

  //Cria Threads X
  for(i=0; i<qtd_threads_x; i++){
    if(pthread_create(&threads_x[i], NULL, x, (void *)i)){
      perror("pthread_create") ;
      exit(1) ;
    }
  }

  //Cria Threads Y
  for(i=0; i<qtd_threads_y; i++){
    if(pthread_create(&threads_y[i], NULL, y, (void *)i)){
      perror("pthread_create") ;
      exit(1) ;
    }
  }

  //Cria Threads Z
  for(i=0; i<qtd_threads_z; i++){
    if(pthread_create(&threads_z[i], NULL, z, (void *)i)){
      perror("pthread_create") ;
      exit(1) ;
    }
  }

  pthread_exit(NULL) ;

}