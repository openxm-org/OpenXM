// rank_check_ff.c
/*
  For test,
  Set Debug=2
  ./rank_check_ff Data/rc-arg1.txt Data/rc-arg2.txt
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
typedef long LONG;
LONG inv_ff(LONG a);  // Data/rref.c 

void *mymalloc(size_t n);
void myfree(void *p);
void freemat(LONG **p,int row);
void freevect(LONG *p);
void freeintvect(int *p);
LONG **newmat(int row,int col);
LONG *newvect(int n);
int *newintvect(int n);
LONG **matrix_identity_matrix(int n);
void matrix_transpose_nc(LONG **destMat,LONG **mat,int row,int col);
void rowx(LONG **mat,int i,int j,int row,int col);
void rowm(LONG **mat,int i,LONG c,int row,int col);
void rowa(LONG **mat,int i,int j,LONG c,int row,int col);
int is_zero_vect(LONG *V,int n);
void print_mat(LONG **mat,int row, int col);

void matrix_transpose_nc_square(LONG **m,int row);
void vect_matrix_mult(LONG *ans, LONG *vec,LONG **p,int row, int col,LONG prime);
int is_zero_vect_from(LONG *vec,int from, int to);
int bsave_mat(LONG **m,int row, int col, char fname[]);
int bsave_intvect(int *m,int n, char fname[]);

struct MData {
  LONG **M;
  int *Indep_columns;
  int len;
  LONG **P;
  int row;
  int col;
};

struct MData gauss_elim_ff(LONG **mat,int row,int col,LONG prime);

/* for checking results */
LONG **matrix_mult(LONG **m1,LONG **m2, int ii,int jj,int kk);
LONG **matrix_sub(LONG **m1,LONG **m2,int row,int col);
void print_intvect(int *v,int len);

/* for outputing results */
void fprint_mat_asir_form(FILE *fp2,LONG **m,int row,int col);
void fprint_intvec_asir_form(FILE *fp2,int *v,int n);
  
LONG Prime=13;
int Check=0;
int Debug=1;
//int Debug=2;
size_t Used_memory=0;

int StoreData=1;

void *mymalloc(size_t n) {
  void *m;
  Used_memory += n;
  m = malloc(n);
  if (m == NULL) {fprintf(stderr,"No more memory. Have allocated %zu bytes\n",Used_memory); exit(-1);
  }
  return m;
}
void myfree(void *p) {
  free(p);
}
void freemat(LONG **p,int row) {
  int i;
  for (i=0; i<row; i++) {
      myfree(p[i]);
  }
  myfree(p);
}
void freevect(LONG *p) {
  myfree(p);
}
void freeintvect(int *p) {
  myfree(p);
}


LONG inv_ff(LONG a)
{
  LONG f1,f2,a1,a2,q,r,mod;
  mod = Prime;
  f1 = a; f2 = mod;
  a1 = 1; a2 = 0;
  while ( 1 ) {
    q = f1/f2; r = f1 - f2*q; 
    f1 = f2; f2 = r;
    if ( r == 0 ) 
      break;
    r = (a2*q)%mod;
    r = a1-r;
    if ( r < 0 ) r += mod;
    a1 = a2; a2 = r;
  }
  return a2;
}
LONG **newmat(int row,int col) {
  LONG **m;
  int i;
  m = (LONG **)mymalloc(sizeof(LONG *)*row);
  for (i=0; i<row; i++) m[i]=(LONG *)mymalloc(sizeof(LONG)*col);
  if (Debug) fprintf(stderr,"Have allocated %zu M bytes.\n",Used_memory/1000000);
  return m;
}
LONG *newvect(int n) {
  LONG *v;
  int i;
  v = (LONG *)mymalloc(sizeof(LONG)*n);
  if (Debug) fprintf(stderr,"Have allocated %zu M bytes.\n",Used_memory/1000000);
  return v;
}
int *newintvect(int n) {
  int *v;
  int i;
  v = (int *)mymalloc(sizeof(int)*n);
  if (Debug) fprintf(stderr,"Have allocated %zu M bytes.\n",Used_memory/1000000);
  return v;
}
LONG **matrix_identity_matrix(int n) {
  LONG **m;
  int i,j;
  m = newmat(n,n);
  for (i=0; i<n; i++) for (j=0; j<n; j++) {
      if (i != j) m[i][j]=0; else m[i][j]=1;
    }
  return m;
}
/* _nc: not_clone */
void matrix_transpose_nc(LONG **destMat,LONG **mat,int row,int col) {
  LONG **m;
  int i,j;
  m = destMat;
  for (i=0; i<row; i++) {
    for (j=0; j<col; j++) {
      m[j][i]=mat[i][j];
    }
  }
}
void rowx(LONG **mat,int i,int j,int row,int col) {
  /* exchange i-th row and j-th row. */
  int q;
  LONG t;
  for (q=0; q<col; q++) {
     t=mat[i][q]; mat[i][q]=mat[j][q]; mat[j][q]=t;
  }
}
void rowm(LONG **mat,int i,LONG c,int row,int col) {
  /* multiply i-th row by c. */
  int q;
  LONG t;
  for (q=0; q<col; q++) {
     mat[i][q] = mat[i][q]*c;
     mat[i][q] = mat[i][q] % Prime;
     if (mat[i][q] < 0) mat[i][q]=mat[i][q]+Prime;
     //printf("%ld ",mat[i][q]);
  }
}
void rowa(LONG **mat,int i,int j,LONG c,int row,int col) {
  /* i-th row = i-th row + c*(j-th row) */
  int q;
  LONG t;
  for (q=0; q<col; q++) {
     mat[i][q] = mat[i][q] + ((c*mat[j][q]) % Prime);
     mat[i][q] = mat[i][q] % Prime;
     if (mat[i][q] < 0) mat[i][q]=mat[i][q]+Prime;
     //printf("%ld ",mat[i][q]);
  }
}

