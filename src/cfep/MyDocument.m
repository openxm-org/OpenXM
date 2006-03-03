//
//  MyDocument.m
//  cfep
//
//  Created by Nobuki Takayama
//  Copyright (c) 2006 OpenXM.org
//

// [self setDefaultPeer BASH] or [self setDefaultPeer OX_TEXMACS] 

#include <signal.h>
#import "MyDocument.h"
#import "MyOutputWindowController.h"
#import "MyOpenGLController.h"

int myDebug=0;
static int myDocumentSaidTheMessageAboutX = 0;


@implementation MyDocument

- (void)dealloc {
    MyOutputWindowController *mowc;
    if (myDebug) NSLog(@"dealloc of MyDocument.\n");
	if (myDebug) NSLog(@"retainCount of task=%d\n",[task retainCount]); // why is it 2?
	[self closeMyModel];
	// [task dealloc];  //BUG. if we do this, error happens.
	mowc = [MyOutputWindowController sharedMyOutputWindowController: self];
	if (mowc != nil) [mowc closeMyOutputWindow: self];
	if (errorLines) { [errorLines autorelease]; errorLines = nil; }
	
	[myDecoder dealloc];
	
	[dataFromFile release];
	[rtfDataFromFile release];
	[textDataFromFile release];
	if (myDebug) NSLog(@"Done: dealloc of MyDocument.\n");
    [super dealloc];
}

// The name of the nib file to load for this document.
- (NSString *)windowNibName
{
    return @"MyDocument";
}

// Loads up the data when the nib file is loaded.
- (void)windowControllerDidLoadNib:(NSWindowController *)aController {
    [super windowControllerDidLoadNib:aController];

    if (dataFromFile) {
        [self loadtextViewWithData:dataFromFile];
        [dataFromFile release];
        dataFromFile = nil;
    } else if (rtfDataFromFile) {
	    [self loadtextViewWithRTFData: rtfDataFromFile];
		[rtfDataFromFile release];
		rtfDataFromFile = nil;
	} else if (textDataFromFile) {
	    [self loadtextViewWithTextData: textDataFromFile];
		[textDataFromFile release];
		textDataFromFile = nil;
	}
    [self addMenuExec]; // adding the execution menu. 
    [textViewIn setAllowsUndo:YES];
	[self sayTheMessageAboutX];
	[self initAux]; 
}

// Load the view with data given an NSData.
- (void)loadtextViewWithData:(NSData *)data {
    [textViewIn replaceCharactersInRange:NSMakeRange(0, [[textViewIn string] length]) withRTFD:data];
}
- (void)loadtextViewWithRTFData:(NSData *)data {
    [textViewIn replaceCharactersInRange:NSMakeRange(0, [[textViewIn string] length]) withRTF:data];
}
-(void) loadtextViewWithTextData:(NSData *)data {
      [textViewIn replaceCharactersInRange: NSMakeRange(0,[[textViewIn string] length]) 
	   withString:[NSString stringWithCString: [data bytes]  encoding:NSUTF8StringEncoding]];
}

// Return an NSData representation of our document's view.
- (NSData *)dataRepresentationOfType:(NSString *)aType {
   if ([@"RTFWithGraphics" compare: aType] == NSOrderedSame) {
     return [textViewIn RTFDFromRange:NSMakeRange(0, [[textViewIn string] length])];
   }else if ([@"RTF" compare: aType] == NSOrderedSame) {
     return [textViewIn RTFFromRange:NSMakeRange(0, [[textViewIn string] length])];
   }else {
     return [[textViewIn string] dataUsingEncoding: NSUTF8StringEncoding];
   }	
}

// If we're loading from a file just retain the data and it will be
//  loaded when windowControllerDidLoadNib is invoked. If not, go
//  ahead and load the view with the data.
// These paths are separate in order to support Revert where the
//  data is reloaded but the nib is not.
- (BOOL)loadDataRepresentation:(NSData *)data ofType:(NSString *)aType {
   if ([@"RTFWithGraphics" compare: aType] == NSOrderedSame) {
    if (textViewIn)
        [self loadtextViewWithData:data];
    else
        dataFromFile = [data retain];
   }else if ([@"RTF" compare: aType] == NSOrderedSame) {
    if (textViewIn)
        [self loadtextViewWithRTFData:data];
    else
        rtfDataFromFile = [data retain];
   }else{
     if (textViewIn)
	    [self loadtextViewWithTextData: data];
     else
	    textDataFromFile = [data retain]; 		   
   }		
    
    return YES;
}

// =======================================================================

-(void) initAux {
    static int serial;
    MyEnvironment *myEnvironment;
	myDocumentKey = [@"myDocumentNo" stringByAppendingString: [NSString stringWithFormat: @"%d",(++serial)]];
	[myDocumentKey retain];
	 
   	onlySelectedArea = 0;
	notebookMode = 0;  // or = 1; // experimental.
	oxEngine = 0; // experimental. For select engine menu.
	errorLines = [NSMutableArray arrayWithCapacity: 10]; [errorLines retain];
	debugMyTunnel = 0;
	task = nil;
	myEnvironment = [[MyEnvironment alloc] init]; [myEnvironment retain];

	myDefaultTypingAttributes = [myEnvironment getMyDefaultTypingAttributes];
	[myDefaultTypingAttributes retain];
	[textViewIn setTypingAttributes: myDefaultTypingAttributes];
	
	myDecoder = [[MyDecoder alloc] init]; [myDecoder retain];

	[self openMyModel: myEnvironment ];
}
-(int) getMyDocumentKey { return myDocumentKey; }

