#include <stdio.h>
#include <iostream>
#include "windows.h"
#include "math.h"

int width,height,depth,size,allsize;

void readraw1(char filename2[])
{
	
	FILE *f1;
	f1=fopen(filename2,"rb");
	if(f1 != NULL)
	{  	
		fscanf(f1, "%d", &width);
		fscanf(f1, "%d", &height);
		fscanf(f1, "%d", &depth);
		size = width*height; 
		allsize=width*height*depth;
		fclose(f1); 
	}  
	else  
	{  
		printf("dim file open error!\n");
		return ;
	}
}

void readraw2(char filename1[], unsigned char output_imagedata[])
{
	
	FILE *f1;
	f1=fopen(filename1,"rb");
	if(f1 != NULL)
	{  	
		for(int i=0;i<allsize;i++)  
		{  		
			*(output_imagedata+i)=fgetc(f1); 		
		} 
		fclose(f1); 
	}  
	else  
	{  
		printf("data file open error!\n");
		return ;
	}

}

void CalculateThreshold(unsigned char *imSUB,int size_x,int size_y)
{
	register int   i, j, k;
	int            ngray, iTHD, x1, x2, y1, y2;
	unsigned char  vimg;
	long           NumberOfPixels;
	double         *histogram;
	double         rm = 0.0, va = 0.0, rh, rmean;
	double         vari, p, a, s, d, amax, pmax, sep, smax = 0.0;
	double         p0, p1, a0, a1;

	ngray = 256;
	x1=y1=0;
	x2=size_x-1;
	y2=size_y-1;

	NumberOfPixels = size_x * size_y;

	histogram =  new double[256];
	if(histogram == NULL)
	{  
		 return ;
	}

	for(i = 0; i < 256; i++) histogram[i] = 0.0;
	for(j = y1; j <= y2; j++)
	{
		for(i = x1; i <= x2; i++)
		{
			vimg =  imSUB[j*size_x+i];		
			histogram[vimg]++;
		}
	}
	for(i=0; i< 256; i++)  histogram[i] /= NumberOfPixels;
	for(i = 0; i < ngray; i++)
	{
		rh = (double)i * histogram[i];
		rm += rh;      
		va += (double)i * rh;
	}
	rmean = rm;
	vari = va - rm * rm;
	p = 0.0;   a = 0.0;
	for(i = 0; i < ngray -1; i++)
	{
		rh = histogram[i];
		p += rh;
		a += (double)i * rh;
		s = rm * p -a;
		d = p * (1.0 - p);
		if(d < 0.00000000000001) continue;
		s = s * s / d;
		if(s > smax)  {  smax = s;  amax = p; pmax = p; k = i;  }
	}
	p0 = pmax; p1 = 1.0- p0;
	a0 = amax / pmax; a1 = (rm - amax) / p1;
	sep = smax / vari;
	iTHD = k;

	delete histogram;


	for(j = y1; j <= y2; j++)	
	{
		for(i = x1; i <= x2; i++)
		{
			vimg = imSUB[j*size_x+i];

			if(vimg <= iTHD)
				imSUB[j*size_x+i]=0;
			else imSUB[j*size_x+i]=255;
		}
	}
	return ;
}

bool Erosion(unsigned char *imBits,int width,int height)
{
	int x,y,x1,y1,count;

	int imW = width;
	int	imH = height;
	int nbWidth=imW;
//	if (nbWidth%4) nbWidth+=4-(nbWidth%4);
	unsigned char *lpMarks=new unsigned char [nbWidth*imH];


	//腐蚀
	for(y=1;y<imH-1;y++)
	{
		for(x=1;x<imW-1;x++)
		{
			count=1;
			for(y1=-1;y1<2;y1++)
			{
				for(x1=-1;x1<2;x1++)
				{
					if(imBits[x+x1+(y+y1)*nbWidth])
						count=0;
				}
			}
			if(count) lpMarks[x+y*nbWidth]=1;
			else lpMarks[x+y*nbWidth]=0;
		}
	}

	for(y=1;y<imH-1;y++)
	{
		for(x=1;x<imW-1;x++)
		{
			//目标图像中含有0和255外的其它灰度值
			if(imBits[x+y*nbWidth] != 255 && imBits[x+y*nbWidth] != 0)
			{
				printf("目标图像中含有0和255外的其它灰度值\n");
				delete lpMarks;
				return 0;
			}
			if(lpMarks[x+y*nbWidth]==0) imBits[x+y*nbWidth]=255;

		}

	}
	delete lpMarks;

	return 1;
}

