#!/bin/sh
cp jpeg_header.out $1
dd if=$2 of=$1 bs=1 seek=613
cat jpeg_footer.out >> $1