-(id) openMyModel: (MyEnvironment *) myEnvironment {
    [myEnvironment showForDebug]; // for debug.
    OpenXM_HOME= [myEnvironment getOpenXM_HOME];
	[OpenXM_HOME retain]; // without these SIGSEGV 
	inputCounter = outputCounter = 0;
	redirectPrint = 0;
	peer = [myEnvironment getPeer];
	peerStartupString = [myEnvironment getPeerStartupString];
	[peerStartupString retain];
	peerEndEvaluteMark = [myEnvironment getPeerEndEvaluateMark];
	[peerEndEvaluteMark retain];
 
	[self stopIndicator];
	// Initialization to call /bin/bash
    outboundPipe = [NSPipe pipe];  // since autorelease is called in pipe.
	inboundPipe   = [NSPipe pipe];
	errPipe      = [NSPipe pipe];
	[outboundPipe retain]; [inboundPipe retain]; [errPipe retain];
	outboundFileHandle = [outboundPipe fileHandleForWriting];
	inboundFileHandle  = [inboundPipe  fileHandleForReading];
	errFileHandle     = [errPipe      fileHandleForReading];

	NSLog(@"Starting /bin/bash\n");
	task = [[NSTask alloc] init];
    [task setStandardInput: outboundPipe];
	[task setStandardOutput: inboundPipe];
	[task setStandardError:  errPipe];
	[task setEnvironment: [myEnvironment getMyUnixEnvironment]];
	//[task setArguments: [NSArray arrayWithObject: @"http://www.google.com"]];
	//[task setLaunchPath: @"/usr/bin/open"];
	//[task setLaunchPath: @"/Users/NobukiTakayama/OX4/OpenXM/bin/asir"];
	//[task setLaunchPath: @"/bin/date"];
	[task setLaunchPath: @"/bin/bash"];
    // launch will be done after nib is loaded.
    @try {  // version >= 10.3,  add option in info
    	[task launch];
    } @catch (NSException *exception) {
       NSLog(@"init task launch Caught %@: %@", [exception name], [exception reason]);
    } @finally {
       NSLog(@"/bin/bash is started.\n");
	   // fprintf(stderr,"pid = %d\n",[task processIdentifier]);
	   signal(SIGPIPE,SIG_IGN); //BUG. non-cocoa way.
	   [self oxEvaluateString: peerStartupString withMark: FALSE]; 
    }
	
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(readInboundData:) 
      name:NSFileHandleDataAvailableNotification object: inboundFileHandle];
    [inboundFileHandle waitForDataInBackgroundAndNotify];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(readErrData:) 
      name:NSFileHandleDataAvailableNotification object: errFileHandle];
    [errFileHandle waitForDataInBackgroundAndNotify];
	
	return self;
}

- (void) closeMyModel {
  [self stopIndicator];
  if (task == nil) {
    NSLog(@"closeMyModel. Task is not running.\n"); return ;
  }
  [[NSNotificationCenter defaultCenter] removeObserver:self 
      name:NSFileHandleDataAvailableNotification object: inboundFileHandle];
  [[NSNotificationCenter defaultCenter] removeObserver:self 
      name:NSFileHandleDataAvailableNotification object: errFileHandle];

  [inboundFileHandle closeFile];
  [outboundFileHandle closeFile];
  [errFileHandle closeFile];
  [task terminate]; 
  task = nil; //BUG. memory leak?
  NSLog(@"Terminated the child process.\n");
} 

-(id) restartMyModel: (enum peer_type)  peerType {
    MyEnvironment *myEnvironment;
    [self closeMyModel]; 
	myEnvironment = [[MyEnvironment alloc] initFor: peerType];  //BUG. leak memory?
	[myEnvironment retain];
    [myEnvironment showForDebug]; // for debug.
	[self openMyModel: myEnvironment ];
    return self;
} 

// messenges
-(void) sayTheMessageAboutX {
  NSLog(@"myDocumentSaidTheMessageAboutX=%d\n",myDocumentSaidTheMessageAboutX);
  if (!myDocumentSaidTheMessageAboutX) {
    if ([MyEnvironment checkX] != 1) 
	   [self messageDialog: 
	     NSLocalizedString(@"A few commands (plot, ...) cannot be used, because X11 is not running.",nil) with: 0];
	myDocumentSaidTheMessageAboutX = 1;
  }
}

- (IBAction) oxEvaluate:(id) sender {
  NSString* cmd0;
  NSMutableString *cmd;
  NSRange r;

  if (inEvaluation) {
	 NSLog(@"In evaluatioin. ");
     [self messageDialog: NSLocalizedString(@"Evaluating...",nil) with: 0];
	 return;
  }
  if (notebookMode) doInsertNewInputCell=0;
  else [[MyOutputWindowController sharedMyOutputWindowController: self] showUp];
  ox_texmacs_level=0;
  [self clearLines];
  
  [textViewIn setTypingAttributes: myDefaultTypingAttributes];
  
  if (notebookMode) {
    cmd0 = [self getContentsOfInputCell];
	[self prepareOutputCell];
  }else if (onlySelectedArea) {	
    r = [textViewIn selectedRange];
	// NSLog(@"r=(%d,%d)\n",r.location,r.length);
    cmd0 = [textViewIn string]; 
	if (r.length != 0) {
		cmd0= [cmd0 substringWithRange: r];
	}else{
	    int pos;
	    r = [cmd0 lineRangeForRange: r];
		pos = r.location+r.length;
		[textViewIn setSelectedRange: r]; // select the range containing the insertion point.
		if (pos < [cmd0 length]) {
          [textViewIn replaceCharactersInRange: NSMakeRange(pos+1,0) withString: @""];
		}else{
          [textViewIn replaceCharactersInRange: NSMakeRange(pos,0) withString: @"\n"];
		}
		cmd0 = [cmd0 substringWithRange:r ]; 
	}	
  }else{
    cmd0 = [textViewIn string];
    MyOutputWindowController *mc;
    mc = [MyOutputWindowController sharedMyOutputWindowController: self];
    [mc clearOutputWindow];
  }
  cmd = [NSMutableString stringWithString: cmd0 ];

  [self changeInputCounterField: 1];  
  if (peer == OX_TEXMACS) [self startIndicator];
  [self oxEvaluateString: cmd withMark: TRUE];
  [self changeOutputCounterField: 1];
}

- (IBAction) oxInterrupt:(id) sender {

  NSAlert *alert = [[NSAlert alloc] init];
  [alert addButtonWithTitle: NSLocalizedString(@"OK",nil)];
  [alert addButtonWithTitle: NSLocalizedString(@"Cancel",nil)];
  [alert setMessageText: NSLocalizedString(@"Interrupt the ox server?",nil)];
  [alert setInformativeText:NSLocalizedString(@"The OX-RFC-100 interruption protocol is used.",nil)];
  [alert setAlertStyle:NSWarningAlertStyle];
  if ([alert runModal] != NSAlertFirstButtonReturn) {
    // Cancel clicked, return.
    return;
  } 
  [alert release];
								 
  fprintf(stderr,"Trying to interrupt.\n");
  [task interrupt];
  inputCounter = outputCounter = 0;
  [self stopIndicator];
  [myDecoder reset];
}
-(IBAction) oxRenderOutput: (id) sender {
  [self messageDialog: @"oxRenderOutput has not been implemented." with: 0];
}

-(IBAction) oxEvaluateSelected: (id) sender {
  // try or test something here
   // end of try or test.
   // fprintf(stderr,"OnlySelectedArea\n");
  if (notebookMode && (onlySelectedArea == 0)) {
    [self messageDialog: NSLocalizedString(@"Selection will be ignored in the notebook style.",nil) with: 0];
  }	
  if (onlySelectedArea) onlySelectedArea = 0; else onlySelectedArea = 1;
}

