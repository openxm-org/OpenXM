/* $OpenXM: OpenXM/src/asir-contrib/packages/src/tk_any2mpfr/solve_ode_by_r4d.c,v 1.2 2021/03/17 11:22:32 takayama Exp $ */
int T_verbose=0;
int output_intermediate_values(double t0,double t1,double f0[], double h,int n_defuse);
void usage() {
  printf("%s [--rank r --t0 t0 --init f1 ... fr  --t1 t1]\n",T_PROGNAME);
  printf("      [--h h --n_defuse n_defuse]\n");
  printf("      [--n_prune n_prune --strategy strat --t_noproj t_noproj]\n");
  printf("      [--ref_value_file ref_file]\n");
  printf("      [--output_step_size h2]\n");
  printf("      [--verbose --go]\n");
}
void usage_example(int type,double t0,double f0[],double t1,double h,double t_noproj,int r, int strat,int n_prune,int n_defuse,char *ref_file) {
  int i;
  if (type==1) printf("Arguments: ");
  else printf("Example: ");
  printf(" %s --rank %d --t0 %g --init ",T_PROGNAME,r,t0);
  for (i=0; i<r; i++) {
    printf("%g ",f0[i]);
  }
  printf(" --t1 %g [ --h %g --n_defuse %d --n_prune %d --t_noproj %g --strategy %d --ref_value_file %s --verbose ]\n",t1,h,n_defuse,n_prune,t_noproj,strat,ref_file);
  printf("If you want to run this program with values above, type in  %s --go\n",T_PROGNAME);
}
void usage_draw_graph() {
  printf("--- To draw a graph of the solution, do\n");
  printf("This_program --output_step_size 0.2 | grep '^gnuplot' | awk '{print $2, $3}' >t.txt\n");
  printf("In the gnuplot, plot 't.txt' w lp\n");
}

void output_tf(double t,double f[]) {
  int i;
  printf("t=%g, ",t);
  for (i=0; i<N; i++) printf("f[%d]=%g, ",i,f[i]); putchar('\n');
  printf("gnuplot %g  %g\n",t,f[0]);
}
void outout_mpfr_vec(mpfr_t m_ans[],int size) {
  int i;
  int m;
  m = sqrt(size);
  for (i=0; i<size; i++) {
    mpfr_out_str(stdout,10,0,m_ans[i],MPFR_RNDD);putchar(',');
    if (i % m == m-1) putchar('\n');
  }
}
int main(int argc,char *argv[]) {
  double t0=T_T0; // 初期時刻
  double f0[N]=T_F0; // 初期条件  gauge.rr より
  double t1=T_T1; // 終了時刻
  double h=T_H; // 微小ステップ
  double t_noproj=T_NOPROJ; // この時刻まではprojection しない
  double output_step_size=0; // defuse する間の時間のデータを出力するか?
  double tt0,tt1;
  int r=N; // ODE の rank
  int strat=T_STRAT; // projection の strategy
  int n_prune=T_N_PRUNE;  /* 最大の Re eigen_val を除く */
  int n_defuse=T_N_DEFUSE;  // matrix factorial の長さ.
  char ref_value_file[4096]=T_REF_VALUE_FILE; // 補間すべき正しい値

  int show_help=0;

  mpfr_t m_t; double t;
  mpfr_t m_h; 
  mpfr_t m_ans[N*N];
  int m_argc=2;
  mpfr_t m_argv[2];
  int i,j;
  double f[N];
  double f_new[N];
  double data[N*N]; // 変換行列
  double proj_mat[N][N];
  double mat_step[N][N];
  if (argc <2) { show_help=1; }
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i],"--rank")==0) {i++; sscanf(argv[i],"%d",&r);if (r != N) myerror("rank != N");	continue;}
    if (strcmp(argv[i],"--t0")==0) {i++; sscanf(argv[i],"%lg",&t0);continue;}
    if (strcmp(argv[i],"--init")==0) {
      for (j=0; j<r; j++) {
	i++;
	sscanf(argv[i],"%lg",&(f0[j]));
      }
      continue;
    }
    if (strcmp(argv[i],"--t1")==0) {i++; sscanf(argv[i],"%lg",&t1);continue;}
    if (strcmp(argv[i],"--h")==0) {i++; sscanf(argv[i],"%lg",&h);continue;}
    if (strcmp(argv[i],"--n_defuse")==0) {i++; sscanf(argv[i],"%d",&n_defuse);continue;}
    if (strcmp(argv[i],"--n_prune")==0) {i++; sscanf(argv[i],"%d",&n_prune);continue;}
    if (strcmp(argv[i],"--strategy")==0) {i++; sscanf(argv[i],"%d",&strat);continue;}
    if (strcmp(argv[i],"--t_noproj")==0) {i++; sscanf(argv[i],"%lg",&t_noproj);continue;}
    if (strcmp(argv[i],"--verbose")==0) {T_verbose=1; continue;}
    if (strcmp(argv[i],"--help")==0) { show_help=1; continue; }
    if (strcmp(argv[i],"--go")==0) { continue; }
    if (strcmp(argv[i],"--output_step_size")==0) { i++; sscanf(argv[i],"%lg",&output_step_size);continue;}
    printf("Error: Unknown option %s.\n",argv[i]); usage(); return(-1);
  }
  if (show_help) {
    usage();
    usage_draw_graph(); printf("\n");
    usage_example(0, t0, f0, t1, h, t_noproj, r, strat, n_prune, n_defuse,ref_value_file);
    return 0;
  }
  if (T_verbose) {
    usage_example(1, t0, f0, t1, h, t_noproj, r, strat, n_prune, n_defuse,ref_value_file);
  }

  mpfr_init2(m_t,MPFR_PREC); 
  mpfr_init2(m_h,MPFR_PREC);
  t=t0;
  mpfr_set_d(m_t,t,MPFR_RNDD);
  mpfr_set_d(m_h,h,MPFR_RNDD);
  for (i=0; i<N*N; i++) mpfr_init2(m_ans[i],MPFR_PREC);
  for (i=0; i<2; i++) mpfr_init2(m_argv[i],MPFR_PREC);

  for (i=0; i<N; i++) f[i]=f0[i];
  while (t < t1) {
    tt0=t;
    output_tf(t,f);
    mat_fac(m_t,m_h,n_defuse,m_ans,&m_argc,m_argv); // matrix factorial
    if (T_verbose) outout_mpfr_vec(m_ans,N*N);
    mat_get_dvec(data,m_ans);  // data に変換行列
    cp_vmatm(data,mat_step);   // mat_step として保存.
    printf("matrix factorial=\n"); output_mat(mat_step);
    t = mpfr_get_d(m_argv[0],MPFR_RNDD); // 次の t.
    tt1=t;
    if (t<=t_noproj) for (i=0; i<N; i++) f_new[i]=f[i]; // projection 無し.
    else {
      projection_matrix_to_subspace_of(n_prune,data,proj_mat); 
      // Warning. data は変更される
      mat_linear_transformation(proj_mat,f,f_new);
      printf("f_init="); output_vec(f,N);
      printf("\nf_new_init="); output_vec(f_new,N);  printf("\n");
      if (strat == 1) {
	vec_scalar_multiplication(f[0]/f_new[0],f_new,f_new); // 初期値の調整
	printf("f_new_init="); output_vec(f_new,N); printf(" (scaled)\n");
      }else if (strat == 2) {
	printf("strategy 2 has not yet been implemented.\n");
	/* 答えを与えて初期値を調整する 補完 strategy はまだ.
	   Todo, the initial value should be modified in the eigen-space so that
	   it attains the target value.
           Data in the file ref_value_file will be used.
	*/
      }
    }

    if (output_step_size > 0) {
      output_intermediate_values(tt0,tt1,f_new,h,(int) (output_step_size/h));
    }
    mat_linear_transformation(mat_step,f_new,f);  // 新しい t での解.
    mpfr_set_d(m_t,t,MPFR_RNDD);
  }
  if (T_verbose) {
    usage_draw_graph();
  }
  return 0;
}

