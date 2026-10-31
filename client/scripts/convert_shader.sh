#/bin/sh

xxd -i ../res/shaders/$1 > ./temp.h
if [ $? -ne 0 ]; then
    echo "ERROR: Couldn't convert shader $1!"
    exit 1
fi

mv ./temp.h $2

