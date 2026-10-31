#include <stdio.h>
#include <stdbool.h>

#include "include/app.h"
#include "include/shaders/square.vert.h"

int main(int argc, char *argv[]) {
    mach_app_t *app = mach_app_create();

    mach_app_run(app);
    mach_app_destroy(app);

	  return 0;
}