int is_zero_vect(LONG *v,int n) {
  int i;
  for (i=0; i<n; i++) {
    if (v[i]%Prime != 0) return 0;
  }
  return 1;
}

void print_mat(LONG **mat,int row, int col) {
  int i,j;
  for (i=0; i<row; i++) {
    for (j=0; j<col; j++) {
      printf("%11ld ",mat[i][j]);
    }
    printf("\n");
  }
}
LONG **matrix_mult(LONG **m1,LONG **m2, int ii,int jj,int kk) {
  int i,j,k;
  LONG **m;
  LONG t;
  m = newmat(ii,kk);
  for (i=0; i<ii; i++) {
    for (k=0; k<kk; k++) {
      t = 0;
      for (j=0; j<jj; j++) {
	t = (t+(m1[i][j]*m2[j][k] % Prime)) % Prime;
      }
      m[i][k]=t;
    }
  }
  return m;
}
void print_intvect(int *v,int len) {
  int i;
  for (i=0; i<len; i++) printf("%12d ",v[i]);
}
LONG **matrix_sub(LONG **m1,LONG **m2,int row,int col) {
  int i,j;
  LONG **m;
  m = newmat(row,col);
  for (i=0; i<row; i++) {
    for (j=0; j<col; j++) {
      if (m2 != NULL) {
	m[i][j] = (m1[i][j]-m2[i][j]) % Prime;
      }else {
	m[i][j] = m1[i][j] % Prime;
      }
    }
  }
  return m;
}

void fprint_mat_asir_form(FILE *fp2,LONG **m,int row,int col) {
  int i,j;
  fprintf(fp2,"[");
  for (i=0; i<row; i++) {
    fprintf(fp2," [");
    for (j=0; j<col; j++) {
      fprintf(fp2,"%ld",m[i][j]);
      if (j != col-1) fprintf(fp2,",");
      else fprintf(fp2,"]");
    }
    if (i != row-1) fprintf(fp2,",");
    else fprintf(fp2,"]");
  }
}
void fprint_intvec_asir_form(FILE *fp2,int *v,int n) {
  int j;
  fprintf(fp2,"[");
  for (j=0; j<n; j++) {
    fprintf(fp2,"%d",v[j]);
    if (j != n-1) fprintf(fp2,",");
    else fprintf(fp2,"]");
  }
}


struct MData gauss_elim_ff(LONG **M,int row,int col,LONG myprime) {
  int RL,CL;
  LONG **P;
  int Current_row,Current_column,I,J,K,index;
  LONG **MT;
  int *Indep_columns;
  struct MData md;
  Prime=myprime;
  RL=row;
  CL=col;
  P=matrix_identity_matrix(RL);
  Current_row=0;
  Current_column=0;
  Indep_columns=newintvect(col); index=0;
  MT=newmat(col,row);
  while((Current_row<RL)&&(Current_column<CL))
    {
      matrix_transpose_nc(MT,M,row,col);
      J=Current_column;
      while((J<CL)&&(is_zero_vect(MT[J],row))){
	J++;
      }
      if(J<CL){
	I=Current_row;
	while((I<RL)&&(M[I][J]==0)){
	  I++;
	}
	if(I<RL){
	  rowx(P,Current_row,I,row,row);
	  rowx(M,Current_row,I,row,col);
	  rowm(P,Current_row,inv_ff(M[Current_row][J]),row,row);
	  rowm(M,Current_row,inv_ff(M[Current_row][J]),row,col);
	  for(K=0;K<RL;K++){
	    if((K!=Current_row)&&(M[K][J]!=0)){
	      rowa(P,K,Current_row,-M[K][J],row,row);
	      rowa(M,K,Current_row,-M[K][J],row,col);
	    }
	  }
	  Current_row++;
	  Indep_columns[index++]=J;
	}
      }
      Current_column=J+1;
    }
 md.M=M;
 md.Indep_columns=Indep_columns;
 md.len=index;
 md.P=P;
 md.row=row;
 md.col=col;

