

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

void sort(int ary[2]){
  int tmp;
  
  if(ary[0]>ary[1]){
    tmp=ary[0];
    ary[0]=ary[1];
    ary[1]=tmp;
  }
}

void labeling(XImage *image, int pixel[W][H]){
  int i,j,k,l,o;
  int check, mode;
  int px=2;//4kinbo
  int label=0;
  int lbp[4]={0};
  int label_lib[W*H];
  
  for(i=0;i<W;i++){
    for(j=0;j<H;j++){
      if((XGetPixel(image,i,j) == 0x000000) && pixel[i][j]==0){//byougasitetarea
	//kinbouno label miru
	lbp[0]=pixel[i-1][j];//1tu ue no
	lbp[1]=pixel[i][j-1];//1tu left
	
	sort(lbp);//label no atai sort
	mode=0;
	check=lbp[0];//kioku sita label tigau ka siraberu
	for(k=1;k<px;k++){
	  if(n!=lbp[k]){
	    mode++;//label tigau kazu ni yotte kinou change
	    check=lbp[k];
	  }
	}

	switch(mode){
	case 0://first black
	  if(lbp[0] == 0){//kinbou ni no label
	    label++;
	    label_lib[label]=label;
	    pixel[i][j]=label;
	  }else{//utigawa no ten
	    pixel[i][j]=lbp[0];
	  }
	  break;
	case 1://kinbou 1tu tigau
	  pixel[i][j]=lbp[px-1];
	  break;
	  /*
	case 2://kinbou 2tu tigau
	  for(k=0;k<px;k++){
	    if(lbp[k]!=0){
	      pixel[i][j]=lbp[k];
	      break;
	    }
	    for(l=0;l<label;l++){
	      for(o=k+1;o<px;o++){
		if(lbp[o]==label_lib[l]){
label_lib[o]=
	  */

	}
      }	
    }
  }
  for(i=0;i<W;i++){
    for(j=0;j<H;j++){
      pixel[i][j]=
	}			
