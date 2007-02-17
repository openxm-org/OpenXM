#!/bin/sh
# $OpenXM: OpenXM/src/util/md5.sh,v 1.2 2006/10/14 08:30:26 ohara Exp $

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
*BSD|Darwin)  _freebsd "$@" ;;
Linux) _linux   "$@" ;;
*)     _other   "$@" ;;
esac
