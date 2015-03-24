#define oxprintf(...)  printf(__VA_ARGS__)
#define oxprintfe(...) fprintf(stderr,__VA_ARGS__)
#define oxflush()  fflush(NULL)
#define oxabort()  abort()
