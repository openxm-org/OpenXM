/* $OpenXM: OpenXM/doc/OpenXM-web/smCommand.h,v 1.2 2000/01/19 00:31:38 takayama Exp $ */
/* smCommand.h */

#define SM_popSerializedLocalObject 258
#define SM_popCMO 262
#define SM_popString 263    /* result ==> string and send the string by CMO */

#define SM_mathcap 264
#define SM_pops 265
#define SM_setName 266
#define SM_evalName 267 
#define SM_executeStringByLocalParser 268 
#define SM_executeFunction 269
#define SM_beginBlock  270
#define SM_endBlock    271
#define SM_shutdown    272
#define SM_setMathCap  273
#define SM_executeStringByLocalParserInBatchMode 274
#define SM_getsp   275
#define SM_dupErrors 276
#define SM_pushCMOtag 277

#define SM_control_kill 1024
#define SM_control_reset_connection  1030
#define SM_control_to_debug_mode 1025
#define SM_control_exit_debug_mode 1026

#define  SM_PRIVATE         0x7fff0000    /*  2147418112  */
