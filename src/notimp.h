#ifndef NOTIMP_HEADER
#define NOTIMP_HEADER

#include <stdint.h>
#include <string.h>

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;
typedef uint8_t     b8;
typedef uint8_t     c8;
typedef uint16_t    c16;
typedef float       f32;
typedef double      f64;

#include "notimp_math.h"

#define assert(expression) if(!(expression)) { *(volatile int*)0 = 0; }
#define alignof(struct) 8

void *os_mem_reserve(u64 size_bytes);
void os_mem_commit(void *p, u64 size_bytes);

typedef struct
{
    void    *base;
    u64     reserved;
    u64     commited;
    u64     used;

} arena, arena_t;

arena *arena_alloc(u64 reserve_bytes, u64 commit_bytes)
{
    void *p = os_mem_reserve(reserve_bytes);
    os_mem_commit(p, commit_bytes);
    arena_t *arena = (arena_t *)p;
    arena->base = p;
    arena->reserved = reserve_bytes;
    arena->commited = commit_bytes;
    arena->used = sizeof(arena_t);
    return arena;
}

void *arena_push_dont_zero(arena *arena, u64 size, u16 alignment)
{
    u64 padding = alignment - arena->used % alignment;
    void *p = (u8*)arena->base + arena->used + padding;
    arena->used += padding + size;
    if(arena->commited < arena->used)
    {
        arena->commited = arena->used / OS_PAGE_SIZE * OS_PAGE_SIZE + OS_PAGE_SIZE;
        os_mem_commit(arena->base, arena->commited);
    }
    return p;
}

void *arena_push(arena *arena, u64 size, u16 alignment)
{
    void *p = arena_push_dont_zero(arena, size, alignment);
    memset(p, 0, size);
    return p;
}

#define arena_push_struct(a, s) (s*)arena_push(a, sizeof(s), alignof(s))

typedef struct 
{
    u8* cstring_ptr;
    u32 length_without_null;

} string8;

u32 cstring_length(c8 *cstr)
{
    u32 i = 0;
    while(cstr[i]) { i++; }
    return i;
}

string8 cstring_add(c8 *a, c8 *b, arena *arena)
{
    u32 length_a = cstring_length(a);
    u32 length_b = cstring_length(b);
    c8 *res = arena_push_dont_zero(arena, length_a + length_b + 1, 8);
    memcpy(res, a, length_a);
    memcpy(res + length_a, b, length_b);
    res[length_a + length_b] = 0;
    return (string8) { res, length_a + length_b };
}

void *os_read_file(c8 *path, arena *arena, u64 *out_size);

#endif