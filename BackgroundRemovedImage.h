/* 
 * File:   BackgroundRemovedImage.h
 * Author: Marc
 *
 * Created on October 4, 2010, 9:17 AM
 */

#ifndef BACKGROUNDREMOVEDIMAGE_H
#define	BACKGROUNDREMOVEDIMAGE_H

#include "cv.h"
#include "ImageMetaData.h"
#include <vector>
#include <iostream>
#include <fstream>

class BackgroundRemovedImage {
public:

    static const unsigned long IdCode =  0xf80921af; //CRC32 hash of "BackgroundRemovedImage" from fileformat.info

    BackgroundRemovedImage (IplImage *src, const IplImage *bak, IplImage *bwbuffer = NULL, IplImage *srcbuffer1 = NULL,  IplImage *srcbuffer2 = NULL, int threshBelowBackground = 0, int threshAboveBackground = 0, int smallDimMinSize = 1, int lgDimMinSize = 1, ImageMetaData *metadata = NULL);
    virtual ~BackgroundRemovedImage();
    virtual void toDisk(std::ofstream &os);
    static BackgroundRemovedImage *fromDisk(std::ifstream& is, const IplImage *bak);
    virtual int sizeOnDisk();
    virtual int sizeInMemory();
    virtual std::string saveDescription();
    virtual void restoreImage (IplImage **dst);
    virtual void annotateImage (IplImage *dst, CvScalar color = CV_RGB(255,0,0), int thickness = 2);

    static const int headerSizeInBytes = 256;

    static inline int bytesPerPixel(const IplImage *src) {
        switch (src->depth) {
            case IPL_DEPTH_8U:
            case IPL_DEPTH_8S:
                return 1;
                break;
            case IPL_DEPTH_16U:
            case IPL_DEPTH_16S:
                return 2;
                break;
            case IPL_DEPTH_32F:
            case IPL_DEPTH_32S:
                return 4;
                break;
            case IPL_DEPTH_64F:
                return 8;
                break;
            case IPL_DEPTH_1U:
                return 1;
                break;
        }
        return 1;
    }

    virtual inline unsigned long idCode () {
        return BackgroundRemovedImage::IdCode;
    }

    typedef struct {unsigned long idcode; int headersize; int depth; int nchannels; int numims;} HeaderInfoT;

protected:
    virtual void extractDifferences(IplImage *src, IplImage *bwbuffer = NULL, IplImage *srcbuffer1 = NULL, IplImage *srcbuffer2 = NULL);
    virtual void extractBlobs (IplImage *src, IplImage *mask);
    virtual void writeHeader (std::ofstream &os);

    virtual void init();

    virtual inline std::string classname() { return std::string("BackgroundRemovedImage");}
    virtual std::string headerDescription();


    BackgroundRemovedImage();
    BackgroundRemovedImage(const BackgroundRemovedImage& orig);

    const IplImage *backgroundIm;
    std::vector<std::pair<CvRect, IplImage *> > differencesFromBackground;
    int threshBelowBackground;
    int threshAboveBackground;
    int smallDimMinSize;
    int lgDimMinSize;
    CvMemStorage *ms;
    ImageMetaData *metadata;
};

#endif	/* BACKGROUNDREMOVEDIMAGE_H */

