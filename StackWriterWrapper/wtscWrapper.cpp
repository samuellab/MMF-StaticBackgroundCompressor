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
#include <io.h>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;
//std::ofstream os("c:\\wtsclog.txt");
void wtscWrapper::init() {
    
  //  assert (!os.fail());
  //  os << "create mutex  called" << endl;
 //   InitializeCriticalSection(&protectedAction);
    protectedAction = CreateMutex(NULL, FALSE, NULL);
        
   // os << "create mutex returned" << endl;
    wtsc = NULL;
    wtsc_old = NULL;
    limitFileSize = false;
    maximumBytesToWriteInOneFile = 2000000000;
    maxCompressionThreads = defaultMaxCompressionThreads;
    fileNumber = 0;
    //os << "init returned " << endl;
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
   // DeleteCriticalSection(&protectedAction);
    CloseHandle(protectedAction);
}
//
//void wtscWrapper::enterCS(const char* str) {
//    enterCS();
//    if (writecstest) {
//        TEST_CS_LOG << str << ": " << " entered critical section" << endl << flush;
//    }
//}
//void wtscWrapper::leaveCS(const char* str) {
//    if (writecstest) {
//        TEST_CS_LOG << str << ": " << " leaving critical section" << endl << flush;
//    }
//    leaveCS();
//}

bool wtscWrapper::enterCS() {
    enterCS(default_timeout);
}

bool wtscWrapper::enterCS(unsigned long waitTimeInMs) {
    //EnterCriticalSection(&protectedAction);
    DWORD waitResult = WaitForSingleObject(protectedAction, waitTimeInMs);
    string msg;
    bool rv = false;
    stringstream ss;
    switch (waitResult) {
        case WAIT_OBJECT_0:
            return false;
            break;
        case WAIT_ABANDONED:
            msg = ": warning : wtscWrapper mutex was abandoned";
            rv = false;
            break;
        case WAIT_TIMEOUT:

            ss << ": error : wtscWrapper timed out waiting for mutex after " << waitTimeInMs/1000 << "seconds";
            msg = ss.str();
            rv = true;
            break;
        default:
            msg = ": error : wtscWrapper mutex error";
            rv = true;
            break;
    }
    
    ofstream os("c:\\stackwriterwrapper_error_log.txt", ios_base::ate);
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    if (!os.fail()) {
        os << asctime(timeinfo) << msg << ", error code = " << GetLastError() << endl << flush;
    }
    return rv;

}

void wtscWrapper::leaveCS() {
    //LeaveCriticalSection(&protectedAction);
    ReleaseMutex(protectedAction);
}


wtscWrapper::wtscWrapper(const char *fname, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int keyFrameInterval, double frameRate) {
//    os << "constructor called " << fname << " " << thresholdAboveBackground << " " << smallDimMinSize << " " << lgDimMinSize << " " << keyFrameInterval << " " << frameRate;
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
    assert (wtsc != NULL);
  //  os << "wtsc created and points to " << (intptr_t) wtsc << endl;
    
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
    assert (wtsc != NULL);
   
    
}

int64_t wtscWrapper::setMaxCompressionThreads(int maxThreads) {
    maxCompressionThreads = maxThreads;
    if (wtsc != NULL) {
        wtsc->setNumCompressionThreads(maxCompressionThreads);
    }
    return 0;
}

void wtscWrapper::newStackWriter() {

    wtsc = new WindowsThreadStackCompressor();
    assert (wtsc != NULL);
    wtsc->setNumCompressionThreads(maxCompressionThreads);
    stringstream ss;
    if (limitFileSize) {
        
        ss << filestub << "-" << setw(3) << setfill('0') << fileNumber << "." << ext;
        fileNumber++;
    } else {
        ss << filestub << "." << ext;
    }
    
    
    wtsc->setOutputFileName(ss.str().c_str());
    
    wtsc->setFrameRate(frameRate);
    
    wtsc->setIntervals(keyFrameInterval, 1);
    
    wtsc->setThresholds(0, thresholdAboveBackground, smallDimMinSize, lgDimMinSize);
    
    wtsc->startThreads();
}

