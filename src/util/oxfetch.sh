#!/bin/sh
# $OpenXM: OpenXM/src/util/oxfetch.sh,v 1.3 2003/11/16 17:20:54 ohara Exp $

MASTER_SITES="ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/"
fetch="wget --no-directories --passive-ftp --timestamping"
url=$1
distdir=${2:-.}
distinfo=$3
distfile=`basename "$url"`

_usage () {
    echo 'usage: oxfetch URL [savedir] [distinfo]'
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

# usage: cat distinfo | _md5 gc6.2.tar.gz
_md5 () {
    if [ $# -gt 0 ]; then
        grep "^MD5 ($1) =" | sed -e "s/^MD5 ($1) = //"
    fi
}

_check () {
    if [ ! -f "$distdir/$distfile" ]; then
        echo "Not found."
        exit 1
    fi
    if [ -f "$distinfo" ]; then
        key1=`(cd $distdir; md5 "$distfile" ) | _md5 $distfile`
        key2=`cat $distinfo | _md5 "$distfile"`
        if [ "$key1" = "$key2" ] ; then
            echo "Checksum OK for $distfile".
        else
            echo "Checksum mismatch for $distfile".
            exit 1
        fi
    fi
}

if [ $# -eq 0 ]; then
    _usage  
fi

_mkdir $distdir
_fetch
_check
exit 0
