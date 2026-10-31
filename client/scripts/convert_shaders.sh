#/bin/sh

# Helper function to convert shaders from GLSL files to C header files
convert_shader() {
    echo "Converting shader at path: $1..."

    xxd -i ../res/shaders/$1 > ../src/include/shaders/$2
    if [ $? -ne 0 ]; then
        echo "ERROR: Couldn't convert shader $1! (Attempted output at $2)"
        exit 1
    fi
}

echo ""

# The actual shaders
convert_shader "square.vert" "square_vert.h"
convert_shader "square.frag" "square_frag.h"

