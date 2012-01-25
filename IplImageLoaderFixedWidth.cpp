/* 
 * File:   IplImageLoaderFixedWidth.cpp
 * Author: Marc
 * 
 * Created on January 25, 2012, 11:36 AM
 */


#include <iostream>
#include <stdint.h>
#include <fstream>
#include <iosfwd>
#include "IplImageLoaderFixedWidth.h"

using namespace std;

typedef uint32_t ptrtype112;
typedef uint64_t ptrtype136;

typedef struct _IplImage112
{
    int32_t  nSize;         /* sizeof(IplImage) */
    int32_t  ID;            /* version (=0)*/
    int32_t  nChannels;     /* Most of OpenCV functions support 1,2,3 or 4 channels */
    int32_t  alphaChannel;  /* ignored by OpenCV */
    int32_t  depth;         /* pixel depth in bits: IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16S,
                           IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F are supported */
    char colorModel[4]; /* ignored by OpenCV */
    char channelSeq[4]; /* ditto */
    int32_t  dataOrder;     /* 0 - interleaved color channels, 1 - separate color channels.
                           cvCreateImage can only create interleaved images */
    int32_t  origin;        /* 0 - top-left origin,
                           1 - bottom-left origin (Windows bitmaps style) */
    int32_t  align;         /* Alignment of image rows (4 or 8).
                           OpenCV ignores it and uses widthStep instead */
    int32_t  width;         /* image width in pixels */
    int32_t  height;        /* image height in pixels */
    ptrtype112 roi;/* image ROI. if NULL, the whole image is selected */
    ptrtype112 maskROI; /* must be NULL */
    ptrtype112 imageId;     /* ditto */
    ptrtype112 tileInfo; /* ditto */
    int32_t  imageSize;     /* image data size in bytes
                           (==image->height*image->widthStep
                           in case of interleaved data)*/
    ptrtype112 imageData;  /* pointer to aligned image data */
    int32_t  widthStep;   /* size of aligned image row in bytes */
    int32_t  BorderMode[4]; /* ignored by OpenCV */
    int32_t  BorderConst[4]; /* ditto */
    ptrtype112 imageDataOrigin; /* pointer to very origin of image data
                              (not necessarily aligned) -
                              needed for correct deallocation */
}
IplImage112;


typedef struct _IplImage136
{
    int32_t  nSize;         /* sizeof(IplImage) */
    int32_t  ID;            /* version (=0)*/
    int32_t  nChannels;     /* Most of OpenCV functions support 1,2,3 or 4 channels */
    int32_t  alphaChannel;  /* ignored by OpenCV */
    int32_t  depth;         /* pixel depth in bits: IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16S,
                           IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F are supported */
    char colorModel[4]; /* ignored by OpenCV */
    char channelSeq[4]; /* ditto */
    int32_t  dataOrder;     /* 0 - interleaved color channels, 1 - separate color channels.
                           cvCreateImage can only create interleaved images */
    int32_t  origin;        /* 0 - top-left origin,
                           1 - bottom-left origin (Windows bitmaps style) */
    int32_t  align;         /* Alignment of image rows (4 or 8).
                           OpenCV ignores it and uses widthStep instead */
    int32_t  width;         /* image width in pixels */
    int32_t  height;        /* image height in pixels */
    ptrtype136 roi;/* image ROI. if NULL, the whole image is selected */
    ptrtype136 maskROI; /* must be NULL */
    ptrtype136 imageId;     /* ditto */
    ptrtype136 tileInfo; /* ditto */
    int32_t  imageSize;     /* image data size in bytes
                           (==image->height*image->widthStep
                           in case of interleaved data)*/
    ptrtype136 imageData;  /* pointer to aligned image data */
    int32_t  widthStep;   /* size of aligned image row in bytes */
    int32_t  BorderMode[4]; /* ignored by OpenCV */
    int32_t  BorderConst[4]; /* ditto */
    ptrtype136 imageDataOrigin; /* pointer to very origin of image data
                              (not necessarily aligned) -
                              needed for correct deallocation */
}
IplImage136;

