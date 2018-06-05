//============================================================================
// Name        : deleteDuplicates.cpp
// Author      : Jon Dellaria
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <cstring>
#include <File.h>
#include <Directory.h>
#include <DLog.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include "configurationFile.h"
#include "coopDB.h"

using namespace std;
configurationFile myConfig;
coopDB myDB;
DLog myLog;
fileAction myFileAction = FILE_ACTION_NOTHING;
bool addToDatabase = true; // add file to database
unsigned long numberOfFilesDeleted = 0;
unsigned long numberOfFilesHardLinked = 0;
unsigned long numberOfFilesSoftLinked = 0;
unsigned long numberOfFilesProcessed = 0;
unsigned long directoryCount = 0;
unsigned int hostFileDirectoryIterator;

void doFindDuplicates(const char *text, int x);
void doRemoveEmptyDirectories(const char *text, int x);
int findDuplicates(string originalFileName);
void myCallBack(const char *text, int event);
int removeEmptyDirectory(const char* directory);
bool cmdOptionExists(char** begin, char** end, const string& option);
int executeFileActionOption(string originalFileName, string serverFileName);

bool cmdOptionExists(char** begin, char** end, const string& option)
{
    return std::find(begin, end, option) != end;
}

int getDirectoryCount(const char* startDirectory)
{
	Directory myDirectory;

	directoryCount = 0;
	myDirectory.Recurse(startDirectory, myCallBack);  // void (*callBack)(const char* directoryEntry, int directoryEntryType)
	return(directoryCount);
}

void myCallBack(const char *text, int event)
{
		directoryCount++;
}

