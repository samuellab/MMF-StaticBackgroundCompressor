/* 
 * File:   LinearStackCompressor.cpp
 * Author: Marc
 * 
 * Created on October 24, 2010, 3:29 PM
 */

#include "LinearStackCompressor.h"
#include "tictoc/Timer.h"
#include <vector>
#include <string>
using namespace std;

LinearStackCompressor::LinearStackCompressor() {
    init();
}

LinearStackCompressor::LinearStackCompressor(const LinearStackCompressor& orig) {
}

LinearStackCompressor::~LinearStackCompressor() {
   closeOutputFile();
   vector<StaticBackgroundCompressor *>::iterator it;
   for (it = imageStacks.begin(); it != imageStacks.end(); ++it) {
       if ((*it) != NULL && (*it) != activeStack) {
           delete *it;
           *it = NULL;
       }
   }
   imageStacks.clear();
   if (activeStack != NULL) {
       delete activeStack;
   }


}

void LinearStackCompressor::init() {
    keyframeInterval = 64;
    backgroundUpdateInterval = 1;
    recordingState = idle;
    framesToRecord = 0;
    outfile = NULL;
    activeStack = NULL;
    stackBeingCompressed = NULL;
    frameRate = 1;
    threshBelowBackground = 5;
    threshAboveBackground = 5;
    processing = false; //really should be a mutex, but whatever
    lockActiveStack = false; //really should be a mutex, but whatever
}

void LinearStackCompressor::newFrame(const IplImage* im) {
    int maxCycles = (int) 1E9; //just so it doesn't hang
    Timer tim = Timer();
    tim.start();
    while (lockActiveStack && --maxCycles > 0) {
        //intentionally blank
    }
  //  cout << "lockActiveStack = " << lockActiveStack << "; processing = " << processing << "\n";

    lockActiveStack = true;
    addFrameToStack(im);
    lockActiveStack = false;

    if (recordingState == recording) {
        if(--framesToRecord <= 0) {
            finishRecording();
            return;
        }
    }


    if (!processing) {
        processing = true;
    //    cout << tim.getElapsedTimeInSec() << " s elapsed\n";
        while (tim.getElapsedTimeInSec() < 0.95/frameRate && compressStack()) {
        //intentionally blank       
        }
        while (tim.getElapsedTimeInSec() < 0.95/frameRate && writeFinishedStack()) {
            //intentionally blank
        }
        processing = false;
    }
}

void LinearStackCompressor::addFrameToStack(const IplImage* im) {
    if (activeStack == NULL) {
        createStack();
    }
    if (activeStack->numToProccess() >= keyframeInterval) {
    //    cout << "pusing active stack on imstacks\n";
        imageStacks.push_back(activeStack);
        if (stacksavedescription.empty()) {
   //         cout << "storing save description as string\n";
            stacksavedescription = activeStack->saveDescription();
        }
 //       cout << "creating new stack\n";
        createStack();
    }
    if (recordingState == recording) {
        activeStack->addFrame(im);
    }
    if (recordingState == updatingBackground) {
        activeStack->updateBackground(im);
    }
}

void LinearStackCompressor::startRecording(int numFramesToRecord) {
    recordingState = recording;
    this->framesToRecord = numFramesToRecord;
}

void LinearStackCompressor::finishRecording() {
    int maxCycles = (int) 1E9; //just so it doesn't hang
    while (processing && --maxCycles > 0) {
        //intentionally blank
    }
    processing = true;
    lockActiveStack = true;

    recordingState = idle;
    if (activeStack != NULL) {
        if (activeStack->numToProccess() > 0) {
            imageStacks.push_back(activeStack);
        } else {
            delete activeStack;
        }
        activeStack = NULL;
    }
    while (compressStack()) {
        //intentionally blank
    }
    while (writeFinishedStack()) {
        //intentionally blank
    }
   

    processing = false;
    lockActiveStack = false;
}

