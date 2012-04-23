/* 
 * File:   StackReader.cpp
 * Author: Marc
 * 
 * Created on October 27, 2010, 4:32 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#include <map>
#include <iostream>
#include <sstream>
#ifdef _WIN32
// Hopefully one day the code may be less windows-centric :)
    #include <windows.h> //required to use multithreaded stack writing
#endif
#include "StackReader.h"
#include "highgui.h"
#include "Timer.h"
#include "StaticBackgroundCompressorLoader.h"
#ifdef _WIN32
    #include "WindowsThreadStackCompressor.h"
#else
    #include "LinearStackCompressor.h"
#endif
using namespace std;

#ifndef _WIN32
    void Sleep (int t);
    void Sleep (int t) {
        cvWaitKey(t);
    }
#endif

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
    cout << "parse input file " << fname << endl;
    infile->seekg(0, ios::end);
    ifstream::pos_type length = (ifstream::pos_type) infile->tellg();
    infile->seekg(0, ios::beg); //go to start
    cout << "infile length = " << length << endl;
    
    if (length < 0 || !infile->good()) {
        stringstream ss;
        ss << "sizeof length = " << sizeof(length) << " length = " << length << " infile->fail = " << infile->fail() << " infile->good() = " << infile->good();
        setError (ss.str().c_str());
        setError("length < 0 or !infile->good");
        delete infile;
        infile = NULL;
        return;
    }
    //blow through text header
    char c = 'a';
    while (c != '\0') {
        infile->get(c);
        if (false){
            cout << c;
            if ((infile->tellg() % 10240) == 0) {
           //     cout << (infile->tellg()/1024) << " kb into header";
            }
        }
    }
    uint32_t idcode;
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
  //      cout << "frame was ok" << endl;
        return;
    }

    //otherwise, nuke the old sbc (if any) and load the right one
    if (sbc != NULL) {
  //      cout << "nuking sbc" << endl;
        delete sbc;
        sbc = NULL;
    }
    //find the first keyframe after frameNum, then go one back
    map<int, ifstream::pos_type>::iterator it = keyframelocations.upper_bound(frameNum);
    if (it != keyframelocations.begin()) {
        --it;
    }
   // cout << "sbc starting with frame " << it->first << "located at " << it->second << " on disk\n";
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

CvSize StackReader::getImageSize() {
    if (sbc == NULL) {
        if (!dataFileOk()) {
            setError("data file not properly opened prior to asking for image size");
            return cvSize(0,0);
        }
        setSBC(startFrame);
    }
    if (sbc == NULL) {
        return cvSize(0,0);
    }
    return sbc->getFrameSize();

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

void StackReader::playMovie(int startFrame, int endFrame, int delay_ms, const char* windowName, bool annotated) {
  //  cout << "entered play movie" << endl;
    
    startFrame = startFrame < 0 ? 0 : startFrame;
    endFrame = endFrame > totalFrames ? totalFrames : endFrame;
    endFrame = endFrame < 0 ? totalFrames : endFrame;

    if (windowName == NULL) {
        windowName =  "background restored movie";
    }
  //  cout << "cvNamedWindow" << windowName << endl;
    cvNamedWindow(windowName, 0);
    IplImage *im = NULL;
   // IplImage *colorim = NULL;

    Timer tim;
//    cout << "starting loop" << endl;
    Sleep(2000); // removed on linux branch
    for (int f = startFrame; f < endFrame; ++f) {

        tim.start();
       // cout << f << "\t";//Sleep(1000);
        if (annotated) {
            annotatedFrame(f, &im);
        } else {
         //   cout << "calling get frame" << endl;// Sleep(1000);
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
        if (iserror || sbc == NULL) {
            break;
        }
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

int StackReader::decimateStack(const char* outputname, int thresholdAboveBackground, int smallDimMinSize, int lgDimMinSize, int decimationCount) {
    #ifdef _WIN32
    WindowsThreadStackCompressor sc;
    #else
    LinearStackCompressor sc;
    #endif
    bool showFrames = false;
    sc.setThresholds(0, thresholdAboveBackground, smallDimMinSize, lgDimMinSize);
    setSBC(0);
    sc.setIntervals(sbc->numProcessed(), 1);
    sc.setOutputFileName(outputname);

    IplImage *im = NULL;
   // IplImage *colorim = NULL;

    Timer tim;
   
   // cout << outputname;
    
    sc.openOutputFile();

    double frameRate = 100;
    #ifndef _WIN32
    frameRate = 10;
    #endif
    sc.setFrameRate(frameRate);

    #ifdef _WIN32
    sc.startThreads();
    #endif

    sc.startRecording(totalFrames);
    tim.start();
    int ethundred = 0;
    if (showFrames) {
        cvNamedWindow("source movie frame",0);
    }
    for (int f = 0; f < totalFrames; f += decimationCount) {
       // cout << f << ", " << f/decimationCount/tim.getElapsedTimeInSec() << " Hz" << "\t";
        
        getFrame(f, &im);
        
        if (im == NULL) {
            break;
        }
        setSBC(f);
        const ImageMetaData* imd = sbc->getMetaData(f - startFrame);
        if (false) {
            cout << imd->saveDescription();
            cout << imd->clone()->saveDescription();

        }
        if (false) {
            map<string,double>mdp = imd->getFieldNamesAndValues();
            for (map<string, double>::const_iterator it = mdp.begin(); it != mdp.end(); ++it) {
                cout << it->first << "," << it->second << "\t";
            }
            cout << "\n";
        }
    
        if (imd != NULL) {
            sc.newFrame(im, imd->clone());
        } else {
            sc.newFrame(im, NULL);
        }
        if (showFrames) {
            cvShowImage("source movie frame", im);
            cvWaitKey(1000/frameRate);
        }/*
        else {
            Sleep(1000/frameRate);
        }*/


        int ntc, ntw;
        sc.numStacksWaiting(ntc, ntw);
        /*
        if (ntc > 1 || ntw > 1) {
            cout << ntc << "waiting to be compressed " << ntw << " waiting to be written" << endl;
        }
         * */
        #ifdef _WIN32
        while (ntc > 1 || ntw > 1) {
            Sleep(200);
            sc.numStacksWaiting(ntc, ntw);
        }
        #endif

        if (ntc > 1 || ntw > 1) {
            cout << ntc << "ERROR: waiting to be compressed " << ntw << " waiting to be written" << endl;
        }

        if (((int) tim.getElapsedTimeInSec()/20) > ethundred) { //more frequent updates = fun
            ethundred = (int) tim.getElapsedTimeInSec()/20;
            cout << "et = " << tim.getElapsedTimeInSec() << ";  " << f << "/" << totalFrames << " done.  " << tim.getElapsedTimeInSec() / f *(totalFrames - f) << " s remain" <<endl;
            cout << (sc.numBytesWritten()>>20) << "MB written, " << f/decimationCount/tim.getElapsedTimeInSec() << " Hz" << endl;
        }

    }

