/* 
 * File:   StackReaderWrapper.h
 * Author: Marc
 *
 * Created on November 3, 2010, 2:58 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef STACKREADERWRAPPER_H
#define	STACKREADERWRAPPER_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef EXPORT
    #ifdef BUILD_DLL
    /* DLL export */
    // Generic helper definitions for shared library support
    #if defined _WIN32 || defined __CYGWIN__
        #define EXPORT __declspec(dllexport)
    #else
        #if __GNUC__ >= 4
            #define EXPORT __attribute__ ((visibility ("default")))
        #else
            #define EXPORT
        #endif
    #endif
    #else
        #if defined _WIN32 || defined __CYGWIN__
            #define EXPORT __declspec(dllimport)
        #else
            #if __GNUC__ >= 4
                #define EXPORT __attribute__ ((visibility ("default")))
            #else
                #define EXPORT
            #endif
        #endif
    #endif
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

    EXPORT void getImageSize(void* SR, int *width, int *height);

    EXPORT void compressImageStack (const char *fstub, const char *extension, const char *outname, int startFrame, int endFrame, int diffThresh, int smallDimMinSize, int lgDimMinSize);

    EXPORT void createSupplementalDataFile (void *SR, const char *fname);

    EXPORT void decimateStack (void *SR, const char *outname, int threshAboveBackground, int smallDimMinSize, int lgDimMinSize, int decimationCount);
#ifdef	__cplusplus
}
#endif

#endif	/* STACKREADERWRAPPER_H */

