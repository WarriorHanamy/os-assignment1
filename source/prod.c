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

/// BASIC STRUCTURE
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


int negative_exponential_distribution(double lambda)
{
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

void *producer(void *param)
{
	int lambda = *(int*)param;
	int random_time = negative_exponential_distribution(lambda);
	usleep(random_time);



	int random_data = rand() % 100 + 1;

///CRTICAL REGION///
	sem_wait(sem_empty);
	pthread_mutex_lock(&(p->mutex));
    p->data[p->rear] = random_data;
    printf("pid: %d, tid: %lu, random_num: %d\n", getpid(), (unsigned long)pthread_self(), p->data[p->rear]);
	p->rear = (p->rear + 1) % MAX_PRODUCT;
	pthread_mutex_unlock(&(p->mutex));
    sem_post(sem_full);

}

int main(int argc, char** argv)
{

    sem_full = sem_open("/Full", O_CREAT, 0644, 0);


    sem_empty = sem_open("/Empty", O_CREAT, 0644, 20);

    ///SHARED MEMORY CREATE
    int fd = shm_open("/sh_product", O_CREAT | O_RDWR, 0644);


    ftruncate(fd, sizeof(product));

    //MAP FOR SHARED MOREMORY
    p = mmap(0, sizeof(product), PROT_WRITE, MAP_SHARED, fd, 0);
    p->init = 0;


    if (p->init != 1) {
        memset(p, 0, sizeof(product));
        p->init = 1;
    }

    pthread_mutex_init(&(p->mutex), NULL);


    pthread_attr_t attr;
    pthread_attr_init(&attr);

    int lambda = atoi(argv[1]);

    for (int loop = 0; loop < 20; loop++)
	{
	    int i;
		for(i = 0; i < NUM_THREADS; i++)
	        pthread_create(&tid[i], &attr, producer, &lambda);

	    for(i = 0; i < NUM_THREADS; i++)
	        pthread_join(tid[i], NULL);
	}


    munmap(p, sizeof(product));
    //UNLINK SHARED MOREMORY
    shm_unlink("/sh_product");

    sem_close(sem_full);
    sem_close(sem_empty);

    return 0;
}
