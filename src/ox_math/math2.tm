/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/math2.tm,v 1.3 1999/11/04 17:53:04 ohara Exp $ */
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

int OX_start_insecure P ((const char *, int, int));
:Begin:
:Function: OX_start_insecure
:Pattern: OxStartInsecure[host_String, ctl_Integer, dat_Integer]
:Arguments: {host, ctl, dat}
:ArgumentTypes: {String, Integer, Integer}
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

int OX_parse P((const char *));

:Begin:
:Function: OX_parse
:Pattern: OxParse[s_String]
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

