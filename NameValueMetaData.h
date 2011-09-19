/* 
 * File:   NameValueMetaData.h
 * Author: Marc
 *
 * A generic metadata class that lets you set (string) name, (double) values
 */

#ifndef NAMEVALUEMETADATA_H
#define	NAMEVALUEMETADATA_H

#include "ImageMetaData.h"


class NameValueMetaData : public ImageMetaData{
public:
    static const unsigned long IdCode = 0xc15ac674; //CRC32 hash of "NameValueMetaData" from http://www.fileformat.info/tool/hash.htm?text=NameValueMetaData
    NameValueMetaData();
   virtual ~NameValueMetaData();
    virtual void toDisk (std::ofstream &os) const;
    virtual std::string saveDescription ()const;
    virtual int sizeOnDisk ()const;
    virtual unsigned long idCode()const {
        return IdCode;
    }
    virtual std::map<std::string, double> getFieldNamesAndValues(void)const;

    virtual void addData (std::string name, double value);
    virtual bool hasField (std::string fieldName)const;
    virtual void replaceData (std::string name, double value);
    virtual void clear();
    virtual NameValueMetaData *copy() const;

    static NameValueMetaData *fromFile (std::ifstream &is);
    virtual ImageMetaData *clone() const;
protected:
    std::map<std::string, double> data;

private:
    NameValueMetaData(const NameValueMetaData& orig);

};

#endif	/* NAMEVALUEMETADATA_H */

