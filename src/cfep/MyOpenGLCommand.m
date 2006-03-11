//
//  MyOpenGLCommand.m
//  cfep
//
//  Created by Nobuki Takayama on 06/02/18.
//  Copyright 2006 OpenXM.org. All rights reserved.
//

#import "MyOpenGLCommand.h"
#define min(a,b) (a>b?b:a)


@implementation MyOpenGLCommand
-(id)init {
  int i;
  command = nil;
  opCode = -1; 
  endGroup = NO;
  for (i=0; i<4; i++) f4[i] = 0.0;
  for (i=0; i<4; i++) i4[i] = 0;
  return self;
}
-(void) dealloc {
  if (argv) [argv autorelease];
}
-(void)print {
  int i;
  NSLog(@"command=%@s, opCode=%d, [",command,opCode);
  for (i=0; i<4; i++) NSLog(@"%f,",f4[i]);
  NSLog(@"; ");
  for (i=0; i<4; i++) NSLog(@"%d,",i4[i]);
  NSLog(@"]\n"); 
} 
-(NSString *)toString {
  return [NSString stringWithFormat: @"%@[%d], %f, %f, %f, %f; %d, %d, %d, %d",command,opCode,f4[0],f4[1],f4[2],f4[3],i4[0],i4[1],i4[2],i4[3]];
}
+(MyOpenGLCommand *)allocAndCompile: (NSString *)cmd by: (id) sender {
  MyOpenGLCommand *c;
  c=[[MyOpenGLCommand alloc] init];
  [c autorelease];
  return [c compile: cmd by: sender];
}
-(BOOL) isEndGroup {
  return endGroup;
}
-(MyOpenGLCommand *) compile: (NSString *)cmd by: (id) sender {
   // parse cmd and set opCode and f4[].  opCode starts with CFEPgl.
   // Interpreter code is in MyOpenGLView.m with the name execute:
   NSArray *a;
   NSString *s;
   NSString *errmsg;
   id x;
   int fargc, iargc;
   int i,n,ii;
   double f;

   a = [MyUtil arrayOfStringFrom: cmd]; //"[glColor4f,1.0,1.0,0.0,1.0]" -> array of [NSString, MyFloat, MyFloat,..]
   [a retain]; argv = a;
   n = [a count];
   fargc = 0; iargc = 0;
   // for (i=0; i<n; i++) { NSLog(@"%@,",[a objectAtIndex: i]); }
   if (n == 0) return nil;
   x = [a objectAtIndex: 0];
   if ([x isKindOfClass: [NSString class]] == YES) s = x;  // isMemberOfClass does not work. ??
   else {
     NSLog(@"NSString is exected.\n");
	 s = @"";
   }	 
   if ([s compare: @"glBegin"] == NSOrderedSame) {
	opCode = CFEPglBegin;  fargc = 0; iargc = 1;  endGroup = NO;

   }else if ([s compare: @"glColor4f"] == NSOrderedSame) {
	opCode = CFEPglColor4f;  fargc = 4; iargc = 0;  endGroup = NO;

   }else if ([s compare: @"glClear"] == NSOrderedSame) {
	opCode = CFEPglClear;  fargc = 0; iargc = 1;  endGroup = YES;

   }else if ([s compare: @"glClearColor"] == NSOrderedSame) {
	opCode = CFEPglClearColor;  fargc = 4; iargc = 0;  endGroup = YES;

   }else if ([s compare: @"glClearDepth"] == NSOrderedSame) {
	opCode = CFEPglClearDepth;  fargc = 1; iargc = 0;  endGroup = YES;

   }else if ([s compare: @"glDisable"] == NSOrderedSame) {
	opCode = CFEPglDisable;  fargc = 0; iargc = 1;  endGroup = YES;

   }else if ([s compare: @"glEnable"] == NSOrderedSame) {
	opCode = CFEPglEnable;  fargc = 0; iargc = 1;  endGroup = YES;

   }else if ([s compare: @"glEnd"] == NSOrderedSame) {
	opCode = CFEPglEnd;  fargc = 0; iargc = 0;  endGroup = YES;

   }else if ([s compare: @"glFlush"] == NSOrderedSame) {
	opCode = CFEPglFlush;  fargc = 0; iargc = 0;  endGroup = YES;

   }else if ([s compare: @"glLineStipple"] == NSOrderedSame) {
	opCode = CFEPglLineStipple;  fargc = 0; iargc = 2;  endGroup = YES;

   }else if ([s compare: @"glLineWidth"] == NSOrderedSame) {
	opCode = CFEPglLineWidth;  fargc = 1; iargc = 0;  endGroup = YES;

   }else if ([s compare: @"glNormal3f"] == NSOrderedSame) {
	opCode = CFEPglNormal3f;  fargc = 3; iargc = 0;  endGroup = YES;

   }else if ([s compare: @"glPointSize"] == NSOrderedSame) {
	opCode = CFEPglPointSize;    fargc = 1; iargc = 0; endGroup = NO;

   }else if ([s compare: @"glRectf"] == NSOrderedSame) {
	opCode = CFEPglRectf;    fargc = 4; iargc = 0; endGroup = YES;

   }else if ([s compare: @"glVertex3f"] == NSOrderedSame) {
	opCode = CFEPglVertex3f;    fargc = 3; iargc = 0; endGroup = NO;

   }else if ([s compare: @"glVertex2f"] == NSOrderedSame) {
	opCode = CFEPglVertex2f;    fargc = 2; iargc = 0; endGroup = NO;

   }else if ([s compare: @"glib_line"] == NSOrderedSame) {
	opCode = CFEPglib_line;    fargc = 4; iargc = 1; endGroup = YES;

   }else if ([s compare: @"glib_putpixel"] == NSOrderedSame) {
	opCode = CFEPglib_putpixel;    fargc = 2; iargc = 1; endGroup = YES;

   }else if ([s compare: @"glib_flush"] == NSOrderedSame) {
	opCode = CFEPglib_flush;    fargc = 0; iargc = 0;   endGroup = YES;

   }else if ([s compare: @"glib3_bounding_box"] == NSOrderedSame) {
	opCode = CFEPglib3_bounding_box;    fargc = 1; iargc = 0;   endGroup = YES;

   }else if ([s compare: @"glib3_ray_init"] == NSOrderedSame) {
	opCode = CFEPglib3_ray_init;    fargc = 0; iargc = 0;   endGroup = YES;

   }else if ([s compare: @"glib3_ray_reshape"] == NSOrderedSame) {
	opCode = CFEPglib3_ray_reshape;    fargc = 0; iargc = 0;   endGroup = YES;

   }else if ([s compare: @"glib3_ray"] == NSOrderedSame) {
	opCode = CFEPglib3_ray;    fargc = 0; iargc = 0;   endGroup = YES;
	[sender output: @"Ray by David Bucciarelli. (MesaDemo, GPL)"];

   }else if ([s compare: @"glib3_std_scene0"] == NSOrderedSame) {
	opCode = CFEPglib3_std_scene0;    fargc = 0; iargc = 0;   endGroup = YES;
	[sender output: @"glib3_std_scene0(0,0,2) using glFrustum"];

   }else if ([s compare: @"glib3_icosahedron"] == NSOrderedSame) {
	opCode = CFEPglib3_icosahedron;    fargc = 0; iargc = 0;   endGroup = YES;
	[sender output: @"Icosahedron"];

   }else {
    NSLog(@"Undefined command=<%@>\n",s);
	errmsg = [NSString stringWithFormat: @"Undefined command=<%@>",s];
	[sender output: errmsg];
    return nil;
   }
   // Format glxxxpfqi  fargc=p, iargc=q.  Example: glxxx4f1i, 0.1,0.2,0.3,0.4,34
   command = s;
   [command retain]; // bug. How to release?
   if (fargc > 0) {
	for (i=1; i< min(n,fargc+1); i++) {
	    x = [a objectAtIndex: i];
        if ([x isMemberOfClass: [MyFloat class]] == YES) f = [x getFValue];
		else {
		  NSLog(@"float is exected.\n");
		  f = 0.0;
		}  
		if (i-1 < 4) f4[i-1] = f;
	}
   }
   if (iargc > 0) {
	for (i=1+fargc; i< min(n,fargc+iargc+1); i++) {
	    x = [a objectAtIndex: i];
        if ([x isMemberOfClass: [MyInt class]] == YES) ii = [x getIValue];
		else {
		  ii = 0;
		  NSLog(@"int is exected.\n");
		}		  
		if (i-1-fargc < 4) i4[i-1-fargc] = ii;
	}
   }
   return self;
}

