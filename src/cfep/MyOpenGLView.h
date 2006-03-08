//
//  MyOpenGLView.h
//  cfep
//
//  Created by Nobuki Takayama on 06/02/18.
//  Copyright 2006 OpenXM.org. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MyOpenGLCommand.h"

@interface MyOpenGLView : NSOpenGLView {
  int initGl;
  double xeye,yeye,zeye;  // eye position
  NSMutableArray *oglComm;
  int oglCommSize;
  NSMutableArray *oglInitComm;
  int oglInitCommSize;
  int gid;
}
-(id) initWithFrame: (NSRect) frameRect;
-(void) drawRectSimple: (NSRect) rect withColor: (double)c;
-(void) drawRect: (NSRect) rect ;
-(void) showEyePos ;
-(IBAction) setXeye: (id) sender;
-(IBAction) setYeye: (id) sender;
-(IBAction) setZeye: (id) sender;
-(void) setGid: (int) gid;
-(void) setInitGL: (id) sender;
-(int) getOglCommSize;
-(int) getOglInitCommSize;
-(void) addOglComm: (NSString *) comm by: (id) sender;
-(void) drawOglComm ;
-(void) addOglInitComm: (NSString *) comm by: (id) sender;

-(NSMutableArray *)getListOfOglComm;
-(NSMutableArray *)getListOfOglInitComm;
-(int) countOfOglComm ;
-(int) countOfOglInitComm;
-(int) removeLastOfOglComm;
-(int) removeLastOfOglInitComm;
-(int) removeAllOfOglComm;
-(int) removeAllOfOglInitComm;

-(void) drawOglInitComm ;
-(void) execute: (MyOpenGLCommand *)gc;
-(void) initGL ;

-(void) glib3_ray;

@end

// original glib functions
void glib_line(float x,float y, float x2,float y2, int color);
void glib_putpixel(float x,float y, int color);

