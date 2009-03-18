/*
Copyright:  Y.Mitou,  2009.01
 
License: This software is distributed under the 2-cluases BSD license 
(http://en.wikipedia.org/wiki/BSD_licenses)
*/

#if defined (_MSC_VER)
#include <windows.h>
#endif
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include<GL/glut.h>
#endif

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

#define TEX_WIDTH 128
#define TEX_HEIGHT 128

#define KAKUDO 0.05
#define DIV 0.3
#define BOXSIZE 1.0
#define FUNCLEN 0.3
#define MAG 1.1
#define MARGIN 0
#define INTERVAL 5.0

#define LINE_MAX  2048
#define LIMIT 1000
#define outOfMemory()  {  fprintf(stderr,"Out of memory.\n"); return -1; }

/* for texture */
static GLuint texInobuta;
static GLubyte image[TEX_HEIGHT][TEX_WIDTH][4];

/* rasters */
GLubyte raster_5[24] = {
  0x3f, 0x00, 0x7f, 0x80, 0x61, 0xc0, 0x00, 0xc0, 0xc0, 0xc0, 
  0xf1, 0xc0, 0xff, 0x80, 0xdf, 0x00, 0xc0, 0x00, 0xc0, 0x00, 
  0xff, 0xc0, 0xff, 0xc0 };

GLubyte raster_X[24] = {
  0x00, 0x00, 0x80, 0x40, 0xc0, 0xc0, 0x61, 0x80, 0x33, 0x00, 
  0x1e, 0x00, 0x0c, 0x00, 0x1e, 0x00, 0x33, 0x00, 0x61, 0x80, 
  0xc0, 0xc0, 0x80, 0x40 };

GLubyte raster_Y[24] = {
  0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 
  0x0c, 0x00, 0x1e, 0x00, 0x12, 0x00, 0x33, 0x00, 0x21, 0x00, 
  0x61, 0x80, 0xc0, 0xc0 };

GLubyte raster_Z[24] = {
  0xff, 0xc0, 0xff, 0xc0, 0xc0, 0x00, 0x60, 0x00, 0x30, 0x00, 
  0x18, 0x00, 0x0c, 0x00, 0x06, 0x00, 0x03, 0x00, 0x01, 0x80, 
  0xff, 0xc0, 0xff, 0xc0 };

GLubyte raster_$[44] = {
  0x04, 0x00, 0x04, 0x00, 0x1f, 0x00, 0x64, 0xc0, 0xc4, 0x60, 
  0xc4, 0x60, 0x04, 0x60, 0x04, 0x60, 0x04, 0x60, 0x04, 0xe0, 
  0x07, 0xc0, 0x1f, 0x00, 0x7c, 0x00, 0xe4, 0x00, 0xc4, 0x00,
  0xc4, 0x00, 0xc4, 0x60, 0xc4, 0x60, 0x64, 0xc0, 0x1f, 0x00, 
  0x04, 0x00 ,0x04, 0x00 };

GLubyte raster_at[40] = {
  0x1f, 0x00, 0x20, 0xc0, 0x60, 0x20, 0x40, 0x00, 0xc0, 0x60, 
  0x9d, 0xc0, 0xa7, 0x60, 0xa3, 0x20, 0xa3, 0x20, 0xa3, 0x20, 
  0xa3, 0x20, 0xa3, 0x20, 0xa3, 0x20, 0x97, 0x20, 0x8d, 0x20,
  0xc0, 0x60, 0x40, 0x40, 0x60, 0xc0, 0x20, 0x80, 0x1f, 0x00 };

/* for lighting */

GLfloat light0_diffuse[] =
{0.0, 0.6, 0.2, 1.0};
GLfloat light0_position[] =
{0.0, 50.0, -50.0, 0.0};
GLfloat light1_diffuse[] =
{0.0, 0.6, 0.2, 1.0};
GLfloat light1_position[] =
{50.0, 0.0, 50.0, 0.0};

