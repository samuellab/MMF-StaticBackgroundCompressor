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
#include "BackgroundRemovedImageLoader.h"
#include "StackReader.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

StaticBackgroundCompressor::StaticBackgroundCompressor() {
    background = NULL;
    bwbuffer = NULL;
    buffer1 = buffer2 = NULL;
    threshAboveBackground = threshBelowBackground = 0;
    smallDimMinSize = lgDimMinSize = 1;
    updateBackgroundFrameInterval = -1;
    updateCount = 0;
    imOrigin.x = imOrigin.y = 0;
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
  //  cout << "size of bri is " << bri.size() << "\n";
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
    addFrame(&imcpy, metadata);
}

void StaticBackgroundCompressor::addFrame(IplImage** im, ImageMetaData* metadata) {
    imsToProcess.insert(imsToProcess.begin(), InputImT(*im,metadata));
    if (updateBackgroundFrameInterval > 0 && updateCount == 0) {
        updateBackground(*im);
        updateCount = updateBackgroundFrameInterval;
    }
    --updateCount;
    *im = NULL;
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
    BackgroundRemovedImage *brim = new BackgroundRemovedImage(im, background, threshBelowBackground, threshAboveBackground, smallDimMinSize, lgDimMinSize,  metadata);
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
    HeaderInfoT hi;
    hi.idcode = idCode();
    hi.numframes = bri.size();
    hi.headerSize = headerSizeInBytes;

    std::ofstream::pos_type start_loc = os.tellp();
    char zero[headerSizeInBytes] = {0};
    os.write(zero, headerSizeInBytes);
    cvResetImageROI(background);
    background->roi = NULL;
    writeIplImageToByteStream(os, background);
    for (vector<BackgroundRemovedImage *>::iterator it = bri.begin(); it != bri.end(); ++it) {
        (*it)->toDisk(os);
    }
    std::ofstream::pos_type end_loc = os.tellp();
    hi.totalSize = end_loc - start_loc;
    os.seekp(start_loc);
    os.write((char *) &hi, sizeof(hi));
    os.seekp(end_loc);
}

std::string StaticBackgroundCompressor::saveDescription() {
  //  cout << "entered sbc save description\n";
    std::stringstream os;
    os << "Stack of common background images, beginning with this header:\n" << headerDescription();
 //   cout << "Stack of common background images, beginning with this header:\n" << headerDescription();
    os << "Then the background image, as an IplImage, starting with the " << sizeof (IplImage) << " byte image header, followed by the image data\n";
//    cout << "Then the background image, as an IplImage, starting with the " << sizeof (IplImage) << " byte image header, followed by the image data\n";
    os << "Then nframes background removed images containing only differences from the background, in this format:\n";
//    cout << "Then nframes background removed images containing only differences from the background, in this format:\n";
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
  //  cout << "ended sbc save description\n";
    return os.str();
}
std::string StaticBackgroundCompressor::headerDescription() {
    std::stringstream os;
    os << headerSizeInBytes << " byte zero-padded header, with the following fields (all " << sizeof(int) << " byte ints, except idcode):\n";
    os << sizeof(unsigned long) << " byte unsigned long idcode = " << hex << idCode() << dec << ", header size in bytes, total size of stack on disk, nframes: number of images in stack\n";
    return os.str();
}

StaticBackgroundCompressor::HeaderInfoT StaticBackgroundCompressor::getHeaderInfo(std::ifstream& is) {
    std::ifstream::pos_type start_loc = is.tellg();
    HeaderInfoT hi;
    is.read((char *) &hi, sizeof(hi));

    is.seekg(start_loc);
    return hi;
}

