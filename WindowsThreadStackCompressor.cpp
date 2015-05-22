/* 
 * File:   WindowsThreadStackCompressor.cpp
 * Author: Marc
 * 
 * Created on November 16, 2010, 11:02 AM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 *
 */
#ifdef WIN32

#include "WindowsThreadStackCompressor.h"
#include "WindowsThreadedStaticBackgroundCompressor.h"
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
#include <io.h>
#include <queue>
using namespace std;

//static ofstream logkludge("c:\\wtsckludge.txt");

WindowsThreadStackCompressor::WindowsThreadStackCompressor() {
    init();
}

WindowsThreadStackCompressor::WindowsThreadStackCompressor(const WindowsThreadStackCompressor& orig) {
}

WindowsThreadStackCompressor::~WindowsThreadStackCompressor() {
    
    //finish recording waits for input buffer to empty
    finishRecording();
    
    writingThreadActive = false;
    compressionThreadActive = false;
    inputBufferThreadActive = false;
    
    closeOutputFile();
    
    endThreads(2000);
    
    DeleteCriticalSection(&activeStackCS);
    DeleteCriticalSection(&compressingStackCS);
    DeleteCriticalSection(&imageStacksCS);
    DeleteCriticalSection(&outfileCS);
    DeleteCriticalSection(&writingStackCS);
    DeleteCriticalSection(&inputBufferCS);
    DeleteCriticalSection(&compressedStacksCS);
    DeleteCriticalSection(&stacksToWriteCS);
}

void WindowsThreadStackCompressor::init() {
    maxCompressionThreads = 4;
    InitializeCriticalSection(&activeStackCS);
    InitializeCriticalSection(&compressingStackCS);
    InitializeCriticalSection(&imageStacksCS);
    InitializeCriticalSection(&outfileCS);
    InitializeCriticalSection(&writingStackCS);
    InitializeCriticalSection(&inputBufferCS);
    InitializeCriticalSection(&compressedStacksCS);
    InitializeCriticalSection(&stacksToWriteCS);
    compressionThread = NULL;
    writingThread = NULL;
    inputBufferThread = NULL;
    compressionThreadActive = false;
    writingThreadActive = false;
    inputBufferThreadActive = false;
    stacksLeftToCompress = false;
    stacksLeftToWrite = false;
    
     numTimesFrameIsNotRecorded=numTimesFrameIsCompressed=0;
     numTimesFramePassedToInputBuffer=numTimesFramePassedToStackCompressor=numTimesNewFrameCalled = 0;
    
}

int WindowsThreadStackCompressor::startThreads() {
    
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
    
    compressionThreadActive = true;
    writingThreadActive = true;
    inputBufferThreadActive = true;
    
    if (compressionThread == NULL) {
        compressionThread = (HANDLE) _beginthreadex(NULL, 0, WindowsThreadStackCompressor::startCompressionThread, this, 0, NULL);
    }
    if (writingThread == NULL) {
        writingThread = (HANDLE) _beginthreadex(NULL, 0, WindowsThreadStackCompressor::startWritingThread, this, 0, NULL);
    }
    if (inputBufferThread == NULL) {
        inputBufferThread = (HANDLE) _beginthreadex(NULL, 0, WindowsThreadStackCompressor::startInputBufferThread, this, 0, NULL);
    }
    
    if (compressionThread == NULL || writingThread == NULL|| inputBufferThread == NULL) {
        return -1;
    }
    return 0;
}
int WindowsThreadStackCompressor::checkIfThreadsAreActive(int timeout) {
    int rv = 0;
    if (inputBufferThread != NULL) {
        if (WaitForSingleObject(inputBufferThread, timeout) != WAIT_OBJECT_0) {
            rv += 1;
        }
    }
    if (compressionThread != NULL) {
        if (WaitForSingleObject(compressionThread, timeout)!= WAIT_OBJECT_0) {
            rv += 2;
        }
    }
    if (writingThread != NULL) {
        if (WaitForSingleObject(writingThread, timeout)!= WAIT_OBJECT_0) {
            rv += 4;
        }
    }
    return rv;    
}

