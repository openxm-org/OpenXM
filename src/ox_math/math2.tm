/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/math2.tm,v 1.8 2000/01/20 15:32:21 ohara Exp $ */

int OX_start P((const char *));
:Begin:
:Function: OX_start
:Pattern: OxStart[s_String]
:Arguments: {s}
:ArgumentTypes: {String}
:ReturnType: Integer
:End:

int OX_start_remote_ssh P((const char *, const char *));
:Begin:
:Function: OX_start_remote_ssh
:Pattern: OxStartRemoteSSH[s_String, host_String]
:Arguments: {s, host}
:ArgumentTypes: {String, String}
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

int OX_execute_string P((const char *));

:Begin:
:Function: OX_execute_string
:Pattern: OxExecuteString[s_String]
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

void OX_popCMO P(());

:Begin:
:Function: OX_popCMO
:Pattern: OxPopCMO[]
:Arguments: {}
:ArgumentTypes: {}
:ReturnType: Manual
:End:

void OX_get P(());

:Begin:
:Function: OX_get
:Pattern: OxGet[]
:Arguments: {}
:ArgumentTypes: {}
:ReturnType: Manual
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