-(IBAction) oxEvaluateRegisteredString: (id) sender {
  int ans;
  NSAlert *alert = [[NSAlert alloc] init];
  [alert addButtonWithTitle: NSLocalizedString(@"Cancel",nil)];
  [alert addButtonWithTitle: NSLocalizedString(@"unix shell",nil)];
  [alert addButtonWithTitle: NSLocalizedString(@"Risa/Asir",nil)];
  [alert setMessageText: NSLocalizedString(@"Restart the computation server?",nil)];
  //[alert setInformativeText:NSLocalizedString(@"",nil)];
  [alert setAlertStyle:NSWarningAlertStyle];
  ans = [alert runModal];
  [alert release];
  if (ans == NSAlertFirstButtonReturn) {
    // Cancel clicked, return.
    return ;								 
  } else if (ans == NSAlertSecondButtonReturn) {
	[self restartMyModel: BASH];
    inputCounter = outputCounter = 0;
	return ;
  }else{
    [self restartMyModel: OX_TEXMACS];
    inputCounter = outputCounter = 0;
	[myDecoder reset];
    return;
  }	
} 

-(IBAction) checkSelectedArea: (id) sender {
  int ans;
  ans = [self selectIllegalCharacter];
  if (ans) return;
  [self selectBlockForward];
}

-(int) oxEvaluateString: (NSString *) cmd withMark: (BOOL) yes {
  NSData* cmdInData;
  NSLog(@"Evaluating...  ");
  NSLog(@"%@",cmd);
  // Evaluation is here.
  cmdInData = [cmd dataUsingEncoding: NSUTF8StringEncoding];
  @try { 
    //todo  if (yes) [outboundFileHandle writeData: peerStartEvaluateMark];
    [outboundFileHandle writeData: cmdInData];
    if (yes) [outboundFileHandle writeData: peerEndEvaluteMark];
  } @catch (NSException *exception) {
      NSLog(@"oxEvalluateString Caught %@: %@", [exception name], [exception reason]);
      [self messageDialog: NSLocalizedString(@"Error: Engine is not responding.",nil) with: 0];
	  return -1;
  } @finally {
      NSLog(@"writeData suceeded.\n");
  }
  // [NSThread sleepUntilDate: [NSDate dateWithTimeIntervalSinceNow: 10.0]];
  NSLog(@"\nDone.\n");
  return 0;
}

- (void) outputString: (NSString *) amt {
  NSRange myRange = NSMakeRange([[textViewOut textStorage] length],0);
  if (myDebug) NSLog(@"Get in outputString\n");  
  [textViewOut replaceCharactersInRange: myRange withString: amt];
  [textViewOut scrollRangeToVisible: NSMakeRange([[textViewOut textStorage] length],0)];
  //[textViewOut replaceCharactersInRange: NSMakeRange(0,NSMakeRange(0,[textViewOut length])) withString: amt];
  if (notebookMode) [self outputStringInNotebookMode: amt];
  else {
    MyOutputWindowController *mc;
    mc = [MyOutputWindowController sharedMyOutputWindowController: self];
    [mc outputStringToOutputWindow: amt];
  }	
}
- (void) outputErrorString: (NSString *) amt {
  int oldEnd;
  int newEnd;
  oldEnd = [[textViewOut textStorage] length];
  NSRange myRange = NSMakeRange(oldEnd,0);
  [textViewOut replaceCharactersInRange: myRange withString: amt];
  newEnd = [[textViewOut textStorage] length];
  [textViewOut setTextColor: [NSColor orangeColor] range: NSMakeRange(oldEnd,newEnd-oldEnd)];
  [textViewOut scrollRangeToVisible: NSMakeRange(newEnd,0)];
}

- (void) printErrorMessage: (NSString *) amt {
  MyOutputWindowController *mc;
  if (notebookMode) [self outputErrorStringInNotebookMode: amt];
  else {
    mc = [MyOutputWindowController sharedMyOutputWindowController: self];
    [mc printErrorMessageToOutputWindow: amt];
  }	
}
-(void)outputStringInNotebookMode: (NSString *)msg {
  [self outputStringInNotebookMode: msg withColor: [NSColor blueColor]];
}
-(void)outputErrorStringInNotebookMode: (NSString *)msg {
  [self outputStringInNotebookMode: msg withColor: [NSColor redColor]];
}
-(void)outputStringInNotebookMode: (NSString *)msg withColor: (NSColor *)color{
  NSRange range;
  int insertPos;
  range=[textViewIn selectedRange];
  insertPos = range.location+range.length;
  range = NSMakeRange(insertPos,0);
  [textViewIn replaceCharactersInRange: range withString: msg];
  range = NSMakeRange(insertPos,[msg length]);
  [textViewIn setTextColor: color range: range];
  range=NSMakeRange(range.location+range.length,0);
  [textViewIn scrollRangeToVisible: range];
  [textViewIn setSelectedRange: range];
 @synchronized(self) {  
  if (doInsertNewInputCell) {
    if (![self getOutOfOutputCell]) [self insertInputCell]; 
	doInsertNewInputCell = 0;
  }
 }
}

-(IBAction) clearTextViewOut: (id) sender {
  NSRange myRange = NSMakeRange(0,[[textViewOut textStorage] length]);
  [textViewOut replaceCharactersInRange: myRange withString: @""];
  [textViewOut replaceCharactersInRange: NSMakeRange(0,0) withString: @" \n \n \n"];
  [textViewOut scrollRangeToVisible: NSMakeRange(0,[[textViewOut textStorage] length])];
}

- (void) messageDialog: (NSString *)msg with: (int) no {
  char s[256];
  NSAlert *alert = [[NSAlert alloc] init];
  sprintf(s,"%d",no);
  [alert addButtonWithTitle: @"OK"];
  [alert setMessageText: msg] ;
  if (no) [alert setInformativeText: [NSString stringWithCString: (const char *) s]];
  [alert setAlertStyle:NSWarningAlertStyle];
  [alert runModal];
  [alert release];
}

-(void) printDocument: (id) sender {
  int status;
  NSAlert *alert = [[NSAlert alloc] init];
  [alert addButtonWithTitle:NSLocalizedString(@"Cancel",nil)];
  [alert addButtonWithTitle:NSLocalizedString(@"Print output view",nil)];
  [alert addButtonWithTitle:NSLocalizedString(@"Print input view",nil)];
  [alert setMessageText:NSLocalizedString(@"Which view will you print?",nil)];
  [alert setAlertStyle:NSWarningAlertStyle];
  status = [alert runModal];
  switch(status) {
    case NSAlertThirdButtonReturn:
		[textViewIn print: sender]; break;
    case NSAlertSecondButtonReturn:
	    [textViewOut print: sender]; break;			
	default:
    // Cancel clicked, return.
		return;
  }
  [alert release];
}

