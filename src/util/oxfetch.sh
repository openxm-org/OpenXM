#!/bin/sh
# $OpenXM: OpenXM/src/util/oxfetch.sh,v 1.4 2004/06/30 10:14:10 ohara Exp $

MASTER_SITES="ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/"
fetch="wget --no-directories --passive-ftp --timestamping"
url=$1
distdir=${2:-.}
distinfo=${3:-./distinfo}
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

# usage: _md5grep gc6.2.tar.gz distinfo
_md5grep () {
    if [ $# -gt 0 ]; then
        awk '/^MD5 \('"$1"'\) =/{print $NF}' $2
    fi
}

_md5sum () {
    if [ "`which md5`" ]; then
        md5 "$1" | awk '{print $NF}'
    elif [ "`which md5sum`" ]; then
        md5sum "$1" | awk '{print $1}'
    fi
}

_check () {
    if [ ! -f "$distdir/$distfile" ]; then
        echo "Error: ${distfile} not found."
        exit 1
    fi
    if [ -f "$distinfo" ]; then
        key1=`_md5sum  "${distdir}/${distfile}"`
        key2=`_md5grep "${distfile}" ${distinfo}`
        if [ "$key1" = "$key2" ] ; then
            echo "Checksum OK for $distfile".
        elif [ -z "$key1" ]; then 
            echo "Warnig: no md5 checker."
        else
            echo "Error: checksum mismatch for $distfile".
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