int output_intermediate_values(double t0,double t1,double f0[], double h,int n_defuse) {
  // double t0 初期時刻
  // double f0[N]; 初期条件 
  // double t1 // 終了時刻
  // double h; // 微小ステップ
  int r=N;  // ODE の rank
  // int n_defuse=T_N_DEFUSE;  // matrix factorial の長さ.

  mpfr_t m_t; double t;
  mpfr_t m_h; 
  mpfr_t m_ans[N*N];
  int m_argc=2;
  mpfr_t m_argv[2];
  int i,j;
  double f[N];
  double f_new[N];
  double data[N*N]; // 変換行列
  double mat_step[N][N];

  if (n_defuse <= 0) return 0;
  mpfr_init2(m_t,MPFR_PREC); 
  mpfr_init2(m_h,MPFR_PREC);
  t=t0;
  mpfr_set_d(m_t,t,MPFR_RNDD);
  mpfr_set_d(m_h,h,MPFR_RNDD);
  for (i=0; i<N*N; i++) mpfr_init2(m_ans[i],MPFR_PREC);
  for (i=0; i<2; i++) mpfr_init2(m_argv[i],MPFR_PREC);

  for (i=0; i<N; i++) f[i]=f0[i];
  while (t < t1) {
    if ( t!= t0) output_tf(t,f);
    mat_fac(m_t,m_h,n_defuse,m_ans,&m_argc,m_argv); // matrix factorial
    if (T_verbose) outout_mpfr_vec(m_ans,N*N);
    mat_get_dvec(data,m_ans);  // data に変換行列
    cp_vmatm(data,mat_step);   // mat_step として保存.
    // printf("matrix factorial=\n"); output_mat(mat_step);
    t = mpfr_get_d(m_argv[0],MPFR_RNDD); // 次の t.

    for (i=0; i<N; i++) f_new[i]=f[i]; // projection 無し.
    mat_linear_transformation(mat_step,f_new,f);  // 新しい t での解.
    mpfr_set_d(m_t,t,MPFR_RNDD);
  }
  return 0;
}

