/* 
 * File:   StaticBackgroundCompressor.cpp
 * Author: Marc
 * 
 * Created on October 4, 2010, 1:06 PM
 */

#include "highgui.h"


#include "StaticBackgroundCompressor.h"
#include <vector>
#include "BackgroundRemovedImage.h"
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <fstream>

using namespace std;

StaticBackgroundCompressor::StaticBackgroundCompressor() {
    background = NULL;
    bwbuffer = NULL;
    buffer1 = buffer2 = NULL;
    threshAboveBackground = threshBelowBackground = 0;
    updateBackgroundFrameInterval = -1;
    updateCount = 0;
}

StaticBackgroundCompressor::StaticBackgroundCompressor(const StaticBackgroundCompressor& orig) {
}

StaticBackgroundCompressor::~StaticBackgroundCompressor() {
    cvReleaseImage(&background);
    for (vector<IplImage *>::iterator it = imsToProcess.begin(); it != imsToProcess.end(); ++it) {
        IplImage *im = *it;
        cvReleaseImage(&im);
    }
    for (vector<BackgroundRemovedImage *>::iterator it = bri.begin(); it != bri.end(); ++it) {
        delete (*it);
        *it = NULL;
    }

}

void StaticBackgroundCompressor::calculateBackground() {
    if (imsToProcess.empty()) {
        return;
    }
    if (background != NULL) {
        cvReleaseImage(&background);
    }
 //   background = cvCloneImage(imsToProcess.front());
   for (vector<IplImage *>::iterator it = imsToProcess.begin(); it != imsToProcess.end(); ++it) {
   //     cvMin(*it, background, background);
       updateBackground(*it);
    }
}
void StaticBackgroundCompressor::updateBackground(const IplImage* im) {
    if (background == NULL) {
        background = cvCloneImage(im);
    } else {
        cvMin(im, background, background);
    }
}


void StaticBackgroundCompressor::addFrame(const IplImage* im) {
    IplImage *imcpy = cvCloneImage(im);
    imsToProcess.insert(imsToProcess.begin(), imcpy);
    if (updateBackgroundFrameInterval > 0 && updateCount == 0) {
        updateBackground(im);
        updateCount = updateBackgroundFrameInterval;
    }
    --updateCount;
}

int StaticBackgroundCompressor::processFrame() {
    if (imsToProcess.empty()) {
        return 0;
    }
    if (background == NULL) {
        return -1;
    }
    IplImage *im = imsToProcess.back();
    imsToProcess.pop_back();
    BackgroundRemovedImage *brim = new BackgroundRemovedImage(im, background, bwbuffer, buffer1, buffer2, threshBelowBackground, threshAboveBackground);
    bri.push_back(brim);
    cvReleaseImage(&im);
    return imsToProcess.size();
}

void StaticBackgroundCompressor::processFrames() {
    while (processFrame() > 0) {
        //process Frame does all the work
        ;
    }
}

void StaticBackgroundCompressor::toDisk(std::ofstream& os) {
    writeIplImageToByteStream(os, background);
 
    int numFrames = bri.size();

    os.write((char *) &numFrames, sizeof(int));
    for (vector<BackgroundRemovedImage *>::iterator it = bri.begin(); it != bri.end(); ++it) {
        (*it)->toDisk(os);
    
    }
}
StaticBackgroundCompressor * StaticBackgroundCompressor::fromDisk(std::ifstream& is) {
    StaticBackgroundCompressor *sbc = new StaticBackgroundCompressor();
    sbc->background = readIplImageFromByteStream(is);
    int numFrames;
    is.read((char *) &numFrames, sizeof(int));
  
    for (int j = 0; j < numFrames; ++j) {
        BackgroundRemovedImage *bri = BackgroundRemovedImage::fromDisk(is, sbc->background);
        sbc->bri.push_back(bri);
    }
    return sbc;
}

int StaticBackgroundCompressor::sizeOnDisk() {
    
    int totalBytes = sizeof(int) + ((background != NULL) ? sizeof(IplImage) + background->imageSize : 0);
   for (vector<BackgroundRemovedImage *>::iterator it = bri.begin(); it != bri.end(); ++it) {
        totalBytes += (*it)->sizeOnDisk();
    }
    return totalBytes;
}

void StaticBackgroundCompressor::writeIplImageToByteStream(std::ofstream& os, const IplImage *src) {
    assert(src != NULL);
    int cloc = os.tellp();

    os.write((char *) src, sizeof(IplImage));
    os.write((char *) src->imageData, src->imageSize);
 }

IplImage * StaticBackgroundCompressor::readIplImageFromByteStream(std::ifstream& is) {
    IplImage *im = (IplImage *) malloc(sizeof(IplImage));
    is.read((char *) im, sizeof(IplImage));
    char *data = (char *) malloc(im->imageSize);
    is.read(data, im->imageSize);
    cvSetData(im, data, im->widthStep);
    //IplImage *imout = cvCloneImage(im);
    IplImage *imout = cvCreateImage(cvGetSize(im), im->depth, im->nChannels);
    cvCopyImage(im, imout);
    free(data);
    free(im);

    return imout;
}

void StaticBackgroundCompressor::playMovie(char* windowName) {
    if (windowName == NULL) {
        windowName = "Movie of stack";
    }
    cvNamedWindow(windowName, 0);
    IplImage *im = NULL;
     for (vector<BackgroundRemovedImage *>::iterator it = bri.begin(); it != bri.end(); ++it) {
         (*it)->restoreImage(&im);
         cvShowImage(windowName, im);
         cvWaitKey(50);
     }
}

int StaticBackgroundCompressor::numProcessed() {
    return bri.size();
}

int StaticBackgroundCompressor::numToProccess() {
    return imsToProcess.size();
}