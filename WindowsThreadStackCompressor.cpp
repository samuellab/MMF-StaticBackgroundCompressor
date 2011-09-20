/* 
 * File:   WindowsThreadStackCompressor.cpp
 * Author: Marc
 * 
 * Created on November 16, 2010, 11:02 AM
 */

#include "WindowsThreadStackCompressor.h"
#include <windows.h>
#include <process.h>
#include <iostream>
#include "LinearStackCompressor.h"
#include "tictoc/Timer.h"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iosfwd>
using namespace std;

//static ofstream logkludge("c:\\wtsckludge.txt");

WindowsThreadStackCompressor::WindowsThreadStackCompressor() {
    init();
}

WindowsThreadStackCompressor::WindowsThreadStackCompressor(const WindowsThreadStackCompressor& orig) {
}

WindowsThreadStackCompressor::~WindowsThreadStackCompressor() {
    //logkludge << "entered destructor" <<endl;
    finishRecording();
    //logkludge << "finished recording" << endl;
    writingThreadActive = false;
    compressionThreadActive = false;

    closeOutputFile();
    //logkludge << "output file closed" <<endl;
    int timeout = 2000;
    if (compressionThread != NULL) {
        WaitForSingleObject(compressionThread, timeout);
    }
    if (writingThread != NULL) {
        WaitForSingleObject(writingThread, timeout);
    }
    CloseHandle(compressionThread);
    CloseHandle(writingThread);
    DeleteCriticalSection(&activeStackCS);
    DeleteCriticalSection(&compressingStackCS);
    DeleteCriticalSection(&imageStacksCS);
    DeleteCriticalSection(&outfileCS);
    DeleteCriticalSection(&writingStackCS);
}

void WindowsThreadStackCompressor::init() {
    InitializeCriticalSection(&activeStackCS);
    InitializeCriticalSection(&compressingStackCS);
    InitializeCriticalSection(&imageStacksCS);
    InitializeCriticalSection(&outfileCS);
    InitializeCriticalSection(&writingStackCS);
    compressionThread = NULL;
    writingThread = NULL;
    compressionThreadActive = false;
    writingThreadActive = false;
}

int WindowsThreadStackCompressor::startThreads() {
    compressionThreadActive = true;
    writingThreadActive = true;
    /*
   uintptr_t _beginthreadex( // NATIVE CODE
   void *security, <-- NULL
   unsigned stack_size, <--- 0
   unsigned ( __stdcall *start_address )( void * ), <--- function
   void *arglist, <--- this
   unsigned initflag, <---(0 for running or CREATE_SUSPENDED for suspended);
   unsigned *thrdaddr <---another return value for the thread handle, can be NULL
);

     */
    if (compressionThread == NULL) {
        compressionThread = (HANDLE) _beginthreadex(NULL, 0, WindowsThreadStackCompressor::startCompressionThread, this, 0, NULL);
    }
    if (writingThread == NULL) {
        writingThread = (HANDLE) _beginthreadex(NULL, 0, WindowsThreadStackCompressor::startWritingThread, this, 0, NULL);
    }
    //logkludge << "started threads " << " compressionThread = " << (unsigned long) compressionThread << " writingThread = " << (unsigned long) writingThread << endl;
    if (compressionThread == NULL || writingThread == NULL) {
        return -1;
    }
    return 0;
}

void WindowsThreadStackCompressor::newFrame(const IplImage* im, ImageMetaData *metadata) {

  //  //logkludge << "new frame called" << endl;
    nonthreadedTimer.tic("cloning image");
    IplImage *imcpy = cvCloneImage(im);
    nonthreadedTimer.toc("cloning image");
   // //cout << "imcopied" <<endl;
    /*****************activeStackCS*****************/
    EnterCriticalSection(&activeStackCS);
   // //cout << "add frame to stack" << endl;
    addFrameToStack(&imcpy, metadata);
    LeaveCriticalSection(&activeStackCS);
    /***********************************************/

    if (recordingState == recording) {
        if(--framesToRecord <= 0) {
            finishRecording();
            return;
        }
    }

}

