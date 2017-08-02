#ifndef _SOL_SPARSE_MAXTRIX_H_
#define _SOL_SPARSE_MAXTRIX_H_ 1

#include "sol_common.h"

enum _SolSparseMatrixRecordType {
    solSparseMatrixRecordType_Char = 1,
    solSparseMatrixRecordType_Int,
    solSparseMatrixRecordType_Ptr,
};

typedef union {
    char rc;
    int ri;
    void *rp;
} SolSparseMatrixRecord;

typedef struct _SolSparseMatrix {
    size_t h; // height
    size_t w; // weight
    size_t s; // size
    size_t c; // count
    enum _SolSparseMatrixRecordType t; // record type
    SolSparseMatrixRecord *r; // records
    size_t *col; // column
    size_t *offset; // offset
} SolSparseMatrix;

SolSparseMatrix* solSparseMatrix_new(size_t, size_t, enum _SolSparseMatrixRecordType);
void solSparseMatrix_free(SolSparseMatrix*);
int solSparseMatrix_add_char(SolSparseMatrix*, size_t, size_t, SolSparseMatrixRecord);

#endif
