/* 
 * File:   BackgroundRemovedImageLoader.cpp
 * Author: Marc Gershow
 * 
 * Created on November 5, 2010, 3:56 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
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

