/* $OpenXM: OpenXM/doc/OpenXM-web/oxMessageTag.h,v 1.1 2000/01/18 07:30:12 takayama Exp $ */
/* oxMessageTag.h */

#define OX_COMMAND                   513	
#define OX_DATA                      514
#define OX_SYNC_BALL                 515      /* ball to interrupt */

#define OX_DATA_WITH_SIZE              521
#define OX_DATA_ASIR_BINARY_EXPRESSION 522  /* This number should be changed*/
#define   OX_DATA_OPENMATH_XML         523
#define   OX_DATA_OPENMATH_BINARY      524
#define   OX_DATA_MP                   525

/* OX BYTE command */
#define OX_BYTE_NETWORK_BYTE_ORDER    0
#define OX_BYTE_LITTLE_ENDIAN         1
#define OX_BYTE_BIG_ENDIAN         0xff

#define OX_PRIVATE               0x7fff0000

