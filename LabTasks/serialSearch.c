#include <stdio.h>
#include "mpi.h"

int f(int i){
	int i_huge = 2147483647;
	int j, k;
	for(j=0 ; j<5 ; j++){
		k = i/12773;
		if(i > 0)
			i = 16807 * (i - k*12773) - k*2836;
		else 
			i = i + i_huge;
	}
	
	return i;
}

int main(int argc, char *argv[]){
 
	int myRank;
    int size;
    int no1;
    int lower,upper;
    int i, j, c;
    double local_fact = 1.0;
    double result;
    double startTime, endTime;
    int no2;    
    int range;
    int localCount = 0, count = 0;  
    int flag = 0;  
      /* initialize MPI */
    MPI_Init(&argc,&argv);
    /* get my rank and the size of the communicator */
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Input.  */
    if(myRank==0){
		printf("Enter the start and end of the range and value of c:\n");
		scanf("%d %d %d",&no1, &no2, &c);
		startTime = MPI_Wtime();
		range = (no2-no1)/size;
    }
	
    MPI_Bcast(&no1,1 ,MPI_INT, 0, MPI_COMM_WORLD); 
    MPI_Bcast(&no2,1 ,MPI_INT, 0, MPI_COMM_WORLD); 
    MPI_Bcast(&c,1 ,MPI_INT, 0, MPI_COMM_WORLD); 
    MPI_Bcast(&range,1 ,MPI_INT, 0, MPI_COMM_WORLD); 

	/* calculate the upper and lower for each process */
    lower = no1 + myRank * ((no2-no1) / size) ;
    if(myRank == size-1) 	upper = no2+1;
    else					upper = lower + range;

    for(i = lower ; i < upper ; i++){
		if( f(i) == c ){
			printf("i = %d\n", i);
		}
    }
 
    if(myRank==0){
		endTime = MPI_Wtime();	
		printf("time = %.16f seconds\n", endTime - startTime);    
    }

    /* shut down MPI */
    MPI_Finalize();

    return 0;
}
