/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */
/* $Id$ */

/* サーバとの通信路を開く */
int OX_start P((const char *));

:Begin:
:Function: OX_start
:Pattern: OxStart[s_String]
:Arguments: {s}
:ArgumentTypes: {String}
:ReturnType: Integer
:End:

int OX_executeStringByLocalParser P((const char *));

:Begin:
:Function: OX_executeStringByLocalParser
:Pattern: OxExecute[s_String]
:Arguments: {s}
:ArgumentTypes: {String}
:ReturnType: Integer
:End:

char *OX_popString P(());

:Begin:
:Function: OX_popString
:Pattern: OxPopString[]
:Arguments: {}
:ArgumentTypes: {}
:ReturnType: String
:End:

int OX_close P(());

:Begin:
:Function: OX_close
:Pattern: OxClose[]
:Arguments: {}
:ArgumentTypes: {}
:ReturnType: Integer
:End:

int OX_reset P(());

:Begin:
:Function: OX_reset
:Pattern: OxReset[]
:Arguments: {}
:ArgumentTypes: {}
:ReturnType: Integer
:End:

