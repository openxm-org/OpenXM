#!/bin/sh
# $OpenXM$

if [ $# -ne 1 -o ! -d $1 ]; then
    echo 'usage: lndir.sh fromdir'
    exit
fi
base=`(cd $1 ; pwd)`

for i in `(cd $base; find . -type d -print | egrep -v '(^.$|/CVS/|/CVS$)' | sed -e s/^..//)`; do 
    mkdir -p "$i"
done
for i in `(cd $base; find . ! -type d -print | egrep -v '/CVS/' | sed -e s/^..//)` ; do 
    ln -s "$base/$i" $i
done
