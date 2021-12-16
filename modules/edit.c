#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LENGTH 31

int min_three(int a,int b, int c){
	int min = a;
	
	if(min > b){
		min = b;
	}

	if(min > c){
		min = c;
	}

	return min;
}

void swap(int **a, int **b){
	int *temp = *a;
	*a = *b;
	*b = temp;
}

int edit_distance(char *a,int na,char *b,int nb) {
    int oo=0x7FFFFFFF;

	static int T[2][MAX_WORD_LENGTH+1];

	int ia, ib;

	int cur=0;
	ia=0;

	for(ib=0;ib<=nb;ib++)
		T[cur][ib]=ib;

	cur=1-cur;

	for(ia=1;ia<=na;ia++)
	{

		T[cur][0]=ia;


		for(ib=1;ib<=nb;ib++)
		{
			int ret=oo;

			int d1=T[1-cur][ib]+1;
			int d2=T[cur][ib-1]+1;
			int d3=T[1-cur][ib-1]; if(a[ia-1]!=b[ib-1]) d3++;

			if(d1<ret) ret=d1;
			if(d2<ret) ret=d2;
			if(d3<ret) ret=d3;

			T[cur][ib]=ret;
		}

		cur=1-cur;
	}

	int ret=T[1-cur][nb];

	return ret;
}



int main(int argc,char *argv[]){
	clock_t start, end;
	start = clock();
	printf("Edit distance: %d\n",edit_distance(argv[1],strlen(argv[1]),argv[2],strlen(argv[2])));
	end = clock();
	double time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time: %f\n",time_used);

}