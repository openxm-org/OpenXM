/* $OpenXM: OpenXM/src/ox_toolkit/glib.c,v 1.3 2000/01/18 12:24:18 takayama Exp $ */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

#ifdef TEST
int main() 
{
    int k;
    gopen();
    for (k=10; k<=500; k++) {
        setpixel(k,k);
    }
    moveto(100,50);
    lineto(400,50);
    for (k=10; k<=300; k += 20) {
        lineto(k,2*k);
    }
    gFlush();
    gclose();
    getchar();
}
  
#endif

/* -------------------- main code of glib.c ---------------- */
#define XSIZE 640
#define YSIZE 400

/* global variables */
Display *glib_d;
Window glib_w;
GC glib_gc;
XSetWindowAttributes glib_a;

static int glib_x = 0;
static int glib_y = 0;

/* functions exported */
int gopen() 
{
    glib_d = XOpenDisplay(NULL);
    glib_w = XCreateSimpleWindow(glib_d,RootWindow(glib_d,0),100,100,
                                 XSIZE,YSIZE,2,0,BlackPixel(glib_d,0));
    glib_a.override_redirect = 1;
    XChangeWindowAttributes(glib_d,glib_w,CWOverrideRedirect, &glib_a);
    XMapWindow(glib_d,glib_w);
    glib_gc = XCreateGC(glib_d,glib_w,0,0);

    XSetBackground(glib_d, glib_gc, BlackPixel(glib_d, 0));
    XSetForeground(glib_d, glib_gc, WhitePixel(glib_d, 0));

    /* initialize line attributes */
    XSetLineAttributes(glib_d,glib_gc,1,LineSolid,CapButt,JoinMiter);
}

int gclose() 
{
    XFlush(glib_d);
}

int gFlush() 
{
    XFlush(glib_d);
}


int moveto(int x, int y)
{
    glib_x = x;
    glib_y = y;
}

int lineto(int x, int y)
{
    XDrawLine(glib_d,glib_w,glib_gc,glib_x,glib_y,x,y);
    glib_x = x; glib_y = y;
}

int setpixel(int x, int y)
{
    XDrawPoint(glib_d,glib_w,glib_gc,x,y);
}

