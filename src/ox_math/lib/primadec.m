(* $OpenXM: OpenXM/src/ox_math/lib/primadec.m,v 1.2 1999/12/17 02:21:41 ohara Exp $ *)

(*
usage:
$ math
Mathematica 3.0 for Linux
Copyright 1988-97 Wolfram Research, Inc.
 -- Motif graphics initialized -- 

In[1]:= <<primadec.m
In[2]:= OxStart["ox_asir"]
In[3]:= primadec[{x^2-3*x+2},{x}]
Out[3]= {{{-1 + x}, {-1 + x}}, {{-2 + x}, {-2 + x}}}
*)

primadec[polys_List, vars_List] := Block[
	{s},
	s = StringJoin[
		{"m_rtostr(primadec( ", asirRtostr[polys], " , ",  asirRtostr[vars], " ));"}];
	OxExecuteString[s];
	ToExpression[OxPopString[]]
	]

Get[Environment["OpenXM_HOME"] <> "/lib/math/oxclient.m"]

