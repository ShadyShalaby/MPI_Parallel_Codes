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

//    printf("Hello world From process %d\n", my_rank);
	if( my_rank == 0 )
	{
	    printf("Enter the number: ");
        scanf("%d", &n);

        t1 = MPI_Wtime();

		for( i=1 ; i<=(p-1) ; i++ ){
            dest = i;
            r = n/(p-1);

			MPI_Send( &r, 1, MPI_INT, dest, tag, MPI_COMM_WORLD );
			MPI_Send( &n, 1, MPI_INT, dest, tag, MPI_COMM_WORLD );
		}

        for(i=1 ; i<=(p-1) ; i++){
            source = i;
            MPI_Recv( &localFact, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status );

            fact *= localFact;
        }
        t2 = MPI_Wtime();
        printf("\nFactorial of %d is %d\n", n, fact);
        printf("\nElapsed Time = %f\n", t2-t1);
	}
	else
	{
	    source = 0;
        MPI_Recv(&r, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status );
        MPI_Recv(&n, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status );

        a = (my_rank-1)*r + 1;
        b = (my_rank)*r;

        if( my_rank <= (n % (p-1)) )   a += (my_rank-1), b += (my_rank);
        else                           a += (n % (p-1)), b+=(n % (p-1));

        for( i = a ; i <=b ; i++)
            localFact *= i;

        printf("P%d: partial factorial from %d to %d = %d\n", my_rank, a, b, localFact);
        MPI_Send( &localFact, 1, MPI_INT, source, tag, MPI_COMM_WORLD );
	}

	/* shutdown MPI */
	MPI_Finalize();
	return 0;
}
