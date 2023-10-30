#include <pthread.h>
#include <stdio.h>

#define NUM_LEITORES 5
#define NUM_ESCRITORES 5

pthread_rwlock_t lock;

void *leitor(void *arg) {
    int id = *((int *)arg);
    pthread_rwlock_rdlock(&lock);
    printf("Leitor %d está lendo\n", id);
    sleep(1);
    printf("Leitor %d terminou de ler\n", id);
    pthread_rwlock_unlock(&lock);
}

void *escritor(void *arg) {
    int id = *((int *)arg);
    printf("Escritor %d está esperando para escrever\n", id);
    pthread_rwlock_wrlock(&lock);
    printf("Escritor %d está escrevendo\n", id);
    sleep(1);
    pthread_rwlock_unlock(&lock);
    printf("Escritor %d terminou de escrever\n", id);
}

int main() {
    pthread_t r[NUM_LEITORES], w[NUM_ESCRITORES];
    int i, id[NUM_LEITORES + NUM_ESCRITORES];

    pthread_rwlock_init(&lock, NULL);

    for (i = 0; i < NUM_LEITORES; i++) {
        id[i] = i;
        pthread_create(&r[i], NULL, leitor, &id[i]);
    }

    for (i = 0; i < NUM_ESCRITORES; i++) {
        id[i+NUM_LEITORES] = i;
        pthread_create(&w[i], NULL, escritor, &id[i+NUM_LEITORES]);
    }

    for (i = 0; i < NUM_LEITORES; i++) {
        pthread_join(r[i], NULL);
    }

    for (i = 0; i < NUM_ESCRITORES; i++) {
        pthread_join(w[i], NULL);
    }

    pthread_rwlock_destroy(&lock);

    return 0;
}

