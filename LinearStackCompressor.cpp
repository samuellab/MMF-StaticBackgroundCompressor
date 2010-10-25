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
}

LinearStackCompressor::LinearStackCompressor(const LinearStackCompressor& orig) {
}

LinearStackCompressor::~LinearStackCompressor() {
}

void LinearStackCompressor::init() {
    keyframeInterval = 64;
    backgroundUpdateInterval = 1;
    recordingState = idle;
    framesToRecord = 0;
    outfile = NULL;
    imageStacks = new vector<StaticBackgroundCompressor *>;
    activeStack = NULL;
    stackBeingCompressed = NULL;
    frameRate = 5;
    threshBelowBackground = 5;
    threshAboveBackground = 5;
    processing = false; //really should be a mutex, but whatever
    lockActiveStack = false; //really should be a mutex, but whatever
}

void LinearStackCompressor::newFrame(const IplImage* im) {
    int maxCycles = 1E9; //just so it doesn't hang
    Timer tim = Timer();
    tim.start();
    while (lockActiveStack && --maxCycles > 0) {
        //intentionally blank
    }

    lockActiveStack = true;
    addFrameToStack(im);
    lockActiveStack = false;

    if (processing) {
        return; // don't do anything if someone else is still trying to work
    }
    processing = true;
    while (tim.getElapsedTimeInSec() < 0.95/frameRate && compressStack()) {
        //intentionally blank
       
    }
    while (tim.getElapsedTimeInSec() < 0.95/frameRate && writeFinishedStack()) {
        //intentionally blank
    }
    processing = false;
}

void LinearStackCompressor::addFrameToStack(const IplImage* im) {
    if (activeStack == NULL) {
        createStack();
    }
    if (activeStack->numToProccess() >= keyframeInterval) {
        imageStacks->push_back(activeStack);
        createStack();
    }
    if (recordingState == recording) {
        activeStack->addFrame(im);
    }
    if (recordingState == updatingBackground) {
        activeStack->updateBackground(im);
    }
}

void LinearStackCompressor::finishRecording() {
    recordingState = idle;
    if (activeStack != NULL) {
        if (activeStack->numToProccess() > 0) {
            imageStacks->push_back(activeStack);
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
    if (outfile != NULL) {
        outfile->close();
        delete outfile;
        outfile = NULL;
    }
}

void LinearStackCompressor::createStack() {
    activeStack = new StaticBackgroundCompressor();
    activeStack->setAutomaticUpdateInterval(backgroundUpdateInterval);
    activeStack->setThresholds(threshBelowBackground, threshAboveBackground);
}

//returns true if there may be images remaining to compress
bool LinearStackCompressor::compressStack() {
     setCompressionStack();
     if (stackBeingCompressed != NULL) {
         stackBeingCompressed->processFrame();
         return true;
     } else {
         return false;
     }
}

void LinearStackCompressor::setCompressionStack() {
   vector<StaticBackgroundCompressor *>::iterator it;
   for (it = imageStacks->begin(); it != imageStacks.end(); ++it) {
       StaticBackgroundCompressor *sbc;
       if (readyForCompression(*it)) {
           stackBeingCompressed = *it;
           return;
       }
   }
   stackBeingCompressed == NULL;
}

//returns true if there may be stacks remaining to write
bool LinearStackCompressor::writeFinishedStack() {
   vector<StaticBackgroundCompressor *>::iterator it;
   for (it = imageStacks->begin(); it != imageStacks.end(); ++it) {
       if (readyForWriting(*it)) {
           openOutputFile();
           if (outfile == NULL) {
               return false;
           }
           StaticBackgroundCompressor *sbc = *it;
           sbc->toDisk(*outfile);
           delete sbc;
           imageStacks->erase(it);
           return true;
           
        }
   }
   return false;
}

bool LinearStackCompressor::readyForCompression(StaticBackgroundCompressor* sc) {
    return (sc != NULL && sc != activeStack && sc->numToProccess() > 0);
}

bool LinearStackCompressor::readyForWriting(StaticBackgroundCompressor* sc) {
    return (sc != NULL && sc != activeStack && sc != stackBeingCompressed && sc->numToProccess() == 0 && sc->numProcessed() > 0);
}

