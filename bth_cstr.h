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

#ifndef BTH_CSTR_H
#define BTH_CSTR_H

#include <stdlib.h>

struct bth_cstr
{
    size_t len;
    char *data;
};

#define BTH_CSTR_AT(cstr, i) (cstr)->data[(i)]
#define BTH_CSTR_TA(cstr, i) (cstr)->data[(cstr)->len-(i)]

#ifndef BTH_CSTR_ERR
#include <err.h>
#define BTH_CSTR_ERR(c, msg, ...) err(c, msg, __VA_ARGS__)
#endif

#ifndef BTH_CSTR_ALLOC
#define BTH_CSTR_ALLOC(n) malloc(n)
#define BTH_CSTR_REALLOC(p, n) realloc(p, n)
#endif

#ifndef BTH_CSTR_MEMCPY
#include <string.h>
#define BTH_CSTR_MEMCPY(dst, src, n) memcpy(dst, src, n)
#endif

#ifndef BTH_CSTR_STRLEN
#include <string.h>
#define BTH_CSTR_STRLEN(s) strlen(s)
#endif

struct bth_cstr *bth_cstr_new(void);
struct bth_cstr *bth_cstr_alloc(size_t size);
struct bth_cstr *bth_cstr_from(char *src);
void bth_cstr_resize(struct bth_cstr *cstr, size_t size);
void bth_cstr_append(struct bth_cstr *cstr, char *src, size_t n);
void bth_cstr_cat(struct bth_cstr *dst, struct bth_cstr *src);

#endif

#ifdef BTH_CSTR_IMPLEMENTATION

struct bth_cstr *bth_cstr_new(void)
{
    struct bth_cstr *cstr = BTH_CSTR_ALLOC(sizeof(struct bth_cstr));

    if (cstr == NULL)
    {
        BTH_CSTR_ERR(1, "Can't allocate cstr");
    }

    cstr->len = 0;
    cstr->data = BTH_CSTR_ALLOC(0);

    return cstr;
}

struct bth_cstr *bth_cstr_alloc(size_t size)
{
    struct bth_cstr *cstr = bth_cstr_new();

    cstr->data = BTH_CSTR_ALLOC(size*sizeof(char));

    if (cstr->data == NULL && size != 0)
    {
        BTH_CSTR_ERR(1, "Can't allocate cstr data of size '%zu'", size);
    }
    else if (size != 0)
    {
        cstr->len = size-1;
    }

    return cstr;
}

void bth_cstr_resize(struct bth_cstr *cstr, size_t size)
{
    char *data = BTH_CSTR_REALLOC(cstr->data, size);

    if (data == NULL && size != 0)
    {
        BTH_CSTR_ERR(1, "Can't resize cstr to size '%zu'", size);
    }

    cstr->data = data;
}

void bth_cstr_append(struct bth_cstr *cstr, char *src, size_t n)
{
    bth_cstr_resize(cstr, cstr->len + 1 + n);
    char *org = cstr->data + cstr->len;
    BTH_CSTR_MEMCPY(org, src, n);
    cstr->len += n;
}

struct bth_cstr *bth_cstr_from(char *src)
{
    struct bth_cstr *cstr = bth_cstr_new();

    size_t len = BTH_CSTR_STRLEN(src);
    bth_cstr_append(cstr, src, len);
    cstr->len = len;

    return cstr;
}


void bth_cstr_cat(struct bth_cstr *dst, struct bth_cstr *src)
{
    bth_cstr_append(dst, src->data, src->len);
}

#endif
