#include <stdio.h>
#include <string.h>
#define BSIZE (2024*10)

int main(int argc,char *argv[]) {
  char comm[1024];
  char base[1014];
  FILE *fp;
  char line[BSIZE];
  if (argc < 2) strcpy(base,".");
  else strcpy(base,argv[1]);

  printf("<html><body>\n");
  sprintf(comm,"find %s -name \"*.pdf\" -print >tmp-pdf.tmp",base);
  system(comm);
  printf("<h1> PDF documents in this folder </h1>\n<ol>\n");
  fp = fopen("tmp-pdf.tmp","r");
  while (fgets(line,BSIZE-2,fp) !=NULL) {
    if (line[strlen(line)-1] == '\n') {
      line[strlen(line)-1]=0;
    }
    printf("<li><a href=\"%s\"> %s </a>\n",line,line);
  }
  printf("</ol>\n\n");
  fclose(fp);

  sprintf(comm,"find %s -name \"*.html\" -print >tmp-pdf.tmp",base);
  system(comm);
  printf("<h1> HTML documents in this folder </h1>\n<ol>\n");
  fp = fopen("tmp-pdf.tmp","r");
  while (fgets(line,BSIZE-2,fp) !=NULL) {
    if (line[strlen(line)-1] == '\n') {
      line[strlen(line)-1]=0;
    }
    printf("<li><a href=\"%s\"> %s </a>\n",line,line);
  }
  printf("</ol>\n\n");
  fclose(fp);

  printf("</body></html>\n");
  return(0);
}