GLfloat light2_diffuse[] =
{0.6, 0.6, 0.0, 1.0};
GLfloat light2_position[] =
{-50.0, -50.0, 0.0, 0.0};
GLfloat light3_diffuse[] =
{0.0, 0.0, 0.6, 1.0};
GLfloat light3_position[] =
{0.0, 3.0, 0.0, 0.0};
GLfloat light4_diffuse[] =
{0.0, 0.0, 0.0, 1.0};
GLfloat light4_position[] =
{3.0, 0.0, 0.0, 0.0};
GLfloat light5_diffuse[] =
{0.0, 1.0, 0.0, 1.0};
GLfloat light5_position[] =
{-3.0, 0.0, 0.0, 0.0};

/*
GLfloat model_ambient[] = { 1.3, 1.3, 1.3, 1.2 };
*/

/* for Display_List name */
GLuint theGraph, theBox, theAxis, theWhole, theTriangle;

/* for rotation */ 
GLfloat angle[2] = {0.0, 0.0};
int moving, begin[2];

/* for normal */
GLfloat v1[3];
GLfloat v2[3];
GLfloat normal[3] = {0.0, 0.0, 0.0};

/* for calc */
int newModel = 1;
int newGraph = 1;

double x, y, z;

int size = 12;
double size_z = 12.0;
double len = 3.0;
double z_interval = 5.0;

double mag = 1.0;
double z_syukusyou = 1.0;

double viewpoint[3] = {0.0, 0.0, 80.0};

/* for TRIANGLE */
int triSwitch = 1;
double **triangles;
int tr_i; 

/* for Texuture of Triangles */
int te_i;
double **texturecoords;
static int exTex = 0;

static int exSample = 1;

static int exAxis = 1;

static int exNet = 0
;
/* control Expression */
GLboolean lightSwitch = GL_FALSE;
GLboolean textureSwitch = GL_FALSE;
GLboolean blendSwitch = GL_FALSE;

void makeTexture(void)
{
  FILE *fp; 
  int x, z;
  char *oxhome;
  char fname[1024];

  /* texture file open (file name is "inobuta.tga")*/
  oxhome = getenv("OpenXM_HOME");
  if (oxhome != NULL) {
	sprintf(fname,"%s/share/oxmgraph/inobuta.tga",oxhome);
  }else{
	sprintf(fname,"/usr/local/share/oxmgraph/inobuta.tgz");
  }
  if((fp=fopen(fname, "rb"))==NULL){
	if ((fp = fopen("inobuta.tgz","rb")) == NULL) {
	  fprintf(stderr, "texture file cannot be open\n");
	  return;
	}
  }
  fseek(fp, 18, SEEK_SET);
  for (x=0; x<TEX_HEIGHT; x++) {
    for (z=0; z<TEX_WIDTH; z++) {
      image[x][z][2]=fgetc(fp);/* B */ 
      image[x][z][1]=fgetc(fp);/* G */
      image[x][z][0]=fgetc(fp);/* R */
      image[x][z][3]=255;/* alpha */
    }
  }
  fclose(fp);
}

double polynomial(double x, double y)
{
  return ((x*x)-(y*y));
}

double norm(double x, double y, double z)
{
  return sqrt(x*x + y*y + z*z);
}

void gaiseki(float vector1[], float vector2[], float vector3[])
{
 vector3[0] = vector1[1]*vector2[2] - vector1[2]*vector2[1];
 vector3[1] = vector1[2]*vector2[0] - vector1[0]*vector2[2];
 vector3[2] = vector1[0]*vector2[1] - vector1[1]+vector2[0];
}

void output(GLfloat x, GLfloat y, char *text)
{
  char *p;

  glPushMatrix();
  glTranslatef(x, y, 0);
  for (p = text; *p; p++)
    glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
  glPopMatrix();
}

