#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#define H 32
#define W 32
#define N  H*W

int labeling(XImage *image, int i, int j,int n, int pixel[W][H]){
  if(i>0 && j>0 && pixel[i-1][j] == 0 && pixel[i-1][j-1] == 0 && pixel[i-1][j+1] == 0 && pixel[i][j-1] == 0){
    n++;
    if(n>10){
      printf("I can't prepare color enough\n");
    }
    pixel[i][j]=n;
  }


  pixel[i][j]=n;
  if(i>0){
    if(pixel[i-1][j]>0){
      pixel[i][j]=pixel[i-1][j];
      if(pixel[i-1][j]==0){
	labeling(image,i-1,j,n,pixel);
      }
    }
  }
  if(j>0){
    if(pixel[i][j-1]>0){
      pixel[i][j]=pixel[i][j-1];
      if(pixel[i][j-1]==0){
	labeling(image,i,j-1,n,pixel);
      }
    }
  }
  if(pixel[i][j+1]>0 && j<W-1){
    pixel[i][j]=pixel[i][j+1];
    if(pixel[i][j+1]==0){
      labeling(image,i,j+1,n,pixel);
    }
  }
  if(pixel[i+1][j]>0 && i<H-1){
    pixel[i][j]=pixel[i+1][j];
    if(pixel[i+1][j]==0){
      labeling(image,i+1,j,n,pixel);
    }
  }
  return n;
}

int main()
{
  Display *d;
  Window w;
  Pixmap pm;
  GC gc;
  unsigned long white,black;
  XEvent event;
  int i,j,k,n,label=0;
  XPoint points[N];
  XImage *image;
  int ifstart[N];
  int err;
  int pixel[W][H]={0};
  char key[8];
  char name[50];
  
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
      XLookupString((XKeyEvent*)&event,key,sizeof(key),NULL,NULL);//
      if(key[0] == 's'){
	printf("save file name:");
	scanf("%s",name);
	XCopyArea(d,w,pm,gc,0,0,H,W,0,0);
	err = XpmWriteFileFromPixmap(d,name,pm,0,NULL);
      }else if(key[0] == 'l'){
	printf("load file name:\n");
	scanf("%s",name);
	err = XpmReadFileToPixmap(d,w,name,&pm,NULL,NULL);
	XCopyArea(d,pm,w,gc,0,0,H,W,0,0);
	XFlush(d);
      }else if(key[0] == 'i'){
	printf("be array file name:\n");

	image = XGetImage(d,w,0,0,W,H,AllPlanes,XYPixmap);
	for(i=0;i<W;i++){
		for(j=0;j<H;j++){
			if((XGetPixel(image,i,j) == 0x000000) && pixel[i][j]==0){	
				label=labeling(image,i,j,label,pixel);
			}else if(XGetPixel(image,i,j) == 0xFFFFFF){
				pixel[i][j]=0;
			}
		}
	}
			

	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			printf("%2d",pixel[i][j]);
		        switch(pixel[i][j]){
			case 1:
			  XPutPixel(image,i,j,0xFF0000);
			  break;
			case 2:
			  XPutPixel(image,i,j,0x00FF00);
			  break;
			case 3:
			  XPutPixel(image,i,j,0x0000FF);
			  break;
			case 4:
			  XPutPixel(image,i,j,0x000000);
			  break;
			case 5:
			  XPutPixel(image,i,j,0xFFFF00);
			  break;
			case 6:
			  XPutPixel(image,i,j,0xFF00FF);
			  break; 
			case 7:
			  XPutPixel(image,i,j,0x00FFFF);
			  break;
			case 8:
			  XPutPixel(image,i,j,0xF0F0F0);
			  break;
			case 9:
			  XPutPixel(image,i,j,0xFFF000);
			  break;
			case 10:
			  XPutPixel(image,i,j,0x000FFF);
			  break;
			}	
		}
		printf("\n");
	}
	XPutImage(d,w,gc,image,0,0,0,0,W,H);

	printf("\n");
      }else{
	exit(0);
      }
    }
  }
}
