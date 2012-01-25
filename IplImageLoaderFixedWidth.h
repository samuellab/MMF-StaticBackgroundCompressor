/* 
 * File:   IplImageLoaderFixedWidth.h
 * Author: Marc
 *
 * Created on January 25, 2012, 11:36 AM
 */

#ifndef IPLIMAGELOADERFIXEDWIDTH_H
#define	IPLIMAGELOADERFIXEDWIDTH_H
#include <stdint.h>
#include <cv.h>


class IplImageLoaderFixedWidth {
    public:
        virtual ~IplImageLoaderFixedWidth();
        static IplImage *loadIplImageFromByteStream(std::ifstream& is);
    private:
        IplImageLoaderFixedWidth();
        IplImageLoaderFixedWidth(const IplImageLoaderFixedWidth& orig);
};



#endif	/* IPLIMAGELOADERFIXEDWIDTH_H */