static void graph(void)
{
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glBindTexture(GL_TEXTURE_2D, texInobuta);

  for(x = -len; x <= len-DIV; x += DIV) { 

    for(y = -len; y <= len-DIV; y += DIV) {
      z = polynomial(x, y);
 
      if ( z >= -size_z && z <= size_z &&
	   z >= -size && z <= size ) {
   
	glColor3f((x+len)/(2*len), -(y-len)/(2*len), 1.0);
	/* normal */
	v1[0] = DIV;
	v1[1] = 0.0;
	v1[2] = polynomial(x+DIV, y) - polynomial(x, y);
	v2[0] = DIV;
	v2[1] = DIV;
	v2[2] = polynomial(x+DIV, y+DIV) - polynomial(x, y);
	gaiseki(v1, v2, normal);
	glNormal3fv(normal);
	
	glBegin(GL_TRIANGLES);
	  glTexCoord2f((x+len)/(2*len-DIV), (y+len)/(2*len-DIV));
	  glVertex3f(x, y, polynomial(x,y));
	  glTexCoord2f((x+DIV+len)/(2*len-DIV), (y+len)/(2*len-DIV));
	  glVertex3f(x+DIV, y, polynomial(x+DIV, y));
	  glTexCoord2f((x+DIV+len)/(2*len-DIV), (y+DIV+len)/(2*len-DIV));
	  glVertex3f(x+DIV, y+DIV, polynomial(x+DIV, y+DIV));
	glEnd();

	glBegin(GL_TRIANGLES);
  	  glTexCoord2f((x+len)/(2*len-DIV), (y+DIV+len)/(2*len-DIV));
	  glVertex3f(x, y+DIV, polynomial(x, y+DIV));
	  glTexCoord2f((x+len)/(2*len-DIV), (y+len)/(2*len-DIV));
	  glVertex3f(x, y, polynomial(x,y));
	  glTexCoord2f((x+DIV+len)/(2*len-DIV), (y+DIV+len)/(2*len-DIV));
	  glVertex3f(x+DIV, y+DIV, polynomial(x+DIV, y+DIV));
	glEnd();
	
	if ( exNet ) {
	  glColor3f(1.0,1.0,1.0);
	  if(!blendSwitch) {
	    glDisable(GL_LIGHTING);
	    glDisable(GL_TEXTURE_2D);
	    glColor3f(0.0,0.0,0.0);
	  }
	  glBegin(GL_LINE_LOOP);
	    glVertex3f(x, y, polynomial(x,y));
	    glVertex3f(x+DIV, y, polynomial(x+DIV, y));
	    glVertex3f(x+DIV, y+DIV, polynomial(x+DIV, y+DIV));
	  glEnd();
	  glBegin(GL_LINE_LOOP);
	    glVertex3f(x, y+DIV, polynomial(x,y+DIV));
	    glVertex3f(x, y, polynomial(x, y));
	    glVertex3f(x+DIV, y+DIV, polynomial(x+DIV, y+DIV));
	  glEnd();
	  if(lightSwitch)
	    glEnable(GL_LIGHTING);
	  if(textureSwitch) 
	    glEnable(GL_TEXTURE_2D); 
      	}
      }
    }
  }
}

