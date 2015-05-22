/* 
 * File:   BackgroundRemovedImage.h
 * Author: Marc Gershow
 *
 * Created on October 4, 2010, 9:17 AM
 *
 * A class used to dramatically compress the storage space required to hold an image
 * that differs in only small areas from a constant background image
 *
 * User provides a source image and a background image
 * the differences between the source and background are stored internally as
 * a series of small iplImages and written to disk as a rectangle size plus data
 *
 * optionally meta data can be stored with the image; see the ImageMetaData class
 * and its subclasses
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 * 
 */

#ifndef BACKGROUNDREMOVEDIMAGE_H
#define	BACKGROUNDREMOVEDIMAGE_H

#include "cv.h"
#include "ImageMetaData.h"
#include <vector>
#include <iostream>
#include <fstream>

/* BackroundRemovedImage
 *
 * A class used to dramatically compress the storage space required to hold an image 
 * that differs in only small areas from a constant background image
 * 
 * User provides a source image and a background image
 * the differences between the source and background are stored internally as 
 * a series of small iplImages and written to disk as a rectangle size plus data
 * 
 * optionally meta data can be stored with the image; see the ImageMetaData class 
 * and its subclasses
 */

class BackgroundRemovedImage {
public:

    static const uint32_t IdCode =  0xf80921af; //CRC32 hash of "BackgroundRemovedImage" from fileformat.info

    /* BackgroundRemovedImage (IplImage *src, const IplImage *bak, int threshBelowBackground = 0, int threshAboveBackground = 0, int smallDimMinSize = 1, int lgDimMinSize = 1, ImageMetaData *metadata = NULL);
     *
     * creating the background removed image automatically runs the compression algorithm;  note that src is not a constant parameter, because the compression changes src's ROI, but
     * src is not altered during the compression, and freeing src remains the caller's responsibility
     *
     * src - image to be compressed
     * bak - background image -- src and bak must have identical sizes, bit depths, and must both have only 1 channel
     * threshBelowBackground, threshAboveBackground - number of gray levels src must be above/below bak to be considered different from the background image
     * smallDimMinSize, lgDimMinSize - defines the minimum size of the bounding box for a region different from the background to be included in the compressed image
     * metadata - metadata to be stored with the image; background removed image assumes responsibility for this memory;
     *      metadata must be allocated on the heap and caller MUST NOT FREE/DELETE metdata after calling
     */
    BackgroundRemovedImage (IplImage *src, const IplImage *bak, int threshBelowBackground = 0, int threshAboveBackground = 0, int smallDimMinSize = 1, int lgDimMinSize = 1, ImageMetaData *metadata = NULL);
    virtual ~BackgroundRemovedImage();

    /* void toDisk(std::ofstream &os);
     *
     * writes the background removed image to a binary file
     * the method by which the image is stored in the file can be found by calling saveDescription()
     * note that the background image itself is not written to disk by toDisk()
     */
    virtual void toDisk(std::ofstream &os);

    /* std::string saveDescription();
     *
     * returns a string describing the structure with which the image is saved to the binary file
     */
    virtual std::string saveDescription();

    /* static BackgroundRemovedImage *fromDisk(std::ifstream& is, const IplImage *bak);
     *
     * reads a background removed image from a binary file
     * is should have its read index at the beginning of a background removed image, i.e. at the IdCode
     * because the background image is not stored on disk, caller must provide a pointer to the background image
     */
    static BackgroundRemovedImage *fromDisk(std::ifstream& is, const IplImage *bak);

    /* virtual int sizeOnDisk();
     * virtual int sizeInMemory();
     *
     * returns the size the image will take up on disk/ currently takes up in memory (not including the background image)
     */

    virtual int32_t sizeOnDisk();
    virtual int sizeInMemory();

    /* void restoreImage (IplImage **dst);
     *
     * converts the background removed image to a standard IPL image
     * if *dst is NULL or points to an incompatible image, new memory is allocated (*dst is freed first if necessary)
     */
    virtual void restoreImage (IplImage **dst);

    /* virtual void annotateImage (IplImage *dst, CvScalar color = CV_RGB(255,0,0), int thickness = 2);
     *
     * draws rectangles around each non-background region of the background removed image
     *
     */
    virtual void annotateImage (IplImage *dst, CvScalar color = CV_RGB(255,0,0), int thickness = 2);

    /* void switchBackground (const IplImage *bak)
     * changes the backgroundIm pointer to point to new background image
     * does not allocate or release memory; does not make a copy of background image
     * 
     */
    virtual void switchBackground (const IplImage *bak) {
        backgroundIm = bak;
    }

    virtual int numRegions() const;

    const ImageMetaData *getMetaData() {
        return this->metadata;
    }

    CvPoint getImageOrigin();
    
    static const int headerSizeInBytes = 1024;

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

    virtual inline uint32_t idCode () {
        return BackgroundRemovedImage::IdCode;
    }

    typedef struct {uint32_t idcode; int32_t headersize; int32_t depth; int32_t nchannels; int32_t numims;} HeaderInfoT;

    typedef void (*mergeFunctionT) (const CvArr *src1, const CvArr *src2, CvArr *dst);
    /* mergeRegions(const BackgroundRemovedImage *bri2);
     
     * 
     * 
     */
    virtual void mergeRegions(const BackgroundRemovedImage *bri2, mergeFunctionT mergeFunction);
    
protected:
    virtual void extractDifferences(IplImage *src);
    virtual void extractBlobs (IplImage *src, IplImage *mask);
    virtual void writeHeader (std::ofstream &os);

    virtual void init();

    virtual inline std::string classname() { return std::string("BackgroundRemovedImage");}
    virtual std::string headerDescription();

    virtual void setImOriginFromMetaData();
    
    virtual IplImage *subImageFromConst (const IplImage *src, CvRect & r);
    
    template<class subclass, class superclass> bool isa (superclass *obj, subclass * &dst);
    template<class subclass, class superclass> bool isa (const superclass *obj, const subclass * &dst);
    template<class subclass, class superclass> bool isa (const superclass *obj);

    inline bool rectanglesIntersect (CvRect r1, CvRect r2) {
        return (r1.x < r2.x+r2.width && r2.x < r1.x+r1.width && r1.y < r2.y+r2.height && r2.y < r1.y + r1.height);
    }
    virtual std::pair<CvRect, IplImage *> mergeBlobs(std::pair<CvRect, IplImage *> b1, std::pair<CvRect, IplImage *> b2, mergeFunctionT mergeFunction);

    virtual bool compactBlobs(mergeFunctionT mergeFunction); //returns true if any blobs were compacted
    
    BackgroundRemovedImage();
    BackgroundRemovedImage(const BackgroundRemovedImage& orig);

    const IplImage *backgroundIm;
    std::vector<std::pair<CvRect, IplImage *> > differencesFromBackground;
    int threshBelowBackground;
    int threshAboveBackground;
    int smallDimMinSize;
    int lgDimMinSize;
    CvPoint imOrigin;
    CvMemStorage *ms;
    ImageMetaData *metadata;
    
    static void avgOfTwoIms (const CvArr *src1, const CvArr *src2, CvArr *dst) {
        cvAddWeighted(src1, 0.5, src2, 0.5, 0, dst);
    }
};

#endif	/* BACKGROUNDREMOVEDIMAGE_H */

