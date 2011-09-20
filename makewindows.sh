#!/bin/sh

if [ $1 ]; then
    make clean 1>/dev/null;
    rel_name=zatackax-$1.win32
    mkdir $rel_name;
    cp -r AUTHORS COPYING NEWS README TODO INSTALL data src dll/* Makefile \
        $rel_name;
    cd $rel_name;
    make windows;
    mv zatackax zatackax.exe;
    rm -rf src Makefile;
    cd ..;
    zip -r $rel_name.zip $rel_name
    rm -rf $rel_name
    echo "Release" $rel_name "ready!"
else
    echo "Specify a release name."
    echo "  Example: ./makewindows.sh 0.1.3"
fi