void triangle(void)
{
  int j;

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glBindTexture(GL_TEXTURE_2D, texInobuta);

  for (j=0; j<tr_i; j++) { 
    if(triangles[j][0] <= size && triangles[j][0] >= -size && 
       triangles[j][3] <= size && triangles[j][3] >= -size && 
       triangles[j][6] <= size && triangles[j][6] >= -size &&
       triangles[j][1] <= size && triangles[j][1] >= -size &&
       triangles[j][4] <= size && triangles[j][4] >= -size && 
       triangles[j][7] <= size && triangles[j][7] >= -size && 
       triangles[j][2] <= size && triangles[j][2] >= -size && 
       triangles[j][5] <= size && triangles[j][5] >= -size && 
       triangles[j][8] <= size && triangles[j][8] >= -size )
      {
	if (j % 2 == 0) {
	  v1[0] = triangles[j][3] - triangles[j][0];
	  v1[1] = triangles[j][4] - triangles[j][1];
	  v1[2] = triangles[j][5] - triangles[j][2];
	  v2[0] = triangles[j][6] - triangles[j][0];
	  v2[1] = triangles[j][7] - triangles[j][1];
	  v2[2] = triangles[j][8] - triangles[j][2];
	  }
	gaiseki(v1, v2, normal);
	glNormal3fv(normal);
	
	if (j < tr_i)
      	glColor3f((float)2*(tr_i-1.0-j)/(float)(tr_i-1.0), 0.6,
		  (float)2*j/(float)(tr_i-1.0)/*, 1.0*/);
	else
      	glColor3f(0.6, (float)2*j/(float)(tr_i-1.0), 
		  (float)2*(tr_i-1.0-j)/(float)(tr_i-1.0)/*, 1.0*/);

	glBegin(GL_TRIANGLES);
	  if (exTex == 1)
	    glTexCoord2f(texturecoords[j][0],texturecoords[j][1]);
	  glVertex3f(triangles[j][0],triangles[j][1],triangles[j][2]);
	  if (exTex == 1)
	    glTexCoord2f(texturecoords[j][2],texturecoords[j][3]);
	  glVertex3f(triangles[j][3],triangles[j][4],triangles[j][5]);
	  if (exTex == 1)
	    glTexCoord2f(texturecoords[j][4],texturecoords[j][5]);
	  glVertex3f(triangles[j][6],triangles[j][7],triangles[j][8]);
	glEnd();

	if( exNet ) {
	  glColor3f(1.0,1.0,1.0);
	  if(!blendSwitch) {
	    glDisable(GL_LIGHTING);
	    glDisable(GL_TEXTURE_2D);
	    glColor3f(0.0,0.0,0.0);
	  }
	  glBegin(GL_LINE_LOOP);
	    glVertex3f(triangles[j][0],triangles[j][1],triangles[j][2]);
	    glVertex3f(triangles[j][3],triangles[j][4],triangles[j][5]);
	    glVertex3f(triangles[j][6],triangles[j][7],triangles[j][8]);
	  glEnd();
	  if(lightSwitch)
	    glEnable(GL_LIGHTING);
	  if(textureSwitch) 
	    glEnable(GL_TEXTURE_2D); 
	}
      }
  }
}

static void box(void)
{
  double i;
  int a = -1;
  double line_length;

  glColor3f(1.0,1.0,1.0);
     glBegin(GL_LINES);
     glVertex3f(-size, -size, -size);  glVertex3f(size, -size, -size);
     glVertex3f(size, -size, -size);  glVertex3f(size, -size, size);
     glVertex3f(size, -size, size);  glVertex3f(-size, -size, size);
     glVertex3f(-size, -size, size);  glVertex3f(-size, -size, -size);


     glVertex3f(-size, -size, -size);  glVertex3f(-size, size, -size);
     glVertex3f(size, -size, -size);  glVertex3f(size, size, -size);
     glVertex3f(size, -size, size);  glVertex3f(size, size, size);
     glVertex3f(-size, -size, size);  glVertex3f(-size, size, size);

     glVertex3f(-size, size, -size);  glVertex3f(size, size, -size);
     glVertex3f(size, size, -size);  glVertex3f(size, size, size);
     glVertex3f(size, size, size);  glVertex3f(-size, size, size);
     glVertex3f(-size, size, size);  glVertex3f(-size, size, -size);
     glEnd();
     
     for ( a=-1; a<=1; a+=2) {
       for (i = 0.0; i <= size; i += INTERVAL){
	 if ( i == 0.0 ) line_length = 1.5;
	 else line_length = 0.5;
	 glBegin(GL_LINES);
           glVertex3f(a*i, size, size); glVertex3f(a*i, size-line_length, size);
	   glVertex3f(a*i, size, size); glVertex3f(a*i, size, size-line_length);
	   glVertex3f(size, a*i, size); glVertex3f(size-line_length, a*i, size);
	   glVertex3f(size, a*i, size); glVertex3f(size, a*i, size-line_length);
	   glVertex3f(size, size, a*i); glVertex3f(size-line_length, size, a*i);
	   glVertex3f(size, size, a*i); glVertex3f(size, size-line_length, a*i);
	   
	   glVertex3f(a*i, -size, size); glVertex3f(a*i, -(size-line_length), size);
	   glVertex3f(a*i, -size, size); glVertex3f(a*i, -size, size-line_length);
	   glVertex3f(size, a*i, -size); glVertex3f(size-line_length, a*i, -size);
	   glVertex3f(size, a*i, -size); glVertex3f(size, a*i, -(size-line_length));
	   glVertex3f(-size, size, a*i); glVertex3f(-size, size-line_length, a*i);
	   glVertex3f(-size, size, a*i); glVertex3f(-(size-line_length), size, a*i);
	   
	   glVertex3f(a*i, size, -size); glVertex3f(a*i, size-line_length, -size);
	   glVertex3f(a*i, size, -size); glVertex3f(a*i, size, -(size-line_length));
	   glVertex3f(-size, a*i, size); glVertex3f(-(size-line_length), a*i, size);
	   glVertex3f(-size, a*i, size); glVertex3f(-size, a*i, size-line_length);
	   glVertex3f(size, -size, a*i); glVertex3f(size, -(size-line_length), a*i);
	   glVertex3f(size, -size, a*i); glVertex3f(size-line_length, -size, a*i);
	   
	   glVertex3f(a*i, -size, -size); glVertex3f(a*i, -(size-line_length), -size);
	   glVertex3f(a*i, -size, -size); glVertex3f(a*i, -size, -(size-line_length));
	   glVertex3f(-size, a*i, -size); glVertex3f(-(size-line_length), a*i, -size);
	   glVertex3f(-size, a*i, -size); glVertex3f(-size, a*i, -(size-line_length));
	   glVertex3f(-size, -size, a*i); glVertex3f(-size, -(size-line_length), a*i);
	   glVertex3f(-size, -size, a*i); glVertex3f(-(size-line_length), -size, a*i);
         glEnd();
       }
     }
}

