#!/bin/sh
# $OpenXM: OpenXM/src/util/md5.sh,v 1.1 2006/10/13 11:51:25 ohara Exp $

_freebsd () {
	if [ "`which md5`" ]; then
		md5 "$@"
	fi
}

_linux () {
	if [ "`which md5sum`" ]; then
		md5sum "$@" | sed -e 's/^\([0-9a-f]*\)  \(.*\)$/MD5 (\2) = \1/' 
	fi
}

_other () {
	if [ "`which md5sum`" ]; then
		md5sum "$@" | sed -e 's/^\([0-9a-f]*\)  \(.*\)$/MD5 (\2) = \1/'
	elif [ "`which md5`" ]; then
		md5 "$@"
	fi
}

case "`uname`" in
*BSD)  _freebsd "$@" ;;
Linux) _linux   "$@" ;;
*)     _other   "$@" ;;
esac