bool Dilation(unsigned char *imBits,int width,int height)
{
	int x,y,x1,y1,count;
	int imW = width;
	int	imH = height;
	int nbWidth=imW;
//	if (nbWidth%4) nbWidth+=4-(nbWidth%4);
	unsigned char *lpMarks=new unsigned char [nbWidth*imH];


	//膨胀
	for(y=1;y<imH-1;y++)
	{
		for(x=1;x<imW-1;x++)
		{
			count=0;
			for(y1=-1;y1<2;y1++)
			{
				for(x1=-1;x1<2;x1++)
				{
					if(imBits[x+x1+(y+y1)*nbWidth]==0)
						count=1;
				}
			}
			if(count) lpMarks[x+y*nbWidth]=1;
			else lpMarks[x+y*nbWidth]=0;
		}
	}

	for(y=1;y<imH-1;y++)
	{
		for(x=1;x<imW-1;x++)
		{
			//目标图像中含有0和255外的其它灰度值
			if(imBits[x+y*nbWidth] != 255 && imBits[x+y*nbWidth] != 0)
			{
				printf("目标图像中含有0和255外的其它灰度值");
				delete lpMarks;
				return 0;
			}  
			if(lpMarks[x+y*nbWidth]) imBits[x+y*nbWidth]=0;  

		}

	}

	delete lpMarks;

	return 1;
}

 
int test(unsigned char *buffer,int x,int y,int z,int width, int height,int depth)
{
	int i,i1,j,j1,k,k1,m,n,l;
	unsigned char *visited,*aim,*lp;
    size=width*height;
	allsize=depth*width*height;
	bool m_b;
	unsigned char m_bColor;


	i=0; 
	j=0;

	for (m=-1;m<2; m++)
	{
		for (n=-1; n<2; n++)
		{
			for (l=-1; l<2; l++)
			{
				k1=z+m;
				j1=y+n;
				i1=x+l;
				if(k1<0 || k1>depth-1) continue;
				if(j1<0 || j1>height-1) continue;
				if(i1<0 || i1>width-1) continue;
								
				if(buffer[k1*size+j1*width+i1]==0)
				{
					i++;
				}
				else
				{
					j++;
				}
			}
		}
	}

	//确定当前点的颜色
	if(i>j)
	{
		m_bColor=0;
	}
	else
	{
		m_bColor=255;
	}

	
	aim=new unsigned char[allsize];
	visited=new unsigned char[allsize];
	lp=new unsigned char[allsize];

	for (m=0; m<allsize; m++)
	{
		aim[m]=0;
		visited[m]=0;
		lp[m]=0;
	}

	i=x;
	j=y;
	k=z;
	
 
	lp[k*size+j*width+i]=1;
	aim[k*size+j*width+i]=1;
	visited[k*size+j*width+i]=1;
	
	int nSum=0;
	while (1)
	{
		for(k=0;k<depth;k++)
		{
			for (j=0;j<height;j++)
			{
				for (i=0;i<width;i++)
				{
					if (!lp[k*size+j*width+i]) continue;
					for (m=-1;m<2; m++)
					{
						for (n=-1; n<2; n++)
						{
							for (l=-1; l<2; l++)
							{
								k1=k+m;
								j1=j+n;
								i1=i+l;
								if(k1<0 || k1>depth-1) continue;
								if(j1<0 || j1>height-1) continue;
								if(i1<0 || i1>width-1) continue;
								if(visited[k1*size+j1*width+i1]) continue;

								visited[k1*size+j1*width+i1]=1;
								if(buffer[k1*size+j1*width+i1]==m_bColor)
								{
									aim[k1*size+j1*width+i1]=1;
									lp[k1*size+j1*width+i1]=1;
								}
							}
						}
					}
				}
			}
		}
		for (k=0; k<depth; k++)
		{
			for (j=0; j<height; j++)
			{
				for (i=0; i<width; i++)
				{
					if(!lp[k*size+j*width+i]) continue;
					m_b=1;
					for (m=-1;m<2;m++)
					{
						for (n=-1;n<2;n++)
						{
							for (l=-1;l<2;l++)
							{
								k1=k+m;
								j1=j+n;
								i1=i+l;
								if(k1<0||k1>depth-1) continue;
								if(j1<0||j1>height-1) continue;
								if(i1<0||i1>width-1) continue;
								if (!visited[k1*size+j1*width+i1])
								{
									m_b=0;
								}
							}
						}
					}
					if (m_b)
					{
						lp[k*size+j*width+i]=0;
					}
				}
			}
		}
		m_b=1;
		for(k=0;k<allsize;k++)
		{
			if (lp[k])
			{
				m_b=0;
				break;
			}
		}
		if(m_b) break;
	}

	nSum=0;

	for(k=0;k<allsize;k++)
	{
		buffer[k]=255-m_bColor;
		if (aim[k])
		{
			buffer[k]=m_bColor;
			nSum++;
		}
	}

	delete []lp;
	delete []aim;
	delete []visited;
	return 1;
}

 


