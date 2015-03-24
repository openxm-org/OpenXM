#include <R_ext/Print.h>
#define oxprintf(...)  Rprintf(__VA_ARGS__)
#define oxprintfe(...) REprintf(__VA_ARGS__)
#define oxflush()  
#define oxabort()  REprintf("Fatal error in the shared lib hgm.\n")
#define oxstdout  NULL
