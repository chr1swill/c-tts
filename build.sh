#!/bin/sh

set -xe

mkdir -p dest

gcc -o dest/main main.c -g
