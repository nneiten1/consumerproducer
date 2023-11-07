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

int consume(struct SharedData *data) {
    // Consume an item from the buffer
    sem_wait(&data->full);
    sem_wait(&data->mutex);

    int item = data->buffer[data->out];
    data->out = (data->out + 1) % BUFFER_SIZE;

    sem_post(&data->mutex);
    sem_post(&data->empty);

    return item;
}

void *consumer_thread(void *arg) {
    struct SharedData *data = (struct SharedData *)arg;

    for (int i = 0; i < 5; i++) {
        int item = consume(data);
        printf("Consumed: %d\n", item);
    }

    return NULL;
}

int main() {
    // Open and initialize shared data
    int fd = shm_open("/shared_memory", O_RDWR, 0666);
    struct SharedData *data = (struct SharedData *)mmap(NULL, sizeof(struct SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Create consumer thread
    pthread_t consumer_tid;
    pthread_create(&consumer_tid, NULL, consumer_thread, data);

    // Wait for the consumer thread to finish
    pthread_join(consumer_tid, NULL);

    // Clean up and close shared memory
    munmap(data, sizeof(struct SharedData));

    return 0;
}