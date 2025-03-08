// MIT License
// 
// Copyright (c) 2025 bobthehuge
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to 
// deal in the Software without restriction, including without limitation the 
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#ifndef BTH_HEAPARRAY_H
#define BTH_HEAPARRAY_H

#include <stdlib.h>

// https://en.wikipedia.org/wiki/D-ary_heap
// https://en.wikipedia.org/wiki/Binary_heap

#define HEAP_NOEXPAND   0x01
#define HEAP_LOCK       0x02
#define HEAP_CANFAIL    0x04

#define HEAP_MAX        0x10
#define HEAP_MIN        0x20

#define BTH_FLAG(hf, f) (((hf) & (f)) == (f))
#define BTH_SETFLAG(hf, f) ((hf) |= (f))

typedef struct
{
    size_t value;
    void *obj;
} HeapElement;

typedef struct
{
    const size_t d;
    size_t cap;
    size_t len;
    int flags;
    HeapElement *elts;
} HeapArray;

int heap_resize(HeapArray *heap, size_t n);
int heap_max_insert(HeapArray *heap, HeapElement elt);
int heap_min_insert(HeapArray *heap, HeapElement elt);
int is_max_heap(HeapArray heap);
int is_min_heap(HeapArray heap);

#endif

#ifdef BTH_HEAPARRAY_IMPLEMENTATION

#include <errno.h>

// resize is never called automatically
int heap_resize(HeapArray *heap, size_t n)
{
    if (BTH_FLAG(heap->flags, HEAP_NOEXPAND | HEAP_CANFAIL))
        return -HEAP_NOEXPAND;

    void *tmp = realloc(heap->elts, sizeof(HeapElement) * n);

    if (tmp == NULL && n != 0 && BTH_FLAG(heap->flags, HEAP_CANFAIL))
        return errno;

    heap->elts = tmp;
    heap->cap = n;

    return 0;
}

int heap_max_insert(HeapArray *heap, HeapElement elt)
{
    if (BTH_FLAG(heap->flags, HEAP_LOCK | HEAP_CANFAIL))
        return -HEAP_LOCK;

    if (!BTH_FLAG(heap->flags, HEAP_MAX | HEAP_CANFAIL))
        return -HEAP_MIN;

    if (heap->len >= heap->cap && BTH_FLAG(heap->flags, HEAP_CANFAIL))
        return -HEAP_NOEXPAND;

    if (heap->len == 0)
    {
        heap->len++;
        *heap->elts = elt;
        return 0;
    }

    size_t i1 = heap->len;
    size_t i2 = (heap->len - 1)/heap->d;

    heap->elts[i1] = elt;
    heap->len++;

    while (i1 > 0 && elt.value >= heap->elts[i2].value)
    {
        HeapElement t = heap->elts[i1];
        heap->elts[i1] = heap->elts[i2];
        heap->elts[i2] = t;
        i1 = i2;
        i2 = (i2 - 1)/heap->d;
    }

    return 0;
}

int heap_min_insert(HeapArray *heap, HeapElement elt)
{
    if (BTH_FLAG(heap->flags, HEAP_LOCK | HEAP_CANFAIL))
        return -HEAP_LOCK;

    if (!BTH_FLAG(heap->flags, HEAP_MIN | HEAP_CANFAIL))
        return -HEAP_MIN;

    if (heap->len >= heap->cap && BTH_FLAG(heap->flags, HEAP_CANFAIL))
        return -HEAP_NOEXPAND;

    if (heap->len == 0)
    {
        heap->len++;
        *heap->elts = elt;
        return 0;
    }

    size_t i1 = heap->len;
    size_t i2 = (heap->len - 1)/heap->d;

    heap->elts[i1] = elt;
    heap->len++;

    while (i1 > 0 && elt.value <= heap->elts[i2].value)
    {
        HeapElement t = heap->elts[i1];
        heap->elts[i1] = heap->elts[i2];
        heap->elts[i2] = t;
        i1 = i2;
        i2 = (i2 - 1)/heap->d;
    }

    return 0;
}

int is_max_heap(HeapArray heap)
{
    for (size_t i = 0; i < heap.len; i++)
        for (size_t j = 1; j <= heap.d && i*heap.d + j < heap.len; j++)
            if (heap.elts[i*heap.d+j].value >= heap.elts[i].value)
                return 0;
    return 1;
}

int is_min_heap(HeapArray heap)
{
    for (size_t i = 0; i < heap.len; i++)
        for (size_t j = 1; j <= heap.d && i*heap.d + j < heap.len; j++)
            if (heap.elts[i*heap.d+j].value <= heap.elts[i].value)
                return 0;
    return 1;
}

#endif
