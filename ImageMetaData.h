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
#include <map>

class ImageMetaData {
public:
     
    
    virtual void toDisk (std::ofstream &os) = 0;
    virtual std::string saveDescription () = 0;
    virtual int sizeOnDisk () = 0;   
    virtual ~ImageMetaData() {};
    virtual unsigned long idCode() = 0;
    virtual std::map<std::string, double> getFieldNamesAndValues(void)const = 0;
    

protected:
    ImageMetaData() {};

private:
     ImageMetaData(const ImageMetaData& orig);
};

#endif	/* IMAGEMETADATA_H */

