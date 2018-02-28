//Python 2.7.11 (v2.7.11:6d1b6a68f775, Dec  5 2015, 20:40:30) [MSC v.1500 64 bit (AMD64)] on win32
#if 0
#include <Python.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

// demo of packaged results

char* indata = "Mouth_open,0.8,0.1,0,0.1,0.8,0.6,0.5,0.8,0.1,0.6,1,1,0.8,0.6,1,1,0.3,0.5,0,0.7 \n \
				   Eyes_left, 0, 0.1, 0.5, 0, 0, 0.9, 0.3, 0, 0.8, 0.1, 0.5, 0.5, 0, 0.9, 0.4, 0.4, 0.7, 0.3, 0.5, 1\n \
				   Eyes_right, 0.5, 0.8, 0.6, 0.5, 0.5, 0.1, 0.7, 0.5, 0, 0.8, 0.3, 0.3, 0.5, 0.1, 0.6, 0.6, 1, 0.7, 0.3, 0.4\n \
				   Eyes_up, 0.3, 0, -0.9, -0.3, -0.3, -0.2, -1, 0.3, 0.5, 0, 0.7, 0.7, 0.4, 0.8, 0.1, 0.1, 0.5, 1, 0.7, 0.6\n \
				   Eyes_down, 0.7, -0.5, -0.1, -0.7, -0.7, -0.1, -0.4, 0.7, 0.3, 0.5, 1, 1, 0.6, 0, 0.8, 0.8, 0.3, 0.4, 1, 0.1\n \
				   Phoneme_mbp, 1, -0.3, -0.2, -1, -1, -0.1, -0.6, 1, 0.7, 0.3, 0.4, 0.4, 0.9, 0.5, 0, 0, 0.7, 0.5, 0.1, 1\n \
				   Phoneme_OO, 0.4, -0.7, -0.1, -0.4, -0.4, -0.8, -0.1, 0.5, 1, 0.7, 0.6, 0.6, 0.1, 0.3, 0.5, 0.5, 1, 0.3, 0.1, 0.4\n \
				   Phoneme_FV, 0.6, -1, -0.1, -0.6, -0.6, 0, -0.8, 0.3, 0.4, 1, 0.1, 0.1, 0.2, 0.7, 0.3, 0.3, 0.4, 0.7, 0.8, 0.8\n \
				   Phoneme_CH, 0.9, -0.4, -0.8, -0.1, -0.9, -0.5, 0, 0.7, 0.5, 0.1, 1, 0.7, 0.1, -1, -0.7, -0.7, -0.6, 1, 0, 0\n \
				   Left_eye_blink, 0.1, -0.6, 0, -0.8, -0.1, -0.3, -0.5, 1, 0.3, 0.1, 0.4, 1, 0.2, -0.4, -1, -1, -0.1, 0.4, 0.5, 0.5\n \
				   Right_eye_blink, 0.2, -0.9, -0.5, 0, -0.2, -0.7, -0.3, 0.4, 0.7, 0.8, 0.8, 0.5, 0.7, -0.8, -0.8, -0.5, -0.7, 0.4, 0.5, 0.5\n \
				   Left_eye_wide_open, 0.8, -0.1, -0.3, -0.5, -0.1, -1, -0.7, 0.6, 1, 0, 0, 0.3, 1, 0, 0, -0.3, -1, 0.6, 0.3, 0.3\n \
				   Right_eye_wide_open, 0, 0.2, 0.7, 0.7, 0.1, 1, 0.7, 0.7, 0.4, 0.5, 0.5, 0.7, 0.4, -0.5, -0.5, -0.1, -0.4, 0.9, 0.7, 0.1\n \
				   Left_brow_up, 0.5, 0.1, 1, 1, 0.2, 0.4, 1, 1, 0.6, 0.3, 0.3, 1, 0.6, -0.3, -0.3, -0.8, -0.6, 0.1, 1, 0.8\n \
				   Right_brow_up, 0.3, 0.1, 0.4, 0.5, 0.7, 0.8, 0.8, 0.5, 0.9, 0.7, 0.1, 0.4, 0.9, -0.7, -0.1, 0, -0.1, 0.2, 0.4, 0\n \
				   Left_brow_down, 0.7, 0.8, 0.8, 0.3, 1, 0, 0, -0.3, -0.1, -1, -0.8, 0.6, 0.1, -1, -0.8, -0.9, -0.3, 0, 0, 0.8\n \
				   Right_brow_down, 1, 0, 0, 0.7, 0.4, 0.5, 0.5, -0.1, -0.2, -0.4, 0, 0.1, 0.2, -0.4, 0, -0.1, -0.7, 0.5, 0.5, 0\n \
				   Mid_brows_up, 0.4, 0.5, 0.5, 1, 0.6, 0.3, 0.3, -0.8, -0.5, 0, -0.9, 0.3, 0, 0, 0.8, 0.8, 0.1, 0.5, 0.3, 0.5\n \
				   Mid_brows_down, 0.6, 0.3, 0.3, 0.4, 0.9, 0.7, 0.1, 0, -0.3, -0.5, -0.1, 0.7, 0.5, 0.5, 0, 0, 0.8, 0.3, 0.7, 0.3\n \
				   Upper_Lip_Up_left, 0.9, 0.7, 0.1, 0.6, 0.1, 1, 0.8, -0.9, -0.7, -0.4, -0.8, 0.1, 0.5, 0.3, 0.5, 1, 0.6, 0, 0.8, 0.3\n \
				   Upper_Lip_Up_right, 0.1, 1, 0.8, 0.1, 0.2, 0.4, 0, -0.1, -1, -0.6, 0, 0.8, 0.3, 0.7, 0.3, 0.4, 0.9, 0.5, 0, 0.7\n \
				   Lower_Lip_Down_left, 0.2, 0.4, 0, 0.3, 0, 0, 0.8, -0.8, -0.4, -0.9, -0.5, 0, 0.7, 1, 0.7, 0.6, 0.1, 0.3, 0.5, 1\n \
				   Lower_Lip_Down_right, 0.8, 0.6, 0.5, 0.7, 0.5, 0.5, 0, 0, -0.6, -0.1, -0.3, 0.5, 1, 0.4, 1, 0.1, 0.2, 0.7, 0.3, 0.4\n \
				   Mouth_Stretch_left, 0, 0.9, 0.3, 0.1, 0.5, 0.3, 0.5, -1, -0.1, -0.2, -0.7, 0.3, 0.4, 0.8, 0.8, 0.5, 0.1, 1, 0.7, 0.6\n \
				   Mouth_Stretch_right, 0.5, 0.1, 0.7, 0.5, 0.8, 0.5, 0.1, -1, -0.7, -0.1, -1, 0.7, 0.6, 0, 0, 0.3, 0.2, 0.4, 1, 0.1\n \
				   Mouth_Right, 0.3, 0.2, 1, 0.3, 0, 0.3, 0.1, -0.4, -1, -0.2, -0.4, 1, 0.1, 0.5, 0.5, 0.7, 0.7, 0.8, 0.8, 0.5\n \
				   Mouth_Left, 0.7, 0.1, 0.4, 0.7, 0.5, 0.7, 0.8, 0.8, 0.5, 0.7, 0.8, 0.8, 0.5, 0.3, 0.3, 0.5, 0, 0.7, 1, 0.7\n \
				   Jaw_left, 1, 0.1, 0.6, 1, 0.3, 1, 0, 0, 0.3, 1, 0, 0, 0.3, 0.7, 0.1, 0.3, 0.5, 1, 0.4, 1\n \
				   Jaw_right, 0.4, 0.8, 0.1, 0.5, 0.7, 0.4, 0.5, 0.5, 0.7, 0.4, 0.5, 0.5, 0.7, 1, 0.8, 0.7, 0.3, 0.4, 0.8, 0.8\n \
				   Mouth_up, 0.6, 0, 0.8, 0.3, 1, 0.6, 0.3, 0.3, 1, 0.6, 0.3, 0.3, 1, 0.4, 0, 1, 0.7, 0.6, 0, 0\n \
				   Mouth_down, 0.9, 0.5, 0, 0.7, 0.4, 0.9, 0.7, 0.1, 0.4, 0.9, 0.7, 0.1, 0.4, 1, 0.8, 0.4, 1, 0.1, 0.5, 0.5\n \
				   Smile_left, 0.1, 0.3, 0.5, 1, 0.6, 0.1, 1, 0.8, 0.6, 0.1, 1, 0.8, 0.6, 0.6, 0.3, 0.8, 0.8, 0.5, 0.3, 0.3\n \
				   Smile_right, 0.2, 0.7, 0.3, 0.4, 0.9, 0.2, 0.4, 0, 0.1, 0.2, 0.4, 0, 0.1, 0.9, 0.7, 0, 0, 0.3, 0.7, 0.1\n \
				   Mouth_Narrow_left, 0.1, 1, 0.7, 0.6, 0.6, 1, 0.1, 0.6, 0.6, 0.1, 1, 0.8, 0.6, 0.1, 1, 0.5, 0.5, 0.7, 1, 0.8\n \
				   Mouth_Narrow_right, 0.2, 0.4, 1, 0.1, 0.9, 0.4, 0.8, 0.1, 0.9, 0.2, 0.4, 0, 0.1, 0.2, 0.4, 0.3, 0.3, 1, 0.4, 0";


