/* 
 * File:   StackReader.cpp
 * Author: Marc
 * 
 * Created on October 27, 2010, 4:32 PM
 */

#include <map>

#include "StackReader.h"
#include "highgui.h"

using namespace std;

StackReader::StackReader() {
    init();
}

StackReader::StackReader(const StackReader& orig) {
}

StackReader::~StackReader() {
    closeInputFile();
    if (sbc != NULL) {
        delete sbc;
        sbc = NULL;
    }
}



void StackReader::init() {
    infile = NULL;
    sbc = NULL;
    totalFrames = 0;
    startFrame = 0;
    endFrame = 0;
}

StackReader::StackReader(const char* fname) {
    init();
    setInputFileName(fname);
    openInputFile();
}

void StackReader::setInputFileName(const char* fname) {
    this->fname = string(fname);
}

void StackReader::closeInputFile() {
    if (infile != NULL) {
        infile->close();
        delete infile;
        infile = NULL;
    }
}

void StackReader::openInputFile() {
    closeInputFile();
    if (!fname.empty()) {
        infile = new ifstream(fname.c_str(), ifstream::binary);
    }
    if (infile == NULL) {
        return;
    }
    parseInputFile();
}

void StackReader::parseInputFile() {
    cout << "parse input file\n";
    infile->seekg(0, ios::end);
    ifstream::pos_type length = infile->tellg();
    infile->seekg(0, ios::beg); //go to start
    
    //blow through text header
    char c = 'a';
    while (c != '\0') {
        infile->get(c);
    }
    unsigned long idcode;
    infile->read((char *) &idcode, sizeof(idcode));
    cout << "id code = " << hex << idcode << dec << "\n";
    //find out headerSize & skip to end
    int headerSize;
    infile->read((char *) &headerSize, sizeof(headerSize));
    infile->seekg(headerSize);

    //determine location of keyframes/common background stacks in file
    int startFrame = 0;
    StaticBackgroundCompressor::HeaderInfoT hi;
    while(infile->good()) {
        ifstream::pos_type cpos = infile->tellg();
        hi = StaticBackgroundCompressor::getHeaderInfo(*infile);
        keyframelocations.insert(std::make_pair(startFrame, cpos));
        startFrame = startFrame + hi.numframes;
        cpos += (ifstream::pos_type) hi.totalSize;

        if (cpos >= length) {
            break;
        }
        infile->seekg(cpos);
    }
    totalFrames = startFrame;
    infile->clear();
}

void StackReader::setSBC(int frameNum) {
    //if the frame is out of range or the infile isn't open, abort
    if (frameNum < 0 || frameNum >= totalFrames || infile == NULL) {
        return;
    }

    //if we have already loaded an sbc, and it's the right one for our frame, do nothing
    if (sbc != NULL && startFrame <= frameNum && frameNum < endFrame) {
        return;
    }

    //otherwise, nuke the old sbc (if any) and load the right one
    if (sbc != NULL) {
        delete sbc;
        sbc = NULL;
    }
    //find the first keyframe after frameNum, then go one back
    map<int, ifstream::pos_type>::iterator it = keyframelocations.upper_bound(frameNum);
    if (it != keyframelocations.begin()) {
        --it;
    }
  //  cout << "sbc starting with frame " << it->first << "located at " << it->second << " on disk\n";
    infile->seekg(it->second, ifstream::beg);
    startFrame = it->first;
    sbc = StaticBackgroundCompressor::fromDisk(*infile);
    if (sbc == NULL) {
        return;
    }
    endFrame = startFrame + sbc->numProcessed();
}

void StackReader::getBackground(int frameNum, IplImage** dst, int frameRange) {
    setSBC(frameNum);
    sbc->copyBackground(dst);
    if (frameRange > 0) {
        //find a range of points with frameNum in the approximate center
        int start = (int) (frameNum - frameRange/2);
        start = start < 0 ? 0 : start;
        int stop = start + frameRange > totalFrames ? totalFrames : start + frameRange;
        start = stop - frameRange < 0 ? 0 : stop - frameRange;
        //iterate through all frames; every time the sbc changes, take the minimum of that sbc's background
        //and the accumulated background so far
        for (int j = start; j < stop; ++j) {
            if (j < startFrame || j >= endFrame) {
                setSBC(j);
                cvMin(sbc->getBackground(), *dst, *dst);
            }
        }
    }
}

void StackReader::getFrame(int frameNum, IplImage** dst) {
    setSBC(frameNum);
    if (sbc != NULL) {
        sbc->reconstructFrame(frameNum - startFrame, dst);
    }
}

void StackReader::annotatedFrame(int frameNum, IplImage** dst) {
    setSBC(frameNum);
    IplImage *buffer = NULL;
    if (sbc != NULL) {
        sbc->annotatedFrame(frameNum - startFrame, &buffer, dst);
    }
    cvReleaseImage(&buffer);
}

void StackReader::playMovie(int startFrame, int endFrame, int delay_ms, char* windowName) {
    startFrame = startFrame < 0 ? 0 : startFrame;
    endFrame = endFrame > totalFrames ? totalFrames : endFrame;
    endFrame = endFrame < 0 ? totalFrames : endFrame;

    if (windowName == NULL) {
        windowName = "background restored movie";
    }
    cvNamedWindow(windowName, 0);
    IplImage *im = NULL;
   // IplImage *colorim = NULL;
    for (int f = startFrame; f < endFrame; ++f) {
     //  cout << f << "\n";
        annotatedFrame(f, &im);
        


        cvShowImage(windowName, im);
        if (tolower(cvWaitKey(delay_ms)) == 'q') {
            break;
        }
    }
    if (im != NULL) {
        cvReleaseImage(&im);
    }
    cvDestroyWindow(windowName);
}
