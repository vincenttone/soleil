#ifndef _SOL_SPARSE_MAXTRIX_H_
#define _SOL_SPARSE_MAXTRIX_H_ 1

#include "sol_common.h"

enum _SolSparseMatrixRecordType {
    SolSparseMatrixRecordType_Char = 1,
    SolSparseMatrixRecordType_Int,
    SolSparseMatrixRecordType_Ptr,
};

typedef union {
    char rc;
    int ri;
    void *rp;
} SolSparseMatrixRecord;

typedef struct _SolSparseMatrix {
    size_t cs; // col size
    size_t rs; // row size
    size_t s; // size
    size_t c; // count
    size_t row_max;
    enum _SolSparseMatrixRecordType rt; // record type
    SolSparseMatrixRecord *r; // records
    size_t *cols; // column
    size_t *offsets; // offset
    int (*f_load)();
} SolSparseMatrix;

SolSparseMatrix* solSparseMatrix_new(size_t, size_t, enum _SolSparseMatrixRecordType);
void solSparseMatrix_free(SolSparseMatrix*);
int solSparseMatrix_load(SolSparseMatrix*, void*, size_t, size_t);

int solSparseMatrix_set(SolSparseMatrix*, size_t, size_t, SolSparseMatrixRecord);
SolSparseMatrixRecord* solSparseMatrix_get(SolSparseMatrix*, size_t, size_t);

#define solSparseMatrix_set_size(m, size) (m)->s = size
#define solSparseMatrix_size(m) (m)->s

#define solSparseMatrix_set_count(m, count) (m)->c = count
#define solSparseMatrix_incr_count(m) (m)->c++
#define solSparseMatrix_count(m) (m)->c

#define solSparseMatrix_set_row_size(m, s) (m)->rs = s
#define solSparseMatrix_row_size(m) (m)->rs

#define solSparseMatrix_set_col_size(m, s) (m)->cs = s
#define solSparseMatrix_col_size(m) (m)->cs

#define solSparseMatrix_set_record_type(m, t) (m)->rt = t
#define solSparseMatrix_record_type(m) (m)->rt

#define solSparseMatrix_set_records(m, records) (m)->r = records
#define solSparseMatrix_records(m) (m)->r

#define solSparseMatrix_set_columns(m, c) (m)->cols = c
#define solSparseMatrix_columns(m) (m)->cols

#define solSparseMatrix_set_offsets(m, o) (m)->offsets = o
#define solSparseMatrix_offsets(m) (m)->offsets

#define solSparseMatrix_record(m, o) (m->r + o)
#define solSparseMatrix_offset(m, o) (m->offsets + o)
#define solSparseMatrix_column(m, o) (m->cols + o)

#endif
