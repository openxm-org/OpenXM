#!/bin/sh
# $OpenXM$

fetch="wget --no-directories --passive-ftp --quiet --timestamping"
url=$1
distdir=${2:-.}
md5file=$3
distfile=`basename "$url"`

_usage () {
    echo 'usage: oxfetch URL [savedir] [md5file]'
    exit 1
}

_mkdir () {
    if [ ! -d "$1" ]; then
        mkdir -p "$1"
    fi
}

_fetch () {
    if [ "distfile" != "" -a ! -f "$distdir/$distfile" ]; then
        (cd $distdir; $fetch $url)
    fi
}

_check () {
    if [ -f "$md5file" ]; then
        (cd $distdir; md5 "$distfile" > "md5.$distfile" )
        if cmp "$distdir/md5.$distfile" "$md5file" ; then
            echo "Checksum OK for $distfile".
        else
            echo "Checksum mismatch for $distfile".
        fi
        rm -f "$distdir/md5.$distfile"
    fi
}

if [ $# -eq 0 ]; then
    _usage  
fi

_mkdir $distdir
_fetch
_check
