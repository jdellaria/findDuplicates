/*
 * configurationFile.h
 *
 *  Created on: Sep 10, 2009
 *      Author: jdellaria
 */

#ifndef CONFIGURATIONFILE_H_
#define CONFIGURATIONFILE_H_
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <vector>

using namespace std;

typedef enum duplicateFileActions {
	FILE_ACTION_NOTHING = 0,
	FILE_ACTION_DELETE,
	FILE_ACTION_HARD_LINK,
	FILE_ACTION_SYMBOLIC_LINK,
} fileAction;

struct fileDirectory {
    string name;
    fileAction action;
    bool addToDatabase;
};

class configurationFile {
public:
	configurationFile();
	virtual ~configurationFile();

	void help(char *);
	int getConfiguration(char *);

	int port;
	std::string logFileName;
	std::string logValue;
	bool logPrintScreen;
	bool logPrintFile;
	bool logPrintTime;
	std::string serverName;
//	std::string hostFileDirectory;
	vector<fileDirectory> hostFileDirectory;
};

#endif /* CONFIGURATIONFILE_H_ */
