/* 
 * File:   MightexMetaData.cpp
 * Author: Marc
 * 
 * Created on October 27, 2010, 9:38 AM
 */

#include <fstream>
#include <sstream>
#include <map>

#include "MightexMetaData.h"
using namespace std;

MightexMetaData::MightexMetaData() {
}

MightexMetaData::MightexMetaData(const MightexMetaData& orig) {
}

MightexMetaData::~MightexMetaData() {
}

MightexMetaData::MightexMetaData(const TProcessedDataProperty *attributes) {
    this->attributes = *attributes;
    
}

void MightexMetaData::toDisk(std::ofstream& os) const{
    unsigned long id = idCode();
    os.write ((char *) &id, sizeof(id));
    os.write((char *)&attributes, sizeof(attributes));
}

int MightexMetaData::sizeOnDisk()const {
    return (sizeof(attributes));
}

std::string MightexMetaData::saveDescription()const {
    std::stringstream os;
    os << "TProcessedDataProperty, a " << sizeof(attributes) << " byte data structure containing information about image aquisition from Mightex Camera engine.\n";
    return os.str();
}

MightexMetaData *MightexMetaData::fromFile(std::ifstream& is) {
    TProcessedDataProperty tpd;
    is.read((char *) &tpd, sizeof(tpd));
    return new MightexMetaData(&tpd);
}
std::map<std::string, double> MightexMetaData::getFieldNamesAndValues() const {
    map<string, double> fnav;
    fnav.insert(pair<string, double>("Mightex_Bin", attributes.Bin));
    fnav.insert(pair<string, double>("Mightex_BlueGain", attributes.BlueGain));
    fnav.insert(pair<string, double>("Mightex_CameraID", attributes.CameraID));
    fnav.insert(pair<string, double>("Mightex_Column", attributes.Column));
    fnav.insert(pair<string, double>("Mightex_ExposureTime", attributes.ExposureTime));
    fnav.insert(pair<string, double>("Mightex_FilterAcceptForFile", attributes.FilterAcceptForFile));
    fnav.insert(pair<string, double>("Mightex_ProcessFrameType", attributes.ProcessFrameType));
    fnav.insert(pair<string, double>("Mightex_RedGain", attributes.RedGain));
    fnav.insert(pair<string, double>("Mightex_Row", attributes.Row));
    fnav.insert(pair<string, double>("Mightex_TimeStamp", attributes.TimeStamp));
    fnav.insert(pair<string, double>("Mightex_TriggerEventCount", attributes.TriggerEventCount));
    fnav.insert(pair<string, double>("Mightex_TriggerOccurred", attributes.TriggerOccurred));
    fnav.insert(pair<string, double>("Mightex_XStart", attributes.XStart));
    fnav.insert(pair<string, double>("Mightex_YStart", attributes.YStart));
    return fnav;

}

TProcessedDataProperty MightexMetaData::getAttributes() const{
    return attributes;
}

ImageMetaData *MightexMetaData::clone() const {
    return new MightexMetaData(&(this->attributes));
}