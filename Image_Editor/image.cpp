// main image
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

using namespace std;

#include "image.h"

ImageType::ImageType()
{
 N = 0;
 M = 0;
 Q = 0;

 pixelValue = NULL;
}

ImageType::ImageType(int tmpN, int tmpM, int tmpQ)
{
 int i, j;

 N = tmpN;
 M = tmpM;
 Q = tmpQ;

 pixelValue = new int* [N];
 for(i=0; i<N; i++) {
   pixelValue[i] = new int[M];
   for(j=0; j<M; j++)
     pixelValue[i][j] = 0;
 }
}

ImageType::ImageType(const ImageType& rhs)
{
 int i, j;

 N = rhs.N;
 M = rhs.M;
 Q = rhs.Q;

 pixelValue = new int* [N];
 for(i=0; i<N; i++) 
 {
   pixelValue[i] = new int[M];
   for(j=0; j<M; j++)
   {
     pixelValue[i][j] = rhs.pixelValue[i][j];
   }
 }
}

ImageType& ImageType::operator=(const ImageType& rhs)
{
 int i, j;

 if(pixelValue != NULL)
 {

   for(i=0; i<N; i++)
   {
     delete [] pixelValue[i];
   }
    
   delete [] pixelValue;
 }

 N = rhs.N;
 M = rhs.M;
 Q = rhs.Q;

 pixelValue = new int*[N];
 for(i=0; i<N; i++) 
 {
   pixelValue[i] = new int[M];
   for(j=0; j<M; j++)
   {
     pixelValue[i][j] = rhs.pixelValue[i][j];
   }
 }

 return *this;
}



void ImageType::getImageInfo(int& rows, int& cols, int& levels)
{
 rows = N;
 cols = M;
 levels = Q;
} 

void ImageType::setImageInfo(int rows, int cols, int levels)
{
 N= rows;
 M= cols;
 Q= levels;
} 

void ImageType::setPixelVal(int i, int j, int val)
{
 pixelValue[i][j] = val;
}

void ImageType::getPixelVal(int i, int j, int& val)
{
 val = pixelValue[i][j];
}

ImageType::~ImageType()
{
    int i;
    
    for(i=0; i<N; i++)
    {
        delete [] pixelValue[i];
    }
    
    delete [] pixelValue;
}

void ImageType::getSubImage(int ULr, int ULc, int LRr, int LRc, ImageType old)
{
  int i, j, tmp;

  ImageType tempImg(LRr-ULr, LRc-ULc, 255);
  createBlankCopy(tempImg);

  // begin row at Upper left corner index end at Lower Right corner index
  for( i = ULr; i < LRr; i++ )
  {
    // begin column at UL corner index, end at LR corner index
    for( j = ULc; j < LRc; j++ )
    {
     // get val from image and store it temporarily
     pixelValue[i-ULr][j-ULc] = old.pixelValue[i][j];
    }
  }
}

void ImageType::negateImage()
{
  int i, j;

  for(i=0; i<N; i++) 
  {
   for(j=0; j<M; j++)
   {
     // selects greyscale value with same distance from opposite extreme
     pixelValue[i][j] = 255-pixelValue[i][j];
   }
  }
}

void ImageType::reflectImage(bool flag, ImageType oldImage) 
{
  ImageType tempImg(oldImage.N, oldImage.M, oldImage.Q);   
  createBlankCopy(tempImg);
 
  if( flag )  // if true, reflect vertically 
  {
    for( int i=0; i<N; i++ )
    {
      for( int j=0; j<M; j++ )
      {
        pixelValue[i][M-1-j] = oldImage.pixelValue[i][j];
      }
    }
  }
  else        // if false, reflect horizontally
  {
    for( int j=0; j<M; j++ )
    {
      for( int i=0; i<N; i++ )
      {
        pixelValue[N-1-i][j] = oldImage.pixelValue[i][j];
      }
    }
  }
}

void ImageType::enlargeImage(int s, ImageType oldImage) 
{
  ImageType tmp( s*oldImage.N, s*oldImage.M, oldImage.Q );
  createBlankCopy(tmp);

  for( int i=0; i<N; i++ )
  {
    for( int j=0; j<M; j++ )
    {
       // uses scale to determine which pixels to copy to in new image
       pixelValue[i][j] = oldImage.pixelValue[i/s][j/s];
    }
  }
}

void ImageType::shrinkImage(int s, ImageType oldImage) 
{
  ImageType tmp( oldImage.N/s, oldImage.M/s, Q );
  createBlankCopy(tmp);

  for( int i=0; i<N*s; i=i+s )
  {
    for( int j=0; j<M*s; j=j+s )
    {  
       // increments by the scale value to determine pixel placement
       pixelValue[i/s][j/s] = oldImage.pixelValue[i][j];
    }
  }
}

int ImageType::meanGray()
{
    int i, j, sum = 0;
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<M; j++)
        {
            sum += pixelValue[i][j];
        }
    }
    
    if(N*M==0)
        return -1;
    
    return sum / (N*M);
}

ImageType ImageType::operator+(const ImageType& rhs)
{
    return add(rhs, 0.5); 
}

