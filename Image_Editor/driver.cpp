#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

#include "image.h"

// given functions
int readImageHeader(char[], int&, int&, int&, bool&);
int readImage(char[], ImageType&);
int writeImage(char[], ImageType&);

// driver functions
char displayMenu();
void loadImage(ImageType&);
void saveImage(ImageType&);
void displayInfo(ImageType&);
void crop(ImageType&);
void resize(ImageType&);
void reflect(ImageType&);
void translate(ImageType&);
void rotate(ImageType&);
void add(ImageType&);
void subtract(ImageType&);
void negative(ImageType&);
bool quitProgram();
bool imageLoaded(ImageType&);
void pressEnterToContinue();
void clearScreen();


int main(int argc, char *argv[])
{
    bool continueProgram = true;
        ImageType image;    

    // loop until program is quit
    while(continueProgram)
    {
        switch(displayMenu()) 
        {
            // load image
            case '1':
                loadImage(image);
                break;
                
            // save image
            case '2':
                saveImage(image);
                break;
                
            // display info and average gray
            case '3':
                displayInfo(image);
                break;
                
            // crop
            case '4':
                crop(image);
                break;
                
            // resize
            case '5':
                resize(image);
                break;
                
            // reflect
            case '6':
                reflect(image);
                break;
                
            // translate
            case '7':
                translate(image);
                break;
                
            // rotate
            case '8':
                rotate(image);
                break;
                
            // add
            case '9':
                add(image);
                break;
                
            // subtract
            case 'S':
            case 's':
                subtract(image);
                break;
            
            // negative
            case 'N':
            case 'n':
                negative(image);
                break;
                
            // quit
            case 'Q':
            case 'q':
                continueProgram = !quitProgram();
                                break;

                        // undocumented features for quick testing
            // open hardcoded input image and save to hardcoded output file
                        case 'o':
                                {
                                        char* fileName = new char[300];
                                        strcpy(fileName, "george1.pgm");
                                        int N, M, Q;
                                        bool type;
                                        readImageHeader(fileName, N, M, Q, type);
                                        ImageType tempImage(N, M, Q);
                                        image = tempImage;
                                        readImage(fileName, image);
                                        delete [] fileName;
                                }
                                break;
                        case 'O':
                                char* fileName = new char[300];
                                strcpy(fileName, "output.pgm");
                                writeImage(fileName, image);
                                delete [] fileName;
                                break;
        }
    }
    
    return 0;
}

char displayMenu()
{
    char input;
    
    // unix and windows compatible system call
    clearScreen();
    
    cout << "=======================================================" << endl
         << "| Image Processing                               v002 |" << endl
         << "+-----------------------------------------------------+" << endl
         << "| 1. Load Image            | 8. Rotate                |" << endl
         << "+-----------------------------------------------------+" << endl
         << "| 2. Save Image            | 9. Add                   |" << endl
         << "+-----------------------------------------------------+" << endl
         << "| 3. Display Image Info    | S. Subtract              |" << endl
         << "+-----------------------------------------------------+" << endl
         << "| 4. Crop                  | N. Negative              |" << endl
         << "+-----------------------------------------------------+" << endl
         << "| 5. Resize                | C. Count & Label Regions |" << endl
         << "+-----------------------------------------------------+" << endl
         << "| 6. Reflect               | Q. Quit                  |" << endl
         << "+-----------------------------------------------------+" << endl
         << "| 7. Translate             |" << endl
         << "+--------------------------+" << endl
         << endl
         << "Enter the number or letter of a menu item: ";
    
    cin >> input; 

        // add line before executing menu item
        cout << endl;
    
    return input;
}

void loadImage(ImageType& image)
{
        int M, N, Q;
    bool type;
        char fileName[300];

        // prompt for file 
        cout << "Enter file name: ";
        cin >> fileName;

        // load image info
        readImageHeader(fileName, N, M, Q, type);

        // create blank image the size of the file's image
        ImageType tempImage(N, M, Q);
    image = tempImage;

    // read image
    readImage(fileName, image);
}

