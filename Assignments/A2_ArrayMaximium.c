#include <stdio.h>
#include <string.h>
#include "mpi.h"


int main(int argc , char * argv[])
{
    int i, j;

	int my_rank;		/* rank of process	*/
	int p;			/* number of process	*/
	int source;		/* rank of sender	*/
	int dest;		/* rank of reciever	*/
	int tag = 0;		/* tag for messages	*/
	MPI_Status status;	/* return status for 	*/
				/* recieve		*/

	/* Start up MPI */
	MPI_Init( &argc , &argv );

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

//    printf("Hello world From process %d\n", my_rank);
	if( my_rank == 0 )
	{
	    printf("Enter the array size:\n");
	    int myArraySize;
	    scanf("%d", &myArraySize);

	    printf("\nEnter the array elements:\n");
		int* myArray = malloc( myArraySize * sizeof(int) ) ;
		for(i=0 ; i<myArraySize; i++)
            scanf( "%d", &myArray[i]);

        printf("\n");
//        int actualP = (myArraySize>=p) ? (p-1) : myArraySize;
		int smallSize = myArraySize / (p-1);
		int count = 0;

		for( i=1 ; i<=(p-1) ; i++ ){

            dest = i;
            int sz = smallSize;
            if(i <= (myArraySize % (p-1)) )
                sz ++;

//            printf("sz %d\n", sz);

			MPI_Send( &sz, 1, MPI_INT, dest, tag, MPI_COMM_WORLD );
			MPI_Send( myArray+count, sz, MPI_INT, dest, tag, MPI_COMM_WORLD );
            count += sz;
		}

        int maxIndex = 0;
        int masterMax = 0;
        for(i=1 ; i<=(p-1) ; i++){
            int slaveMax, slaveIndex;
            source = i;
            MPI_Recv( &slaveMax, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status );
            MPI_Recv( &slaveIndex, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status );

            if( i == 1 ){
                masterMax = slaveMax;
                maxIndex = (smallSize*(source-1)) + slaveIndex;

                if( i <= (myArraySize % (p-1)) ) maxIndex += (i-1);
                else                               maxIndex += (myArraySize % (p-1));
            }
            else if( slaveMax > masterMax ){
                masterMax = slaveMax;
                maxIndex = (smallSize*(source-1))+slaveIndex;

                if( i <= (myArraySize % (p-1)) )   maxIndex += (i-1);
                else                               maxIndex += (myArraySize % (p-1));
            }
        }
        printf("\nMaster Max = %d with index %d\n", masterMax, maxIndex);
	}
	else
	{
	    source = 0;
	    int smallSize = 0;
        MPI_Recv(&smallSize, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status );

        int* smallArray = malloc( smallSize*sizeof(int) );
        MPI_Recv(smallArray, smallSize, MPI_INT, source, tag, MPI_COMM_WORLD, &status );

        int maxNum = smallArray[0];
        int index = 0;

        for( i = 1; i < smallSize; i++){
            if(smallArray[i] > maxNum){
                maxNum = smallArray[i];
                index = i;
            }
        }

        printf("Max from SLAVE %d is %d and its index is %d\n", my_rank, maxNum, index);
        MPI_Send( &maxNum, 1, MPI_INT, 0, tag, MPI_COMM_WORLD );
        MPI_Send( &index, 1, MPI_INT, 0, tag, MPI_COMM_WORLD );
	}

	/* shutdown MPI */
	MPI_Finalize();
	return 0;
}
