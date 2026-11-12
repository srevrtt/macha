#include <stdlib.h>

#include "include/utils.h"

/* creates a CGLM-compatible vec3 (float[3]) representation of a color in
   hexadecimal form; make sure to free when done since this is heap memory */
void mach_color_from_hex(unsigned int color, float *vec3) {
    vec3[0] = ((color >> 16) & 0xFF) / 255.0f; // red
    vec3[1] = ((color >> 8) & 0xFF) / 255.0f; // green
    vec3[2] = (color & 0xFF) / 255.0f; // blue
}

/* creates a CGLM-compatible vec3 (float[3]) representation of a color in RGB
   form; make sure to free when done since this is heap memory */
void mach_color_from_rgb(int r, int g, int b, float *vec3) {
    vec3[0] = r;
    vec3[1] = g;
    vec3[2] = b;
}

