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
#include <sstream>
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
    stackBeingWritten = NULL;
    frameRate = 1;
    threshBelowBackground = 5;
    threshAboveBackground = 5;
    lgDimMinSize = 2;
    smallDimMinSize = 3;
    processing = false; //really should be a mutex, but whatever
    lockActiveStack = false; //really should be a mutex, but whatever
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

void LinearStackCompressor::addFrameToStack(IplImage **im, ImageMetaData *metadata) {
    if (activeStack == NULL) {
        createStack();
    }
    if (activeStack->numToProccess() >= keyframeInterval) {
    //    cout << "pusing active stack on imstacks\n";
        imageStacks.push_back(activeStack);
        
 //       cout << "creating new stack\n";
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
    activeStack->setThresholds(threshBelowBackground, threshAboveBackground, smallDimMinSize, lgDimMinSize);
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
               openOutputFile();
           }
           if (outfile == NULL) {
               return false;
           }
           StaticBackgroundCompressor *sbc = *it;
           ofstream::pos_type sp = outfile->tellp();
           int sod = sbc->sizeOnDisk();
           if (stacksavedescription.empty()) {
                stacksavedescription = sbc->saveDescription();
            }
           //cout << "sbc to disk\n";
           sbc->toDisk(*outfile);
         //  cout << "delete sbc\n";
           delete sbc;
       //    cout << "imageStacks.erase\n";
           imageStacks.erase(it);
      //     cout << "return\n";
           return true;
           
        }
   }
   return false;
}

ofstream::pos_type LinearStackCompressor::numBytesWritten() {
    if (outfile == NULL) {
        return 0;
    }
    ofstream::pos_type rv = outfile->tellp();
    if (rv > 0) {
        return rv;
    } else {
        return 0;
    }
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
    recordingState = idle;
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
    unsigned long idcode = idCode();
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
    os << sizeof(unsigned long) << " byte unsigned long idcode = " << hex << idCode() << dec << ", header size in bytes, key frame interval, threshold below background, threshold above background\n";
    return os.str();
}

string LinearStackCompressor::saveDescription() {
    stringstream os;
    os << "Set of Image Stacks representing a movie. Beginning of file is a header, with this format:\n" << headerDescription();
    os << "Header is followed by a set of common background image stacks, with the following format:\n" << stacksavedescription;
    return os.str();
}

void LinearStackCompressor::setWritingStack() {
   vector<StaticBackgroundCompressor *>::iterator it;
   for (it = imageStacks.begin(); it != imageStacks.end(); ++it) {
       if (readyForWriting(*it)) {
           stackBeingWritten = *it;
           return;
       }
   }
   stackBeingWritten = NULL;
}