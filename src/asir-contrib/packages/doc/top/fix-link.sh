#!/bin/sh
# fix-link.sh ja ~/OpenXM/bin/oxreplace
# $OpenXM: OpenXM/src/asir-contrib/packages/doc/top/fix-link.sh,v 1.11 2016/10/12 02:19:17 takayama Exp $
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

NEW0="[[nn_ndbf]]"
NEW1="nn_ndbf-html/nn_ndbf-${LLL}.html"
NEW2="nn_ndbf (Local b-functioin)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[noro_mwl]]"
NEW1="noro_mwl-html/noro_mwl-${LLL}.html"
NEW2="noro_mwl (Mordel Weil Lattice)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[noro_pd]]"
NEW1="noro_pd-html/noro_pd-${LLL}.html"
NEW2="noro_pd (New Primary Ideal Decomposition)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[ns_twistedlog]]"
NEW1="ns_twistedlog-html/ns_twistedlog-${LLL}.html"
NEW2="ns_twistedlog (twisted logarithmic cohomology group)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[nk_fb_gen_c]]"
NEW1="nk_fb_gen_c-html/nk_fb_gen_c-${LLL}.html"
NEW2="nk_fb_gen_c (Fisher-Bingham MLE)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[gtt_ekn]]"
NEW1="gtt_ekn-html/gtt_ekn-${LLL}.html"
NEW2="gtt_ekn (Two way contingency tables by HGM)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[noro_module_syz]]"
NEW1="noro_module_syz-html/noro_module_syz-${LLL}.html"
NEW2="noro_module_syz (syzygies for modules)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[n_wishartd]]"
NEW1="n_wishartd-html/n_wishartd-${LLL}.html"
NEW2="n_wishartd "
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

NEW0="[[ox_pari]]"
NEW1="ox_pari-html/ox_pari-${LLL}.html"
NEW2="ox_pari (OpenXM pari server)"
${OXREPLACE} --replaceLine --old "${NEW0}" --new "<li> <a href=\"../${NEW1}\"> ${NEW2} </a>" cman-${LLL}.html

rm -f cman-*.old

