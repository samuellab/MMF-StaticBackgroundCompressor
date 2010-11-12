/* 
 * File:   ImageMetaDataLoader.cpp
 * Author: Marc
 * 
 * Created on November 5, 2010, 3:31 PM
 */

#include "ImageMetaDataLoader.h"
#include "NameValueMetaData.h"
#include <iostream>
#include <fstream>
#include <iosfwd>
ImageMetaDataLoader::ImageMetaDataLoader() {
}

ImageMetaDataLoader::ImageMetaDataLoader(const ImageMetaDataLoader& orig) {
}

ImageMetaDataLoader::~ImageMetaDataLoader() {
}

ImageMetaData *ImageMetaDataLoader::fromFile (std::ifstream &is) {
        unsigned long id;
        std::ifstream::pos_type cloc = is.tellg();
        is.read((char *) &id, sizeof(id));
     //   std::cout << "id = " << id << "\n";
        switch (id) {
            case BlankMetaData::IdCode:
                return BlankMetaData::fromFile(is);
                break;
            case MightexMetaData::IdCode:
                return MightexMetaData::fromFile(is);
                break;
            case CompositeImageMetaData::IdCode:
                return CompositeImageMetaData::fromFile(is);
                break;
            case NameValueMetaData::IdCode:
                return NameValueMetaData::fromFile(is);
                break;
            default://this is a kludge but we have some old data files that just stored mightex meta data directly
                is.seekg(cloc);
                return MightexMetaData::fromFile(is);
                break;
        }
        return NULL;
}

