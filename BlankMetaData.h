/* 
 * File:   BlankMetaData.h
 * Author: Marc
 *
 * Created on October 27, 2010, 1:26 PM
 */

#ifndef BLANKMETADATA_H
#define	BLANKMETADATA_H

#include "ImageMetaData.h"


class BlankMetaData : ImageMetaData {
public:
    BlankMetaData();
    BlankMetaData(const BlankMetaData& orig);
    virtual ~BlankMetaData();
    virtual void toDisk (std::ofstream &os) {
        return;
    }
    virtual std::string saveDescription () {
        return std::string("no meta data\n");
    }
    virtual int sizeOnDisk () {
        return 0;
    }
private:

};

#endif	/* BLANKMETADATA_H */

