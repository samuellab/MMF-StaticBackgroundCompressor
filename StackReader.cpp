/* 
 * File:   StackReader.cpp
 * Author: Marc
 * 
 * Created on October 27, 2010, 4:32 PM
 */

#include <map>
#include <iostream>

#include "StackReader.h"
#include "highgui.h"
#include "Timer.h"
#include "StaticBackgroundCompressorLoader.h"

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
    iserror = false;
    errormessage = "no error";
    totalFrames = 0;
    startFrame = 0;
    endFrame = 0;
    validROI.x = 0; validROI.y = 0; validROI.width = -1; validROI.height = -1;
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
        iserror = true;
        errormessage = "failed to open " + fname;
        cout << "failed to open " << fname;
        return;
    }
    parseInputFile();
}

void StackReader::parseInputFile() {
   // cout << "parse input file " << fname << endl;
    infile->seekg(0, ios::end);
    ifstream::pos_type length = infile->tellg();
    infile->seekg(0, ios::beg); //go to start
 //   cout << "infile length = " << length << endl;
    if (length < 0 || !infile->good()) {
        setError("length < 0 or !infile->good");
        delete infile;
        infile = NULL;
        return;
    }
    //blow through text header
    char c = 'a';
    while (c != '\0') {
        infile->get(c);
        cout << c;
        if ((infile->tellg() % 10240) == 0) {
            cout << (infile->tellg()/1024) << " kb into header";
        }
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
        hi = StaticBackgroundCompressorLoader::getHeaderInfo(*infile);
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
    if (frameNum < 0 || frameNum >= totalFrames || !dataFileOk()) {
        if (!dataFileOk()) {
            if (infile == NULL) {
                std::cerr << "infile is NULL";
                assert(false);
            } else {
                if (infile->bad()) {
                    std::cerr << "infile badbit is set indicating loss of integrity of the file stream";
                    assert(false);
                } else if (infile->fail()) {
                    std::cerr << "infile failbit is set indicating previous operation did not complete OK";
                    assert(false);
                } else {
                    std::cerr << "dataFileOK returned false, but all indications are data file is OK";
                    assert(false);
                }
            }
            return;
        }

            
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
    cout << "sbc starting with frame " << it->first << "located at " << it->second << " on disk\n";
    infile->seekg(it->second, ifstream::beg);
    startFrame = it->first;
    sbc = StaticBackgroundCompressorLoader::fromFile(*infile);
    if (sbc == NULL) {
        setError("failed to read static background compressor from disk for frame number " + frameNum);
        std::cerr << "failed to read static background compressor from disk for frame number " << frameNum;
        return;
    }
    endFrame = startFrame + sbc->numProcessed();
    validROI = sbc->getValidRoi();
}

void StackReader::getBackground(int frameNum, IplImage** dst, int frameRange) {

    setSBC(frameNum);
    if (!dataFileOk() || sbc == NULL) {
        if (*dst != NULL) {
            cvReleaseImage(dst);
            *dst = NULL;
        }
        return;
    }
    sbc->copyBackground(dst);
    if (frameRange > 0) {
        //find a range of points with frameNum in the approximate center
        int start = (int) (frameNum - frameRange/2);
        start = start < 0 ? 0 : start;
        int stop = start + frameRange > totalFrames ? totalFrames : start + frameRange;
        start = stop - frameRange < 0 ? 0 : stop - frameRange;
        //iterate through all frames; every time the sbc changes, take the minimum of that sbc's background
        //and the accumulated background so far
        IplImage *temp = NULL;
        for (int j = start; j < stop; ++j) {
            if (j < startFrame || j >= endFrame) {
                setSBC(j);
                sbc->copyBackground(&temp);
                cvMin(temp, *dst, *dst);
            }
        }
        if (temp != NULL) {
            cvReleaseImage(&temp);
        }
    }
}

void StackReader::getFrame(int frameNum, IplImage** dst) {
    setSBC(frameNum);
    if (!dataFileOk() || sbc == NULL) {
        if (*dst != NULL) {
            cvReleaseImage(dst);
            *dst = NULL;
        }
        return;
    }
    if (sbc != NULL) {
        sbc->reconstructFrame(frameNum - startFrame, dst);
    }
}

void StackReader::annotatedFrame(int frameNum, IplImage** dst) {
    setSBC(frameNum);
    if (!dataFileOk() || sbc == NULL) {
        if (*dst != NULL) {
            cvReleaseImage(dst);
            *dst = NULL;
        }
        return;
    }
    IplImage *buffer = NULL;
    if (sbc != NULL) {
        sbc->annotatedFrame(frameNum - startFrame, &buffer, dst);
    }
    cvReleaseImage(&buffer);
}

void StackReader::playMovie(int startFrame, int endFrame, int delay_ms, char* windowName, bool annotated) {
    startFrame = startFrame < 0 ? 0 : startFrame;
    endFrame = endFrame > totalFrames ? totalFrames : endFrame;
    endFrame = endFrame < 0 ? totalFrames : endFrame;

    if (windowName == NULL) {
        windowName = "background restored movie";
    }
    cvNamedWindow(windowName, 0);
    IplImage *im = NULL;
   // IplImage *colorim = NULL;

    Timer tim;
    for (int f = startFrame; f < endFrame; ++f) {

        tim.start();
     //  cout << f << "\n";
        if (annotated) {
            annotatedFrame(f, &im);
        } else {
            getFrame(f, &im);
        }
        if (im == NULL) {
            break;
        }


        cvShowImage(windowName, im);
        int delaytime = (int) (delay_ms - tim.getElapsedTimeInMilliSec());
        tim.stop();
        delaytime = delaytime < 1 ? 1: delaytime;
        if (tolower(cvWaitKey(delaytime)) == 'q') {
            break;
        }
    }
    if (im != NULL) {
        cvReleaseImage(&im);
    }
    cvDestroyWindow(windowName);
}

ExtraDataWriter *StackReader::getSupplementalData() {
    ExtraDataWriter *edw = new ExtraDataWriter();
    for (int frameNum = 0; frameNum < totalFrames; ++frameNum) {
        setSBC(frameNum);
        const ImageMetaData *imd = sbc->getMetaData(frameNum-startFrame);
        edw->goToFrame(frameNum);
        if (imd != NULL) {
            edw->addElements(imd->getFieldNamesAndValues());
        }
        edw->addElement(string("FrameNumber"), (double) frameNum);
        edw->addElement(string("NumberOfRegionsDiffFromBackground"), (double) sbc->numRegionsInFrame(frameNum-startFrame));
    }
    return edw;
}

void StackReader::createSupplementalDataFile(const char* fname) {
    ExtraDataWriter *edw = getSupplementalData();
    edw->writeFile(fname);
    delete edw;
}

CvRect StackReader::getLargestROI() {
    if (validROI.width <= 0 || validROI.height <= 0) {
        if (sbc == NULL) {
            setSBC(0);
        }
        assert (sbc != NULL);
        validROI = sbc->getValidRoi();
    }
    return validROI;
}