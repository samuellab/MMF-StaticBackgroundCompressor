/* 
 * File:   main.cpp
 * Author: Marc
 *
 * Created on January 25, 2012, 11:49 AM
 */

#include <cstdlib>
#include <cv.h>
#include <ostream>
#include <iostream>
using namespace std;

/*
 * 
 */

//typedef struct _IplImage
//{
//    int  nSize;         /* sizeof(IplImage) */
//    int  ID;            /* version (=0)*/
//    int  nChannels;     /* Most of OpenCV functions support 1,2,3 or 4 channels */
//    int  alphaChannel;  /* ignored by OpenCV */
//    int  depth;         /* pixel depth in bits: IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16S,
//                           IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F are supported */
//    char colorModel[4]; /* ignored by OpenCV */
//    char channelSeq[4]; /* ditto */
//    int  dataOrder;     /* 0 - interleaved color channels, 1 - separate color channels.
//                           cvCreateImage can only create interleaved images */
//    int  origin;        /* 0 - top-left origin,
//                           1 - bottom-left origin (Windows bitmaps style) */
//    int  align;         /* Alignment of image rows (4 or 8).
//                           OpenCV ignores it and uses widthStep instead */
//    int  width;         /* image width in pixels */
//    int  height;        /* image height in pixels */
//    struct _IplROI *roi;/* image ROI. if NULL, the whole image is selected */
//    struct _IplImage *maskROI; /* must be NULL */
//    void  *imageId;     /* ditto */
//    struct _IplTileInfo *tileInfo; /* ditto */
//    int  imageSize;     /* image data size in bytes
//                           (==image->height*image->widthStep
//                           in case of interleaved data)*/
//    char *imageData;  /* pointer to aligned image data */
//    int  widthStep;   /* size of aligned image row in bytes */
//    int  BorderMode[4]; /* ignored by OpenCV */
//    int  BorderConst[4]; /* ditto */
//    char *imageDataOrigin; /* pointer to very origin of image data
//                              (not necessarily aligned) -
//                              needed for correct deallocation */
//}
//IplImage;
int main(int argc, char** argv) {

    cout << "size of Ipl Image is: " << sizeof(IplImage) << "  win32 size = 112" << endl;
    cout << "size of int is: " << sizeof(int) << "  win32 size = 4" << endl;
    cout << "size of pointer is: " << sizeof (void *) << "  win32 size = 4" << endl;
    cout << "20*sizeof(int) + 6*sizeof(pointer) + 8 (2 char arrays[4]) = " << 20*sizeof(int) + 6 * sizeof (void *) + 8 << endl;
    cout << "sizeof (CvRect) = " << sizeof(CvRect) << "  win32 size = 16" << endl;
    cout << "sizeof (double) = " << sizeof(double) << "  win32 size = 8" << endl;
    //you can comment out the next 4 lines if you don't want to link to the cv libraries
    IplImage *im = cvCreateImage(cvSize(1024,1024), IPL_DEPTH_8U, 1);
    cout << "im->nsize = " << im->nSize << endl;
    cout << "*(int *) im = " << *(int *) im << endl;
    cvReleaseImage(&im);
    return 0;
}

