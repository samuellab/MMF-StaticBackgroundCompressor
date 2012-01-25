/* 
 * File:   MightexMetaData.h
 * Author: Marc
 *
 * Created on October 27, 2010, 9:38 AM
 *
 * stores metadata recovered from the mightex camera API
 * used in conjunction with libraries that directly access the mightex API
 * but otherwise probably not useful to you
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#ifndef MIGHTEXMETADATA_H
#define	MIGHTEXMETADATA_H
#include "Necessary Libraries and Includes/BUF_USBCamera_SDK.h"
#include "ImageMetaData.h"


class MightexMetaData : public ImageMetaData {
public:
    static const uint32_t IdCode = 0xdf4f1592; //CRC32 hash of "MightexMetaData" from fileformat.info

    
    MightexMetaData(const TProcessedDataProperty *attributes);
    virtual ~MightexMetaData();
    virtual void toDisk (std::ofstream &os)const;
    virtual std::string saveDescription ()const;
    virtual int sizeOnDisk ()const;
    virtual uint32_t idCode()const {
        return MightexMetaData::IdCode;
    }
    virtual TProcessedDataProperty getAttributes()const;

    virtual std::map<std::string, double> getFieldNamesAndValues(void)const;
    /* static MightexMetaData *fromFile(std::ifstream &is);
     * assumes idCode has already been read from is, so starts at the next byte
     *
     */
    static MightexMetaData *fromFile(std::ifstream &is);
    
    virtual ImageMetaData *clone() const;

protected:
    TProcessedDataProperty attributes;
private:
    MightexMetaData(const MightexMetaData& orig);
    MightexMetaData();
};

#endif	/* MIGHTEXMETADATA_H */

