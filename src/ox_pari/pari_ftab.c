#include "ox_pari.h"

/* type=1 : one num/poly/mat arg */
/* type=2 : 1starg=num/poly/mat arg, 2ndarg=0(flag) */

struct parif parif_tab[] = {
/* (ulong)allocatemoremem(ulong) */
  {"allocatemem",(GEN (*)())allocatemoremem,0},
/* num/num */
  {"abs",gabs,1},
  {"erfc",gerfc,1},
  {"arg",garg,1},
  {"isqrt",racine,1},
  {"gamma",ggamma,1},
  {"zeta",gzeta,1},
  {"floor",gfloor,1},
  {"frac",gfrac,1},
  {"imag",gimag,1},
  {"conj",gconj,1},
  {"ceil",gceil,1},
  {"isprime",gisprime,2},
  {"bigomega",gbigomega,1},
  {"denom",denom,1},
  {"numer",numer,1},
  {"lngamma",glngamma,1},
  {"logagm",glogagm,1},
  {"classno",classno,1},
  {"classno2",classno2,1},
  {"dilog",dilog,1},
  {"disc",discsr,1},
  {"discf",discf,1},
  {"nextprime",nextprime,1},
  {"eintg1",eint1,1},
  {"eta",eta,1},
  {"issqfree",gissquarefree,1},
  {"issquare",gcarreparfait,1},
  {"gamh",ggamd,1},
  {"hclassno",classno3,1},

  /* num/array */
  {"binary",binaire,1},
  {"factorint",factorint,2},
  {"factor",Z_factor,1},
  {"cf",gcf,1},
  {"divisors",divisors,1},
  {"smallfact",smallfact,1},

  /* poly/poly */
  {"centerlift",centerlift,1},
  {"content",content,1},

  /* poly/array */
  {"galois",galois,1},
  {"roots",roots,1},
  {"factpol",factpol,1},

  /* mat/mat */
  {"adj",adj,1},
  {"lll",lll,1},
  {"lllgen",lllgen,1},
  {"lllgram",lllgram,1},
  {"lllgramgen",lllgramgen,1},
  {"lllgramint",lllgramint,1},
  {"lllgramkerim",lllgramkerim,1},
  {"lllgramkerimgen",lllgramkerimgen,1},
  {"lllint",lllint,1},
  {"lllkerim",lllkerim,1},
  {"lllkerimgen",lllkerimgen,1},
  {"trans",gtrans,1},
  {"eigen",eigen,1},
  {"hermite",hnf,1},
  {"mat",gtomat,1},
  {"matrixqz2",matrixqz2,1},
  {"matrixqz3",matrixqz3,1},
  {"hess",hess,1},
  {"ker",ker,1},
  {"keri",keri,1},
  {"kerint",kerint,1},
  {"kerintg1",kerint1,1},

  /* mat/poly */
  {"det",det,1},
  {"det2",det2,1},

  /* not examined yet */
  {"image",image,1},
  {"image2",image2,1},
  {"indexrank",indexrank,1},
  {"indsort",indexsort,1},
  {"initalg",initalg,1},
  {"isfund",gisfundamental,1},
  {"ispsp",gispsp,1},
  {"jacobi",jacobi,1},
  {"jell",jell,1},
  {"length",(GEN(*)())glength,1},
  {"lexsort",lexsort,1},
  {"lift",lift,1},
  {"lindep",lindep,1},
  {"modreverse",polymodrecip,1},
  {"mu",gmu,1},
  {"norm",gnorm,1},
  {"norml2",gnorml2,1},
  {"numdiv",numbdiv,1},
  {"omega",gomega,1},
  {"order",order,1},
  {"ordred",ordred,1},
  {"phi",phi,1},
  {"pnqn",pnqn,1},
  {"primroot",gener,1},
  {"psi",gpsi,1},
  {"quadgen",quadgen    ,1},
  {"quadpoly",quadpoly    ,1},
  {"recip",polrecip       ,1},
  {"redreal",redreal       ,1},
  {"regula",regula  ,1},
  {"reorder",reorder  ,1},
  {"rhoreal",rhoreal       ,1},
  {"sigma",sumdiv,1},
  {"signat",signat,1},
  {"simplify",simplify,1},
  {"smith",smith,1},
  {"smith2",smith2,1},
  {"sort",sort,1},
  {"sqr",gsqr,1},
  {"sqred",sqred,1},
  {"sqrt",gsqrt,1},
  {"supplement",suppl,1},
  {"trace",gtrace,1},
  {"trunc",gtrunc,1},
  {"unit",fundunit,1},
  {"wf",wf,1},
  {"wf2",wf2,1},

   /* obsolete or useless */
#if 0
  {"matsize",matsize,1},
  {"real",greal,1},
  {"round",ground,1},
  {"vec",gtovec,1},
  {"reverse",recip  ,1},
  {"polred",polred,1},
  {"polred2",polred2,1},
  {"smalldiscf",smalldiscf,1},
  {"smallpolred",smallpolred,1},
  {"smallpolred2",smallpolred2,1},
#endif
};

struct parif *search_parif(char *name)
{
  int tablen,i;

  tablen = sizeof(parif_tab)/sizeof(struct parif);
  for ( i = 0; i < tablen; i++ ) {
    if ( !strcmp(parif_tab[i].name,name) )
      return &parif_tab[i];
  }
  return 0;
}

