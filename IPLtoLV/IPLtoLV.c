/* IPLtoLV - converts a labview image to an ipl image and vice versa
 *
 * derived from code in this forum post: http://forums.ni.com/t5/Machine-Vision/Using-OpenCV-library-in-LabVIEW/td-p/648429
 * 
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include "cv.h"
#include "IPLtoLV.h"
#include <stdio.h>

void *IplImageFromLVImage(void* lvsrc, int lvwidth, int lvheight, int lvlinewidth) {
    IplImage *lvim, *im;
    if (lvsrc == NULL || lvwidth <= 0 || lvheight <= 0 || lvlinewidth < lvwidth) {
        return NULL;
    }
    lvim = cvCreateImageHeader(cvSize(lvwidth, lvheight), IPL_DEPTH_8U, 1);
    cvSetData(lvim, lvsrc, lvlinewidth);
    im = cvCloneImage(lvim);
    cvReleaseImageHeader(&lvim);
    return im;
}

int IplImageSize(void* iplImage, int* width, int* height) {
   // FILE *f;
    IplImage *im = (IplImage *) iplImage;
    if (iplImage == NULL || width == NULL || height == NULL) {
        return -1;
    }
 //   f = fopen("c:\\foo.txt", "w");
  //  fprintf (f, "iplImage = %d: width, height %d,%d\n", (unsigned long) im, im->width, im->height);
  //  fclose(f);
    *width = im->width;
    *height = im->height;
    return 0;
}

int LVImageFromIplImage(void* iplImage, void* lvdst, int lvwidth, int lvheight, int lvlinewidth) {
    IplImage *im = (IplImage *) iplImage;
    IplImage *lvim;
    if (iplImage == NULL || lvdst == NULL) {
        return -1;
    }
    if (im->width != lvwidth || im->height != lvheight) {
        return -2;
    }
    lvim = cvCreateImageHeader(cvSize(lvwidth, lvheight), IPL_DEPTH_8U, 1);
    cvSetData(lvim, lvdst, lvlinewidth);
    cvCopyImage(im, lvim);
    cvReleaseImageHeader(&lvim);
    return 0;
}

void destroyIplImage(void* iplImage) {
    IplImage *im = (IplImage *) iplImage;
    if (im != NULL) {
        cvReleaseImage(&im);
    }
}
