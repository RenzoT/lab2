#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_CLIENTS 12 
#define NUM_SEATS 5
#define NUM_BARBERS 1

sem_t sem_seats, write_panel, read_panel;
sem_t sem_barber_chair[NUM_BARBERS], sem_cut[NUM_BARBERS], sem_client_seat[NUM_BARBERS];
int panel;

void* f_barbero(void *v){
  int b_id =*(int*) v;
  while(1){
    sem_wait(&read_panel);
    panel = b_id;
    sem_post(&read_panel);
    sem_wait(&sem_client_seat[b_id]);
    printf("barber done haircut\n"); 
    sem_post(&sem_cut[b_id]);}
  return NULL;

}

void* f_client(void* v){
  int id=*(int*) v;

  if (sem_trywait(&sem_seats) == 0 ){

    printf("client id %d, enters barbershop\n",id);
    sem_post(&sem_seats);
    sem_wait(&read_panel);

    if(id == panel)
      printf("client id %d, at barber chair\n",id);

    sem_post(&write_panel);
    sem_wait(&sem_barber_chair[id]);
    sem_post(&sem_client_seat[id]);

    sem_wait(&sem_cut[id]);
    sem_post(&sem_barber_chair[id]);

    printf("client id %d, done\n",id);

  } else
    printf("client id %d, exits barbershop\n",id);

  return NULL;
}


int main() {
    pthread_t thr_client[NUM_CLIENTS];
    pthread_t thr_barbero[NUM_BARBERS];
    int i;
  	int id[NUM_CLIENTS] = {0,1,2,3,4,5,6,7,8,9,10,11};
    int b_id[NUM_BARBERS] = {0};

    for (i = 0; i < NUM_BARBERS; i++){
      sem_init(&sem_barber_chair[i], 0, 0);
      sem_init(&sem_client_seat[i], 0, 0);
      sem_init(&sem_cut[i], 0, 0);
    }

    sem_init(&sem_seats, 0, NUM_SEATS);
    sem_init(&write_panel, 0, 1);
    sem_init(&read_panel, 0, 0);
    
    //Barberos
    for (i = 0; i < NUM_BARBERS; i++) 
  	  pthread_create(&thr_barbero[0], NULL, f_barbero, (void*)&b_id[i]);

    //Clientes
    for (i = 0; i < NUM_CLIENTS; i++) 
      pthread_create(&thr_client[i], NULL, f_client, (void*)&id[i]);

    for (i = 0; i < NUM_CLIENTS; i++) 
      pthread_join(thr_client[i], NULL);

    for (i = 0; i < NUM_BARBERS; i++) 
      pthread_join(thr_barbero[i], NULL);

    return 0;
}

