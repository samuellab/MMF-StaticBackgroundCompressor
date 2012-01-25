/* 
 * File:   StaticBackgroundCompressor.h
 * Author: Marc
 *
 * Created on October 4, 2010, 1:06 PM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 *
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

    static const uint32_t IdCode = 0xbb67ca20; //CRC32 hash of "StaticBackgroundCompressor" from fileformat.info

    static const int headerSizeInBytes = 512;

    /*
     * use addFrame (const IplImage *im, ...) to add an image to the stack: the image will be copied internally
     * use addFrame (IplImage **im,...) to add an image to the stack; im will be added directly and then set to NULL,
     * signifying that you are no longer allowed to change or required to free im
     */
    virtual void addFrame (const IplImage *im, ImageMetaData *metadata = NULL);
    virtual void addFrame (IplImage **im, ImageMetaData *metadata = NULL);
    virtual int processFrame();
    virtual void processFrames();
    virtual void calculateBackground();
    virtual void updateBackground(const IplImage *im);
    virtual void toDisk (std::ofstream &os);
    virtual int sizeOnDisk();
    StaticBackgroundCompressor();
    virtual ~StaticBackgroundCompressor();
    static StaticBackgroundCompressor *fromDisk(std::ifstream& is);


    /*  CvSize getFrameSize
     *  returns the size of the frame copied out by copyBackground or
     *  reconstructFrame
     */
    virtual CvSize getFrameSize ();

    /* CvRect getValidRoi ();
     * returns the region of the background or reconstructed frame that actually
     * contains data 
     */

    virtual CvRect getValidRoi ();

    typedef struct {uint32_t idcode; int headerSize; int totalSize; int numframes;} HeaderInfoT;

    /* static headerInfoT getHeaderInfo(std::ifstream &is);
     * gets header info, then returns file pointer to starting location
     */
    static HeaderInfoT getHeaderInfo(std::ifstream &is);

    static void writeIplImageToByteStream (std::ofstream &os, const IplImage *src);
    static IplImage *readIplImageFromByteStream(std::ifstream &is);
    inline void setThresholds(int threshBelowBackground, int threshAboveBackground, int smallDimMinSize, int lgDimMinSize) {
        this->threshAboveBackground = threshAboveBackground;
        this->threshBelowBackground = threshBelowBackground;
        this->smallDimMinSize = smallDimMinSize;
        this->lgDimMinSize = lgDimMinSize;
    }

    //virtual const IplImage *getBackground();
    virtual void copyBackground(IplImage **dst);
    virtual void reconstructFrame (int frameNum, IplImage **dst);
    virtual void annotatedFrame (int frameNum, IplImage **buffer, IplImage **annotatedImage);

    int numRegionsInFrame (int frameNum) const;

    virtual void playMovie (const char *windowName = NULL);

    inline void setAutomaticUpdateInterval (int interval) {
        updateBackgroundFrameInterval = interval;
    }
    inline void disableAutomaticBackgroundUpdating () {
        updateBackgroundFrameInterval = -1;
    }

    virtual int numToProccess ();
    virtual int numProcessed ();

    virtual std::string saveDescription();

    virtual inline uint32_t idCode () {
        return StaticBackgroundCompressor::IdCode;
    }

    virtual const ImageMetaData *getMetaData(int frameNumber);

protected:
     StaticBackgroundCompressor(const StaticBackgroundCompressor& orig);

     void setImageOriginFromBRI(void);
     typedef std::pair<IplImage *, ImageMetaData *> InputImT;

    IplImage *background;
    std::vector<BackgroundRemovedImage *> bri;
    std::vector<InputImT> imsToProcess;

    int threshBelowBackground;
    int threshAboveBackground;
    //an extracted blob must have a bounding box at least smallDimMinSize x lgDimMinSize to be counted (lgDimSize automatically >= smallDimSize)
    int lgDimMinSize;
    int smallDimMinSize;
    IplImage *bwbuffer;
    IplImage *buffer1;
    IplImage *buffer2;

    int updateBackgroundFrameInterval;
    int updateCount;
    CvPoint imOrigin;
    virtual std::string headerDescription();
};

#endif	/* STATICBACKGROUNDCOMPRESSOR_H */

