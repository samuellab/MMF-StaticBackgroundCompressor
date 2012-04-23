/* 
 * File:   WindowsThreadedStaticBackgroundCompressor.cpp
 * Author: Marc
 * 
 * Created on April 17, 2012, 11:18 AM
 */

#include "WindowsThreadedStaticBackgroundCompressor.h"
#include <windows.h>
#include <process.h>
struct processingInfo {
public:
    int imageNumber;
    IplImage *im;
    ImageMetaData *metadata;
    WindowsThreadedStaticBackgroundCompressor *sbc;
    processingInfo (int ImageNumber, IplImage *Im, ImageMetaData *Metadata, WindowsThreadedStaticBackgroundCompressor *Sbc) : imageNumber(ImageNumber), im(Im), metadata(Metadata), sbc(Sbc) {        
    }
};


WindowsThreadedStaticBackgroundCompressor::WindowsThreadedStaticBackgroundCompressor() {
    InitializeCriticalSection(&backgroundImCS);
    InitializeCriticalSection(&backgroundRemovedImageStackCS);
    InitializeCriticalSection(&imsToProcessCS);
    compressionThreadSemaphore = CreateSemaphore(NULL, maxCompressionThreads, maxCompressionThreads, NULL);
    numBeingCompressedRightNow = 0;
}

WindowsThreadedStaticBackgroundCompressor::WindowsThreadedStaticBackgroundCompressor(const WindowsThreadedStaticBackgroundCompressor& orig) {
}

WindowsThreadedStaticBackgroundCompressor::~WindowsThreadedStaticBackgroundCompressor() {
    CloseHandle(compressionThreadSemaphore);
    DeleteCriticalSection(&backgroundImCS);
    DeleteCriticalSection(&backgroundRemovedImageStackCS);
    DeleteCriticalSection(&imsToProcessCS);
}

int WindowsThreadedStaticBackgroundCompressor::numProcessed() {
    EnterCriticalSection(&backgroundRemovedImageStackCS);
    int sz = bri.size();
    LeaveCriticalSection(&backgroundRemovedImageStackCS);
    return sz;
}

int WindowsThreadedStaticBackgroundCompressor::numToProccess() {
    EnterCriticalSection(&imsToProcessCS);
    int sz = imsToProcess.size();
    LeaveCriticalSection(&imsToProcessCS);
    
    return sz + (numBeingCompressedRightNow < 0 ? 0 : numBeingCompressedRightNow);
    
}
    
void WindowsThreadedStaticBackgroundCompressor::frameCompressionFunction(void *ptr) {
    processingInfo *pi = (processingInfo *) ptr;
    
    //background is read-only so we don't need to protect it with a mutex
    BackgroundRemovedImage *brim = new BackgroundRemovedImage(pi->im, pi->sbc->background, pi->sbc->threshBelowBackground, pi->sbc->threshAboveBackground, pi->sbc->smallDimMinSize, pi->sbc->lgDimMinSize,  pi->metadata);
    
    EnterCriticalSection(&(pi->sbc->backgroundRemovedImageStackCS));
    pi->sbc->bri[pi->imageNumber] = brim;
    LeaveCriticalSection(&(pi->sbc->backgroundRemovedImageStackCS));
    
    cvReleaseImage(&(pi->im));
    pi->sbc->numBeingCompressedRightNow--;
    ReleaseSemaphore(pi->sbc->compressionThreadSemaphore,1,NULL);
    delete(pi);
}

//you do not need to clean up after the thread (it cleans itself up automatically)
 /* 
 The _beginthread function creates a thread that begins execution of a routine at start_address. 
 The routine at start_address must use the __cdecl calling convention and should have no return value. 
 When the thread returns from that routine, it is terminated automatically.

_
You can call _endthread or _endthreadex explicitly to terminate a thread; 
however, _endthread or _endthreadex is called automatically when the thread returns from the routine passed as a parameter. 
Terminating a thread with a call to endthread or _endthreadex helps to ensure proper recovery of resources allocated for the thread.

_endthread automatically closes the thread handle (whereas _endthreadex does not).
 Therefore, when using _beginthread and _endthread, do not explicitly close the thread handle by calling the Win32
 http://msdn.microsoft.com/en-us/library/cc429605.aspx API. 
 This behavior differs from the Win32 http://msdn.microsoft.com/en-us/library/cc429100.aspx API.
 
 */