int WindowsThreadStackCompressor::endThreads(int timeout) {
    int rv;
    int numtries = 3;
    writingThreadActive = false;
    compressionThreadActive = false;
    inputBufferThreadActive = false;
    for (int j = 0; ((rv = checkIfThreadsAreActive(timeout)) != 0) && j < numtries; ++j) {
        //blank
    }
    if (rv != 0) {
        if ((inputBufferThread != NULL) && WaitForSingleObject(inputBufferThread, 0) != WAIT_OBJECT_0) {
            TerminateThread(inputBufferThread, 999);
        }
        if ((compressionThread != NULL) && WaitForSingleObject(compressionThread, 0) != WAIT_OBJECT_0) {
            TerminateThread(compressionThread, 999);
        }
        if ((writingThread != NULL) && WaitForSingleObject(writingThread, 0) != WAIT_OBJECT_0) {
            TerminateThread(writingThread, 999);
        }
    }
    if (inputBufferThread != NULL) {
        CloseHandle(inputBufferThread);
        inputBufferThread = NULL;
    }
    if (compressionThread != NULL) {
        CloseHandle(compressionThread);
        compressionThread = NULL;
    }
    if (writingThread != NULL) {
        CloseHandle(writingThread);
        writingThread = NULL;
    }
    return rv;
}

bool WindowsThreadStackCompressor::readyForNewFrame() {
    EnterCriticalSection(&inputBufferCS);
    bool rv = (inputBufferLength() < keyframeInterval);
    LeaveCriticalSection(&inputBufferCS);
    return rv;
}

void WindowsThreadStackCompressor::newFrame(const IplImage* im, ImageMetaData *metadata) {

    nonthreadedTimer.tic("newFrame function");
    ++numTimesNewFrameCalled;
    EnterCriticalSection(&inputBufferCS);
    if (inputBufferLength() > keyframeInterval) {
        nonthreadedTimer.tic("discarding frame because input buffer is over-full");
        LeaveCriticalSection(&inputBufferCS);
        if (metadata != NULL) {
            delete metadata;
        }
        //Sleep(200/keyframeInterval);
        nonthreadedTimer.toc("discarding frame because input buffer is over-full");
        nonthreadedTimer.toc("newFrame function");
        return;
    }
    LeaveCriticalSection(&inputBufferCS);
    nonthreadedTimer.tic("cloning image");
    IplImage *imcpy = cvCloneImage(im);
    nonthreadedTimer.toc("cloning image");
   
    
    nonthreadedTimer.tic("adding frame to input buffer");
    
    EnterCriticalSection(&inputBufferCS);
    ++numTimesFramePassedToInputBuffer;
    inputBuffer.push(InputImT(imcpy,metadata));
    LeaveCriticalSection(&inputBufferCS);
    
    nonthreadedTimer.toc("adding frame to input buffer");
    
    if (recordingState == recording) {
        if(--framesToRecord <= 0) {
            finishRecording();
            return;
        }
    }
    nonthreadedTimer.toc("newFrame function");

}

void WindowsThreadStackCompressor::addFrameToStack(IplImage** im, ImageMetaData* metadata) {
    
    EnterCriticalSection(&activeStackCS);
    if (activeStack == NULL) {
        createStack();
    }
    if (activeStack->numToProccess() >= keyframeInterval) {

        /*****************imageStacksCS*******************/
        EnterCriticalSection(&imageStacksCS);
        imageStacks.push(activeStack);
        LeaveCriticalSection(&imageStacksCS);
        /*************************************************/

        createStack();
    }
    if (recordingState == recording) {
        ++numTimesFramePassedToStackCompressor;
        activeStack->addFrame(im, metadata);
    } else {
        ++numTimesFrameIsNotRecorded;
        if (recordingState == updatingBackground) {
            activeStack->updateBackground(*im);
        }
        if (metadata != NULL) {
            delete metadata;
        }
        cvReleaseImage(im);
    }
    LeaveCriticalSection(&activeStackCS);
}

void WindowsThreadStackCompressor::setCompressionStack() {
   EnterCriticalSection(&compressingStackCS);
   if (stackBeingCompressed != NULL && stackBeingCompressed->numToProccess() <= 0) {
        EnterCriticalSection(&compressedStacksCS);
        compressedStacks.push(stackBeingCompressed);
        memoryUsedByCompressedStacks += stackBeingCompressed->sizeInMemory();
        LeaveCriticalSection(&compressedStacksCS);
        stackBeingCompressed = NULL;
    }

   if (stackBeingCompressed == NULL && !imageStacks.empty()) {
        EnterCriticalSection(&imageStacksCS);
        stackBeingCompressed = imageStacks.front();
        imageStacks.pop();
        LeaveCriticalSection(&imageStacksCS);
    }      
   stacksLeftToCompress = (stackBeingCompressed != NULL);
   LeaveCriticalSection(&compressingStackCS);
}