-(void) showHelp: (id)sender {
   BOOL ans;
   NSString *path=nil;
   //bug. temporary.   
   path = [OpenXM_HOME stringByAppendingString: @"/doc/cfep/intro-ja.html"]; 

   ans = [[NSWorkspace sharedWorkspace] openFile:path withApplication: @"Help Viewer"];
   if (ans != YES) {
      [self messageDialog: NSLocalizedString(@"Help file is not found at cfep.app/OpenXM/doc/cfep",nil) with: 0];
   }
}

-(void) changeInputCounterField: (int) inc {
  NSString *ss;
  ss = [NSString stringWithFormat: NSLocalizedString(@"Input%d",nil), inputCounter];
  [inputCounterField setStringValue: ss];
  inputCounter += inc;
  return;
}
-(void) changeOutputCounterField: (int) inc {
  NSString *ss;
  ss = [NSString stringWithFormat: NSLocalizedString(@"Out%d",nil),outputCounter];
  [outputCounterField setStringValue: ss];
  outputCounter += inc;
  return;
}
-(void) changeOutputCounterFieldWithString: (NSString *) ss {
  [outputCounterField setStringValue: ss];
  return;
}

-(void) startIndicator {
  if (inEvaluation) return ;
  inEvaluation = 1;
  [myIndicator startAnimation: self];
}
-(void) stopIndicator {
  if (inEvaluation == 0) return ;
  inEvaluation = 0;
  [myIndicator stopAnimation: self];
}


int debugInbound = 0;
- (void)readInboundData:(NSNotification*)aNotification
{
  static int state=0;
  static int u0 = 0;
  static int u1 = 0;
  static int u2 = 0;
  int n,i,j,k;
  unsigned char *d;
  unsigned char *d2;
  int c;
  NSMutableString* inboundString;
  NSData* data;
  NSMutableData* data2; 
  NSString *act;
  int channel;
  @synchronized (self) {
    if (debugInbound) NSLog(@"Reading inboudData\n");
    data=[ inboundFileHandle availableData ];
    n = [data length]; d = (unsigned char *)[data bytes];
    if (debugInbound) NSLog(@"n=%d\n",n);
//	if (n < 30) { for (k=0; k<n; k++) printf("%2x ",d[k]); printf("\n"); }
    if (n == 0) { NSLog(@"Broken pipe?\n"); [self messageDialog: @"Error: Engine is not responding." with: 0]; return ; }
    data2 = [NSMutableData dataWithCapacity: (n+4)];
    d2 = (unsigned char *)[data2 mutableBytes];
    i = 0;  d2[i] = 0;
    for (j = 0; j< n; j++) {
      c = d[j];
      if (c == END_OF_EVALUATION_OF_OX_TEXMACS) {
        [self stopIndicator]; 
		ox_texmacs_level--;
		if (ox_texmacs_level <= 0) doInsertNewInputCell = 1; 
      } else if (c == START_OF_RESULT_OF_OX_TEXMACS) {
	    ox_texmacs_level++;
	  }
      switch(state) {
      case 1:
        u1 = d[j];
        d2[i] = u0; i++; d2[i] = u1; i++; d2[i] = 0;
        state = 0; break;
      case 2:
        u1 = d[j]; state = 3; break;
      case 3:
        u2 = d[j];
        d2[i] = u0; i++; d2[i] = u1; i++; d2[i] = u2; i++ ; d2[i] = 0;
        state = 0; break;
      case 4:  // MYDECODER
        u1 = d[j]; state = 5; break;
      case 5:
        u2 = d[j];
        c = ((u0 & 0x3) << 6) | (((u1 & 0x7) << 3) & 0x38) | (u2 & 0x7);
        // NSLog(@"|%2x| ",c);  // send it to myDecoder
		act = [myDecoder myDecoder: c from: self];
		if (act != nil) {
		  if (debugMyTunnel) [self outputErrorString: act]; // for debug.
          channel = [myDecoder getChannel];
          if (channel == 0) [self errorActionFor: act];
		  else if (channel == 1) [self openGLActionFor: act];
		  else if (channel == 2) [self openGLInitActionFor: act];
        }
        state = 0; break;
      default:
        c=d[j];
        if ((c >> 5) == UTF8_2) { u0=c ; state=1; }
        else if ((c >> 4) == UTF8_3) { u0 =c ; state = 2; }
        else if ((c >> 2) == MYDECODER) { u0=c; state = 4; }
        else if ( c & 0x80) {  
          state = 0; NSLog(@"err|%2x| ",c);
        }else {
          if (c > 5) {
            d2[i] = c; i++; d2[i] = 0; state = 0;
          }
        }
      }
    }
  }
  if (i) {
    inboundString = [NSString stringWithCString: d2 encoding: NSUTF8StringEncoding];
    [self outputString: inboundString];  // do this out of the synchronized session.  
  }

  if (n) [[aNotification object] waitForDataInBackgroundAndNotify];
}


- (void)readErrData:(NSNotification*)aNotification
{
  int n,i,j;
#define MD_SIZE 5000
  unsigned char *d;
  int c;
  NSMutableString* inboundString;
  NSData*  inboundData ;
  @synchronized (self) {
  inboundString = [NSMutableString stringWithCapacity: (MD_SIZE+2)];

  // NSLog(@"(err) Reading inboudData\n");
  inboundData=[ errFileHandle availableData ];
  n = [inboundData length]; d = (unsigned char *)[inboundData bytes];
  i = 0;   
  while (i < n ) {
    for (j = 0; j<MD_SIZE ; j++ ) {
	   c = d[i];   // putchar(c);
	   [inboundString appendString: [NSString stringWithFormat: @"%c", (c & 0x7f)]  ];
	   i++; if (i >= n) { j++; break;}
	} 
	// NSLog( [NSString stringWithFormat: @"i=%d, j=%d\n", i, j] );
	j++;
//    NSLog(@"%@", inboundString );
    [self outputErrorString: inboundString];
    inboundString = [NSMutableString stringWithCapacity: (MD_SIZE+2)] ;
  }	  
  }
  if (n) [[aNotification object] waitForDataInBackgroundAndNotify];

}

