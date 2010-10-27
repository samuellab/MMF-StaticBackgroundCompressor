/* 
 * File:   BackgroundRemovedImage.cpp
 * Author: Marc
 * 
 * Created on October 4, 2010, 9:17 AM
 */

#include "BackgroundRemovedImage.h"

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

BackgroundRemovedImage::BackgroundRemovedImage(IplImage* src, const IplImage* bak, IplImage* bwbuffer, IplImage* srcbuffer1, IplImage* srcbuffer2, int threshBelowBackground, int threshAboveBackground, ImageMetaData *metadata) {

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
    
    extractDifferences(src, bwbuffer, srcbuffer1, srcbuffer2);
}

void BackgroundRemovedImage::init() {
    ms = NULL;
    backgroundIm = NULL;
    metadata = NULL;
    threshAboveBackground = threshBelowBackground = 0;
    
}


void BackgroundRemovedImage::extractDifferences(IplImage* src, IplImage* bwbuffer, IplImage* srcbuffer1, IplImage* srcbuffer2) {
    bool bwfreewhendone = (bwbuffer == NULL || bwbuffer->width != src->width || bwbuffer->height != src->height || bwbuffer->depth != IPL_DEPTH_8U);

    //allocate storage if necessary
    if (bwfreewhendone) {
        bwbuffer = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    }
    bool src1freewhendone = (threshBelowBackground > 0 && (srcbuffer1 == NULL || srcbuffer1->width != src->width || srcbuffer1->height != src->height || srcbuffer1->depth != src->depth));
    if (src1freewhendone) {
        srcbuffer1 = cvCreateImage(cvGetSize(src), src->depth, 1);
    }
    bool src2freewhendone = ((threshBelowBackground > 0 || threshAboveBackground > 0) && (srcbuffer2 == NULL || srcbuffer2->width != src->width || srcbuffer2->height != src->height || srcbuffer2->depth != src->depth));
    if (src2freewhendone) {
        srcbuffer2 = cvCreateImage(cvGetSize(src), src->depth, 1);
    }

    if (src->roi != NULL) {
        cvSetImageROI(bwbuffer, cvGetImageROI(src));
        if (srcbuffer1 != NULL) {
             cvSetImageROI(srcbuffer1, cvGetImageROI(src));
        }
        if (srcbuffer2 != NULL) {
             cvSetImageROI(srcbuffer2, cvGetImageROI(src));
        }
    }

    //compare image to background and find differences
    if (threshAboveBackground <= 0 && threshBelowBackground <= 0) {
        cvCmp (src, backgroundIm, bwbuffer, CV_CMP_NE);
    } else {
        //turn < bak into < bak + 1 which approximates <= bax
        cvAddS (backgroundIm, cvScalarAll(threshAboveBackground + 1), srcbuffer2);
        if (threshBelowBackground > 0) {
            cvSubS(backgroundIm, cvScalarAll(threshBelowBackground), srcbuffer1);
        } else {
            srcbuffer1 = const_cast <IplImage *> (backgroundIm);
            src1freewhendone = false;
        }
        cvInRange(src, srcbuffer1, srcbuffer2, bwbuffer);
        cvNot(bwbuffer, bwbuffer);
    }

    //turn those differences into mini images
    extractBlobs (src, bwbuffer);

    if (bwfreewhendone) {
        cvReleaseImage(&bwbuffer);
    }
    if (src1freewhendone) {
        cvReleaseImage(&srcbuffer1);
    }
    if (src2freewhendone) {
        cvReleaseImage(&srcbuffer2);
    }
    
}

void BackgroundRemovedImage::extractBlobs(IplImage *src, IplImage *mask) {
    bool freems = (ms == NULL);
    if (freems) {
        ms = cvCreateMemStorage(src->width*src->depth); //big block to save having to reallocate later
    }
    CvSeq *contour;
    cvSetImageROI(mask, cvGetImageROI(src));
    cvFindContours(mask, ms, &contour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    CvPoint offset = (mask->roi == NULL) ? cvPoint(0,0) : cvPoint(mask->roi->xOffset, mask->roi->yOffset);

    IplImage *copy;
    CvRect r = cvBoundingRect(contour, 0);
    CvRect roi = cvGetImageROI(src);
   // subIm = cvCreateImageHeader(cvSize(r.width, r.height), src->depth, src->nChannels);
    for ( ; contour != NULL; contour = contour->h_next) {
        r = cvBoundingRect(contour, 0);
        cvSetImageROI(src, r);
        copy = cvCreateImage(cvSize(r.width, r.height), src->depth, src->nChannels);
        cvCopy(src, copy);
        r.x += offset.x;
        r.y += offset.y;
        differencesFromBackground.push_back(pair<CvRect, IplImage *> (r, copy));
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

    //fill in header with 0s
    std::ofstream::pos_type cur_loc = os.tellp();
    char zero[headerSizeInBytes] = {0};
    os.write(zero, headerSizeInBytes);
    std::ofstream::pos_type end_loc = os.tellp();

    //return to the beginning of the header and write data
    os.seekp(cur_loc);
    int info[4] = {0}; //header size, depth, nchannels, npoints
    info[0] = headerSizeInBytes;
    if (differencesFromBackground.empty()) {
        os.write((char *) info, sizeof(info));
    } else {
        info[1] = differencesFromBackground.front().second->depth;
        info[2] = differencesFromBackground.front().second->nChannels;
        info[3] = differencesFromBackground.size();
        os.write((char *) info, sizeof(info));
    }
    if (metadata != NULL) {
     //   cout << "trying to write metadata\n";
        metadata->toDisk(os);
  //      cout << "finished writing metadata\n";
    }
    //go back to the end of the header
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
    int depth, nChannels, numims;
    std::ifstream::pos_type cur_loc = is.tellg();
    int headersize;
    is.read((char *) &headersize, sizeof(int));
    is.read((char *) &depth, sizeof(int));
    is.read((char *) &nChannels, sizeof(int));
    is.read((char *) &numims, sizeof(int));
    is.seekg(cur_loc + (std::ifstream::pos_type) headersize);

    for (int j = 0; j < numims; ++j) {
        CvRect r;
        is.read((char *) &r, sizeof(CvRect));
        IplImage *im = readImageData(is, r.width, r.height, depth, nChannels);
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
    os << headerSizeInBytes << " byte zero padded header with the following data fields (all " << sizeof(int) << " byte ints)\n";
    os << "headersize (number of bytes in header), depth (IplImage depth), nChannels (IplImage number of channels), numims (number of image blocks that differ from background) then metadata:\n";
    if (metadata != NULL) {
 //       cout << metadata->saveDescription();
        os << metadata->saveDescription();
    } else {
        os << "no metadata\n";
    }
    return os.str();
}
