#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#define H 32
#define W 32
#define N  H*W

main()
{
  Display *d;
  Window w;
  Pixmap pm;
  GC gc;
  unsigned long white,black;
  XEvent event;
  int i,j,k,n;
  XPoint points[N];
  int ifstart[N];
  int err;
  int pixel[H][W];
  char key[8];
  char name[50];
  int c;
  char tmp[H+4];
  FILE *fp;
  int countn;
  char cha[2];
  
  d=XOpenDisplay(NULL);
  white=WhitePixel(d,DefaultScreen(d));
  black=BlackPixel(d,DefaultScreen(d));

  w = XCreateSimpleWindow(d,DefaultRootWindow(d),50,100,H,W,1,BlackPixel(d,DefaultScreen(d)),WhitePixel(d,DefaultScreen(d)));
  XSelectInput(d,w,ExposureMask|ButtonPressMask|ButtonMotionMask|KeyPressMask);
  XMapWindow(d,w);

  gc = XCreateGC(d,w,0,0);
  pm = XCreatePixmap(d,w,H,W,DefaultDepth(d,DefaultScreen(d)));

  XSetLineAttributes(d,gc,5,LineSolid, CapRound,JoinRound);
  XSetForeground( d, gc, BlackPixel(d,DefaultScreen(d)) );
  
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
      n++;
      break;
      
    case MotionNotify :
      ifstart[n] = 0;
      points[n].x = event.xmotion.x;
      points[n].y = event.xmotion.y;
      XDrawLine(d,w,gc,points[n-1].x,points[n-1].y,points[n].x,points[n].y);
      XCopyArea(d,w,pm,gc,0,0,H,W,0,0);
      XFlush(d);
      n++;
      
      break;
    case KeyPress : 
      XLookupString((XKeyEvent*)&event,key,sizeof(key),NULL,NULL);
      if(key[0] == 's'){
	printf("save file name:");
	scanf("%s",&name);
	XCopyArea(d,w,pm,gc,0,0,H,W,0,0);
	err = XpmWriteFileFromPixmap(d,name,pm,0,NULL);
      }else if(key[0] == 'l'){
	printf("load file name:");
	scanf("%s",&name);
	err = XpmReadFileToPixmap(d,w,name,&pm,NULL,NULL);
	XCopyArea(d,pm,w,gc,0,0,H,W,0,0);
	XFlush(d);
      }else if(key[0] == 'i'){
	printf("be array file name:");
	scanf("%s",&name);
	fp = fopen(name,"r");
	if(fp == NULL){
	  printf("file nothing\n");
	  return -1;
	}
	for(i=0;i<3;i++){
	  fgets(tmp,H+4,fp);
	}

	/*
	countn=0;
	while((c == fgetc(fp)) != EOF){
	  snprintf(cha,sizeof(c),"%d",c);
	  if(cha[0] != '\n'){
	    k++;
	  }else if(cha[0] == '\n'){
	    countn++;
	    if(countn==5){
	      break;
	    }
	  }
	}
	*/
	i=0;
	j=0;
	

	while((c = fgetc(fp)) != EOF){
	  //putchar(c);
	  if(c ==  ' '){
	    pixel[i][j]=0;
	  }else if(c == '.'){
	    pixel[i][j]=1;
	  }
	  printf("%d",pixel[i][j]);
	  j++;
	  if(j==W){
	    printf("\n");
	    i++;
	    j=0;
	    fseek(fp,4,SEEK_CUR);
	  }
	}
	printf("\n");
      }else{
	exit(0);
      }
    }
  }
}
