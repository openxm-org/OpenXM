(* $OpenXM$ *)

Install[Environment["OpenXM_HOME"] <> "/bin/math2ox"]

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

(* The following functions are incomplete. *)
ssh[prog_String, host_String, data_Integer, control_Integer] := Module[
	{cmd, list},
	cmd = StringJoin[{
		"ssh -f ", host, " ", Environment["OpenXM_HOME"], 
		"/bin/oxlog /usr/X11R6/bin/xterm -icon -e ",
		Environment["OpenXM_HOME"], "/bin/ox -insecure -ox ",
		Environment["OpenXM_HOME"], "/bin/", prog, 
		" -data ", ToString[data], " -control ", ToString[control], 
		" -host ", host}];
	Run[cmd]
	]

oxStartRemoteSSH[prog_String, host_String] := Module[
	{},
	ssh[prog, host, 1200, 1300];
	OxStartInsecure[host, 1200, 1300]
]
