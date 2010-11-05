/* 
 * File:   BackgroundRemovedImageLoader.cpp
 * Author: Marc
 * 
 * Created on November 5, 2010, 3:56 PM
 */

#include "BackgroundRemovedImageLoader.h"
#include <iostream>
#include "BackgroundRemovedImage.h"

using namespace std;

BackgroundRemovedImage *BackgroundRemovedImageLoader::fromFile(std::ifstream& is, const IplImage *bak) {
    unsigned long id;
    ifstream::pos_type cloc = is.tellg();
    is.read((char *) &id, sizeof(id));
    is.seekg(cloc);
    switch(id) {
        case BackgroundRemovedImage::IdCode:
            return BackgroundRemovedImage::fromDisk(is, bak);
            break;
        default:
            return BackgroundRemovedImage::fromDisk(is, bak);
            break;
    }
    return NULL;
}


BackgroundRemovedImageLoader::BackgroundRemovedImageLoader() {
}

BackgroundRemovedImageLoader::BackgroundRemovedImageLoader(const BackgroundRemovedImageLoader& orig) {
}

BackgroundRemovedImageLoader::~BackgroundRemovedImageLoader() {
}

