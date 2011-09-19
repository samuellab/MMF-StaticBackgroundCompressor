/* 
 * File:   CompositeImageMetaData.h
 * Author: Marc
 *
 * Created on November 5, 2010, 10:00 AM
 * 
 * An image metadata class that contains a list of other image meta datas
 * this way, you can combine metadata from multiple sources fairly easily
 */

#ifndef COMPOSITEIMAGEMETADATA_H
#define	COMPOSITEIMAGEMETADATA_H

#include "ImageMetaData.h"
#include <vector>

class CompositeImageMetaData : public ImageMetaData {
public:
    static const unsigned long IdCode = 0x9844e951; //CRC32 hash of "CompositeImageMetaData" from http://www.fileformat.info/tool/hash.htm?text=CompositeImageMetaData

    CompositeImageMetaData();
    virtual ~CompositeImageMetaData();
    void addMetaData(ImageMetaData *md);
    virtual void toDisk (std::ofstream &os)const;
    virtual std::string saveDescription ()const;
    virtual int sizeOnDisk ()const;
    virtual unsigned long idCode()const {
        return CompositeImageMetaData::IdCode;
    }
    virtual std::map<std::string, double> getFieldNamesAndValues(void)const;

    std::vector<const ImageMetaData *> getMetaDataVector();

    static CompositeImageMetaData *fromFile (std::ifstream &is);

    virtual ImageMetaData *clone() const;

protected:
    std::vector <ImageMetaData *> imd;
    void removeNulls();
private:
    CompositeImageMetaData(const CompositeImageMetaData& orig);


};

#endif	/* COMPOSITEIMAGEMETADATA_H */