unsigned __stdcall WindowsThreadStackCompressor::compressionThreadFunction() {
    
    while (compressionThreadActive) {
        
         compressionThreadTimer.tic("setting compression stack");        
         setCompressionStack();
         compressionThreadTimer.toc("setting compression stack");
         
         if (stacksLeftToCompress) {             
             EnterCriticalSection(&compressingStackCS);             
             if (stackBeingCompressed->framesWaitingToProcess()) {
                 compressionThreadTimer.tic("compressing a frame");
                 compressionThreadTimer.tic("waiting for thread availability");
                 if (stackBeingCompressed->readyToProcess()) {
                     compressionThreadTimer.toc("waiting for thread availability");
                     compressionThreadTimer.tic("calling process frame function");                     
                     stackBeingCompressed->processFrame();
                     ++numTimesFrameIsCompressed;                     
                     compressionThreadTimer.toc("calling process frame function");                     
                 } else {
                      compressionThreadTimer.toc("waiting for thread availability");
                 }
                     
                 compressionThreadTimer.toc("compressing a frame");
                 LeaveCriticalSection(&compressingStackCS);
             } else {
                 LeaveCriticalSection(&compressingStackCS);
                 Sleep((int) (1000/frameRate));
                 
             }
             
             
         } else {
             Sleep((int) (1000/frameRate));
         }
    }
     return 0;
}

unsigned __stdcall WindowsThreadStackCompressor::startCompressionThread(void* ptr) {
    WindowsThreadStackCompressor *wtsc = (WindowsThreadStackCompressor *) ptr;
    return wtsc->compressionThreadFunction();
}


void WindowsThreadStackCompressor::mergeCompressedStacks() {
  
    int target;
    const size_t maxMemoryUsedByCompressedStacks = 8E8;
    //if we are done recording and we don't have any more compressed stacks coming in, then merge any leftovers
    EnterCriticalSection(&imageStacksCS);
    EnterCriticalSection(&inputBufferCS);
    if (recordingState != recording && imageStacks.empty() && inputBuffer.empty() && activeStack == NULL) {
        target = 1;
    } else {
        target = numStacksToMerge;
    }
    LeaveCriticalSection(&inputBufferCS);
    LeaveCriticalSection(&imageStacksCS);
    
    //trigger compression if stacks are taking up too much memory
    if (memoryUsedByCompressedStacks > maxMemoryUsedByCompressedStacks) {
        target = std::min(compressedStacks.size(), (size_t) target);
    }
   
    target = target < 1 ? 1 : target;
    
    EnterCriticalSection(&compressedStacksCS);
   
    
    if (compressedStacks.size() >= target) {
   //     cout << "compressed stacks take up " << memoryUsedByCompressedStacks/(1024*1024) << " MB" << endl;
    //    cout << "merging compressed stacks" << endl << flush;
        writingThreadTimer.tic("merging compressed stacks");
        StaticBackgroundCompressor *sbc = compressedStacks.front();
        compressedStacks.pop();
        vector<StaticBackgroundCompressor *> stacksToMerge;
        for (int j = 1; !compressedStacks.empty() && j < numStacksToMerge; ++j) {
            stacksToMerge.push_back(compressedStacks.front());
            compressedStacks.pop();
        }
        memoryUsedByCompressedStacks *= (compressedStacks.size() / (compressedStacks.size() + numStacksToMerge)); //estimates, in the unusual case compressedStacks wasn't emptied
        LeaveCriticalSection(&compressedStacksCS);
        
        stacksLeftToWrite = true; //so we know we still have stacks out there to write, even though they're not in any of the vectors right now
     //   cout << "calling merge stacks" << endl << flush;
        sbc->mergeStacks(stacksToMerge);
     //   cout << "deleting sbcs" << endl << flush;
        for (vector<StaticBackgroundCompressor *>::iterator it = stacksToMerge.begin(); it != stacksToMerge.end(); ++it) {
            delete (*it);
            *it = NULL;
        }      
    //    cout << "pushing sbc onto stackstowrite" << endl << flush;
        
        EnterCriticalSection(&stacksToWriteCS);
        stacksToWrite.push(sbc);
        LeaveCriticalSection(&stacksToWriteCS);
        writingThreadTimer.toc("merging compressed stacks");
     //   cout << "done merging compressed stacks" << endl << flush;
    } else {
        LeaveCriticalSection(&compressedStacksCS);
    }
}


