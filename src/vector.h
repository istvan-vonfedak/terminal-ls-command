#ifndef VECTOR_H__
#define VECTOR_H__

#include <stdio.h>
#include <stdlib.h>

/* Since this vector stores void pointers it's up to the
 * user to provide a delete func for the data.
 * Resize does not delete any elements. The array can only
 * get larger.
 * It is up to the user to implement deep copy */

typedef struct _vector {
  int length, capacity;
  void **data;
} VECTOR;

VECTOR * makeVec()
{ VECTOR * ret;
  ret = (VECTOR *)malloc(sizeof(VECTOR));
  ret->length = 0;
  ret->capacity = 0;
  ret->data = NULL;
  return ret; }

/* if given no delete func it will do a shallow delete */
void deleteVec(VECTOR *v, void (*delete)(void *))
{ if(delete == NULL)
  { free(v->data);
    free(v); }
  else
  { int i;
    for(i = 0; i < v->capacity; i++)
    { delete(v->data[i]); }
    free(v->data);
    free(v); } }

void clearVec(VECTOR *v, void (*delete)(void *))
{ deleteVec(v,delete);
  v = makeVec(); }

/* initializes array pointers to NULL */
void _initVoidArr(void **d, int from, int to)
{ int i;
  for(i = from; i < to; i++)
    d[i] = NULL; }

/* increases the capacity of vector */
void resizeVec(VECTOR * v, int size)
{ if(v->data == NULL)
  { v->data = malloc(size * sizeof(void *));
    v->capacity = size;
    _initVoidArr(v->data,v->length,v->capacity); }
  if(size > v->capacity)
  { v->data = realloc(v->data,size * sizeof(void*));
    v->capacity = size;
    _initVoidArr(v->data,v->length,v->capacity); }
  if(size < v->length)
  { v->length = size; } }

void addAtBackVec(VECTOR * v, void * d)
{ if(v->data == NULL)
  { resizeVec(v, 10); }
  else if(v->length == v->capacity)
  { resizeVec(v, v->length+10); }
  v->data[v->length] = d;
  v->length++; }

void addDataVec(VECTOR *v, void *d, int index)
{ if(v->data == NULL)
  { resizeVec(v, index + 1); }
  if(index + 1 > v->capacity)
  { resizeVec(v, index + 1); }
  if(index + 1 > v->length)
  { v->length = index + 1; }
  v->data[index] = d; }

void* get(VECTOR *v, int index)
{ if(index < v->length) return v->data[index];
  else return NULL; }

void _swapVoidP(void **data, int a, int b)
{ void* temp = data[a];
  data[a] = data[b];
  data[b] = temp; }

/* helper funcs for quickSortVec */

/* partitions array based on the value of pivot */
int _partitionVoidP(void **d, int (*compare)(void*, void*), int low, int high)
{ int i, j;
  void * pivot = d[high];
  i = (low - 1);
  for (j = low; j <= high- 1; j++)
  { if (compare(d[j],pivot) <= 0)
    { i++;
      _swapVoidP(d, i, j); } }
  _swapVoidP(d, i + 1, high);
  return (i + 1); }

/* recursive QuickSort func */
void _quickSortVoidArr(void **d,int (*compare)(void*, void*), int low, int high)
{ if (low < high)
  { int pivot = _partitionVoidP(d, compare, low, high);
    _quickSortVoidArr(d, compare, low, pivot - 1);
    _quickSortVoidArr(d, compare, pivot + 1, high); } }

void quickSortVec(VECTOR *v, int (*compare)(void*, void*))
{ _quickSortVoidArr(v->data, compare, 0, v->length-1); }

/* helper function used by freeEmptySpaceVec(...) */
int _compareVoidPVec(void *a, void *b)
{ if(a == NULL && b == NULL) return 0;
  else if(a == NULL) return 1;
  else if(b == NULL) return -1;
  else return 0; }

void freeEmptySpaceVec(VECTOR * v)
{ int i, count;
  quickSortVec(v, _compareVoidPVec);
  for(i = 0; i < v->capacity; i++)
  { if(v->data[i] == NULL) count++; }
  v->data = realloc(v->data, (v->capacity - count) * sizeof(void*));
  v->capacity -= count;
  v->length = v->capacity; }

void printVec(VECTOR * v, void (*print)(void *))
{ int i;
//  printf("size = %d\nmax size = %d\nVector:\n", v->length, v->capacity);
  for(i = 0; i < v->length; i++)
  { print(v->data[i]); } }

/*
// These functions are not in use because they can
// cause memory leaks.

void shallowCopyVec(VECTOR * v, void ** d, int size)
{ int i;
  if(v->length > size)
  { for(i = 0; i < size; i++)
    { d[i] = v->data[i]; } }
  else
  { initArr(d,v->length-1, size);
    for(i = 0; i < v->length; i++)
    { d[i] = v->data[i]; } } }

// frees excess capacity
void shrinkVec(VECTOR *v)
{ v->data = realloc(v->data,v->length * sizeof(void*));
  v->capacity = v->length; }

*/
#endif /* VECTOR_H__ */

