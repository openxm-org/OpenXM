(* $OpenXM: OpenXM/src/ox_math/lib/primadec.m,v 1.1 1999/12/15 03:40:36 ohara Exp $ *)

(*
usage:
$ math
Mathematica 3.0 for Linux
Copyright 1988-97 Wolfram Research, Inc.
 -- Motif graphics initialized -- 

In[1]:= <<primadec.m
In[2]:= primadec[{x^2-3*x+2},{x}]
Out[2]= {{{-1 + x}, {-1 + x}}, {{-2 + x}, {-2 + x}}}
*)

asirRtostr[x_] := Module[
	{list = {}, i, s},
	If[ x[[0]] === List,
	  list = Append[list, " [ " ];
	  If[ Length[x] > 0 ,
		list = Append[list, asirRtostr[ x[[1] ] ] ];
		For[i=2, i<= Length[x], i++, 
		   list = Append[list, " , "];
		   list = Append[list, asirRtostr[ x[[i]] ] ]
			]
		  ];
	  list = Append[list, " ] "];
	  s = StringJoin[list],
	  s = ToString[x, InputForm]
		]
	]

primadec[polys_List, vars_List] := Block[
	{s},
	s = StringJoin[
		{"m_rtostr(primadec( ", asirRtostr[polys], " , ",  asirRtostr[vars], " ));"}];
	OxExecuteString[s];
	ToExpression[OxPopString[]]
	]

Install[Environment["OpenXM_HOME"] <> "/bin/math2ox"]
OxStart["ox_asir"]
