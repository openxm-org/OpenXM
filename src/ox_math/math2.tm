/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM: OpenXM/src/ox_math/math2.tm,v 1.10 2000/02/14 09:39:12 ohara Exp $ */

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

int OX_execute_string P((int id, const char *));

:Begin:
:Function: OX_execute_string
:Pattern: OxExecuteString[id_Integer, s_String]
:Arguments: {id, s}
:ArgumentTypes: {Integer, String}
:ReturnType: Integer
:End:

int OX_parse P((int id, const char *));

:Begin:
:Function: OX_parse
:Pattern: OxParse[id_Integer, s_String]
:Arguments: {id, s}
:ArgumentTypes: {Integer, String}
:ReturnType: Integer
:End:

int OX_sendMessage P((int id, const char *));

:Begin:
:Function: OX_sendMessage
:Pattern: OxSendMessage[id_Integer, s_String]
:Arguments: {id, s}
:ArgumentTypes: {Integer, String}
:ReturnType: Integer
:End:

char *OX_popString P((int id));

:Begin:
:Function: OX_popString
:Pattern: OxPopString[id_Integer]
:Arguments: {id}
:ArgumentTypes: {Integer}
:ReturnType: String
:End:

void OX_popCMO P((int id));

:Begin:
:Function: OX_popCMO
:Pattern: OxPopCMO[id_Integer]
:Arguments: {id}
:ArgumentTypes: {Integer}
:ReturnType: Manual
:End:

void OX_get P((int id));

:Begin:
:Function: OX_get
:Pattern: OxGet[id_Integer]
:Arguments: {id}
:ArgumentTypes: {Integer}
:ReturnType: Manual
:End:

int OX_close P((int id));

:Begin:
:Function: OX_close
:Pattern: OxClose[id_Integer]
:Arguments: {id}
:ArgumentTypes: {Integer}
:ReturnType: Integer
:End:

int OX_reset P((int id));

:Begin:
:Function: OX_reset
:Pattern: OxReset[id_Integer]
:Arguments: {id}
:ArgumentTypes: {Integer}
:ReturnType: Integer
:End:

