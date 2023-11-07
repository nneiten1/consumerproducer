//
//
//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define BUFFER_SIZE 2

// Define shared data structure
struct SharedData {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
    sem_t empty;
    sem_t full;
    sem_t mutex;
};

void produce(struct SharedData *data, int item) {
    // Produce an item and add it to the buffer
    sem_wait(&data->empty);
    sem_wait(&data->mutex);
    
    data->buffer[data->in] = item;
    data->in = (data->in + 1) % BUFFER_SIZE;

    sem_post(&data->mutex);
    sem_post(&data->full);
}

void *producer_thread(void *arg) {
    struct SharedData *data = (struct SharedData *)arg;

    for (int i = 0; i < 5; i++) {
        int item = i + 1;
        produce(data, item);
        printf("Produced: %d\n", item);
    }

    return NULL;
}

int main() {
    // Create and initialize shared data
    int fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(struct SharedData));
    struct SharedData *data = (struct SharedData *)mmap(NULL, sizeof(struct SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    data->in = 0;
    data->out = 0;
    sem_init(&data->empty, 1, BUFFER_SIZE);
    sem_init(&data->full, 1, 0);
    sem_init(&data->mutex, 1, 1);

    // Create producer thread
    pthread_t producer_tid;
    pthread_create(&producer_tid, NULL, producer_thread, data);

    // Wait for the producer thread to finish
    pthread_join(producer_tid, NULL);

    // Clean up and close shared memory
    sem_destroy(&data->empty);
    sem_destroy(&data->full);
    sem_destroy(&data->mutex);
    munmap(data, sizeof(struct SharedData));
    shm_unlink("/shared_memory");

    return 0;
}