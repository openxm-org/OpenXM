/* $OpenXM$ */
#define MATHCAP_SIZE  1024
struct mathCap {
  void *infop;
  int n;
  int cmo[MATHCAP_SIZE];
  int oxSize;
  int ox[MATHCAP_SIZE];
  int smSize;
  int sm[MATHCAP_SIZE];
};
/*  If you change the format of mathcap, do the follows.
    Mofify  cmoCheckMathCap in oxmisc2.c,
    oxReq, SM_setMathCap:, and
    grep mathCap and make all modifications.
*/

