/* $OpenXM: OpenXM/src/k097/startup.k,v 1.3 2001/01/04 12:29:31 takayama Exp $ */
/*  startup.k */
Println("Loading startup files (startup.k)   2000, 1/3.");
sm1("(sm1 version = ) messagen"); sm1("[(Version)] system_variable message ");
load("help.k");  if (K00_verbose)   Println(".... Help module.");
load("man.k");  if (K00_verbose)   Println(".... Manual module.");
load("debug/db.k"); if (K00_verbose) Println(".... Debugger module.");
load("debug/asir0.k"); if (K00_verbose) Println(".... Factor and Cancel.");
load("object.kk"); 
sm1(" [(KanGBmessage) 0] system_variable ");

