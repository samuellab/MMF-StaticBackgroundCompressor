/* 
 * File:   BlankMetaData.h
 * Author: Marc
 *
 * Created on October 27, 2010, 1:26 PM
 *
 * implements ImageMetaData but doesn't actually store any meta data
 * if that isn't meta, what is?
 */

#ifndef BLANKMETADATA_H
#define	BLANKMETADATA_H

#include "ImageMetaData.h"
#include <iostream>
#include <fstream>

/*
 * BlankMetaData
 *
 * implements ImageMetaData but doesn't actually store any meta data
 * if that isn't meta, what is?
 */
class BlankMetaData : public ImageMetaData {
public:
    static const unsigned long IdCode = 0x0ccd07bc; //CRC32 hash of "BlankMetaData" from fileformat.info

    BlankMetaData() {};
    BlankMetaData(const BlankMetaData& orig);
    virtual ~BlankMetaData() {};
    virtual void toDisk (std::ofstream &os)const {
        unsigned long id = idCode();
        os.write((char *) &id, sizeof(id));
        return;
    }
    virtual std::string saveDescription ()const {
      //  std::cout << "into saveDescription\n";
        std::string s ("no meta data\n");
        return s;
    }
    virtual int sizeOnDisk () const{
        return sizeof(unsigned long);
    }
    virtual unsigned long idCode() const {
        return BlankMetaData::IdCode;
    }

    virtual std::map<std::string, double> getFieldNamesAndValues(void)const{
        std::map<std::string, double> rval;
        return rval;
    }

    virtual ImageMetaData *clone() const{
        return new BlankMetaData();
    }
    static BlankMetaData *fromFile (std::istream &is) {
        return new BlankMetaData();
    }


private:

};

#endif	/* BLANKMETADATA_H */

