/* ox_message_tag.h */
/* If you add new OX_DATA_your_new_format,
   you need to change KSmathCapByStruct() in cmo.c
*/

#define OX_COMMAND                   513	
#define OX_DATA                      514
#define OX_SYNC_BALL                 515      /* ball to interrupt */

/* level 2 stack machine with programming facility. */
#define OX_START_OF_FUNCTION_BLOCK    518
#define OX_END_OF_FUNCTION_BLOCK      519
#define OX_ADDRESS                    520

#define OX_DATA_WITH_SIZE              521
#define OX_DATA_ASIR_BINARY_EXPRESSION 522  /* This number should be changed*/
#define   OX_DATA_OPENMATH_XML         523
#define   OX_DATA_OPENMATH_BINARY      524
#define   OX_DATA_MP                   525

/* OX BYTE command */
#define OX_BYTE_NETWORK_BYTE_ORDER    0
#define OX_BYTE_LITTLE_INDIAN         1
#define OX_BYTE_BIG_INDIAN         0xff