void WindowsThreadStackCompressor::setWritingStack() {
     /**************writingStackCS, stacksToWriteCS******************/
    EnterCriticalSection(&writingStackCS);
    EnterCriticalSection(&stacksToWriteCS);
    //if (!stacksToWrite.empty()) {
   //     cout << stacksToWrite.size() << " stacks left to write" << endl;
   // }
    if (stackBeingWritten == NULL && !stacksToWrite.empty()) {
    //    cout << "setting writing stack" << endl << flush;
        stackBeingWritten = stacksToWrite.front();
        stacksToWrite.pop();
     //   cout << "done setting writing stack" << endl << flush;
    }
    stacksLeftToWrite = (stackBeingWritten != NULL);
    LeaveCriticalSection(&stacksToWriteCS);
    LeaveCriticalSection(&writingStackCS);
    /*************************************************************/
}

unsigned __stdcall WindowsThreadStackCompressor::writingThreadFunction() {
    while (writingThreadActive) {
        
        mergeCompressedStacks();
        
        writingThreadTimer.tic("setting writing stack");       
        setWritingStack();
        writingThreadTimer.toc("setting writing stack");
        
        if (stacksLeftToWrite) {
         //   cout << "writing stack" << endl;

            /**********************writingStackCS******************/
            EnterCriticalSection(&writingStackCS);
            if (stacksavedescription.empty()) {
                writingThreadTimer.tic("creating save description");
                stacksavedescription = stackBeingWritten->saveDescription();
                writingThreadTimer.toc("creating save description");
    //          cout << "created save description\n";
            }
            /*********************+outfileCS***********************/

            EnterCriticalSection(&outfileCS);
            writingThreadTimer.tic("writing a stack to disk");
            if (outfile == NULL) {
                    openOutputFile();
            }
            if (outfile == NULL) {
                cout << "error opening output file" << endl;
                LeaveCriticalSection(&outfileCS);
                return 1;
            }

            //    cout << "writing a stack" << endl;
            stackBeingWritten->toDisk(*outfile);
            currentFileSize = outfile->tellp();
            writingThreadTimer.toc("writing a stack to disk");
            LeaveCriticalSection(&outfileCS);
            /*******************-outfile, writing still active****************/

           

            
            writingThreadTimer.tic("deleting written stack from memory");            
            delete stackBeingWritten;
            stackBeingWritten = NULL;
            LeaveCriticalSection(&writingStackCS);
             /**********************-writing********************************/
            writingThreadTimer.toc("deleting written stack from memory");

        //    cout << "done writing stack" << endl;
               
            }else {
                    Sleep((int) (1000/frameRate));
            }       
    }
    return 0;
}
unsigned __stdcall WindowsThreadStackCompressor::startWritingThread(void* ptr) {
    WindowsThreadStackCompressor *wtsc = (WindowsThreadStackCompressor *) ptr;
    wtsc->writingThreadTimer.tic("writing thread start called");
    wtsc->writingThreadTimer.toc("writing thread start called");
    return wtsc->writingThreadFunction();
}

unsigned __stdcall WindowsThreadStackCompressor::startInputBufferThread(void* ptr) {
    WindowsThreadStackCompressor *wtsc = (WindowsThreadStackCompressor *) ptr;
    
    wtsc->nonthreadedTimer.tic("input buffer thread start called");
    wtsc->nonthreadedTimer.toc("input buffer thread start called");
    
    wtsc->inputBufferThreadTimer.tic("input buffer thread start called");
    wtsc->inputBufferThreadTimer.toc("input buffer thread start called");
    
    return wtsc->inputBufferThreadFunction();
}