-(int) specialCharacterInNotebookMode: (int) c {
  if (!notebookMode) return 0;
  if (c == BEGIN_INPUT_CELL) return 1;
  else if (c==BEGIN_OUTPUT_CELL) return 1;
  else if (c==END_INPUT_CELL) return 1;
  else if (c==END_OUTPUT_CELL) return 1;
  return 0;
}
-(int) selectIllegalCharacter {
  NSRange r;
  NSString *in;
  int i, n;
  int state,offset;
  int c,c2;
  // 1: in /*
  // 2: in //
  // 3: in "
  // 4: in '
  in = [textViewIn string];
  r = [textViewIn selectedRange];
  //NSLog(@"%d, %d\n",r.location, r.length);
  if (r.length < 1) {
    offset = 0;
    n = [in length];
  }else{
    offset = r.location;
    n = offset + r.length;
  }
  state = 0;
  for (i=offset; i<n; i++) {
    c = [in characterAtIndex: i];  // NSLog(@"|%04x| ",c);
    if (i != n-1) c2 = [in characterAtIndex: (i+1)];
    else c2 = 0;
    switch (state) {
    case 0:
      if ((c == 0x1b)) {
        [textViewIn setSelectedRange: NSMakeRange(i,1)];
        [self outputErrorString:
                NSLocalizedString(@"Error: Illegal control character.\n",nil)];
        return 1;
      }
      if ((c > 0x7e) && (![self specialCharacterInNotebookMode: c])) {
        [textViewIn setSelectedRange: NSMakeRange(i,1)];
        [self outputErrorString:
                NSLocalizedString(@"Error: Program part must be written only by ascii characters.\n",nil)];
        return 1;
      }
      if (((c == '/') && (c2 == '*')) || (notebookMode && (c == BEGIN_OUTPUT_CELL))) {
        i++; state = 1;
      }else if ((c == '/') && (c2 == '/')) {
        i++; state = 2;
      }else if (c == '"') {
        state = 3;
      }else if (c == '\'') {
        state = 4;
      }else state=0;
      break;
    case 1:
      if (((c == '*') && (c2 == '/')) || (notebookMode && (c2 == END_OUTPUT_CELL))) {
        i++; state = 0;
      }
      break;
    case 2:
      if (c == '\n') state = 0;
      break;
    case 3:
      if (c == '"') state = 0;
      else if (c == '\\') {
        i++; //BUG. skip only one letter.
      }
      break;
    case 4:
      if (c == '\'') state = 0;
      else if (c == '\\') {
        i++; //BUG. skip only one letter.
      }
      break;
    }
  }
//  [self outputErrorString: @"No illegal character.\n"];
  return 0; 
}

-(NSRange) selectBlockForward {
  NSRange r;
  NSRange ans;
  NSString *in;
  int i, n;
  int first, last,offset, state;
  int c,c2;
  static int prevStart = -1;

  first = last = 0;
  ans = NSMakeRange(0,0);

  // 1: in /*
  // 2: in //
  // 3: in "
  // 4: in '
  in = [textViewIn string];
  r = [textViewIn selectedRange];
  if (r.length < 1) {
    offset = 0;
  } else {
    offset = r.location;
  }
  if (offset == prevStart) {
    offset = offset+1;
  }
  n = [in length]; 
  if (offset >= n) offset = 0;
  state = 0;
  for (i=offset; i<n; i++) {
    c = [in characterAtIndex: i];
    if (i != n-1) c2 = [in characterAtIndex: (i+1)];
    else c2 = 0;
    switch (state) {
    case 0:
      if (c == '{') {
        ans=[self selectBlockForwardFrom: i with: c and: '}'];
		if (prevStart != ans.location) { prevStart = ans.location; }
		return ans;
      }else if (c == '(') {
        ans=[self selectBlockForwardFrom: i with: c and: ')']; 
		if (prevStart != ans.location) { prevStart = ans.location; }
		return ans;
      }else if (c == '[') {
        ans=[self selectBlockForwardFrom: i with: c and: ']']; 
		if (prevStart != ans.location) { prevStart = ans.location; }
		return ans;
	  }
      if ((c == '/') && (c2 == '*')) {
        i++; state = 1;
      }else if ((c == '/') && (c2 == '/')) {
        i++; state = 2;
      }else if (c == '"') {
        state = 3;
      }else if (c == '\'') {
        state = 4;
      }else state=0;
      break;
    case 1:
      if ((c == '*') && (c2 == '/')) {
        i++; state = 0;
      }
      break;
    case 2:
      if (c == '\n') state = 0;
      break;
    case 3:
      if (c == '"') state = 0;
      else if (c == '\\') {
        i++; //BUG. skip only one letter.
      }
      break;
    case 4:
      if (c == '\'') state = 0;
      else if (c == '\\') {
        i++; //BUG. skip only one letter.
      }
      break;
    }
  }
  return ans;
}
-(NSRange) selectBlockForwardFrom: (int) offset with: (int) first and: (int) last {
  NSRange r;
  NSRange ans;
  NSString *in;
  int i, n;
  int state;
  int c,c2;
  int level;
  int startpos;

  level = 0;
  ans = NSMakeRange(0,0);

  // 1: in /*
  // 2: in //
  // 3: in "
  // 4: in '
  in = [textViewIn string];
  r = [textViewIn selectedRange];
  n = [in length];
  state = 0;
  for (i=offset; i<n; i++) {
    c = [in characterAtIndex: i];
    if (i != n-1) c2 = [in characterAtIndex: (i+1)];
    else c2 = 0;
    switch (state) {
    case 0:
      if (level == 0) {
        if (c == first) {
          level = 1; startpos = i;
        }
      } else if (level > 0) {
        if (c == first) level++;
        else if (c == last) level--;
        else ;
        if (level == 0) {
          ans = NSMakeRange(startpos, i+1-startpos);
          [textViewIn setSelectedRange: ans];
          [textViewIn scrollRangeToVisible: NSMakeRange(startpos,0)];
          return ans;
        }
      }

      if ((c == '/') && (c2 == '*')) {
        i++; state = 1;
      }else if ((c == '/') && (c2 == '/')) {
        i++; state = 2;
      }else if (c == '"') {
        state = 3;
      }else if (c == '\'') {
        state = 4;
      }else state=0;
      break;
    case 1:
      if ((c == '*') && (c2 == '/')) {
        i++; state = 0;
      }
      break;
    case 2:
      if (c == '\n') state = 0;
      break;
    case 3:
      if (c == '"') state = 0;
      else if (c == '\\') {
        i++; //BUG. skip only one letter.
      }
      break;
    case 4:
      if (c == '\'') state = 0;
      else if (c == '\\') {
        i++; //BUG. skip only one letter.
      }
      break;
    }
  }
  if (level > 0) {
    [self outputErrorString: NSLocalizedString(@"Error: No right parenthesis.\n",nil)] ;
    [textViewIn setSelectedRange: NSMakeRange(startpos,n-startpos)];
    ans = NSMakeRange(startpos,-1);
    return ans;
  }
  return ans;
}

