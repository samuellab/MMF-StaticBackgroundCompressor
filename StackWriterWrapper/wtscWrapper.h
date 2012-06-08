/* 
 * File:   wtscWrapper.h
 * Author: Marc
 *
 * Created on March 16, 2011, 2:22 PM
 * 
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
#include "WindowsThreadStackCompressor.h"
#include <windows.h>
#include <limits>
#include "NameValueMetaData.h"
#include "Timer.h"
#ifndef WTSCWRAPPER_H
#define	WTSCWRAPPER_H

class wtscWrapper {
    public:
        static const int defaultMaxCompressionThreads = 4;
        wtscWrapper();
        wtscWrapper(const char *fname, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int keyFrameInterval, double frameRate);
        wtscWrapper(const char *fstub, const char *ext, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int keyFrameInterval, double frameRate, uint64_t maxBytesToWrite);

        virtual ~wtscWrapper();
        WindowsThreadStackCompressor *wtsc;
        WindowsThreadStackCompressor *wtsc_old;
        uint64_t maximumBytesToWriteInOneFile;
        bool limitFileSize;
        NameValueMetaData md;
        Timer tim;
        void enterCS();
        void leaveCS();
        int setMaxCompressionThreads (int maxThreads);
        
        int addFrame (void *ipl_im);

        int setMetaData(char* fieldname, double fieldvalue);

        int startRecording (int nframes);
        int stopRecording ();

        int64_t numBytesWritten ();
        static inline uint64_t maxFileSizeSupported() {
            return std::numeric_limits<std::streamoff>::max();
        }
        int getTimingStatistics (double *avgAddTime, double *avgCompressTime, double *avgWriteTime);
        int getNumStacksQueued (int *numToCompress, int *numToWrite);
        int getTimingReport (char *dst, int maxchars);
        
        
    protected:
        CRITICAL_SECTION protectedAction;
        std::string filestub;
        std::string ext;
        void init();
        int thresholdAboveBackground;
        int smallDimMinSize;
        int lgDimMinSize;
        int keyFrameInterval;
        double frameRate;
        int fileNumber;
        void newStackWriter();
        int nframes;
        int maxCompressionThreads;
};




#endif	/* WTSCWRAPPER_H */