unsigned __stdcall WindowsThreadStackCompressor::inputBufferThreadFunction() {
    
    InputImT toAdd; 
    inputBufferThreadTimer.tic("input buffer thread function");
    while(inputBufferThreadActive) {
           EnterCriticalSection(&inputBufferCS);
            if (inputBufferEmpty()) {
                LeaveCriticalSection(&inputBufferCS);
                /***************[end inputBufferCS]****************/
                Sleep((int) (1000/frameRate + 0.5));
                continue;
            }
           LeaveCriticalSection((&inputBufferCS));
            
            int ntc, ntw;
            inputBufferThreadTimer.tic("getting number of stacks waiting");
            numStacksWaiting(ntc, ntw);
            inputBufferThreadTimer.toc("getting number of stacks waiting");
            switch (ntc) {
                case 0: case 1:
                    break;
                case 2:
                    inputBufferThreadTimer.tic("2 compression stacks waiting; sleeping");
                    Sleep((int) (1000/frameRate + 0.5));
                    inputBufferThreadTimer.toc("2 compression stacks waiting; sleeping");
                    break;
                default:
                    inputBufferThreadTimer.tic("more than 2 compression stacks waiting, or error; sleeping");
                    Sleep((int) (1000));
                    inputBufferThreadTimer.toc("more than 2 compression stacks waiting, or error; sleeping");
                    break;                    
            }
            inputBufferThreadTimer.tic("transferring frame from input buffer to stack");
            EnterCriticalSection(&inputBufferCS);
            toAdd = inputBuffer.front();
            inputBuffer.pop();
            LeaveCriticalSection((&inputBufferCS));
            /***************[end inputBufferCS]****************/
            addFrameToStack(&(toAdd.first), toAdd.second);
            inputBufferThreadTimer.toc("transferring frame from input buffer to stack");
    }
   inputBufferThreadTimer.toc("input buffer thread function");
    
    
    return 0;
}

bool WindowsThreadStackCompressor::inputBufferEmpty() {
    bool rv;
    EnterCriticalSection(&inputBufferCS);
    rv = inputBuffer.empty();
    LeaveCriticalSection(&inputBufferCS);
    return rv;
}

int WindowsThreadStackCompressor::inputBufferLength() {
    int rv;
    EnterCriticalSection(&inputBufferCS);
    rv = inputBuffer.size();
    LeaveCriticalSection(&inputBufferCS);
    return rv;
}

void WindowsThreadStackCompressor::goIdle() {
    nonthreadedTimer.tic("going idle");
    
    
    //wait for input buffer to empty out -- taken care of by inputBufferThreadFunction
    nonthreadedTimer.tic("waiting for input buffer to empty");
//    cout << "waiting for input buffer to empty" << endl;
    for(int j = 0; j < 10 && !inputBufferEmpty(); ++j) {
        Sleep(1000);
    }
 //   cout << "done waiting for input buffer to empty" << endl;
    nonthreadedTimer.toc("waiting for input buffer to empty");
    recordingState = idle;
    
    
    /*****************activeStackCS*********************/
    EnterCriticalSection(&activeStackCS);
    if (activeStack != NULL) {
        if (activeStack->numToProccess() > 0) {
            /********imageStackCS (+activeStack)*********/
            EnterCriticalSection(&imageStacksCS);
            imageStacks.push(activeStack);
            LeaveCriticalSection(&imageStacksCS);
            /**********-imageStack (activeStack still on)*************/
        } else {
            delete (activeStack);
        }
        activeStack = NULL;
    }
    LeaveCriticalSection(&activeStackCS);
    nonthreadedTimer.toc("going idle");
}
void WindowsThreadStackCompressor::emptyInputBuffer() {
    EnterCriticalSection(&inputBufferCS);
    while (!inputBuffer.empty()) {
        InputImT it = inputBuffer.front();
        cvReleaseImage(&(it.first));
        if (it.second != NULL) {
                delete (it.second);
        }
        inputBuffer.pop();
    }
    LeaveCriticalSection(&inputBufferCS);
    
}

