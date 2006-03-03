//
//  MyOpenGLCommand.h
//  cfep
//
//  Created by Nobuki Takayama on 06/02/18.
//  Copyright 2006 OpenXM.org. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

//  // -- has not been implemented.
// glib original functions.  xxxx
// a   b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  
// 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26  glib original
#define CFEPglib_end_group   0501
#define CFEPglib_flush       0601
#define CFEPglib_line        1201
#define CFEPglib_putpixel    1601
// Demos
// a   b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  
// 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56  demo
#define CFEPglib3_bounding_box 3201
#define CFEPglib3_icosahedron 3901
#define CFEPglib3_std_scene0_ortho 4901  //
#define CFEPglib3_ray         4801
#define CFEPglib3_ray_init    4802
#define CFEPglib3_ray_reshape 4803
#define CFEPglib3_std_scene0  4901
// gl primitives.
// a   b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  
// 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86  gl primitives
#define CFEPglBegin         6201       
#define CFEPglColor4f       6301
#define CFEPglEnd           6501       
#define CFEPglLineStipple   7201       //
#define CFEPglLineWidth     7202       //
#define CFEPglRectf         7801
#define CFEPglVertex3f      8201       

@interface MyFloat: NSObject {
  float fValue;
}
+(MyFloat *) allocWith: (float) f;
-(void) setFValue: (float) f;
-(float) getFValue;
-(NSString *)description;
@end

@interface MyInt: NSObject {
  int iValue;
}
+(MyInt *) allocWith: (int) i;
-(void) setIValue: (int) i;
-(int) getIValue;
-(NSString *)description;
@end

@interface MyUtil: NSObject {
}
+(void) setDebugMyUtil;
+(NSMutableArray *) arrayOfStringFrom: (NSString *)args;
@end

@interface MyOpenGLCommand : NSObject {
  NSString *command;
  BOOL endGroup;
  int opCode;
  NSMutableArray *argv;
  // f4 and i4 will become obsolete.
  double f4[4];
  int    i4[4];
}

+(MyOpenGLCommand *) allocAndCompile: (NSString *)cmd by: (id) sender;
-(id)init;
-(void) dealloc;
-(void)print;
-(MyOpenGLCommand *) compile: (NSString *)cmd by: (id) sender;
-(int) getOpCode;
-(double *)getF4;
-(int *)getI4;
-(NSArray *) getArgv;
-(BOOL) isEndGroup;
@end
