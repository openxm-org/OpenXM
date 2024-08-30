/* -*- mode: C -*- */
/* $OpenXM: OpenXM/src/ox_math/mlo.c,v 1.19 2003/03/30 08:07:10 ohara Exp $ */

/* 
   Copyright (C) Katsuyoshi OHARA, 2000.
   Portions copyright 1999 Wolfram Research, Inc. 

   You must see OpenXM/Copyright/Copyright.generic.
   The MathLink Library is licensed from Wolfram Research Inc..
   See OpenXM/Copyright/Copyright.mathlink for detail.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mathlink.h>
#include <ox_toolkit.h>
#include "mlo.h"
#include "sm.h"

static int send_mlo_int32(cmo *m);
static int send_mlo_string(cmo *m);
static int send_mlo_zz(cmo *m);
static int send_mlo_list(cmo *c);

static mlo *ml_read_returnpacket();
static int ml_read_menupacket();
static int ml_read_textpacket();
static int ml_clear_interruption();
static int ml_clear_abortion();
static mlo *ml_return0();

static int ml_current_packet = -1;

static double mathkernel_version;
static char *mathkernel_versionstring = NULL;

/* MLDisownString was replaced by MLReleaseString */
#define MLDisownString(a,b) (MLReleaseString((a),(b)))

/* If this flag sets then we identify MLTKSYM to CMO_INDETERMINATE. */
int flag_mlo_symbol = FLAG_MLTKSYM_IS_INDETERMINATE;

/* MLINK is an identifier of MathLink connection. */
MLINK stdlink;

static unsigned flag_ml_state = 0;

/* state management for the OpenXM robust interruption */
unsigned ml_state_set(unsigned fl)
{
    return flag_ml_state |= fl;
}

unsigned ml_state_clear(unsigned fl)
{
    return flag_ml_state &= ~fl;
}

unsigned ml_state(unsigned fl)
{
    return (flag_ml_state & fl);
}

void ml_state_clear_all()
{
    flag_ml_state = 0;
}

mlo *receive_mlo_real()
{
    double d;
    MLGetReal(stdlink, &d);
    ox_printf("%lf", d);
    return new_cmo_double(d);
}

mlo *receive_mlo_error()
{
    int errcode = MLError(stdlink);
    char *s = MLErrorMessage(stdlink);
    MLClearError(stdlink);
    ox_printf("MLTKERR(%d,\"%s\")", errcode, s);
    return (cmo *)make_error_object(errcode, new_cmo_string(s));
}

mlo *receive_mlo_zz()
{
    char *s;
    mlo  *m;

    MLGetString(stdlink, &s);
    ox_printf("%s", s);
    m = (mlo *)new_cmo_zz_set_string(s);
    MLDisownString(stdlink, s);
    return m;
}

mlo *receive_mlo_string()
{
    char *s;
    mlo  *m;
    MLGetString(stdlink, &s);
    ox_printf("\"%s\"", s);
    m = (cmo *)new_cmo_string(s);
    MLDisownString(stdlink, s);
    return m;
}

cmo *receive_mlo_function()
{
    char *s;
    cmo *m;
    cmo  *ob;
    int  i,n;

    MLGetFunction(stdlink, &s, &n);
    ox_printf("%s#%d[", s, n);
    m = (cmo *)new_cmo_list();
    list_append((cmo_list *)m, new_cmo_string(s));

    for (i=0; i<n; i++) {
        ob = receive_mlo();
        ox_printf(", ");
        list_append((cmo_list *)m, ob);
    }
    ox_printf("]");
    MLDisownString(stdlink, s);
    return m;
}

#if 0
cmo *convert_mlo_to_cmo(mlo *m)
{
	if (m->tag == MLO_FUNCTION) {
		if (strcmp(((mlo_function *)m)->function, "List") == 0) {
			return convert_mlo_function_list_to_cmo_list(m);
		}
	}
	return m;
}
#endif

#define MLO_FUNCTION   (CMO_PRIVATE+1)

mlo_function *new_mlo_function(char *function)
{
    mlo_function *c = malloc(sizeof(mlo_function));
    c->tag = MLO_FUNCTION;
    c->length = 0;
    c->head->next = NULL;
    c->function = function;
    return c;
}

cmo *receive_mlo_function_newer()
{
    char *s;
    mlo_function *m;
    cmo  *ob;
    int  i,n;

    MLGetFunction(stdlink, &s, &n);
    ox_printf("%s#%d[", s, n);
    m = new_mlo_function(s);
    for (i=0; i<n; i++) {
        ob = receive_mlo();
        ox_printf(", ");
        list_append((cmo_list *)m, ob);
    }
    ox_printf("]");

    MLDisownString(stdlink, s);
    return (cmo *)m;
}

