/* 
 * File:   BackgroundRemovedImageLoader.h
 * Author: Marc
 *
 * Created on November 5, 2010, 3:56 PM
 *
 * a factory class that loads the proper subclass of background removed image from disk
 * if you implement a subclass of background removed image, add its id code and load method to the
 * case structure in fromFile
 */

#ifndef BACKGROUNDREMOVEDIMAGELOADER_H
#define	BACKGROUNDREMOVEDIMAGELOADER_H

#include "BackgroundRemovedImage.h"

/* BackgroundRemovedImageLoader
 *
 * a factory class that loads the proper subclass of background removed image from disk
 * if you implement a subclass of background removed image, add its id code and load method to the
 * case structure in fromFile
 *
 */

class BackgroundRemovedImageLoader {
public:
    static BackgroundRemovedImage *fromFile(std::ifstream &is, const IplImage *bak);
    virtual ~BackgroundRemovedImageLoader();
private:
   BackgroundRemovedImageLoader();
    BackgroundRemovedImageLoader(const BackgroundRemovedImageLoader& orig);

};

#endif	/* BACKGROUNDREMOVEDIMAGELOADER_H */

