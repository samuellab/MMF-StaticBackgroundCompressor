/* 
 * File:   wtscWrapper.h
 * Author: Marc
 *
 * Created on March 16, 2011, 2:22 PM
 */
#include "WindowsThreadStackCompressor.h"
#include "NameValueMetaData.h"
#include "Timer.h"
#ifndef WTSCWRAPPER_H
#define	WTSCWRAPPER_H

struct wtscWrapper {
    public:
        WindowsThreadStackCompressor wtsc;
        NameValueMetaData md;
        Timer tim;
};




#endif	/* WTSCWRAPPER_H */