IplImage readIplImageHeader112 (std::ifstream& is, int &nsize);
IplImage readIplImageHeader136 (std::ifstream& is, int &nsize);


IplImage *IplImageLoaderFixedWidth::loadIplImageFromByteStream(std::ifstream& is) {

    ifstream::pos_type cloc = is.tellg();
    int nsize;
    IplImage imheader;
    imheader = readIplImageHeader112(is, nsize);
    if (nsize != 112) {
        is.seekg(cloc);
        cout << "112 nsize = " << nsize << endl;
        imheader = readIplImageHeader136(is, nsize);
        if (nsize != 136) {
            is.seekg(cloc);
            cout << "136 nsize = " << nsize << endl;
            cout << "did not recognize size on disk; run cv size tester to determine size on system that wrote this file and then modify IplImageLoaderFixedWidth accordingly" <<endl;
            assert(false);
            return NULL;
        }
    }


    char *data = (char *) malloc(imheader.imageSize);
    is.read(data, imheader.imageSize);

    cvSetData(&imheader, data, imheader.widthStep);

    IplImage *imout = cvCreateImage(cvSize(imheader.width, imheader.height), imheader.depth, imheader.nChannels);
   // cout << "nsize = " << nsize << "  imheader.nSize = " << imheader.nSize  << "  imout->nSize = " << imout->nSize << "  sizeof(IplImage) = " << sizeof(IplImage) << endl;
    assert(imout != NULL);

    cvCopyImage(&imheader, imout);
    free(data);

    return imout;

}

/*
IplImage *IplImageLoaderFixedWidth::loadIplImageFromByteStream(std::ifstream& is) {

    ifstream::pos_type cloc = is.tellg();
    int32_t nsize;
    is >> nsize; is.seekg(cloc);
    cout << "int32_t nsize = " << nsize << endl;
    IplImage imheader;
    if (nsize == sizeof(IplImage)) {
        is.read((char *) &imheader, sizeof(IplImage));
        imheader.roi = NULL;
    }
    switch (nsize) {
        case 112:
            imheader = readIplImageHeader112 (is);
            break;
        case 136:
            imheader = readIplImageHeader136 (is);
            break;
        default:
            cout << "int32_t nsize is " << nsize << " which is not a recognized quantity";
            int64_t nsize2;
            is >> nsize2; is.seekg(cloc);
            cout << "int64_t nsize = " << nsize2 << endl;
            if (nsize2 == sizeof(IplImage)) {
                is.read((char *) &imheader, sizeof(IplImage));
                imheader.roi = NULL;
            }
            switch(nsize2) {
                case 112:
                    imheader = readIplImageHeader112 (is);
                    break;
                case 136:
                    imheader = readIplImageHeader136 (is);
                    break;
                default:
                    cout << "did not recognize size on disk; run cv size tester to determine size on system that wrote this file and then modify IplImageLoaderFixedWidth accordingly" <<endl;
                    return NULL;
            }
            break;
    }
    
  //  cout << "im params: w= " << im->width << ", h= " << im->height << ", nchannels = " << im->nChannels << ", depth = " << im->depth << ", width step = " << im->widthStep << "imageSize = " << im->imageSize << endl;
   // cout << "hmm..." << endl;
  //  cout << "imsize = " << cvGetSize(im).width << " x " << cvGetSize(im).height << endl;
    char *data = (char *) malloc(imheader.imageSize);
   // cout << "data = " << ((unsigned long long) data) << endl;
  //  cout << "memory allocated , imsize = " << cvGetSize(im).width << " x " << cvGetSize(im).height << endl;
    is.read(data, imheader.imageSize);
   // cout << "data read in; setting image data , imsize = " << cvGetSize(im).width << " x " << cvGetSize(im).height << endl;

    cvSetData(&imheader, data, imheader.widthStep);
  //  cout << "image data = " << ((unsigned long long) im->imageData) << ", image data origin = " << ((unsigned long) im->imageDataOrigin) << endl;
 //   cout << "im params: w= " << im->width << ", h= " << im->height << ", nchannels = " << im->nChannels << ", depth = " << im->depth << ", width step = " << im->widthStep << "imageSize = " << im->imageSize << endl;

    //IplImage *imout = cvCloneImage(im);
   // cout << "image data set; cloning image, imsize = " << cvGetSize(im).width << " x " << cvGetSize(im).height << endl;
    IplImage *imout = cvCreateImage(cvSize(imheader.width, imheader.height), imheader.depth, imheader.nChannels);
    assert(imout != NULL);
   // cout << "imout params: w= " << imout->width << ", h= " << imout->height << ", nchannels = " << imout->nChannels << ", depth = " << imout->depth << ", width step = " << imout->widthStep << "imageSize = " << imout->imageSize << endl;

    cvCopyImage(&imheader, imout);
 //   cout << "freeing memory" << endl;
    free(data);
   
  //  cout << "returning" << endl;
    return imout;

}
*/


