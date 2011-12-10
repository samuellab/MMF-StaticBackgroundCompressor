/* 
 * File:   StaticBackgroundCompressorLoader.h
 * Author: Marc
 *
 * Created on November 5, 2010, 4:14 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
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