void LinearStackCompressor::createStack() {
    activeStack = new StaticBackgroundCompressor();
    activeStack->setAutomaticUpdateInterval(backgroundUpdateInterval);
    activeStack->setThresholds(threshBelowBackground, threshAboveBackground);
}

//returns true if there may be images remaining to compress
bool LinearStackCompressor::compressStack() {
     setCompressionStack();
 //    cout << "stack being compressed = " << (int) stackBeingCompressed << "\n";
     if (stackBeingCompressed != NULL) {
        // cout << "processing compression frame\n";
         stackBeingCompressed->processFrame();
         return true;
     } else {
         return false;
     }
}

void LinearStackCompressor::setCompressionStack() {
   vector<StaticBackgroundCompressor *>::iterator it;
   for (it = imageStacks.begin(); it != imageStacks.end(); ++it) {
       if (readyForCompression(*it)) {
       //    cout << "found one ready for compression\n";
           stackBeingCompressed = *it;
           return;
       }
   }
 //  cout << "no one is ready for compression\n";
   stackBeingCompressed = NULL;
}

//returns true if there may be stacks remaining to write
bool LinearStackCompressor::writeFinishedStack() {
   vector<StaticBackgroundCompressor *>::iterator it;
   for (it = imageStacks.begin(); it != imageStacks.end(); ++it) {
       if (readyForWriting(*it)) {
           if (outfile == NULL) {
        //       cout << "opening output file\n";
               openOutputFile();
           }
           if (outfile == NULL) {
               return false;
           }
           StaticBackgroundCompressor *sbc = *it;
      //     cout << "writing to disk\n";
           ofstream::pos_type sp = outfile->tellp();
           int sod = sbc->sizeOnDisk();
           sbc->toDisk(*outfile);
      //     cout << "wrote " << outfile->tellp() - sp << "/" << sbc->sizeOnDisk() << "\n";
     //      cout << "deleting sbc\n";
           delete sbc;
           imageStacks.erase(it);
           return true;
           
        }
   }
   return false;
}

bool LinearStackCompressor::readyForCompression(StaticBackgroundCompressor* sc) {
 //   if (sc != NULL) {
//        cout << "num to compress = " << sc->numToProccess() << "\n";
 //   }
    return (sc != NULL && sc != activeStack && sc->numToProccess() > 0);
}

bool LinearStackCompressor::readyForWriting(StaticBackgroundCompressor* sc) {
    return (sc != NULL && sc != activeStack && sc != stackBeingCompressed && sc->numToProccess() == 0 && sc->numProcessed() > 0);
}

void LinearStackCompressor::openOutputFile() {
    closeOutputFile();
    if (!fname.empty()) {
        outfile = new ofstream (fname.c_str(),ofstream::binary);
        writeHeader();
    }
}

void LinearStackCompressor::closeOutputFile() {
     if (outfile != NULL) {
        outfile->close();
        delete outfile;
        outfile = NULL;
    }
}

void LinearStackCompressor::setOutputFileName(const char* fname) {
    if (fname != NULL) {
        this->fname = string(fname);
    }
}

void LinearStackCompressor::stopRecording() {
    if (recording) {
        finishRecording();
    }
    recordingState = idle;
}

void LinearStackCompressor::goIdle() {
    finishRecording();
}

void LinearStackCompressor::startUpdatingBackground() {
    recordingState = updatingBackground;
}

void LinearStackCompressor::writeHeader() {
    if (outfile == NULL) {
        return;
    }
    char zero[headerSizeInBytes] = {0};
    ofstream::pos_type cloc = outfile->tellp();
    outfile->write(zero, headerSizeInBytes);
    outfile->seekp(cloc);
    int info[10] = {0};
    info[0] = keyframeInterval;
    info[1] = threshBelowBackground;
    info[2] = threshAboveBackground;
    outfile->seekp(headerSizeInBytes);

}
