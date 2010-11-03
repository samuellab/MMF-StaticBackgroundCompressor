#include "cv.h"
#include "IPLtoLV.h"

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
    IplImage *im = (IplImage *) iplImage;
    if (iplImage == NULL || width == NULL || height == NULL) {
        return -1;
    }
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