-(int) getOpCode { return opCode; }
-(double *)getF4 { return f4; }
-(int *) getI4 { return i4; }
-(NSArray *)getArgv { return argv; }

@end

/* Code to test MyFloat put in compile:
   MyFloat *x;
   NSLog(@"member test.\n"
   x = [MyFloat allocWith: 1.0];
   if ([x isMemberOfClass: [MyFloat class]] == YES) NSLog(@"[x class] is MyFloat\n");
   if ([x isMemberOfClass: [MyInt class]] == YES) NSLog(@"[x class] is MyInt\n");
*/
@implementation MyFloat 
+(MyFloat *)allocWith: (float) f {
   MyFloat *ff;
   ff=[MyFloat alloc];
   [ff setFValue: f];
   return ff;
}
-(void) setFValue: (float) f { fValue = f; }
-(float) getFValue { return fValue; }
-(NSString *)description { return [NSString stringWithFormat: @"(float)%f",fValue]; }
@end

@implementation MyInt
+(MyInt *)allocWith: (int) i {
  MyInt *ii;
  ii=[MyInt alloc];
  [ii setIValue: i];
  return ii;
}
-(void) setIValue: (int) i { iValue = i; }
-(int) getIValue { return iValue; }
-(NSString *)description { return [NSString stringWithFormat: @"(int)%d",iValue]; }
@end

