/*
	- I used to send the first matrix using MPI_Scatter according to the matrix rows 
	- I used to send the second matrix using MPI_Bcast to all processes
	- Each process takes a group of rows (r1/p), multiply with second matrix
	- Then I used to send the results of each process using MPI_Gather
	- Then I handled the reminder (r1%p) in the master without send or recieve
*/

#include <stdio.h>
#include <stdbool.h>
#include "mpi.h"

int i, j, k;
int r1, c1;
int r2, c2;
int* mtx1, * mtx2;
int* subMTX1;
int* subResult;
int* finalResult;

int error = 0;
FILE* iFile;

bool resultAllocation();

bool matrixAllocation () {
    mtx1 = malloc((r1*c1) * sizeof(int));
    mtx2 = malloc((r2*c2) * sizeof(int));
    return resultAllocation ();
}

char* readFileName () {
    char* fileName;
    printf("Enter File name: ");

    fgets(fileName, 15, stdin);
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

void readDimensionsFromConsole (){
    printf("Enter dimensions of the first matrix\n");
    scanf( "%d %d", &r1, &c1);

    printf("Enter dimensions of the second matrix\n");
    scanf("%d %d", &r2, &c2);
}

void readMatricesFromFile () {

    for(i = 0; i < (r1*c1); i++)
        fscanf(iFile, "%d", &mtx1[i]);

    for(i = 0; i < (r2*c2); i++)
        fscanf(iFile, "%d", &mtx2[i]);
}

void readMatricesFromConsole () {

    printf("Enter the first matrix\n");
    for(i = 0; i < (r1*c1); i++)
        scanf("%d", &mtx1[i]);

    printf("Enter the second matrix\n");
    for(i = 0; i < (r2*c2); i++)
        scanf("%d", &mtx2[i]);

}

bool resultAllocation () {

    if (c1 != r2){
        printf("INVALID dimensions!!!\n");
        printf("Matrix operation can NOT be performed");
        return false;
    }
    else {
        finalResult = malloc((r1*c2) * sizeof(int));
        return true;
    }
}

void processWork (int my_rank, int p){
    MPI_Bcast(&r1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&c1, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r2, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&c2, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(my_rank != 0)
        matrixAllocation ();

    MPI_Bcast(mtx2, (r2*c2), MPI_INT, 0, MPI_COMM_WORLD);

    subMTX1 = malloc((r1/p)*c1 * sizeof(int));
    MPI_Scatter(mtx1, (r1/p)*c1, MPI_INT, subMTX1, (r1/p)*c1, MPI_INT, 0, MPI_COMM_WORLD);

    subResult = malloc((r1/p)*c2 * sizeof(int));

    for(i=0 ; i<(r1/p)*c1 ; i+=c1){

        for (j=0 ; j<c2 ; j++) {
            int sum = 0;
            for(k=0 ; k<c1 ; k++)
                sum += subMTX1[ i+k ] * mtx2[ j+k*r2 ];

            subResult[ i+j ] = sum;
        }
    }

    MPI_Gather(subResult, (r1/p)*c2, MPI_INT, finalResult, (r1/p)*c2, MPI_INT, 0, MPI_COMM_WORLD);
}

void reminderHandling (int my_rank, int p){

    for(i=(r1/p)*p*c1 ; i<(r1*c1) ; i+=c1){

        for (j=0 ; j<c2 ; j++) {
            int sum = 0;
            for(k=0 ; k<c1 ; k++)
                sum += mtx1[ i+k ] * mtx2[ j+k*r2 ];

            finalResult[ i+j ] = sum;
        }
    }
}

void viewResult () {

    printf("\nThe result matrix ( %d , %d ) is\n", r1, c2);

    for(i=0 ; i<r1*c2 ; i++)
        i%c2==0 ?
            printf("\n%d ", finalResult[i]) :
            printf("%d ", finalResult[i]);

    printf("\n");
}

void writeResultOnFile () {

    viewResult ();

    FILE* oFile = fopen("out.txt", "w");
    fprintf(oFile, "The result matrix ( %d , %d ) is\n", r1, c2);

    for(i=0 ; i<r1*c2 ; i++)
        i%c2==0 ?
            fprintf(oFile, "\n%d ", finalResult[i]) :
            fprintf(oFile, "%d ", finalResult[i]);

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
            if( matrixAllocation () )       readMatricesFromFile ();
            else                            error = 1;
        }
        else if(op == 2){
            readDimensionsFromConsole ();
            if( matrixAllocation () )       readMatricesFromConsole ();
            else                            error = 1;
        }
        else {
            error = 1;
        }
    }

    MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (error == 1){
        MPI_FINALIZE ();
        return 0;
    }

    processWork (my_rank, p);

    if (my_rank == 0){
        reminderHandling (my_rank, p);

        if (op == 2)
            viewResult ();
        else if (op == 1)
            writeResultOnFile ();
    }

	/* shutdown MPI */
	MPI_Finalize();
	return 0;
}
