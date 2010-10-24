/* 
 * File:   LinearStackCompressor.h
 * Author: Marc
 *
 * Created on October 24, 2010, 3:29 PM
 *
 * compresses images as they are added to the stack without multi-threading
 */

#ifndef LINEARSTACKCOMPRESSOR_H
#define	LINEARSTACKCOMPRESSOR_H

#include "StaticBackgroundCompressor.h"


class LinearStackCompressor {
public:
    LinearStackCompressor();
   
    virtual ~LinearStackCompressor();
protected:
    int keyframeInterval;
    enum recordingState {idle, updatingBackground, recording};
    int framesToRecord;
    std::ofstream *outfile;
    std::vector<StaticBackgroundCompressor *> imageStacks;
    
private:
     LinearStackCompressor(const LinearStackCompressor& orig);
};

#endif	/* LINEARSTACKCOMPRESSOR_H */

