/* 
 * File:   BlankMetaData.h
 * Author: Marc
 *
 * Created on October 27, 2010, 1:26 PM
 */

#ifndef BLANKMETADATA_H
#define	BLANKMETADATA_H

#include "ImageMetaData.h"
#include <iostream>
#include <fstream>
class BlankMetaData : public ImageMetaData {
public:
    static const unsigned long IdCode = 0x0ccd07bc; //CRC32 hash of "BlankMetaData" from fileformat.info

    BlankMetaData() {};
    BlankMetaData(const BlankMetaData& orig);
    virtual ~BlankMetaData() {};
    virtual void toDisk (std::ofstream &os) {
        unsigned int id = idCode();
        os.write((char *) &id, sizeof(id));
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
        return BlankMetaData::IdCode;
    }
    static BlankMetaData *fromFile (std::istream &is) {
        return new BlankMetaData();
    }

private:

};

#endif	/* BLANKMETADATA_H */

