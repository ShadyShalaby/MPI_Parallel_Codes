#include <stdio.h>
#include <stdbool.h>

int i, j, k;
int r1, c1;
int r2, c2;
int** mtx1, ** mtx2;
int** result;

void matrixAllocation () {
    mtx1 = malloc(r1 * sizeof(int *));
	for(i = 0; i < r1; i++)
		mtx1[i] = malloc(c1 * sizeof(int));

    mtx2 = malloc(r2 * sizeof(int *));
	for(i = 0; i < r2; i++)
		mtx2[i] = malloc(c2 * sizeof(int));
}

void readMatricesFromFile (FILE* file) {

    for(i = 0; i < r1; i++)
        for(j = 0; j < c1; j++)
            fscanf(file, "%d", &mtx1[i][j]);

    for(i = 0; i < r2; i++)
        for(j = 0; j < c2; j++)
            fscanf(file, "%d", &mtx2[i][j]);
}

void readMatricesFromConsole () {

    printf("Enter the first matrix\n");
    for(i = 0; i < r1; i++)
        for(j = 0; j < c1; j++)
            scanf("%d", &mtx1[i][j]);

    printf("Enter the second matrix\n");
    for(i = 0; i < r2; i++)
        for(j = 0; j < c2; j++)
            scanf("%d", &mtx2[i][j]);
}

bool resultAllocation () {
    if (c1 != r2)
        return false;
    else {
        result = malloc(r1 * sizeof(int *));
        for(i = 0; i < r1; i++)
            result[i] = malloc(c2 * sizeof(int));
        return true;
    }
}

bool multiplication () {

    if( ! resultAllocation() ){
        printf("Invalid dimensions !!!");
        return false;
    }
    else {
        for(i = 0; i < r1; i++){
            for(j = 0; j < c2; j++){
                int sum = 0;
                for(k = 0; k < r2; k++){
                    sum += (mtx1[i][k] * mtx2[k][j]);
                }
                result[i][j] = sum;
            }
        }
        return true;
    }
}

void viewResult () {

    printf("\nThe result matrix ( %d , %d ) is\n\n", r1, c2);
    for(i = 0; i < r1; i++){
        for(j = 0; j < c2; j++){
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }
}

void wiriteResultOnFile () {

    FILE *file = fopen("out.txt", "w");
    fprintf(file, "The result matrix ( %d , %d ) is\n\n", r1, c2);
    for(i = 0; i < r1; i++){
        for(j = 0; j < c2; j++){
            fprintf(file, "%d ", result[i][j]);
        }
        fprintf(file, "\n");
    }
    printf("\nDone :)\nOutput is written on out.txt\n");
}

int main(){

    printf("Press 1 To Read from file \n");
    printf("Press 2 To Read from console\n");

    int op;
    scanf("%d", &op);

    if(op == 1){
        FILE *file = fopen("in.txt", "r");
        fscanf(file, "%d %d", &r1, &c1);
        fscanf(file, "%d %d", &r2, &c2);

        matrixAllocation ();
        readMatricesFromFile (file);
        if ( multiplication () )
            wiriteResultOnFile ();
    }
    else if(op == 2){
        printf("Enter dimensions of the first matrix\n");
        scanf( "%d %d", &r1, &c1);

        printf("Enter dimensions of the second matrix\n");
        scanf("%d %d", &r2, &c2);

        matrixAllocation ();
        readMatricesFromConsole ();
        if ( multiplication () )
            viewResult ();
    }
    else {
        printf("WRONG choice !!!\n");
    }

    return 0;
}
