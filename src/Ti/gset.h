/*
** gset.h                                 Birk Huber, 4/99 
** -- header file with definitions and basic operations on gsets
**  
**
** TiGERS,  Toric Groebner Basis Enumeration by Reverse Search 
** copyright (c) 1999  Birk Huber
**
*/
#include "binomial.h"

typedef struct gset_tag *gset;

struct gset_tag{
 int id,nfacets,nelts,deg;
 binomial bottom;
 binomial cache_edge;
 struct gset_tag *cache_vtx;
 struct gset_tag *next;
 };

#define gset_first(g) (g->bottom)                            
#define gset_cache_vtx(g) (g->cache_vtx)
#define gset_cache_edge(g) (g->cache_edge)
#define gset_nelts(g) (g->nelts)
#define gset_nfacets(g) (g->nfacets)
#define gset_id(g) (g->id)
#define gset_deg(g) (g->deg)
gset gset_new();
void gset_free(gset g);
int gset_read(FILE *is,gset g);
void gset_print(FILE *of,gset g);
void gset_init_print(FILE *of,gset g);
void gset_insert(gset g, binomial b);
binomial gset_downedge(gset g);
gset gset_flip(gset g1, binomial b);
void gset_autoreduce();
int gset_isfacet(gset g,binomial b);
void gset_setfacets(gset g);
gset gset_toric_ideal(int **,int,int);
void gset_rgb(gset g, int (*comp)(monomial,monomial));







