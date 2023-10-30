#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#define N 5
#define PENSANDO 0
#define FAMINTO 1
#define COMENDO 2
#define ESQUERDA (num_filosofo + 4) % N
#define DIREITA (num_filosofo + 1) % N

sem_t mutex;
sem_t S[N];

void *filosofo(void *num);
void pegar_garfo(int);
void colocar_garfo(int);
void testar(int);

int estado[N];
int num_filosofo[N] = {0, 1, 2, 3, 4};

int main() {
  int i;
  pthread_t thread_id[N];

  // inicializa os semáforos
  sem_init(&mutex, 0, 1);
  for (i = 0; i < N; i++)
    sem_init(&S[i], 0, 0);

  // cria as threads dos filósofos
  for (i = 0; i < N; i++) {
    pthread_create(&thread_id[i], NULL, filosofo, &num_filosofo[i]);
    printf("Filosofo %d esta pensando\n", i + 1);
  }

  for (i = 0; i < N; i++)
    pthread_join(thread_id[i], NULL);
}

void *filosofo(void *num) {
  while (1) {
    int *i = num;
    sleep(1);
    pegar_garfo(*i);
    sleep(0);
    colocar_garfo(*i);
  }
}

void pegar_garfo(int num_filosofo) {
  sem_wait(&mutex);
  estado[num_filosofo] = FAMINTO;
  printf("Filosofo %d esta com fome\n", num_filosofo + 1);
  testar(num_filosofo);
  sem_post(&mutex);
  sem_wait(&S[num_filosofo]);
  sleep(1);
}

void testar(int num_filosofo) {
  if (estado[num_filosofo] == FAMINTO && estado[ESQUERDA] != COMENDO &&
      estado[DIREITA] != COMENDO) {
    estado[num_filosofo] = COMENDO;
    sleep(2);
    printf("Filosofo %d pega garfos %d e %d\n", num_filosofo + 1, ESQUERDA + 1,
           num_filosofo + 1);
    printf("Filosofo %d esta comendo\n", num_filosofo + 1);
    sem_post(&S[num_filosofo]);
  } else {
    printf("Filosofo %d não pode comer agora\n", num_filosofo + 1);
  }
}

void colocar_garfo(int num_filosofo) {
  sem_wait(&mutex);
  estado[num_filosofo] = PENSANDO;
  printf("Filosofo %d coloca garfos %d e %d para baixo\n", num_filosofo + 1,
         ESQUERDA + 1, num_filosofo + 1);
  printf("Filosofo %d esta pensando\n", num_filosofo + 1);
  testar(ESQUERDA);
  testar(DIREITA);
  sem_post(&mutex);
}