/* 
 * File:   ImageMetaData.h
 * Author: Marc
 *
 * Created on October 27, 2010, 9:28 AM
 *
 * ImageMetaData is an abstract class that defines an interface for
 * storing meta data about images in binary data files
 *
 * must implement these 3 functions
 *
 * toDisk (ofstream &os) -- writes the meta data to disk at the current file poisition
 * saveDescription -- a text description of the information contained in the metadata
 * sizeOnDisk -- the number of bytes the ImageMetaData occupies on disk
 * should also create a unique unsigend int id code that
 */

#ifndef IMAGEMETADATA_H
#define	IMAGEMETADATA_H

#include <ostream>
#include <string>

class ImageMetaData {
public:
    ImageMetaData() {};
    virtual void toDisk (std::ofstream &os) = 0;
    virtual std::string saveDescription () = 0;
    virtual int sizeOnDisk () = 0;
   
    virtual ~ImageMetaData() {};

    virtual unsigned long idCode() = 0;
    /*
    {
        return CRC32 hash code of "ImageMetaData" from http://www.fileformat.info/tool/hash.htm
      e.g. return 0x44760428;
    }
    */
private:
     ImageMetaData(const ImageMetaData& orig);
};

#endif	/* IMAGEMETADATA_H */

