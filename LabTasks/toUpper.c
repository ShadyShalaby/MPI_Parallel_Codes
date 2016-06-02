#include <stdio.h>
#include "mpi.h"

/* 
	Input: a string of lower or upper case characters
	Output: the same string but lower case characters are converted to upper case characters 
	No reminder is handled in this code 
	It handles only the divisible part and the reminder is NOT converted
*/

int main (int argc ,char * argv[])
{
	int i, j;
    int sz, rem, chunkSZ=0;
    char str[20];
    char localStr[20];

	int myRank;
    int p;
    MPI_Status status;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &myRank);
    MPI_Comm_size (MPI_COMM_WORLD, &p);

    if(myRank == 0){
		printf("Enter a string\n");
        scanf("%s", str);

        sz = strlen( str );
        chunkSZ = sz / p;
        rem = sz % p;
    }
	
    // To be used as a scatter parameter with correct value
	MPI_Bcast(&chunkSZ, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
    MPI_Scatter (str, chunkSZ, MPI_CHAR, localStr, chunkSZ, MPI_CHAR, 0, MPI_COMM_WORLD);

    for(i=0 ; i<chunkSZ ; i++)
        localStr[i] = ( char ) toupper( (int) localStr[i] );
	
    MPI_Gather (localStr, chunkSZ, MPI_CHAR, str, chunkSZ, MPI_CHAR, 0, MPI_COMM_WORLD);

    if(myRank == 0)
        printf("%s\n", str);

    MPI_Finalize();
}
