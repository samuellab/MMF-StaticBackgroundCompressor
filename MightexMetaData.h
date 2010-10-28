/* 
 * File:   MightexMetaData.h
 * Author: Marc
 *
 * Created on October 27, 2010, 9:38 AM
 */

#ifndef MIGHTEXMETADATA_H
#define	MIGHTEXMETADATA_H
#include "BUF_USBCamera_SDK.h"
#include "ImageMetaData.h"


class MightexMetaData : public ImageMetaData {
public:
    
    MightexMetaData(const TProcessedDataProperty *attributes);
    virtual ~MightexMetaData();
    virtual void toDisk (std::ofstream &os);
    virtual std::string saveDescription ();
    virtual int sizeOnDisk ();
    virtual unsigned long idCode() {
        return 0xdf4f1592; //CRC32 hash of "MightexMetaData" from fileformat.info
    }
protected:
    TProcessedDataProperty attributes;
private:
    MightexMetaData(const MightexMetaData& orig);
    MightexMetaData();
};

#endif	/* MIGHTEXMETADATA_H */

