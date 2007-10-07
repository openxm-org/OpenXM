//
//  MyEnvironment.m
//  cfep
//
//  Created by Nobuki Takayama on 06/01/22.
//  Copyright 2006 OpenXM.org. All rights reserved.
//

#import "MyEnvironment.h"


@implementation MyEnvironment

// -(MyEnvironment *) initFor:with: ;    // init with a filename to load.  MyEnvironment may be written in the file.

// init sets up  protocol resources and environment for the task.
-(MyEnvironment *) init {
  return [self initFor: OX_TEXMACS];
}
-(MyEnvironment *) initFor: (enum peer_type) myPeerType {
  int i;
  NSString *s;
  NSSocketPort *s6000;
#define SIZE_e  5
  NSString *e[]       ={@"ASIRRC", @"OX_XTERM", @"LC_CTYPE",@"LC_ALL", @"LANG"};
  NSString *eDefault[]={nil      ,  nil,          nil,         nil,      nil    };     
  if (self = [super init]) {
    peer = myPeerType;
		
	isXRunning = [MyEnvironment checkX];
	
    //long initialization to the default value.  Personal initialization will be read in the different method.
    //(1)local string ( or default value)-->(2)preferences by .cfep/cfep.init  
    // --> private in the file 
    OpenXM_HOME=[[[NSBundle mainBundle]  bundlePath] stringByAppendingString: @"/OpenXM"];
    [OpenXM_HOME retain];


    s = NSLocalizedString(@"PEER_END_EVALUATE_MARK",nil); 
    if ([s compare: @"PEER_END_EVALUATE_MARK"] == NSOrderedSame) { 
      // default value
	  if (myPeerType == OX_TEXMACS) 
		peerEndEvaluateMark = [NSData dataWithBytes: (void *)"\005" length: 1];
	  else peerEndEvaluateMark = [NSData dataWithBytes: (void *)"\n" length: 1];
    }else{ 
      NSLog(@"PEER_END_EVALUATION_MARK. Not yet implemented.");
    }  
    [peerEndEvaluateMark retain];

    s = NSLocalizedString(@"PEER_STARTUP_STRING",nil); 
    if ([s compare: @"PEER_STARTUP_STRING"] == NSOrderedSame) { 
      // default value
	  if (myPeerType == OX_TEXMACS) 
        peerStartupString = [OpenXM_HOME stringByAppendingString: @"/bin/ox_texmacs --view cfep --engineLogToStdout -asirInitFile cfep-init.rr\n"];
	  else peerStartupString = @"\n";	
    }else{ 
	  if (myPeerType == OX_TEXMACS) 
		peerStartupString = [OpenXM_HOME stringByAppendingString: [NSString stringWithString: s]];
	  else peerStartupString = @"\n";	
    }  
    [peerStartupString retain]; 

    s = NSLocalizedString(@"PEER_INITIALIZATION_STRING",nil); 
    if ([s compare: @"PEER_INITIALIZATION_STRING"] == NSOrderedSame) { 
      // default value
      peerInitializationString = @"";
    }else{ 
      peerInitializationString = [NSString stringWithString: s];
    }  
    [peerInitializationString retain];

    s = NSLocalizedString(@"PEER_HELP_DIR",nil); 
    if ([s compare: @"PEER_HELP_DIR"] == NSOrderedSame) { 
      // default value
      peerHelpDir = [OpenXM_HOME stringByAppendingString:
						@"/doc/cfep/intro-ja.html"];
    }else{ 
      peerHelpDir = [NSString stringWithString: s];
    }  
    [peerHelpDir retain];

    s = NSLocalizedString(@"PEER_OPTION_STRING",nil); 
    if ([s compare: @"PEER_OPTION_STRING"] == NSOrderedSame) { 
      // default value
      if (myPeerType == OX_TEXMACS) peerOptionString = @"";  // @" --noWindow ";
	  else peerOptionString =@"";
    }else{ 
      peerOptionString = [NSString stringWithString: s];
    }  
    [peerOptionString retain];

    myUnixEnvironment = [NSMutableDictionary dictionaryWithCapacity: 32];
    [myUnixEnvironment retain];
    [myUnixEnvironment setObject: OpenXM_HOME forKey: @"OpenXM_HOME"];
    for (i=0; i< SIZE_e; i++) {
      s = NSLocalizedString(e[i],nil); 
      if ([s compare: e[i]] == NSOrderedSame) { 
		if (eDefault[i] != nil) {
			[myUnixEnvironment setObject: eDefault[i] forKey: e[i]];
		}
      }else{ 
		[myUnixEnvironment setObject: [NSString stringWithString: s]
			   forKey: e[i]];
	  }
	}

    s = NSLocalizedString(@"DISPLAY",nil); 
    if ([s compare: @"DISPLAY"] == NSOrderedSame) { 
	  if (isXRunning) [myUnixEnvironment setObject: @":0.0" forKey: @"DISPLAY"];
    }else{ 
        [myUnixEnvironment setObject: s forKey: @"DISPLAY"];
    }  

    s = NSLocalizedString(@"XAUTHORITY",nil); 
    if ([s compare: @"XAUTHORITY"] == NSOrderedSame) { 
	  if (isXRunning) 
	   [myUnixEnvironment setObject: [NSHomeDirectory() stringByAppendingString: @"/.Xauthority"]
			forKey: @"XAUTHORITY"];
    }else{ 
        [myUnixEnvironment setObject: s forKey: @"XAUTHORITY"];
    }  

    s = NSLocalizedString(@"DYLD_LIBRARY_PATH",nil); 
    if ([s compare: @"DYLD_LIBRARY_PATH"] == NSOrderedSame) { 
      [myUnixEnvironment setObject: [OpenXM_HOME stringByAppendingString: @"/lib"]
		forKey: @"DYLD_LIBRARY_PATH"];
    }else{ 
      [myUnixEnvironment setObject: s forKey: @"DYLD_LIBRARY_PATH"];
    }  

    s = NSHomeDirectory();
    [myUnixEnvironment setObject: s forKey: @"HOME"];

    s = NSLocalizedString(@"PATH",nil); 
    if ([s compare: @"PATH"] == NSOrderedSame) { 
      char *path;
	  NSString *defaultPath;
	  defaultPath=@"/usr/bin:/bin:/usr/sbin:/sbin:/usr/X11R6/bin:/sw/bin:/sw/sbin:/usr/local/bin";
	  path = getenv("PATH");
	  if (!path) {
	    s = [NSString stringWithCString: path length: strlen(path)];
		s = [s stringByAppendingString: @":"];
		s = [s stringByAppendingString: defaultPath];
		[myUnixEnvironment setObject: s forKey: @"PATH"];
       } else [myUnixEnvironment setObject: defaultPath forKey: @"PATH"];
    } else {
	   [myUnixEnvironment setObject: s forKey: @"PATH"];
    }

	
    myDefaultTypingAttributes=
      [NSDictionary dictionaryWithObject: [NSFont fontWithName: @"Courier" size: 12] forKey: @"NSFont"];
    [myDefaultTypingAttributes retain];
    //  [textViewIn setTypingAttributes: adic];


  }
  [self retain];
  return self;
}
-(void) dealloc {
  NSLog(@"dealloc of MyEnvironment.\n");
  [OpenXM_HOME autorelease];  // or release ?
  [peerEndEvaluateMark autorelease];
  [peerStartupString autorelease];
  [peerInitializationString autorelease];
  [peerHelpDir autorelease];
  [peerOptionString autorelease];
  [myUnixEnvironment autorelease];
  [super dealloc];
}
+(int) checkX {
  NSEnumerator *apps = [[[NSWorkspace sharedWorkspace] launchedApplications] objectEnumerator ];
  NSDictionary *dicApp;
  while (dicApp = [apps nextObject]) {
    if ([@"X11" compare: [dicApp objectForKey: @"NSApplicationName"]] == NSOrderedSame) {
	  NSLog(@"X11 is running.\n");
	  return 1;
	}  
  }	
  NSLog(@"X11 is NOT running.\n");
  return 0;  
}
-(void) showForDebug {
  NSLog(@"OpenXM_HOME=%@\n",OpenXM_HOME);
  NSLog(@"peer=%d\n",peer);
  NSLog(@"peerEndEvaluateMark=%@\n",peerEndEvaluateMark);
  NSLog(@"peerStartupString=%@\n",peerStartupString);
  NSLog(@"peerInitializationString=%@\n",peerInitializationString);
  NSLog(@"peerHelpDir=%@\n",peerHelpDir);
  NSLog(@"peerOptionString=%@\n",peerOptionString);
  NSLog(@"myUnixEnvironment=%@\n",myUnixEnvironment);
}
-(NSMutableDictionary *) getMyUnixEnvironment {
  return myUnixEnvironment;
}
-(enum peer_type) getPeer {
  return peer;
}
-(NSData *)getPeerEndEvaluateMark {
  return peerEndEvaluateMark;
}
-(NSString *)getOpenXM_HOME {
  return OpenXM_HOME;
}
-(NSString *)getPeerStartupString {
  return peerStartupString;
}
-(NSString *)getPeerInitializationString {
  return peerInitializationString;
}
-(NSString *)getPeerOptionString {
  return peerOptionString; 
}
-(NSDictionary *)getMyDefaultTypingAttributes {
  return myDefaultTypingAttributes;
}
+(int) isX11Installed {
  NSFileManager *manager = [NSFileManager defaultManager];
  if ([manager fileExistsAtPath: @"/usr/X11R6/bin/X"]) return 1;
  else return 0;
}
+(int) isGccInstalled {
  NSFileManager *manager = [NSFileManager defaultManager];
  if ([manager fileExistsAtPath: @"/usr/bin/gcc"]) return 1;
  else return 0;
}



@end