static void axis(void)
{
  double i;
  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(-size, 0, 0);  glVertex3f(size, 0, 0);

  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0, -size, 0);  glVertex3f(0, size, 0);
  
  glColor3f(1.0, 1.0, 0.0);
  glVertex3f(0, 0, -size);  glVertex3f(0, 0, size);
  glEnd();
  glLineWidth(1.0);

  glColor3f(1.0, 0.0, 0.0);
  glRasterPos3i(size+1,0,0);
  glBitmap (10, 12, 0.0, 0.0, 11.0, 0.0, raster_X);

  glColor3f(0.0, 0.0, 1.0);
  glRasterPos3i(0,size+1,0);
  glBitmap (10, 12, 0.0, 0.0, 11.0, 0.0, raster_Y);

  glColor3f(1.0, 1.0, 0.0);
  glRasterPos3i(0,0,size+1);
  glBitmap (10, 12, 0.0, 0.0, 11.0, 0.0, raster_Z);

  glColor3f(1.0, 1.0, 1.0);
  glRasterPos3i(5,1,0);
  glBitmap (10, 22, 0.0, 0.0, 11.0, 0.0, raster_$);

  glColor3f(1.0, 1.0, 1.0);
  glRasterPos3i(1,0,z_interval);
  glBitmap (10, 20, 0.0, 0.0, 11.0, 0.0, raster_at);

   for (i = 0.0; i <= size; i += INTERVAL){
     glBegin(GL_LINES);
       glVertex3f(i, 0, 0); glVertex3f(i, 0.5, 0);
       glVertex3f(i, 0, 0); glVertex3f(i, 0, 0.5);
       glVertex3f(0, i, 0); glVertex3f(0.5, i, 0);
       glVertex3f(0, i, 0); glVertex3f(0, i, 0.5);

       glVertex3f(-i, 0, 0); glVertex3f(-i, 0.5, 0);
       glVertex3f(-i, 0, 0); glVertex3f(-i, 0, 0.5);
       glVertex3f(0, -i, 0); glVertex3f(0.5, -i, 0);
       glVertex3f(0, -i, 0); glVertex3f(0, -i, 0.5);

     glEnd();
   }
   for (i = 0.0; i <= size; i += z_interval){
     glBegin(GL_LINES);
       glVertex3f(0, 0, i); glVertex3f(0.5, 0, i);
       glVertex3f(0, 0, i); glVertex3f(0, 0.5, i);

       glVertex3f(0, 0, -i); glVertex3f(0.5, 0, -i);
       glVertex3f(0, 0, -i); glVertex3f(0, 0.5, -i);
     glEnd();
   }
}

