/* 
 * File:   StaticBackgroundCompressor.h
 * Author: Marc
 *
 * Created on October 4, 2010, 1:06 PM
 */

#ifndef STATICBACKGROUNDCOMPRESSOR_H
#define	STATICBACKGROUNDCOMPRESSOR_H

#include <vector>
#include "BackgroundRemovedImage.h"
#include "cv.h"
#include <iostream>
#include <fstream>

class StaticBackgroundCompressor {
public:

    virtual void addFrame (const IplImage *im);
    virtual int processFrame();
    virtual void processFrames();
    virtual void calculateBackground();
    virtual void updateBackground(const IplImage *im);
    virtual void toDisk (std::ofstream &os);
    virtual int sizeOnDisk();
    StaticBackgroundCompressor();
    virtual ~StaticBackgroundCompressor();
    static StaticBackgroundCompressor *fromDisk(std::ifstream& is);
    static void writeIplImageToByteStream (std::ofstream &os, const IplImage *src);
    static IplImage *readIplImageFromByteStream(std::ifstream &is);
    inline void setThresholds(int threshBelowBackground, int threshAboveBackground) {
        this->threshAboveBackground = threshAboveBackground;
        this->threshBelowBackground = threshBelowBackground;
    }
    virtual void playMovie (char *windowName = NULL);

    inline void setAutomaticUpdateInterval (int interval) {
        updateBackgroundFrameInterval = interval;
    }
    inline void disableAutomaticBackgroundUpdating () {
        updateBackgroundFrameInterval = -1;
    }

    virtual int numToProccess ();
    virtual int numProcessed ();


protected:
     StaticBackgroundCompressor(const StaticBackgroundCompressor& orig);

    IplImage *background;
    std::vector<BackgroundRemovedImage *> bri;
    std::vector<IplImage *> imsToProcess;

    int threshBelowBackground;
    int threshAboveBackground;
    IplImage *bwbuffer;
    IplImage *buffer1;
    IplImage *buffer2;

    int updateBackgroundFrameInterval;
    int updateCount;
};

#endif	/* STATICBACKGROUNDCOMPRESSOR_H */