void WindowsThreadStackCompressor::addFrameToStack(IplImage** im, ImageMetaData* metadata) {
    
    EnterCriticalSection(&activeStackCS);
    nonthreadedTimer.tic("adding frame to stack");
    if (activeStack == NULL) {
        createStack();
    }
    if (activeStack->numToProccess() >= keyframeInterval) {

        /*****************imageStacksCS*******************/
        EnterCriticalSection(&imageStacksCS);
        imageStacks.push_back(activeStack);
        LeaveCriticalSection(&imageStacksCS);
        /*************************************************/

        createStack();
    }
    if (recordingState == recording) {
        activeStack->addFrame(im, metadata);
    } else {
        if (recordingState == updatingBackground) {
            activeStack->updateBackground(*im);
        }
        if (metadata != NULL) {
            delete metadata;
        }
        cvReleaseImage(im);
    }
    LeaveCriticalSection(&activeStackCS);
    nonthreadedTimer.toc("adding frame to stack");
}

unsigned __stdcall WindowsThreadStackCompressor::compressionThreadFunction() {
    static int numcompressed = 0;
    while (compressionThreadActive) {
        /*****************imageStacksCS*******************/
         EnterCriticalSection(&imageStacksCS);
         setCompressionStack();
         LeaveCriticalSection(&imageStacksCS);
         /************************************************/

         stacksLeftToCompress = (stackBeingCompressed != NULL);
         if (stacksLeftToCompress) {
             
             EnterCriticalSection(&compressingStackCS);
             compressionThreadTimer.tic("compressing a frame");
             stackBeingCompressed->processFrame();
            // cout << ++numcompressed << " processed" << endl;
             LeaveCriticalSection(&compressingStackCS);
             compressionThreadTimer.toc("compressing a frame");
         } else {
             Sleep((int) (1000/frameRate));
         }
    }
    //logkludge << "leaving compression thread";
    return 0;
}

unsigned __stdcall WindowsThreadStackCompressor::startCompressionThread(void* ptr) {
    WindowsThreadStackCompressor *wtsc = (WindowsThreadStackCompressor *) ptr;
    return wtsc->compressionThreadFunction();
}

unsigned __stdcall WindowsThreadStackCompressor::writingThreadFunction() {
  //  cout << "started writing thread" << endl;
    while (writingThreadActive) {
        /**************writingStackCS, imageStacksCS******************/
        EnterCriticalSection(&writingStackCS);
        EnterCriticalSection(&imageStacksCS);
        setWritingStack();
        LeaveCriticalSection(&imageStacksCS);
        LeaveCriticalSection(&writingStackCS);
        /*************************************************************/

        stacksLeftToWrite = (stackBeingWritten != NULL);
        if (stacksLeftToWrite) {
    //        cout << "writing a stack\n";
            /**********************writingStackCS******************/
            EnterCriticalSection(&writingStackCS);
            if (stacksavedescription.empty()) {
                writingThreadTimer.tic("creating save description");
                stacksavedescription = stackBeingWritten->saveDescription();
                writingThreadTimer.toc("creating save description");
      //          cout << "created save description\n";
            }
            /*********************outfileCS***********************/
            
            EnterCriticalSection(&outfileCS);
            writingThreadTimer.tic("writing a stack to disk");
            if (outfile == NULL) {
               openOutputFile();
            }
            if (outfile == NULL) {
               cout << "error opening output file" << endl;
               return 1;
            }

        //    cout << "writing a stack" << endl;
           stackBeingWritten->toDisk(*outfile);
           writingThreadTimer.toc("writing a stack to disk");
           LeaveCriticalSection(&outfileCS);
           /*******************-outfile, writing still active****************/

           /*************imageStacksCS (+writing)**************************/
           
           EnterCriticalSection(&imageStacksCS);
           writingThreadTimer.tic("deleting written stack from memory");
           vector<StaticBackgroundCompressor *>::iterator it;
           it = find(imageStacks.begin(), imageStacks.end(), stackBeingWritten);
           while (it != imageStacks.end()) {
               imageStacks.erase(it);
               it = find(imageStacks.begin(), imageStacks.end(), stackBeingWritten);
           }
           delete stackBeingWritten;
           stackBeingWritten = NULL;
           LeaveCriticalSection(&imageStacksCS);
           LeaveCriticalSection(&writingStackCS);
           writingThreadTimer.toc("deleting written stack from memory");
          
           /******************************************************/
        }else {
          //  cout << "writing asleep\n"<<endl;
             Sleep((int) (1000/frameRate));
        }
        
    }
    //logkludge << "leaving writing thread" << endl;
    return 0;
}
unsigned __stdcall WindowsThreadStackCompressor::startWritingThread(void* ptr) {
    WindowsThreadStackCompressor *wtsc = (WindowsThreadStackCompressor *) ptr;
    return wtsc->writingThreadFunction();
}


