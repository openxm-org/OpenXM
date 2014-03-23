#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#define MAX_DIM  32
#define MAX_DATA 1056 /*  32*32 + 32  */
#define RK_STEP_SIZE 0x0001

extern double hgm_ko_orthant(int dimension, double *sigma, double *mu, double rk_step_size);

static int input_data(FILE *fp, int *dim, double *data);

#ifdef _DEBUG
extern void test_dgetrf_(void);
extern void test_dpotri_(void);
extern void test_runge_kutta(void);
extern void test_update_xy(void);
extern void test_cal_sigmaI_muI(void);
extern void test1(void);
extern void test2(void);
extern void test3(void);

#define TEST_DGETRF_ 0x0100
#define TEST_DPOTRI_ 0x0101
#define TEST_RUNGE_KUTTA 0x0102
#define TEST_UPDATE_XY 0x0103
#define TEST_CAL_SIGMAI_MUI 0x0104

#define TEST1 0x0201
#define TEST2 0x0202
#define TEST3 0x0203
#define TEST4 0x0204
#endif

int 
input_data(FILE *fp, int *dim, double *data)
{
  fscanf(fp, "%d", dim);
  int i;
  double length = *dim * (*dim+1);
  double *p = data;
  for(i=0; i< length; i++)
    fscanf(fp, "%lf", p++); 
  return 0;
}

int 
main(int argc, char *argv[])
{
  double rk_step_size = 1e-3;

  int c;

  while (1) {
    int option_index = 0; 
    static struct option long_options[] = {
      {"rk_step_size", required_argument, 0, 0x0001},
#ifdef _DEBUG
      {"test_dgetrf_", no_argument,0, TEST_DGETRF_},
      {"test_dpotri_", no_argument,0, TEST_DPOTRI_},
      {"test_runge_kutta", no_argument,0, TEST_RUNGE_KUTTA},
      {"test_update_xy", no_argument, 0, TEST_UPDATE_XY},
      {"test_cal_sigmaI_muI", no_argument, 0, TEST_CAL_SIGMAI_MUI},
      {"test1", no_argument,0, TEST1},
      {"test2", no_argument,0, TEST2},
      {"test3", no_argument,0, TEST3},
      {"test4", no_argument,0, TEST4},
#endif
      {0,0,0,0}
    };

    c = getopt_long(argc, argv, "", long_options, &option_index);
    
    if (c == -1)
      break;
    
    switch (c) {
    case RK_STEP_SIZE:
      if (sscanf(optarg, "%lg", &rk_step_size) != 1 || rk_step_size<=0.0){
	fprintf(stderr, "Error: the argument of the option '--rk_step_size' must be a positive number.\n");
	exit(EXIT_FAILURE);
      }
      break;
#ifdef _DEBUG
    case TEST_DGETRF_:
      test_dgetrf_();
      exit(EXIT_SUCCESS);
      break;
    case TEST_DPOTRI_:
      test_dpotri_();
      exit(EXIT_SUCCESS);
      break;
    case TEST_RUNGE_KUTTA:
      test_runge_kutta();
      exit(EXIT_SUCCESS);
      break;
    case TEST_UPDATE_XY:
      test_update_xy();
      exit(EXIT_SUCCESS);
      break;
    case TEST_CAL_SIGMAI_MUI:
      test_cal_sigmaI_muI();
      exit(EXIT_SUCCESS);
      break;
    case TEST1:
      test1();
      exit(EXIT_SUCCESS);
      break;
    case TEST2:
      test2();
      exit(EXIT_SUCCESS);
      break;
    case TEST3:
      test3();
      exit(EXIT_SUCCESS);
      break;
    case TEST4:
      exit(EXIT_SUCCESS);
      break;
#endif
    default:
      fprintf(stderr, "Error: unknown option\n");
      exit(EXIT_FAILURE);
      break;
    }
  }


  if (argc < 2) {
    fprintf(stderr, "usage: ./a.out [-h rk_step_size] filename.txt\n");
    exit(EXIT_FAILURE);
  }

  FILE *ifp = fopen(argv[optind], "r");
  if(ifp == NULL){
    fprintf(stderr, "Error: can not open %s.\n", argv[optind]);
    exit(EXIT_FAILURE);
  }
  int dim;
  double data[MAX_DATA];
  input_data(ifp, &dim, data);
  fclose(ifp);

  double result = hgm_ko_orthant(dim, data, data+dim*dim, rk_step_size);
  printf("probability=%10.9f\n", result);

  return 0;
}

