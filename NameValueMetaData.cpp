/* 
 * File:   NameValueMetaData.cpp
 * Author: Marc
 * 
 * Created on November 12, 2010, 12:16 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#include <sstream>
#include <fstream>
#include "NameValueMetaData.h"
#include <iostream>
#include <ostream>
#include <istream>
#include <assert.h>
using namespace std;

NameValueMetaData::NameValueMetaData() {
}

NameValueMetaData::NameValueMetaData(const NameValueMetaData& orig) {
}

NameValueMetaData::~NameValueMetaData() {
}

void NameValueMetaData::addData(std::string name, double value) {
    data.insert(pair<string, double> (name, value));
}

NameValueMetaData *NameValueMetaData::copy() const {
    NameValueMetaData *nvmd = new NameValueMetaData;
    nvmd->data.insert(data.begin(), data.end());
    return nvmd;
}

ImageMetaData *NameValueMetaData::clone() const {
    return copy();
}

map<string, double> NameValueMetaData::getFieldNamesAndValues() const {
    return data;
}

bool NameValueMetaData::hasField(std::string fieldName) const {
    return (data.find(fieldName) != data.end());
}

void NameValueMetaData::replaceData(std::string name, double value) {
    data[name] = value;
}

void NameValueMetaData::toDisk(ofstream& os) const {
    uint32_t id = idCode();
    os.write((char *) &id, sizeof(id));
    int numElems = data.size();
    os.write((char *) &numElems, sizeof(numElems));
    for (map<string, double>::const_iterator it = data.begin(); it != data.end(); ++it) {
        const char *str = it->first.c_str();
        double val = it->second;
        os.write(str, it->first.length() + 1);
        os.write((char *) &val, sizeof(val));
    }
}

string NameValueMetaData::saveDescription() const {
    stringstream os;
    os << "Name-Value MetaData: idcode (uint32_t) = " << hex << idCode() << dec << ", int number of key-value pairs stored, then each pair" << endl;
    os << "in the format \\0-terminated string of chars then " << sizeof(double) << " byte double value" <<endl;
    return os.str();
}

int32_t NameValueMetaData::sizeOnDisk() const {
    int sod = sizeof(uint32_t) + sizeof(int);
    for (map<string, double>::const_iterator it = data.begin(); it != data.end(); ++it) {
        sod += it->first.length() + 1 + sizeof(double);
    }
    return sod;
}

NameValueMetaData *NameValueMetaData::fromFile(ifstream& is) {
    NameValueMetaData *nvmd = new NameValueMetaData();
    //we've already read id code, so move on to num elems
    int numElems;
    is.read((char *) &numElems, sizeof(numElems));
    for (int j = 0; j < numElems; ++j) {
        string s;
        s.clear();
        char c;
        is.read(&c, 1);
        while (c != '\0') {
            s.append(1, c);
            is.read(&c, 1);
        }
        double val;
        assert (sizeof(double) == 8); //if size of double is not 8, we need to adjust code to make compatible
        is.read((char *)&val, sizeof(val));
        nvmd->addData(s,val);
    }
    return nvmd;
}

void NameValueMetaData::clear() {
    data.clear();
}