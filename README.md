# consumerproducer

Producer-Consumer Solution

This project provides a solution to the producer-consumer problem using C++ in a Linux environment. 
It includes two programs; a producer and consumer. Both use threads, semaphores, and shared memory.

'producer.c' this program creates a producer thread that generates items and adds them to a shared buffer
'consumer.c' this program creates a consumer thread that retrieves items from the shared buffer & consumes them

Compile and run the programs with the following commands:

```wsl
$ gcc producer.c -pthread -lrt -o producer
$ gcc consumer.c -pthread -lrt -o consumer
$ ./producer & ./consumer &
