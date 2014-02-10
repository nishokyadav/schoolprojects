#ifndef IMAGE_H
#define IMAGE_H

const bool VERTICAL = true;
const bool HORIZONTAL = false;

class ImageType 
{
    public:
        ImageType();
        ImageType(int, int, int);
        ~ImageType();
        ImageType(const ImageType&);
        ImageType& operator=(const ImageType&);
        ImageType operator+(const ImageType&);
        ImageType operator-(const ImageType&); 

		// utility function		
		void createBlankCopy(const ImageType &oldImage);
    
        // extra credit
        void betterRotateImage(double theta, ImageType oldImage);
		void betterRotateImage(int cX, int cY, double theta, ImageType oldImage);
        ImageType betterSubtract(const ImageType&);
    
        void getImageInfo(int&, int&, int&);
        void setImageInfo(int, int, int);
        void setPixelVal(int, int, int);
        void getPixelVal(int, int, int&);
        void getSubImage(int ULr, int ULc, int LRr, int LRc, ImageType old);
        int meanGray();
        void enlargeImage(int s, ImageType oldImage);
        void shrinkImage(int s, ImageType oldImage);
        void reflectImage(bool flag, ImageType oldImage);
		void translateImage(int t, ImageType oldImage);
		void translateImage(int x, int y, ImageType oldImage);
        void rotateImage(double theta, ImageType oldImage);
		void rotateImage(int cX, int cY, double theta, ImageType oldImage);
        void negateImage();
		ImageType add(const ImageType&, double);
    
    private:
        int N, M, Q;
        int **pixelValue;
};

#endif

/*
not me
copy constructor
assignment operator
getSubImage
negateImage
enlarge, shrink 
reflect 
if you have time, better shrink

me
destructor
+ and - operator
meanGray
rotate and extra credit rotate
translate 
threshold thing for subtracting images
*/