IplImage readIplImageHeader112 (std::ifstream& is, int &nsize) {
    IplImage112 im;
    IplImage imout;
    is.read((char *) &im, sizeof(im));
    imout.ID = im.ID;
    imout.align = im.align;
    imout.alphaChannel = im.alphaChannel;
    imout.dataOrder = im.dataOrder;
    imout.depth = im.depth;
    imout.height = im.height;
    imout.imageSize = im.imageSize;
    imout.nChannels = im.nChannels;
    imout.nSize = sizeof(imout);
    nsize = im.nSize;
    imout.origin = im.origin;
    imout.width = im.width;
    imout.widthStep = im.widthStep;
    for (int j = 0; j < 4; ++j) {
        imout.BorderConst[j] = im.BorderConst[j];
        imout.BorderMode[j] = im.BorderMode[j];
        imout.channelSeq[j] = im.channelSeq[j];
        imout.colorModel[j] = im.colorModel[j];
    }
    imout.imageData = NULL;
    imout.imageDataOrigin = NULL;
    imout.imageId = NULL;
    imout.maskROI = NULL;
    imout.roi = NULL;
    imout.tileInfo = NULL;
    return imout;
}
IplImage readIplImageHeader136 (std::ifstream& is, int &nsize) {
    IplImage136 im;
    IplImage imout;
    is.read((char *) &im, sizeof(im));
    imout.ID = im.ID;
    imout.align = im.align;
    imout.alphaChannel = im.alphaChannel;
    imout.dataOrder = im.dataOrder;
    imout.depth = im.depth;
    imout.height = im.height;
    imout.imageSize = im.imageSize;
    imout.nChannels = im.nChannels;
    imout.nSize = sizeof(imout);
    nsize = im.nSize;
    imout.origin = im.origin;
    imout.width = im.width;
    imout.widthStep = im.widthStep;
    for (int j = 0; j < 4; ++j) {
        imout.BorderConst[j] = im.BorderConst[j];
        imout.BorderMode[j] = im.BorderMode[j];
        imout.channelSeq[j] = im.channelSeq[j];
        imout.colorModel[j] = im.colorModel[j];
    }
    imout.imageData = NULL;
    imout.imageDataOrigin = NULL;
    imout.imageId = NULL;
    imout.maskROI = NULL;
    imout.roi = NULL;
    imout.tileInfo = NULL;
    return imout;
}

IplImageLoaderFixedWidth::IplImageLoaderFixedWidth() {
}

IplImageLoaderFixedWidth::IplImageLoaderFixedWidth(const IplImageLoaderFixedWidth& orig) {
}

IplImageLoaderFixedWidth::~IplImageLoaderFixedWidth() {
}

