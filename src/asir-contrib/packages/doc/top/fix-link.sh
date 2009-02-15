#!/bin/sh
# fix-link.sh ja ~/OpenXM/bin/oxreplace
# $OpenXM: OpenXM/src/asir-contrib/packages/doc/top/fix-link.sh,v 1.4 2009/02/14 08:34:38 takayama Exp $
LLL=$1
OXREPLACE=$2

NEW0="[[dsolv]]"
NEW1="dsolv-html/dsolv-${LLL}.html"
NEW2="dsolv (Solving the initial ideal for holonomic systems)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[ok_diff]]"
NEW1="ok_diff-html/ok_diff-${LLL}.html"
NEW2="ok_diff (Okutani library for differential operators)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[ok_dmodule]]"
NEW1="ok_dmodule-html/ok_dmodule-${LLL}.html"
NEW2="ok_dmodule (Okutani library for D-modules)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[plucker]]"
NEW1="plucker-html/plucker-${LLL}.html"
NEW2="plucker (Plucker relations)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[pfpcoh]]"
NEW1="pfpcoh-html/pfpcoh-${LLL}.html"
NEW2="pfpcoh (Ohara library for homology/cohomology groups for p F q )"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[gnuplot]]"
NEW1="gnuplot-html/gnuplot-${LLL}.html"
NEW2="gnuplot (gnuplot ox server for graphics)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[mathematica]]"
NEW1="mathematica-html/mathematica-${LLL}.html"
NEW2="mathematica (Mathematica (TM) ox server)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[om]]"
NEW1="om-html/om-${LLL}.html"
NEW2="om (om (java) ox server for translating CMO and OpenMath)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[phc]]"
NEW1="phc-html/phc-${LLL}/phc-${LLL}.html"
NEW2="phc (PHC ox server for solving systems of algebraic equations by the homotopy method)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[sm1]]"
NEW1="sm1-html/sm1-${LLL}.html"
NEW2="sm1 (Kan/sm1 ox server for the ring of differential operators)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[tigers]]"
NEW1="tigers-html/tigers-${LLL}.html"
NEW2="tigers (tigers ox server for toric universal Grobner bases)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[todo_parametrize]]"
NEW1="todo_parametrize-html/todo_parametrize-${LLL}.html"
NEW2="todo_parametrize (Todo library for parametrizing algebraic curves)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[f_res]]"
NEW1="f_res-html/f_res-${LLL}.html"
NEW2="f_res (computing resultants )"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[mt_graph]]"
NEW1="mk_graph-html/mk_graph-${LLL}.html"
NEW2="mt_graph (mt_graph 3D grapher)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

rm -f cman-*.old

