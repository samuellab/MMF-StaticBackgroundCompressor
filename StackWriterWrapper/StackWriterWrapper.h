/* 
 * File:   StackWriterWrapper.h
 * Author: Marc
 *
 * Created on March 16, 2011, 2:31 PM
 */

#ifndef STACKWRITERWRAPPER_H
#define	STACKWRITERWRAPPER_H

#ifdef	__cplusplus
extern "C" {
#endif


    #ifdef BUILD_DLL
    /* DLL export */
    #define EXPORT __declspec(dllexport)
    #else
    /* EXE import */
    #define EXPORT __declspec(dllimport)
    #endif

    EXPORT void *createBrightFieldStackWriter (const char *fname, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSizem, int keyFrameInterval, double frameRate);


    EXPORT void destroyStackWriter (void *sw);
    
 
    EXPORT int addFrame (void *sw, void *ipl_im);

    EXPORT int setMetaData (void *sw, char *fieldname, double fieldvalue);
    
    EXPORT int startRecording (void *sw, int nframes);

    EXPORT int stopRecording (void *sw);

    EXPORT unsigned long long numBytesWritten (void *sw);

    EXPORT int getTimingStatistics (void *sw, double *avgAddTime, double *avgCompressTime, double *avgWriteTime);

    EXPORT int getTimingReport (void *sw, char *dst, int maxchars);
    
    EXPORT unsigned long long maxBytesSupported ();

#ifdef	__cplusplus
}
#endif

#endif	/* STACKWRITERWRAPPER_H */

