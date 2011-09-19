/* 
 * File:   ImageMetaDataLoader.h
 * Author: Marc
 *
 * Created on November 5, 2010, 3:31 PM
 *
 * Factory class that loads meta data from disk;
 * if you implement a new metadata type, include its .h file here
 * and modify the fromFile function in ImageMetaDataLoader.cpp by including the
 * appropriate idCode and fromFile function.
 *
 * note that imageMetaData fromFile functions begin at the location AFTER the idCode
 */

#ifndef IMAGEMETADATALoader_H
#define	IMAGEMETADATALoader_H

#include "ImageMetaData.h"
#include "BlankMetaData.h"
#include "MightexMetaData.h"
#include "CompositeImageMetaData.h"

#include <istream>

class ImageMetaDataLoader {
public:
    
    virtual ~ImageMetaDataLoader();
    static ImageMetaData *fromFile (std::ifstream &is);
private:

    ImageMetaDataLoader();
    ImageMetaDataLoader(const ImageMetaDataLoader& orig);
};

#endif	/* IMAGEMETADATALoader_H */

