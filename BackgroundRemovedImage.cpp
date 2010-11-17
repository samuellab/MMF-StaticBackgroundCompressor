/* 
 * File:   BackgroundRemovedImage.cpp
 * Author: Marc
 * 
 * Created on October 4, 2010, 9:17 AM
 */

#include "BackgroundRemovedImage.h"
#include "ImageMetaDataLoader.h"
#include "cv.h"
#include "cxcore.h"
#include "cvtypes.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

using namespace std;

static void writeImageData (ofstream &os, IplImage *im);
static IplImage *readImageData (ifstream &is, int width, int height, int depth, int nChannels);
static ofstream logkludge("c:\\brilogstream.txt");
BackgroundRemovedImage::BackgroundRemovedImage() {
    init();
}

BackgroundRemovedImage::BackgroundRemovedImage(const BackgroundRemovedImage& orig) {
}

BackgroundRemovedImage::~BackgroundRemovedImage() {
    vector<pair<CvRect, IplImage *> >::iterator it;
    for (it = differencesFromBackground.begin(); it != differencesFromBackground.end(); ++it) {
        cvReleaseImage(&(it->second));
    }
    if (ms != NULL) {
        cvReleaseMemStorage(&ms);
    }
    if (metadata != NULL) {
        delete metadata;
    }
}

BackgroundRemovedImage::BackgroundRemovedImage(IplImage* src, const IplImage* bak, int threshBelowBackground, int threshAboveBackground, int smallDimMinSize, int lgDimMinSize, ImageMetaData* metadata) {
    init();
    //source and background must both be single channel arrays
    assert (src != NULL);
    assert (bak != NULL);
    assert (src->width == bak->width);
    assert (src->height == bak->height);
    assert (src->depth == bak->depth);
    assert (src->nChannels == 1);
    assert (bak->nChannels == 1);

    

    //update later, but for now just set memstorage to null, so it will be allocated on need
    ms = NULL;
    this->metadata = metadata;
    backgroundIm = bak;
    this->threshAboveBackground = threshAboveBackground;
    this->threshBelowBackground = threshBelowBackground;
    this->lgDimMinSize = lgDimMinSize;
    this->smallDimMinSize = smallDimMinSize;
    // logkludge << "entered extract differences" << endl<< flush;
    extractDifferences(src);
    // logkludge << "exited extrac differences" << endl<<flush;
}

void BackgroundRemovedImage::init() {
    ms = NULL;
    backgroundIm = NULL;
    metadata = NULL;
    threshAboveBackground = threshBelowBackground = 0;
    smallDimMinSize = 2;
    lgDimMinSize = 3;
}


void BackgroundRemovedImage::extractDifferences(IplImage* src) {
  //  bool bwfreewhendone = (bwbuffer == NULL || bwbuffer->width != src->width || bwbuffer->height != src->height || bwbuffer->depth != IPL_DEPTH_8U);
    IplImage *bwbuffer = NULL, *srcbuffer1 = NULL, *srcbuffer2 = NULL;
    //allocate storage if necessary
    bwbuffer = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    srcbuffer1 = cvCreateImage(cvGetSize(src), src->depth, 1);
    srcbuffer2 = cvCreateImage(cvGetSize(src), src->depth, 1);
    
    if (src->roi != NULL) {
        cvSetImageROI(bwbuffer, cvGetImageROI(src));
        if (srcbuffer1 != NULL) {
             cvSetImageROI(srcbuffer1, cvGetImageROI(src));
        }
        if (srcbuffer2 != NULL) {
             cvSetImageROI(srcbuffer2, cvGetImageROI(src));
        }
    }
    // logkludge << "test point 1 in extract differences" << endl << flush;
    //compare image to background and find differences
    if (threshAboveBackground <= 0 && threshBelowBackground <= 0) {
        cvCmp (src, backgroundIm, bwbuffer, CV_CMP_NE);
    } else {
        //turn < bak into < bak + 1 which approximates <= bax
        threshAboveBackground = threshAboveBackground < 0 ? 0 : threshAboveBackground;
        threshBelowBackground = threshBelowBackground < 0 ? 0 : threshBelowBackground;
        cvAddS (backgroundIm, cvScalarAll(threshAboveBackground + 1), srcbuffer2);
        cvSubS (backgroundIm, cvScalarAll(threshBelowBackground), srcbuffer1);
        
        cvInRange(src, srcbuffer1, srcbuffer2, bwbuffer);
        cvNot(bwbuffer, bwbuffer);
    }
    logkludge << cvSumPixels(bwbuffer) << "nonzero pixels" <<endl;

    // logkludge <<  "test point 2 in extract differences" << endl << flush;
    //turn those differences into mini images
    extractBlobs (src, bwbuffer);

     // logkludge <<  "test point 3 in extract differences" << endl << flush;


    cvReleaseImage(&bwbuffer);
    cvReleaseImage(&srcbuffer1);
    cvReleaseImage(&srcbuffer2);
    
}

