/* 
 * File:   ImageMetaData.h
 * Author: Marc
 *
 * Created on October 27, 2010, 9:28 AM
 *
 * ImageMetaData is an abstract class that defines an interface for
 * storing meta data about images in binary data files
 *
 * must implement these functions
 *
 * toDisk (ofstream &os) -- writes the meta data to disk at the current file poisition
 * saveDescription -- a text description of the information contained in the metadata
 * sizeOnDisk -- the number of bytes the ImageMetaData occupies on disk
 * idCode -- a unique unsigned int that identifies the type of meta data, so that it can
 *      be properly read from disk later; crc32 hash of class name is recommended
 * getFieldNamesAndValues -- returns a table of names and double values, (e.g. "FrameNumber", 27)
 *                           allows generation of metadata tables
 * clone -- generate a new copy of the metadata object
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#ifndef IMAGEMETADATA_H
#define	IMAGEMETADATA_H


#include <ostream>
#include <string>
#include <map>
#include <stdint.h> //if you don't have stdint (not C99 compatible), use google, or see http://stackoverflow.com/questions/126279/c99-stdint-h-header-and-ms-visual-studio

class ImageMetaData {
public:
     
    
    virtual void toDisk (std::ofstream &os)const = 0;
    virtual std::string saveDescription ()const = 0;
    virtual int32_t sizeOnDisk ()const = 0;
    virtual ~ImageMetaData() {};
    virtual uint32_t idCode()const = 0;
    virtual std::map<std::string, double> getFieldNamesAndValues(void)const = 0;
    virtual ImageMetaData *clone()const = 0;
    

protected:
    ImageMetaData() {};

private:
     ImageMetaData(const ImageMetaData& orig);
};

#endif	/* IMAGEMETADATA_H */

