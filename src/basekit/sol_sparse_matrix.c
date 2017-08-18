#include <stdio.h>
#include "sol_sparse_matrix.h"

SolSparseMatrix* solSparseMatrix_new(size_t row_size, size_t col_size, enum _SolSparseMatrixRecordType t)
{
    SolSparseMatrix *m = sol_calloc(1, sizeof(SolSparseMatrix));
    if (m == NULL) {
        return NULL;
    }
    solSparseMatrix_set_row_size(m, row_size);
    solSparseMatrix_set_col_size(m, col_size);
    solSparseMatrix_set_record_type(m, t);
    solSparseMatrix_set_size(m, row_size + col_size);
    solSparseMatrix_set_records(m, sol_calloc(1, sizeof(SolSparseMatrixRecord) * (solSparseMatrix_size(m))));
    solSparseMatrix_set_columns(m, sol_calloc(1, sizeof(size_t) * (solSparseMatrix_size(m))));
    solSparseMatrix_set_offsets(m, sol_calloc(1, sizeof(size_t) * (solSparseMatrix_size(m))));
    if (solSparseMatrix_records(m) == NULL
        ||solSparseMatrix_columns(m) == NULL
        || solSparseMatrix_offsets(m) == NULL
        ) {
        solSparseMatrix_free(m);
        return NULL;
    }
    return m;
}

void solSparseMatrix_free(SolSparseMatrix *m)
{
    if (solSparseMatrix_records(m)) sol_free(solSparseMatrix_records(m));
    if (solSparseMatrix_columns(m)) sol_free(solSparseMatrix_columns(m));
    if (solSparseMatrix_offsets(m)) sol_free(solSparseMatrix_offsets(m));
    sol_free(m);
}

int solSparseMatrix_load(SolSparseMatrix *m, SolSparseMatrixRecord **a, size_t rs, size_t cs)
{
    if (rs > solSparseMatrix_row_size(m) - 1) return -1;
    if (cs > solSparseMatrix_col_size(m) - 1) return -1;
    size_t r, c;
    SolSparseMatrixRecord *r1;
    SolSparseMatrixRecord *r2;
    size_t *ro;
    size_t *rc;
    for (r = 0; r < rs; r++) {
        ro = solSparseMatrix_offset(m, r);
        *ro = solSparseMatrix_count(m);
        for (c = 0; c < cs; c++) {
            r2 = *(a + rs * r + c);
            if (m->f_load) {
                if ((*m->f_load)(solSparseMatrix_record(m, solSparseMatrix_count(m)), r2) == 0) {
                    rc = solSparseMatrix_column(m, solSparseMatrix_count(m));
                    *rc = c;
                    solSparseMatrix_incr_count(m);
                }
            } else if (r2) {
                r1 = solSparseMatrix_record(m, solSparseMatrix_count(m));
                *r1 = *r2;
                rc = solSparseMatrix_column(m, solSparseMatrix_count(m));
                *rc = c;
                solSparseMatrix_incr_count(m);
            }
        }
    }
    ro = solSparseMatrix_offset(m, r);
    *ro = solSparseMatrix_count(m);
    return 0;
}

int solSparseMatrix_traverse(SolSparseMatrix *m, int (*f_out)(SolSparseMatrixRecord*, size_t, size_t))
{
    int ret;
    size_t r, c1, c2, o1, o2;
    for (r = 0; r < solSparseMatrix_row_size(m); r++) {
        o1 = *(size_t*)(solSparseMatrix_offsets(m) + r);
        o2 = *(size_t*)(solSparseMatrix_offsets(m) + r + 1);
        for (c1 = 0; c1 < solSparseMatrix_col_size(m); c1++) {
            c2 = *(solSparseMatrix_columns(m) + o1);
            if (c1 == c2 && o1 < o2) {
                ret = f_out((SolSparseMatrixRecord*)(solSparseMatrix_records(m) + o1), r, c1);
                if (ret != 0) {
                    return ret;
                }
                o1++;
            } else {
                ret = f_out(NULL, r, c1);
                if (ret != 0) {
                    return ret;
                }
            }
        }
    }
    return 0;
}

SolSparseMatrixRecord* solSparseMatrix_get(SolSparseMatrix *m, size_t row, size_t col)
{
    if (row > solSparseMatrix_row_size(m) - 1) return NULL;
    if (col > solSparseMatrix_col_size(m) - 1) return NULL;
    size_t o1 = *(size_t*)(solSparseMatrix_offsets(m) + row);
    size_t o2 = *(size_t*)(solSparseMatrix_offsets(m) + row + 1);
    for (; o1 < o2; o1++) {
        if (col == *(size_t*)(solSparseMatrix_columns(m) + o1)) {
            return (SolSparseMatrixRecord*)(solSparseMatrix_records(m) + o1);
        }
    }
    return NULL;
}
int solSparseMatrix_set(SolSparseMatrix *m, size_t row, size_t col, SolSparseMatrixRecord r)
{
    if (row > solSparseMatrix_row_size(m) - 1) return -1;
    if (col > solSparseMatrix_col_size(m) - 1) return -1;
    size_t o1 = *(size_t*)(solSparseMatrix_offsets(m) + row);
    size_t o2 = *(size_t*)(solSparseMatrix_offsets(m) + row + 1);
    size_t *c1;
    size_t *c2;
    size_t oc = solSparseMatrix_count(m) + 1;
    SolSparseMatrixRecord *r1;
    SolSparseMatrixRecord *r2;
    if (o1 == o2) {
        goto append;
    } else {
        for (; o1 < o2; o1++) {
            c1 = solSparseMatrix_columns(m) + o1;
            if (*c1 > col) {
                break;
            }
        }
        o1--;
    }
append:
    do {
        c1 = solSparseMatrix_column(m, oc);
        r1 = solSparseMatrix_record(m, oc--);
        c2 = solSparseMatrix_column(m, oc);
        r2 = solSparseMatrix_record(m, oc);
        *c1 = *c2;
        *r1 = *r2;
    } while (o1 < oc);
    c1 = solSparseMatrix_column(m, o1);
    *c1 = col;
    r1 = solSparseMatrix_record(m, o1);
    *r1 = r;
    solSparseMatrix_incr_count(m);
    size_t *o;
    for (; row <= solSparseMatrix_row_size(m); row++) {
        o = solSparseMatrix_offset(m, row + 1);
        (*o)++;
    }
    return 0;
}
