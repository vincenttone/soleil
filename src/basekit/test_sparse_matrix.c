#include <stdio.h>
#include "sol_sparse_matrix.h"

int main()
{
    SolSparseMatrix *m = solSparseMatrix_new(10, 10, SolSparseMatrixRecordType_Char);
    SolSparseMatrixRecord r1;
    r1.rc = 1;
    solSparseMatrix_set(m, 3, 5, r1);
    SolSparseMatrixRecord *rr1 = solSparseMatrix_get(m, 3, 5);
    if (rr1) {
        printf("r1: %c\n", rr1->rc);
    }
    solSparseMatrix_free(m);
    return 0;
}
