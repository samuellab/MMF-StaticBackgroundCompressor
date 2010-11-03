#include "StackReaderWrapper.h"
#include "cv.h"
#include "StackReader.h"
#include <ostream>

using namespace std;

void *createStackReader(const char* fname) {
  //  ofstream os("c:\\stackreadlog.txt");
    if (fname == NULL) {
   //     os << "fname was null " << endl;
        return NULL;
    }
 //   os << "trying to open " << fname << endl;
    StackReader *sr = new StackReader(fname);
    if (sr == NULL) {
 //       os << "sr is null" << endl;
        return NULL;
    }
    if (!sr->dataFileOk()) {
 //       os << "data file not OK" << endl;
        delete sr;
        return NULL;
    }
 //   os << "returning sr" << endl;
 //   os.close();
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
  //  ofstream os("c:\\stackreadlog.txt");
//    os << "dst = " << (unsigned long) dst << "  dst has width " << dst->width << " and height " << dst->height << endl;
 //   os.close();
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

int getTotalFrames(void* SR) {
    StackReader *sr = (StackReader *) SR;
    if (sr == NULL || !sr->dataFileOk()) {
        return -1;
    }
    return sr->getTotalFrames();

}
