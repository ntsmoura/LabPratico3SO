#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define MAX 4

int vazios = 0; // Escrita por Joao e Pedro / Lida por Pedro
int com_semente = 0; // Escrita por Pedro e Paulo / Lida por Paulo
int buracos = 0; // Escrita por Joao e Paulo / Lida por Joao
bool pa_ocupada = false; // Escrita por Joao e Paulo / Lida por Joao e Paulo

sem_t s_vazios, s_com_semente, s_buracos, s_pa_ocupada;
	
void espera_aleatoria (int n)
{
  sleep (random() % n) ;	// pausa entre 0 e n segundos (inteiro)
  usleep (random() % 1000000) ;	// pausa entre 0 e 1 segundo (float)
}


void *joao(void *arg) {
    while(true) 
    {
        if(buracos < MAX && !pa_ocupada) 
        {
            sem_wait(&s_pa_ocupada);            
            pa_ocupada = true;

            printf("João está com a pá e está cavando...\n");
            //cava buraco
            espera_aleatoria(1);
          
            pa_ocupada = false;
            sem_post(&s_pa_ocupada);  
            printf("João terminou de cavar.\n");
            

            sem_wait(&s_vazios);
            vazios++;
            sem_post(&s_vazios);

            sem_wait(&s_buracos);
            buracos++;
            sem_post(&s_buracos);             
        }
    }
}

void *pedro(void *arg) 
{
    while(true) 
    {
        if(vazios > 0)
        {
            //semeia
            printf("Pedro está colocando as sementes...\n");
            espera_aleatoria(1);
            printf("Pedro colocou as sementes.\n");
            sem_wait(&s_vazios);
            vazios--;
            sem_post(&s_vazios);

            sem_wait(&s_com_semente);
            com_semente++;
            sem_post(&s_com_semente);
        }        
    }
}

void *paulo(void *arg) 
{
    while(true) 
    {
        if(com_semente > 0 && !pa_ocupada)
        {
            sem_wait(&s_pa_ocupada);
            pa_ocupada = true;

            printf("Paulo está com a pá fechando o buraco...\n");
            //fecha buraco
            espera_aleatoria(1);

            pa_ocupada = false;
            sem_post(&s_pa_ocupada);
            printf("Paulo fechou o buraco...\n");
             

            sem_wait(&s_com_semente);
            com_semente--;
            sem_post(&s_com_semente); 

            sem_wait(&s_buracos);
            buracos--;
            sem_post(&s_buracos);              
        }
    }
}


int main (int argc, char *argv[])
{
    pthread_t joao_cava, pedro_semente, paulo_fecha; 
    sem_init(&s_vazios, 0, 1);
    sem_init(&s_com_semente, 0, 1);
    sem_init(&s_buracos, 0, 1);
    sem_init(&s_pa_ocupada, 0, 1);

    if(pthread_create(&joao_cava, NULL, joao, (void *) 0))
    {
      perror("pthread_create");
      exit(1);
    }

    if(pthread_create(&pedro_semente, NULL, pedro, (void *) 1))
    {
      perror("pthread_create");
      exit(1);
    }

    if(pthread_create(&paulo_fecha, NULL, paulo, (void *) 2))
    {
      perror("pthread_create");
      exit(1);
    }

    pthread_exit (NULL) ;
}