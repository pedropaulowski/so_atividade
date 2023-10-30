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
sem_t S[N]; // array de semáforos

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
  sem_init(&mutex, 0, 1); // inicializa com 1 indicando que o recurso está disponível
  for (i = 0; i < N; i++)
    sem_init(&S[i], 0, 0); // inicializa com 0, pois estão indisponíveis
                           // (garantir que todos os filósofos iniciem pensando)

  // cria uma thread para cada filosofo chamando a função filosofo
  for (i = 0; i < N; i++) {
    pthread_create(&thread_id[i], NULL, filosofo, &num_filosofo[i]);
    printf("Filosofo %d esta pensando\n", i + 1);
  }

  for (i = 0; i < N; i++) {
    pthread_join(
        thread_id[i],
        NULL); // esperar que todas threads terminem para então finalizar o
               // programa e evitar finalização abrupta das threads (mera
               // formalidade, já que o programa é "infinito")
  }
}

// ciclo de vida de um filosofo
void *filosofo(void *num) {
  while (1) { // infinito
    int *i = num;
    sleep(1); // simula o tempo do filosofo pensando
    pegar_garfo(*i);
    sleep(0); // faz a thread desisitr voluntariamente de sua fatia de tempo de
              // CPU (assim outras threads podem ser agendadas para execução).
    // Se não tiver outras threads prontas para serem executadas, então a
    // execução dessa continua normalmente

    colocar_garfo(*i);
  }
}

void pegar_garfo(int num_filosofo) {
  sem_wait(&mutex); // espera pelo semaforo mutex para garantir exclusão mutua
                    // (apenas essa thread pode acessar o recurso até que ele
                    // seja liberado)
  estado[num_filosofo] = FAMINTO;
  printf("Filosofo %d esta com fome\n", num_filosofo + 1);
  testar(num_filosofo);
  sem_post(&mutex); // libera o recurso mutex para outras threads (permite que
                    // as outras threads tentem acessar o recurso))
  sem_wait(&S[num_filosofo]); // espera até que seja seguro comer
  sleep(1);
}

void testar(int num_filosofo) {
  if (estado[num_filosofo] == FAMINTO && estado[ESQUERDA] != COMENDO &&
      estado[DIREITA] != COMENDO) {
    estado[num_filosofo] = COMENDO;

    sleep(2); // simula o tempo que o filosofo está comendo
    printf("Filosofo %d pega garfos %d e %d\n", num_filosofo + 1, ESQUERDA + 1,
           num_filosofo + 1);
    printf("Filosofo %d esta comendo\n", num_filosofo + 1);
    sem_post(&S[num_filosofo]); // libera o semaforo do filosofo no array S,
                                // assim as outras threads que estão esperando
                                // possam prosseguir

  } else {
    printf("Filosofo %d não pode comer agora\n", num_filosofo + 1);
  }
}

void colocar_garfo(int num_filosofo) {
  sem_wait(&mutex); // espera pelo semaforo mutex para garantir exclusão mutua
  estado[num_filosofo] = PENSANDO;

  printf("Filosofo %d coloca garfos %d e %d para baixo\n", num_filosofo + 1,
         ESQUERDA + 1, num_filosofo + 1);

  printf("Filosofo %d esta pensando\n", num_filosofo + 1);

  // verifica se os filosofos adjacentes podem comer
  testar(ESQUERDA);
  testar(DIREITA);
  sem_post(&mutex); // perite que outro filoso pegue os garfos
}
