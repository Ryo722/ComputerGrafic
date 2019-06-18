#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#define H 320
#define W 320
#define N  H*W

main()
{
  Display *d;
  Window w;
  Pixmap pm;
  GC gc;
  unsigned long white,black;
  XEvent event;
  int i,j,n;
  XPoint points[N];
  int ifstart[N];
  int err;
  int pixcel[H][W]={0};

  d=XOpenDisplay(NULL);
  white=WhitePixel(d,DefaultScreen(d));
  black=BlackPixel(d,DefaultScreen(d));

  w = XCreateSimpleWindow(d,DefaultRootWindow(d),50,100,H,W,1,BlackPixel(d,DefaultScreen(d)),WhitePixel(d,DefaultScreen(d)));
  XSelectInput(d,w,ExposureMask|ButtonPressMask|ButtonMotionMask|KeyPressMask);

  gc = XCreateGC(d,w,0,0);
  pm = XCreatePixmap(d,w,H,W,DefaultDepth(d,DefaultScreen(d)));

  XSetLineAttributes(d,gc,5,LineSolid, CapRound,JoinRound);
  XSetForeground( d, gc, BlackPixel(d,DefaultScreen(d)) );
  XSetBackground(d, gc, WhitePixel(d,DefaultScreen(d)));
  
  err = XpmReadFileToPixmap(d,w,"pixmadp.xpm",&pm,NULL,NULL);
  XSetWindowBackgroundPixmap(d,w,pm);

  
  XMapWindow(d,w);
  XFlush(d);
  
  n=0;
  while (1) {
    XNextEvent(d,&event);
    switch(event.type) {
    case Expose:
      
      for(i=1;i<n;i++)if(ifstart[i] != 1)
			XDrawLine(d,w,gc,points[i-1].x,points[i-1].y,points[i].x,points[i].y);
      
      break;
      
    case ButtonPress:
      
      ifstart[n] = 1;
      points[n].x = event.xbutton.x;
      points[n].y = event.xbutton.y;
      pixcel[event.xbutton.x][event.xbutton.y] = 1;
      n++;
      break;
      
    case MotionNotify :
      ifstart[n] = 0;
      points[n].x = event.xmotion.x;
      points[n].y = event.xmotion.y;
      pixcel[event.xbutton.x][event.xbutton.y] = 1;
      XDrawLine(d,pm,gc,points[n-1].x,points[n-1].y,points[n].x,points[n].y);
      XCopyArea(d,pm,w,gc,0,0,H,W,0,0);
      XFlush(d);
      n++;
      
      break;
    case KeyPress : 
      err = XpmWriteFileFromPixmap(d,"pixmap.xpm",pm,0,NULL);
      exit(0);
    }
  }
}