 freemat(MT,col);

 return md;
}

void matrix_transpose_nc_square(LONG **m,int row) {
  // m is a square matrix
  int i,j;
  LONG t;
  for (i=0; i<row; i++) {
    for (j=0; j<i; j++) {
      t=m[i][j]; m[i][j]=m[j][i]; m[j][i]=t;
    }
  }
}
void vect_matrix_mult(LONG *ans, LONG *vec,LONG **p,int row, int col,LONG prime) {
  // ans <- vec*P  len(vec)=row, len(ans)=col
  LONG t;
  int i,j;
  for (j=0; j<col; j++) {
    t=0;
    for (i=0; i<row; i++) {
      t += vec[i]*p[i][j]; t = t % prime;
    }
    ans[j]=t;
  }
}
int is_zero_vect_from(LONG *vec,int from, int to) {
  // (vec[from], ..., vec[to-1])=0, then 1 else 0
  int i;
  for (i=from; i<to; i++) {
    if ((vec[i]%Prime) != 0) return 0;
  }
  return 1;
}

/* 2022.01.22 */
int bsave_mat(LONG **m,int row, int col, char fname[]) {
  FILE *fp;
  int i;
  fp = fopen(fname,"w");
  if (fp == NULL) return -1;
  fwrite(&row,sizeof(int),(size_t) 1,fp);
  fwrite(&col,sizeof(int),(size_t) 1,fp);
  for (i=0; i<row; i++) {
    fwrite(m[i],sizeof(LONG),(size_t)col,fp);
  }
  fclose(fp);
  return 0;
}
int bsave_intvect(int *m,int n, char fname[]) {
  FILE *fp;
  fp = fopen(fname,"w");
  if (fp == NULL) return -1;
  fwrite(&n,sizeof(int),(size_t)1,fp);
  fwrite(m,sizeof(int),(size_t)n,fp);
  fclose(fp);
  return 0;
}

int main_test1() {
  LONG **m;
  LONG **m_orig;
  int row,col;
  struct MData md;
  row=3; col=2;
  m = newmat(row,col);
  m[0][0]=1; m[0][1]=2;
  m[1][0]=3; m[1][1]=4;
  m[2][0]=4; m[2][1]=6;
  m_orig=matrix_sub(m,NULL,row,col);
  print_mat(m,row,col); printf("\n");
  md=gauss_elim_ff(m,row,col,Prime);
  printf("m=\n");
  print_mat(m,row,col);
  printf("independent col=\n");
  print_intvect(md.Indep_columns,md.len); printf("\n");
  printf("P=\n");
  print_mat(md.P,row,row);

  printf("Should be 0=\n");
  print_mat(matrix_sub(m,matrix_mult(md.P,m_orig,row,row,col),row,col),row,col);
  return 0;
}

