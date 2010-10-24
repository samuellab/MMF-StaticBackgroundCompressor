/* 
 * File:   LinearStackCompressor.h
 * Author: Marc
 *
 * Created on October 24, 2010, 3:29 PM
 *
 * compresses images as they are added to the stack without multi-threading
 */

#ifndef LINEARSTACKCOMPRESSOR_H
#define	LINEARSTACKCOMPRESSOR_H

#include "StaticBackgroundCompressor.h"


class LinearStackCompressor {
public:
    LinearStackCompressor();
   
    virtual ~LinearStackCompressor();
    virtual void newFrame(const IplImage *im);

    virtual void setOutputFileName (const char *fname);
    virtual void openOutputFile ();
    virtual void startRecording(int numFramesToRecord);
    virtual void stopRecording();
    virtual void startUpdatingBackground();
    virtual void goIdle();
    

protected:
    int keyframeInterval;
    int backgroundUpdateInterval;
    enum recordingState {idle, updatingBackground, recording};
    int framesToRecord;
    std::ofstream *outfile;
    std::vector<StaticBackgroundCompressor *> imageStacks;
    double frameRate;
    

private:
     LinearStackCompressor(const LinearStackCompressor& orig);
};

#endif	/* LINEARSTACKCOMPRESSOR_H */