cmo *receive_mlo_symbol()
{
    cmo *ob;
    char *s;

    MLGetSymbol(stdlink, &s);
    ox_printf("MLTKSYM(%s)", s);
    if(flag_mlo_symbol == FLAG_MLTKSYM_IS_INDETERMINATE) {
        ob = (cmo *)new_cmo_indeterminate((cmo *)new_cmo_string(s));
    }else {
        ob = (cmo *)new_cmo_string(s);
    }
    MLDisownString(stdlink, s);
    return ob;
}

/* starting a MathLink connection. */
int ml_init()
{
    int argc = 2;
    char *argv[] = {"-linkname", "math -mathlink"};

    if(MLInitialize(NULL) == NULL
       || (stdlink = MLOpen(argc, argv)) == NULL) {
        ox_printf("Mathematica Kernel not found.\n");
        exit(1);
    }
    /* set the version of Mathematica kernel. */
    ml_evaluateStringByLocalParser("$VersionNumber");
    mathkernel_version = ((cmo_double *)ml_return())->d;
    ml_evaluateStringByLocalParser("$Version");
    mathkernel_versionstring = ((cmo_string *)ml_return())->s;
    ox_printf("Mathematica %lf <%s>\n", 
              mathkernel_version, mathkernel_versionstring);
    return 0;
}

/* closing a MathLink connection. */
int ml_exit()
{
    /* quit Mathematica then close the link */
    MLPutFunction(stdlink, "Exit", 0);
    MLClose(stdlink);
}

/* Remember calling ml_select() before ml_return(). */
int ml_select()
{
    int i=0;
    MLFlush(stdlink);
    while(!MLReady(stdlink)) {
        if (i==0 && ml_state(RESERVE_INTERRUPTION)) {
            ml_interrupt();
            i++;
        }
        usleep(10);
    }
}

/* Never forget call ml_flush() after calling send_mlo(). */
int ml_flush()
{
    MLEndPacket(stdlink);
}

cmo *receive_mlo()
{
    int type = MLGetNext(stdlink);

    switch(type) {
    case MLTKINT:
        return (cmo *)receive_mlo_zz();
    case MLTKSTR:
        return (cmo *)receive_mlo_string();
    case MLTKREAL:
        return (cmo *)receive_mlo_real();
    case MLTKSYM:
        return (cmo *)receive_mlo_symbol();
    case MLTKFUNC:
        return (cmo *)receive_mlo_function();
    case MLTKERR:
        return (cmo *)receive_mlo_error();
    default:
        ox_printf("broken MLO\(%d)", type);
        return NULL;
    }
}

static int send_mlo_int32(cmo *m)
{
    MLPutInteger(stdlink, ((cmo_int32 *)m)->i);
}

static int send_mlo_string(cmo *m)
{
    char *s = ((cmo_string *)m)->s;
    MLPutString(stdlink, s);
}

static int send_mlo_zz(cmo *m)
{
    char *s;
    MLPutFunction(stdlink, "ToExpression", 1);
    s = new_string_set_cmo(m);
    MLPutString(stdlink, s);
}

static int send_mlo_list(cmo *c)
{
    cell *cp = list_first((cmo_list *)c);
    int len = list_length((cmo_list *)c);

    MLPutFunction(stdlink, "List", len);
    while(!list_endof(c, cp)) {
        send_mlo(cp->cmo);
        cp = list_next(cp);
    }
}

int send_mlo(cmo *m)
{
    switch(m->tag) {
    case CMO_INT32:
        send_mlo_int32(m);
        break;
    case CMO_ZERO:
    case CMO_NULL:
        send_mlo_int32(new_cmo_int32(0));
        break;
    case CMO_STRING:
        send_mlo_string(m);
        break;
    case CMO_LIST:
        send_mlo_list(m);
        break;
    case CMO_MATHCAP:
        send_mlo(((cmo_mathcap *)m)->ob);
        break;
    case CMO_ZZ:
        send_mlo_zz(m);
        break;
    default:
        MLPutFunction(stdlink, "ToExpression", 1);
        MLPutString(stdlink, new_string_set_cmo(m));
        break;
    }
}

int ml_evaluateStringByLocalParser(char *str)
{
    ox_printf("ox_evaluateString(%s)\n", str);
    MLPutFunction(stdlink, "EvaluatePacket", 1);
    MLPutFunction(stdlink, "ToExpression", 1);
    MLPutString(stdlink, str);
    MLEndPacket(stdlink);
}

int ml_executeFunction(char *function, int argc, cmo *argv[])
{
    int i;
    MLPutFunction(stdlink, "EvaluatePacket", 1);
    MLPutFunction(stdlink, function, argc);
    for (i=0; i<argc; i++) {
        send_mlo(argv[i]);
    }
    MLEndPacket(stdlink);
}

int ml_next_packet()
{
    if (ml_current_packet < 0) {
        ml_current_packet = MLNextPacket(stdlink);
        ox_printf("PKT=%d ", ml_current_packet);
    }
    return ml_current_packet;
}

int ml_new_packet()
{
    ml_current_packet = -1;
    MLNewPacket(stdlink);
}

