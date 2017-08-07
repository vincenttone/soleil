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

int solSparseMatrix_set(SolSparseMatrix *m, size_t row, size_t col, SolSparseMatrixRecord r)
{
    if (row > solSparseMatrix_row_size(m) - 1) return -1;
    if (col > solSparseMatrix_col_size(m) - 1) return -1;
    size_t *o1 = solSparseMatrix_offsets(m) + row;
    size_t *o2 = o1 + 1;
    size_t o;
    int ac = 1; // prepend
    for (o = *o1; o < *o2; o++) {
        if (col == *(size_t*)(solSparseMatrix_columns(m) + o)) {
            ac = 2; // swap
            break;
        } else if (col < *(size_t*)(solSparseMatrix_columns(m) + o)) {
            continue;
        } else {
            break;
        }
    }
    if (ac == 2) { // swap record
        SolSparseMatrixRecord *r2s = (solSparseMatrix_records(m) + o);
        // todo: free pre record
        *r2s = r;
        return 0;
    }
    if (solSparseMatrix_count(m) >= solSparseMatrix_size(m)) {
        // todo: resize
        return -1;
    }
    size_t count;
    size_t *c2s;
    SolSparseMatrixRecord *r2s;
    for (count = solSparseMatrix_count(m); count > o; count--) {
        c2s = solSparseMatrix_columns(m) + count;
        *c2s = *(c2s - 1);
        r2s = solSparseMatrix_records(m) + count;
        *r2s = *(r2s -1);
    }
    c2s = solSparseMatrix_columns(m) + o;
    *c2s = col;
    r2s = solSparseMatrix_records(m) + o;
    *r2s = r;
    solSparseMatrix_incr_count(m);
    return 0;
}

SolSparseMatrixRecord* solSparseMatrix_get(SolSparseMatrix *m, size_t row, size_t col)
{
    if (row > solSparseMatrix_row_size(m) - 1) return NULL;
    if (col > solSparseMatrix_col_size(m) - 1) return NULL;
    size_t *o1 = solSparseMatrix_offsets(m) + row;
    size_t *o2 = o1 + 1;
    size_t o;
    for (o = *o1; o < *o2; o++) {
        if (col == *(size_t*)(solSparseMatrix_columns(m) + o)) {
            return (SolSparseMatrixRecord*)(solSparseMatrix_records(m) + o);
        }
    }
    return NULL;
}
