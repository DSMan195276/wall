
#include <stdio.h>
#include <stdlib.h>

#include "gl_math.h"
#include "render.h"
#include "map.h"

void map_render(struct map *map, struct tri **tri_map, int *tri_count)
{
    int i, j;
    int wal_count = 0, count = 0;

    for (i = 0; i < map->width; i++)
        for (j = 0; j < map->height; j++)
            if (map->walls[i * map->width + j])
                wal_count++;

    *tri_map = realloc(*tri_map, sizeof(struct tri) * wal_count * 12);

    /* Height on Z, width on X */
    for (i = 0; i < map->width; i++) {
        for (j = 0; j < map->height; j++) {
            if (map->walls[i * map->width + j]) {
                struct cube cube = {
                    .tlf = { .x = i , .y = 0, .z = j },
                    .brb = { .x = i + 1, .y = 1, .z = j + 1 },
                };
                print_cube(&cube);
                render_cube(*tri_map + count * 12, &cube);
                count++;
            }
        }
    }

    *tri_count = wal_count * 12;
}

