/* 
 * File:   main.cpp
 * Author: Marc
 *
 * Created on September 12, 2012, 3:29 PM
 */

#include <cstdlib>
#include "MultiStackReader.h"
using namespace std;

/*
 * 
 */

const string test_fname = "{E:\\from bruno- multiple mmfs\\20120616_103100\\20120616_103100\\20120616_103100@FCF_attp2_1500062@UAS_TNT_2_0003@t12@n#n#n#n@100@221330792.mmf, E:\\from bruno- multiple mmfs\\20120616_103100\\20120616_103100\\20120616_103100@FCF_attp2_1500062@UAS_TNT_2_0003@t12@n#n#n#n@100@221330793.mmf}";
const string test_mdat_fname = "E:\\from bruno- multiple mmfs\\20120616_103100\\20120616_103100\\20120616_103100@FCF_attp2_1500062@UAS_TNT_2_0003@t12@n#n#n#n@100.mdat";
const string test_decimation_fname = "E:\\from bruno- multiple mmfs\\20120616_103100\\20120616_103100\\20120616_103100@FCF_attp2_1500062@UAS_TNT_2_0003@t12@n#n#n#n@100_decimated.mmf";
void testFileNameParsing (string fname);

void testMultiStackReader ();

int main(int argc, char** argv) {
    
    testFileNameParsing(test_fname);
    testMultiStackReader();
    return 0;
}

void testFileNameParsing (string fname) {
   vector<string> fn = MultiStackReader::parseFileNameInput(fname.c_str());
   if (fn.empty()) {
       cout << "could not parse " << fname << endl;
   } else {
       cout << "fn length = " << fn.size() << endl;
       for (vector<string>::iterator it = fn.begin(); it != fn.end(); ++it) {
           cout << *it << endl << endl;
       }
   }
}

void testMultiStackReader () {
    MultiStackReader sr(test_fname.c_str());
   // sr.createSupplementalDataFile(test_mdat_fname.c_str());
    sr.decimateStack(test_decimation_fname.c_str(), 10, 5, 5, 5);
    
}