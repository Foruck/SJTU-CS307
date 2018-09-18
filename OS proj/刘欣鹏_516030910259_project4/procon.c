#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 5
typedef int buffer_item;
buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t empty,full;
pthread_t pro_id[10],con_id[10];
pthread_attr_t pro_attr[10],con_attr[10];
int l,r;
int insert_item(buffer_item item){
	sem_wait(&empty);
	pthread_mutex_lock(&mutex);
	r++;
	if (r==5) r=0;
	buffer[r]=item;
	pthread_mutex_unlock(&mutex);
	sem_post(&full);
	return 0;
}
int remove_item(buffer_item item){
	int x=0;
	sem_wait(&full);
	pthread_mutex_lock(&mutex);
	x=buffer[l];
	buffer[l]=item;
	l++;
	if (l==5) l=0;
	pthread_mutex_unlock(&mutex);
	sem_post(&empty);
	return x;
}
void *producer(int *param){
	buffer_item r;
	while (1){
		r=rand()%10;
		sleep(r);
		r=rand()%10000;
		if (!insert_item(r)) printf("Producer %d produced %d successfully.\n",*param,r);
	}
}

void *consumer(int *param){
	buffer_item r,x=0;
	while (1){
		r=rand()%10;
		sleep(r);
		r=rand()%10000;
		if (x=remove_item(r)) printf("Consumer %d consumed %d with %d successfully.\n",*param,x,r);
	}
}
int main(int argc, char *argv[]){
	if (argc!=4) {
		printf("Invalid arguments!\n");
		return 0;
	}
	int x,y,z,i;
	x=atoi(argv[1]);
	y=atoi(argv[2]);
	z=atoi(argv[3]);
	if (x<=0 || y>10 || z>10 || y<=0 || z<=0) {
		printf("Too many threads!\n");
		return 0;
	}
	void *data=NULL;
	pthread_mutex_init(&mutex,NULL);
	sem_init(&empty,0,5);
	sem_init(&full,0,0);
	srand((unsigned)time(NULL));
	l=0;r=-1;
	for (i=0;i<y;i++) pthread_attr_init(&pro_attr[i]);
	for (i=0;i<z;i++) pthread_attr_init(&con_attr[i]);
	for (i=0;i<y;i++){
		int *data=(int *) malloc(sizeof(int));
		*data=i;
		pthread_create(&pro_id[i],&pro_attr[i],producer,data);
	}
	for (i=0;i<z;i++){
		int *data=(int *) malloc(sizeof(int));
		*data=i;
		pthread_create(&con_id[i],&con_attr[i],consumer,data);
	}
	sleep(x);
	return 0;
}
