//
//  MyDecoder.h
//  cfep
//
//  Created by Nobuki Takayama on 06/01/29.
//  Copyright 2006 OpenXM.org. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#define OX_PACKET_START '{'
#define OX_PACKET_END   '}'
#define OX_DATA_CMO_DATA_START '<' 
#define OX_DATA_CMO_DATA_END_SIZE ' '
#define OX_DATA_CMO_DATA_END   '>'
#define MYDECODER_MAX  30000

@interface MyDecoder : NSObject {
  int channel;
  int size;
  NSMutableData *data;
  
  int state;
}

-(void) dealloc;
-(NSString *)myDecoder: (int) c from: (id) myDocument;
-(int) getChannel;
-(void) reset;
-(void) reportError: (NSString *)msg to: (id) myDocument;

@end