static void recalcGraph(void)
{
  theBox = glGenLists(1);
  glNewList(theBox, GL_COMPILE);
  box();
  glEndList();
  
  theAxis = glGenLists(1);
  glNewList(theAxis, GL_COMPILE);
  axis();
  glEndList();

  theGraph = glGenLists(1);
  glNewList(theGraph, GL_COMPILE);
  graph();
  glEndList();

  theTriangle = glGenLists(1);
  glNewList(theTriangle, GL_COMPILE);
  triangle();
  glEndList();


  glCallList(theAxis);
  glCallList(theBox);

  newGraph = 0;
}

static void init(void)
{
  static GLfloat amb[] = {0.4, 0.4, 0.4, 0.0};
  static GLfloat dif[] = {1.0, 1.0, 1.0, 0.0};
  
  glClearColor(0.2, 0.2, 0.2, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
  /* 
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
  */
  glMatrixMode(GL_PROJECTION);
  gluPerspective(40.0, 1.0, 1.0, 500.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 80.0,0.0, 0.0, 0.0, 0.0, 1.0, 0.);
  glPushMatrix();
  
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
  
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_ONE, GL_ONE);
}

void initTexture(void)
{
  makeTexture();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &texInobuta);
  glBindTexture(GL_TEXTURE_2D, texInobuta);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_WIDTH, 
	       TEX_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

void
recalcModelView(void)
{
  glPopMatrix();
  
  glPushMatrix();
  glRotatef(angle[0], 0.0, 1.0, 0.0);
  glRotatef(angle[1], 1.0, 0.0, 0.0);
  glScalef(mag, mag, mag);
  newModel = 0;
}

void display(void)
{
  if (newModel)
    recalcModelView();
  if (newGraph)
    recalcGraph();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
    glScalef(1.0,1.0,z_syukusyou);
    if(triSwitch == 0) 
      glCallList(theGraph);
    else 
      glCallList(theTriangle);
  glPopMatrix();

  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  glCallList(theBox);
  if(exAxis)
    glCallList(theAxis);

  if(lightSwitch)
    glEnable(GL_LIGHTING);
  if(textureSwitch)
    glEnable(GL_TEXTURE_2D);

  glutSwapBuffers();

}

void reshape(int w, int h)
{
  glViewport(0,0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50.0, (double)w/(double)h, 1.0, 1000.0);
  glMatrixMode(GL_MODELVIEW);
}


void mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    moving = 1;
    begin[0] = x;
    begin[1] = y;
  }
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    moving = 0;
  }
}

void motion(int x, int y)
{
  if (moving) {
    angle[0] = angle[0] + (x - begin[0]);
    angle[1] = angle[1] + (y - begin[1]);
    begin[0] = x;
    begin[1] = y;
    newModel = 1;
    glutPostRedisplay();
  }
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 's':
    size += BOXSIZE;
    size_z += BOXSIZE;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 'S':
    size -= BOXSIZE;
    size_z -= BOXSIZE;
    if ( size < 0.0 )
      size = 0.0;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 'l':
    if (triSwitch == 0) {
      len += FUNCLEN;
      newGraph = 1;
      glutPostRedisplay();
    }
    break;
  case 'L':
    if (triSwitch == 0) {
      len -= FUNCLEN;
      if ( len < 0.0 )
	len = 0.0; 
      newGraph = 1;
      glutPostRedisplay();
    }
    break;
  case 'm':
    mag *= MAG;
    newModel = 1;
    glutPostRedisplay();
    break;
  case 'M':
    mag /= MAG;
    newModel = 1;
    glutPostRedisplay();
    break;
  case 'z':
    z_syukusyou *= MAG;
    z_interval *= MAG;
    size_z /= MAG;
    newModel = 1;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 'Z':
    z_syukusyou /= MAG;
    z_interval /= MAG;
    size_z *= MAG;
    newModel = 1;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 'i':
  case 'I':
    mag = 1.0;
    z_syukusyou = 1.0;
    z_interval = 5.0;
    size = 12;
    size_z = 12.0;
    len = 3.0;
    /*
    angle[0] = 0.0;
    angle[1] = 0.0;
    */
    newModel = 1;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 'q':
  case 'Q':
    exit(0);
    break;
  }
}

