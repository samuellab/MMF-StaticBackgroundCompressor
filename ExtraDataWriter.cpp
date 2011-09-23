/* 
 * File:   ExtraDataWriter.cpp
 * Author: Marc
 * 
 * Created on November 10, 2010, 3:43 PM
 */

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "ExtraDataWriter.h"

using namespace std;

ExtraDataWriter::ExtraDataWriter() {
    currentFrame = NULL;
    currentFrameNumber = 0;
}

ExtraDataWriter::ExtraDataWriter(const ExtraDataWriter& orig) {
}

ExtraDataWriter::~ExtraDataWriter() {
    while (!frames.empty()) {
        if (frames.back() != NULL) {
            delete frames.back();
        }
        frames.pop_back();
    }
}

void ExtraDataWriter::addElement(std::string columnName, double value) {
    if (currentFrame == NULL) {
        currentFrame = new FrameT;
        frames[currentFrameNumber] = currentFrame;
    }
    currentFrame->insert(FrameEntryT(columnName, value));

    if (columnHeadings.empty() || !binary_search(columnHeadings.begin(), columnHeadings.end(), columnName)) {
        columnHeadings.push_back(columnName);
        sort(columnHeadings.begin(), columnHeadings.end());
    }
}

void ExtraDataWriter::addElement(std::pair<std::string,double> element) {
    addElement(element.first, element.second);
}

void ExtraDataWriter::addElements(const std::vector<std::string>& columnNames, const std::vector<double>& values){
    for (int i = 0; i < columnNames.size() && i < values.size(); ++i) {
        addElement(columnNames.at(i), values.at(i));
    }
}

void ExtraDataWriter::addElements(const std::map<std::string,double> &elements) {
    for (map<string, double>::const_iterator it = elements.begin(); it != elements.end(); ++it) {
        addElement(*it);
    }
}

void ExtraDataWriter::addElements(const std::vector<std::pair<std::string,double> >& elements) {
    for (vector<FrameEntryT>::const_iterator it = elements.begin(); it != elements.end(); ++it) {
        addElement(*it);
    }
}

int ExtraDataWriter::getCurrentFrameNumber() {
    return currentFrameNumber;
}

void ExtraDataWriter::goToFrame(int frameNum) {
    if (frames.size() < frameNum + 1) {
        frames.resize(frameNum + 1, NULL);
    }
    currentFrame = frames.at(frameNum);
    currentFrameNumber = frameNum;
}

void ExtraDataWriter::nextFrame() {
    goToFrame(currentFrameNumber + 1);
}

void ExtraDataWriter::writeColumnHeaders(std::ostream &os, const std::vector<std::string> &columnsToWrite) {
    for (vector<string>::const_iterator it = columnsToWrite.begin(); it != columnsToWrite.end(); ++it) {
        os << (*it) << "\t";
    }
    os << endl;
}

void ExtraDataWriter::writeFile(std::ostream& os) {
    writeFile(os, columnHeadings);
}

void ExtraDataWriter::writeFile(std::ostream& os, const std::vector<std::string>& columnsToWrite) {
    writeColumnHeaders(os, columnsToWrite);
    for (vector<FrameT *>::iterator it = frames.begin(); it != frames.end(); ++it) {
        writeRow(os, columnsToWrite, *it);
    }
}

void ExtraDataWriter::writeFile(const char* fname) {
    writeFile(fname, columnHeadings);
}

void ExtraDataWriter::writeFile(const char* fname, const std::vector<std::string>& columnsToWrite) {
    ofstream os(fname);
    writeFile(os, columnsToWrite);
    os.close();
}

void ExtraDataWriter::writeRow(std::ostream& os, const std::vector<std::string>& columnsToWrite, FrameT* frame) {
    map<string, double>::iterator it;
    for (vector<string>::const_iterator cn = columnsToWrite.begin(); cn != columnsToWrite.end(); ++cn) {
        if (frame == NULL || (it = frame->find(*cn)) == frame->end()) {
            os << "NaN\t";
        } else {
            os << setprecision(12) << it->second << "\t";
        }
    }
    os << endl;
}
