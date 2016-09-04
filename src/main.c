
#include "render.h"
#include "map.h"
#include <stdio.h>
#include "gl_math.h"

static struct map map = {
    .width = 10,
    .height = 10,
    .walls = (int[]) {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 1, 1, 0, 1, 0, 1,
        1, 0, 0, 0, 1, 1, 0, 0, 0, 1,
        1, 1, 1, 0, 1, 1, 1, 0, 0, 1,
        1, 1, 1, 0, 1, 0, 1, 0, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 1, 0, 1, 1, 0, 1, 0, 1,
        1, 0, 0, 0, 1, 1, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    },
};

int main(int argc, char **argv)
{
    int ret;

    ret = renderer_start();
    if (ret)
        return ret;

    render_main_loop(&map);

    return renderer_end();
}