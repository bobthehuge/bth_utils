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

#ifndef BTH_DYNARRAY_H
#define BTH_DYNARRAY_H

#include <stdlib.h>

// total size is isize * cap
struct bth_dynarray
{
    // item count
    size_t len;
    // item size
    size_t isize;
    // allocated capacity (not total size)
    size_t cap;
    void *items;
};

#ifndef BTH_DYNARRAY_ERRX
#include <err.h>
#define BTH_DYNARRAY_ERRX(c, msg, ...) errx(c, msg, __VA_ARGS__)
#endif

#ifndef BTH_DYNARRAY_ALLOC
#define BTH_DYNARRAY_ALLOC(n) malloc(n)
#define BTH_DYNARRAY_FREE(p) free(p)
#define BTH_DYNARRAY_REALLOC(p, n) realloc(p, n)
#endif

#ifndef BTH_DYNARRAY_MEMCPY
#include <string.h>
#define BTH_DYNARRAY_MEMCPY(dst, src, n) memcpy(dst, src, n)
#endif

struct bth_dynarray bth_dynarray_init(size_t isize, size_t prealloc);
void bth_dynarray_get(struct bth_dynarray *da, size_t index, void *e);
void bth_dynarray_set(struct bth_dynarray *da, size_t index, void *e);
void bth_dynarray_append(struct bth_dynarray *da, void *e);
void bth_dynarray_pop(struct bth_dynarray *da, void *e);

#endif

#ifdef BTH_DYNARRAY_IMPLEMENTATION

struct bth_dynarray bth_dynarray_init(size_t isize, size_t prealloc)
{
    void *data = BTH_DYNARRAY_ALLOC(isize * prealloc);

    if (!data && prealloc)
        BTHBTH_DYNARRAY_ERRX(1,
            "Cannot prealloc %d items for dynarray", prealloc);

    struct bth_dynarray da = {
        .len = 0,
        .isize = isize,
        .cap = prealloc,
        .items = data,
    };

    return da;
}

void bth_dynarray_free(struct bth_dynarray *da)
{
    BTH_DYNARRAY_FREE(da->items);
    da->cap = 0;
    da->len = 0;
}

void bth_dynarray_resize(struct bth_dynarray *da, size_t n)
{
    void *data = BTH_DYNARRAY_REALLOC(da->items, da->isize * n);

    if (!data && n)
    {
        BTH_DYNARRAY_ERRX(1, 
             "Cannot realloc %d items of size %d for dynarray",
             n, da->isize);
    }

    da->items = data;
    da->len = n > da->len ? da->len : n;
    da->cap = n;
}

void bth_dynarray_get(struct bth_dynarray *da, size_t index, void *e)
{
    if (index >= da->len)
        BTH_DYNARRAY_ERRX(1, "Index out of bound of dynarray");

    char *start = da->items;
    BTH_DYNARRAY_MEMCPY(e, start + index * da->isize, da->isize);
}

void bth_dynarray_set(struct bth_dynarray *da, size_t index, void *e)
{
    if (index >= da->len)
        BTH_DYNARRAY_ERRX(1, "Index out of bound of dynarray");

    char *start = da->items;
    BTH_DYNARRAY_MEMCPY(start + index * da->isize, e, da->isize);
}

void bth_dynarray_append(struct bth_dynarray *da, void *e)
{
    if (da->len >= da->cap)
        bth_dynarray_resize(da, da->cap + 1);

    da->len++;
    bth_dynarray_set(da, da->len - 1, e);
}

void bth_dynarray_pop(struct bth_dynarray *da, void *e)
{
    if (!da->len)
        BTH_DYNARRAY_ERRX(1, "Invalid pop on empty dynarray");

    if (e)
        bth_dynarray_get(da, da->len - 1, e);
    
    da->len--;
}

#endif
