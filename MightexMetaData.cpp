/* 
 * File:   MightexMetaData.cpp
 * Author: Marc
 * 
 * Created on October 27, 2010, 9:38 AM
 */

#include <fstream>
#include <sstream>

#include "MightexMetaData.h"

MightexMetaData::MightexMetaData() {
}

MightexMetaData::MightexMetaData(const MightexMetaData& orig) {
}

MightexMetaData::~MightexMetaData() {
}

MightexMetaData::MightexMetaData(const TProcessedDataProperty *attributes) {
    this->attributes = *attributes;
    
}

void MightexMetaData::toDisk(std::ofstream& os) {
    unsigned long id = idCode();
    os.write ((char *) &id, sizeof(id));
    os.write((char *)&attributes, sizeof(attributes));
}

int MightexMetaData::sizeOnDisk() {
    return (sizeof(attributes));
}

std::string MightexMetaData::saveDescription() {
    std::stringstream os;
    os << "TProcessedDataProperty, a " << sizeof(attributes) << " byte data structure containing information about image aquisition from Mightex Camera engine.\n";
    return os.str();
}

MightexMetaData *MightexMetaData::fromFile(std::ifstream& is) {
    TProcessedDataProperty tpd;
    is.read((char *) &tpd, sizeof(tpd));
    return new MightexMetaData(&tpd);
}
