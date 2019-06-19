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
  if(check<0){
    n++;
  }
  return n;
}
