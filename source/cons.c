#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

//SAME TO prodocuer.c

///BASIC STRUCTURE
#define NUM_THREADS 3
#define MAX_PRODUCT 20
typedef struct product_s
{
	pthread_mutex_t mutex;
	short init;
	int rear;
	int front;
	int data[20];
}product;

int negative_exponential_distribution(double lambda){
	double pV = 0.0;
    while(1)
    {
        pV = (double)rand()/(double)RAND_MAX;
        if (pV != 1)
        {
            break;
        }
    }
    pV = (-1.0/lambda)*log(1-pV);
    return (int)(pV * 1000000);
}

sem_t *sem_full;
sem_t *sem_empty;
product *p;
pthread_t tid[NUM_THREADS];

void *consumer(void *param)
{
	int lambda = *(int*)param;
	int random_time = negative_exponential_distribution(lambda);
	usleep(random_time);


    sem_wait(sem_full);


    pthread_mutex_unlock(&(p->mutex));
    printf("pid: %d, tid: %lu, random_num: %d\n", getpid(), (unsigned long)pthread_self(), p->data[p->front]);
    p->data[p->front] = 0;
    p->front = (p->front + 1) % MAX_PRODUCT;
	pthread_mutex_unlock(&(p->mutex));

    sem_post(sem_empty);


}

int main(int argc, char** argv)
{

	sem_full = sem_open("/Full", O_EXCL, 0644, 0);
    if (sem_full == SEM_FAILED) {
        fprintf(stderr, "sem_open error\n");
        exit(1);
    }


    sem_empty = sem_open("/Empty", O_EXCL, 0644, 20);
    if (sem_empty == SEM_FAILED) {
        fprintf(stderr, "sem_open error\n");
        exit(1);
    }


    int fd = shm_open("/sh_product", O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        fprintf(stderr, "shm_open error\n");
        exit(1);
    }


    p = mmap(NULL, sizeof(product), PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        fprintf(stderr, "mmap error\n");
        exit(1);
    }

    if (p->init != 1) {
        memset(p, 0, sizeof(product));
        p->init = 1;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int lambda = atoi(argv[1]);
    int loop;
    for (loop = 0; loop < 20; loop++)
	{
	    int i;
		for(i = 0; i < NUM_THREADS; i++)
	        pthread_create(&tid[i], &attr, consumer, &lambda);

	    for(i = 0; i < NUM_THREADS; i++)
	        pthread_join(tid[i], NULL);
	}

    munmap(p, sizeof(product));
    shm_unlink("/sh_product");

    sem_close(sem_full);
    sem_close(sem_empty);

    return 0;
}

//prod cons 3threads buffer contains 20 dataum
//prod random produce a data ,id for process and thread
//thread retrieve a data print id and data
//communication with shared memory, synchornization by semaphore
//proce period a parameter fu zhi shu
//the same to consumer
