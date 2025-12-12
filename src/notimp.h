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

#define assert(expression) if(!expression) { *(int*)0 = 0; }

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

void *os_read_file(c8 *path, arena *arena, u64 *out_size);