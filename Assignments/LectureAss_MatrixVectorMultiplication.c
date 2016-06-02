/*
    This parallel code to solve Traffic Flow Problem
    The problem is to determine the amount of traffic between
    each of the four intersections(A,B,C,D).

    This is an example of the equations:
    Intersection A: x4 + 610 = x1 + 450
    Intersection B: x1 + 400 = x2 + 640
    Intersection C: x2 + 600 = x3
    Intersection D: x3 = x4 + 520

    Link1: http://www.math.tamu.edu/~yvorobet/MATH304-504/Lect1-03web.pdf
    Link2: https://www.math.ucdavis.edu/~daddel/linear_algebra_appl/Applications/trafic_flow/trafic_flow.html
*/

#include <stdio.h>
#include <stdbool.h>
#include "mpi.h"

int i, j, k;
int r1, c1;
int r2, c2;
double* mtx1, * mtx2;
double* subMTX1;
double* subMTX2;
double* subResult;
double* finalResult;

double* mergedMTX;

int error = 0;
FILE* iFile;

bool resultAllocation();

bool matrixAllocation () {
    mtx1 = malloc((r1*c1) * sizeof(double));
    mtx2 = malloc((r2*c2) * sizeof(double));
    mergedMTX = malloc((r1*(c1+1)) * sizeof(double));
    return resultAllocation ();
}

char* readFileName () {
    char* fileName;
    printf("Enter File name: ");

    fgets(fileName, 25, stdin);
    scanf(" %[^\n]", fileName);

    return fileName;
}

bool readDimensionsFromFile (){
    char* fileName = readFileName();
    iFile = fopen(fileName, "r");

    if (iFile == NULL) {
        printf("The file can NOT be opened !!!\n");
        return false;
    }

    fscanf(iFile, "%d %d", &r1, &c1);
    fscanf(iFile, "%d %d", &r2, &c2);
    return true;
}

void readDimensionsFromConsole () {
    printf("Enter the dimensions of the coefficient matrix\n");
    scanf( "%d %d", &r1, &c1);

    printf("Enter dimensions of the vector\n");
    scanf("%d %d", &r2, &c2);
}

void readMatricesFromFile () {

    for(i = 0; i < (r1*c1); i++)
        fscanf(iFile, "%lf", &mtx1[i]);

    for(i = 0; i < (r2*c2); i++)
        fscanf(iFile, "%lf", &mtx2[i]);
}

void readMatricesFromConsole () {

    printf("Enter the matrix\n");
    for(i = 0; i < (r1*c1); i++)
        scanf("%lf", &mtx1[i]);

    printf("Enter the vector\n");
    for(i = 0; i < (r2*c2); i++)
        scanf("%lf", &mtx2[i]);
}

bool resultAllocation () {

    if (c1 != r2){
        printf("INVALID dimensions!!!\n");
        printf("Matrix operation can NOT be performed");
        return false;
    }
    else {
        finalResult = malloc((r1*c2) * sizeof(double));
        return true;
    }
}

void mergeMatrices(){

    int index1=0, index2=0;
    for(i=0; i<(r1*(c1+1)); i++) {
        if (i%(c1+1) == c1){
            mergedMTX[i] = mtx2[index2++];
        } else {
            mergedMTX[i] = mtx1[index1++];
        }
    }

}

void processWork (int my_rank, int p){

    MPI_Bcast(&r1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&c1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r2, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&c2, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(my_rank != 0)
        matrixAllocation ();

    subMTX1 = malloc((r1/p)*(c1+1) * sizeof(double));
    MPI_Scatter(mergedMTX, (r1/p)*(c1+1), MPI_DOUBLE, subMTX1, (r1/p)*(c1+1), MPI_DOUBLE, 0, MPI_COMM_WORLD);

    subMTX2 = malloc((r1/p) * sizeof(double));
    for(i=0 ; i<(r1/p) ; i++)
        subMTX2[i] = subMTX1[ i*(c1+1) + c1 ];
    MPI_Allgather(subMTX2, (r1/p), MPI_DOUBLE, mtx2, (r1/p), MPI_DOUBLE, MPI_COMM_WORLD);


    subResult = malloc((r1/p)*c2 * sizeof(double));
    // C2 here = 1

    for(i=0 ; i<(r1/p)*c1 ; i+=c1){

        // Second loop is removed because always C2 = 1

            double sum = 0;
            for(k=0 ; k<c1 ; k++)
                sum += subMTX1[ i+k +((i+k)/c1) ] * mtx2[ k ];
//                printf("Rank %d, %d, index %d %d * %d\n", my_rank, i+k, i+k +((i+k)/c1), subMTX1[ i+k +((i+k)/c1) ] , mtx2[ k ]);

            subResult[ i/c1 ] = sum;
    }

    MPI_Gather(subResult, (r1/p)*c2, MPI_DOUBLE, finalResult, (r1/p)*c2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void viewResult () {

    printf("\nThe Vector of the unknowns ( %d , %d ) is\n", r1, c2);

    for(i=0 ; i<r1*c2 ; i++)
        i%c2==0 ?
            printf("\n%lf ", finalResult[i]) :
            printf("%lf ", finalResult[i]);

    printf("\n");
}

void writeResultOnFile () {

    viewResult ();

    FILE* oFile = fopen("out.txt", "w");
    fprintf(oFile, "The vector of the unknowns ( %d , %d ) is\n", r1, c2);

    for(i=0 ; i<r1*c2 ; i++)
        i%c2==0 ?
            fprintf(oFile, "\n%lf ", finalResult[i]) :
            fprintf(oFile, "%lf ", finalResult[i]);

    fprintf(oFile, "\n");
    printf("\nDone :)\nOutput is written on out.txt\n");

    fclose(oFile);
}

int main(int argc , char * argv[])
{
    int op;

	int my_rank;		/* rank of process	*/
	int p;			    /* number of process	*/
	int source;		    /* rank of sender	*/
	int dest;		    /* rank of reciever	*/
	int tag = 0;		/* tag for messages	*/
	MPI_Status status;	/* return status for 	*/
                        /* recieve		*/

	/* Start up MPI */
	MPI_Init( &argc , &argv );

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

    if(my_rank == 0){
        printf("Press 1 To Read from file\n");
        printf("Press 2 To Read from console\n");
        printf("Press any key To Exit\n");

        scanf("%d", &op);

        if(op == 1){
            if(!readDimensionsFromFile ())  error = 1;
            if( matrixAllocation () )       readMatricesFromFile (), mergeMatrices ();
            else                            error = 1;
        }
        else if(op == 2){
            readDimensionsFromConsole ();
            if( matrixAllocation () )       readMatricesFromConsole (), mergeMatrices ();
            else                            error = 1;
        }
        else {
            error = 1;
        }
    }

    // Error Handling
    MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (error == 1){
        MPI_FINALIZE ();
        return 0;
    }

    processWork (my_rank, p);

    if (my_rank == 0){

        if (op == 2)
            viewResult ();
        else if (op == 1)
            writeResultOnFile ();
    }

	/* shutdown MPI */
	MPI_Finalize();
	return 0;
}
