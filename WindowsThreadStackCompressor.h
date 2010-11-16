/* 
 * File:   WindowsThreadStackCompressor.h
 * Author: Marc
 *
 * modifies LinearStackCompressor to use windows threads and mutexes
 */

#ifndef WINDOWSTHREADSTACKCOMPRESSOR_H
#define	WINDOWSTHREADSTACKCOMPRESSOR_H

#include "LinearStackCompressor.h"
#include <windows.h>
#include <process.h>

class WindowsThreadStackCompressor : public LinearStackCompressor {
public:
    WindowsThreadStackCompressor();
    virtual ~WindowsThreadStackCompressor();
    virtual void newFrame(const IplImage *im, ImageMetaData *metadata);
    virtual void finishRecording();
    virtual void openOutputFile ();
    virtual void closeOutputFile ();
    virtual int startThreads(); //returns -1 on error, 0 on ok

protected:
    CRITICAL_SECTION activeStackCS;
    CRITICAL_SECTION compressingStackCS;
    CRITICAL_SECTION imageStacksCS;
    CRITICAL_SECTION outfileCS;
    CRITICAL_SECTION writingStackCS;

    HANDLE compressionThread;
    HANDLE writingThread;

    unsigned __stdcall compressionThreadFunction();
    unsigned __stdcall writingThreadFunction();
    static unsigned __stdcall startCompressionThread(void *ptr);
    static unsigned __stdcall startWritingThread(void *ptr);
    
    virtual void addFrameToStack(IplImage **im, ImageMetaData *metadata);
    bool compressionThreadActive;
    bool writingThreadActive;

    bool stacksLeftToCompress;
    bool stacksLeftToWrite;

private:
     WindowsThreadStackCompressor(const WindowsThreadStackCompressor& orig);
     void init();

};

#endif	/* WINDOWSTHREADSTACKCOMPRESSOR_H */