-(void) errorActionFor: (NSString *)cmd
{
  NSArray *a;
  NSRange r;
  static NSRange r2;
  static NSRange r3;
  static int r2IsSet;
  static int r3IsSet;
  NSString *s;
  int line,i, currentLine;
  // NSLog(@"cmd=%@\n",cmd);
  if ([cmd hasPrefix: @"Begin"] == YES) {
     r3IsSet = 0; r2IsSet = 0;
  }else if ([cmd hasPrefix: @"End"] == YES) {
     if (r2IsSet) {
       [textViewIn setSelectedRange: r2];
       [textViewIn scrollRangeToVisible: r2];
	    r2IsSet = 0; 
     }
     if (r3IsSet) {  //gotoLine, xyz is stronger than findAndSelect.
        [textViewIn setSelectedRange: r3];
        [textViewIn scrollRangeToVisible: r3];
		r3IsSet = 0; 
	 }
  }else if ([cmd hasPrefix: @"findAndSelect"] == YES) {
    a = [cmd componentsSeparatedByString: @", "];
	if ([a count] > 1) {
	  a = [[a objectAtIndex: 1] componentsSeparatedByString: @"\n"];
	  s = [a objectAtIndex: 0];
      r2 = [[textViewIn string] rangeOfString: s]; 
	  r2IsSet = 1;
	  // selection will be done in End.
  	  [self printErrorMessage: [NSLocalizedString(@"Error in executing ",nil) stringByAppendingString: s]];
  	  [self printErrorMessage: @"\n"];
	}
  }else if ([cmd hasPrefix: @"findAndShow"] == YES) {
    a = [cmd componentsSeparatedByString: @", "];
	if ([a count] > 1) {
	  a = [[a objectAtIndex: 1] componentsSeparatedByString: @"\n"];
	  s = [a objectAtIndex: 0];
  	  [self printErrorMessage: [NSLocalizedString(@"Error in executing ",nil) stringByAppendingString: s]];
  	  [self printErrorMessage: @"\n"];
	}
  }else if ([cmd hasPrefix: @"printErrorMessage"] == YES) {
    a = [cmd componentsSeparatedByString: @"ErrorMessage, "];
	if ([a count] > 1) {
	  s = [a objectAtIndex: 1];
	  [self printErrorMessage: [NSLocalizedString(@"Error: ",nil) stringByAppendingString: s]];
  	  [self printErrorMessage: @"\n"];
    }
  }else if ([cmd hasPrefix: @"gotoLine"] == YES) {
    a = [cmd componentsSeparatedByString: @"Line, "];
	if ([a count] > 1) {
	  s = [a objectAtIndex: 1];
      [self printErrorMessage: [NSLocalizedString(@"Error at ",nil) stringByAppendingString: s]];
      [self printErrorMessage: @"\n"];
	  line = [s intValue];
	  [self addLine: line];
	  // move the selection to the line.
	  i = [self gotoLine: line];
	  if (i >= 0) {
		  r3 = NSMakeRange(i,0);   // NSLog(@"Goto the location=%d\n",i);
		  r3IsSet = 1;
	  }	  
    }
  }
}

-(void) clearLines {
  [errorLines removeAllObjects];
  [self nextLine: -1];
}
-(void) addLine: (int) n {
  [errorLines addObject: [NSNumber numberWithInt: n]];
}
-(int) nextLine: (int) n {
  static int p = 0;
  int v;
  if (n < 0) { p = 0; return -1; }
  if (p < [errorLines count]) ; else { p = 0; return -1;} 
  v = [[errorLines objectAtIndex: p] intValue];
  p++;
  return v;	
}
-(int) gotoLine: (int) line {
      NSString *s;
	  NSRange r;
	  NSRange r3;
	  int currentLine,i;
	  // move the selection to the line line.   
	  s = [textViewIn string];
	  if (onlySelectedArea || notebookMode) r = [textViewIn selectedRange];
	  else r = NSMakeRange(0,[s length]);
	  // NSLog(@"r.location=%d",r.location);
	  currentLine = 1;
	  for (i=r.location; i< r.location+r.length; i++) {
	    if (currentLine == line) {
		  r3 = NSMakeRange(i,0);   // NSLog(@"Goto the location=%d\n",i);
          [textViewIn setSelectedRange: r3 ];
          [textViewIn scrollRangeToVisible: r3];
		  return i; // set r3IsSet 
		  break;
		}
	    if ([s characterAtIndex: i] == '\n') ++currentLine;
	  }
	  return -1;
}
-(int) gotoNextError: (id) sender{
  int n;
  n = [self nextLine: 0];
  if (n >= 0) { [self gotoLine: n]; return n;}
  else return -1;
}

/* act= 0;[glRectf,-0.5, ...] */
-(int) openGLActionFor: (NSString *)act {
  NSArray *a;
  int gid;
  a=[act componentsSeparatedByString: @";"];
  if ([a count] < 2) {
    [self printErrorMessage: @"Invalid format in openGLActionFor. gid and command must be separated by ;\n"];
	return -1;
  }
  gid = [[a objectAtIndex: 0] intValue];
  [MyOpenGLController addOglComm: [a objectAtIndex: 1] to: gid from: self];
  return 0;
}
-(int) openGLInitActionFor: (NSString *) act {
  NSArray *a;
  int gid;
  a=[act componentsSeparatedByString: @";"];
  if ([a count] < 2) {
    [self printErrorMessage: @"Invalid format in openGLInitActionFor. gid and command must be separated by ;\n"];
	return -1;
  }
  gid = [[a objectAtIndex: 0] intValue];
  [MyOpenGLController addOglInitComm: [a objectAtIndex: 1] to: gid from: self];
  return 0;
}