int64_t wtscWrapper::addFrame (void *ipl_im) {
    if (wtsc == NULL) {
        return -2;
    }
    if (ipl_im == NULL) {
        return -3;
    }
    static int frameAddedNumber = 0;
    ++frameAddedNumber;
    if (enterCS()) {
        return GetLastError();
    }
    
    if (wtsc_old != NULL) {
  //      ofstream os("c:\\testnewwtsc.txt", ios_base::out | ios_base::app);
 //       os << "on frame: " << frameAddedNumber << " detected old stackwriter" << endl << flush;
        if (wtsc_old->nothingLeftToCompressOrWrite()) {
 //           os << "closing old stackwriter output file" << endl << flush;
            wtsc_old->closeOutputFile();
  //          os << "nuking old stackwriter" << endl << flush;
            delete(wtsc_old);
            wtsc_old = NULL;
 //           os << "old stacwriter is now " << (intptr_t) wtsc_old; 
        }
    }
    if (limitFileSize && wtsc->numBytesWritten() >= (0.99*maximumBytesToWriteInOneFile)) {
  //      ofstream os("c:\\testnewwtsc.txt", ios_base::out | ios_base::app);
 //       os << "on frame: " << frameAddedNumber << " I need to make a new stackwriter" << endl << flush;
        if (wtsc_old != NULL) {
   //         os << "finishing off wtsc_old" << endl << flush;
            wtsc_old->finishRecording();
            wtsc_old->closeOutputFile();
            delete (wtsc_old);
            wtsc_old = NULL;
       //     os << "finsihed off wtsc_old" << endl << flush;
        }
    //    os << "getting num frames left to record" << endl << flush;
        int nframes = wtsc->numFramesLeftToRecord();
  //      os << "going idle" << endl << flush;
        wtsc_old = wtsc;
        wtsc_old->goIdle();
        
        //wtsc->finishRecording();
        //delete(wtsc);
        wtsc = NULL;
 //       os << "creating new stack writer" << endl << flush;
        newStackWriter();
//        os << "new stack writer created" << endl << flush;
        wtsc->startRecording(nframes);
 //       os << "started recording" << endl << flush;
        
    }
    md.replaceData("frameAddedTimeStamp", tim.getElapsedTimeInMilliSec());
    wtsc->newFrame((IplImage *) ipl_im, md.copy());
    md.clear();
    leaveCS();
    return 0;
}

int64_t wtscWrapper::setMetaData(char* fieldname, double fieldvalue) {
    
    if (enterCS()) {
        return GetLastError();
    }
    md.replaceData(string(fieldname), fieldvalue);
    leaveCS();
    return 0;
}

int64_t wtscWrapper::startRecording (int nframes) {     
    if (wtsc == NULL) {
        return -1;
    }
    if (enterCS()) {
        return GetLastError();
    }
    tim.start();
    wtsc->startRecording(nframes);
    this->nframes = nframes;
    leaveCS();
    return 0;
}

int64_t wtscWrapper::stopRecording() {
    if (wtsc == NULL) {
        return -1;
    }
    if (enterCS()) {
        return GetLastError();
    }
    wtsc->stopRecording();
    leaveCS();
    return 0;
}

int64_t wtscWrapper::numBytesWritten () {
    if (wtsc == NULL) {
        return -1;
    }
    if (enterCS()) {
        return -1*GetLastError();
    }
    int64_t nbr = wtsc->numBytesWritten();
    leaveCS();
    return nbr;
}
int64_t wtscWrapper::getTimingStatistics (double *avgAddTime, double *avgCompressTime, double *avgWriteTime) {
    if (wtsc == NULL) {
        return -1;
    }
    if (enterCS()) {
        return GetLastError();
    }
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
int64_t wtscWrapper::getNumStacksQueued (int *numToCompress, int *numToWrite) {
    if (wtsc == NULL) {
        return -1;
    }
    if (enterCS()) {
        return GetLastError();
    }
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

int64_t wtscWrapper::getTimingReport (char *dst, int maxchars) {
    if (wtsc == NULL) {
        return -1;
    }
    if (enterCS(30000)) {
        return GetLastError();
    }
    string s = wtsc->generateTimingReport();
    if (dst != NULL) {
        s.copy(dst, maxchars);
    }
    leaveCS();
    return 0;
}
