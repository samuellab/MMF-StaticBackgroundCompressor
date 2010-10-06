/* 
 * File:   BackgroundRemovedImage.h
 * Author: Marc
 *
 * Created on October 4, 2010, 9:17 AM
 */

#ifndef BACKGROUNDREMOVEDIMAGE_H
#define	BACKGROUNDREMOVEDIMAGE_H

#include "cv.h"
#include <vector>
#include <iostream>
#include <fstream>

class BackgroundRemovedImage {
public:

    BackgroundRemovedImage (IplImage *src, const IplImage *bak, IplImage *bwbuffer = NULL, IplImage *srcbuffer1 = NULL,  IplImage *srcbuffer2 = NULL, int threshBelowBackground = 0, int threshAboveBackground = 0);
    virtual ~BackgroundRemovedImage();
    void toDisk(std::ofstream &os);
    static BackgroundRemovedImage *fromDisk(std::ifstream& is, const IplImage *bak);
    int sizeOnDisk();
    int sizeInMemory();
    void restoreImage (IplImage **dst);

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

protected:
    void extractDifferences(IplImage *src, IplImage *bwbuffer = NULL, IplImage *srcbuffer1 = NULL, IplImage *srcbuffer2 = NULL);
    void extractBlobs (IplImage *src, IplImage *mask);

    BackgroundRemovedImage();
    BackgroundRemovedImage(const BackgroundRemovedImage& orig);

    const IplImage *backgroundIm;
    std::vector<std::pair<CvRect, IplImage *> > differencesFromBackground;
    int threshBelowBackground;
    int threshAboveBackground;
    CvMemStorage *ms;
};

#endif	/* BACKGROUNDREMOVEDIMAGE_H */

