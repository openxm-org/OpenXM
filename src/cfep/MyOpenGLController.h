//
//  MyOpenGLController.h
//  cfep
//
//  Created by Nobuki Takayama on 06/02/18.
//  Copyright 2006 OpenXM.org. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MyOpenGLCommand.h"
#import "MyOpenGLView.h"
@class MyDocument;
#define MY_GID_MAX 1024

@interface MyOpenGLController : NSWindowController {
  IBOutlet NSWindow *window;
  IBOutlet MyOpenGLView *myogl;
  IBOutlet NSTextField *mymessage;
// MyOpenGL window id (gid) is an indeger.  
  int gid;
}

+(MyOpenGLController *)getOglWindow: (int) tid ;
+(MyDocument *)getOglParent: (int) tid;
+(void)initMyOpenGLController ;
+(int) myOpenGLControllerOwnedBy: (MyDocument *) owner with: (int) tid;
+(int) myOpenGLControllerOwnedBy: (MyDocument *) owner;
+(void) removeMyOpenGLControllerOwnedBy: (MyDocument *) owner;
+(MyOpenGLController *) removeMyOpenGLControllerWithGid: (int) gid;
+(void) addOglComm:     (NSString *)comm to: (int) tid from: (MyDocument *) owner;
+(void) addOglInitComm: (NSString *)comm to: (int) tid from: (MyDocument *) owner;
-(id) init ;
-(void) setGid: (int) mygid;
-(int) getGid ;
-(void) windowDidLoad ;
-(void) dealloc ;
-(void) closeMyOpenGL: (MyDocument *) md ;
-(BOOL) windowShouldClose: (NSWindow *)sender ;
-(void) addOglComm: (NSString *)comm ;   // Adding a OpenGL command.
-(void) addOglInitComm: (NSString *)comm ;   // Adding a OpenGL initialization command.
-(int) countOfOglComm ;
-(int) countOfOglInitComm;
-(int) removeLastOfOglComm;
-(int) removeLastOfOglInitComm;
-(int) removeAllOfOglComm;
-(int) removeAllOfOglInitComm;
-(NSMutableArray *) getListOfOglComm;
-(NSMutableArray *) getListOfOglInitComm; 
-(void) showCount;
-(void) showEyeX: (float) x Y: (float) y Z: (float) z ;
-(void) clearOutput;
-(void) output: (NSString *)msg;
@end
