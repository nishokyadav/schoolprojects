// Matrix Multiplication with threads
// 

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

typedef struct
{
    int rows;           // number of rows for the matrix
    int cols;           // number of columns for the matrix
    int **data;         // 2-D array pointer to store the matrix data
}matrix;

typedef struct
{
    int rowNum;         // number of rows for the matrix of matrices
    int colNum;         // number of columns
    matrix *A;          // first matrix for multiplication
    matrix *B;          // second matrix for multiplication
    matrix *C;          // resultant matrix 
}multMat;

// Prototypes /////////////////////////////////////////////////////////////////////
void printMatrix( matrix );
void* threadMult(void*);
void loadMatrix( FILE* , matrix*, int );

// Main Function //////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    FILE *filePointer;
    char *ioMode = "r";
    matrix A, B, C;
    int temp, i, j;
    multMat **matrices;
    pthread_t **threadID;
    int matrixNumber = 0;
    
    // open first input file to retrieve 1st matrix data
    filePointer = fopen( argv[1], ioMode);
    
    // set which matrix this is for display purposes
    matrixNumber = 1;
    
    // load the data from the file into the first matrix
    loadMatrix(filePointer, &A, matrixNumber);
    
    // close the file
    fclose(filePointer);
    
    // open second input file to retrieve 2nd matrix data
    filePointer = fopen( argv[2], ioMode);
    
    // set which matrix this is for display purposes
    matrixNumber = 2;
    
    // load the data from the file into the second matrix
    loadMatrix(filePointer, &B, matrixNumber);
    
    // close the file
    fclose(filePointer);

    // Allocate memory for the product matrix C
    C.rows = A.rows;
    C.cols = B.cols;
    C.data = (int**)malloc(C.rows * sizeof(int*));
    for (i=0; i<C.rows; i++)
    {
        C.data[i] = (int*)malloc(C.cols * sizeof(int*));
    }
    
    // Allocate memory for a 2-D array of matrices
    matrices = (multMat**)malloc(A.rows*sizeof(multMat*));
    for (i=0; i<A.rows; i++)
    {
        matrices[i] = (multMat*)malloc(B.cols*sizeof(multMat));
    }
    
    // Allocate memory for 2-D array of threads
    threadID = (pthread_t**)malloc(A.rows*sizeof(pthread_t*));
    for (i=0; i<A.rows; i++)
    {
        threadID[i] = (pthread_t*)malloc(B.cols*sizeof(pthread_t));
    }
    
    // Check to see if matrices can be multiplied
    if(A.cols == B.rows)
    {
        // Create threads to do matrix multiplication
        for (i=0; i<A.rows; i++)
        {
            for (j=0; j<B.cols; j++)
            {
                matrices[i][j].A = &A;
                matrices[i][j].B = &B;
                matrices[i][j].C = &C;
                matrices[i][j].rowNum = i;
                matrices[i][j].colNum = j;
                pthread_create(&threadID[i][j], NULL, threadMult, (void*) &matrices[i][j]);
            }
        }
        
        // Wait for all threads to finish
        for (i=0; i<A.rows; i++)
        {
            for (j=0; j<B.cols; j++)
            {
                pthread_join(threadID[i][j], NULL);
            }
        }
        
        // Print the product matrix, C
        printf("Matrix C\n");
        printMatrix(C);
    }
    // If the two matrices can't be multiplied in that order, print error message
    else
    {
        printf("These two matrices can't be multiplied in this order.\n");
        
        // Deallocate memory for threads
        for (i=0; i<A.rows; i++)
        {
            free( threadID[i] );
        }
        free(threadID);
        
        // Deallocate memory for the matrices
        for (i=0; i<A.rows; i++)
        {
            free( matrices[i] );
        }
        free(matrices);
    }
    
    return 0;
}

/*///////////////////////////////////////////////////////////////////////////////
Function Implementations
///////////////////////////////////////////////////////////////////////////////*/
void printMatrix(matrix m)
{
    int i, j;
    
    // Print the number of rows and columns
    printf("%d\n", m.rows);
    printf("%d\n", m.cols);
    
    // Iterate through the matrix data and print the elements
    for ( i = 0; i < m.rows; i++)
    {
        for ( j = 0; j < m.cols; j++)
        {
            printf("%d ", m.data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void* threadMult(void* arg)
{
    multMat* matrices = (multMat*) arg;
    int i, sum = 0;
    
    // calculate multiplication at row and column
    matrices->C->data[matrices->rowNum][matrices->colNum] = 0;
	for(i=0; i < matrices->A->cols; i++)
	{
        
		// multiply row A by col B
		sum = matrices->A->data[matrices->rowNum][i] * matrices->B->data[i][matrices->colNum] + sum;
	}
	matrices->C->data[matrices->rowNum][matrices->colNum] = sum;
    
    return 0;
}


void loadMatrix( FILE* filePointer , matrix* A, int matNum)
{
    int temp, i, j;
    
    // display error is something is wrong with the input file
    if(filePointer == NULL)
    {
        printf("The 1st input file can't be opened.\n");
    }
    
    // Read in the number of rows
    fscanf(filePointer, "%d", &temp);
    A->rows = temp;
    
    // Read in the number of columns
    fscanf(filePointer, "%d", &temp);
    A->cols = temp;
    
    // Allocate memory for the data in the matrix
    A->data = (int**)malloc(A->rows * sizeof(int*));
    for(i = 0; i < A->rows; i++)
    {
        A->data[i] = (int*)malloc(A->cols * sizeof(int*));
    }
    
    // Save matrix data into my matrix
    for ( i = 0; i < A->rows; i++)
    {
        for ( j = 0; j < A->cols; j++)
        {
            fscanf(filePointer, "%d", &temp);
            A->data[i][j] = temp;
        }
    }
    
    // Print out the first matrix
    if (matNum == 1)
    {
        printf("\nMatrix A\n");
    }
    else if(matNum == 2)
    {
        printf("\nMatrix B\n");        
    }

    printMatrix(*A);

}









































