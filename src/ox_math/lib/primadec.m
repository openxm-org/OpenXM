(* $OpenXM: OpenXM/src/ox_math/lib/primadec.m,v 1.3 2000/01/20 15:00:02 ohara Exp $ *)

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

primadec[polys_List, vars_List] := Block[
	{s, expr, fd},
	s = StringJoin[
		{"m_rtostr(primadec( ", asirRtostr[polys], " , ",  asirRtostr[vars], " ));"}];
	fd = OxStart["ox_asir"];
	OxExecuteString[fd, s];
	expr = ToExpression[OxPopString[fd]];
	OxClose[fd];
	Return[expr]
	]

Get[Environment["OpenXM_HOME"] <> "/lib/math/oxclient.m"]