int WindowsThreadedStaticBackgroundCompressor::processFrame() {
    EnterCriticalSection(&imsToProcessCS);
    if (imsToProcess.empty()) {
        LeaveCriticalSection(&imsToProcessCS);
        Sleep(5); // prevent constant querying while last frames process
        return numToProccess();
    }
    LeaveCriticalSection(&imsToProcessCS);
    
    
    EnterCriticalSection(&backgroundImCS);
    if (background == NULL) {
        LeaveCriticalSection(&backgroundImCS);
        return -1;
    }
    LeaveCriticalSection(&backgroundImCS);
    //wait for space to open for another thread
    DWORD timeout_ms = 100L;
    DWORD dwWaitResult = WaitForSingleObject(compressionThreadSemaphore,   timeout_ms);
    
    //if there is space for another thread, pop an image off the stack and start a thread to process it
    if (dwWaitResult == WAIT_OBJECT_0) {
       
        EnterCriticalSection(&imsToProcessCS);
        EnterCriticalSection(&backgroundRemovedImageStackCS);
        bri.reserve(imsToProcess.size());
        LeaveCriticalSection(&backgroundRemovedImageStackCS);
        InputImT nextim = imsToProcess.back();
        ++numBeingCompressedRightNow;
        imsToProcess.pop_back();   
        int imnum = imsToProcess.size();
        LeaveCriticalSection(&imsToProcessCS);
        
        processingInfo *pi = new processingInfo(imnum, nextim.first, nextim.second, this);
        _beginthread(WindowsThreadedStaticBackgroundCompressor::frameCompressionFunction, 0, (void *) pi);
    }
        
    
    return numToProccess();
}


void WindowsThreadedStaticBackgroundCompressor::calculateBackground() {
    EnterCriticalSection(&imsToProcessCS);
    EnterCriticalSection(&backgroundImCS);
    StaticBackgroundCompressor::calculateBackground();
    LeaveCriticalSection(&imsToProcessCS);
    LeaveCriticalSection(&backgroundImCS);
}
void WindowsThreadedStaticBackgroundCompressor::updateBackground(const IplImage* im) {
    EnterCriticalSection(&backgroundImCS);
    StaticBackgroundCompressor::updateBackground(im);
    LeaveCriticalSection(&backgroundImCS);
}



void WindowsThreadedStaticBackgroundCompressor::addFrame(IplImage** im, ImageMetaData* metadata) {
    EnterCriticalSection(&imsToProcessCS);
    EnterCriticalSection(&backgroundImCS);
    StaticBackgroundCompressor::addFrame(im,metadata);
    LeaveCriticalSection(&imsToProcessCS);
    LeaveCriticalSection(&backgroundImCS);
}

void WindowsThreadedStaticBackgroundCompressor::toDisk(std::ofstream& os) {
    EnterCriticalSection(&backgroundImCS);
    EnterCriticalSection(&backgroundRemovedImageStackCS);
    StaticBackgroundCompressor::toDisk(os);
    LeaveCriticalSection(&backgroundImCS);
    LeaveCriticalSection(&backgroundRemovedImageStackCS);
}

std::string WindowsThreadedStaticBackgroundCompressor::saveDescription() {
    std::string rv;
    EnterCriticalSection(&backgroundRemovedImageStackCS);
    rv = StaticBackgroundCompressor::saveDescription();
    LeaveCriticalSection(&backgroundRemovedImageStackCS);
    return rv;
  
}


int WindowsThreadedStaticBackgroundCompressor::sizeOnDisk() {
   int rv;
   EnterCriticalSection(&backgroundImCS);
   EnterCriticalSection(&backgroundRemovedImageStackCS);
   rv = StaticBackgroundCompressor::sizeOnDisk();
   LeaveCriticalSection(&backgroundImCS);
   LeaveCriticalSection(&backgroundRemovedImageStackCS);
   return rv;
}