void WindowsThreadStackCompressor::finishRecording() {
    recordingState = idle;
    /*****************activeStackCS*********************/
    EnterCriticalSection(&activeStackCS);
    recordingState = idle;
    if (activeStack != NULL) {
        if (activeStack->numToProccess() > 0) {
            /********imageStackCS (+activeStack)*********/
            EnterCriticalSection(&imageStacksCS);
            imageStacks.push_back(activeStack);
            LeaveCriticalSection(&imageStacksCS);
            /**********-imageStack (activeStack still on)*************/
        } else {
            delete activeStack;
        }
        activeStack = NULL;
    }
    LeaveCriticalSection(&activeStackCS);
    /******************************************************************/

    //wait for active stacks to empty out --
    //if it doesn't after ~10 seconds, then we'll check to see if anything is still be written
    bool done = false;
    for (int j = 0; j < 1000 && !done; ++j) {
         /*****************imageStacksCS*********************/
        EnterCriticalSection(&imageStacksCS);
        done = imageStacks.empty();
        LeaveCriticalSection(&imageStacksCS);
        /****************************************************/
        Sleep(10);
    }
    while (!done) {
        done = !(stacksLeftToCompress || stacksLeftToWrite); //changed by MHG without full understanding 3/15/11 -- added !()
        //logkludge << "stacks left to compress = " << stacksLeftToCompress << "\t stacks left to write = " << stacksLeftToWrite << endl;
        Sleep(10);
    }
}



void WindowsThreadStackCompressor::openOutputFile() {
    EnterCriticalSection(&outfileCS);
    LinearStackCompressor::openOutputFile();
    LeaveCriticalSection(&outfileCS);
}

void WindowsThreadStackCompressor::closeOutputFile() {
    finishRecording();
    EnterCriticalSection(&outfileCS);
    LinearStackCompressor::closeOutputFile();
    LeaveCriticalSection(&outfileCS);
}

std::string WindowsThreadStackCompressor::generateTimingReport() {
    std::stringstream ss;
    ss << nonthreadedTimer.generateReport() << compressionThreadTimer.generateReport() << writingThreadTimer.generateReport();
    const char *version[100], *addons[100];
    for (int j = 0; j < 100; ++j) {
        version[j] = addons[j] = NULL;
    }
    cvGetModuleInfo(NULL, version, addons);
    for (int j = 0; j < 100 && version[j] != 0; ++j) {
        ss << version[j] << endl;
    }
    for (int j = 0; j < 100 && addons[j] != 0; ++j) {
        ss << addons[j] << endl;
    }

    return ss.str();
}


ofstream::pos_type WindowsThreadStackCompressor::numBytesWritten() {
    EnterCriticalSection(&outfileCS);
    ofstream::pos_type nb = LinearStackCompressor::numBytesWritten();
    LeaveCriticalSection(&outfileCS);
    return nb;
}

void WindowsThreadStackCompressor::numStacksWaiting(int& numToCompress, int& numToWrite) {
    EnterCriticalSection(&imageStacksCS);
    LinearStackCompressor::numStacksWaiting(numToCompress, numToWrite);
    LeaveCriticalSection(&imageStacksCS);
}