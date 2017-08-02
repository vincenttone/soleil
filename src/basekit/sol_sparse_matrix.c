#include "sol_spare_matrix.h"

SolSparseMatrix* solSparseMatrix_new(size_t h, size_t w, enum _SolSparseMatrixRecordType t)
{
    SolSparseMatrix *s = sol_calloc(1, sizeof(SolSparseMatrix));
    s->h = h;
    s->w = w;
    s->t = t;
    s->s = h + w;
    s->r = sol_calloc(1, sizeof(SolSparseMatrixRecord) * s->s);
    s->offset = sol_calloc(1, sizeof(size_t) * s->s);
    s->col = sol_calloc(1, sizeof(size_t) * s->s);
    return s;
}

void solSparseMatrix_free(SolSparseMatrix *s)
{
    sol_free(s->r);
    sol_free(s->offset);
    sol_free(s->col);
    sol_free(s);
}

int solSparseMatrix_add_char(SolSparseMatrix *m, size_t col, size_t row, SolSparseMatrixRecord r)
{
    
}