-(void) setBasicStyle: (id) sender {
  notebookMode = 0;
  [self updateInterfaceStyleMenu];
}
-(void) setNotebookStyle: (id) sender {
  notebookMode = 1;
  [self updateInterfaceStyleMenu];
  [self insertInputCell];
}
-(void) updateInterfaceStyleMenu {
  if (!menuItemNotebookMode) return;
  if (!menuItemBasicMode) return;
  if (notebookMode) {
    [menuItemNotebookMode setState: NSOnState];
	[menuItemBasicMode setState: NSOffState];
  }else	{
    [menuItemNotebookMode setState: NSOffState];
	[menuItemBasicMode setState: NSOnState];
  }	
}
-(void) setEngineRisaAsir: (id) sender {
  oxEngine = 0;
  [self updateSelectEngineMenu];
  [self oxEvaluateString: @"!asir;" withMark: TRUE];
}
-(void) setEngineKanSm1: (id) sender {
  oxEngine = 1;
  [self updateSelectEngineMenu];
  [self oxEvaluateString: @"!sm1;" withMark: TRUE];
}
-(void) updateSelectEngineMenu {
  if (!menuItemRisaAsir) return;
  if (!menuItemKanSm1) return;
  if (oxEngine == 0) {
    [menuItemRisaAsir setState: NSOnState];
	[menuItemKanSm1 setState: NSOffState];
  }else	if (oxEngine == 1) {
    [menuItemRisaAsir setState: NSOffState];
	[menuItemKanSm1 setState: NSOnState];
  }	
}
-(void) setDebugMyTunnel: (id) sender {
  if (debugMyTunnel) {
    debugMyTunnel = 0;
	if (menuItemOutputDebugMessages) [menuItemOutputDebugMessages setState: NSOffState];
  } else {
    debugMyTunnel = 1;
	if (menuItemOutputDebugMessages) [menuItemOutputDebugMessages setState: NSOnState];
  }	
  [MyUtil setDebugMyUtil];
}
-(void) mytest: (id) sender {
  // Add code here for testing.
  int n;
  n=[self gotoNextError: nil];
  NSLog(@"error line=%d\n",n);
}
-(void) addMenuExec {
  static int done=0;
  NSMenu *menuItem;
  NSMenu *menuExec;
  NSMenuItem *menuItemEvaluate;
  NSMenuItem *menuItemInterrupt;
  NSMenuItem *menuItemInterfaceStyle;
  NSMenuItem *menuItemSelectEngine;
  NSMenuItem *menuItemGotoNextError;
  NSMenuItem *menuItemForTest;
  if (done) return;
  done = 1;
  // oxEvalute
  menuExec = [[[NSMenu alloc] init] autorelease];
  [menuExec setTitle: NSLocalizedString(@"Execution",nil)];
  menuItemEvaluate = [[[NSMenuItem alloc] init] autorelease];
  [menuItemEvaluate setTitle: NSLocalizedString(@"Evaluate",nil)];
  [menuItemEvaluate setAction: @selector(oxEvaluate:)];
  [menuItemEvaluate setTarget: [[NSApp mainWindow] document]];
  [menuItemEvaluate setKeyEquivalentModifierMask: NSCommandKeyMask];
  [menuItemEvaluate setKeyEquivalent: @"\n"];
  [menuExec addItem: menuItemEvaluate];
  // oxInterrupt
  menuItemInterrupt = [[[NSMenuItem alloc] init] autorelease];
  [menuItemInterrupt setTitle: NSLocalizedString(@"Interrupt",nil)];
  [menuItemInterrupt setAction: @selector(oxInterrupt:)];
  [menuItemInterrupt setTarget: [[NSApp mainWindow] document]];  // do not use self.
  [menuExec addItem: menuItemInterrupt];

  // gotoNextError
  menuItemGotoNextError = [[[NSMenuItem alloc] init] autorelease];
  [menuItemGotoNextError setTitle: NSLocalizedString(@"Next error line",nil)];
  [menuItemGotoNextError setAction: @selector(gotoNextError:)];
  [menuItemGotoNextError setTarget: [[NSApp mainWindow] document]]; 
  [menuExec addItem: menuItemGotoNextError];

  // gotoNextError
  menuItemOutputDebugMessages = [[[NSMenuItem alloc] init] autorelease];
  [menuItemOutputDebugMessages setTitle: NSLocalizedString(@"Output debug messages",nil)];
  [menuItemOutputDebugMessages setAction: @selector(setDebugMyTunnel:)];
  [menuItemOutputDebugMessages setTarget: [[NSApp mainWindow] document]]; 
  [menuExec addItem: menuItemOutputDebugMessages];

  // interface style
  NSMenu *submenuInterfaceItem = [[[NSMenu alloc] init] autorelease];
    // basic-like
  NSMenuItem *menuItemBasicLike = [[[NSMenuItem alloc] init] autorelease];
  [menuItemBasicLike setTitle: NSLocalizedString(@"BASIC-like",nil)];
  [menuItemBasicLike setAction: @selector(setBasicStyle:)];
  [menuItemBasicLike setTarget: [[NSApp mainWindow] document]];
  menuItemBasicMode = menuItemBasicLike;
  [submenuInterfaceItem addItem: menuItemBasicLike];
    // notebook-like
  NSMenuItem *menuItemNotebookLike = [[[NSMenuItem alloc] init] autorelease];
  [menuItemNotebookLike setTitle: NSLocalizedString(@"Notebook-like",nil)];
  [menuItemNotebookLike setAction: @selector(setNotebookStyle:)];
  [menuItemNotebookLike setTarget: [[NSApp mainWindow] document]];
  menuItemNotebookMode = menuItemNotebookLike;
  [submenuInterfaceItem addItem: menuItemNotebookLike];
  
  menuItemInterfaceStyle = [[[NSMenuItem alloc] init] autorelease];
  [menuItemInterfaceStyle setTitle: NSLocalizedString(@"Interface style",nil)];
  [menuItemInterfaceStyle setSubmenu: submenuInterfaceItem];
  [menuExec addItem: menuItemInterfaceStyle];
  [self updateInterfaceStyleMenu];
  
    // select engine
  NSMenu *submenuSelectEngineItem = [[[NSMenu alloc] init] autorelease];
    // asir
  menuItemRisaAsir = [[[NSMenuItem alloc] init] autorelease];
  [menuItemRisaAsir setTitle: NSLocalizedString(@"Risa/Asir",nil)];
  [menuItemRisaAsir setAction: @selector(setEngineRisaAsir:)];
  [menuItemRisaAsir setTarget: [[NSApp mainWindow] document]];
  [submenuSelectEngineItem addItem: menuItemRisaAsir];
    // notebook-like
  menuItemKanSm1 = [[[NSMenuItem alloc] init] autorelease];
  [menuItemKanSm1 setTitle: NSLocalizedString(@"Kan/sm1",nil)];
  [menuItemKanSm1 setAction: @selector(setEngineKanSm1:)];
  [menuItemKanSm1 setTarget: [[NSApp mainWindow] document]];
  [submenuSelectEngineItem addItem: menuItemKanSm1];
  
  menuItemSelectEngine = [[[NSMenuItem alloc] init] autorelease];
  [menuItemSelectEngine setTitle: NSLocalizedString(@"Select engine",nil)];
  [menuItemSelectEngine setSubmenu: submenuSelectEngineItem];
  [menuExec addItem: menuItemSelectEngine];
  [self updateSelectEngineMenu];


  // test
  menuItemForTest = [[[NSMenuItem alloc] init] autorelease];
  [menuItemForTest setTitle: NSLocalizedString(@"For test",nil)];
  [menuItemForTest setAction: @selector(mytest:)];
  [menuItemForTest setTarget: [[NSApp mainWindow] document]]; 
  [menuExec addItem: menuItemForTest];

  menuItem = [[[NSMenuItem alloc] init] autorelease];
  [[NSApp mainMenu] insertItem: menuItem atIndex: 4];
  [menuItem setSubmenu: menuExec];
}