int main(int argc, char* argv[])
{
	Directory myDirectory;
	time_t Start_t, End_t;
	int time_task1;
	string message;
	char stringBuffer[50];

	cout << "starting find Duplicates.\n";

// Set the action defaults
    if(cmdOptionExists(argv, argv+argc, "-delete")||cmdOptionExists(argv, argv+argc, "-d"))
    {
    	myFileAction = FILE_ACTION_DELETE;
    }
    if(cmdOptionExists(argv, argv+argc, "-hard")||cmdOptionExists(argv, argv+argc, "-h"))
    {
    	myFileAction = FILE_ACTION_HARD_LINK;
    }
    if(cmdOptionExists(argv, argv+argc, "-symbolic")||cmdOptionExists(argv, argv+argc, "-s"))
    {
    	myFileAction = FILE_ACTION_SYMBOLIC_LINK;
    }
    if(cmdOptionExists(argv, argv+argc, "-noadd")||cmdOptionExists(argv, argv+argc, "-n")) // will not add new files to the database
    {
    	addToDatabase = false;
    }
    else
    {
    	addToDatabase = true;
    }
	myConfig.getConfiguration("config.xml");

	myDB.OpenConnection();
    if(cmdOptionExists(argv, argv+argc, "-reset")||cmdOptionExists(argv, argv+argc, "-r"))
    {
    	message.clear();
		message.append("configure parameters instruct to reset the Database");
		myLog.print((enum logLevel)logLog, message);
        myDB.deleteEverything();
    }
	Start_t = time(NULL);    //record time that task 1 begins

    for (hostFileDirectoryIterator = 0; hostFileDirectoryIterator < myConfig.hostFileDirectory.size(); hostFileDirectoryIterator++)
    {
        if(myConfig.hostFileDirectory[hostFileDirectoryIterator].action == FILE_ACTION_DELETE)
        {
        	message.clear();
    		message.append("configure parameters instruct to delete duplicate files.");
    		myLog.print((enum logLevel)logLog, message);
        }
        if(myConfig.hostFileDirectory[hostFileDirectoryIterator].action == FILE_ACTION_HARD_LINK)
        {
        	message.clear();
    		message.append("configure parameters instruct to replace duplicate files with a hard link.");
    		myLog.print((enum logLevel)logLog, message);
        }
        if(myConfig.hostFileDirectory[hostFileDirectoryIterator].action == FILE_ACTION_SYMBOLIC_LINK)
        {
        	message.clear();
    		message.append("configure parameters instruct to replace duplicate files with a symbolic link.");
    		myLog.print((enum logLevel)logLog, message);
        }
        if(myConfig.hostFileDirectory[hostFileDirectoryIterator].action == FILE_ACTION_NOTHING)
        {
        	message.clear();
    		message.append("configure parameters instruct to NOT perform any action on files. (default)");
    		myLog.print((enum logLevel)logLog, message);
        }
        if(myConfig.hostFileDirectory[hostFileDirectoryIterator].addToDatabase == false) // will not add new files to the database
        {
        	message.clear();
    		message.append("configure parameters instruct to NOT add any files to the database.");
    		myLog.print((enum logLevel)logLog, message);
        }
        else
        {
         	message.clear();
    		message.append("configure parameters instruct to add files to the database. (detault)");
    		myLog.print((enum logLevel)logLog, message);
        }
		if (myDirectory.Recurse(myConfig.hostFileDirectory[hostFileDirectoryIterator].name.c_str(), doFindDuplicates) == 0)
		{
			message = "myDirectory.Recurse error occured";
			myLog.print(logError, message);
		}
		myDirectory.Recurse(myConfig.hostFileDirectory[hostFileDirectoryIterator].name.c_str(), doRemoveEmptyDirectories);
    }
	End_t = time(NULL);    //record time that task 1 ends
	time_task1 = difftime(End_t, Start_t);    //compute elapsed time of task 1
	message.clear();
	message.append("Task took ");
	char intbuffer[50];
	int days,hour,min,sec;

	days = time_task1/86400;
	time_task1 = time_task1%86400;
	hour = time_task1/3600;
	time_task1 = time_task1%3600;
	min = time_task1/60;
	time_task1 = time_task1%60;
	sec = time_task1;
	sprintf(intbuffer,"%d",days);
	message.append(intbuffer);
	message.append(" days ");
	sprintf(intbuffer,"%d",hour);
	message.append(intbuffer);
	message.append(" hours ");
	sprintf(intbuffer,"%d",min);
	message.append(intbuffer);
	message.append(" minutes ");
	sprintf(intbuffer,"%d",sec);
	message.append(intbuffer);
	message.append(" seconds.");
	sprintf(intbuffer,"%d",days);

	myLog.print((enum logLevel)logLog, message);
	message = "Number of Logged Warnings: ";
	sprintf(stringBuffer,"%lu",myLog.numberOfWarnings);
	message.append(stringBuffer);
	myLog.print((enum logLevel)logLog, message);
	message = "Number of Logged Errors: ";
	sprintf(stringBuffer,"%lu",myLog.numberOfErrors);
	message.append(stringBuffer);
	myLog.print((enum logLevel)logLog, message);
	message = "Number of number of files deleted: ";
	sprintf(stringBuffer,"%lu",numberOfFilesDeleted);
	message.append(stringBuffer);
	myLog.print((enum logLevel)logLog, message);
	message = "Number of number of files hard linked: ";
	sprintf(stringBuffer,"%lu",numberOfFilesHardLinked);
	message.append(stringBuffer);
	myLog.print((enum logLevel)logLog, message);
	message = "Number of number Of files soft linked: ";
	sprintf(stringBuffer,"%lu",numberOfFilesSoftLinked);
	message.append(stringBuffer);
	myLog.print((enum logLevel)logLog, message);
	message = "Number of number Of files processed: ";
	sprintf(stringBuffer,"%lu",numberOfFilesProcessed);
	message.append(stringBuffer);
	myLog.print((enum logLevel)logLog, message);
	myDB.CloseConnection();
}

void doFindDuplicates(const char *text, int x)
{
	string message;
	char stringBuffer[50];

	if(DIRECTORYENTRYTYPE_REG == x) // if it is a file and a duplicate... then remove
	{
		message.clear();
		message.append("doFindDuplicates: Processing file:");
		message.append(text);
		myLog.print(logInformation, message);
		findDuplicates(text);
	}
}

void doRemoveEmptyDirectories(const char *text, int x)
{
	if(DIRECTORYENTRYTYPE_DIR == x) // if a Directory, remove if it is empty
	{
		removeEmptyDirectory(text);
	}
}

