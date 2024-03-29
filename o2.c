#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#define Kinbo 4
#define H 32
#define W 32
#define N  H*W

void Ssort(int ary[4],int len){
  int i,j,min,tmp;
  int count=0;
  
  for(i=0;i<len;i++){
    min=i;
    for(j=i+1;j<len;j++){
      if(ary[j]<ary[min]){
	min=j;
      }
    }

    if(min!=i){
      tmp=ary[i];
      ary[i]=ary[min];
      ary[min]=tmp;
    }
  }
}

void labeling(XImage *image, int pixel[W][H]){
  int i,j,k,l,n,o;
  int check, mode;
  int px=2;//4kinbo
  int label=0;
  int lbp[Kinbo]={0};
  int label_lib[11];
int a;

  
  for(i=0;i<H;i++){
    for(j=0;j<W;j++){
      if(XGetPixel(image,i,j) == 0x000000){//byougasitetarea
	
	//kinbouno label miru
	if(i>0){
	  lbp[0]=pixel[i-1][j];//1tu ue 
	}
	if(j>0){
	  lbp[1]=pixel[i][j-1];//1tu left
	}
	if(i<H-1){
	  lbp[2]=pixel[i+1][j];
	}
	if(j<W-1){
	  lbp[3]=pixel[i][j+1];
	}

	Ssort(lbp,Kinbo);//label no atai sort
	
	pixel[i][j]=lbp[Kinbo];

	if(i>0 && j>0 && pixel[i-1][j] == 0 && pixel[i-1][j-1] == 0 && pixel[i-1][j+1] == 0 && pixel[i][j-1] == 0){//mawari ni label nai
	  label++;
	  label_lib[label]=label;
	  pixel[i][j]=label;
	}
      }
    }
  }

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
	printf("Save file name:");
	scanf("%s",name);
	XCopyArea(d,w,pm,gc,0,0,H,W,0,0);
	err = XpmWriteFileFromPixmap(d,name,pm,0,NULL);
      }else if(key[0] == 'l'){
	printf("Load file name:");
	scanf("%s",name);
	err = XpmReadFileToPixmap(d,w,name,&pm,NULL,NULL);
	XCopyArea(d,pm,w,gc,0,0,H,W,0,0);
	XFlush(d);
      }else if(key[0] == 'r'){
	printf("Labeling\n");
	image = XGetImage(d,w,0,0,W,H,AllPlanes,XYPixmap);
	labeling(image,pixel);

	for(i=0;i<H;i++){
		for(j=0;j<W;j++){
			printf("%2d",pixel[i][j]);
			if(pixel[i][j]!=0){
				XPutPixel(image,i,j,0xFF0000);
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
