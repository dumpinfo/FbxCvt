#ifndef READ_CONFIG_HEAD
#define READ_CONFIG_HEAD

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <math.h>
#include <string.h>
#include "c24bitmap.h"
#include "c256bitmap.h"
#include <algorithm>
using namespace std;
 
//######################################################################################

#include <fstream>
#include <iostream>

string trim(const string& str)
{
	string::size_type pos = str.find_first_not_of(' ');
	if (pos == string::npos)
	{
		return str;
	}
	string::size_type pos2 = str.find_last_not_of(' ');
	if (pos2 != string::npos)
	{
		return str.substr(pos, pos2 - pos + 1);
	}
	return str.substr(pos);
}

/*string trimstr(string str)
{
	int left(0), right(0),i(0);
 	int length = str.length();

	for (i = 0; i < str.length(); i++)
	{
		if (!isspace(str.c_str()[i]))
		{
			left = i; 	break;
		}
	}
	
	for (i = 0; i < str.length(); i++)
	{
		if (!isspace(str.c_str()[str.length()-1- i]))
		{
			right = str.length() - 1 - i; break;
		}
	}
	return str.substr(left , right);
}*/

bool inline readFile(const char* path, std::string& content)
{
	content.clear();
	std::ifstream fin;
	fin.open(path, std::ios::binary);
	if (!fin) {
		return false;
	}
	const int LINE_LENGTH = 100000;
	char str[LINE_LENGTH];
	while(fin.getline(str,LINE_LENGTH)) {
		content.append(str);
		content.append("\n");
	}
	return true;
}

void inline splitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while(std::string::npos != pos2) {
		string item = s.substr(pos1, pos2-pos1);

		if( item.size()>1&& item[item.length()-1] == '\r')
			item = item.substr(0, item.length()-1);

		if(item.size()>0)
			v.push_back(item);
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length()) {
		v.push_back(s.substr(pos1));
	}
}

string inline getpathfilename(string fname)
{
	int i;
	for(i= (fname.length()-1); i>=0; i--) {
		if(fname[i] == '/')
			return fname.substr( i+1 );
	}
	return fname;
}

void inline readlistfile(const char* filename, std::vector<std::string>&lines)
{
	std::string content;
	bool ret = readFile(filename, content);
	if (ret == false) {
		std::cout << "Read File Failed File:" << filename << std::endl;
	}
	splitString(content, lines, "\n");
	
}
//######################################################################################
//######################################################################################
void inline getfiledata(char * filename, std::vector<char> & data)
{
// data.clear();

	FILE* finput = fopen(filename, "rb");
	if(finput==0)
		return ;
	fseek (finput, 0, SEEK_END);
	long filelen = ftell(finput);
	//printf("%d\n",filelen);

	data.resize( filelen );
	fseek(finput, 0, SEEK_SET);
	fread(&data[0], filelen, 1, finput);
	fclose(finput);

}


void inline writefiledata(char * filename, char* data, int datalen)
{

	FILE* foutput = fopen(filename, "wb+" );
	fwrite(data, datalen, 1, foutput);
	fclose(foutput);

}
/**/

struct FbxNode
{
	string nodeName;
	FbxNode *nodeParent;
	vector<FbxNode *>subNodes;
	int nodeIdx;
};

struct FbxNodeLink
{
	string linkName;
	FbxNode * fromeNode;
	FbxNode * toNode;
	double linkLength;
	double nodePos[3];
	double nodeRot[3];
	int linkIdx;
};


void ReadFileFbxObjList(char*filename, vector<FbxNode>&fbxNodes, vector<FbxNodeLink>&fbxLinks)
{

	vector <string>        nodeNameVec;
	map<string, int>       strMap;
	map<string, FbxNode*>  strNodeMap;

	std::vector<std::string> lines;
	readlistfile(filename, lines);

	int i, j;
	for (i = 0; i < lines.size(); i++)
	{
		std::vector<std::string> strvec; strvec.clear();
		splitString(lines[i], strvec, ",");
		string node_name = trim(strvec[0].c_str());
		if (strMap[node_name] == 0)
		{
			nodeNameVec.push_back(node_name);
			strMap[node_name] = 1;
		}
		node_name = trim(strvec[1].c_str());
		if (strMap[node_name] == 0)
		{
			nodeNameVec.push_back(node_name);
			strMap[node_name] = 1;
		}
	}

	fbxNodes.resize(nodeNameVec.size());
	for (i = 0; i < nodeNameVec.size(); i++)
	{
		fbxNodes[i].nodeName = nodeNameVec[i];
		strNodeMap[fbxNodes[i].nodeName] = &fbxNodes[i];
		fbxNodes[i].nodeParent = NULL;
		fbxNodes[i].nodeIdx = i;
	}

	fbxLinks.resize(lines.size());


	for (i = 0; i < lines.size(); i++)
	{   // 0          1                     2      3     4    5      6      7
		//RootNode , ZombieResident_Mesh , 0.00, 0.00 ,0.00, -90.00, -0.00 ,0.00
		std::vector<std::string> strvec;
		splitString(lines[i], strvec, ",");
		//FbxNodeObjVec[i].nodeName = strvec[0];
		fbxLinks[i].fromeNode = strNodeMap[trim(strvec[0])];
		fbxLinks[i].toNode = strNodeMap[trim(strvec[1])];
		FbxNode* n1 = fbxLinks[i].fromeNode;
		FbxNode* n2 = fbxLinks[i].toNode;
		printf("%s\n", lines[i].c_str());
		for (j = 0; j < 3; j++) fbxLinks[i].nodePos[j] = atof(strvec[2 + j].c_str())/150.0;
		for (j = 0; j < 3; j++) fbxLinks[i].nodeRot[j] = atof(strvec[5 + j].c_str());
		//nodeDict[FbxNodeObjVec[i].nodeName] = &FbxNodeObjVec[i];
		fbxLinks[i].fromeNode->subNodes.push_back(fbxLinks[i].toNode);
		fbxLinks[i].toNode->nodeParent = fbxLinks[i].fromeNode;
		fbxLinks[i].linkLength = sqrt(fbxLinks[i].nodePos[0] * fbxLinks[i].nodePos[0] +
		                                              fbxLinks[i].nodePos[1] * fbxLinks[i].nodePos[1] +
		                                              fbxLinks[i].nodePos[2] * fbxLinks[i].nodePos[2]);
		//fbxLinks[i].SetDirection
		printf("link length%.2lf\n", fbxLinks[i].linkLength);
		fbxLinks[i].linkIdx = i ;
	}

}

void traverseFbsNodes(FbxNode*node, int node_level)
{
	int i;
	for (i = 0; i < node_level; i++)
		printf("   ");
	printf("+");
	printf("%s\n", node->nodeName.c_str());
	for (i = 0; i < node->subNodes.size(); i++)
		traverseFbsNodes(node->subNodes[i], node_level + 1);
}



#endif  