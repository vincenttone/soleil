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

int solSparseMatrix_load(SolSparseMatrix *m, void *a, size_t rs, size_t cs)
{
    if (rs > solSparseMatrix_row_size(m) - 1) return -1;
    if (cs > solSparseMatrix_col_size(m) - 1) return -1;
    size_t r, c;
    size_t o = 0;
    SolSparseMatrixRecord *rcd;
    SolSparseMatrixRecord *a1;
    size_t *ro;
    size_t *rc;
    for (r = 0; r < rs; r++) {
        ro = solSparseMatrix_offset(m, r);
        *ro = o;
        for (c = 0; c < cs; c++) {
            a1 = a + (c * cs + r);
            if (m->f_load) {
                if ((*m->f_load)(solSparseMatrix_record(m, o), a1) == 0) {
                    rc = solSparseMatrix_column(m, c);
                    *rc = c;
                    o++;
                }
            } else if (a1) {
                rcd = solSparseMatrix_record(m, o);
                *rcd = *a1;
                rc = solSparseMatrix_column(m, c);
                *rc = c;
                o++;
            }
        }
    }
    ro = solSparseMatrix_offset(m, r);
    *ro = o;
    return 0;
}

int solSparseMatrix_set(SolSparseMatrix *m, size_t row, size_t col, SolSparseMatrixRecord r)
{
    if (row > solSparseMatrix_row_size(m) - 1) return -1;
    if (col > solSparseMatrix_col_size(m) - 1) return -1;
    SolSparseMatrixRecord *r2c;
    size_t *c2c;
    if (solSparseMatrix_count(m) == 0) {
        r2c = solSparseMatrix_records(m);
        *r2c = r;
        c2c = solSparseMatrix_columns(m);
        *c2c = col;
        goto set_finish;
    }
    size_t *o2c;
    size_t o1;
    if (row < m->row_max) {
        o2c = (solSparseMatrix_offsets(m) + row);
        o1 = *o2c;
        size_t o2 = *(o2c + 1);
        for (; o1 < o2; o1++) {
            c2c = solSparseMatrix_columns(m) + o1;
            if (*c2c == col) { // swap
                r2c = (solSparseMatrix_records(m) + o1);
                *r2c = r;
                return 0;
            } else if (*c2c > o1) {
                break;
            }
        }
        size_t c = solSparseMatrix_count(m);
        for (; c > o1; c--) {
            r2c = (solSparseMatrix_records(m) + c);
            *r2c = *(solSparseMatrix_records(m) + c - 1);
            c2c = (solSparseMatrix_columns(m) + c);
            *c2c = *(solSparseMatrix_columns(m) + c - 1);
        }
    } else {
        o2c = solSparseMatrix_offsets(m) + m->row_max + 1;
        o1 = *o2c;
    }
    r2c = solSparseMatrix_records(m) + o1;
    *r2c = r;
    c2c = solSparseMatrix_columns(m) + o1;
    *c2c = col;
set_finish:
    // udpate offsets and total counts
    if (row > m->row_max) { // fix offsets
        size_t opre = *(size_t*)(solSparseMatrix_offsets(m) + m->row_max + 1);
        size_t fo;
        for (fo = m->row_max + 2; fo <= row; fo++) {
            o2c = solSparseMatrix_offsets(m) + fo;
            *o2c = opre;
        }
    }
    m->row_max = row;
    for (; row <= m->row_max; row++) {
        o2c = (solSparseMatrix_offsets(m) + row + 1);
        (*o2c)++;
    }
    solSparseMatrix_incr_count(m);
    o2c = (solSparseMatrix_offsets(m) + m->row_max + 1);
    (*o2c) = solSparseMatrix_count(m);
    return 0;
}

SolSparseMatrixRecord* solSparseMatrix_get(SolSparseMatrix *m, size_t row, size_t col)
{
    if (row > solSparseMatrix_row_size(m) - 1) return NULL;
    if (row > m->row_max) return NULL;
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
