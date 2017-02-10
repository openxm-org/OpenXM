## $OpenXM: OpenXM/misc/packages/Linux/Debian2/dot.bashrc,v 1.2 2016/02/12 07:37:32 takayama Exp $
## oxfetch is put under $HOME/bin for debugging.
## mdir /home/knoppix ; cd /home/knoppix ; ln -s /home/taka/.asir-tmp .asir-tmp
export PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/X11R6/bin:$HOME/bin:.
export OpenXM_HOME=`pwd`/work/OpenXM
export LD_LIBRARY_PATH=$OpenXM_HOME/lib:/usr/lib
CVS_RSH=ssh-takayama
export CVS_RSH
