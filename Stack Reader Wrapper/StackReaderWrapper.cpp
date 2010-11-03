#include "StackReaderWrapper.h"
#include "cv.h"
#include "StackReader.h"

void *createStackReader(const char* fname) {
    if (fname == NULL) {
        return NULL;
    }
    StackReader *sr = new StackReader(fname);
    if (sr == NULL) {
        return NULL;
    }
    if (!sr->dataFileOk()) {
        delete sr;
        return NULL;
    }
    return (void *) sr;
}

void destroyStackReader(void* SR) {
    StackReader *sr = (StackReader *) SR;
    if (sr != NULL) {
        delete sr;
    }
}

void *getFrame(void* SR, int frameNumber) {
    StackReader *sr = (StackReader *) SR;
    if (sr == NULL) {
        return NULL;
    }
    if (!sr->dataFileOk()) {
        return NULL;
    }
    IplImage *dst = NULL;

    sr->getFrame(frameNumber, &dst);
    return (void *) dst;
}

void *getBackground(void* SR, int frameNumber, int frameRange) {
    StackReader *sr = (StackReader *) SR;
    if (sr == NULL) {
        return NULL;
    }
    if (!sr->dataFileOk()) {
        return NULL;
    }
    IplImage *dst = NULL;
    sr->getBackground(frameNumber, &dst, frameRange);
    return (void *) dst;
}