class PyStream{

public:

	void loadBsNames(std::string filename){
		
		bsNames.clear();
		std::ifstream in(filename);
		std::string lineStr;
		if (in.is_open()){
			while (std::getline(in, lineStr)) {
				std::cout << lineStr << std::endl;
				bsNames.push_back(lineStr);
			}
		}
		else{
			std::cout << "Failed to open file " << filename << std::endl;
		}
		in.close();
	}

	void loadBsIndex(std::string filename, int nBs){

		bsIndex.clear();
		std::ifstream in(filename);
		int nIdx = 0;
		if (in.is_open()){
			for (int kk = 0; kk < nBs; kk++) {
				in >> nIdx;
				bsIndex.push_back(nIdx);
			}
		}
		else{
			std::cout << "Failed to open file " << filename << std::endl;
		}
		in.close();
	}

	bool initPyStream(){
		Py_Initialize();
		streamResults.clear();
		return 0;
	}
	bool releasePyStream(){
		Py_Finalize();
		return 0;
	}
	bool pushFrameRes(std::vector<double> frameRes){
		streamResults.push_back(frameRes);
		return true;
	}

	void packFrameResults(){
		
		//std::ostringstream oss;
		//for (int ii = 1; ii < bsNames.size(); ii++){
		//	oss << bsNames[ii].c_str();
		//	for (int kk = 0; kk < streamResults.size(); kk++){
		//		oss << "," << streamResults[kk][ii];
		//	}
		//	if (ii < bsNames.size() - 1){
		//		oss << std::string("\n \ ");
		//	}
		//}
		//streamResultsStr = oss.str();
		//return;


		int sortedIdx = 0;
		std::ostringstream oss;
		for (int ii = 0; ii < bsIndex.size(); ii++){
			
			sortedIdx = bsIndex[ii];
			sortedIdx += 1; // simple hack
			//std::cout << "sortedIdx = " << sortedIdx << std::endl;
			oss << bsNames[sortedIdx].c_str();
			for (int kk = 0; kk < streamResults.size(); kk++){
			
				oss << "," << streamResults[kk][sortedIdx];
			}
			if (ii < bsIndex.size()-1){
				oss << std::string("\n \ ");
			}
		}
		streamResultsStr = oss.str();
		return;
	}

