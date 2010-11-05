/* 
 * File:   StaticBackgroundCompressorLoader.h
 * Author: Marc
 *
 * Created on November 5, 2010, 4:14 PM
 */

#ifndef STATICBACKGROUNDCOMPRESSORLOADER_H
#define	STATICBACKGROUNDCOMPRESSORLOADER_H

#include "StaticBackgroundCompressor.h"


class StaticBackgroundCompressorLoader {
public:
    static StaticBackgroundCompressor *fromFile (std::ifstream &is);
    static StaticBackgroundCompressor::HeaderInfoT getHeaderInfo(std::ifstream &is);
    virtual ~StaticBackgroundCompressorLoader();
private:
    StaticBackgroundCompressorLoader();
    StaticBackgroundCompressorLoader(const StaticBackgroundCompressorLoader& orig);

};

#endif	/* STATICBACKGROUNDCOMPRESSORLOADER_H */

