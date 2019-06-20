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
	
	
	
    
int labeling(XImage *image, int i, int j, int n, int pixel[W][H]){
  if(i<W-1 && pixel[i+1][j]>0){
    pixel[i][j]=labeling(image,i+1,j,n,pixel);
  }else if (j<H-1 && pixel[i][j+1]>0){
    pixel[i][j+1]=labeling(image,i,j+1,n,pixel);
  }else if (i>0 && pixel[i-1][j]>0){
    pixel[i-1][j]=labeling(image,i-1,j,n,pixel);
  }else if (j>0 && pixel[i][j-1]>0){
    pixel[i][j-1]=labeling(image,i,j-1,n,pixel);
  }else{
    n++;
    pixel[i][j]=n;
  }
  return pixel[i][j];
}



int labeling(XImage *image, int i, int j,int n, int pixel[W][H]){
  int check=0;

  pixel[i][j]=n;
  if(i>0){
    if(pixel[i-1][j]>0){
      pixel[i][j]=pixel[i-1][j];
      if(pixel[i-1][j]==0){
	labeling(image,i-1,j,n,pixel);
	check++;
      }
    }
  }
  if(j>0){
    if(pixel[i][j-1]>0){
      pixel[i][j]=pixel[i][j-1];
      if(pixel[i][j-1]==0){
	labeling(image,i,j-1,n,pixel);
	check++;
      }
    }
  }
  if(pixel[i][j+1]>0 && j<W-1){
    pixel[i][j]=pixel[i][j+1];
    if(pixel[i][j+1]==0){
      labeling(image,i,j+1,n,pixel);
      check++;
    }
  }
  if(pixel[i+1][j]>0 && i<H-1){
    pixel[i][j]=pixel[i+1][j];
    if(pixel[i+1][j]==0){
      labeling(image,i+1,j,n,pixel);
      check++;
    }
  }
  if(check==0){
    n++;
  }
  return n;
}

void labeling(XImage *image, int pixel[W][H]){
	int i,j;
	int label=0;
	
	
	for(i=0;i<W;i++){
		for(j=0;j<H;j++){
			if((XGetPixel(image,i,j) == 0x000000) && pixel[i][j]==0){
				if(i<W-1 && pixel[i+1][j]>0){
					pixel[i][j]=pixel[i+1][j];
				}else if (j<H-1 && pixel[i][j+1]>0){
					pixel[i][j+1]=labeling(image,i,j+1,n,pixel);
				}else if (i>0 && pixel[i-1][j]>0){
					pixel[i-1][j]=labeling(image,i-1,j,n,pixel);
				}else if (j>0 && pixel[i][j-1]>0){
					pixel[i][j-1]=labeling(image,i,j-1,n,pixel);
				}else{
					label++;
					pixel[i][j]=label;
				}
			}
		}	
	}
}			