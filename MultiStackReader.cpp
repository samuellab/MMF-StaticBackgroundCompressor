/* 
 * File:   MultiStackReader.cpp
 * Author: Marc
 * 
 * Created on 9/11/2012
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#include <map>
#include <iostream>
#include <sstream>
#include <ctype.h>
#ifdef _WIN32
// Hopefully one day the code may be less windows-centric :)
    #include <windows.h> //required to use multithreaded stack writing
#endif
#include "MultiStackReader.h"
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
    static void Sleep (int t);
    static void Sleep (int t) {
        cvWaitKey(t);
    }
#endif

MultiStackReader::MultiStackReader() {
    init();
}

MultiStackReader::MultiStackReader(const MultiStackReader& orig) {
}

MultiStackReader::~MultiStackReader() {
    closeInputFile();
}

MultiStackReader::MultiStackReader(const char* fname) {
    init();
    setInputFileName(fname);
    openInputFile();
}

void MultiStackReader::init() {
    StackReader::init();
    sr.clear();
    fnames.clear();
}
//
//inline bool isopenbracket(char c) {
//    return ('{' == c || '[' == c  '(' == c);
//}
//
//inline bool isclosebracket(char c) {
//    return ('}' == c ||  ']' == c || ')' == c);
//}
//
//inline bool isbracket(char c) {
//    return isopenbracket(c) || isclosebracket(c);
//}
//
//inline bool isseparator(char c) {
//    return (',' == c || ';' == c)
//}
static string& trimwhitespace (string &s);
string& trimwhitespace (string &s) {
    const string whitespace(" \t\n\f\r");
    size_t pos = s.find_first_not_of(whitespace);
    if (pos > s.length()) {
        s.clear();
    } else {
        s = s.substr(pos, s.find_last_not_of(whitespace)-pos+1);
    }
    return s;
    
}
std::vector<std::string> MultiStackReader::parseFileNameInput (const char *fname) {
    //currently (9/12) only parses a single file name to a vector with that single file name, or 
    //a list of filenames {filename1, filename2, filename3} to a vector of file names
    //extensions could be to determine if the fname is a directory and use all files in that directory
    //or to do wild card searches, etc
    
    vector<string> fn;
    
    
    const string openbrackets("{[");
    const string closebrackets("]}");
    const string separators(",;");
    
    string s(fname);
    
    size_t pos = s.find_first_of(openbrackets);
    if (pos == string::npos) {
        fn.push_back(trimwhitespace(s));
        return fn;
    }
    //remove enclosing brackets
    s.erase(0,pos+1);
    pos = s.find_last_of(closebrackets);
    s.erase(pos, string::npos);
   // cout << "after removing brackets, s = " << endl << s << endl;
    
    
    while (!s.empty()) {
        pos = s.find_first_of(separators);
      //  pos = (pos == string::npos) ? pos : pos-1; 
        string ss = s.substr(0,pos);
       // cout << "pos = " << pos << endl;
       // cout << "pretrim substring = " << ss << endl;
        trimwhitespace(ss);
      //  cout << "posttrim substring = " << ss << endl;
        if (!ss.empty()) {
            fn.push_back(ss);
        }
        
        s.erase(0,MAX(pos,pos+1)); //max command takes care of wrapping at npos
        trimwhitespace(s);
       // cout << "post trim s = " << endl << s << endl;
    }
    return fn;
    
}

void MultiStackReader::setInputFileName(const char* fname) {
    setInputFileName(parseFileNameInput(fname));
}
void MultiStackReader::setInputFileName(const std::vector<std::string> fnames) {
    this->fnames = fnames;

}

void MultiStackReader::closeInputFile() {
    for (vector<pair<StackReader *, int> >::iterator it = sr.begin(); it != sr.end(); ++it) {
        delete it->first;
        it->first = NULL;
    }
    sr.clear();
    endFrames.clear();
}

void MultiStackReader::openInputFile() {
    closeInputFile();
    checkError(); if (isError()) { return; }
    int totalframes = 0;
    for (vector<string>::iterator it = fnames.begin(); it != fnames.end(); ++ it) {
        StackReader *s = new StackReader(it->c_str());
        sr.push_back(make_pair(s,totalframes));
        totalframes += s->getTotalFrames();
        endFrames.push_back(totalframes);
    }
    totalFrames = totalframes;
    checkError();
}

bool MultiStackReader::dataFileOk() {
        if (!sr.empty()) {
            for (vector<pair<StackReader *, int> >::iterator it = sr.begin(); it != sr.end(); ++it) {
                if (it->first == NULL || !it->first->dataFileOk()) {
                    return false;
                }
            }
            return true;
        } else {
            return false;
        }
        return false;
    }

CvSize MultiStackReader::getImageSize() {
    checkError(); 
    if (sr.empty()) {
        setError("get image size called with no stacks open");
    }
    if (isError()) { return cvSize(0,0); }
    int srnum = findStackReader(sr.front().second);
    if (srnum < 0 || srnum >= sr.size()) {
        if (!isError()) {
            setError("findStackReader returned bad value");           
        }
        return cvSize(0,0);
    }
    
    return sr.at(srnum).first->getImageSize();

}

void MultiStackReader::getBackground(int frameNum, IplImage** dst, int frameRange) {
    int srnum = findStackReader(frameNum);
    if (srnum < 0 || srnum >= sr.size()) {
        if (!isError()) {
            setError("findStackReader returned bad value");           
        }
        if (*dst != NULL) {
            cvReleaseImage(dst);
            *dst = NULL;
        }
        return;
    }
    sr.at(srnum).first->getBackground(frameNum-sr.at(srnum).second, dst, frameRange);
    checkError();
}

void MultiStackReader::getFrame(int frameNum, IplImage** dst) {
    int srnum = findStackReader(frameNum); checkError();
    if (isError() || srnum < 0 || srnum >= sr.size()) {
        if (!isError()) {
            setError("findStackReader returned bad value");           
        }
        if (*dst != NULL) {
            cvReleaseImage(dst);
            *dst = NULL;
        }
        return;
    }
    sr.at(srnum).first->getFrame(frameNum-sr.at(srnum).second, dst);
    checkError();
}

const ImageMetaData *MultiStackReader::getMetaData(int frameNum) {
    int srnum = findStackReader(frameNum); checkError();
    if (isError() || srnum < 0 || srnum >= sr.size()) {
        if (!isError()) {
            setError("findStackReader returned bad value");           
        }
        return NULL;
    }
    return sr.at(srnum).first->getMetaData(frameNum-sr.at(srnum).second);
}


void MultiStackReader::annotatedFrame(int frameNum, IplImage** dst) {
    int srnum = findStackReader(frameNum); checkError();
    if (isError() || srnum < 0 || srnum >= sr.size()) {
        if (!isError()) {
            setError("findStackReader returned bad value");           
        }
        if (*dst != NULL) {
            cvReleaseImage(dst);
            *dst = NULL;
        }
        return;
    }
    sr.at(srnum).first->annotatedFrame(frameNum-sr.at(srnum).second, dst);
}

int MultiStackReader::getKeyFrameInterval() {
    if (sr.empty() || sr.at(0).first == NULL) {
        setError("no open stack readers in getKeyFrameInterval");
        return -1;
    }
    
    
    int kfi = sr.at(0).first->getKeyFrameInterval();
    checkError();
    return kfi;
}

ExtraDataWriter *MultiStackReader::getSupplementalData() {
    return addToSupplementalData(NULL,0);
}
ExtraDataWriter *MultiStackReader::addToSupplementalData(ExtraDataWriter* edw, int frameOffset) {
    for (vector<pair<StackReader *, int> >::iterator it = sr.begin(); it != sr.end(); ++it) {
        edw = it->first->addToSupplementalData(edw, it->second + frameOffset);
    }
    checkError();
    return edw;
}

CvRect MultiStackReader::getLargestROI() {
    if (validROI.width <= 0 || validROI.height <= 0) {
        if (sr.empty()) {
            setError("getLargestROI called with no stack readers open");
            return validROI;
        }
        StackReader *s = sr[0].first;
        if (s == NULL) {
            setError("first stackReader is NULL");
            return validROI;
        }
        validROI = s->getLargestROI();
    }
    checkError();
    return validROI;
}




int MultiStackReader::findStackReader(int frameNumber) {
    checkError();    if (isError()) {return -1;}
    int nstacks = MIN (endFrames.size(), sr.size());
    for (int j = 0; j < nstacks; ++j) {
        if (sr.at(j).second <= frameNumber && endFrames.at(j) > frameNumber) {
            return j;
        }
    }
    
    stringstream s;
    if (endFrames.size()*sr.size() == 0) {
        s << "tried to set stack reader with no open stacks";
    } else {
        s << "could not find frame number " << frameNumber << " : frame ranges are: ";
        for (int j = 0; j < nstacks; ++j) {
            s << "[" <<sr.at(j).second << "," << endFrames.at(j) << ");  ";
        }
    }
    setError(s.str());
    return -1;
}

void MultiStackReader::checkError() {
    if (isError()) {
        return;
    }
    int j = 0;
    for (vector<pair<StackReader *, int> >::iterator it = sr.begin(); it != sr.end(); ++it) {
        if (it->first->isError()) {
            stringstream s;
            s << "stack #" << j << ": " <<it->first->getError();
            setError(s.str());
            return;
        }
        j++;
    }
}

std::string MultiStackReader::diagnostics() {
    stringstream s;
    if (sr.empty()) {
        return "no stacks opened";
    }
    int j = 0;
    for (vector<pair<StackReader *, int> >::iterator it = sr.begin(); it != sr.end(); ++it) {
        s << "stack #" << j++ << ": " << it->first->diagnostics();
    }
    return s.str();
}