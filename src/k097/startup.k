/*  startup.k */
Println("Loading startup files (startup.k)   1997, 3/11.");
sm1("(sm1 version = ) messagen"); sm1("[(Version)] system_variable message ");
load("help.k");  if (K00_verbose)   Println(".... Help module.");
load("debug/db.k"); if (K00_verbose) Println(".... Debugger module.");
load("debug/asir0.k"); if (K00_verbose) Println(".... Factor and Cancel.");
load("object.kk"); 
sm1(" [(KanGBmessage) 0] system_variable ");

