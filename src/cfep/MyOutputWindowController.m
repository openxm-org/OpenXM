//
//  MyOutputWindowController.m
//  cfep
//
//  Created by nobuki on 06/01/25.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import "MyOutputWindowController.h"
#import "MyDocument.h"

@implementation MyOutputWindowController

+(MyOutputWindowController *) sharedMyOutputWindowController: (MyDocument *) sender action: (enum actionType) act {
  static NSMutableDictionary * sharedMOWC;  // class variable.
  MyOutputWindowController *owin;
  if (sharedMOWC == nil) {
    sharedMOWC = [NSMutableDictionary dictionaryWithCapacity: 1024]; //BUG. finite number.
	[sharedMOWC retain];
  }
  if (act == FIND) {
	owin = [sharedMOWC objectForKey: [sender getMyDocumentKey]];
	if (owin != nil) return owin;
    owin = [[MyOutputWindowController allocWithZone:[MyOutputWindowController zone]] init];
	[owin retain];
	NSLog(@"MyOutputWindowController. new owc is created.\n");
	[sharedMOWC setObject: owin forKey: [sender getMyDocumentKey]];
	return owin;
  }else{ // REMOVE
	owin = [sharedMOWC objectForKey: [sender getMyDocumentKey]];
    if (!owin) [sharedMOWC removeObjectForKey: [sender getMyDocumentKey]];
	// [owin autorelease];
	return owin;
  }
}

+(MyOutputWindowController *) sharedMyOutputWindowController: (MyDocument *)sender {
   return [self sharedMyOutputWindowController: sender action: FIND];
}


-(id) init {
  static int serial;
  self = [self initWithWindowNibName: @"MyOutputWindow"];
  if (self) [self setWindowFrameAutosaveName: @"cfep Output View"];
  winNo = serial++;
  return self;
}

-(void) windowDidLoad {
  [super windowDidLoad];
  // [textViewOut retain]; ?
}

-(void) dealloc {
  NSLog(@"dealloc of MyOutputWindowController instance.\n");
  [[NSNotificationCenter defaultCenter] removeObserver: self];
  [super dealloc];
}

-(void) closeMyOutputWindow: (MyDocument *) md {
  NSLog(@"closeMyOutputWindow. \n");
  [MyOutputWindowController sharedMyOutputWindowController: md action: REMOVE];
  removable = 1;
  [self close];
}

-(BOOL) windowShouldClose: (NSWindow *)sender {  // book. p.149   Delegate outlet of Window should me File's owner.
   if (removable) return [super windowShouldClose: sender];
   return NO;
}

-(void)outputStringToOutputWindow:(NSString *)msg {
  NSRange myRange = NSMakeRange([[textViewOut textStorage] length],0);
  // NSLog(@"<MyOutputWindowController> outputStringToOutputWindow\n");  
  [textViewOut replaceCharactersInRange: myRange withString: msg];
  [textViewOut scrollRangeToVisible: NSMakeRange([[textViewOut textStorage] length],0)];
}

-(void)printErrorMessageToOutputWindow:(NSString *)msg {
  int oldEnd;
  int newEnd;
  oldEnd = [[textViewOut textStorage] length];
  NSRange myRange = NSMakeRange(oldEnd,0);
  [textViewOut replaceCharactersInRange: myRange withString: msg];
  newEnd = [[textViewOut textStorage] length];
  [textViewOut setTextColor: [NSColor redColor] range: NSMakeRange(oldEnd,newEnd-oldEnd)];
  [textViewOut scrollRangeToVisible: NSMakeRange(newEnd,0)];
}

-(void)clearOutputWindow {
  NSRange myRange = NSMakeRange(0,[[textViewOut textStorage] length]);
  [textViewOut replaceCharactersInRange: myRange withString: @""];
} 
-(void)changeWindowTitle: (NSString *)msg {
  NSLog(@"title=%@\n",[window title]);
  // [window setTitle: msg];  it does not work. Why?
}
-(void)showUp {
  if (displayed == 0) { 
    [self showWindow: nil]; 
	displayed = 1;
	[self changeWindowTitle: [[NSString stringWithFormat: @"OutputView %d",winNo] retain]];
  }
}

-(void) printDocument: (id) sender {
   [textViewOut print: sender];
}


@end
