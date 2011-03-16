/* 
 * File:   StackWriterWrapper.cpp
 * Author: Marc
 *
 * Created on March 16, 2011, 3:07 PM
 */

#include <cstdlib>

#include "wtscWrapper.h"
#include "NameValueMetaData.h"
#include "StackWriterWrapper.h"
#include "time.h"
using namespace std;

void *createBrightFieldStackWriter (const char *fname, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int keyFrameInterval, double frameRate) {
    wtscWrapper *ww = new wtscWrapper();
    ww->wtsc.setOutputFileName(fname);
    ww->wtsc.setIntervals(keyFrameInterval, 1);
    ww->wtsc.setThresholds(0, thresholdAboveBackground, smallDimMinSize, lgDimMinSize);
    ww->wtsc.setFrameRate(frameRate);
    ww->wtsc.startThreads();
    return (void *) ww;
}

 void destroyStackWriter (void *sw) {
     wtscWrapper *ww = (wtscWrapper *) sw;
     ww->wtsc.finishRecording(); //these are already done by deleting ww, but it's not the biggest deal to short-cut the process
     ww->wtsc.closeOutputFile(); //to make clear this happens here
     delete (ww);
 }

int addFrame (void *sw, void *ipl_im) {
    if (sw == NULL) {
        return -1;
    }
    if (ipl_im == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->md.replaceData("frameAddedTimeStamp", ww->tim.getElapsedTimeInMilliSec());
    ww->wtsc.newFrame((IplImage *) ipl_im, ww->md.copy());
    ww->md.clear();
    return 0;
}

int setMetaData(void* sw, char* fieldname, double fieldvalue) {
    if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->md.replaceData(string(fieldname), fieldvalue);
    return 0;
}

int startRecording (void *sw, int nframes) {
     if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->tim.start();
    ww->wtsc.startRecording(nframes);
    return 0;
}

int stopRecording (void *sw) {
     if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    ww->wtsc.stopRecording();
    return 0;
}