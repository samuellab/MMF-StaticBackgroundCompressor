/* 
 * File:   ImageMetaDataLoader.h
 * Author: Marc
 *
 * Created on November 5, 2010, 3:31 PM
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

