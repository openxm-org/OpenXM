(* $OpenXM: OpenXM/src/ox_math/lib/beta.m,v 1.1 1999/12/17 04:01:35 ohara Exp $ *)

(*
This program requires asir programs beta, igraph, sets;
these files are in OpenXM/src/asir-contrib/packages/src/.
you must add load("beta") to your .asirrc.

usage:
$ math
Mathematica 3.0 for Linux
Copyright 1988-97 Wolfram Research, Inc.
 -- Motif graphics initialized -- 

In[1]:= <<beta.m
In[2]:= OxStart["ox_asir"]
In[3]:= betaNbcBasis[ {z-t, t, t-s, s, 1-s}, {t,s} ];
Out[3]= {{{t, -s + t}, {s}}, {{t}, {1 - s}}, {{-s + t}, {1 - s}}}

---
We explains output above.
the elements of ``Out[2]'' are a differential form.
For example, {{t, -s + t}, {s}} means the following form:
( a_1 dlog(t) + a_2 dlog(-s+t) ) \wedge ( a_3 dlog(s) ),
where a_1, a_2, a_3 are exponents.
*)

toForm[polys_List, bnbc_List] := Module[
	{list1, list2, list3, i,j,k},
	list1 = {};
	For[i=1, i<=Length[bnbc], i++,
	   list2 = {};
	   For[j=1, j<=Length[bnbc[[i]]], j++,
		  list3 = {};
		  For[k=1, k<=Length[bnbc[[i]][[j]]], k++,
			 list3 = Append[list3, polys[[bnbc[[i]][[j]][[k]]]]];
			  ];
		  list2 = Append[list2, list3]
		   ];
	   list1 = Append[list1, list2]
		];
	list1
	]

betaNbcBasis[polys_List, vars_List] := Module[
	{s, arr, bnbc},
	arr = {polys, vars};
	order = Range[Length[polys]];
	s = StringJoin[	{"IG = igraph_getIGraph( ", asirRtostr[arr], " );"}];
	OxExecuteString[s];
	s = StringJoin[	{"Order = ", asirRtostr[order], ";"}];
	OxExecuteString[s];
	s = "BF = beta_betaNbcSet2forms(beta_getBetaNbcSet(IG, Order), IG, Order);";
	OxExecuteString[s];
	OxExecuteString["m_rtostr(BF);"];
	bnbc = ToExpression[OxPopString[]];
	toForm[polys, bnbc]
	]

Get[Environment["OpenXM_HOME"] <> "/lib/math/oxclient.m"]
