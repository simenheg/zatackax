#!/bin/sh

if [ $1 ]; then
    make clean 1>/dev/null;
    rel_name=zatackax-$1.src;
    mkdir $rel_name;
    cp -r AUTHORS.md COPYING NEWS.md README.md TODO.md data src \
        configure Makefile.in $rel_name;
    tar czvpf $rel_name.tar.gz $rel_name;
    tar cJvpf $rel_name.tar.xz $rel_name;
    rm -rf $rel_name;
    echo "Release" $rel_name "ready!";
else
    echo "Specify a release name.";
    echo "  Example: ./makedist.sh 0.1.3";
fi
