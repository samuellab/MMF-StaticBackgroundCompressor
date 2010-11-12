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
    NameValueMetaData();
   virtual ~NameValueMetaData();
    virtual void toDisk (std::ofstream &os)const;
    virtual std::string saveDescription ()const;
    virtual int sizeOnDisk ()const;
    virtual ~ImageMetaData() {};
    virtual unsigned long idCode()const;
    virtual std::map<std::string, double> getFieldNamesAndValues(void)const;

    virtual void addData (std::string name, double value);
    virtual bool hasField (std::string fieldName)const;
    virtual void replaceData (std::string name, double value);

    virtual NameValueMetaData *copy() const;
protected:
    std::map<std::string, double> data;

private:
    NameValueMetaData(const NameValueMetaData& orig);

};

#endif	/* NAMEVALUEMETADATA_H */