int findDuplicates(string originalFileName)
{
	File myFile;
	char originalFileHash[36];
	string destinationFileName;
	string destinationVersionFileName;
	string message;

	myFile.getMD5Hash((char* const)originalFileName.c_str(),originalFileHash);
	numberOfFilesProcessed++;
	if (myDB.doesHashExistInDataBase(originalFileHash))
	{
		if (myDB.getServerFile(originalFileHash) >= 0)
		{
			executeFileActionOption(originalFileName,myDB.serverDirectoryEntry);
		}
	}
	else
	{
		if (myConfig.hostFileDirectory[hostFileDirectoryIterator].addToDatabase == true)
		{
			message.clear();
			message.append("findDuplicates: ");
			message.append(" Adding File:");
			message.append(originalFileName);
			myLog.print(logInformation, message);
			myDB.setServerDirectoryEntry(originalFileName.c_str());
			myDB.setMD5Hash(originalFileHash);
			myDB.addHashFile();
		}
	}
	return 0;
}

int removeEmptyDirectory(const char* directory)
{
	int myDirectoryCount;
	Directory myDirectory;
	string message;

	myDirectoryCount = getDirectoryCount(directory);
	if (myDirectoryCount == 0)
	{
		myDirectory.Remove(directory);
		if (myDirectory.Remove(directory) == 0) // if delete directory succeeds
		{
			message.clear();
			message.append("removeEmptyDirectory: directory removed:");
			message.append(directory);
			myLog.print(logWarning, message);
		}
		else
		{
			message.clear();
			message.append("Error removeEmptyDirectory:"); // report error if directory is not deleted
			message.append(myDirectory.errorMessageBuffer);
			message.append(" directory:");
			message.append(directory);
			myLog.print(logError, message);
		}
	}
}
#ifdef JON
bool isSpecialFile(string originalFileName)
{
	if (originalFileName.compare())
	{
		return (true);
	}
	return(false);
}
#endif

int executeFileActionOption(string originalFileName, string serverFileName)
{
	File myFile;
	string message;

	if(originalFileName.compare(serverFileName) == 0)
	{
		message.clear();
		message.append("executeFileActionOption: File is original file. File action not performed:" + originalFileName);
		myLog.print(logWarning, message);
		return(0);
	}

	if(myConfig.hostFileDirectory[hostFileDirectoryIterator].action ==  FILE_ACTION_DELETE)
	{
		if (myFile.remove(originalFileName.c_str()) == 0) // if delete file succeeds
		{
			message = "Deleting file:"; // log the event
			message.append(originalFileName);
			myLog.print(logWarning, message);
			numberOfFilesDeleted++;
		}
		else
		{
			message = "Error deleting file:"; // report error if file is not deleted
			message.append(myFile.errorMessageBuffer);
			message.append(": ");
			message.append(originalFileName);
			myLog.print(logError, message);
		}
	}
	if(myConfig.hostFileDirectory[hostFileDirectoryIterator].action ==  FILE_ACTION_HARD_LINK)
	{
		message.clear();
		message.append("Replacing File:");
		message.append(originalFileName);
		message.append(": with a hard link");
		myLog.print(logWarning, message);
		if (myFile.remove(originalFileName.c_str()) == 0) // if delete file succeeds
		{
			message = "Deleting file:"; // log the event
			message.append(originalFileName);
			myLog.print(logWarning, message);
		}
		else
		{
			message = "Error deleting file:"; // report error if file is not deleted
			message.append(myFile.errorMessageBuffer);
			message.append(": ");
			message.append(originalFileName);
			myLog.print(logError, message);
		}
		myFile.createHardLink(serverFileName.c_str(),originalFileName.c_str());
		numberOfFilesHardLinked++;
	}
	if(myConfig.hostFileDirectory[hostFileDirectoryIterator].action ==  FILE_ACTION_SYMBOLIC_LINK)
	{
		message.clear();
		message.append("Replacing File:");
		message.append(originalFileName);
		message.append(": with a symbolic link");

		myLog.print(logWarning, message);
		if (myFile.remove(originalFileName.c_str()) == 0) // if delete file succeeds
		{
			message = "Deleting file:"; // log the event
			message.append(originalFileName);
			myLog.print(logWarning, message);
		}
		else
		{
			message = "Error deleting file:"; // report error if file is not deleted
			message.append(myFile.errorMessageBuffer);
			message.append(": ");
			message.append(originalFileName);
			myLog.print(logError, message);
		}
		myFile.createSoftLink(serverFileName.c_str(),originalFileName.c_str());
		numberOfFilesSoftLinked++;
	}
	return(1);
}
