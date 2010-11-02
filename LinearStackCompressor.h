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
    static const int headerSizeInBytes = 10240;
    LinearStackCompressor();
   
    virtual ~LinearStackCompressor();
    virtual void newFrame(const IplImage *im, ImageMetaData *metadata = NULL);

    virtual void setOutputFileName (const char *fname);
    virtual void openOutputFile ();
    virtual void closeOutputFile ();
    virtual void startRecording(int numFramesToRecord);
    virtual void stopRecording();
    virtual void startUpdatingBackground();
    virtual void goIdle();
    virtual inline void setThresholds(int threshBelowBackground, int threshAboveBackground, int smallDimMinSize, int lgDimMinSize) {
        this->threshAboveBackground = threshAboveBackground;
        this->threshBelowBackground = threshBelowBackground;
        this->smallDimMinSize = smallDimMinSize;
        this->lgDimMinSize = lgDimMinSize;
    }
    virtual inline void setIntervals (int keyframeInterval, int backgroundUpdateInterval = 1) {
        this->keyframeInterval = keyframeInterval;
        this->backgroundUpdateInterval = backgroundUpdateInterval;
    }
    virtual inline void setFrameRate (int frameRate) {
        this->frameRate = frameRate;
    }

    virtual std::string saveDescription();

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
    int smallDimMinSize;
    int lgDimMinSize;
    std::string fname;
    bool processing; //really should be a mutex, but whatever
    bool lockActiveStack; //really should be a mutex, but whatever

    std::string stacksavedescription;

    virtual void createStack();
    virtual void addFrameToStack(const IplImage *im, ImageMetaData *metadata);
    virtual bool compressStack();
    virtual bool writeFinishedStack();
    virtual void setCompressionStack();
    virtual bool readyForCompression (StaticBackgroundCompressor *sc);
    virtual bool readyForWriting (StaticBackgroundCompressor *sc);
    virtual void finishRecording ();

    virtual void init();
    virtual void writeHeader();
    virtual std::string headerDescription();
    virtual inline unsigned long idCode () {
        return 0xa3d2d45d; //CRC32 hash of "LinearStackCompressor" from fileformat.info
    }
private:
     LinearStackCompressor(const LinearStackCompressor& orig);
};

#endif	/* LINEARSTACKCOMPRESSOR_H */

