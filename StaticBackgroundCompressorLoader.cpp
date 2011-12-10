/* 
 * File:   StaticBackgroundCompressorLoader.cpp
 * Author: Marc
 * 
 * Created on November 5, 2010, 4:14 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#include "StaticBackgroundCompressorLoader.h"
using namespace std;
StaticBackgroundCompressor *StaticBackgroundCompressorLoader::fromFile(std::ifstream& is) {
    unsigned long id;
    ifstream::pos_type cloc = is.tellg();
    is.read((char *) &id, sizeof(id));
    is.seekg(cloc);
    switch(id) {
        case StaticBackgroundCompressor::IdCode:
            return StaticBackgroundCompressor::fromDisk(is);
            break;
        default:
            cout << "did not recognize ID code " << hex << id << dec;
            return StaticBackgroundCompressor::fromDisk(is);
            break;

    }
}

StaticBackgroundCompressor::HeaderInfoT StaticBackgroundCompressorLoader::getHeaderInfo(std::ifstream& is) {
    unsigned long id;
    ifstream::pos_type cloc = is.tellg();
    is.read((char *) &id, sizeof(id));
    is.seekg(cloc);
    switch(id) {
        case StaticBackgroundCompressor::IdCode:
            return StaticBackgroundCompressor::getHeaderInfo(is);
            break;
        default:
            cout << "did not recognize ID code";
            return StaticBackgroundCompressor::getHeaderInfo(is);
            break;
    }
    StaticBackgroundCompressor::HeaderInfoT hi;
    return hi;
}

StaticBackgroundCompressorLoader::StaticBackgroundCompressorLoader() {
}

StaticBackgroundCompressorLoader::StaticBackgroundCompressorLoader(const StaticBackgroundCompressorLoader& orig) {
}

StaticBackgroundCompressorLoader::~StaticBackgroundCompressorLoader() {
}

