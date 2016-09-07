
#include <stdio.h>
#include <stdlib.h>

#include "gl_math.h"
#include "render.h"
#include "cube.h"
#include "map.h"

void map_render(struct map *map, struct tri **tri_map, int *tri_count)
{
    int i, j;
    int wal_count = 0, count = 0;

    for (i = 0; i < map->width; i++)
        for (j = 0; j < map->height; j++)
            if (map->walls[i * map->height + j])
                wal_count++;

    *tri_map = realloc(*tri_map, sizeof(struct tri) * wal_count * 8);

    /* Height on Z, width on X */
    for (i = 0; i < map->width; i++) {
        for (j = 0; j < map->height; j++) {
            if (map->walls[i * map->height + j]) {
                struct cube cube = {
                    .tlf = { .x = i , .y = 0, .z = j },
                    .brb = { .x = i + 1, .y = 1, .z = j + 1 },
                };
                print_cube(&cube);
                render_cube_no_top_or_bottom(*tri_map + count * 8, &cube);
                count++;
            }
        }
    }

    *tri_count = wal_count * 8;
}

static void map_render_wall(struct map *map, struct tri **tri_map, int *tri_count, float yval)
{
    *tri_map = malloc(sizeof(struct tri) * 2);
    *tri_count = 2;

    (*tri_map)[0] = (struct tri) {
        .p2 = { .x = map->width, .y = yval, .z = 0 },
        .p1 = { .x = 0,          .y = yval, .z = 0 },
        .p3 = { .x = map->width, .y = yval, .z = map->height },
    };

    (*tri_map)[1] = (struct tri) {
        .p2 = { .x = 0,          .y = yval, .z = map->height },
        .p1 = { .x = 0,          .y = yval, .z = 0 },
        .p3 = { .x = map->width, .y = yval, .z = map->height },
    };
}

void map_render_floor(struct map *map, struct tri **tri_map, int *tri_count)
{
    map_render_wall(map, tri_map, tri_count, 0);
}

void map_render_ceiling(struct map *map, struct tri **tri_map, int *tri_count)
{
    map_render_wall(map, tri_map, tri_count, 1);
}

