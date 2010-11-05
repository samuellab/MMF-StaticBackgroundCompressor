/* 
 * File:   ImageMetaDataLoader.cpp
 * Author: Marc
 * 
 * Created on November 5, 2010, 3:31 PM
 */

#include "ImageMetaDataLoader.h"
#include <iostream>
#include <fstream>
ImageMetaDataLoader::ImageMetaDataLoader() {
}

ImageMetaDataLoader::ImageMetaDataLoader(const ImageMetaDataLoader& orig) {
}

ImageMetaDataLoader::~ImageMetaDataLoader() {
}

ImageMetaData *ImageMetaDataLoader::fromFile (std::ifstream &is) {
        unsigned long id;
        is.read((char *) &id, sizeof(id));
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
            default:
                return NULL;
                break;
        }
        return NULL;
}

