/* 
 * File:   IPLtoLV.h
 * Author: Marc
 *
 * Created on November 3, 2010, 5:19 PM
 */

#ifndef IPLTOLV_H
#define	IPLTOLV_H

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

    /*
     * creates an IplImage from a labview image pointer; single channel, depth 8U
     * returns NULL on failure
     */

    EXPORT void *IplImageFromLVImage (void *lvsrc, int lvwidth, int lvheight, int lvlinewidth);
    /*
     * places size of ipl image in width & height
     * returns 0 on sucess, -1 if any pointer is null
     */
    EXPORT int IplImageSize (void *iplImage, int *width, int *height);
    /*
     * copies ipl image to labview image
     * returns 0 on success, -1 if any pointer is null, -2 if there is a dimension mismatch
     *
     */
    EXPORT int LVImageFromIplImage (void *iplImage, void *lvdst, int lvwidth, int lvheight, int lvlinewidth);

    /*
     * releases memory storage for iplImage
     *
     */
    EXPORT void destroyIplImage (void *iplImage);
#ifdef	__cplusplus
}
#endif

#endif	/* IPLTOLV_H */

