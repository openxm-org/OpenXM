/*
 * This program is under the GNU GPL.
 * Use at your own risk.
 *
 * written by David Bucciarelli (tech.hmw@plus.it)
 *            Humanware s.r.l.
 * MesaDemo. Mesa-5.1/progs/demos/ray.c. Modified by Nobuki Takayama. 2006-02-22.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "myenv.h"

#ifdef COCOA
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#else
#include <GL/glut.h>
#endif

#include "mygl.h"

static int WIDTH = 640;
static int HEIGHT = 480;

static GLint T0 = 0;
static GLint Frames = 0;

#define BASESIZE 7.5f
#define SPHERE_RADIUS 0.75f

#define TEX_CHECK_WIDTH 256
#define TEX_CHECK_HEIGHT 256
#define TEX_CHECK_SLOT_SIZE (TEX_CHECK_HEIGHT/16)
#define TEX_CHECK_NUMSLOT (TEX_CHECK_HEIGHT/TEX_CHECK_SLOT_SIZE)

#define TEX_REFLECT_WIDTH 256
#define TEX_REFLECT_HEIGHT 256
#define TEX_REFLECT_SLOT_SIZE (TEX_REFLECT_HEIGHT/16)
#define TEX_REFLECT_NUMSLOT (TEX_REFLECT_HEIGHT/TEX_REFLECT_SLOT_SIZE)

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#define EPSILON 0.0001

#define clamp255(a)  ( (a)<(0.0f) ? (0.0f) : ((a)>(255.0f) ? (255.0f) : (a)) )

#define fabs(x) ((x)<0.0f?-(x):(x))

#define vequ(a,b) { (a)[0]=(b)[0]; (a)[1]=(b)[1]; (a)[2]=(b)[2]; }
#define vsub(a,b,c) { (a)[0]=(b)[0]-(c)[0]; (a)[1]=(b)[1]-(c)[1]; (a)[2]=(b)[2]-(c)[2]; }
#define	dprod(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define vnormalize(a,b) { \
  register float m_norm; \
  m_norm=sqrt((double)dprod((a),(a))); \
  (a)[0] /=m_norm; \
  (a)[1] /=m_norm; \
  (a)[2] /=m_norm; }

static GLubyte checkmap[TEX_CHECK_HEIGHT][TEX_CHECK_WIDTH][3];
static GLuint checkid;
static int checkmap_currentslot = 0;

static GLubyte reflectmap[TEX_REFLECT_HEIGHT][TEX_REFLECT_WIDTH][3];
static GLuint reflectid;
static int reflectmap_currentslot = 0;

static GLuint lightdlist;
static GLuint objdlist;

static float lightpos[3] = { 2.1, 2.1, 2.8 };
static float objpos[3] = { 0.0, 0.0, 1.0 };

static float sphere_pos[TEX_CHECK_HEIGHT][TEX_REFLECT_WIDTH][3];

static int win = 0;

static float fogcolor[4] = { 0.05, 0.05, 0.05, 1.0 };

static float obs[3] = { 7.0, 0.0, 2.0 };
static float dir[3];
static float v = 0.0;
static float alpha = -90.0;
static float beta = 90.0;

static int fog = 1;
static int bfcull = 1;
static int poutline = 0;
static int help = 1;
static int showcheckmap = 1;
static int showreflectmap = 1;
static int joyavailable = 0;
static int joyactive = 0;

static void
calcposobs(void)
{
   dir[0] = sin(alpha * M_PI / 180.0);
   dir[1] = cos(alpha * M_PI / 180.0) * sin(beta * M_PI / 180.0);
   dir[2] = cos(beta * M_PI / 180.0);

   if (dir[0] < 1.0e-5 && dir[0] > -1.0e-5)
      dir[0] = 0;
   if (dir[1] < 1.0e-5 && dir[1] > -1.0e-5)
      dir[1] = 0;
   if (dir[2] < 1.0e-5 && dir[2] > -1.0e-5)
      dir[2] = 0;

   obs[0] += v * dir[0];
   obs[1] += v * dir[1];
   obs[2] += v * dir[2];
}



static void
reshape(int w, int h)
{
   WIDTH = w;
   HEIGHT = h;
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(45.0, w / (float) h, 0.8, 40.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}



static GLboolean
seelight(float p[3], float dir[3])
{
   float c[3], b, a, d, t, dist[3];

   vsub(c, p, objpos);
   b = -dprod(c, dir);
   a = dprod(c, c) - SPHERE_RADIUS * SPHERE_RADIUS;

   if ((d = b * b - a) < 0.0 || (b < 0.0 && a > 0.0))
      return GL_FALSE;

   d = sqrt(d);

   t = b - d;

   if (t < EPSILON) {
      t = b + d;
      if (t < EPSILON)
	 return GL_FALSE;
   }

   vsub(dist, lightpos, p);
   if (dprod(dist, dist) < t * t)
      return GL_FALSE;

   return GL_TRUE;
}

static int
colorcheckmap(float ppos[3], float c[3])
{
   static float norm[3] = { 0.0f, 0.0f, 1.0f };
   float ldir[3], vdir[3], h[3], dfact, kfact, r, g, b;
   int x, y;

   x = (int) ((ppos[0] + BASESIZE / 2) * (10.0f / BASESIZE));
   if ((x < 0) || (x > 10))
      return GL_FALSE;

   y = (int) ((ppos[1] + BASESIZE / 2) * (10.0f / BASESIZE));
   if ((y < 0) || (y > 10))
      return GL_FALSE;

   r = 255.0f;
   if (y & 1) {
      if (x & 1)
	 g = 255.0f;
      else
	 g = 0.0f;
   }
   else {
      if (x & 1)
	 g = 0.0f;
      else
	 g = 255.0f;
   }
   b = 0.0f;

   vsub(ldir, lightpos, ppos);
   vnormalize(ldir, ldir);

   if (seelight(ppos, ldir)) {
      c[0] = r * 0.05f;
      c[1] = g * 0.05f;
      c[2] = b * 0.05f;

      return GL_TRUE;
   }

   dfact = dprod(ldir, norm);
   if (dfact < 0.0f)
      dfact = 0.0f;

   vsub(vdir, obs, ppos);
   vnormalize(vdir, vdir);
   h[0] = 0.5f * (vdir[0] + ldir[0]);
   h[1] = 0.5f * (vdir[1] + ldir[1]);
   h[2] = 0.5f * (vdir[2] + ldir[2]);
   kfact = dprod(h, norm);
   kfact = pow(kfact, 6.0) * 7.0 * 255.0;

   r = r * dfact + kfact;
   g = g * dfact + kfact;
   b = b * dfact + kfact;

   c[0] = clamp255(r);
   c[1] = clamp255(g);
   c[2] = clamp255(b);

   return GL_TRUE;
}

static void
updatecheckmap(int slot)
{
   float c[3], ppos[3];
   int x, y;

   glBindTexture(GL_TEXTURE_2D, checkid);

   ppos[2] = 0.0f;
   for (y = slot * TEX_CHECK_SLOT_SIZE; y < (slot + 1) * TEX_CHECK_SLOT_SIZE;
	y++) {
      ppos[1] = (y / (float) TEX_CHECK_HEIGHT) * BASESIZE - BASESIZE / 2;

      for (x = 0; x < TEX_CHECK_WIDTH; x++) {
	 ppos[0] = (x / (float) TEX_CHECK_WIDTH) * BASESIZE - BASESIZE / 2;

	 colorcheckmap(ppos, c);
	 checkmap[y][x][0] = (GLubyte) c[0];
	 checkmap[y][x][1] = (GLubyte) c[1];
	 checkmap[y][x][2] = (GLubyte) c[2];
      }
   }

   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, slot * TEX_CHECK_SLOT_SIZE,
		   TEX_CHECK_WIDTH, TEX_CHECK_SLOT_SIZE, GL_RGB,
		   GL_UNSIGNED_BYTE,
		   &checkmap[slot * TEX_CHECK_SLOT_SIZE][0][0]);

}

static void
updatereflectmap(int slot)
{
   float rf, r, g, b, t, dfact, kfact, rdir[3];
   float rcol[3], ppos[3], norm[3], ldir[3], h[3], vdir[3], planepos[3];
   int x, y;

   glBindTexture(GL_TEXTURE_2D, reflectid);

   for (y = slot * TEX_REFLECT_SLOT_SIZE;
	y < (slot + 1) * TEX_REFLECT_SLOT_SIZE; y++)
      for (x = 0; x < TEX_REFLECT_WIDTH; x++) {
	 ppos[0] = sphere_pos[y][x][0] + objpos[0];
	 ppos[1] = sphere_pos[y][x][1] + objpos[1];
	 ppos[2] = sphere_pos[y][x][2] + objpos[2];

	 vsub(norm, ppos, objpos);
	 vnormalize(norm, norm);

	 vsub(ldir, lightpos, ppos);
	 vnormalize(ldir, ldir);
	 vsub(vdir, obs, ppos);
	 vnormalize(vdir, vdir);

	 rf = 2.0f * dprod(norm, vdir);
	 if (rf > EPSILON) {
	    rdir[0] = rf * norm[0] - vdir[0];
	    rdir[1] = rf * norm[1] - vdir[1];
	    rdir[2] = rf * norm[2] - vdir[2];

	    t = -objpos[2] / rdir[2];

	    if (t > EPSILON) {
	       planepos[0] = objpos[0] + t * rdir[0];
	       planepos[1] = objpos[1] + t * rdir[1];
	       planepos[2] = 0.0f;

	       if (!colorcheckmap(planepos, rcol))
		  rcol[0] = rcol[1] = rcol[2] = 0.0f;
	    }
	    else
	       rcol[0] = rcol[1] = rcol[2] = 0.0f;
	 }
	 else
	    rcol[0] = rcol[1] = rcol[2] = 0.0f;

	 dfact = 0.1f * dprod(ldir, norm);

	 if (dfact < 0.0f) {
	    dfact = 0.0f;
	    kfact = 0.0f;
	 }
	 else {
	    h[0] = 0.5f * (vdir[0] + ldir[0]);
	    h[1] = 0.5f * (vdir[1] + ldir[1]);
	    h[2] = 0.5f * (vdir[2] + ldir[2]);
	    kfact = dprod(h, norm);
            kfact = pow(kfact, 4.0);
            if (kfact < 1.0e-10)
               kfact = 0.0;
         }

	 r = dfact + kfact;
	 g = dfact + kfact;
	 b = dfact + kfact;

	 r *= 255.0f;
	 g *= 255.0f;
	 b *= 255.0f;

	 r += rcol[0];
	 g += rcol[1];
	 b += rcol[2];

	 r = clamp255(r);
	 g = clamp255(g);
	 b = clamp255(b);

	 reflectmap[y][x][0] = (GLubyte) r;
	 reflectmap[y][x][1] = (GLubyte) g;
	 reflectmap[y][x][2] = (GLubyte) b;
      }

   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, slot * TEX_REFLECT_SLOT_SIZE,
		   TEX_REFLECT_WIDTH, TEX_REFLECT_SLOT_SIZE, GL_RGB,
		   GL_UNSIGNED_BYTE,
		   &reflectmap[slot * TEX_REFLECT_SLOT_SIZE][0][0]);
}

static void
drawbase(void)
{
   glColor3f(0.0, 0.0, 0.0);
   glBindTexture(GL_TEXTURE_2D, checkid);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f, 0.0f);
   glVertex3f(-BASESIZE / 2.0f, -BASESIZE / 2.0f, 0.0f);

   glTexCoord2f(1.0f, 0.0f);
   glVertex3f(BASESIZE / 2.0f, -BASESIZE / 2.0f, 0.0f);

   glTexCoord2f(1.0f, 1.0f);
   glVertex3f(BASESIZE / 2.0f, BASESIZE / 2.0f, 0.0f);

   glTexCoord2f(0.0f, 1.0f);
   glVertex3f(-BASESIZE / 2.0f, BASESIZE / 2.0f, 0.0f);

   glEnd();
}

static void
drawobj(void)
{
   glColor3f(0.0, 0.0, 0.0);
   glBindTexture(GL_TEXTURE_2D, reflectid);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   glPushMatrix();
   glTranslatef(objpos[0], objpos[1], objpos[2]);
   glCallList(objdlist);
   glPopMatrix();
}


static void
updatemaps(void)
{
   updatecheckmap(checkmap_currentslot); 
   checkmap_currentslot = (checkmap_currentslot + 1) % TEX_CHECK_NUMSLOT;

   updatereflectmap(reflectmap_currentslot);
   reflectmap_currentslot =
      (reflectmap_currentslot + 1) % TEX_REFLECT_NUMSLOT;
}

static void
draw(void)
{
   static char frbuf[80] = "";


   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glEnable(GL_TEXTURE_2D);
   glEnable(GL_DEPTH_TEST);
   if (fog)
      glEnable(GL_FOG);
   else
      glDisable(GL_FOG);

   glPushMatrix();
   calcposobs();

   gluLookAt(obs[0], obs[1], obs[2],
	     obs[0] + dir[0], obs[1] + dir[1], obs[2] + dir[2],
	     0.0, 0.0, 1.0);

   drawbase();
   drawobj();

   glColor3f(1.0, 1.0, 1.0);
   glDisable(GL_TEXTURE_2D);

   glPushMatrix();
   glTranslatef(lightpos[0], lightpos[1], lightpos[2]);
   glCallList(lightdlist);
   glPopMatrix();

   glPopMatrix();

   glDisable(GL_DEPTH_TEST);
   glDisable(GL_FOG);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(-0.5, 639.5, -0.5, 479.5, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);

   glColor3f(0.0f, 0.3f, 1.0f);

   if (showcheckmap) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, checkid);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glBegin(GL_QUADS);
      glTexCoord2f(1.0f, 0.0f);
      glVertex2i(10, 30);
      glTexCoord2f(1.0f, 1.0f);
      glVertex2i(10 + 90, 30);
      glTexCoord2f(0.0f, 1.0f);
      glVertex2i(10 + 90, 30 + 90);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2i(10, 30 + 90);
      glEnd();

      glDisable(GL_TEXTURE_2D);
      glBegin(GL_LINE_LOOP);
      glVertex2i(10, 30);
      glVertex2i(10 + 90, 30);
      glVertex2i(10 + 90, 30 + 90);
      glVertex2i(10, 30 + 90);
      glEnd();
      glRasterPos2i(105, 65);
   }

   if (showreflectmap) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, reflectid);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glBegin(GL_QUADS);
      glTexCoord2f(1.0f, 0.0f);
      glVertex2i(540, 30);
      glTexCoord2f(1.0f, 1.0f);
      glVertex2i(540 + 90, 30);
      glTexCoord2f(0.0f, 1.0f);
      glVertex2i(540 + 90, 30 + 90);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2i(540, 30 + 90);
      glEnd();

      glDisable(GL_TEXTURE_2D);
      glBegin(GL_LINE_LOOP);
      glVertex2i(540, 30);
      glVertex2i(540 + 90, 30);
      glVertex2i(540 + 90, 30 + 90);
      glVertex2i(540, 30 + 90);
      glEnd();
      glRasterPos2i(360, 65);
   }

   glDisable(GL_TEXTURE_2D);

   glRasterPos2i(10, 10);
   glRasterPos2i(360, 470);


   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

}

static void
inittextures(void)
{
   int y;

   glGenTextures(1, &checkid);
   glBindTexture(GL_TEXTURE_2D, checkid);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glTexImage2D(GL_TEXTURE_2D, 0, 3, TEX_CHECK_WIDTH, TEX_CHECK_HEIGHT,
		0, GL_RGB, GL_UNSIGNED_BYTE, checkmap);

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   for (y = 0; y < TEX_CHECK_NUMSLOT; y++)
      updatecheckmap(y);



   glGenTextures(1, &reflectid);
   glBindTexture(GL_TEXTURE_2D, reflectid);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glTexImage2D(GL_TEXTURE_2D, 0, 3, TEX_REFLECT_WIDTH, TEX_REFLECT_HEIGHT,
		0, GL_RGB, GL_UNSIGNED_BYTE, reflectmap);

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   for (y = 0; y < TEX_REFLECT_NUMSLOT; y++)
      updatereflectmap(y);


}

static void
initspherepos(void)
{
   float alpha, beta, sa, ca, sb, cb;
   int x, y;

   for (y = 0; y < TEX_REFLECT_HEIGHT; y++) {
      beta = M_PI - y * (M_PI / TEX_REFLECT_HEIGHT);

      for (x = 0; x < TEX_REFLECT_WIDTH; x++) {
	 alpha = -x * (2.0f * M_PI / TEX_REFLECT_WIDTH);

	 sa = sin(alpha);
	 ca = cos(alpha);

	 sb = sin(beta);
	 cb = cos(beta);

	 sphere_pos[y][x][0] = SPHERE_RADIUS * sa * sb;
	 sphere_pos[y][x][1] = SPHERE_RADIUS * ca * sb;
	 sphere_pos[y][x][2] = SPHERE_RADIUS * cb;
      }
   }
}

static void
initdlists(void)
{
   GLUquadricObj *obj;

   obj = gluNewQuadric();
   lightdlist = glGenLists(1);
   glNewList(lightdlist, GL_COMPILE);
   gluQuadricDrawStyle(obj, GLU_FILL);
   gluQuadricNormals(obj, GLU_NONE);
   gluQuadricTexture(obj, GL_TRUE);
   gluSphere(obj, 0.25f, 6, 6);  // 0.25 is the radius.
   glEndList();

   objdlist = glGenLists(1);
   glNewList(objdlist, GL_COMPILE);
   gluQuadricDrawStyle(obj, GLU_FILL);
   gluQuadricNormals(obj, GLU_NONE);
   gluQuadricTexture(obj, GL_TRUE);
   gluSphere(obj, SPHERE_RADIUS, 16, 16);  // main sphere radius is 0.75
   glEndList();

}

#ifdef MYTEST
main(int argc, char **argv) {
  /* glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(400,400);
  glutInitWindowPosition(100,100);
  glutCreateWindow("glicosahedron");
  */ // They does not work.

   glutInitWindowPosition(0, 0);
   glutInitWindowSize(WIDTH, HEIGHT);
   // glutInit(&ac, av);

   //glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);

   if (!(win = glutCreateWindow("Ray"))) {
      fprintf(stderr, "Error, couldn't open window\n");
      return -1;
   }


  glib3_ray_init();
  glib3_ray_change_alpha(-90.0);
  glib3_ray_change_beta(90.0);
  glutReshapeFunc(reshape);
  glutDisplayFunc(glib3_ray);
  // glutIdleFunc(glib3_ray);
  glib3_ray();
  glutMainLoop();
}
#endif

void glib3_ray_change_alpha(float e) { alpha = e; }
void glib3_ray_change_beta(float e) { beta = e; }


void glib3_ray_reshape(float w,float h) {
  reshape(w,h);
}
void glib3_ray() {
  // glib3_ray_change_alpha(-90.0+xnear*5);
  // glib3_ray_change_beta(90.0+ynear*3);
  draw();
}

void glib3_ray_init() {


   reshape(WIDTH, HEIGHT);

   glShadeModel(GL_FLAT);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glEnable(GL_CULL_FACE);
   glEnable(GL_TEXTURE_2D);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glEnable(GL_FOG);
   glFogi(GL_FOG_MODE, GL_EXP2);
   glFogfv(GL_FOG_COLOR, fogcolor);

   glFogf(GL_FOG_DENSITY, 0.01);

   calcposobs();

   initspherepos();

   inittextures();
   initdlists();

   glClearColor(fogcolor[0], fogcolor[1], fogcolor[2], fogcolor[3]);
}


