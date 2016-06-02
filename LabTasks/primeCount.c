#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[]){

    int myRank;
    int p;

    int i, j;
    int no1, no2, range;
    int lower, upper;
    int localCount = 0, count = 0;  
    int flag = 0;
//    double local_fact = 1.0;
//    double result;
    double startTime, endTime;

	/* initialize MPI */
    MPI_Init(&argc,&argv);

    /* get my rank and the size of the communicator */
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    /* Input in master */
    if(myRank == 0){
		printf("Enter the start and end of the range:\n");
		scanf("%d %d",&no1, &no2);
		range = (no2-no1)/p;
		startTime = MPI_Wtime();
    }
	
    MPI_Bcast(&no1,1 ,MPI_INT, 0, MPI_COMM_WORLD); 
    MPI_Bcast(&no2,1 ,MPI_INT, 0, MPI_COMM_WORLD); 
    MPI_Bcast(&range,1 ,MPI_INT, 0, MPI_COMM_WORLD); 

	/* Calculate the upper and lower for each process */
	/* The reminder is handled in the last rank */
    lower = no1 + myRank * ( (no2-no1) / p ) ;
    if(myRank == p-1)   upper = no2+1;
    else				upper = lower + range;

    for(i = lower ; i < upper ; i++){
		flag = 0;
		
		// to Handle if the range contains 0, 1 (not primes)
		if(i==0 || i==1)
			flag = 1;
		for(j=2 ; j<i ; j++){
			if(i%j == 0) {
				flag = 1;
				break;
			}
		}
		if(flag == 0)
			localCount ++;
    }

	MPI_Reduce(&localCount, &count, 1, MPI_INT, MPI_SUM,0, MPI_COMM_WORLD); 

    if(myRank==0){
		printf("The num of primes in this range is %d\n", count);
		endTime = MPI_Wtime();	
		printf("time = %.16f seconds\n", endTime - startTime);    
    }

    /* shut down MPI */
    MPI_Finalize();

    return 0;
}