bool SaveBmp(char *bmpName,unsigned char *pBmpBuf,int width,int height)
{
	FILE *fp;
	if( (fp = fopen(bmpName,"wb") )== NULL)   
	{
		printf("打开失败!");
		return 0;
	}
	int i,j;
	BITMAPFILEHEADER fileHead;
	int lineByte= (width+ 3) / 4*4;
	fileHead.bfType = 0x4D42;   
	fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 1024+lineByte* height;
	fileHead.bfReserved1 = 0;
	fileHead.bfReserved2 = 0;
	fileHead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD);
	fwrite(&fileHead,sizeof(BITMAPFILEHEADER),1,fp);
	BITMAPINFOHEADER infoHead;
	infoHead.biSize = 40;
	infoHead.biWidth = width;
	infoHead.biHeight = height;
	infoHead.biPlanes = 1;
	infoHead.biBitCount = 8;
	infoHead.biCompression = BI_RGB;
	infoHead.biSizeImage = lineByte* height;
	infoHead.biXPelsPerMeter = 0;
	infoHead.biYPelsPerMeter = 0;
	infoHead.biClrUsed = 256;
	infoHead.biClrImportant = 256;
	fwrite(&infoHead,sizeof(BITMAPINFOHEADER),1,fp);
	RGBQUAD palette[256];
	for (i=0;i<256;i++)      
	{  
		palette[i].rgbBlue=i;		palette[i].rgbGreen=i;
		palette[i].rgbRed=i;
		palette[i].rgbReserved=0;
		fwrite(&palette[i].rgbBlue,sizeof(unsigned char),1,fp);  
		fwrite(&palette[i].rgbGreen,sizeof(unsigned char),1,fp);  
		fwrite(&palette[i].rgbRed,sizeof(unsigned char),1,fp);  
		fwrite(&palette[i].rgbReserved,sizeof(unsigned char),1,fp);  
	} 
	if (width<lineByte)  //如果有效数据宽度小于BMP格式要求宽度  
	{  
		unsigned char *imgBufBMP=new unsigned char [lineByte*height];   
		for (i=0;i<height;i++)  
		{  
			for (j=0;j<width;j++)  
			{  
				*(imgBufBMP+i*lineByte+j)=*(pBmpBuf+i*width+j);    
			}  
		}  
		for (i=0;i<height;i++)  
		{  
			for (j=width;j<lineByte;j++)  
			{  
				*(imgBufBMP+i*lineByte+j)=0;      
			}  
		}   
		fwrite(imgBufBMP, height*lineByte, 1, fp);  
	}  
	else  
	{  
		fwrite(pBmpBuf, height*lineByte, 1, fp);  
	}  
	fclose(fp); 
	return 1;
}