void saveImage(ImageType& image)
{
        if(imageLoaded(image))
        {
                char fileName[300];

                // prompt for file
                cout << "Enter output file name: ";
                cin >> fileName;        

                // save file
                writeImage(fileName, image);

                cout << endl << "Image successfully saved.";

                pressEnterToContinue();
        }
}

void displayInfo(ImageType& image)
{
    if(imageLoaded(image))
        {
                int N, M, Q;
                
                // get values
                image.getImageInfo(N,M,Q);

                cout << "Height           : " << N << endl
                         << "Width            : " << M << endl
                         << "Max Pixel Value  : " << Q << endl
                         << "Mean Gray Value  : " << image.meanGray();

                pressEnterToContinue();
        }
}

void crop(ImageType& image)
{
    if(imageLoaded(image))
        {
                // initialize to -1 for input validation
                int ULr = -1, ULc = -1, LRr = -1, LRc = -1;
                int N, M, Q;
                int errorCode = 0;

                // get values
                image.getImageInfo(N,M,Q);

                // get inputs
                cout << "Enter the upper left corner's row: ";
                cin >> ULr;             

                cout << endl << "Enter the upper left corner's column: ";
                cin >> ULc;

                cout << endl << "Enter the lower right corner's row: ";
                cin >> LRr;

                cout << endl << "Enter the lower right corner's column: ";
                cin >> LRc;

                // check for errors
                if(ULr < 0 || ULc < 0 || LRr < 0 || LRc < 0)
                        errorCode = 1;
                        
                else if(ULr > N || LRr > N || ULc > M || LRc > M)
                        errorCode = 2;

                else if(ULr >= LRr || ULc >= LRc)
                        errorCode = 3;

                switch(errorCode)
                {
                        case 1:
                                cout << "ERROR: All inputs must be non-negative.";
                                break;
                        case 2:
                                cout << "ERROR: All crop boundaries must be within image boundaries.";
                                break;
                        case 3:
                                cout << "ERROR: All crop boundaries must be in the correct order.";
                                break;
                }
                
                // crop image if no error was found
                if(errorCode == 0)
                {
                        image.getSubImage(ULr, ULc, LRr, LRc, image);
                        cout << endl << endl << "Image has been cropped successfully.";
                }
        
                pressEnterToContinue();
        }
}               

void resize(ImageType& image)
{
    if(imageLoaded(image))
        {
                char option, charScale;
                int scale;

                cout << "1) Enlarge" << endl
                     << "2) Shrink" << endl << endl
                         << "Select option 1 or 2: ";
                
                cin >> option;

                // check for appropriate selection and get scale
                if(option == '1' || option == '2')
                {
                        cout << endl <<  "Enter the scaling value: ";
                        cin >> charScale;
                        cout << endl << endl;
                        scale = int(charScale) - int('0');

            // shrink or enlarge
                        switch(option)
                        {
                                case '1':
                                        image.enlargeImage(scale, image);
                                        break;
                                case '2':
                                        image.shrinkImage(scale, image);
                                        break;
                        }

                        cout << "The image was successfully scaled.";
                }

                else
                {
                        cout << "ERROR: That was not an acceptable option.";
                }

                pressEnterToContinue();
        }
}

void reflect(ImageType& image)
{
 bool VERTICAL = true;
 bool HORIZONTAL = false;

    if(imageLoaded(image))
        {
                char option;

                cout << "1) Vertical" << endl
                     << "2) Horizontal" << endl << endl
                         << "Select option 1 or 2: ";
                
                cin >> option;

                // check for appropriate selection and reflect
                switch(option)
                {
                        case '1':
                                image.reflectImage(VERTICAL, image);
                                cout << "The image was successfully reflected vertically.";
                                break;
                        case '2':
                                image.reflectImage(HORIZONTAL, image);
                                cout << "The image was successfully reflected horizontally.";
                                break;
                        default:
                                cout << "ERROR: That was not an acceptable option.";
                                break;
                }

                pressEnterToContinue();
        }
}

void translate(ImageType& image)
{
    if(imageLoaded(image))
        {
                int xOffset = 0, yOffset = 0;

                // get offset values
                cout << "Enter distance to translate in the x direction: ";
                cin >> xOffset;
                cout << endl; 

                // clear for inputs
                cin.clear();
                cin.ignore(5000, '\n');

                cout << "Enter distance to translate in the y direction: ";
                cin >> yOffset;
                cout << endl; 

                // translate
                image.translateImage(xOffset, yOffset, image);

                cout << endl << "The image was successfully translated.";
                pressEnterToContinue();
        }    
}

