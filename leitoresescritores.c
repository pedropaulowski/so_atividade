#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

sem_t mutex;
sem_t db;
int contador_leitor = 0;

void *leitor(void *arg) {
  int f;
  f = ((int)arg);
  sem_wait(&mutex); // espera pelo mutex (bloqueia o acesso a região crítica))
  contador_leitor = contador_leitor + 1;

  if (contador_leitor == 1)
    sem_wait(
        &db); // o primeiro leitor bloqueia o acesso para todos os escritores

  sem_post(&mutex); // libera o mutex permitindo que os outros leitores alterem
                    // o contador de leitores

  printf("Leitor %d lê\n", f);
  sleep(1); // simulando leitura
  printf("Leitor %d para de ler\n", f);

  sem_wait(&mutex); // solicita o mutex para decrementar o numero de leitores

  contador_leitor = contador_leitor - 1;

  if (contador_leitor == 0)
    sem_post(
        &db); // o último leitor desbloqueia o acesso para todos os escritores

  sem_post(&mutex); // leitor libera o mutex para permitir que outros leitores
                    // alterem o contador
}

void *escritor(void *arg) {
  int f;
  f = ((int)arg);
  printf("Escritor %d quer escrever\n", f);
  sem_wait(
      &db); // solicita acesso ao db (bloqueia para todos leitores e escritores)
  printf("Escritor %d escreve\n", f);
  sleep(1); // simulando escrita
  printf("Escritor %d para de escrever\n", f);
  sem_post(&db); // libera o db
}

int main() {
  int i, b;
  pthread_t rtid[5],
      wtid[5]; // arrays para armazenar os IDs das threads leitoras e escritoras

  sem_init(&mutex, 0, 1);
  sem_init(&db, 0, 1);

  // cria as threads leitoras e escritoras

  for (i = 0; i <= 2; i++) {
    pthread_create(&wtid[i], NULL, escritor, (void *)i);
    pthread_create(&rtid[i], NULL, leitor, (void *)i);
    pthread_create(&rtid[i + 3], NULL, leitor, (void *)(i + 3));
    pthread_create(&wtid[i + 3], NULL, escritor, (void *)(i + 3));
    pthread_join(wtid[i], NULL);
    pthread_join(rtid[i], NULL);
    pthread_join(rtid[i + 3], NULL);
    pthread_join(wtid[i + 3], NULL);
  }
}
