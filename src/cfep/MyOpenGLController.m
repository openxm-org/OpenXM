//
//  MyOpenGLController.m
//  cfep
//
//  Created by Nobuki Takayama on 06/02/18.
//  Copyright 2006 OpenXM.org. All rights reserved.
//

#import "MyOpenGLController.h"

// MyOpenGL window id (gid) is an indeger.  
static MyOpenGLController *oglWindow[GID_MAX];  // gid --> MyOpenGLController instance.
static MyDocument *oglParent[GID_MAX];           // gid --> parent MyDocument.
static int MyOpenGLControllerInitialized = 0;
static int Gid=0;


@implementation MyOpenGLController
+(void) initMyOpenGLController {
  int i;
  if (MyOpenGLControllerInitialized) return;
  for (i=0; i<GID_MAX; i++) { oglWindow[i] = nil; oglParent[i] = nil; }
  MyOpenGLControllerInitialized = 1;
}
+(MyOpenGLController *)getOglWindow: (int) tid {
  return oglWindow[tid];
}
+(MyDocument *)getOglParent: (int) tid {
  return oglParent[tid];
}

+(int) myOpenGLControllerOwnedBy: (MyDocument *) owner with: (int) tid {
   MyOpenGLController *ogl;
   if ((tid >= 0) && (tid <GID_MAX)) {
     if (oglWindow[tid]) return tid; 
	 else {
		ogl = [[MyOpenGLController allocWithZone:[MyOpenGLController zone]] init];
		[ogl setGid: tid];
		[ogl retain];
		[ogl showWindow: nil];
		oglWindow[tid] = ogl;
		oglParent[tid] = owner;
		return tid;
	}	
   }
   return -1;
}
+(int) myOpenGLControllerOwnedBy: (MyDocument *) owner {
   int tid;

   [MyOpenGLController initMyOpenGLController];
   tid = Gid; 
   if (tid < GID_MAX) {
    Gid++;
	return [MyOpenGLController myOpenGLControllerOwnedBy: owner with: tid];
   }else{
    return -1;
   }	
}
+(void) removeMyOpenGLControllerOwnedBy: (MyDocument *) owner {
  int i;
  [MyOpenGLController initMyOpenGLController];
  for (i=0; i<GID_MAX; i++) {
    if (oglParent[i] == owner) {
	   [MyOpenGLController removeMyOpenGLControllerWithGid: i];
	}
  }
}
+(MyOpenGLController *) removeMyOpenGLControllerWithGid: (int) tid {
  MyOpenGLController *oglC;
  [MyOpenGLController initMyOpenGLController];
  oglC = oglWindow[tid];
  if (oglWindow[tid]) [oglWindow[tid] closeMyOpenGL: oglParent[tid]];
  oglWindow[tid] = nil;
  oglParent[tid] = nil;
  [oglC autorelease]; // it is OK?
  return oglC;
}

/*  They are put at oxEvaluateSelected in MyDocument.m to test the system. --> gtest.rr
   [MyOpenGLController addOglComm: @"[glColor4f,1.0,0.0,0.0,1.0]" to: 0 from: self];
   [MyOpenGLController addOglComm: @"[glRectf,-0.5,-0.5,0.5,0.5]" to: 0 from: self];
   [MyOpenGLController addOglComm: @"[glib_flush0]" to: 0 from: self];
*/

+(void) addOglComm:     (NSString *)comm to: (int) tid from: (MyDocument *) owner {
  if ((0<=tid) && (tid<GID_MAX)) {
	if (oglWindow[tid] == nil) {
	  tid = [MyOpenGLController myOpenGLControllerOwnedBy: owner with: tid];
	}
    if (tid >= 0) [oglWindow[tid] addOglComm: comm];
  }else ;
}
+(void) addOglInitComm: (NSString *)comm to: (int) tid from: (MyDocument *) owner {
  if ((0<=tid) && (tid<GID_MAX)) {
	if (oglWindow[tid] == nil) {
	  tid = [MyOpenGLController myOpenGLControllerOwnedBy: owner with: tid];
	}
    if (tid >= 0) [oglWindow[tid] addOglInitComm: comm];
  }else ;
}


-(id) init {
  self = [self initWithWindowNibName: @"MyOpenGL"];
  if (self) [self setWindowFrameAutosaveName: @"cfep OpenGLView"];
  return self;
}
-(void) setGid: (int) mygid {
  gid = mygid;
}
-(int)getGid {
  return gid;
}

-(void) windowDidLoad {
  [super windowDidLoad];
  [myogl setGid: gid]; 
  // [self showCount];
}

-(void) dealloc {
  NSLog(@"dealloc of MyOpenGL instance.\n");
  // [[NSNotificationCenter defaultCenter] removeObserver: self];
  [super dealloc];
}

-(void) closeMyOpenGL: (MyDocument *) md {
  NSLog(@"closeMyOpenGL. \n");
  [self close];
}

-(BOOL) windowShouldClose: (NSWindow *)sender {
   // p.299 of O'reilly book.  Connect "Window --> file's owner" to connect the delegate outlet of the window
   //                          to MyOpenGLController.
   NSLog(@"x is clicked.\n");
   [MyOpenGLController removeMyOpenGLControllerWithGid: [self getGid]];
   return YES;
}
-(void) addOglComm: (NSString *)comm {
  [myogl addOglComm: comm by: self];
}
-(void) addOglInitComm: (NSString *)comm {
  [myogl addOglInitComm: comm by: self];
}

-(int) countOfOglComm {return [myogl countOfOglComm];}
-(int) countOfOglInitComm {return [myogl countOfOglInitComm];}
-(int) removeLastOfOglComm {return [myogl removeLastOfOglComm];}
-(int) removeLastOfOglInitComm {return [myogl removeLastOfOglInitComm];}
-(int) removeAllOfOglComm { return [myogl removeAllOfOglComm];}
-(int) removeAllOfOglInitComm {return [myogl removeAllOfOglInitComm];}
-(NSMutableArray *) getListOfOglComm { return [myogl getListOfOglComm];}
-(NSMutableArray *) getListOfOglInitComm { return [myogl getListOfOglInitComm]; } 
-(void) showEyeX: (float) x Y: (float) y Z: (float) z {
  [self clearOutput];
  [self output: [NSString stringWithFormat: @"gid=%d, n=%d, position of your eye:(%1.2f,%1.2f,%1.2f)",gid,[self countOfOglComm],x,y,z]];
}
-(void) showCount {
  [self clearOutput];
  [self output: [NSString stringWithFormat: @"gid=%d, number of OpenGL commands=%d, init=%d",
                                               gid, [self countOfOglComm], [self countOfOglInitComm]]];
}
-(void) clearOutput { [mymessage setBackgroundColor: [NSColor whiteColor]]; } //does it work? 
-(void) output: (NSString *)msg {
  [mymessage setStringValue: msg];
}

@end
