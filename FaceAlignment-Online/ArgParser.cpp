#include "ArgParser.h"

bool ArgParser::Parse(int argc, char *argv[])
{
	std::string config_file;
	if (argc == 1)//argc = 1��ʹ��Ĭ���ļ�
	{
		std::cout << "Using default.conf as configure file...." << std::endl;
		config_file = "default.conf";
	}
	else if (argc == 2) //    argc=2��ʹ��ѡ���ļ�
	{
		config_file = argv[1];
		std::cout << "Using " << config_file << " as configure file...." << std::endl;
	}
	else //    argc=������return false
	{
		std::cout << "Error parameters, Usage: config_file" << std::endl;
		return false;
	}
	return Parse(config_file);
}

bool ArgParser::Parse(const std::string &configFile)
{

	if (configFile.empty()) //  if configFileΪ�գ�return false
	{
		return false;
	}
	else 
//else �ж������keyword�����ͣ�indexFile,numLandmark,landmarkFile,numBs,cameraIndex,enableStream,
//videoInput,bsFolder,bsNames,bsIndex,landmarkFolder,bsCoefficients,bsModeBin������������Ӧ��������ֱ��
//size���˻�������=�˳�
	{
		std::ifstream inputConfigFile(configFile);
		std::string lineStr;

		while (std::getline(inputConfigFile, lineStr)) 
		{
			std::istringstream inputLine(lineStr);
			std::string keyWord;
			inputLine >> keyWord;
			if (keyWord == std::string("indexFile"))
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_indexFile_;
			}
			else if (keyWord == std::string("numLandmark")) 
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_numLandmark_;
			}
			else if (keyWord == std::string("landmarkFile")) 
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_landmarkFile_;
			}
			else if (keyWord == std::string("numBs")) 
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_numBs_;
			}
			else if (keyWord == std::string("cameraIndex")) 
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_cameraIndex_;
			}
			else if (keyWord == std::string("enableStream"))
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_enableStream_;
			}
			else if (keyWord == std::string("videoInput")) 
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_videoInput_;
			}
			else if (keyWord == std::string("bsFolder")) 
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_bsFolder_;
			}
			else if (keyWord == std::string("bsNames")) 
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_bsNames_;
			}
			else if (keyWord == std::string("bsIndex")) 
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_bsIndex_;
			}
			else if (keyWord == std::string("landmarkFolder")) 
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_landmarkFolder_;
			}
			else if (keyWord == std::string("bsCoefficients")) 
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_bsCoefficients_;
			}
			else if (keyWord == std::string("bsModelBin")) 
			{
				inputLine.ignore(lineStr.size(), '=');
				inputLine >> m_bsModelBin_;
			}
		}
	}
	return true;
}

