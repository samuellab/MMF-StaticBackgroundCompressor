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
#include <sstream>

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
  //  cout << "size of imsToProcess is " << imsToProcess.size() << "\n";
    for (vector<InputImT>::iterator it = imsToProcess.begin(); it != imsToProcess.end(); ++it) {
        IplImage *im = it->first;
        cvReleaseImage(&im);
        if (it->second != NULL) {
            delete it->second;
        }
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
   for (vector<InputImT>::iterator it = imsToProcess.begin(); it != imsToProcess.end(); ++it) {
   //     cvMin(*it, background, background);
       updateBackground(it->first);
    }
}
void StaticBackgroundCompressor::updateBackground(const IplImage* im) {
    if (background == NULL) {
        background = cvCloneImage(im);
    } else {
        cvMin(im, background, background);
    }
}


void StaticBackgroundCompressor::addFrame(const IplImage* im, ImageMetaData *metadata) {
    IplImage *imcpy = cvCloneImage(im);
    imsToProcess.insert(imsToProcess.begin(), InputImT(imcpy,metadata));
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
    InputImT nextim = imsToProcess.back();
//    IplImage *im = imsToProcess.back();
    imsToProcess.pop_back();
    IplImage *im = nextim.first;
    ImageMetaData *metadata = nextim.second;
    BackgroundRemovedImage *brim = new BackgroundRemovedImage(im, background, bwbuffer, buffer1, buffer2, threshBelowBackground, threshAboveBackground, metadata);
    bri.push_back(brim);
    cvReleaseImage(&im);
    //NB: we do NOT release metadata storage, as this is now background removed images problem
    return imsToProcess.size();
}

void StaticBackgroundCompressor::processFrames() {
    while (processFrame() > 0) {
        //process Frame does all the work
        ;
    }
}

void StaticBackgroundCompressor::toDisk(std::ofstream& os) {
    int info[3] = {0};

    info[0] = headerSizeInBytes;
    info[2] = bri.size(); //numframes

    std::ofstream::pos_type start_loc = os.tellp();
  //  os.write((char *) info, sizeof(info));
    //fill in rest of header with zeros
    char zero[headerSizeInBytes] = {0};
    os.write(zero, headerSizeInBytes);
    writeIplImageToByteStream(os, background);
    for (vector<BackgroundRemovedImage *>::iterator it = bri.begin(); it != bri.end(); ++it) {
        (*it)->toDisk(os);
    }
    std::ofstream::pos_type end_loc = os.tellp();
    info[1] = end_loc - start_loc;
    os.seekp(start_loc);
    os.write((char *) info, sizeof(info));
    os.seekp(end_loc);
}

std::string StaticBackgroundCompressor::saveDescription() {
//    cout << "entered sbc save description\n";
    std::stringstream os;
    os << "Stack of common background images, beginning with this header:\n" << headerDescription();
 //   cout << "Stack of common background images, beginning with this header:\n" << headerDescription();
    os << "Then the background image, as an IplImage, starting with the " << sizeof (IplImage) << " byte image header, followed by the image data\n";
 //   cout << "Then the background image, as an IplImage, starting with the " << sizeof (IplImage) << " byte image header, followed by the image data\n";
    os << "Then nframes background removed images containing only differences from the background, in this format:\n";
 //   cout << "Then nframes background removed images containing only differences from the background, in this format:\n";
    if (bri.empty()) {
        os << "<no background removed images in stack>\n";
    } else {
 //       cout << "bri.front = " << (int) bri.front();
        if (bri.front() == NULL) {
            os << "<background removed image is a NULL pointer>\n";
        } else {
            os << bri.front()->saveDescription();
        }
    }
//    cout << "ended sbc save description\n";
    return os.str();
}
std::string StaticBackgroundCompressor::headerDescription() {
    std::stringstream os;
    os << headerSizeInBytes << " byte zero-padded header, with the following fields (all " << sizeof(int) << " byte ints):\n";
    os << "header size in bytes, total size of stack on disk, nframes: number of images in stack\n";
    return os.str();
}

StaticBackgroundCompressor * StaticBackgroundCompressor::fromDisk(std::ifstream& is) {
    std::ifstream::pos_type start_loc = is.tellg();
    int info[3];
    is.read((char *) info, sizeof(info));
    int numFrames = info[2];
    int headerSize = info[0];
    is.seekg(start_loc + (std::ifstream::pos_type) headerSize);
    StaticBackgroundCompressor *sbc = new StaticBackgroundCompressor();
    sbc->background = readIplImageFromByteStream(is);
  
    for (int j = 0; j < numFrames; ++j) {
        BackgroundRemovedImage *bri = BackgroundRemovedImage::fromDisk(is, sbc->background);
        sbc->bri.push_back(bri);
    }
    return sbc;
}

int StaticBackgroundCompressor::sizeOnDisk() {
    
   int totalBytes = headerSizeInBytes + ((background != NULL) ? sizeof(IplImage) + background->imageSize : 0);
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
