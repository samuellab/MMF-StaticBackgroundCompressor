/* 
 * File:   WindowsThreadedStaticBackgroundCompressor.h
 * Author: Marc
 *
 * Created on April 17, 2012, 11:18 AM
 */

#ifndef WINDOWSTHREADEDSTATICBACKGROUNDCOMPRESSOR_H
#define	WINDOWSTHREADEDSTATICBACKGROUNDCOMPRESSOR_H
#ifdef WIN32
    #include "StaticBackgroundCompressor.h"
    #include <windows.h>

    class WindowsThreadedStaticBackgroundCompressor : public StaticBackgroundCompressor {
    public:
        WindowsThreadedStaticBackgroundCompressor(int maxThreads = 4);
        virtual void addFrame (IplImage **im, ImageMetaData *metadata = NULL);
        virtual bool readyToProcess();
        virtual int processFrame();
        virtual void calculateBackground();
        virtual void updateBackground(const IplImage *im);
        virtual void toDisk (std::ofstream &os);

        virtual std::string saveDescription();    
        virtual int sizeOnDisk();
        virtual ~WindowsThreadedStaticBackgroundCompressor();

        virtual int numProcessed();
        virtual bool framesWaitingToProcess();
        virtual int numToProccess();


    protected:
        const int maxCompressionThreads;
        CRITICAL_SECTION backgroundImCS;
        CRITICAL_SECTION backgroundRemovedImageStackCS;
        CRITICAL_SECTION imsToProcessCS;
        HANDLE compressionThreadSemaphore;
        static void frameCompressionFunction(void *ptr);


    private:
        WindowsThreadedStaticBackgroundCompressor(const WindowsThreadedStaticBackgroundCompressor& orig);

    };
#else
     #include "StaticBackgroundCompressor.h"
    typedef StaticBackgroundCompressor WindowsThreadedStaticBackgroundCompressor;
#endif
#endif	/* WINDOWSTHREADEDSTATICBACKGROUNDCOMPRESSOR_H */