	void packFrameHeaders(){

		int sortedIdx = 0;
		std::ostringstream oss;
		for (int ii = 0; ii < bsIndex.size(); ii++){

			sortedIdx = bsIndex[ii];
			sortedIdx += 1; 
			oss << bsNames[sortedIdx].c_str();
			oss << ",";
			oss << std::string("\n \ ");
		}
		streamResultsHeader = oss.str();
		return;
	}

	bool sendPackagedHeaders(){
		facialStreamHead(streamResultsHeader.c_str());
		return true;
	}

	bool sendPackagedResults(){
		facialStreamValue(streamResultsStr.c_str());
		return true;
	}

	bool facialStreamConnect(){

		PyObject *pFSModule, *pFSFacialConnect;
		PyObject *pFSArgs = NULL;
		pFSModule = PyImport_Import(PyString_FromString("mkFacialStream"));
		pFSFacialConnect = PyObject_GetAttrString(pFSModule, "FacialStreamConnect");
		PyObject_CallObject(pFSFacialConnect, pFSArgs);

		return true;
	}

	bool facialStreamHead(const char* indata){

		PyObject *pFSModule, *pFSFacialStream;
		PyObject *pFSArgs;
		pFSModule = PyImport_Import(PyString_FromString("mkFacialStream"));
		pFSFacialStream = PyObject_GetAttrString(pFSModule, "FacialStreamHead");
		pFSArgs = PyTuple_New(1);
		PyTuple_SetItem(pFSArgs, 0, PyString_FromString(indata));
		PyObject_CallObject(pFSFacialStream, pFSArgs);

		return true;
	}

	bool facialStreamValue(const char* indata){

		PyObject *pFSModule, *pFSFacialStream;
		PyObject *pFSArgs;
		pFSModule = PyImport_Import(PyString_FromString("mkFacialStream"));
		pFSFacialStream = PyObject_GetAttrString(pFSModule, "FacialStreamValue");
		pFSArgs = PyTuple_New(1);
		PyTuple_SetItem(pFSArgs, 0, PyString_FromString(indata));
		PyObject_CallObject(pFSFacialStream, pFSArgs);

		return true;
	}

	bool facialStreamClose(){

		PyObject *pFSModule, *pFSFacialStream;
		PyObject *pFSArgs = NULL;
		pFSModule = PyImport_Import(PyString_FromString("mkFacialStream"));
		pFSFacialStream = PyObject_GetAttrString(pFSModule, "FacialStreamClose");
		PyObject_CallObject(pFSFacialStream, pFSArgs);
		
		return true;
	}

public:
	std::string streamResultsStr;
	std::string streamResultsHeader;
	std::vector<std::string> bsNames;
	std::vector<std::vector<double>> streamResults;
	std::vector<int> bsIndex;
};

#endif
