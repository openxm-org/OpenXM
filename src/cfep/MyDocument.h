//
//  MyDocument.h
//  cfep
//
//  Created by Nobuki Takayama
//  Copyright (c) 2006 openxm.org
//

#import <Cocoa/Cocoa.h>
#import "MyEnvironment.h"
#import "MyDecoder.h"

#define START_OF_RESULT_OF_OX_TEXMACS   0x2
#define END_OF_EVALUATION_OF_OX_TEXMACS 0x5
#define UTF8_2  0x6    // 110. header of utf8 2 byte data 
#define UTF8_3  0xe    // 1110. header of utf8 3 byte data
#define MYDECODER 0x3e // 1111 10. header of MyDecoder 3 byte data
//     u0           u1           u2
//     110* ****/1  10** ****                UTF8_2
//     1110 ****/2  10** ****/3  10** ****   UTF8_3
//     1111 10**/4  1111 0***/5  1111 0***   MYDECODER
//     /state

// unicode of @"【"
#define BEGIN_INPUT_CELL  0x3010  
// @"】"
#define END_INPUT_CELL    0x3011
// @"《"
#define BEGIN_OUTPUT_CELL 0x300a
// @"》"
#define END_OUTPUT_CELL   0x300b


@interface MyDocument : NSDocument
{
	

  IBOutlet NSTextView *textViewIn;
  IBOutlet NSTextView *textViewOut;
  IBOutlet NSTextField *inputCounterField;
  IBOutlet NSTextField *outputCounterField;
  IBOutlet NSProgressIndicator *myIndicator;


  NSData *dataFromFile;
  NSData *textDataFromFile;
  NSData *rtfDataFromFile;

  NSString *OpenXM_HOME ;
  int onlySelectedArea;
  int inputCounter;
  int outputCounter;
  int redirectPrint;

  // For notebook style
  int notebookMode;
  int doInsertNewInputCell;
  int ox_texmacs_level;
  // For select engine menu.
  int oxEngine;  // cf. peer_type. temporal.
   // For error handling.
  NSMutableArray *errorLines;
  // For debug the tunnel
  int debugMyTunnel;
  // asir-contrib is loaded or not.
  int asir_contrib;
  // pretty printing
  int prettyPrint;
  //  cf. NoEngine
  int restartMode;
  //////////
  NSTask *task;
  NSPipe *inboundPipe;
  NSPipe *outboundPipe;
  NSPipe *errPipe;
  NSFileHandle *inboundFileHandle;
  NSFileHandle *outboundFileHandle;
  NSFileHandle *errFileHandle;
  
  int inEvaluation;
  NSString *myDocumentKey;
  MyDecoder *myDecoder;
  /////////
  // Parameters below should be synchronized with values in myEnvironemnt.
  enum peer_type peer;
  NSData *peerEndEvaluteMark;
  NSString *peerStartupString;
  NSDictionary *myDefaultTypingAttributes;
  
}

-(IBAction) oxEvaluate:(id) sender;
-(IBAction) oxInterrupt:(id) sender;
-(IBAction) oxRenderOutput: (id) sender;
-(IBAction) oxEvaluateSelected: (id) sender;
-(IBAction) oxEvaluateRegisteredString: (id) sender;
-(IBAction) checkSelectedArea: (id) sender;
-(IBAction) clearTextViewOut: (id) sender ; 

// loading
-(void)loadtextViewWithData:(NSData *)data;
-(void)loadtextViewWithRTFData:(NSData *)data;
-(void) loadtextViewWithTextData:(NSData *)data ;

// initilizers
-(void) initAux ;
-(id) openMyModel ;
-(void) closeMyModel;
-(id) restartMyModel ;

-(NSString *) getMyDocumentKey;

// messenges
-(void) sayTheMessageAboutX ;

-(int) oxEvaluateString: (NSString *)msg withMark: (BOOL) yes;
- (void)outputString:(NSString *)amt;
- (void)outputErrorString:(NSString *)amt;
-(void) printErrorMessage: (NSString *)msg;
-(void)outputStringInNotebookMode: (NSString *)msg;
-(void)outputErrorStringInNotebookMode: (NSString *)msg;
-(void)outputStringInNotebookMode: (NSString *)msg withColor: (NSColor *)color;
-(void)outputBorderLine: (NSColor *)color;

- (void)messageDialog:(NSString *)msg with:(int) no;
-(void) printDocument: (id) sender;
-(void) showHelp: (id) sender;
-(void) changeInputCounterField: (int) inc;
-(void) changeOutputCounterField: (int) inc;
-(void) changeOutputCounterFieldWithString: (NSString *) msg;
-(void) startIndicator;
-(void) stopIndicator;

-(void) readInboundData: (NSNotification *) sender;
-(void) readErrData: (NSNotification *) sender;

-(NSData *) getPeerEndEvaluateMark;

-(int) selectIllegalCharacter ;
-(NSRange) selectBlockForwardFrom: (int) offset with: (int) first and: (int) last ;
-(NSRange) selectBlockForward ;

// managing errors. (channel 0)
-(void) errorActionFor: (NSString *)cmd;
-(void) clearLines;
-(void) addLine: (int) n;
-(int) nextLine: (int) flag ;
-(int) gotoLine: (int) n;
-(int) gotoNextError: (id) sender;  // Can be an action.
// managing OpenGL messages. (channel 1 and 2)
-(int) openGLActionFor: (NSString *)cmd;
-(int) openGLInitActionFor: (NSString *)cmd;
-(void) showListOfOglComm: (int) gid;
-(void) openGLMeta: (NSString *) cmd to: (int) gid;
// managing png messages. (channel 10)
-(void) pngActionFor: (NSString *)cmd;
// notebookMode
-(void) insertInputCell;
-(NSString *)getContentsOfInputCell;
-(void)prepareOutputCell;
-(int)getOutOfOutputCell;
-(int)specialCharacterInNotebookMode: (int) c;
// Add menus
-(void) addMenuExec;
-(void) setBasicStyle: (id) sender;
-(void) setNotebookStyle: (id) sender;
-(void) updateInterfaceStyleMenu; 
-(void) setEngineRisaAsir: (id) sender;
-(void) setEngineKanSm1: (id) sender;
-(void) updateSelectEngineMenu; 
-(void) setEngineRisaAsir: (id) sender;
-(void) setEngineKanSm1: (id) sender;
-(void) setDebugMyTunnel: (id) sender;
-(void) loadAsirContrib: (id) sender;
-(NSMenuItem *) menuItemLoadLibrary: (int) oxengine;
-(void) setPrettyPrint: (id) sender;
-(void) mytest: (id) sender;

@end
