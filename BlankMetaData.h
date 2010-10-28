/* 
 * File:   BlankMetaData.h
 * Author: Marc
 *
 * Created on October 27, 2010, 1:26 PM
 */

#ifndef BLANKMETADATA_H
#define	BLANKMETADATA_H

#include "ImageMetaData.h"


class BlankMetaData : public ImageMetaData {
public:
    BlankMetaData() {};
    BlankMetaData(const BlankMetaData& orig);
    virtual ~BlankMetaData() {};
    virtual void toDisk (std::ofstream &os) {
        return;
    }
    virtual std::string saveDescription () {
      //  std::cout << "into saveDescription\n";
        std::string s ("no meta data\n");
        return s;
    }
    virtual int sizeOnDisk () {
        return 0;
    }
    virtual unsigned long idCode() {
        return 0x0ccd07bc; //CRC32 hash of "BlankMetaData" from fileformat.info
    }
private:

};

#endif	/* BLANKMETADATA_H */