void usage() {
  printf("check_rank_ff file_of_matrix_M file_of_ES2 [file_of_check_value]\n");
}
/*

*/
int main(int argc, char *argv[]) {
  FILE *fp;
  FILE *fp2;
  FILE *fp3;
  int row,col,i,j;
  int row2,col2; // for es2
  int r,result;
  LONG **m;
  LONG **m_orig;
  LONG **es2;
  LONG **diff;
  LONG *ans=NULL;
  struct MData md;

  char fname_m_orig[1024*10];
  char fname_m[1024*10];
  char fname_indep[1024*10];
  char fname_p[1024*10];
  char fname_info[1024*10];
  if (argc < 3) {
    usage(); return -1;
  }
  fp = fopen(argv[1],"r");
  if (fp==NULL) {
    fprintf(stderr,"Error: file %s is not found.\n",argv[1]);
    return -1;
  }
  fp2 = fopen(argv[2],"r");
  if (fp2==NULL) {
    fprintf(stderr,"Error: file %s is not found.\n",argv[2]);
    return -1;
  }
  if (argc==3) {
    fp3=stdout;
    fname_info[0]=0;
  }else {
    fp3 = fopen(argv[3],"w");
    sprintf(fname_info,"%s-info.txt",argv[3]);
  }
  if (StoreData) {
    sprintf(fname_m_orig,"%s-Morig-transposed-in.bin",argv[1]);
    sprintf(fname_m,"%s-M-transposed-out.bin",argv[1]);
    sprintf(fname_indep,"%s-Indep_col-out.bin",argv[1]);
    sprintf(fname_p,"%s-P-transposed-out.bin",argv[1]);
  }
  
  fscanf(fp,"%ld",&Prime);
  fscanf(fp,"%d",&row); fscanf(fp,"%d",&col);
  m = newmat(row,col);
  for (i=0; i<row; i++) for (j=0; j<col; j++) fscanf(fp,"%ld",&(m[i][j]));

  fscanf(fp2,"%d",&row2); fscanf(fp2,"%d",&col2);
  if (col2 != row) {
    fprintf(stderr,"Format error of ES2\n"); return(-1);
  }
  es2 = newmat(row2,col2);
  for (i=0; i<row2; i++) for (j=0; j<col2; j++) fscanf(fp2,"%ld",&(es2[i][j]));

  fclose(fp); fclose(fp2);
  m_orig=matrix_sub(m,NULL,row,col);
  md=gauss_elim_ff(m,row,col,Prime);
  if (Check) {
    /* diff should be 0 */
    LONG **tm;
    diff=matrix_sub(m,tm=matrix_mult(md.P,m_orig,row,row,col),row,col);
    for (i=0; i<row; i++) for (j=0; j<col; j++) {
	if (diff[i][j]%Prime != 0) {
	  fprintf(stderr,"Error of diff check\n"); return(-1);
	}
      }
    freemat(diff,row); freemat(tm,row);
  }
  /* 
     L=[m of row*col, 
        md.Indep_columns of length md.len, 
        md.P of row*row]
     L を bsave する.
  */
  if (StoreData) {
    bsave_mat(m_orig,row,col,fname_m_orig);
    bsave_mat(m,row,col,fname_m);
    bsave_intvect(md.Indep_columns,md.len,fname_indep);
    bsave_mat(md.P,row,row,fname_p);
  }

  fprintf(stderr,"NT_info=[rank,[row, col]( of m=transposed MData[0]),indep_columns(L[2])]\n");
  if (fname_info[0] != 0) fp = fopen(fname_info,"w");
  else fp = stdout;
  fprintf(fp,"NT_info=[");
  fprintf(fp,"%d, ",md.len);
  fprintf(fp,"[%d,%d], [",row,col);
  for (i=0; i<md.len; i++) {
    fprintf(fp,"%d",(md.Indep_columns)[i]);
    if (i != (md.len-1)) fprintf(fp,",");
  }
  fprintf(fp,"]]$ ");
  if (fp != stdout)  fclose(fp);


  /* start of rank check. m の転置は不要. */
  r=md.len;
  printf("Rank=%d\n",r);
  if (r == row) {
    printf("The matrix is full rank.\n");
    result=1;
  }else{
    matrix_transpose_nc_square(md.P,row);
    ans = newvect(row);
    result=1;
    for (i=0; i<row2; i++) {
      vect_matrix_mult(ans,es2[i],md.P,col2,row,Prime);
      if (!is_zero_vect_from(ans,r,row)) {
	result=0;
	printf("%d-th rank condition is failed.\n ",i);
	break;
      }
      printf("%d-th rank condition is OK.\n",i);
    }
  }
  if (Debug>=2) {
    LONG **mt;
    mt = newmat(col,row);
    matrix_transpose_nc(mt,m,row,col);
    fprint_mat_asir_form(fp3,mt,col,row);
    fprintf(fp3,",\n");
    freemat(mt,col);

    fprint_intvec_asir_form(fp3,md.Indep_columns,md.len);
    fprintf(fp3,",\n");
    fprint_mat_asir_form(fp3,md.P,row,row);
    fprintf(fp3,"]$\n");
  }

  fprintf(fp3,"NT_result=%d$ ",result);
  if (fp3 != stdout) fclose(fp3);

  freemat(m,row);
  freemat(m_orig,row);
  freemat(md.P,row);
  freemat(es2,row2);
  if (ans) freevect(ans);
  freeintvect(md.Indep_columns);
  return 0;
}
