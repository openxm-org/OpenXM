/* -*- mode: C -*- */
/* $OpenXM$ */

#include <stdio.h>
#include <unistd.h>
#include <mathlink.h>
#include <ox_toolkit.h>
#include "mlo.h"

#define CMD1 "If[True, Pause[200]; 20, -1]"
/* #define CMD1 "If[True, 20, -1]" */
// #define CMD2 "If[True, Pause[1]; SessionTime[], -1]"
#define CMD2 "Print[\"hoge\"] ; 4+5"

int fase1(char *cmd)
{
    ml_evaluateStringByLocalParser(cmd);
    sleep(1);
    ml_interrupt();
    ml_return();
    ox_printf("====\n");
}

int fase2(char *cmd)
{
    ml_evaluateStringByLocalParser(cmd);
    ml_return();
    ox_printf("====\n");
}

int main()
{
/*    ox_stderr_init(fopen("ZZ.Linux", "w+")); */
    ox_stderr_init(NULL);
    ml_init();
    fase2(CMD2);
    fase1(CMD1);
    fase2(CMD2);
    ml_exit();
}
