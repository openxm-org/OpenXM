/* $OpenXM$ */
struct object KsocketOpen(struct object obj);
/* [  optional integer port default 0, optional string host default localhost]
   [ integer socketid, integer port ]
   socket, bind, listen */
struct object KsocketConnect(struct object obj);
/* [ integer port, optional string host default localhost]
   [ integer socketid, integer port ]
   socket, connect */
struct object KsocketAccept(struct object obj);
/* [ integer socketid ]
   [ integer newsocketid ]
   accept */
int KsocketSelect0(int fd,int timeout);
struct object KsocketSelect(struct object obj);
/* [ integer socketid optional integer timeout default 0]
   integer true or false
*/
struct object KsocketSelectMulti(struct object obj);
/* [ [integer socketid1, integer socketid2, ...] 
     optional integer timeout default 0]
   [ result1, result2, ....]
*/
struct object KsocketRead(struct object obj);
/* [ integer socketid ]
   string data 
*/
struct object KsocketWrite(struct object obj);
/* [ integer socketid, string data ]
   integer
*/
struct object KsocketClose(struct object obj);
/* [ integer socketid ]
   integer ok or error
*/

struct object Kplugin_sm1Socket(char *key,struct object obj);

