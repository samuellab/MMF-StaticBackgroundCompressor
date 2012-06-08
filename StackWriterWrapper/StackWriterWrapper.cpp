/* 
 * File:   StackWriterWrapper.cpp
 * Author: Marc
 *
 * Created on March 16, 2011, 3:07 PM
 * 
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include <cstdlib>
#include <limits>

#include "wtscWrapper.h"
#include "NameValueMetaData.h"
#include "StackWriterWrapper.h"
#include "time.h"
using namespace std;

void *createBrightFieldStackWriter (const char *fname, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int keyFrameInterval, double frameRate) {
    wtscWrapper *ww = new wtscWrapper(fname, thresholdAboveBackground, smallDimMinSize, lgDimMinSize, keyFrameInterval, frameRate);
    assert (ww != NULL);
    return (void *) ww;
}
void *createBrightFieldStackWriterWithSizeLimit (const char *fstub, const char *ext, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int keyFrameInterval, double frameRate, uint64_t maxBytesToWrite) {
    wtscWrapper *ww = new wtscWrapper(fstub, ext, thresholdAboveBackground, smallDimMinSize, lgDimMinSize, keyFrameInterval, frameRate, maxBytesToWrite);
    assert (ww != NULL);
    return (void *) ww;
}

 void destroyStackWriter (void *sw) {
     if (sw == NULL) {
        return;
    }
     wtscWrapper *ww = (wtscWrapper *) sw;
     delete (ww);
 }

int addFrame (void *sw, void *ipl_im) {
    if (sw == NULL) {
        return -1;
    }
   
    wtscWrapper *ww = (wtscWrapper *) sw;
    return ww->addFrame(ipl_im);
}

int setMetaData(void* sw, char* fieldname, double fieldvalue) {
    if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    return ww->setMetaData(fieldname, fieldvalue);
}

int startRecording (void *sw, int nframes) {
     if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    return ww->startRecording(nframes);
}

int stopRecording (void *sw) {
     if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    return ww->stopRecording();
}

int64_t numBytesWritten (void *sw) {
    if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    return ww->numBytesWritten();
}
uint64_t maxBytesSupported() {
    return wtscWrapper::maxFileSizeSupported();
}
int getTimingStatistics (void *sw, double *avgAddTime, double *avgCompressTime, double *avgWriteTime) {
     if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    return ww->getTimingStatistics(avgAddTime, avgCompressTime, avgWriteTime);
}
int getNumStacksQueued (void *sw, int *numToCompress, int *numToWrite) {
     if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    return ww->getNumStacksQueued(numToCompress, numToWrite);
}

int getTimingReport (void *sw, char *dst, int maxchars) {
    if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    return ww->getTimingReport(dst, maxchars);
}

int setNumCompressionThreads (void *sw, int maxCompressionThreads) {
    if (sw == NULL) {
        return -1;
    }
    wtscWrapper *ww = (wtscWrapper *) sw;
    return ww->setMaxCompressionThreads(maxCompressionThreads);
}