#include <stdio.h>
#include <stdbool.h>

#include "include/app.h"

int main(int argc, char *argv[]) {
    mach_app_t *app = mach_app_create();

    if (app) {
        mach_app_run(app);
    }

    mach_app_destroy(app);
	return 0;
}