void BackgroundRemovedImage::extractBlobs(IplImage *src, IplImage *mask) {
    bool freems = (ms == NULL);
    if (freems) {
        ms = cvCreateMemStorage(src->width*src->depth); //big block to save having to reallocate later
    }
    logkludge << "created memstorage at " << (unsigned long) ms << endl;
    CvSeq *contour;
    cvSetImageROI(mask, cvGetImageROI(src));
    cvFindContours(mask, ms, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    CvPoint offset = (mask->roi == NULL) ? cvPoint(0,0) : cvPoint(mask->roi->xOffset, mask->roi->yOffset);
    logkludge << "contour = " << (unsigned long) contour << endl;

    IplImage *copy;
    CvRect roi = cvGetImageROI(src);
    if (contour != NULL) {
        CvRect r = cvBoundingRect(contour, 0);
        
       // subIm = cvCreateImageHeader(cvSize(r.width, r.height), src->depth, src->nChannels);
        for ( ; contour != NULL; contour = contour->h_next) {
            r = cvBoundingRect(contour, 0);
            if (r.width >= smallDimMinSize && r.height >= smallDimMinSize && (r.width >= lgDimMinSize || r.height >= lgDimMinSize)) {
                cvSetImageROI(src, r);
                copy = cvCreateImage(cvSize(r.width, r.height), src->depth, src->nChannels);
                cvCopy(src, copy);
                r.x += offset.x;
                r.y += offset.y;
                differencesFromBackground.push_back(pair<CvRect, IplImage *> (r, copy));
            }
        }
    }
    if (freems) {
        cvReleaseMemStorage(&ms);
        ms = NULL;
    }
    cvSetImageROI(src, roi);
}

void BackgroundRemovedImage::toDisk(std::ofstream &os) {
    writeHeader(os);
    vector<pair<CvRect, IplImage *> >::iterator it;
    for (it = differencesFromBackground.begin(); it != differencesFromBackground.end(); ++it) {
        os.write((char *) &(it->first), sizeof(CvRect));
        writeImageData(os, it->second);
    }
}

void BackgroundRemovedImage::writeHeader(std::ofstream& os) {

    HeaderInfoT hi;

    hi.headersize = headerSizeInBytes;
    hi.idcode = idCode();


    //fill in header with 0s
    std::ofstream::pos_type cur_loc = os.tellp();
    char zero[headerSizeInBytes] = {0};
    os.write(zero, headerSizeInBytes);
    std::ofstream::pos_type end_loc = os.tellp();

    //return to the beginning of the header and write data
    os.seekp(cur_loc);

    if (differencesFromBackground.empty()) {
        hi.depth = 0;
        hi.nchannels = 0;
        hi.numims = 0;
    } else {
        hi.depth = differencesFromBackground.front().second->depth;
        hi.nchannels = differencesFromBackground.front().second->nChannels;
        hi.numims = differencesFromBackground.size();
    }
    os.write((char *) &hi, sizeof(hi));
    if (metadata != NULL) {
        metadata->toDisk(os);
    }
    os.seekp(end_loc);
    
}

int BackgroundRemovedImage::sizeOnDisk() {
    int totalbytes = headerSizeInBytes;
    for (vector<pair<CvRect, IplImage *> >::iterator it = differencesFromBackground.begin(); it != differencesFromBackground.end(); ++it) {
        IplImage *im = it->second;
        totalbytes += sizeof(CvRect) + bytesPerPixel(im)*im->width*im->height*im->nChannels;
    }
    return totalbytes;
}

//does not include background image or memstorage
int BackgroundRemovedImage::sizeInMemory() {
    int totalbytes = sizeof(this);
    for (vector<pair<CvRect, IplImage *> >::iterator it = differencesFromBackground.begin(); it != differencesFromBackground.end(); ++it) {
        IplImage *im = it->second;
        totalbytes += sizeof(CvRect) + sizeof(IplImage) + im->imageSize;
    }
    return totalbytes;
}
void writeImageData (ofstream &os, IplImage *im) {
    //if the width and widthstep are the same, we can just dump all the data
    if (im->width*im->nChannels*BackgroundRemovedImage::bytesPerPixel(im) == im->widthStep) {
        os.write(im->imageData, im->imageSize);
    } else {
        //otherwise copy the image data to a contiguous block of memory, then dump
        int bytes_per_row = im->width*im->nChannels*BackgroundRemovedImage::bytesPerPixel(im);
        char *buffer = (char *) malloc(bytes_per_row * im->height);
        for (int row = 0; row < im->height; ++row) {
            memcpy(buffer + row * bytes_per_row, im->imageDataOrigin + row*im->widthStep, bytes_per_row);
        }
        os.write(buffer, im->height * bytes_per_row);
        free(buffer);
    }
}

IplImage *readImageData (ifstream &is, int width, int height, int depth, int nChannels) {
    IplImage *im = cvCreateImage(cvSize(width, height), depth, nChannels);
    assert (im != NULL);
    if (im->width*im->nChannels*BackgroundRemovedImage::bytesPerPixel(im) == im->widthStep) {
        is.read(im->imageData, im->imageSize);
    } else {
         //otherwise copy the image data to a contiguous block of memory, then dump
        int bytes_per_row = im->width*im->nChannels*BackgroundRemovedImage::bytesPerPixel(im);
        char *buffer = (char *) malloc(bytes_per_row * im->height);
        is.read(buffer, im->height * bytes_per_row);
        for (int row = 0; row < im->height; ++row) {
            memcpy(im->imageDataOrigin + row*im->widthStep, buffer + row * bytes_per_row, bytes_per_row);
        }
        
        free(buffer);
    }
    return im;
    
}

BackgroundRemovedImage *BackgroundRemovedImage::fromDisk(std::ifstream& is, const IplImage *bak) {
    BackgroundRemovedImage *bri = new BackgroundRemovedImage();
    bri->backgroundIm = bak;
    bri->threshAboveBackground = bri->threshBelowBackground = 0;
    bri->ms = NULL;
   
    std::ifstream::pos_type cur_loc = is.tellg();

    HeaderInfoT hi;
    is.read((char *) &hi, sizeof(hi));
    if (hi.idcode != bri->idCode()) {
        cout << "WARNING: id code does not match when loading BackgroundRemovedImage";
    }
    bri->metadata = ImageMetaDataLoader::fromFile(is);
    is.seekg(cur_loc + (std::ifstream::pos_type) hi.headersize);

    for (int j = 0; j < hi.numims; ++j) {
        CvRect r;
        is.read((char *) &r, sizeof(CvRect));
        IplImage *im = readImageData(is, r.width, r.height, hi.depth, hi.nchannels);
        bri->differencesFromBackground.push_back(pair<CvRect, IplImage *> (r, im));
    }
    return bri;
}

void BackgroundRemovedImage::restoreImage(IplImage** dst) {
    if (backgroundIm == NULL) {
        return;
    }
    assert(dst != NULL);
    if (*dst == NULL || (*dst)->width != backgroundIm->width ||  (*dst)->height != backgroundIm->height ||
             (*dst)->nChannels != backgroundIm->nChannels ||  (*dst)->depth != backgroundIm->depth) {
        if (*dst != NULL) {
            cvReleaseImage(dst);
        }
        *dst = cvCloneImage(backgroundIm);
    } else {
        cvCopyImage(backgroundIm, *dst);
    }
    CvRect roi = cvGetImageROI(*dst);
    vector< pair<CvRect, IplImage *> >::iterator it;
    for (it = differencesFromBackground.begin(); it != differencesFromBackground.end(); ++it) {
        cvSetImageROI(*dst, it->first);
        cvCopyImage(it->second, *dst);
    }
    cvSetImageROI(*dst, roi);
    
}

void BackgroundRemovedImage::annotateImage(IplImage* dst, CvScalar color, int thickness) {
    assert(dst != NULL);
    for (vector< pair<CvRect, IplImage *> >::iterator it = differencesFromBackground.begin(); it != differencesFromBackground.end(); ++it) {
        CvRect r = it->first;
        cvRectangle(dst, cvPoint(r.x, r.y), cvPoint(r.x + r.width, r.y + r.height), color, thickness, 8, 0);
    }
}

std::string BackgroundRemovedImage::saveDescription() {
 //   cout << "entered bri save description\n";
    std::stringstream os;
    os << classname() << ": header is a" << headerDescription() << "header is followed by numims image blocks of the following form:\n";
    os << "(" << sizeof(CvRect) << " bytes) CvRect [x y w h] describing location of image data, then interlaced row ordered image data\n";
    return os.str();
 //   cout << "ended bri save description\n";
}

std::string BackgroundRemovedImage::headerDescription() {
    std::stringstream os;
    os << headerSizeInBytes << " byte zero padded header with the following data fields (all " << sizeof(int) << " byte ints, except id code)\n";
    os << sizeof(unsigned long) << " byte unsigned long idcode = " << hex << idCode() << dec << "headersize (number of bytes in header), depth (IplImage depth), nChannels (IplImage number of channels), numims (number of image blocks that differ from background) then metadata:\n";
    if (metadata != NULL) {
 //       cout << metadata->saveDescription();
        os << metadata->saveDescription();
    } else {
        os << "no metadata\n";
    }
    return os.str();
}

int BackgroundRemovedImage::numRegions() const {
    return differencesFromBackground.size();
}