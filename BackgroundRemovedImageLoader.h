/* 
 * File:   BackgroundRemovedImageLoader.h
 * Author: Marc Gershow
 *
 * Created on November 5, 2010, 3:56 PM
 *
 * a factory class that loads the proper subclass of background removed image from disk
 * if you implement a subclass of background removed image, add its id code and load method to the
 * case structure in fromFile
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
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

