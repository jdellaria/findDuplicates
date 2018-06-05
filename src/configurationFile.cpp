/*
 * configurationFile.cpp
 *
 *  Created on: Sep 10, 2009
 *      Author: jdellaria
 */

#include <sys/stat.h>
#include <sys/dir.h>
#include <errno.h>
#include <dirent.h>
//#include <string>
//#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>
#include "configurationFile.h"
#include <tinyxml.h>
#include <DLog.h>
//#include <ctime>

using namespace std;
//#pragma hdrstop

extern DLog myLog;
extern fileAction myFileAction;
extern bool addToDatabase; // add file to database

configurationFile::configurationFile()
{
	port = 0;
	logFileName.clear();
	logPrintScreen = true;
	logPrintFile = true;
	logPrintTime = true;
	serverName.clear();
	hostFileDirectory.clear();
}

configurationFile::~configurationFile()
{
	// TODO Auto-generated destructor stub
}

// Read a file into a vector

void configurationFile::help(char *program) {
	std::cout << program;
	std::cout << ": Need a filename for a parameter.\n";
}

int configurationFile::getConfiguration(char *Name)
{
	string message;
	TiXmlDocument doc(Name);
	bool loadOkay = doc.LoadFile();
	string myString;
	fileDirectory tempFileDirectory;
	const char * attributeType;

	const char* exampleXMLFile =
		"Config file should look like this:\n"
		"<?xml version=\"1.0\"  standalone='no' >\n"
		"<!-- Configuration Parmeters -->\n"
		"<port>1235</port>\n"
		"<logfilename>/home/jdellaria/Desktop/log.txt</logfilename>\n"
		"<logvalue>logDebug/logInformation/logWarning/logError</logvalue>\n"
		"<logprintscreen>true/false</logprintscreen>\n"
		"<logprintfile>true/false</logprintfile>\n"
		"<logprinttime>true/false</logprinttime>\n"
		"<servername>DLirius Archive Server</servername>\n"
		"<hostdirectory>/home/jdellaria/Desktop/myNewFile</hostdirectory>\n";
	if ( !loadOkay )
	{
		message.clear();
		message.append("Could not load configuration file ");
		message.append(Name);
		message.append(" Error='");
		message.append(doc.ErrorDesc());
		message.append("'.\n");
		message.append(exampleXMLFile );
		message.append("\nExiting.");
		myLog.print(logError, message);
		exit( 1 );
	}
	TiXmlNode* node = 0;
	TiXmlElement* portElement = 0;
	// --------------------------------------------------------
	// An example of changing existing attributes, and removing
	// an element from the document.
	// --------------------------------------------------------

	// Get the "ToDo" element.
	// It is a child of the document, and can be selected by name.

	node = doc.FirstChild( "logfilename" );
	if (!node)
	{
		message.clear();
		message.append("logfilename child not found!! XML file is invalid.\n");
		message.append(exampleXMLFile);
		myLog.print(logError, message);
		exit(0);
	}
	portElement = node->ToElement();
	if (!portElement)
	{
		message.clear();
		message.append("logfilename element not found!! XML file is invalid.\n");
		message.append(exampleXMLFile);
		myLog.print(logError, message);
		exit(0);
	}
	message.clear();
	message.append("logfilename element GetText:" );
	message.append(portElement->GetText() );
	logFileName.append(portElement->GetText());
	myLog.logFileName = logFileName;
	myLog.print(logInformation, message);

	node = doc.FirstChild( "logvalue" );
	if (!node)
	{
		message.clear();
		message.append("logvalue child not found!! XML file is invalid.\n");
		message.append(exampleXMLFile);
		myLog.print(logError, message);
		exit(0);
	}
	portElement = node->ToElement();
	if (!portElement)
	{
		message.clear();
		message.append("logvalue element not found!! XML file is invalid.\n");
		message.append(exampleXMLFile);
		myLog.print(logError, message);
		exit(0);
	}
	message.clear();
	message.append("logvalue element GetText:");
	message.append(portElement->GetText() );
	myLog.print(logInformation, message);
	logValue.append(portElement->GetText());
	if (logValue.find("logDebug")!=string::npos)
	{
		myLog.logValue = logDebug;
		message = "myLog.logValue = logDebug";
		myLog.print(logInformation, message);
	}
	if (logValue.find("logInformation")!=string::npos)
	{
		myLog.logValue = logInformation;
		message = "myLog.logValue = logInformation";

		myLog.print(logInformation, message);
	}
	if (logValue.find("logWarning")!=string::npos)
	{
		myLog.logValue = logWarning;
		message = "myLog.logValue = logWarning";

		myLog.print(logInformation, message);
	}
	if (logValue.find("logError")!=string::npos)
	{
		myLog.logValue = logError;
		message = "myLog.logValue = logError";

		myLog.print(logInformation, message);
	}

	node = doc.FirstChild( "logprintscreen" );
	if (!node)
	{
		message.clear();
		message.append("logprintscreen child not found!! XML file is invalid.\n");
		message.append(exampleXMLFile);
		myLog.print(logError, message);
		exit(0);
	}
	portElement = node->ToElement();
	if (!portElement)
	{
		message.clear();
		message.append("logprintscreen element not found!! XML file is invalid.\n");
		message.append(exampleXMLFile);
		myLog.print(logError, message);
		exit(0);
	}
	message.clear();
	message.append("logprintscreen element GetText:");
	message.append(portElement->GetText() );
	myLog.print(logInformation, message);
	myString = portElement->GetText();
	if (myString.find("false")!=string::npos)
	{
		logPrintScreen = false;
		message = "logPrintScreen = false";
		myLog.print(logInformation, message);
	}

	if (myString.find("true")!=string::npos)
	{
		logPrintScreen = true;
		message = "logPrintScreen = true";

		myLog.print(logInformation, message);
	}
	myLog.printScreen = logPrintScreen;

	node = doc.FirstChild( "logprintfile" );
	if (!node)
	{
		message.clear();
		message.append("logprintfile child not found!! XML file is invalid.\n");
		message.append(exampleXMLFile);
		myLog.print(logError, message);
		exit(0);
	}
	portElement = node->ToElement();
	if (!portElement)
	{
		message.clear();
		message.append("logprintfile element not found!! XML file is invalid.\n");
		message.append(exampleXMLFile);
		myLog.print(logError, message);
		exit(0);
	}
	message.clear();
	message.append("logprintfile element GetText:");
	message.append(portElement->GetText() );
	myLog.print(logInformation, message);
	myString = portElement->GetText();
	if (myString.find("false")!=string::npos)
	{
		logPrintFile = false;
		message = "logPrintFile = false";
		myLog.print(logInformation, message);
	}

	if (myString.find("true")!=string::npos)
	{
		logPrintFile = true;
		message =  "logPrintFile = true";
		myLog.print(logInformation,message);
	}
	myLog.printFile = logPrintFile;

	node = doc.FirstChild( "logprinttime" );
	if (!node)
	{
		message.clear();
		message.append("logprinttime child not found!! XML file is invalid.\n");
		message.append(exampleXMLFile);
		myLog.print(logError, message);
		exit(0);
	}
	portElement = node->ToElement();
	if (!portElement)
	{
		message.clear();
		message.append("logprinttime element not found!! XML file is invalid.\n");
		message.append(exampleXMLFile);
		myLog.print(logError, message);
		exit(0);
	}
	message.clear();
	message.append("logprinttime element GetText:");
	message.append(portElement->GetText() );
	myLog.print(logInformation, message);
	myString = portElement->GetText();
	if (myString.find("false")!=string::npos)
	{
		logPrintTime = false;
		message = "logPrintTime = false";
		myLog.print(logInformation, message);
	}
	if (myString.find("true")!=string::npos)
	{
		logPrintTime = true;
		message = "logPrintTime = true";
		myLog.print(logInformation, message);
	}
	myLog.printTime = logPrintTime;

	node = doc.FirstChild( "hostdirectory" );
	if (!node)
	{
		message.clear();
		message.append("hostdirectory child not found!! XML file is invalid.\n");
		message.append(exampleXMLFile);
		myLog.print(logError, message);
		exit(0);
	}
	while (node != 0 )
	{
		portElement = node->ToElement();
		if (!portElement)
		{
			message.clear();
			message.append("hostdirectory element not found!! XML file is invalid.\n");
			message.append(exampleXMLFile);
			myLog.print(logError, message);
			exit(0);
		}
		message.clear();
		message.append("hostdirectory element GetText:");
		message.append( portElement->GetText() );
		myLog.print(logInformation, message);

		tempFileDirectory.name.clear();
		tempFileDirectory.name.append(portElement->GetText());
		tempFileDirectory.action = myFileAction; // set the default actions
		tempFileDirectory.addToDatabase = addToDatabase; // set the default actions

		attributeType = portElement->Attribute("fileaction");
		if (attributeType != 0)
		{
			if(strcmp( attributeType,"delete")==0)
			{
				tempFileDirectory.action = FILE_ACTION_DELETE;
				message = "fileAction = FILE_ACTION_DELETE";
				myLog.print(logInformation, message);
			}
			if(strcmp( attributeType,"hardlink")==0)
			{
				tempFileDirectory.action = FILE_ACTION_HARD_LINK;
				message = "fileAction = FILE_ACTION_HARD_LINK";
				myLog.print(logInformation, message);
			}
			if(strcmp( attributeType,"softlink")==0)
			{
				tempFileDirectory.action = FILE_ACTION_SYMBOLIC_LINK;
				message = "fileAction = FILE_ACTION_SYMBOLIC_LINK";
				myLog.print(logInformation, message);
			}
			if(strcmp( attributeType,"nothing")==0)
			{
				tempFileDirectory.action = FILE_ACTION_NOTHING;
				message = "fileAction = FILE_ACTION_NOTHING";
				myLog.print(logInformation, message);
			}
		}
		attributeType = portElement->Attribute("addtodatabase");
		if (attributeType != 0)
		{
			if(strcmp( attributeType,"true")==0)
			{
				tempFileDirectory.addToDatabase = true;
				message = "addToDatabase = true";
				myLog.print(logInformation, message);
			}
			if(strcmp( attributeType,"false")==0)
			{
				tempFileDirectory.addToDatabase = false;
				message = "addToDatabase = false";
				myLog.print(logInformation, message);
			}
		}
		hostFileDirectory.push_back(tempFileDirectory);
		node = node->NextSibling();
	}
}

