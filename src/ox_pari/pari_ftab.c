#include "ox_pari.h"

/* type=1 : one num/poly/mat arg with prec */
/* type=2 : 1starg=num/poly/mat arg, 2ndarg(int) */
/* type=3 : one GEN, return ulong */

struct parif parif_tab[] = {
/* num/num */
  {"abs",gabs,1},
  {"erfc",gerfc,1},
  {"arg",garg,1},
  {"isqrt",sqrtint,1},
  {"gamma",ggamma,1},
  {"zeta",gzeta,1},
  {"floor",gfloor,1},
  {"frac",gfrac,1},
  {"imag",gimag,1},
  {"conj",gconj,1},
  {"ceil",gceil,1},
  {"denom",denom,1},
  {"numer",numer,1},
  {"lngamma",glngamma,1},
  {"logagm",glog,1},
  {"classno",classno,1},
  {"dilog",dilog,1},
  {"discf",nfdisc,1},
  {"nextprime",nextprime,1},
  {"eintg1",eint1,1},
  {"eta",eta,1},
  {"gamh",ggamd,1},
  {"hclassno",hclassno,1},

  /* num/array */
  {"binary",binaire,1},
  {"factor",factor,1},
  {"cf",gcf,1},
  {"divisors",divisors,1},
  {"smallfact",factor,1},

  /* poly/poly */
  {"centerlift",centerlift,1},
  {"content",content,1},

  /* poly/array */
  {"galois",polgalois,1},
  {"roots",roots,1},
  {"factpol",factor,1},

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
  {"hess",hess,1},
  {"ker",ker,1},
  {"keri",keri,1},
  {"kerint",kerint,1},

  /* mat/poly */
  {"det",det,1},
  {"det2",det2,1},

  {"factorint",factorint,2,0},
  {"isprime",gisprime,2,0},
  {"disc",poldisc0,2,-1},

  {"bigomega",(GEN(*)())bigomega,3},
  {"issqfree",(GEN(*)())issquarefree,3},
  {"isfund",(GEN(*)())isfundamental,3},
  {"ispsp",(GEN(*)())BPSW_psp,3}, // miller-rabin

#if 1
  /* not examined yet */
  {"image",image,1},
  {"image2",image2,1},
  {"indexrank",indexrank,1},
  {"indsort",indexsort,1},
  {"initalg",initalg,1},
  {"jacobi",jacobi,1},
  {"jell",jell,1},
  {"length",(GEN(*)())glength,1},
  {"lexsort",lexsort,1},
  {"lift",lift,1},
  {"lindep",lindep,1},
  {"modreverse",polymodrecip,1},
  {"norm",gnorm,1},
  {"norml2",gnorml2,1},
  {"numdiv",numbdiv,1},
  {"omega",(GEN(*)())omega,1},
  {"order",order,1},
  {"pnqn",pnqn,1},
  {"psi",gpsi,1},
  {"quadgen",quadgen    ,1},
  {"quadpoly",quadpoly    ,1},
  {"recip",polrecip       ,1},
  {"redreal",redreal       ,1},
  {"regula",quadregulator  ,1},
  {"rhoreal",rhoreal       ,1},
  {"sigma",sumdiv,1},
  {"signat",qfsign,1},
  {"simplify",simplify,1},
  {"smith",smith,1},
  {"smith2",smith2,1},
  {"sort",sort,1},
  {"sqr",gsqr,1},
  {"sqred",qfgaussred,1},
  {"sqrt",gsqrt,1},
  {"supplement",suppl,1},
  {"trace",gtrace,1},
  {"trunc",gtrunc,1},
  {"unit",quadunit,1},
#endif

   /* obsolete or useless */
#if 0
  {"reorder",reorder  ,1},
  {"phi",phi,1},
  {"primroot",gener,1},
  {"wf",wf,1},
  {"wf2",wf2,1},
  {"ordred",ordred,1},
  {"mu",gmu,1},
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

