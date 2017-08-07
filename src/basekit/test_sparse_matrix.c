#include <stdio.h>
#include "sol_sparse_matrix.h"

int sm_set_char_value(SolSparseMatrix *m, size_t row, size_t col, char c)
{
    SolSparseMatrixRecord r;
    r.rc = c;
    printf("--------set [%zu, %zu, %c] ---------\n", row, col, c);
    return solSparseMatrix_set(m, row, col, r);
}

void debug_info(SolSparseMatrix *m)
{
    size_t i;
    SolSparseMatrixRecord *r1;
    size_t *x;
    printf("RECORDS: ");
    for (i = 0; i < m->c; i++) {
        r1 = (m->r + i);
        printf("%c ", r1->rc);
    }
    printf("\n");
    printf("COLS: ");
    for (i = 0; i < m->c; i++) {
        x = (m->cols + i);
        printf("%zu ", *(size_t*)x);
    }
    printf("\n");
    printf("OFFSETS: ");
    for (i = 0; i < m->s; i++) {
        x = (m->offsets + i);
        printf("%zu ", *(size_t*)x);
    }
    printf("\n");
}

int main()
{
    size_t r = 10;
    size_t c = 10;
    SolSparseMatrix *m = solSparseMatrix_new(r, c, SolSparseMatrixRecordType_Char);
    sm_set_char_value(m, 2, 3, 'a');
    debug_info(m);
    sm_set_char_value(m, 5, 6, 'b');
    debug_info(m);
    sm_set_char_value(m, 8, 9, 'c');
    debug_info(m);
    sm_set_char_value(m, 3, 6, 'd');
    debug_info(m);
    SolSparseMatrixRecord *rr1;
    size_t row, col;
    printf("\\\t");
    for (col = 0; col < c; col++) {
        printf("%zu\t", col);
    }
    printf("\n");
    for (row = 0; row < r; row++) {
        printf("%zu\t", row);
        for (col = 0; col < c; col++) {
            rr1 = solSparseMatrix_get(m, row, col);
            if (rr1) {
                printf("%c\t", rr1->rc);
            } else {
                printf("-\t");
            }
        }
        printf("\n");
    }
    solSparseMatrix_free(m);
    return 0;
}