StaticBackgroundCompressor * StaticBackgroundCompressor::fromDisk(std::ifstream& is) {
    std::ifstream::pos_type start_loc = is.tellg();
    HeaderInfoT hi;

    hi = getHeaderInfo(is);
   // cout << "header size is " << hi.headerSize << "  id code is " << hex << hi.idcode <<dec<< "  numframes = " << hi.numframes << std::endl;
    is.seekg(start_loc + (std::ifstream::pos_type) hi.headerSize);
    StaticBackgroundCompressor *sbc = new StaticBackgroundCompressor();
 //   cout << "reading background" << endl;
    sbc->background = readIplImageFromByteStream(is);
//    cout << "background read in" << endl;
    for (int j = 0; j < hi.numframes; ++j) {
        //BackgroundRemovedImage *bri = BackgroundRemovedImage::fromDisk(is, sbc->background);
        BackgroundRemovedImage *bri = BackgroundRemovedImageLoader::fromFile(is, sbc->background);
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
  //  bool resetroi = false;
   // CvRect roi;
    int cloc = os.tellp();

    /*
    if (src->roi != NULL) {
        roi = cvGetImageROI(src);
        cvResetImageROI(src);
        src->roi = NULL;
        resetroi = true;
    }
*/
    os.write((char *) src, sizeof(IplImage));
    os.write((char *) src->imageData, src->imageSize);

    //if (resetroi) {
      //  cvSetImageROI(src, roi);
    //}
 }

IplImage * StaticBackgroundCompressor::readIplImageFromByteStream(std::ifstream& is) {
    IplImage *im = (IplImage *) malloc(sizeof(IplImage));
    is.read((char *) im, sizeof(IplImage));
    im->roi = NULL;
  //  cout << "im params: w= " << im->width << ", h= " << im->height << ", nchannels = " << im->nChannels << ", depth = " << im->depth << ", width step = " << im->widthStep << "imageSize = " << im->imageSize << endl;
   // cout << "hmm..." << endl;
  //  cout << "imsize = " << cvGetSize(im).width << " x " << cvGetSize(im).height << endl;
    char *data = (char *) malloc(im->imageSize);
   // cout << "data = " << ((unsigned long) data) << endl;
  //  cout << "memory allocated , imsize = " << cvGetSize(im).width << " x " << cvGetSize(im).height << endl;
    is.read(data, im->imageSize);
   // cout << "data read in; setting image data , imsize = " << cvGetSize(im).width << " x " << cvGetSize(im).height << endl;
    
    cvSetData(im, data, im->widthStep);
  //  cout << "image data = " << ((unsigned long) im->imageData) << ", image data origin = " << ((unsigned long) im->imageDataOrigin) << endl;
 //   cout << "im params: w= " << im->width << ", h= " << im->height << ", nchannels = " << im->nChannels << ", depth = " << im->depth << ", width step = " << im->widthStep << "imageSize = " << im->imageSize << endl;
   
    //IplImage *imout = cvCloneImage(im);
   // cout << "image data set; cloning image, imsize = " << cvGetSize(im).width << " x " << cvGetSize(im).height << endl;
    IplImage *imout = cvCreateImage(cvSize(im->width, im->height), im->depth, im->nChannels);
    assert(imout != NULL);
   // cout << "imout params: w= " << imout->width << ", h= " << imout->height << ", nchannels = " << imout->nChannels << ", depth = " << imout->depth << ", width step = " << imout->widthStep << "imageSize = " << imout->imageSize << endl;

    cvCopyImage(im, imout);
 //   cout << "freeing memory" << endl;
    free(data);
    free(im);
  //  cout << "returning" << endl;
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

void StaticBackgroundCompressor::reconstructFrame(int frameNum, IplImage** dst) {
    if (frameNum < 0 || frameNum >= bri.size()) {
        if (*dst != NULL) {
            cvReleaseImage(dst);
        }
        *dst = NULL;
        return;
    }
    BackgroundRemovedImage *brim = bri.at(frameNum);
    brim->restoreImage(dst);
}

/*
const IplImage *StaticBackgroundCompressor::getBackground() {
    return this->background;
}
 * */
void StaticBackgroundCompressor::copyBackground(IplImage** dst) {
    if (dst == NULL) {
        return;
    }
    if (background == NULL) {
        if (*dst != NULL) {
            cvReleaseImage(dst);
            *dst = NULL;
        }
        return;
    }
    setImageOriginFromBRI();
    if (*dst == NULL || (*dst)->width != background->width + imOrigin.x || (*dst)->height != background->height + imOrigin.y || (*dst)->depth != background->depth || (*dst)->nChannels != background->nChannels) {
        if (*dst != NULL) {
            cvReleaseImage(dst);
        }
        *dst = cvCreateImage(cvSize(background->width + imOrigin.x, background->height+imOrigin.y), background->depth, background->nChannels);
    }
    cvSetZero(*dst);
    CvRect r; r.x = imOrigin.x; r.y = imOrigin.y; r.width = background->width; r.height = background->height;
    CvRect roi = cvGetImageROI(*dst);
    cvSetImageROI(*dst, r);
    cvCopyImage(background, *dst);
    cvSetImageROI(*dst, roi);
}

void StaticBackgroundCompressor::annotatedFrame(int frameNum, IplImage** buffer, IplImage** annotatedImage) {
    reconstructFrame(frameNum, buffer);
    if (*buffer == NULL) {
        if (*annotatedImage != NULL) {
            cvReleaseImage(annotatedImage);        
            *annotatedImage = NULL;
            return;
        }
    }
    if (*annotatedImage == NULL || (*annotatedImage)->width != (*buffer)->width || (*annotatedImage)->height != (*buffer)->height) {
        if (*annotatedImage != NULL) {
            cvReleaseImage(annotatedImage);
        }
        *annotatedImage = cvCreateImage(cvGetSize(*buffer), (*buffer)->depth, 3);
    }
    cvConvertImage(*buffer, *annotatedImage,0);

    BackgroundRemovedImage *brim = bri.at(frameNum);
    brim->annotateImage(*annotatedImage);
}

const ImageMetaData *StaticBackgroundCompressor::getMetaData(int frameNumber) {
    BackgroundRemovedImage *brim = bri.at(frameNumber);
    if (brim == NULL) {
        return NULL;
    }
    return brim->getMetaData();
}

int StaticBackgroundCompressor::numRegionsInFrame(int frameNum) const {
    if (frameNum < 0 || frameNum >= bri.size()) {
        return -1;
    }
    const BackgroundRemovedImage *brim = bri.at(frameNum);
    return brim->numRegions();
}

void StaticBackgroundCompressor::setImageOriginFromBRI() {
    if (bri.empty()) {
        return;
    }
    imOrigin = bri.front()->getImageOrigin();
}

CvSize StaticBackgroundCompressor::getFrameSize() {
    setImageOriginFromBRI();
    if (background == NULL) {
        return cvSize(0,0);
    }
    CvSize sz = cvGetSize(background);
   // cout << endl << "sz.width = " << sz.width << ", sz.height = " << sz.height << endl;
    IplImage *im = background;
   // cout << "background params: w= " << im->width << ", h= " << im->height << ", nchannels = " << im->nChannels << ", width step = " << im->widthStep << "imageSize = " << im->imageSize << endl;

    sz.width += imOrigin.x;
    sz.height += imOrigin.y;
    //cout << endl << "imorigin.x= " << imOrigin.x << ", imorigin.y= " << imOrigin.y << ", sz.width = " << sz.width << ", sz.height = " << sz.height << endl;
    return sz;
}

CvRect StaticBackgroundCompressor::getValidRoi() {
    CvRect r;
    r.x = r.y = r.width = r.height = 0;
    if (background == NULL) {
        return r;
    }
    setImageOriginFromBRI();
    CvSize sz = cvGetSize(background);
    r.x = imOrigin.x; r.y = imOrigin.y; r.width = sz.width; r.height = sz.height;
    return r;
}