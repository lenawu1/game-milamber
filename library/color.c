#include "../include/color.h"
#include "math.h"
#include <stdio.h>
#include "list.h"
#include <stdlib.h>
#include <assert.h>

rgb_color_t rgb_color_rand(){
    rgb_color_t random_color = { 
        .r = drand48(),
        .g = drand48(),
        .b = drand48()
    };
    return random_color;
}

rgb_color_t rgb_color_init(double r, double g, double b){
    rgb_color_t color = {.r = r / 255.0, .g = g / 255.0, .b = b / 255.0};
    return color;
}

rgb_color_t *rgb_color_list_init(double r, double g, double b){
    rgb_color_t *color = malloc(sizeof(rgb_color_t));
    color->r = r / 255.0;
    color->g = g / 255.0;
    color->b = b / 255.0;
    return color;
}

bool rgb_color_equal(rgb_color_t color1, rgb_color_t color2) {
    bool r = color1.r == color2.r;
    bool g = color1.g == color2.g;
    bool b = color1.b == color2.b;
    return (r && g && b);
}

rgb_color_t rgb_color_red() {
    rgb_color_t red = rgb_color_init(255, 0, 0);
    return red;
}

rgb_color_t rgb_color_yellow() {
    rgb_color_t yellow = rgb_color_init(255, 255, 0);
    return yellow;
}

rgb_color_t rgb_color_gray() { 
    rgb_color_t gray = rgb_color_init(105, 105, 105);
    return gray;
}

rgb_color_t rgb_color_neon_green() { 
    rgb_color_t neon_green = rgb_color_init(57, 255, 20);
    return neon_green;
}

rgb_color_t rgb_color_rainbows(int i) {
    list_t *rainbow = list_init(6, (free_func_t) free);
    assert (rainbow != NULL);

    list_add(rainbow, rgb_color_list_init(255, 154, 162));
    list_add(rainbow, rgb_color_list_init(255, 183, 178));
    list_add(rainbow, rgb_color_list_init(255, 218, 193));
    list_add(rainbow, rgb_color_list_init(238, 215, 199));
    list_add(rainbow, rgb_color_list_init(255, 255, 204));

    list_add(rainbow, rgb_color_list_init(226, 240, 203));
    list_add(rainbow, rgb_color_list_init(181, 234, 215));
    list_add(rainbow, rgb_color_list_init(199, 214, 236));
    list_add(rainbow, rgb_color_list_init(199, 206, 234));
    list_add(rainbow, rgb_color_list_init(204, 201, 236));

    rgb_color_t *my_pointer = list_get(rainbow, i);    
    rgb_color_t my_color = {my_pointer->r, my_pointer->g, my_pointer->b};

    list_free(rainbow);
    return my_color;
}

rgb_color_t rgb_color_pastel(){
    rgb_color_t *pastel_blue = rgb_color_list_init(117, 199, 234);
    rgb_color_t *spanish_pink = rgb_color_list_init(242, 183, 184);
    rgb_color_t *pastel_orange = rgb_color_list_init(249, 216, 206);
    rgb_color_t *salmon_pink = rgb_color_list_init(244, 179, 206);
    rgb_color_t *pastel_yellow = rgb_color_list_init(250, 239, 215);
    rgb_color_t *pastel_aqua = rgb_color_list_init(104, 209, 197);

    list_t *pastels = list_init(6, (free_func_t) free);
    assert (pastels != NULL);

    list_add(pastels, pastel_blue);
    list_add(pastels, spanish_pink);
    list_add(pastels, pastel_orange);
    list_add(pastels, salmon_pink);
    list_add(pastels, pastel_yellow);
    list_add(pastels, pastel_aqua);

    int index = rand() % 6;
    assert (index >= 0 & index <= 5);
    rgb_color_t *my_pointer = list_get(pastels, index);
    rgb_color_t my_color = {my_pointer->r, my_pointer->g, my_pointer->b};

    list_free(pastels);
    return my_color;
}
