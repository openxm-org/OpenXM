#include <stdio.h>
#include <math.h>
#include "myenv.h"
#include "mygl.h"

#ifdef COCOA
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif


#ifdef MYTEST
main(int argc,char **argv) {
  test_main(argc,argv);
}
void ourDisplay() {
  glib3_std_scene0(0.0, 0.0, 3.0);
  glib3_icosahedron(0.0, 0.0, -3.0, 1.0);
  printf("ourDisplay is called.\n");
}
int test_main(int argc,char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(400,400);
  glutInitWindowPosition(100,100);
  glutCreateWindow("glicosahedron");

  glib3_std_scene0(0.0, 0.0, 3.0);
  glutDisplayFunc(ourDisplay);
  glutReshapeFunc(ourDisplay);
  glutMainLoop();
  return 0;
}
#endif

void glib3_std_scene0_aux(int frustum, int usebox, 
                          float xnear, float ynear, float znear,
                          float left,float right,float bottom,float top,float near,float far) {
  GLfloat light_position0[] = { 10.0, 10.0, 10.0, 1.0 };
  GLfloat light_position1[] = { -10.0, 10.0, 10.0, 1.0 }; 
  GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat diffuse[] = { 0.1, 0.8, 0.8, 1.0 };
  GLfloat specular[] = { 0.9, 0.9, 0.9, 1.0 };
  GLfloat shininess[] = { 70.0 };
  GLfloat std_light_param0[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat std_light_param[] = { 0.5, 0.5, 0.5, 1.0 };

  glShadeModel(GL_SMOOTH);  // GL_SMOOTH or GL_FLAT
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);

  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (frustum) {
	if (znear >= 2.0) gluPerspective(40.0,1.0,1.0,200.0);
	else gluPerspective(40.0,1.0,znear/2,20.0);
  }else{
    glOrtho(left,right,bottom,top,near,far);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(xnear,ynear,znear,0.0,0.0,0.0,0.0,1.0,0.0);

  
  glLightfv( GL_LIGHT0, GL_AMBIENT, ambient ); 
  /* glLightfv( GL_LIGHT0, GL_AMBIENT, std_light_param ); */
  glLightfv( GL_LIGHT0, GL_DIFFUSE, std_light_param );
  glLightfv( GL_LIGHT0, GL_SPECULAR, std_light_param ); 
  glLightfv( GL_LIGHT0, GL_POSITION, light_position0 );

  glLightfv( GL_LIGHT1, GL_DIFFUSE, std_light_param );
  glLightfv( GL_LIGHT1, GL_SPECULAR, std_light_param ); 

  glLightfv( GL_LIGHT1, GL_POSITION, light_position1 );
  glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
  glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
  glMaterialfv( GL_FRONT, GL_SHININESS, shininess );

  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 ); 
  glEnable( GL_LIGHT1 );

  glEnable(GL_COLOR_MATERIAL); 

  if (usebox > 0) glib3_bounding_box((float) usebox);
  
}
void glib3_std_scene0(float xnear, float ynear, float znear) {
  glib3_std_scene0_aux(1,0,xnear,  ynear, znear, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
}
void glib3_std_scene0_ortho(float xnear, float ynear,float znear,float size) {
  glib3_std_scene0_aux(0,0, xnear, ynear, znear, 
           (-1.0+xnear)*size, (1.0+xnear)*size,
           (-1.0+ynear)*size, (1.0+ynear)*size, -100.0*size, 100.0*size);
}

#define X 0.525731112119133606
#define Z 0.850650808352039932
static void normalize(float v[3]) {
  float norm;
  norm = sqrt((double) (v[0]*v[0]+v[1]*v[1]+v[2]*v[2])); 
  if (norm > 0.0) {
    v[0] = v[0]/norm;
	v[1] = v[1]/norm;
	v[2] = v[2]/norm;
  }
}
static void outerProd(float v1[3], float v2[3], float ans[3]) {
  ans[0] = v1[1]*v2[2]-v1[2]*v2[1];
  ans[1] = v1[2]*v2[0]-v1[0]*v2[2];
  ans[2] = v1[0]*v2[1]-v1[1]*v2[0];
  normalize(ans);
}
static int icosaFace[20][3] = {
  {0,4,1},{0,9,4}, {9,5,4},{4,5,8}, {4,8,1},
  {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
  {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
  {6,1,10},{9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} 
};
void glib3_icosahedron(float x,float y,float z, float size) {
  float vv[12][3];
  int i,j;
  float e1[3], e2[3], normal[3];
  static int nnn=0;
  nnn++; if (nnn > 20) nnn=1;
  for (i=0; i<12; i++) {vv[i][0] = x; vv[i][1] = y; vv[i][2] = z; }
  vv[0][0] += -X*size, vv[0][1] += 0.0; vv[0][2] += Z*size;
  vv[1][0] +=  X*size, vv[1][1] += 0.0; vv[1][2] += Z*size;
  vv[2][0] += -X*size, vv[2][1] += 0.0; vv[2][2] += -Z*size;
  vv[3][0] +=  X*size, vv[3][1] += 0.0; vv[3][2] += -Z*size;
  vv[4][0] +=  0.0, vv[4][1] += Z*size; vv[4][2] += X*size;
  vv[5][0] +=  0.0, vv[5][1] += Z*size; vv[5][2] += -X*size;
  vv[6][0] +=  0.0, vv[6][1] += -Z*size; vv[6][2] += X*size;
  vv[7][0] +=  0.0, vv[7][1] += -Z*size; vv[7][2] += -X*size;
  vv[8][0] +=  Z*size, vv[8][1] += X*size; vv[8][2] += 0.0;
  vv[9][0] +=  -Z*size, vv[9][1] += X*size; vv[9][2] += 0.0;
  vv[10][0] +=  Z*size, vv[10][1] += -X*size; vv[10][2] += 0.0;
  vv[11][0] +=  -Z*size, vv[11][1] += -X*size; vv[11][2] += 0.0;

  // glMaterialf(GL_FRONT,GL_SHININESS,80.0);
  glColor3f(1.0,0.0,0.0);
  // glEnable(GL_AUTO_NORMAL);
  for (i=0; i<20; i++) { // use 20 or use "nnn"
	if (i%3 == 0) glColor3f(1.0,0.0,0.0);
	else if (i%3 == 1) glColor3f((i%10)/10.0,1.0,0.0);
	else glColor3f((i%10)/10.0, 0.0, 1.0);
	for (j=0; j<3; j++) {
	  e1[j] = vv[icosaFace[i][0]][j] - vv[icosaFace[i][1]][j];
	  e2[j] = vv[icosaFace[i][1]][j] - vv[icosaFace[i][2]][j];
	}
    outerProd(e1,e2,normal);
    glNormal3fv(normal);

//    NSLog(@"icosaface=(%d,%d,%d)\n",icosaFace[i][0],icosaFace[i][1],icosaFace[i][2]);
    glBegin(GL_TRIANGLES);
	glVertex3fv(&(vv[icosaFace[i][0]][0]));
	glVertex3fv(&(vv[icosaFace[i][1]][0]));
	glVertex3fv(&(vv[icosaFace[i][2]][0]));
    glEnd();
  }

}
// Draw a wireflame of [-box,box]^3.
void glib3_bounding_box(float box) {
  glPolygonMode(GL_FRONT,GL_LINE);
  glPolygonMode(GL_BACK,GL_LINE);

// fix z  back
 glBegin(GL_POLYGON);
 glColor3f(0,1.0,0);
 glNormal3f(0,0,1);
 glVertex3f(-box,-box,-box); glVertex3f(box,-box,-box); glVertex3f(box,box,-box); 
 glVertex3f(-box,box,-box); glVertex3f(-box,-box,-box);
 glEnd();

 //  fix z, front
 glBegin(GL_POLYGON);
 glNormal3f(0,0,1);  // 0,0,-1 : invisible
 glVertex3f(-box,-box,box); glVertex3f(box,-box,box); glVertex3f(box,box,box); 
 glVertex3f(-box,box,box); glVertex3f(-box,-box,box);
 glEnd();
 
// fix x    left box
 glBegin(GL_POLYGON);
 glNormal3f(1,0,0);
 glVertex3f(-box,-box,-box); glVertex3f(-box,box,-box); glVertex3f(-box,box,box); 
 glVertex3f(-box,-box,box); glVertex3f(-box,-box,-box);
 glEnd();

 // right box
 glBegin(GL_POLYGON);
 glNormal3f(1,0,0);
 glVertex3f(box,-box,-box); glVertex3f(box,box,-box); glVertex3f(box,box,box); 
 glVertex3f(box,-box,box); glVertex3f(box,-box,-box);
 glEnd();

 glPolygonMode(GL_FRONT,GL_FILL);
 glPolygonMode(GL_BACK,GL_FILL);

}

void glib3_test_depth() {
  int i,j;
  float vv0[3];
  float vv1[3];
  float vv2[3];
  
  vv0[0] = 0.0; vv0[1] = 0.0; vv0[2] = 0.0;
  vv1[0] = 0.0; vv1[1] = 1.0; vv1[2] = 0.0;
  vv2[0] = 1.0; vv2[1] = 0.0; vv2[2] = 0.0;
  for (i=0; i<20; i++) { // use 20 or use "nnn"
	if (i%3 == 0) glColor3f(1.0,0.0,0.0);
	else if (i%3 == 1) glColor3f(0.0,1.0,0.0);
	else glColor3f(0.0, 0.0, 1.0);

    vv1[1] += i/10.0;
    vv0[2] = vv1[2] = vv2[2] = -i/10.0;
    glBegin(GL_TRIANGLES);
	glVertex3fv(vv0);
	glVertex3fv(vv1);
	glVertex3fv(vv2);
    glEnd();
  }
}
