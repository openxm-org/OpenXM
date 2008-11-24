//
//  MyOpenGLView.m
//  cfep
//
//  Created by Nobuki Takayama on 06/02/18.
//  Copyright 2006 OpenXM.org. All rights reserved.
//

#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <GLUT/glut.h>

#import "MyOpenGLView.h"
#import "MyOpenGLController.h"
#include "mygl.h"



@implementation MyOpenGLView
-(id) initWithFrame: (NSRect) frame {
  NSLog(@"initWithFrame\n");
  oglComm = [NSMutableArray arrayWithCapacity: 100];
  [oglComm retain];
  oglInitComm = [NSMutableArray arrayWithCapacity: 1];
  [oglInitComm retain];
  xeye = 0.0;
  yeye = 0.0;
  zeye = 2.0;
  initGl = 1;
  myfont = [ NSFont systemFontOfSize: 10];
  [super initWithFrame: frame];
  if (self) {
    NSOpenGLPixelFormatAttribute attributes[]={
	  NSOpenGLPFAAccelerated,
	  NSOpenGLPFADepthSize,16,    // We need this, otherwise depth buffer will no be enabled.
	  NSOpenGLPFAMinimumPolicy,
	  NSOpenGLPFAClosestPolicy,
	  0};
	NSOpenGLPixelFormat *format;
	format = [[[NSOpenGLPixelFormat alloc] initWithAttributes: attributes] autorelease];
	[self initWithFrame: frame pixelFormat: format];
  }
  return self;
}

