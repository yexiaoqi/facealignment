#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


class ArgParser 
{	
private:

	std::string m_indexFile_;
	std::string m_videoInput_;
	std::string m_bsFolder_;
	std::string m_landmarkFolder_;
	std::string m_landmarkFile_;
	std::string m_bsCoefficients_;
	std::string m_bsModelBin_;
	std::string m_bsNames_;
	std::string m_bsIndex_;
	int m_numLandmark_;
	int m_numBs_;
	int m_cameraIndex_;
	int m_enableStream_;

public:
	ArgParser() {}
	~ArgParser() {}

	bool Parse(int argc, char* argv[]);
	bool Parse(const std::string &configFile);

	std::string get_indexFile() { return m_indexFile_; }
	std::string get_videoInput(){ return m_videoInput_; }
	std::string get_bsFolder(){ return m_bsFolder_; }
	std::string get_landmarkFolder(){ return m_landmarkFolder_; }
	std::string get_bsCoefficients() { return m_bsCoefficients_; }
	std::string get_bsModelBin() { return m_bsModelBin_; }
	std::string get_landmarkFile(){ return m_landmarkFile_; }
	std::string get_bsNames(){ return m_bsNames_; }
	std::string get_bsIndex(){ return m_bsIndex_; }
	int get_numLandmark(){ return m_numLandmark_; }
	int get_numBs(){ return m_numBs_; }
	int get_cameraIndex(){ return m_cameraIndex_; }
	int get_enableStream(){ return m_enableStream_; }

};