-(void) insertInputCell {
  NSRange r;
  NSString *ic;
  unichar uu[2];
  uu[0] = BEGIN_INPUT_CELL; uu[1] = END_INPUT_CELL;
  r = [textViewIn selectedRange]; r.length=0;
  ic = [NSString stringWithCharacters: uu length: 2]; 
  // NSLog(@"insertInputCell: r.location=%d, r.length=%d, %@\n",r.location,r.length,ic); 
  [textViewIn replaceCharactersInRange: r withString: ic];
  r.location = r.location+1; r.length = 0;
  [textViewIn setSelectedRange: r ];
  r.location = r.location-1; r.length = 2;
  [textViewIn setTextColor: [NSColor blackColor] range: r];
}
-(NSString *) getContentsOfInputCell {
    int start,end,i,n;
	NSString *cmd0;
	NSRange r;
    cmd0 = [textViewIn string]; 
	start=0; end = n = [cmd0 length]; if (end > 0) end--;
    r = [textViewIn selectedRange];
	if (r.location < n) i = r.location; else i = n-1;
	if (i < 0) i=0;
	for (; i>=0; i--) {
	  if ([cmd0 characterAtIndex: i] == BEGIN_INPUT_CELL) {
	    start = i+1; break;
	  }
	}
	for (i=start; i<n; i++) {
	  if ([cmd0 characterAtIndex: i] == END_INPUT_CELL) {
	    end = i-1; break;
	  }
	}
	// NSLog(@"start=%d, end=%d\n",start,end);
	r.location = start; r.length = end-start+1; 
	if (r.length > 0) {
		cmd0= [cmd0 substringWithRange: r];
	}else{
	    cmd0 = @""; 
	}	
	if (end+2 <= n) r.location=end+2; 
	r.length = 0;
	[textViewIn setSelectedRange: r];
    return cmd0;
}
// return 1 if there is the next inputCell
-(int) getOutOfOutputCell {
    int start,end,i,n;
	NSString *cmd0;
	NSRange r;
    cmd0 = [textViewIn string]; 
	n = [cmd0 length]; 
    r = [textViewIn selectedRange];
	start = r.location;
	// NSLog(@"r.location=%d, r.length=%d\n",r.location,r.length);
	if (r.location < n) i = r.location; else i = n-1;
	if (i>0) i--;
	for (; i<n; i++) {
	  if ([cmd0 characterAtIndex: i] == END_OUTPUT_CELL) {
	    start = i+1; break;
	  }else if ([cmd0 characterAtIndex: i] == BEGIN_INPUT_CELL) {
	    r.location = i+1; r.length=0;
		[textViewIn setSelectedRange: r];  [textViewIn scrollRangeToVisible: r];
		return 1;
	  }else if ([cmd0 characterAtIndex: i] == END_INPUT_CELL) {
	    r.location = i-1; r.length=0;
		[textViewIn setSelectedRange: r];  [textViewIn scrollRangeToVisible: r];
		return 1;
	  }
	}
	r.location = start; r.length = 0;
	[textViewIn setSelectedRange: r];

	for (i = start; i<n; i++) {
      // Try to find the next input cell.
	  if ([cmd0 characterAtIndex: i] == BEGIN_INPUT_CELL) {
	    r.location = i+1; r.length = 0;
		[textViewIn setSelectedRange: r];  [textViewIn scrollRangeToVisible: r];
		return 1;
	  }else if ([cmd0 characterAtIndex: i] == END_INPUT_CELL) {
	    // We are inside an input cell.
	    r.location = i-1; r.length = 0;
		[textViewIn setSelectedRange: r]; [textViewIn scrollRangeToVisible: r];
		return 1;
	  }
	}
	return 0;
}
-(void) prepareOutputCell {
  int start,end,i,j,n,m;
  NSString *cmd0;
  NSString *ic;
  unichar uu[7];
	NSRange r;
	start = -1; end = -1;
    cmd0 = [textViewIn string]; 
	n = [cmd0 length]; 
    r = [textViewIn selectedRange];
	i = r.location; 
	if (i < 0) i = 0;
	if (i >= n) i = n-1;
	for (; i<n; i++) {
	  if ([cmd0 characterAtIndex: i] == BEGIN_OUTPUT_CELL) {
	    start = i;
		for (j=i; j>=0; j--) {
		  if ([cmd0 characterAtIndex: j] <= ' ') {
		    start = j;
		  }	else { 
		    if (start>0) start--; 
		    break;
	      }
		}
		break;
	  }else if ([cmd0 characterAtIndex: i] == BEGIN_INPUT_CELL) {
	    start = i-1;
		if (start < 0) start = 0;
		end = -2;
		break;
	  }
	}
	if (end == -2) {
	  r.location = start; r.length=0;
	}else{
	  end = start;
  	  i = start; if (i < 0) i = 0;
	  for (; i<n; i++) {
	    if ([cmd0 characterAtIndex: i] == END_OUTPUT_CELL) {
	      end = i; break;
	    }
	  }
      if ((start == -1) || (end == -1)) {
  	    r.location = n; r.length=0;
      }else{
        r.location = start; r.length = end-start+1; 
	  }  
	}  
    [textViewIn replaceCharactersInRange: r withString: @""];

  uu[0] = '\n';
  uu[1] = BEGIN_OUTPUT_CELL; uu[2] = END_OUTPUT_CELL;
  m = 3;
  // If there is no next input cell, add a new one.
  for (i=r.location; i<n; i++) {
    if ([cmd0 characterAtIndex:i] == BEGIN_INPUT_CELL) {
      break;
	}
  }
  if ((i == n) || (r.location >= n)) {
	  uu[3] = '\n'; uu[4] = BEGIN_INPUT_CELL; uu[5] = END_INPUT_CELL; m=6;
  }  
  // NSLog(@"prepareOutputCell start=%d, length=%d, m=%d\n",start,r.length,m);
  r.length = 0;
  ic = [NSString stringWithCharacters: uu length: m]; 
  [textViewIn replaceCharactersInRange: r withString: ic];
  r.location = r.location+2; r.length = 0;
  [textViewIn setSelectedRange: r ];
}
@end