GLboolean netSwitch = GL_FALSE;
void controlExpression(int value)
{
  switch (value) {
  case 1:
    lightSwitch = 0;
    textureSwitch = 0;
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);  
    newModel = 1;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 2:
    lightSwitch = 1;
    textureSwitch = 0;
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    newModel = 1;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 3:
    textureSwitch = 1;
    lightSwitch = 0;
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    newModel = 1;
    newGraph = 1;
    glutPostRedisplay();
   break;
  case 4:
    blendSwitch = !blendSwitch;
    if (blendSwitch) {
      glEnable(GL_BLEND);
      glDisable(GL_DEPTH_TEST);
    }
    else {
      glDisable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);
    }
    newGraph = 1;
    break;
  case 5:
    exAxis = !exAxis;
    break;
  case 6:
    exNet = !exNet;
    newGraph = 1;
    break;
  case 7:
    triSwitch = !triSwitch;
    break;
  case 8:
    netSwitch = !netSwitch;
    if ( netSwitch )
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else 
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    newGraph = 1;
    break;
  case 9:
    mag *= MAG;
    newModel = 1;
    glutPostRedisplay();
    break;
  case 10:
    mag /= MAG;
    newModel = 1;
    glutPostRedisplay();
    break;
  case 11:
    size += BOXSIZE;
    size_z += BOXSIZE;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 12:
    size -= BOXSIZE;
    size_z -= BOXSIZE;
    if ( size < 0.0 )
      size = 0.0;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 13:
    z_syukusyou *= MAG;
    z_interval *= MAG;
    size_z /= MAG;
    newModel = 1;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 14:
    z_syukusyou /= MAG;
    z_interval /= MAG;
    size_z *= MAG;
    newModel = 1;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 16:
    mag = 1.0;
    z_syukusyou = 1.0;
    z_interval = 5.0;
    size = 12;
    size_z = 12.0;
    /*
    angle[0] = 0.0;
    angle[1] = 0.0;
    */
    len = 3.0;
    newModel = 1;
    newGraph = 1;
    glutPostRedisplay();
    break;
  case 15:
    exit(0);
    break;
  }
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
  FILE *fp;
  char com[LINE_MAX+1];
  double x1,y1,z1;
  double x2,y2,z2;
  double x3,y3,z3;
  double tex_x1,tex_y1;
  double tex_x2,tex_y2;
  double tex_x3,tex_y3;
  int limit1 = LIMIT;
  int limit2 = LIMIT;
  double **t1;
  double **t2;
  double *tr;
  double *te;

  if (argc == 1) {
    triSwitch = 0;
    exTex = 1;
    exSample = 0;
    printf("$=5.0, @=5.0\n"); 
  }
  else if (argc != 2) {
    fprintf(stderr,"Usage: graph filename\n");
  }
  else {
    triangles = (double **) malloc(sizeof(double *)*limit1);
    texturecoords = (double **) malloc(sizeof(double *)*limit2);
    
    if (triangles == NULL)
      outOfMemory();
    if (texturecoords == NULL)
      outOfMemory();  
    
    fp = fopen(argv[1],"r");
    
    if (fp == NULL) {
      fprintf(stderr,"The file %s is not found.\n",argv[1]);
      return -1;
    }
    
    tr_i = 0;
    te_i = 0;
    
    while (fgets(com,LINE_MAX,fp) != NULL) {
      if (com[0] <= ' ') 
	continue;
      
      switch (com[0]) {
      case '#':  /* comment line */
	break;
	
      case 't':  /* read data for a triangle. */
	sscanf(&(com[1]),"%lf %lf %lf %lf %lf %lf %lf %lf %lf",
	       &x1,&y1,&z1,&x2,&y2,&z2,&x3,&y3,&z3);
	tr = (double *) malloc(sizeof(double)*9);
	if (tr == NULL) 
	  outOfMemory();
	tr[0] = x1; tr[1] = y1; tr[2] = z1;
	tr[3] = x2; tr[4] = y2; tr[5] = z2;
	tr[6] = x3; tr[7] = y3; tr[8] = z3;
	
	triangles[tr_i] = tr;
	tr_i++;
	
	if (tr_i >= limit1) {
	  t1 = (double **) malloc(sizeof(double *)*limit1*2);
	  if (t1 == NULL) outOfMemory();
	  for (tr_i=0; tr_i<limit1; tr_i++)
	    t1[tr_i] = triangles[tr_i]; 
	  free(triangles);
	  triangles=t1;
	  limit1 = limit1*2;
	}
	break;
	
      case 'x':  /* read data for a texture coordinate. */ 
	sscanf(&(com[1]),"%lf %lf %lf %lf %lf %lf", 
	       &tex_x1, &tex_y1, &tex_x2, &tex_y2, &tex_x3, &tex_y3);
	te = (double *) malloc(sizeof(double)*6);
	exTex = 1;
	if (te == NULL) 
	  outOfMemory();
	te[0] = tex_x1; te[1] = tex_y1;
	te[2] = tex_x2; te[3] = tex_y2;
	te[4] = tex_x3; te[5] = tex_y3;
	
	texturecoords[te_i] = te;
	te_i++;
	
	if (te_i >= limit2) {
	  t2 = (double **) malloc(sizeof(double *)*limit2*2);
	  if (t2 == NULL) outOfMemory();
	  for (te_i=0; te_i<limit2; te_i++)
	    t2[te_i] = texturecoords[te_i]; 
	  free(texturecoords);
	  texturecoords=t2;
	  limit2 = limit2*2;
	}
	break;
	
      default:
	fprintf(stderr,"Undefined command : %s \n",com);
	return -1;
      }
    } 
  }
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DOUBLE |
		      GLUT_DEPTH | GLUT_MULTISAMPLE);
