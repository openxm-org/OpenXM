/* -*- mode: C; coding: euc-japan -*- */
/* $OpenXM$ */
/* $Id$ */
/* 
OX  expression -> バイト列
CMO expression -> バイト列
コンバータ
*/

#include <stdio.h>
#include <stdlib.h>
#include "ox.h"
#include "parse.h"

char* dump_ox_data(char* array, ox_data* m);
char* dump_ox_command(char* array, ox_command* m);

int display(ox *m)
{
    int i;
    int len = 0;
    unsigned char* array;
    
    switch(m->tag) {
    case OX_DATA:
        len = sizeof(int) + sizeof(int) + cmolen_cmo(((ox_data *)m)->cmo);
        array = malloc(len);
        dump_ox_data(array, (ox_data *)m);
        break;
    case OX_COMMAND:
        len = sizeof(int) + sizeof(int) + sizeof(int);
        array = malloc(len);
        dump_ox_command(array, (ox_command *)m);
        break;
    default:
        len = cmolen_cmo(m);
        array = malloc(len);
        dump_cmo(array, m);
    }

    for(i=0; i<len; i++) {
        fprintf(stdout, "%02x ", array[i]);
        if(i%20==19) {
            fprintf(stdout, "\n");
        }
    }
    fprintf(stdout, "\n");
}

#define SIZE_CMDLINE  4096

char cmdline[SIZE_CMDLINE];

int prompt(char* s, int size)
{
    fprintf(stdout, "> ");
    fgets(s, size, stdin);
    setmode_mygetc(s, size);
}

int main()
{
    cmo *m;
    setbuf(stderr, NULL);
    setbuf(stdout, NULL);

    cmo_addrev = 1;
    setgetc(mygetc);

    while(1) {
        prompt(cmdline, SIZE_CMDLINE);
        if ((m = parse()) == NULL) {
            break;
        }
        display(m);
    }
    return 0;
}