//    sc.finishRecording();
    sc.closeOutputFile();
    if (im != NULL) {
        cvReleaseImage(&im);
        return 0;
    } else {
        return -1;
    }
}

std::string StackReader::diagnostics() {
    stringstream s;
    if (iserror) {
        s << "error: " << getError();
        return s.str();
    }
//    s << "no error" << endl;
    setSBC(0);
    if (sbc == NULL) {
        s << getError() << endl;
        s << "sbc was null" << endl;
        return s.str();
    }
    s << "sbc save description:\n" << sbc->saveDescription() << endl;
    //return s.str();
    s << "sbc has " << sbc->numProcessed() << "frames" << endl;
    s << "sbc frame size is " << sbc->getFrameSize().width << " x " << sbc->getFrameSize().height << endl;
    s << "frame 0:" << endl << "meta data:" << endl;
    const ImageMetaData* imd = sbc->getMetaData(0);
    if (imd == NULL) {
        s << "meta data is NULL" << endl;
    } else {
        s << imd->saveDescription() << endl;
        map<string,double>mdp = imd->getFieldNamesAndValues();
        for (map<string, double>::const_iterator it = mdp.begin(); it != mdp.end(); ++it) {
            s << it->first << "," << it->second << "\t";
        }
        s << endl;
    }
    s << sbc->numRegionsInFrame(0) << " regions different from background" << endl;
    //return s.str();
    IplImage *im = NULL;
    sbc->reconstructFrame(0, &im);
    s << "im params: w= " << im->width << ", h= " << im->height << ", nchannels = " << im->nChannels << ", width step = " << im->widthStep << "imageSize = " << im->imageSize << endl;

    return s.str();
    
}