-(void) showEyePos {[[MyOpenGLController getOglWindow: gid] showEyeX: xeye Y: yeye Z: zeye];}
-(IBAction) setXeye: (id) sender {
   xeye=([sender floatValue]-50)*0.1; initGl = 1;
   NSLog(@"xeye=%f\n",xeye);
   [self showEyePos];
   [self setNeedsDisplay: YES];
}
-(IBAction) setYeye: (id) sender {
   float y;
   y=([sender floatValue]-50)*0.1; initGl = 1;
   yeye=y; 
   [self showEyePos];
   //NSLog(@"yeye=%f\n",yeye);
   [self setNeedsDisplay: YES];
}
-(IBAction) setZeye: (id) sender {
   float z;
   z=([sender floatValue]-50)*0.1+2.0; initGl = 1;
   zeye=z; 
   [self showEyePos];
   //NSLog(@"zeye=%f\n",zeye);
   [self setNeedsDisplay: YES];
}
-(void) drawRect: (NSRect) rect {
  if (initGl) { [self initGL]; initGl = 0;}
  [self drawOglComm];
 //For testing. [self drawRectSimple: rect withColor: 0.0];
}
-(void) setFrameSize: (NSSize) newSize {
  [super setFrameSize: newSize];
  //[[self openGLContext] makeCurrentContext];
  glViewport(0,0,(GLsizei)newSize.width,(GLsizei) newSize.height);
  initGl=1;
  [self setNeedsDisplay: YES];
}
-(void) setGid: (int) p{
  gid = p;
} 
-(void) initGL {
  // Initialization codes are here.
  glClearColor(1.0,1.0,1.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  [self drawOglInitComm];
}
-(void) setInitGL: (id) sender { initGl = 1; }
// It was for a test.
-(void) drawRectSimple: (NSRect) rect withColor: (double) c {
  glClearColor(1.0,1.0,1.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor4f(1.0,1.0,c,1.0);
  glRectf(-0.6,-0.6,0.6,0.6);
  
  glFlush();
  [[self openGLContext] flushBuffer];
}

-(void) drawOglComm {
  int i,n;
  MyOpenGLCommand *gc;
  // Execute oglComm.
  n = [oglComm count];
  NSLog(@"drawRect is called with n=%d, oglComSize=%d\n",n,oglCommSize);
  for (i=0; i<oglCommSize; i++) {
    gc = [oglComm objectAtIndex: i];
	[self execute: gc];
  }
  glFlush();
  [[self openGLContext] flushBuffer];
}
-(void) addOglComm: (NSString *) comm by: (id) sender {
   MyOpenGLCommand *oc;
   oc = [MyOpenGLCommand allocAndCompile: comm by: sender];
   if (oc != nil) {
    [oglComm addObject: oc];  // retain is automatically done.  // dealloc should be implemented.
	@synchronized(self) {
		if ([oc isEndGroup] == YES) oglCommSize=[oglComm count];
	}
	if ([oc getOpCode] == CFEPglib_flush) [self setNeedsDisplay: YES]; 
	else if ([oc getOpCode] == CFEPglFlush) [self setNeedsDisplay: YES];
	// If oc is glib_flush, then call drawRect. (Generate an event.)
	// Calling [self drawOglComm] directly is not safe, because the window might not be ready.
   }
}

-(NSMutableArray *)getListOfOglComm { return oglComm; }
-(NSMutableArray *)getListOfOglInitComm { return oglInitComm; }
-(int) getOglCommSize { return oglCommSize; }
-(int) getOglInitCommSize { return oglInitCommSize; }
-(int) countOfOglComm { return [oglComm count]; }
-(int) countOfOglInitComm { return [oglInitComm count];}
-(int) removeLastOfOglComm { if ([oglComm count]>0) [oglComm removeLastObject]; [self updateOglCommSize]; return [self countOfOglComm];}
-(int) removeLastOfOglInitComm {if ([oglInitComm count]>0) [oglInitComm removeLastObject]; [self updateOglInitCommSize]; return [self countOfOglInitComm];}
-(int) removeAllOfOglComm { if ([oglComm count]>0) [oglComm removeAllObjects]; oglCommSize = 0; return 0; }
-(int) removeAllOfOglInitComm {if ([oglInitComm count]>0) [oglInitComm removeAllObjects]; oglInitCommSize = 0; return 0; }
-(void) updateOglCommSize {
  int n,i;
  MyOpenGLCommand *cc;
  n = [oglComm count];
  for (i=n-1; i>=0; i--)  { 
    cc = [oglComm objectAtIndex: i]; 
	if ([cc isEndGroup]) { oglCommSize = i+1; return ; }
  }
  oglCommSize = 0;
}
-(void) updateOglInitCommSize {
  int n,i;
  MyOpenGLCommand *cc;
  n = [oglInitComm count];
  for (i=n-1; i>=0; i--)  { 
    cc = [oglInitComm objectAtIndex: i]; 
	if ([cc isEndGroup]) { oglInitCommSize = i+1; return ; }
  }
  oglInitCommSize = 0;
}

-(void) drawOglInitComm {
  int i,n;
  MyOpenGLCommand *gc;
  // Execute oglComm.
  n = [oglInitComm count];
  NSLog(@"drawOglInitComm is called with n=%d, oglInitComSize=%d\n",n,oglInitCommSize);
  for (i=0; i<oglInitCommSize; i++) {
    gc = [oglInitComm objectAtIndex: i];
	[self execute: gc];
  }
}
-(void) addOglInitComm: (NSString *) comm by: (id) sender {
   MyOpenGLCommand *oc;
   oc = [MyOpenGLCommand allocAndCompile: comm by: sender];
   if (oc != nil) {
    [oglInitComm addObject: oc];  // retain is automatically done.  // dealloc should be implemented.
	@synchronized(self) {
		if ([oc isEndGroup] == YES) oglInitCommSize=[oglInitComm count];
	}
	initGl = 1;
	if ([oc getOpCode] == CFEPglib_flush) { [self setNeedsDisplay: YES]; }
	else if ([oc getOpCode] == CFEPglFlush) [self setNeedsDisplay: YES];
   }
}

// Byte compile is done by comple: in MyOpenGLCommand.m
-(void) execute: (MyOpenGLCommand *)gc {
  int op;
  float x,y,z,c;
  int    p,q,r,s;
  double *v;
  int *ii;
  op = [gc getOpCode]; 
  v = [gc getF4];
  ii = [gc getI4];
  x = v[0]; y = v[1]; z = v[2]; c = v[3];
  p = ii[0]; q=ii[1]; r=ii[2];  s=ii[3];
  // NSLog(@"opCode=%d, (x,y,z,c)=(%f,%f,%f,%f), (p,q,r,s)=(%d,%d,%d,%d)\n",op,x,y,z,c,p,q,r,s);
  switch(op) {
  case CFEPglBegin:
    glBegin(p);
	break;
  case CFEPglClear:
    glClear(p); break;
  case CFEPglClearColor:
    glClearColor(x,y,z,c); break;
  case CFEPglClearDepth:
    glClearDepth(x); break;
  case CFEPglColor4f:
    glColor4f(x,y,z,c); break;
  case CFEPglDisable:
    glDisable(p); break;
  case CFEPglEnable:
    glEnable(p); break;
  case CFEPglEnd:
    glEnd(); break;	
  case CFEPglFlush:
    glFlush(); [self setInitGL: nil]; [self setNeedsDisplay: YES]; break;
  case CFEPglLineStipple:
    glLineStipple(p,q); break;
  case CFEPglLineWidth:
    glLineWidth(x); break;
  case CFEPglNormal3f:
    glNormal3f(x,y,z); break;
  case CFEPglPointSize:
    glPointSize(x); break;
  case CFEPglRectf:
	glRectf(x,y,z,c); break;
  case CFEPglVertex3f:
    glVertex3f(x,y,z); break;	
  case CFEPglVertex2f:
    glVertex2f(x,y); break;	

  case CFEPglib_line:
    glib_line(x,y,z,c,p); break;
  case CFEPglib_putpixel:
    glib_putpixel(x,y,p); break;	
  case CFEPglib_flush:
	[self setInitGL: nil]; [self setNeedsDisplay: YES]; 
	// [[MyOpenGLController getOglWindow: gid] showCount];
	break;

  case CFEPglib3_bounding_box:
	glib3_bounding_box(x); break;
  case CFEPglib3_icosahedron:
    glib3_icosahedron(0.0f, 0.0f, -1.0f, 1.0f); break;
  case CFEPglib3_ray:
    [self glib3_ray]; break;
  case CFEPglib3_ray_init:
    glib3_ray_init(); break;
  case CFEPglib3_ray_reshape:
    glib3_ray_reshape(x,y); break;	
  case CFEPglib3_std_scene0:
    [self glib3_std_scene0]; break;
  default:
    NSLog(@"Unknown opCode %d\n",op);
	break;
  }
}

-(void) glib3_ray {
  glib3_ray_change_alpha(-90.0+xeye*5);
  glib3_ray_change_beta(90.0+yeye*3);
  glib3_ray();
}
-(void) glib3_std_scene0 {
  glib3_std_scene0(xeye,yeye,zeye);
}
-(void) mouseDown: (NSEvent *) theEvent {
  NSLog(@"Mouse is down\n");
}
@end

// Original glib functions
void glib_line(float x,float y, float x2,float y2, int color) {
  glColor4f((color/0x10000)/256.0,((color/0x100)& 0xff)/256.0,(color % 0x100)/256.0,0.0);
  // NSLog(@"color=%f,%f,%f\n",(color/0x10000)/256.0,((color/0x100)& 0xff)/256.0,(color % 0x100)/256.0);
  glBegin(GL_LINES);
    glVertex2f(x,y);
	glVertex2f(x2,y2);
  glEnd();
}
void glib_putpixel(float x,float y,int color) {
  // NSLog(@"color=%f,%f,%f\n",(color/0x10000)/256.0,((color/0x100)& 0xff)/256.0,(color % 0x100)/256.0);
  glBegin(GL_POINTS);
    glColor4f((color/0x10000)/256.0,((color/0x100)& 0xff)/256.0,(color % 0x100)/256.0,0.0);
    glVertex2f(x,y);
  glEnd();
}
