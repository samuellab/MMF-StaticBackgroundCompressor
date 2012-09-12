/* 
 * File:   MultiStackReader.cpp
 * Author: Marc
 * 
 * Created on 9/11/2012
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#include <map>
#include <iostream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#ifdef _WIN32
// Hopefully one day the code may be less windows-centric :)
    #include <windows.h> //required to use multithreaded stack writing
#endif
#include "MultiStackReader.h"
#include "highgui.h"
#include "Timer.h"
#include "StaticBackgroundCompressorLoader.h"
#ifdef _WIN32
    #include "WindowsThreadStackCompressor.h"
#else
    #include "LinearStackCompressor.h"
#endif
using namespace std;

#ifndef _WIN32
    void Sleep (int t);
    void Sleep (int t) {
        cvWaitKey(t);
    }
#endif

MultiStackReader::MultiStackReader() {
    init();
}

MultiStackReader::MultiStackReader(const StackReader& orig) {
}

MultiStackReader::~MultiStackReader() {
    closeInputFile();
}



void MultiStackReader::init() {
    StackReader::init();
    nstacks = 0;
    sr.clear();
    fnames.clear();
    
}



void MultiStackReader::setInputFileName(const char* fname) {
    fnames.clear();
    fnames.push_back(string(fname));
//    this->fname = string(fname);
    
}
void MultiStackReader::setInputFileName(const std::vector<std::string> fnames) {
    this->fnames = fnames;
//    for (vector<string>::iterator it = fnames.begin(); it != fnames.end(); ++it) {
//        this->fnames.push_back(*it);
//    }
//    nstacks = this->fnames.size();
//    checkError();
}

void MultiStackReader::closeInputFile() {
    for (vector<pair<StackReader *, int> >::iterator it = sr.begin(); it != sr.end(); ++it) {
        delete it->first;
        it->first = NULL;
    }
    sr.clear();
    endFrames.clear();
}

void MultiStackReader::openInputFile() {
    closeInputFile();
    checkError(); if (isError()) { return; }
    int totalframes = 0;
    for (vector<string>::iterator it = fnames.begin(); it != fnames.end(); ++ it) {
        StackReader *s = new StackReader(it->c_str());
        sr.push_back(make_pair(s,totalframes));
        totalframes += s->getTotalFrames();
        endFrames.push_back(totalframes);
    }
    totalFrames = totalframes;
    checkError();
}



CvSize MultiStackReader::getImageSize() {
    checkError(); 
    if (sr.empty()) {
        setError("get image size called with no stacks open");
    }
    if (isError()) { return cvSize(0,0); }
    int srnum = findStackReader(sr.front()->second);
    if (srnum < 0 || srnum >= sr.size()) {
        if (!isError()) {
            setError("findStackReader returned bad value");           
        }
        return cvSize(0,0);
    }
    
    return sr.at(srnum).first->getImageSize();

}

void MultiStackReader::getBackground(int frameNum, IplImage** dst, int frameRange) {
    int srnum = findStackReader(frameNum);
    if (srnum < 0 || srnum >= sr.size()) {
        if (!isError()) {
            setError("findStackReader returned bad value");           
        }
        if (*dst != NULL) {
            cvReleaseImage(dst);
            *dst = NULL;
        }
        return;
    }
    sr.at(srnum).first->getBackground(frameNum-sr.at(srnum).second, dst, frameRange);
    checkError();
}

void MultiStackReader::getFrame(int frameNum, IplImage** dst) {
    int srnum = findStackReader(frameNum); checkError();
    if (isError() || srnum < 0 || srnum >= sr.size()) {
        if (!isError()) {
            setError("findStackReader returned bad value");           
        }
        if (*dst != NULL) {
            cvReleaseImage(dst);
            *dst = NULL;
        }
        return;
    }
    sr.at(srnum).first->getFrame(frameNum-sr.at(srnum).second, dst);
    checkError();
}

const ImageMetaData *MultiStackReader::getMetaData(int frameNum) {
    int srnum = findStackReader(frameNum); checkError();
    if (isError() || srnum < 0 || srnum >= sr.size()) {
        if (!isError()) {
            setError("findStackReader returned bad value");           
        }
        return NULL;
    }
    return sr.at(srnum).first->getMetaData(frameNum-sr.at(srnum).second);
}


void MultiStackReader::annotatedFrame(int frameNum, IplImage** dst) {
    int srnum = findStackReader(frameNum); checkError();
    if (isError() || srnum < 0 || srnum >= sr.size()) {
        if (!isError()) {
            setError("findStackReader returned bad value");           
        }
        if (*dst != NULL) {
            cvReleaseImage(dst);
            *dst = NULL;
        }
        return;
    }
    sr.at(srnum).first->annotatedFrame(frameNum-sr.at(srnum).second, dst);
}

void MultiStackReader::playMovie(int startFrame, int endFrame, int delay_ms, const char* windowName, bool annotated) {
   
    startFrame = startFrame < 0 ? 0 : startFrame;
    endFrame = endFrame > totalFrames ? totalFrames : endFrame;
    endFrame = endFrame < 0 ? totalFrames : endFrame;

    if (windowName == NULL) {
        windowName =  "background restored movie";
    }
    cvNamedWindow(windowName, 0);
    IplImage *im = NULL;
  
    Timer tim;
    Sleep(2000); // removed on linux branch
    for (int f = startFrame; f < endFrame; ++f) {

        tim.start();
         if (annotated) {
            annotatedFrame(f, &im);
        } else {
            getFrame(f, &im);
        }
        if (im == NULL) {
            break;
        }


        cvShowImage(windowName, im);
        int delaytime = (int) (delay_ms - tim.getElapsedTimeInMilliSec());
        tim.stop();
        delaytime = delaytime < 1 ? 1: delaytime;
        if (tolower(cvWaitKey(delaytime)) == 'q') {
            break;
        }
    }
    if (im != NULL) {
        cvReleaseImage(&im);
    }
    cvDestroyWindow(windowName);
}

ExtraDataWriter *MultiStackReader::getSupplementalData() {
    return addToSupplementalData(NULL,0);
}
ExtraDataWriter *MultiStackReader::addToSupplementalData(ExtraDataWriter* edw, int frameOffset) {
    for (vector<pair<StackReader *, int> >::iterator it = sr.begin(); it != sr.end(); ++it) {
        edw = it->first.addToSupplementalData(edw, it->second + frameOffset);
    }
    checkError();
    return edw;
}

CvRect MultiStackReader::getLargestROI() {
    if (validROI.width <= 0 || validROI.height <= 0) {
        if (sr.empty()) {
            setError("getLargestROI called with no stack readers open");
            return validROI;
        }
        StackReader *s = sr[0].first;
        if (s == NULL) {
            setError("first stackReader is NULL");
            return validROI;
        }
        validROI = s->getLargestROI();
    }
    checkError();
    return validROI;
}


std::string MultiStackReader::diagnostics() {
    stringstream s;
    if (iserror) {
        s << "error: " << getError();
        return s.str();
    }
//    s << "no error" << endl;
    setSBC(0);
    if (sbc == NULL) {
        s << getError() << endl;
        s << "sbc was null" << endl;
        return s.str();
    }
    s << "sbc save description:\n" << sbc->saveDescription() << endl;
    //return s.str();
    s << "sbc has " << sbc->numProcessed() << "frames" << endl;
    s << "sbc frame size is " << sbc->getFrameSize().width << " x " << sbc->getFrameSize().height << endl;
    s << "frame 0:" << endl << "meta data:" << endl;
    const ImageMetaData* imd = sbc->getMetaData(0);
    if (imd == NULL) {
        s << "meta data is NULL" << endl;
    } else {
        s << imd->saveDescription() << endl;
        map<string,double>mdp = imd->getFieldNamesAndValues();
        for (map<string, double>::const_iterator it = mdp.begin(); it != mdp.end(); ++it) {
            s << it->first << "," << it->second << "\t";
        }
        s << endl;
    }
    s << sbc->numRegionsInFrame(0) << " regions different from background" << endl;
    //return s.str();
    IplImage *im = NULL;
    sbc->reconstructFrame(0, &im);
    s << "im params: w= " << im->width << ", h= " << im->height << ", nchannels = " << im->nChannels << ", width step = " << im->widthStep << "imageSize = " << im->imageSize << endl;

    return s.str();
    
}

int MultiStackReader::findStackReader(int frameNumber) {
    checkError();    if (isError()) {return -1;}
    for (int j = 0; j < nstacks && j < endFrames.size() && j < startFrames.size() && j < sr.size(); ++j) {
        if (startFrames[j] <= frameNumber && endFrames[j] >= frameNumber) {
            return j;
        }
    }
    
    stringstream s;
    if (nstacks*endFrames.size()* startFrames.size()*sr.size() == 0) {
        s << "tried to set stack reader with no open stacks";
    } else {
        s << "could not find frame number " << frameNumber << " : frame ranges are: ";
        for (int j = 0; j < nstacks && j < endFrames.size() && j < startFrames.size() && j < sr.size(); ++j) {
            s << startFrames[j] << "->" << endFrames[j] << "  ";
        }
    }
    return -1;
}