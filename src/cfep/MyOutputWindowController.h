//
//  MyOutputWindowController.h
//  cfep
//
//  Created by nobuki on 06/01/25.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class MyDocument ;
enum actionType { FIND, REMOVE} ;

@interface MyOutputWindowController : NSWindowController {
  IBOutlet NSWindow *window;
  IBOutlet NSTextView *textViewOut;
  IBOutlet NSTextField *inputCounterField;
  
  int removable;
  int displayed;
  int winNo;
}

+(MyOutputWindowController *) sharedMyOutputWindowController: (MyDocument *) sender action: (enum actionType) a;
+(MyOutputWindowController *) sharedMyOutputWindowController: (MyDocument *)sender ;
-(id) init;
-(void) closeMyOutputWindow: (MyDocument *) md;
-(void) windowDidLoad;
-(void) dealloc;
-(BOOL) windowShouldClose: (NSWindow *)sender ;

-(void)changeWindowTitle: (NSString *)msg; 
-(void)outputStringToOutputWindow:(NSString *)msg;  
-(void)insertText: (id) text;
-(void)clearOutputWindow;
-(void)printErrorMessageToOutputWindow:(NSString *)msg;  

-(void) printDocument: (id) sender; 




@end
