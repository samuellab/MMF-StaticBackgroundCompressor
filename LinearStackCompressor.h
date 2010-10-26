/* 
 * File:   LinearStackCompressor.h
 * Author: Marc
 *
 * Created on October 24, 2010, 3:29 PM
 *
 * compresses images as they are added to the stack without explicit multi-threading
 * but it is designed to be used as a callback to the mightex api, in which case it will become
 * multi-threaded
 */

#ifndef LINEARSTACKCOMPRESSOR_H
#define	LINEARSTACKCOMPRESSOR_H

#include "StaticBackgroundCompressor.h"


class LinearStackCompressor {
public:
    static const int headerSizeInBytes = 1024;
    LinearStackCompressor();
   
    virtual ~LinearStackCompressor();
    virtual void newFrame(const IplImage *im);

    virtual void setOutputFileName (const char *fname);
    virtual void openOutputFile ();
    virtual void closeOutputFile ();
    virtual void startRecording(int numFramesToRecord);
    virtual void stopRecording();
    virtual void startUpdatingBackground();
    virtual void goIdle();
    inline void setThresholds(int threshBelowBackground, int threshAboveBackground) {
        this->threshAboveBackground = threshAboveBackground;
        this->threshBelowBackground = threshBelowBackground;
    }
    inline void setIntervals (int keyframeInterval, int backgroundUpdateInterval = 1) {
        this->keyframeInterval = keyframeInterval;
        this->backgroundUpdateInterval = backgroundUpdateInterval;
    }

protected:
    int keyframeInterval;
    int backgroundUpdateInterval;
    enum recordingState_t {idle, updatingBackground, recording};
    recordingState_t recordingState;
    int framesToRecord;
    std::ofstream *outfile;
    std::vector<StaticBackgroundCompressor *> imageStacks;
    StaticBackgroundCompressor *activeStack;
    StaticBackgroundCompressor *stackBeingCompressed;
    double frameRate;
    int threshBelowBackground;
    int threshAboveBackground;
    std::string fname;
    bool processing; //really should be a mutex, but whatever
    bool lockActiveStack; //really should be a mutex, but whatever

    std::string stacksavedescription;

    virtual void createStack();
    virtual void addFrameToStack(const IplImage *im);
    virtual bool compressStack();
    virtual bool writeFinishedStack();
    virtual void setCompressionStack();
    virtual bool readyForCompression (StaticBackgroundCompressor *sc);
    virtual bool readyForWriting (StaticBackgroundCompressor *sc);
    virtual void finishRecording ();

    virtual void init();
    virtual void writeHeader();
    
private:
     LinearStackCompressor(const LinearStackCompressor& orig);
};

#endif	/* LINEARSTACKCOMPRESSOR_H */