void rotate(ImageType& image)
{
    if(imageLoaded(image))
        {
                char option;
                int cX, cY, theta;

                cout << "1) Basic Rotation" << endl
                     << "2) Smooth Rotation" << endl << endl
                         << "Select option 1 or 2: ";
                
                cin >> option;
                cout << endl;

        
        // select rotation details
                if(option == '1' || option == '2')
                {
                        cout << "Enter the x-coordinate to rotate about: ";
                        cin >> cX;
                        cout << endl;

                        cout << "Enter the y-coordinate to rotate about: ";
                        cin >> cY;
                        cout << endl;

                        cout << "Enter the angle of rotation (in degrees): ";
                        cin >> theta;
                        cout << endl;

            // rotate using selected version of rotate
                        switch(option)
                        {
                                case '1':
                                        image.rotateImage(cX, cY, theta, image);
                                        break;
                                case '2':
                                        image.betterRotateImage(cX, cY, theta, image);
                                        break;
                        }

                        cout << "The image has been successfully rotated.";
                }
                
                else
                {
                        cout << "ERROR: That was not an acceptable option.";
                }

                pressEnterToContinue();
        }  
}

void add(ImageType& image)
{
    if(imageLoaded(image))
        {
                double weight;

                cout << "Load second image:" << endl << endl;
                ImageType image2;
        
        // get second image
                loadImage(image2);

        // clear cin to allow another entry
                cin.clear();
                cin.ignore(50000, '\n');

                cout << endl
                         << "Enter a decimal between 0 and 1 for the weight of the first image: ";
                cin >> weight;
                cout << endl;

        // use + operator for equally weighted images
                if(weight > 1 || weight < 0)
                {
                        cout << "The weight entered was invalid, so equal weights are assumed.";
                        image = image + image2;
                }

                else
                {
                        image = image.add(image2, weight);
                }               

                cout << "The images have been successfully added.";

                pressEnterToContinue();
        }    
}

void subtract(ImageType& image)
{
    if(imageLoaded(image))
        {
    if(imageLoaded(image))
        {
                char option;

                cout << "1) Basic Subtraction" << endl
                     << "2) Subtraction with automatic threshold" << endl << endl
                         << "Select option 1 or 2: ";
                
                cin >> option;
                cout << endl;

                if(option == '1' || option == '2')
                {
            // prompt for second image and load it
                        cout << "Load second image:" << endl << endl;
                        ImageType image2;
                        loadImage(image2);

            // subtract using the normal subtract or the extra credit version
                        switch(option)
                        {
                                case '1':
                                        image = image - image2;
                                        break;
                                case '2':
                                        image = image.betterSubtract(image2);
                                        break;
                        }

                        cout << "The image has been successfully subtracted.";
                }
                
                else
                {
                        cout << "ERROR: That was not an acceptable option.";
                }

                pressEnterToContinue();
        }       
        }    
}

void negative(ImageType& image)
{
    if(imageLoaded(image))
        {
                image.negateImage();
                cout << "The image was successfully negated.";
                pressEnterToContinue();
        }    
}

bool quitProgram()
{
    return true;
}

bool imageLoaded(ImageType& image)
{
        // get Q value
        int a, Q;
    image.getImageInfo(a,a,Q);
        
        // check if an image was loaded in
        if(Q==0)
        {
                // Somebody didn't load a file... YOU!
                cout << "ERROR: No image file was loaded.";
                
                pressEnterToContinue();

                return false;
        }       

        else
        {
                return true;
        }
}

void pressEnterToContinue()
{
        cout << endl << endl << "Press enter to continue...";

        // wait until enter pressed
        do
        {
        // here is a new comment
                cin.clear();
                cin.ignore(50000, '\n');
        } while(cin.get()!='\n');
}

void clearScreen()
{
    #ifdef WINDOWS
    
        system("CLS");
    
    #else
    
        system("clear");
    
    #endif

}
