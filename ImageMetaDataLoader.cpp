/* 
 * File:   ImageMetaDataLoader.cpp
 * Author: Marc
 * 
 * Created on November 5, 2010, 3:31 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
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
        uint32_t id;
        std::ifstream::pos_type cloc = is.tellg();
        is.read((char *) &id, sizeof(id));
     //   std::cout << "id = " << id << "\n";
        switch (id) {
            case BlankMetaData::IdCode:
                return BlankMetaData::fromFile(is);
                break;
//            case MightexMetaData::IdCode:
//                return MightexMetaData::fromFile(is);
//                break;
            case CompositeImageMetaData::IdCode:
                return CompositeImageMetaData::fromFile(is);
                break;
            case NameValueMetaData::IdCode:
                return NameValueMetaData::fromFile(is);
                break;
            default://this is a kludge but we have some old data files that just stored mightex meta data directly
                is.seekg(cloc);
//                return MightexMetaData::fromFile(is);
                break;
        }
        return NULL;
}

