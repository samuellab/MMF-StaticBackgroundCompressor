/* 
 * File:   LinearStackCompressor.cpp
 * Author: Marc
 * 
 * Created on October 24, 2010, 3:29 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#include "LinearStackCompressor.h"
#include "tictoc/Timer.h"
#include <vector>
#include <string>
#include <sstream>
#include <queue>
using namespace std;

LinearStackCompressor::LinearStackCompressor() {
    init();
}

LinearStackCompressor::LinearStackCompressor(const LinearStackCompressor& orig) {
}

LinearStackCompressor::~LinearStackCompressor() {
   closeOutputFile();
   while (!imageStacks.empty()) {
       delete imageStacks.front();
       imageStacks.pop();
   }
   while (!compressedStacks.empty()) {
       delete compressedStacks.front();
       compressedStacks.pop();
   }
   while (!stacksToWrite.empty()) {
       delete stacksToWrite.front();
       stacksToWrite.pop();
   }
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
    stackBeingWritten = NULL;
    frameRate = 30;
    threshBelowBackground = 5;
    threshAboveBackground = 5;
    lgDimMinSize = 2;
    smallDimMinSize = 3;
    processing = false; //really should be a mutex, but whatever
    lockActiveStack = false; //really should be a mutex, but whatever
    currentFileSize = 0;
    numStacksToMerge = 12;
    memoryUsedByCompressedStacks = 0;
    
}

void LinearStackCompressor::newFrame(const IplImage* im, ImageMetaData *metadata) {
    int maxCycles = (int) 1E9; //just so it doesn't hang
    Timer tim = Timer();
    tim.start();

    IplImage *imcpy = cvCloneImage(im);
    while (lockActiveStack && --maxCycles > 0) {
        //intentionally blank
    }
  //  cout << "lockActiveStack = " << lockActiveStack << "; processing = " << processing << "\n";


    lockActiveStack = true;
    addFrameToStack(&imcpy, metadata);
    lockActiveStack = false;

    if (recordingState == recording) {
        if(--framesToRecord <= 0) {
            finishRecording();
            return;
        }
    }


    if (!processing) {
        processing = true;
        while (tim.getElapsedTimeInSec() < 0.95/frameRate && compressStack()) {
        //intentionally blank       
        }
        while (tim.getElapsedTimeInSec() < 0.95/frameRate && writeFinishedStack()) {
            //intentionally blank
        }
        processing = false;
    }
}

void LinearStackCompressor::addFrameToStack(IplImage **im, ImageMetaData *metadata) {
    if (activeStack == NULL) {
        createStack();
    }
    if (activeStack->numToProccess() >= keyframeInterval) {
        imageStacks.push(activeStack);        
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
            imageStacks.push(activeStack);
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
    activeStack->setThresholds(threshBelowBackground, threshAboveBackground, smallDimMinSize, lgDimMinSize);
}

//returns true if there may be images remaining to compress
bool LinearStackCompressor::compressStack() {
     setCompressionStack();
     if (stackBeingCompressed != NULL) {
         stackBeingCompressed->processFrame();
         return true;
     } 
         
     return false;
}



void LinearStackCompressor::numStacksWaiting(int& numToCompress, int& numToWrite) {
    
    numToCompress = imageStacks.size();
    numToWrite = compressedStacks.size() + stacksToWrite.size();
    
}

void LinearStackCompressor::setCompressionStack() {
   
   if (stackBeingCompressed != NULL && stackBeingCompressed->numToProccess() <= 0) {
        compressedStacks.push(stackBeingCompressed);
        memoryUsedByCompressedStacks += stackBeingCompressed->sizeInMemory();
        stackBeingCompressed = NULL;
    }

   if (stackBeingCompressed == NULL && !imageStacks.empty()) {
        stackBeingCompressed = imageStacks.front();
        imageStacks.pop();
    }      
}

//returns true if there may be stacks remaining to write
bool LinearStackCompressor::writeFinishedStack() {
    mergeCompressedStacks();
    setWritingStack();
    if (stackBeingWritten != NULL) {
        if (outfile == NULL) {
            openOutputFile();
        }
        if (outfile == NULL) {
            return false;
        }

        if (stacksavedescription.empty()) {
            stacksavedescription = stackBeingWritten->saveDescription();
        }
        stackBeingWritten->toDisk(*outfile);
        currentFileSize = outfile->tellp();
        delete stackBeingWritten;
        stackBeingWritten = NULL;    
        return true;          
    }
   return false;
}
    

ofstream::pos_type LinearStackCompressor::numBytesWritten() {

    return currentFileSize;
}


void LinearStackCompressor::openOutputFile() {
    if (outfile != NULL) {
        LinearStackCompressor::closeOutputFile();
    }
    if (!fname.empty()) {
        outfile = new ofstream (fname.c_str(),ofstream::binary);
        writeHeader();
    }
}

void LinearStackCompressor::closeOutputFile() {
     if (outfile != NULL) {
        outfile->seekp(0);
        writeHeader(); //now that we have an updated save description
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
    //recordingState = idle;
    finishRecording();
}

void LinearStackCompressor::goIdle() {
    stopRecording();
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
    ofstream::pos_type eloc = outfile->tellp();
    outfile->seekp(cloc);

    string sd = saveDescription();
    outfile->write(sd.c_str(), sd.length() + 1);
    uint32_t idcode = idCode();
    outfile->write((char *) &idcode, sizeof(idcode));
    int info[10] = {0};
    info[0] = headerSizeInBytes;
    info[1] = keyframeInterval;
    info[2] = threshBelowBackground;
    info[3] = threshAboveBackground;
    outfile->write((char *) info, sizeof(info));
    outfile->seekp(eloc);

}

string LinearStackCompressor::headerDescription() {
    stringstream os;
    os << headerSizeInBytes << " byte zero padded header beginning with a textual description of the file, followed by \\0 then the following fields (all ints, except idcode)\n";
    os << sizeof(uint32_t) << " byte uint32_t idcode = " << hex << idCode() << dec << ", header size in bytes, key frame interval, threshold below background, threshold above background\n";
    return os.str();
}

string LinearStackCompressor::saveDescription() {
    stringstream os;
    os << "Set of Image Stacks representing a movie. Beginning of file is a header, with this format:\n" << headerDescription();
    os << "Header is followed by a set of common background image stacks, with the following format:\n" << stacksavedescription;
    return os.str();
}

void LinearStackCompressor::mergeCompressedStacks() {
  
    
    if ((recordingState != recording && !compressedStacks.empty()) || compressedStacks.size() >= numStacksToMerge) {
        cout << "merging stacks " << endl << flush;
        StaticBackgroundCompressor *sbc = compressedStacks.front();
        compressedStacks.pop();
        vector<StaticBackgroundCompressor *> stacksToMerge;
        for (int j = 1; !compressedStacks.empty() && j < numStacksToMerge; ++j) {
            stacksToMerge.push_back(compressedStacks.front());
            compressedStacks.pop();
        }
        memoryUsedByCompressedStacks *= (compressedStacks.size() / (compressedStacks.size() + numStacksToMerge)); //estimates, in the unusual case compressedStacks wasn't emptied
        sbc->mergeStacks(stacksToMerge);
        for (vector<StaticBackgroundCompressor *>::iterator it = stacksToMerge.begin(); it != stacksToMerge.end(); ++it) {
            delete (*it);
            *it = NULL;
        }      
        stacksToWrite.push(sbc);
        cout << "done merging stacks " << endl << flush;
    }
}






void LinearStackCompressor::setWritingStack() {
     
    if (stackBeingWritten == NULL && !stacksToWrite.empty()) {
        stackBeingWritten = stacksToWrite.front();
        stacksToWrite.pop();
    }
    
}