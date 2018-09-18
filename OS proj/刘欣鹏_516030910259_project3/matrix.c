#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
int n,k,m;
int a[50][50],b[50][50],c[50][50];
struct node{
	int x;
	int y;
};
void *runner(struct node * param){
	c[param->x][param->y]=0;
	int i;
	for (i=0;i<k;i++) c[param->x][param->y]+=a[param->x][i]*b[i][param->y];
	printf("%d outputs %d,%d=%d\n",pthread_self(),param->x,param->y,c[param->x][param->y]);
	pthread_exit(0);
}
int main()
{
	int i,j,x;
	printf("M,K,N=");
	scanf("%d%d%d",&n,&k,&m);
	printf("A:\n");
	for (i=0;i<n;i++)
		for (j=0;j<k;j++) 
			scanf("%d",&a[i][j]);
	printf("B:\n");
	for (i=0;i<k;i++)
		for (j=0;j<m;j++)
			scanf("%d",&b[i][j]);
	
	pthread_t tid[2501];
	pthread_attr_t attr[2501];
	
	for (i=0;i<n;i++)
		for (j=0;j<m;j++){
			x=i*m+j;
			pthread_attr_init(&attr[x]);
			struct node *data=(struct node *) malloc(sizeof(struct node));
			data->x=i;
			data->y=j;
			pthread_create(&tid[x],&attr[x],runner,data);
		}
	for (i=0;i<m*n;i++)
		pthread_join(tid[i],NULL);
	for (i=0;i<n;i++)
	{
		for (j=0;j<m;j++) printf("%d ", c[i][j]);
		printf("\n");
	}	
}


