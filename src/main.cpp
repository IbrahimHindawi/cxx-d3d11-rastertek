#include <core.h>
#include <saha.h>
#include <Array.h>
#include <stdio.h>

auto main() -> int {
    Arena arena {};
    arenaInit(&arena);
    Array<i8> array = Array_reserve<i8>(&arena, 12);
    for (i32 i = 0; i < array.length; i++) {
        printf("array[%d] = %d", i, array.data[i]);
    }
    return 0;
}
