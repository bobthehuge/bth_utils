// MIT No Attribution
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

#define BTH_HEAP_FLAG(hf, f) (((hf) & (f)) == (f))
#define BTH_HEAP_SETFLAG(hf, f) ((hf) |= (f))

#define BTH_HEAP_CMP(_max, x, y) ((_max) ? (x) > (y) : (x) < (y))
#define BTH_HEAP_CMP_EQ(_max, x, y) ((_max) ? (x) >= (y) : (x) <= (y))

#define BTH_HEAP_GETVAL(h, i) ((heap)->elts[i].value)
#define BTH_HEAP_CMP_IDX(_max, h, x, y) \
    BTH_HEAP_CMP(_max, BTH_HEAP_GETVAL(h, x), BTH_HEAP_GETVAL(h, y))
#define BTH_HEAP_CMP_EQ_IDX(_max, h, x, y) \
    BTH_HEAP_CMP_EQ(_max, BTH_HEAP_GETVAL(h, x), BTH_HEAP_GETVAL(h, y))

#ifndef BTH_HEAPARRAY_REALLOC
#include <errno.h>
#define BTH_HEAPARRAY_ERRNO errno
#define BTH_HEAPARRAY_REALLOC(p, n) realloc(p, n)
#else
#define BTH_HEAPARRAY_ERRNO 0xFF
#endif

struct bth_heap_elt
{
    size_t value;
    void *obj;
};

struct bth_heaparray
{
    const size_t d;
    size_t cap;
    size_t len;
    char flags;
    struct bth_heap_elt *elts;
};

int bth_heap_resize(struct bth_heaparray *heap, size_t n);
int bth_sift_down(struct bth_heaparray *heap, size_t idx);
int bth_heap_push(struct bth_heaparray *heap, struct bth_heap_elt elt);
int bth_is_max_heap(struct bth_heaparray *heap);
int bth_is_min_heap(struct bth_heaparray *heap);

#endif

#ifdef BTH_HEAPARRAY_IMPLEMENTATION


// resize is never called automatically
int bth_heap_resize(struct bth_heaparray *heap, size_t n)
{
    if (BTH_HEAP_FLAG(heap->flags, HEAP_NOEXPAND | HEAP_CANFAIL))
        return -HEAP_NOEXPAND;

    void *tmp =
        BTH_HEAPARRAY_REALLOC(heap->elts, sizeof(struct bth_heap_elt) * n);

    if (tmp == NULL && n != 0 && BTH_HEAP_FLAG(heap->flags, HEAP_CANFAIL))
        return BTH_HEAPARRAY_ERRNO;

    heap->elts = tmp;
    heap->cap = n;

    return 0;
}

int bth_heap_sift_up(struct bth_heaparray *heap, size_t idx)
{
    size_t sift_idx = idx;
    const int ismax = BTH_HEAP_FLAG(heap->flags, HEAP_MAX);
    const struct bth_heap_elt entry = heap->elts[sift_idx];

    for (size_t parent_idx; 0 < sift_idx; sift_idx = parent_idx)
    {
        parent_idx = (sift_idx - 1) / heap->d;
        size_t pval = BTH_HEAP_GETVAL(heap, parent_idx);
        if (BTH_HEAP_CMP_EQ(ismax, pval, entry.value))
            break;

        heap->elts[sift_idx] = heap->elts[parent_idx];
    }
    heap->elts[sift_idx] = entry;
}

int bth_heap_sift_down(struct bth_heaparray *heap, size_t idx)
{
    if (BTH_HEAP_FLAG(heap->flags, HEAP_LOCK | HEAP_CANFAIL))
        return -HEAP_LOCK;

    if (idx >= heap->len && BTH_HEAP_FLAG(heap->flags, HEAP_CANFAIL))
        return -HEAP_NOEXPAND;

    
    size_t sift_idx = idx;
    size_t last_idx = heap->len - 1;

    const int ismax = BTH_HEAP_FLAG(heap->flags, HEAP_MAX);
    const struct bth_heap_elt entry = heap->elts[sift_idx];

    size_t last_parent = (last_idx - 1) / heap->d;

    while (sift_idx <= last_parent)
    {
        size_t child = sift_idx * heap->d + 1;
        size_t last_sib = (sift_idx + 1) * heap->d;

        if (last_sib >= last_idx)
            last_sib = last_idx;

        for (size_t sib_idx = child + 1; sib_idx <= last_sib; sib_idx++)
            if (BTH_HEAP_CMP_IDX(ismax, heap, sib_idx, child))
                child = sib_idx;

        if (BTH_HEAP_CMP_EQ(ismax, entry.value, BTH_HEAP_GETVAL(heap, child)))
            break;

        heap->elts[sift_idx] = heap->elts[child];

        sift_idx = child;
    }

    heap->elts[sift_idx] = entry;

    return 0;
}

int bth_heap_push(struct bth_heaparray *heap, struct bth_heap_elt elt)
{
    if (BTH_HEAP_FLAG(heap->flags, HEAP_LOCK | HEAP_CANFAIL))
        return -HEAP_LOCK;

    if (heap->len >= heap->cap && BTH_HEAP_FLAG(heap->flags, HEAP_CANFAIL))
        return -HEAP_NOEXPAND;

    if (heap->len == 0)
    {
        heap->len++;
        *heap->elts = elt;
        return 0;
    }

    heap->elts[heap->len] = elt;
    bth_heap_sift_up(heap, heap->len);
    heap->len++;

    return 0;
}

int bth_heap_pop(struct bth_heaparray *heap, struct bth_heap_elt *res)
{
    if (BTH_HEAP_FLAG(heap->flags, HEAP_LOCK | HEAP_CANFAIL))
        return -HEAP_LOCK;

    if (heap->len == 0 && BTH_HEAP_FLAG(heap->flags, HEAP_CANFAIL))
        return -HEAP_NOEXPAND;

    *res = heap->elts[0];
    heap->elts[0] = heap->elts[heap->len - 1];

    if (!--heap->len)
        return 0;
    
    return bth_heap_sift_down(heap, 0);
}

int bth_is_max_heap(struct bth_heaparray *heap)
{
    for (size_t i = 0; i < heap->len; i++)
        for (size_t j = 1; j <= heap->d && i*heap->d + j < heap->len; j++)
            if (heap->elts[i*heap->d+j].value >= heap->elts[i].value)
                return 0;
    return 1;
}

int bth_is_min_heap(struct bth_heaparray *heap)
{
    for (size_t i = 0; i < heap->len; i++)
        for (size_t j = 1; j <= heap->d && i*heap->d + j < heap->len; j++)
            if (heap->elts[i*heap->d+j].value <= heap->elts[i].value)
                return 0;
    return 1;
}

#endif