#define SMAX  0x1000   //4096*16
@implementation MyUtil
static int debugMyUtil = 0;
+(void) setDebugMyUtil {
  if (debugMyUtil) debugMyUtil = 0;
  else debugMyUtil = 1;
}
+(NSMutableArray *) arrayOfStringFrom: (NSString *) args {
  int n,i,k,type;
  unichar c;
  unichar s[SMAX];
  NSMutableArray *a; // for debug
  NSMutableArray *aa;
  NSString *ss;
  int status,level;
  status = -1; level = 0; type = 0;
  n = [args length];
  if (n >= SMAX-1) {
    NSLog(@"Too big string for stringToArrayOfString.\n");
    return nil;
  }	
  a = [NSMutableArray arrayWithCapacity: 1];
  aa = [NSMutableArray arrayWithCapacity: 1];
  for (i=0, k=0; i<n; i++) {
    c = [args characterAtIndex: i];
	if (level == 0) {
		switch (status) {  // See my note on 2006-02-20.
		case -1:
			if ( c == '[') { k=0; status=0;}
			break;
		case 0:
			if ( c == '[') {
				k = 0; s[k] = c; k++; status = 1; type = 1; // type 1 : list
			} else if (c == ',') {	
			} else if (c == '(') {
			   k=0; status = 21; type=0;  //ex. (int) 
		    } else if ((('A' <= c) && (c <= 'Z')) || (('a' <= c) && (c <= 'z'))) {
			   k=0; status = 2; type=3; s[k] = c; k++;      // type -1 : string.  
			} else if (c > ' ') {
				k=0; status = 2; type=0; s[k] = c; k++;     // type 0 : float
			} else if (c == ']') status = -1;
			break;
		case 2:
			if ((c == ',') || (c == ']')) {
			   if (c == ',') status = 0; else status = -1;
               if (k > 0) ss = [NSString stringWithCharacters: s length: k];
			   else ss = @"";
			   [a addObject: ss];
			   if (type == 2) [aa addObject: [MyInt allocWith: [ss intValue]]];
			   else if (type == 0) [aa addObject: [MyFloat allocWith: [ss floatValue]]];
			   else [aa addObject: ss];
			}else{
				s[k] = c; k++;
			}
			break;
		case 1:
			if (c == '[') {
			   s[k] = c; k++;
			   level++;
			}else if (c == ']') {
			   s[k] = ']'; k++;
			   if (k > 0) ss = [NSString stringWithCharacters: s length: k];
			   else ss = @"";
			   [a addObject: ss];
			   [aa addObject: [MyUtil arrayOfStringFrom: ss]];
			   status = 0;
			}else {
			   s[k] = c; k++;
			} 
			break;
		case 21:
            if (c == ')') {
			  if ((s[0] == 'i') && (s[1] == 'n') && (s[2] == 't')) type=2;
			  else type = 0;
			  status = 2; k = 0; // starting to read data.
			}else if (c > ' ') {
			  s[k] = c; k++;
			}
			break;  
		}     
	}else if (level > 0) {
	    s[k] = c; k++;
		if (c == '[') level++;
		else if (c == ']') level--;
	}	
  }
  if (debugMyUtil) {
    NSLog(@"a=%@\n",a);
    NSLog(@"aa=%@\n",aa);
  }	
  return aa;
}

