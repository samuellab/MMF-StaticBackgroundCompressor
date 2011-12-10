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
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
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