/* Remember calling ml_new_packet() after ml_read_packet(). */
int ml_read_packet()
{
    int pkt = ml_next_packet();
    switch(pkt) {
    case MENUPKT:
        ml_read_menupacket();
        break;
    case TEXTPKT:
        ml_read_textpacket();
        break;
    case RETURNPKT:
        ml_read_returnpacket();
        break;
    case INPUTNAMEPKT:
        ox_printf("INPUTNAMEPKT[]");
        break;
    case ILLEGALPKT:
        ox_printf("ILLEGALPKT[]");
        break;
    case SUSPENDPKT:
        ox_printf("SUSPENDPKT[]");
        break;
    case RESUMEPKT:
        ox_printf("RESUMEPKT[]");
        break;
    default:
        ;
    }
    ox_printf("\n");
    return pkt;
}

static mlo *ml_read_returnpacket()
{
    mlo *ob;
    ox_printf("RETURNPKT[");
    ob=receive_mlo();
    ox_printf("]");

    return ob;
}

static int ml_read_menupacket()
{
    ox_printf("MENUPKT[");
    receive_mlo();
    ox_printf(", ");
    receive_mlo();
    ox_printf("]");
}

static int ml_read_textpacket()
{
    char *s;
    int type = MLGetNext(stdlink);
    if (type == MLTKSTR) {
        MLGetString(stdlink, &s);
        ox_printf("TEXTPKT[\"%s\"]", s);
        MLDisownString(stdlink, s);
    }else {
        ox_printf("TEXTPKT is broken? (%d)", type);
    }
}

/* References:
[1] Todd Gayley: "Re: How to interrupt a running evaluation in MathLink",
http://forums.wolfram.com/mathgroup/archive/1999/Apr/msg00174.html

From: tgayley@linkobjects.com (Todd Gayley)
To: mathgroup@smc.vnet.net
Subject: [mg17015] Re: How to interrupt a running evaluation in MathLink
*/

int ml_interrupt()
{
    /* On UNIX, the MLPutMessage(process, MLInterruptMessage)
       sends ``SIGINT" to the process running on the local machine. */
    MLPutMessage(stdlink, MLInterruptMessage);
    ml_state_set(INTERRUPTED);
}

/* Remark:
read MENUPKT[MLTKINT(1), MLTKSTR("Interrupt> ")]
write "\n"
read MENUPKT[MLTKINT(0), MLTKSTR("Interrupt> ")]
write "a"
read TEXTPKT[Your options are:
        abort (or a) to abort current calculation
        continue (or c) to continue
        exit (or quit) to exit Mathematica
        inspect (or i) to enter an interactive dialog
        show (or s) to show current operation (and then continue)
        trace (or t) to show all operations
]
*/

static int ml_clear_interruption()
{
    if (ml_read_packet() == MENUPKT) {
        MLPutString(stdlink, "\n");
        ox_printf("MLPutString(\"\\n\");\n");
        ml_new_packet();
        if(ml_read_packet() == MENUPKT) {
            MLPutString(stdlink, "a");
            ox_printf("MLPutString(\"a\");\n");
            ml_new_packet();
            if(ml_read_packet() == TEXTPKT) {
                ml_new_packet();
                ox_printf("\n---END of ml_clear_interruption()---\n");
                return 0; /* success */
            }
        }
    }
    ml_new_packet();
    ox_printf("Ooops!\n");
    return -1;
}

int ml_abort()
{
    MLPutMessage(stdlink, MLAbortMessage);
    ml_state_set(ABORTED);
}

/* broken */
static int ml_clear_abortion()
{
    while(ml_read_packet()==MENUPKT) {
        ml_new_packet();
    }
    MLPutString(stdlink, "a");
    ml_new_packet();
    ox_printf("aborted.\n");
    if (MLError(stdlink)) {
        ox_printf("MLError=%s\n", MLErrorMessage(stdlink));
    }
    receive_mlo();
    ml_state_clear_all();
}

static mlo *ml_return0()
{
    mlo *ob;
    int pkt;
    /* seeking to RETURNPKT */
    while((pkt = ml_next_packet()) != RETURNPKT) {
        if (pkt == ILLEGALPKT) {
            ob = receive_mlo_error();
            ml_new_packet(); /* OK? */
            return ob;
        }
        ml_read_packet(); /* debug only */
        ml_new_packet();
    }
    ob = ml_read_returnpacket();
    ml_new_packet();
    ox_printf("\n---END of ml_return0()---\n");
    return ob;
}

mlo *ml_return()
{
    mlo *ob;
    if (ml_state(INTERRUPTED)) { 
        if (ml_next_packet() == RETURNPKT) {
            /* a computation has done before the interruption */
            ob = ml_return0();
            ml_clear_interruption();
        }else {
            ml_clear_interruption();
            MLFlush(stdlink);                    /* need for 4.x */
            ob = ml_return0();                   /* ReturnPacket[$Aborted] */
        }
    }else {
        ob = ml_return0();
    }
    return ob;
}
