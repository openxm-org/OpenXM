#!/bin/sh
# $OpenXM: OpenXM/src/util/oxfetch.sh,v 1.1 2003/11/14 02:58:20 ohara Exp $

fetch="wget --no-directories --passive-ftp --quiet --timestamping"
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
    if [ -f "$distinfo" ]; then
        key1=`(cd $distdir; md5 "$distfile" ) | _md5 $distfile`
        key2=`cat $distinfo | _md5 "$distfile"`
        if [ "$key1" = "$key2" ] ; then
            echo "Checksum OK for $distfile".
        else
            echo "Checksum mismatch for $distfile".
        fi
    fi
}

if [ $# -eq 0 ]; then
    _usage  
fi

_mkdir $distdir
_fetch
_check
