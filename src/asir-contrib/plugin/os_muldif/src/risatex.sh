#!/bin/sh
if [ ! -d $HOME/asir/tex ]; then
 echo "Creating a working folder for risatex.sh" ; \
 mkdir -p  $HOME/asir/tex ; \
fi
if [ ! -f $HOME/asir/tex/risaout.tex ]; then
  echo "Copying the template file risaout.tex" ; \
  cp ${OpenXM_HOME}/lib/asir/bin/risaout.tex $HOME/asir/tex ; \
fi
cd $HOME/asir/tex
platex -src=cr,display,hbox,math,par risaout
dvipdfmx risaout
if [ `which gnome-open` ]; then
  echo "gnome-open" ; \
  gnome-open risaout.pdf ; \
  exit 0 ; \
fi
if [  `which open` ]; then
  open risaout.pdf ; \
  exit 0 ; \
fi
if [ `which evince` ]; then
  evince risaout.pdf ; \
  exit 0 ; \
fi 
echo "Error: no PDF viewer found by risatex.sh. Please edit ${OpenXM_HOME}/lib/asir/bin/risatex.sh to set your pdf viewer." 
exit 0
# os_md.xygraph(1/(1+x^2),-32,[-6,6],0,[0,1.5]|ax=[0,0,1,1,1],scale=[12.5,20],dviout=1);
# $OpenXM$