void main()
{
	double result=0;

	char CLIP_fileName1[] = "testdata.raw";
	char CLIP_fileName2[] = "testdata.dim";

	readraw1(CLIP_fileName2);
	unsigned char *data= new unsigned char [allsize];
	unsigned char *buf=new unsigned char[size],*lpSrc,*lpSrc1,*lpSrc2,*lpDst1,*lpDst,*lpDst2,*lpSrc3;
	unsigned char *buf1=new unsigned char[size];
	unsigned char *buf2=new unsigned char[size],px[9];
	readraw2(CLIP_fileName1,data);

    ///////////  滤波参数设置 
	int i,j,k,s,num,m,n;
    float   xihe,he,diff,xishu1,xishu2,temp,temp1,temp2,Yuzhi,Yuzhi1,alpha;
	int     length,length2,length3,q,p,cishu;
	
 
    static unsigned char value[1000][1000][9];
    ////////////////////////////////////////
    alpha=0.6;
	Yuzhi=alpha*120; 

	length=8;//
    length3=3;//相似窗 
	length2=3;//搜索窗
	Yuzhi1=Yuzhi*Yuzhi;

	for(k=0;k<depth;k++)
	{
		char file_name[10] = {0};
		sprintf(file_name,"%d.bmp",k);
		for (i=0; i<height; i++)
		{
			for (j=0; j<width; j++)
			{
				*(buf1+i*width+j)=*(data+k*size+i*width+j);
				*(buf2+i*width+j)=0;
			}
		}

       //边缘检测 
     for(i=length/2;i<height-length/2;i++)
	 {
     lpDst1=data+k*size+width*(i+length3/2);
	 lpDst2=buf2+i*width;
	 lpSrc3=data+k*size+width*i;
	 for(j=length/2;j<width-length/2;j++)
	 {
	 lpDst=lpDst2+j;
	 lpSrc2=lpDst1+j-length3/2;
	 lpSrc1=lpSrc3+j;
	 cishu=0;
	 for(m=0;m<length3;m++)
	 {
     temp=abs(*(lpSrc2-width*m)-*lpSrc1);
	 if(temp>=5)
	 {
     *lpDst=255;
	 break;
	 }
	 }
	 }
	 }


        //非局部均值 
        //保存矩阵
        for(i=length/2;i<height-length/2;i++)
		{
		lpDst2=data+k*size+width*(i+length3/2);
    	for(j=length/2;j<width-length/2;j++)
		{
		 cishu=0;
	     lpSrc2=lpDst2+j-length3/2;
		 
        
         for(m=0;m<length3;m++)
		 {
         value[i][j][cishu++]=*(lpSrc2+m);
		 }
		}
		}

		//开始滤波
		for(i=length/2;i<height-length/2;i++)
		{
        lpDst2=buf1+width*i;
		lpSrc2=buf2+width*i;
    	for(j=length/2;j<width-length/2;j++)
		{
	     he=0;
	     xishu2=0;
         lpDst1=lpDst2+j;
		 lpDst=lpSrc2+j;

         if(*lpDst==255)
		 {
	     for(p=i-length3/2;p<=i+length3/2;p++)
		 {
	     for(q=j-length3/2;q<=j+length3/2;q++)
		 { 
		  diff=0;
		  lpSrc1=data+k*size+width*p+q;
          for(m=0;m<length3;m++)
		  {
		  temp=abs(value[i][j][m]-value[p][q][m]);
		  diff+=float(temp)*float(temp); 
          }    
          
		  if(diff==0)
		  xishu1=1;
		  else if(diff<=0.5*Yuzhi1)
		  {
		  temp=-1*diff/Yuzhi1;
		  temp1=temp*temp;
		  temp2=temp1*temp;
          xishu1=1+temp+temp1/2+temp2/6;
		  }
		  else
          xishu1=exp(-1*diff/Yuzhi1);

		  xishu2+=xishu1;
		  he+=xishu1*(*lpSrc1);	
		 }
		 } 
	      *lpDst1=he/xishu2;
		 }
		}
		}
        //非局部均值 
		SaveBmp(file_name,buf1,width,height);
	}

	delete []data;
	delete []buf;
	delete []buf1;
	system("pause");
}