bool WindowsThreadStackCompressor::nothingLeftToCompressOrWrite(bool verbose) {
    if (recordingState == recording) {
        if (verbose) {
            cout << "still recording" << endl;
        }
        //more images may be coming, so we're not done
        return false;
    }
    bool done = false;
  //  cout << "input buffer" << endl;
    EnterCriticalSection(&inputBufferCS);
    done = inputBuffer.empty();
    LeaveCriticalSection(&inputBufferCS);
    if (!done) {
        if (verbose) {
            cout << "input buffer not empty" << endl;
        }
        return false;
    }
    
   // cout << "active stack" << endl;
    EnterCriticalSection(&activeStackCS);
    done = (activeStack == NULL || activeStack->numToProccess() <= 0);
    LeaveCriticalSection(&activeStackCS);
    if (!done) {
        if (verbose) {
            cout << "active stack not NULL or still needs processing" << endl;
        }
        return false;
    }
    
  //  cout << "image stack" << endl << flush;
    EnterCriticalSection(&imageStacksCS);
   // cout << "image stack entered" << endl;
    done = imageStacks.empty();
    LeaveCriticalSection(&imageStacksCS);
   // cout << "image stack left" << endl;
    if (!done) {
        if (verbose) {
            cout << "image stacks not empty" << endl << flush;
        }
        return false;
    }
   // cout << "compressed stack" << endl << flush;
    EnterCriticalSection(&compressedStacksCS);
    done = compressedStacks.empty();
    LeaveCriticalSection(&compressedStacksCS);
    if (!done) {
         if (verbose) {
            cout << "compressed stacks not empty" << endl;
        }
        return false;
    }
    
  //  cout << "stackstowrite" << endl;
    EnterCriticalSection(&stacksToWriteCS);
    done = stacksToWrite.empty();
    LeaveCriticalSection(&stacksToWriteCS);
    if (!done) {
         if (verbose) {
            cout << "stacks to write not empty" << endl;
        }
        return false;
    }
    if (verbose && stacksLeftToCompress) {
        cout << "stacksLeftToCompress" << endl;
    }
    if (verbose && stacksLeftToWrite) {
        cout << "stacksLeftToWrite" << endl;
    }
    return !(stacksLeftToCompress || stacksLeftToWrite);
}

void WindowsThreadStackCompressor::finishRecording() {
    nonthreadedTimer.tic("finishing recording");
    goIdle();
    emptyInputBuffer();
    double timeoutInSeconds = 40;
    for (int j = 0; j < (int) (timeoutInSeconds*10 + 0.5) && !nothingLeftToCompressOrWrite(); ++j) {
//        if (!((j+1)%10)){
//            nothingLeftToCompressOrWrite(true);
//        }
        Sleep(100);
    } 
  //  cout << "finished recording " << endl;
     nonthreadedTimer.toc("finishing recording");
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
    ss << numTimesNewFrameCalled << " images passed to stack compressor" << endl;
    ss << numTimesFramePassedToInputBuffer << " images passed to input buffer (others were discarded because input was too fast)" << endl;
    ss << numTimesFramePassedToStackCompressor << " images were passed to static background compressor for compression" << endl;
    ss << numTimesFrameIsNotRecorded << " images were discarded because recording state was idle" << endl;
    ss << numTimesFrameIsCompressed << " images were compressed by static background compressor" << endl;
    
    ss << "main thread: " << endl<< nonthreadedTimer.generateReport() 
            << "compression thread: " << endl << compressionThreadTimer.generateReport() 
            << "writing thread: " << endl << writingThreadTimer.generateReport()
            << "input buffer thread: " << endl << inputBufferThreadTimer.generateReport();
    
    const char *version[100], *addons[100];
    for (int j = 0; j < 100; ++j) {
        version[j] = addons[j] = NULL;
    }
    cvGetModuleInfo(NULL, version, addons); 
    //The returned information is stored inside the libraries, so user should not deallocate or modify the returned text strings.
    for (int j = 0; j < 100 && version[j] != 0; ++j) {
        ss << version[j] << endl;
    }
    for (int j = 0; j < 100 && addons[j] != 0; ++j) {
        ss << addons[j] << endl;
    }
    
    return ss.str();
}



void WindowsThreadStackCompressor::numStacksWaiting(int& numToCompress, int& numToWrite) {
    
    EnterCriticalSection(&imageStacksCS); 
    numToCompress = imageStacks.size();
    LeaveCriticalSection(&imageStacksCS);
    
    EnterCriticalSection(&compressedStacksCS);
    numToWrite = compressedStacks.size(); 
    LeaveCriticalSection(&compressedStacksCS);
    
    EnterCriticalSection(&stacksToWriteCS);
    numToWrite += stacksToWrite.size();
    LeaveCriticalSection(&stacksToWriteCS);
    
}

void WindowsThreadStackCompressor::createStack() {
    if (maxCompressionThreads > 1) {
        activeStack = new WindowsThreadedStaticBackgroundCompressor(maxCompressionThreads);
    } else {
        activeStack = new StaticBackgroundCompressor();
    }
    activeStack->setAutomaticUpdateInterval(backgroundUpdateInterval);
    activeStack->setThresholds(threshBelowBackground, threshAboveBackground, smallDimMinSize, lgDimMinSize);
}

#endif