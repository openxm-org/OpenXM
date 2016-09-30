//
//  MyEnvironment.h
//  cfep
//
//  Created by Nobuki Takayama on 06/01/22.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
@class MyDocument ;

enum peer_type {
  GENERIC, BASH, OX_TEXMACS
};

@interface MyEnvironment : NSObject {
  NSString *OpenXM_HOME;
  
  enum peer_type peer;              //PEER
  int isXRunning;
  NSData *peerEndEvaluateMark;       //PEER_END_EVALUATE_MARK
  NSString *peerStartupString;      //PEER_STARTUP_STRING


  NSString *peerInitializationString;  //PEER_INITIALIZATION_STRING
  NSString *peerHelpDir ;              //PEER_HELP_DIR
  NSString *peerOptionString ;         //PEER_OPTION_STRING

  NSMutableDictionary *myUnixEnvironment;  // ASIRRC, DISPLAY, OX_XTERM, LC_CTYPE, LC_ALL, LANG
  NSDictionary *myDefaultTypingAttributes;
}

-(MyEnvironment *) init ;
-(MyEnvironment *) initFor: (enum peer_type) myPeerType ;
-(void) dealloc;
+(int) checkX;
+(int) isX11Installed;
+(int) isGccInstalled;
-(void) showForDebug;
-(NSMutableDictionary *) getMyUnixEnvironment;
-(enum peer_type) getPeer;
-(NSData *)getPeerEndEvaluateMark;
-(NSString *) getOpenXM_HOME;
-(NSString *)getPeerStartupString;
-(NSString *)getPeerInitializationString;
-(NSString *)getPeerOptionString;
-(NSDictionary *)getMyDefaultTypingAttributes;


@end
