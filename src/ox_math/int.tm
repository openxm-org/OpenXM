/* -*- mode: C -*- */
/* $OpenXM$ */

int INT_addto P((int x, int y));

:Begin:
:Function: INT_addto
:Pattern: IntAddto[x_Integer, y_Integer]
:Arguments: {x, y}
:ArgumentTypes: {Integer, Integer}
:ReturnType: Integer
:End:

int INT_addtoo P((int x, int y));

:Begin:
:Function: INT_addtoo
:Pattern: IntAddtoo[x_Integer, y_Integer]
:Arguments: {x, y}
:ArgumentTypes: {Integer, Integer}
:ReturnType: Integer
:End:

int INT_block P((int n));

:Begin:
:Function: INT_block
:Pattern: IntBlock[n_Integer]
:Arguments: {n}
:ArgumentTypes: {Integer}
:ReturnType: Integer
:End:

int INT_signal P((int n));

:Begin:
:Function: INT_signal
:Pattern: IntSignal[n_Integer]
:Arguments: {n}
:ArgumentTypes: {Integer}
:ReturnType: Integer
:End:
