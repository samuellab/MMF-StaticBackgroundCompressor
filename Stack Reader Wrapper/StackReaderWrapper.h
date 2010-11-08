/* 
 * File:   StackReaderWrapper.h
 * Author: Marc
 *
 * Created on November 3, 2010, 2:58 PM
 */

#ifndef STACKREADERWRAPPER_H
#define	STACKREADERWRAPPER_H

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

    EXPORT void *createStackReader (const char *fname);
    EXPORT void destroyStackReader (void *sr);
    /* EXPORT void *getFrame (void *sr, int frameNumber);
     * returns a pointer to an IplImage, receiver is responsible for deallocation
     * returns 0 on error
     */
    EXPORT void *getFrame (void *sr, int frameNumber);
    /* EXPORT void *getBackground (void *sr, int frameNumber, int frameRange);
     * returns a pointer to an IplImage, receiver is responsible for deallocation
     * returns 0 on error
     */
    EXPORT void *getBackground (void *sr, int frameNumber, int frameRange);

    EXPORT int getTotalFrames (void *sr);

    EXPORT void compressImageStack (const char *fstub, const char *extension, const char *outname, int startFrame, int endFrame, int diffThresh, int smallDimMinSize, int lgDimMinSize);
#ifdef	__cplusplus
}
#endif

#endif	/* STACKREADERWRAPPER_H */

