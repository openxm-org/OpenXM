#include <R.h>
#include <R_ext/Utils.h>
#include <R_ext/Print.h>
#define oxprintf(...)  Rprintf(__VA_ARGS__)
#define oxprintfe(...) REprintf(__VA_ARGS__)
#define oxflush()  
#define oxabort()  error("Fatal error in the shared lib hgm.\n")
#define oxstdout  NULL
#define oxstdin  NULL
#define oxexit(a)  error("Fatal error in the shared lib hgm.\n");
#define oxexit0(a)  error("Fatal error in the shared lib hgm.\n");
