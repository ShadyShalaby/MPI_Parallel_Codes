#include <stdio.h>
#include <string.h>
#include "mpi.h"


int main(int argc , char * argv[])
{
    int i, j;
    int n, r;
    int a, b;
    int localFact = 1;
    int fact = 1;

    double t1, t2;

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

	if( my_rank == 0 )
	    printf("Enter the number: "),
        scanf("%d", &n),
        t1 = MPI_Wtime();


    r = n/p;
    MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    a = (my_rank)*r + 1;
    b = (my_rank+1)*r;

    if( my_rank < (n % p) )   a += (my_rank), b += (my_rank+1);
    else                      a += (n % p), b+=(n % p);

    for( i = a ; i <=b ; i++)
        localFact *= i;

    printf("P%d: Partial factorial from %d to %d = %d\n", my_rank, a, b, localFact);

    MPI_Reduce(&localFact, &fact, 1, MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);

    if(my_rank == 0)
        t2 = MPI_Wtime(),
        printf("\nFactorial of %d is %d\n", n, fact),
        printf("\nElapsed time = %f\n", t2-t1) ;

	/* shutdown MPI */
	MPI_Finalize();
	return 0;
}
