/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */
/* $Id$ */

/* 
ソケットで通信する場合のサーバの立ち上げ方。
dc3$ cd ~/openasir/bin
dc3$ ./ox -ox ox_sm1 -host dc3 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "ox.h"

ox_file_t sv;

int dumpx(int fd, int n)
{
    unsigned char buff[2048]; 
    int i;
    int len = read(fd, buff, n);

    fprintf(stderr, "I have read %d byte from socket.\n", len);
    for(i = 0; i < len; i++) {
        fprintf(stderr, "%02x ", buff[i]);
        if (i%20 == 19) {
            fprintf(stderr, "\n");
        }
    }
    fprintf(stderr, "\n");
    return len;
}

int mathematica_main()
{
    int fd = sv->stream;
    char* s = "Plus[31415926535, -1234567]";
    char* s2 = "Plus[1, 2]";
    char* s_result;
    cmo* m = NULL;

    fprintf(stderr, "%s\n", s2);
    ox_executeStringByLocalParser(sv, s2);

    m = ox_pop_cmo(sv, sv->stream);
    fprintf(stderr, "the tag of result is %d.\n", m->tag);

    fprintf(stderr, "executeFunction(\"Plus\", 2, zz(5), zz(3))\n");
    send_ox_cmo(sv->stream, new_cmo_zz_set_si(3));
    send_ox_cmo(sv->stream, new_cmo_zz_set_si(5));
    send_ox_cmo(sv->stream, new_cmo_int32(2));  /* number of arguments */
    send_ox_cmo(sv->stream, new_cmo_string("Plus"));
    send_ox_command(sv->stream, SM_executeFunction);

    s_result = ox_popString(sv, sv->stream);
    fprintf(stderr, "result = %s.\n", s_result);

    return;
}
  
void asir_main()
{
    cmo* m = NULL;
    int fd = sv->stream;

    send_ox_cmo(sv->stream, new_cmo_int32(12345));
    ox_executeStringByLocalParser(sv, "print(\"Hello world!!\");");
    fprintf(stderr, "result = %s.\n", ox_popString(sv, sv->stream));
    return;

    send_ox_command(fd, SM_popString);
    receive_ox_tag(fd); /* OX_DATA */
    m = receive_cmo(fd);

    m = ox_pop_cmo(sv, sv->stream);
    fprintf(stderr, "tag = %d.\n", m->tag);
}
  
void asir_main0()
{

    char* sf1 = "diff((x+2*y)^2,x);";

    /* この辺は ox_asir に実行させる場合に使う。 */
    ox_executeStringByLocalParser(sv, "print(\"Hello world!!\");");

    ox_executeStringByLocalParser(sv, "diff((x+2*y)^2,x);");
    fprintf(stderr, 
            "I let Asir evaluate \"%s\"\n and get a result \"%s\"\n", sf1, ox_popString(sv, sv->stream));
}
  
void asir_main1()
{
    send_ox_cmo(sv->stream, new_cmo_zz_set_si(10));
    send_ox_command(sv->stream, SM_popCMO);
    fprintf(stderr, "I send SM_popCMO.\n");

    dumpx(sv->stream, 17);
}
  
void asir_main2()
{
    cmo* m = NULL;

    /* add 関数の定義 */
    /* char* adddef = "def add(X,Y){return X+Y;};"; */

    /* この辺は ox_asir に実行させる場合に使う。 */
    /*  ox_executeStringByLocalParser(sv, adddef); */
    send_ox_cmo(sv->stream, new_cmo_int32(3));
    send_ox_cmo(sv->stream, new_cmo_int32(5));
    send_ox_cmo(sv->stream, new_cmo_int32(2));  /* number of arguments */
    send_ox_cmo(sv->stream, new_cmo_string("igcd"));
    send_ox_command(sv->stream, SM_executeFunction);
    /* この段階でCMO_ERROR2 が積まれるはず */

    send_ox_command(sv->stream, SM_popCMO);

#if 0
    fprintf(stderr, "dump:\n");
    dumpx(sv->stream, 36);
    return;
#endif

    receive_ox_tag(sv->stream);    /* OX_DATA */
    m = receive_cmo(sv->stream);
    fprintf(stderr, "tag is (%d).\n", m->tag);
}

void asir_main3()
{
    int i1, i2;
    cmo_zz* z1;
    cmo_zz* z2;

    i1 = 2*7;
    i2 = 2*11;

    z1 = new_cmo_zz_set_si(i1);
    z2 = new_cmo_zz_set_si(i2);
    fprintf(stderr, "i1 = %d, i2 = %d.\n", i1, i2);
  
    send_ox_cmo(sv->stream, z1);
    send_ox_cmo(sv->stream, z2);
    send_ox_cmo(sv->stream, new_cmo_int32(2));  /* number of arguments */
    send_ox_cmo(sv->stream, new_cmo_string("igcd"));
    send_ox_command(sv->stream, SM_executeFunction);

    fprintf(stderr, "result1 = (%s).\n", ox_popString(sv, sv->stream));
}

void sm_main()
{
    ox_mathcap(sv);
}

/* 平成11年7月10日 */
#define VERSION 0x11071000
#define ID_STRING  "client version 0.11071000"

int prompt()
{
    printf(":- ");
}


int main(int argc, char* argv[])
{
    ox* m = NULL;
    cmo* c = NULL;
    int code;

    setbuf(stderr, NULL);

    if (argc>1) {
        fprintf(stderr, "I use ox_math.\n");
        sv = ox_start("localhost", "ox", "ox_math"); 
        ox_mathcap(sv);
        send_ox_cmo(sv->stream, make_mathcap_object(VERSION, ID_STRING));
    }else {
        fprintf(stderr, "I use ox_sm1.\n");
        sv = ox_start("localhost", "ox", "ox_sm1");
    }

    ox_reset(sv);
    send_ox_cmo(sv->stream, new_cmo_string("N[ArcTan[1]]"));
    send_ox_command(sv->stream, SM_executeStringByLocalParser);
    send_ox_command(sv->stream, SM_popCMO);
    fprintf(stderr, "for debug.\n");

    receive_ox_tag(sv->stream);
    fprintf(stderr, "for debug.\n");
    c = receive_cmo(sv->stream);
    fprintf(stderr, "for debug.\n");
    fprintf(stderr, "local:: cmo received.\n");
    print_cmo(c);

    while(prompt(), (m = parse()) != NULL) {
        send_ox(sv, m);
        if (m->tag == OX_COMMAND) {
            code = ((ox_command *)m)->command;
            if (code >= 1024) {
                break;
            }else if (code == SM_popCMO || code == SM_popString) {
                receive_ox_tag(sv->stream);
                c = receive_cmo(sv->stream);
                fprintf(stderr, "local:: cmo received.\n");
                print_cmo(c);
            }
        }
    }

#if 0
    sm_main();
    asir_main();
    mathematica_main();
#endif

    ox_reset(sv);
    fprintf(stderr, "The client resets.\n");
    ox_close(sv);
    fprintf(stderr, "The client halts.\n");

    return 0;
}
