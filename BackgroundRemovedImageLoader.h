/* 
 * File:   BackgroundRemovedImageLoader.h
 * Author: Marc
 *
 * Created on November 5, 2010, 3:56 PM
 */

#ifndef BACKGROUNDREMOVEDIMAGELOADER_H
#define	BACKGROUNDREMOVEDIMAGELOADER_H

#include "BackgroundRemovedImage.h"

class BackgroundRemovedImageLoader {
public:
    static BackgroundRemovedImage *fromFile(std::ifstream &is, const IplImage *bak);
    virtual ~BackgroundRemovedImageLoader();
private:
   BackgroundRemovedImageLoader();
    BackgroundRemovedImageLoader(const BackgroundRemovedImageLoader& orig);

};

#endif	/* BACKGROUNDREMOVEDIMAGELOADER_H */

