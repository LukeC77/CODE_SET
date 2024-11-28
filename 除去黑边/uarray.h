#ifndef UARRAY_INCLUDED
#define UARRAY_INCLUDED

#define T UArray_T
typedef struct T *T;

extern T    UArray_new (int length, int size);
extern void UArray_free(T *uarray);
extern int UArray_length(T uarray);
extern int UArray_size  (T uarray);
void *UArray_at(T uarray, int i);
extern void UArray_resize(T uarray, int length);
extern T    UArray_copy  (T uarray, int length);

#undef T
#endif
