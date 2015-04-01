#define oxprintf(...)  printf(__VA_ARGS__)
#define oxprintfe(...) fprintf(stderr,__VA_ARGS__)
#define oxflush()  fflush(NULL)
#define oxabort()  abort()
#define oxstdout  stdout
#define oxexit(a) exit(a)
#define oxexit0(a) exit(a)
