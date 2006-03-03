//
//  MyDecoder.m
//  cfep
//
//  Created by Nobuki Takayama on 06/01/29.
//  Copyright 2006 OpenXM.org. All rights reserved.
//

#import "MyDecoder.h"


@implementation MyDecoder
-(void) dealloc {
  if (data) {[data autorelease];  data=nil;}
  [super dealloc];
}

-(NSString *)myDecoder: (int) c from: (id) myDocument {
  Byte b[1];
  NSString *s;
  // NSLog(@"state=%d\n",state);
  @synchronized(self) {
    switch(state) {
	case 0:
	  if (c == OX_PACKET_START)  {state = 1; channel=0;}
	  break;
    case 1:
	  if (c == OX_DATA_CMO_DATA_START) {
	    size = 0; state = 2;
	  }else if ((c < '0') || (c > '9')) { 
	    [self reportError: @"myDecoder: invalid channel.\n" to: myDocument];
		[self reset]; return nil;
	  } else { channel = channel*10+c-'0'; }
	  break;
	case 2:
	  if (c == OX_DATA_CMO_DATA_END_SIZE) {
	    state = 3;  
		if (size > MYDECODER_MAX) { 
		  [self reportError: @"myDecoder: too big size.\n" to: myDocument];
		  [self reset];
		  return nil;
		}  
		data = [NSMutableData dataWithCapacity: (size+1)]; [data retain];
	  }else if ((c < '0') || (c > '9')) { 
	    [self reportError: @"myDecoder: invalid size.\n" to: myDocument];
		[self reset]; return nil;
	  } else { size = size*10+c-'0'; }
	  break;
	case 3:
	  if (size <= 0) { 
	    state = 4;
		if (c != OX_DATA_CMO_DATA_END) {
		  [self reportError: @"myDecoder: unexpected end of ox_data_cmo_data.\n" to: myDocument];
		  [self reset];
		  return nil;
		} 
	  }	else {
        b[0] = c; 
	    [data appendBytes: b length: 1];
	    size--;
	  }	
	  break;
	case 4:
	  if (c != OX_PACKET_END) {
		  [self reportError: @"myDecoder: unexpected end of ox_packet.\n" to: myDocument];
		  [self reset];
		  return nil;
	  }else{
	    b[0] = 0; [data appendBytes: b length: 1];
	    s = [NSString stringWithCString: [data bytes] encoding: NSUTF8StringEncoding];
		[self reset];
		return s;
	  }
	  break;
	default:
	  [self reportError: @"myDecoder: unknown state.\n" to: myDocument]; [self reset]; 
	  break;	  
	}		
  }
  return nil;
}

-(int) getChannel {
  return channel;
}

-(void) reset {
  @synchronized(self) {
	state = 0;
	// NSLog(@"data retainCount=%d\n",[data retainCount]);   // --> 2.
	if (data) { [data autorelease]; data=nil;}
  }
}

-(void) reportError: (NSString *) msg to: (id) myDocument {
   [myDocument outputErrorString: msg];
   NSLog(@"data=%@",data);
}


@end