/*
   NSLog(@"mytest=%@\n",[MyUtil arrayOfStringFrom: @"[]"]);
   NSLog(@"mytest=%@\n",[MyUtil arrayOfStringFrom: @"[1,[]]"]);
   NSLog(@"mytest=%@\n",[MyUtil arrayOfStringFrom: @"[1,2,[3,4],5]"]);
   NSLog(@"mytest=%@\n",[MyUtil arrayOfStringFrom: @" [1, 2,3, 4,5]"]);
   NSLog(@"mytest=%@\n",[MyUtil arrayOfStringFrom: @"[[1],2,[[3,[4]]],5]"]); // There is a bug. Output is 1,2,...
*/

+(NSString *)pruneThings: (NSString *)ss {
  int n,i, start,end;
  unichar c;
  unichar s[SMAX];
  NSString *ans;
  n = [ss length];
  if (n >= SMAX-1) {
    NSLog(@"Too big string for pruneThings.\n");
    return nil;
  }	
  start = 0; end = n-1;
  for (i=0; i<n; i++) {
    c = [ss characterAtIndex: i];
	start = i;
	if (c > 0x20) break;
  }
  for (i=n-1; i>=0; i--) {
    c = [ss characterAtIndex: i];
    end = i;
	if (c > 0x20) break;
  }
  if (start > end) return nil;
  for (i=0; i<= end-start ;  i++) {
    s[i] = [ss characterAtIndex: (start+i)];
	s[i+1] =0;
  }
  ans = [NSString stringWithCharacters: s length: (end-start+1)];
  return ans;
}

+(id)attributedStringWithPath: (NSString *)path {
   NSFileWrapper *theWrapper;
   NSTextAttachment *theAttachment;
   NSAttributedString *aStr;
   theWrapper = [[NSFileWrapper alloc] initWithPath:  path];
   if (!theWrapper) NSLog(@"theWrapper is nil. Path=[%@]\n",path);
   if (!theWrapper) return nil;
   theAttachment = [[NSTextAttachment alloc] initWithFileWrapper:theWrapper];
   aStr = [NSAttributedString attributedStringWithAttachment:theAttachment];
   return aStr;   // How should I do autorelease?
}


@end