ImageType ImageType::add(const ImageType& rhs, double a)
{
    ImageType image(N, M, Q);
    
    int i, j;
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<M; j++)
        {
            image.pixelValue[i][j] = pixelValue[i][j] * a + rhs.pixelValue[i][j] * (1-a);
        }
    }
    return image; 
}

ImageType ImageType::operator-(const ImageType& rhs)
{
    ImageType image(N, M, Q);
    
    int i, j;
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<M; j++)
        {
            image.pixelValue[i][j] = pixelValue[i][j] - rhs.pixelValue[i][j];
            
            if(image.pixelValue[i][j] < 0)
            {
                image.pixelValue[i][j] *= -1;
            }
        }
    }
    return image;
}

ImageType ImageType::betterSubtract(const ImageType& rhs)
{
    ImageType image(N, M, Q);
    
    int i, j, meanGrayVal;
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<M; j++)
        {
            image.pixelValue[i][j] = pixelValue[i][j] - rhs.pixelValue[i][j];
            
            if(image.pixelValue[i][j] < 0)
            {
                image.pixelValue[i][j] *= -1;
            }

			if(image.pixelValue[i][j] < 20)
            {
                image.pixelValue[i][j] = 0;
            }
        }
    }
    
    return image; 
}

void ImageType::rotateImage(double theta, ImageType oldImage)
{
	rotateImage(0, 0, theta, oldImage);
}

void ImageType::rotateImage(int cX, int cY, double theta, ImageType oldImage)
{
    int i, j, newX, newY;
    double thetaRadians = theta * M_PI / 180.0;

    createBlankCopy(oldImage);
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<M; j++)
        {
            newX = round(cX + (i - cX) * cos(thetaRadians) - (j - cY) * sin(thetaRadians));
            newY = round(cX + (i - cX) * sin(thetaRadians) + (j - cY) * cos(thetaRadians));
            
            if(newX >= 0 && newX < N && newY >= 0 && newY < M)
            {
                pixelValue[newX][newY] = oldImage.pixelValue[i][j];
            }
        }
    }
}

void ImageType::betterRotateImage(double theta, ImageType oldImage)
{
	betterRotateImage(0, 0, theta, oldImage);
}

void ImageType::betterRotateImage(int cX, int cY, double theta, ImageType oldImage)
{
    int i, j;
    double newX, newY;
    double thetaRadians = theta * M_PI / 180.0;
    
    int newXCeil, newXFloor, newYCeil, newYFloor;
    
    createBlankCopy(oldImage);
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<M; j++)
        {
            newX = cX + (i - cX) * cos(thetaRadians) - (j - cY) * sin(thetaRadians);
            newY = cX + (i - cX) * sin(thetaRadians) + (j - cY) * cos(thetaRadians);
            
            if(newX >= 0 && newX < N && newY >= 0 && newY < M)
            {
                newXCeil = ceil(newX);
                newXFloor = floor(newX);
                newYCeil = ceil(newY);
                newYFloor = floor(newY);
                
                pixelValue[newXCeil][newYCeil] += oldImage.pixelValue[i][j];
                pixelValue[newXCeil][newYFloor] += oldImage.pixelValue[i][j];
                pixelValue[newXFloor][newYFloor] += oldImage.pixelValue[i][j];
                pixelValue[newXFloor][newYCeil] += oldImage.pixelValue[i][j];
                
                // if it wasn't zero before, average the two
                if(pixelValue[newXCeil][newYCeil] != oldImage.pixelValue[i][j])
                    pixelValue[newXCeil][newYCeil] /= 2;
                
                if(pixelValue[newXCeil][newYFloor] != oldImage.pixelValue[i][j])
                    pixelValue[newXCeil][newYFloor] /= 2;
                
                if(pixelValue[newXFloor][newYFloor] != oldImage.pixelValue[i][j])
                    pixelValue[newXFloor][newYFloor] /= 2;
                
                if(pixelValue[newXFloor][newYCeil] != oldImage.pixelValue[i][j])
                    pixelValue[newXFloor][newYCeil] /= 2;
            }
        }
    }
}

void ImageType::translateImage(int t, ImageType oldImage)
{
    createBlankCopy(oldImage);
    
    translateImage(t,t,oldImage);
}

void ImageType::translateImage(int x, int y, ImageType oldImage)
{
    int i,j;
    
    createBlankCopy(oldImage);
    
    for(i=0; i<N; i++)
    {
        for(j=0; j<M; j++)
        {
            if(i+x >= 0 && i+x < N && j+y >= 0 && j+y < M)
                pixelValue[i+x][j+y] = oldImage.pixelValue[i][j];
        }
    }
}

void ImageType::createBlankCopy(const ImageType &oldImage)
{
    int i,j;
    
    if(pixelValue!=NULL)
    {
        for(i=0; i<N; i++)
        {
            delete [] pixelValue[i];
        }
    
        delete [] pixelValue;
    }
    
    M = oldImage.M;
    N = oldImage.N;
    Q = oldImage.Q;
    
    pixelValue = new int* [N];
    for(i=0; i<N; i++) 
    {
        pixelValue[i] = new int[M];
        for(j=0; j<M; j++)
            pixelValue[i][j] = 0;
    }
}

