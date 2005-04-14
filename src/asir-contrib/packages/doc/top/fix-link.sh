#!/bin/sh
# fix-link.sh ja ~/OpenXM/bin/oxreplace
LLL=$1
OXREPLACE=$2

NEW0="[[dsolv]]"
NEW1="dsolv-html/dsolv-${LLL}_toc.html"
NEW2="dsolv (Solving the initial ideal for holonomic systems)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

NEW0="[[ok_diff]]"
NEW1="ok_diff-html/ok_diff-${LLL}_toc.html"
NEW2="ok_diff (Okutani library for differential operators)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

NEW0="[[ok_dmodule]]"
NEW1="ok_dmodule-html/ok_dmodule-${LLL}_toc.html"
NEW2="ok_dmodule (Okutani library for D-modules)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

NEW0="[[plucker]]"
NEW1="plucker-html/plucker-${LLL}_toc.html"
NEW2="plucker (Plucker relations)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

NEW0="[[pfpcoh]]"
NEW1="pfpcoh-html/pfpcoh-${LLL}_toc.html"
NEW2="pfpcoh (Ohara library for homology/cohomology groups for p F q )"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

NEW0="[[gnuplot]]"
NEW1="gnuplot-html/gnuplot-${LLL}_toc.html"
NEW2="gnuplot (gnuplot ox server for graphics)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

NEW0="[[mathematica]]"
NEW1="mathematica-html/mathematica-${LLL}_toc.html"
NEW2="mathematica (Mathematica (TM) ox server)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

NEW0="[[om]]"
NEW1="om-html/om-${LLL}_toc.html"
NEW2="om (om (java) ox server for translating CMO and OpenMath)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

NEW0="[[phc]]"
NEW1="phc-html/phc-${LLL}_toc.html"
NEW2="phc (PHC ox server for solving systems of algebraic equations by the homotopy method)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

NEW0="[[sm1]]"
NEW1="sm1-html/sm1-${LLL}_toc.html"
NEW2="sm1 (Kan/sm1 ox server for the ring of differential operators)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

NEW0="[[tigers]]"
NEW1="tigers-html/tigers-${LLL}_toc.html"
NEW2="tigers (tigers ox server for toric universal Grobner bases)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

NEW0="[[todo_parametrize]]"
NEW1="todo_parametrize-html/todo_parametrize-${LLL}_toc.html"
NEW2="todo_parametrize (Todo library for parametrizing algebraic curves)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}_toc.html

rm -f cman-ja_toc*.old