#if defined(__APPLE__)
  glutInitWindowSize(512, 512);
#else
  glutInitWindowSize(500, 500);
#endif
  glutCreateWindow(argv[0]);
  init();
  initTexture();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  glutCreateMenu(controlExpression);
  glutAddMenuEntry("Default", 1);
  glutAddMenuEntry("Add Light", 2);
  if ( exTex )
    glutAddMenuEntry("Add Texture", 3);
  glutAddMenuEntry("Turn on/off Translucent Mode", 4);
  glutAddMenuEntry("Turn on/off Wireframe Mode", 8);
  glutAddMenuEntry("Hide/Show Axis", 5);
  glutAddMenuEntry("Hide/Show Net", 6);
  glutAddMenuEntry("Magnify (m)", 9);
  glutAddMenuEntry("Reduce (M)", 10);
  glutAddMenuEntry("Make the Box Larger (s)", 11);
  glutAddMenuEntry("Make the Box Smaller (S)", 12);
  glutAddMenuEntry("Magnify z Direction (z)", 13);
  glutAddMenuEntry("Reduce z Direction (Z)", 14);
  glutAddMenuEntry("Initialize (i)", 16);
  if ( exSample ) {
      glutAddMenuEntry("Change Sample/Data Graph", 7);
  }
  glutAddMenuEntry("Quit (q)", 15);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
  glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
  glLightfv(GL_LIGHT3, GL_DIFFUSE, light3_diffuse);
  glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
  glLightfv(GL_LIGHT4, GL_DIFFUSE, light4_diffuse);
  glLightfv(GL_LIGHT5, GL_POSITION, light5_position);
  glLightfv(GL_LIGHT5, GL_DIFFUSE, light5_diffuse);
  
  glEnable(GL_LIGHT0); 
  glEnable(GL_LIGHT1);

  glutMainLoop();
  return 0;
}


