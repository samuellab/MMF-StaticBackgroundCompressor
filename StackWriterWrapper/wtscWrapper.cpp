/* 
 * File:   wtscWrapper.cpp
 * Author: Marc
 * 
 * Created on March 16, 2011, 2:22 PM
 * 
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include "wtscWrapper.h"
#include <sstream>
#include <iomanip>

using namespace std;

void wtscWrapper::init() {
    InitializeCriticalSection(&protectedAction);
    wtsc = NULL;
    wtsc_old = NULL;
    limitFileSize = false;
    maximumBytesToWriteInOneFile = 2000000000;
}

wtscWrapper::wtscWrapper() {
    init();
}

wtscWrapper::~wtscWrapper() {
    if (wtsc != NULL) {
        wtsc->finishRecording();
        wtsc->closeOutputFile();
        delete (wtsc);
    }
    if (wtsc_old != NULL) {
        wtsc_old->finishRecording();
        wtsc_old->closeOutputFile();
        delete(wtsc_old);
    }
    DeleteCriticalSection(&protectedAction);
}

void wtscWrapper::enterCS() {
    EnterCriticalSection(&protectedAction);
}

void wtscWrapper::leaveCS() {
    LeaveCriticalSection(&protectedAction);
}


wtscWrapper::wtscWrapper(const char *fname, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int keyFrameInterval, double frameRate) {
    init();
    this->thresholdAboveBackground = thresholdAboveBackground;
    this->smallDimMinSize = smallDimMinSize;
    this->lgDimMinSize = lgDimMinSize;
    this->keyFrameInterval = keyFrameInterval;
    this->frameRate = frameRate;
    string fn(fname);
    size_t ind = fn.find_last_of('.');
    filestub = fn.substr(0,ind);
    ext = fn.substr(ind+1);    
    
    newStackWriter();       
}

wtscWrapper::wtscWrapper(const char *fstub, const char *ext, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int keyFrameInterval, double frameRate, uint64_t maxBytesToWrite) {
    init();
    this->thresholdAboveBackground = thresholdAboveBackground;
    this->smallDimMinSize = smallDimMinSize;
    this->lgDimMinSize = lgDimMinSize;
    this->keyFrameInterval = keyFrameInterval;
    this->frameRate = frameRate;
    maximumBytesToWriteInOneFile = maxBytesToWrite;
    limitFileSize = true;
    filestub = string(fstub);
    this->ext = string(ext);
    newStackWriter();
   
    
}

void wtscWrapper::newStackWriter() {
    wtsc = new WindowsThreadStackCompressor();
    string fname;
    if (limitFileSize) {
        stringstream ss;
        ss << filestub << "-" << setw(3) << setfill('0') << fileNumber << "." << ext;
        fname = ss.str();
        fileNumber++;
    } else {
        fname = filestub + "." + ext;
    }
        
    wtsc->setOutputFileName(fname.c_str());
    wtsc->setIntervals(keyFrameInterval, 1);
    wtsc->setThresholds(0, thresholdAboveBackground, smallDimMinSize, lgDimMinSize);
    wtsc->setFrameRate(frameRate);
    wtsc->startThreads();
}

int wtscWrapper::addFrame (void *ipl_im) {
    if (wtsc == NULL) {
        return -2;
    }
    if (ipl_im == NULL) {
        return -3;
    }
    enterCS();
    if (wtsc_old != NULL) {
        if (wtsc_old->nothingLeftToCompressOrWrite()) {
            wtsc_old->closeOutputFile();
            delete(wtsc_old);
            wtsc_old == NULL;
        }
    }
    if (limitFileSize && wtsc->numBytesWritten() >= (0.99*maximumBytesToWriteInOneFile)) {
        if (wtsc_old != NULL) {
            wtsc_old->finishRecording();
            wtsc_old->closeOutputFile();
            delete (wtsc_old);
        }
        wtsc_old = wtsc;
        wtsc_old->goIdle();
        newStackWriter();
    }
    md.replaceData("frameAddedTimeStamp", tim.getElapsedTimeInMilliSec());
    wtsc->newFrame((IplImage *) ipl_im, md.copy());
    md.clear();
    leaveCS();
    return 0;
}

int wtscWrapper::setMetaData(char* fieldname, double fieldvalue) {
    
    enterCS();
    md.replaceData(string(fieldname), fieldvalue);
    leaveCS();
    return 0;
}

int wtscWrapper::startRecording (int nframes) {     
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    tim.start();
    wtsc->startRecording(nframes);
    leaveCS();
    return 0;
}

int wtscWrapper::stopRecording() {
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    wtsc->stopRecording();
    leaveCS();
    return 0;
}

int64_t wtscWrapper::numBytesWritten () {
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    int64_t nbr = wtsc->numBytesWritten();
    leaveCS();
    return nbr;
}
int wtscWrapper::getTimingStatistics (double *avgAddTime, double *avgCompressTime, double *avgWriteTime) {
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    if (avgAddTime != NULL) {
        *avgAddTime = wtsc->NonthreadedTimer().getStatistics("adding frame to stack");;
    }
    if (avgCompressTime != NULL) {
        *avgCompressTime = wtsc->CompressionThreadTimer().getStatistics("compressing a frame");
    }
    if (avgWriteTime != NULL) {
        *avgWriteTime =  (wtsc->WritingThreadTimer().getStatistics("writing a stack to disk") + wtsc->WritingThreadTimer().getStatistics("deleting written stack from memory"))/wtsc->getKeyFrameInterval();
    }
    leaveCS();
    return 0;
}
int wtscWrapper::getNumStacksQueued (int *numToCompress, int *numToWrite) {
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    int ntc, ntw;
    wtsc->numStacksWaiting(ntc, ntw);
    if (numToCompress != NULL) {
        *numToCompress = ntc;
    }
    if (numToWrite != NULL) {
        *numToWrite = ntw;
    }
    leaveCS();
    return 0;
}

int wtscWrapper::getTimingReport (char *dst, int maxchars) {
    if (wtsc == NULL) {
        return -1;
    }
    enterCS();
    string s = wtsc->generateTimingReport();
    if (dst != NULL) {
        s.copy(dst, maxchars);
    }
    leaveCS();
    return 0;
}
