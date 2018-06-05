/*
 * coopDB.cpp
 *
 *  Created on: Oct 3, 2010
 *      Author: jdellaria
 */

#include "coopDB.h"
#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>
#include <DLog.h>

extern DLog myLog;

int asciiToUtf8(char *string, size_t maxLen);

coopDB::coopDB() {
	// TODO Auto-generated constructor stub
}

coopDB::~coopDB() {
	// TODO Auto-generated destructor stub
}

//Music Database Connection
MYSQL coopDB::OpenConnection()
{
	string message;

	int	err = 0;
	/* now to connect to the database */

	if(mysql_init(&dbaseConnection) == NULL)
	{
		err = 1;
	}
	else
	{
		if(mysql_real_connect(&dbaseConnection,"localhost","root","dlirius","coopData",0,NULL,0) == NULL)
			err = 1;
	}
	if(err)
	{
		message.append("Failed to connect to database: Error: ");
		message.append(mysql_error(&dbaseConnection));
		myLog.print(logError,message);
		return(dbaseConnection);
	}
	return (dbaseConnection);
}

void coopDB::CloseConnection()
{
	mysql_close(&dbaseConnection);
}

void coopDB::setMD5Hash(string s)
{
	md5Hash = mysqlRealEscapeString(s);
}

void coopDB::setClientDirectoryEntry(string s)
{
	clientDirectoryEntry = mysqlRealEscapeString(s);
}

void coopDB::setServerDirectoryEntry(string s)
{
	serverDirectoryEntry = mysqlRealEscapeString(s);
}

void coopDB::setDirectoryEntryType(string s)
{
	directoryEntryType = mysqlRealEscapeString(s);
}

void coopDB::setIpaddress(string s)
{
	ipaddress = mysqlRealEscapeString(s);
}

void coopDB::setMachineName(string s)
{
	MachineName = mysqlRealEscapeString(s);
}

void coopDB::setClientStatus(string s)
{
	clientStatus = mysqlRealEscapeString(s);
}

void coopDB::setServerStatus(string s)
{
	serverStatus = mysqlRealEscapeString(s);
}

void coopDB::setClientError(string s)
{
	clientError = mysqlRealEscapeString(s);
}

void coopDB::setServerConnection(int connection)
{
	serverConnection = connection;
}

void coopDB::setFileSize(string s)
{
	string message;
	setFileSize(s.c_str());
    if (sscanf(s.c_str(), "%Lu", &fileSize) < 1)
    {
    	message.append("coopDB::setFileSize attribute error: string:");
    	message.append(s.c_str());
    	message.append(" fileSize:");
    	char intbuffer[500];
    	sprintf(intbuffer,"%Lu",fileSize);
    	message.append(intbuffer);
		myLog.print(logError,message);
    }
}

void coopDB::setFileSize(const char *s)
{
	string message;

    if (sscanf(s, "%Lu", &fileSize) < 1)
    {
    	message.append("coopDB::setFileSize attribute error: string:");
    	message.append(s);
    	message.append(" fileSize:");
    	char intbuffer[500];
    	sprintf(intbuffer,"%Lu",fileSize);
    	message.append(intbuffer);
		myLog.print(logError,message);
    	exit(0);
    }
}

void coopDB::setFileSize(uint64_t s)
{
	fileSize = s;
}

void coopDB::setFileOffset(string s)
{
	setFileOffset(s.c_str());
}

void coopDB::setFileOffset(uint64_t s)
{
	fileOffset = s;
}

void coopDB::setFileOffset(const char *s)
{
	string message;
    if (sscanf(s, "%Lu", &fileOffset) < 1)
    {
    	message.append("coopDB::setFileOffset attribute error: string:");
    	message.append(s);
    	message.append(" fileOffset:");
    	char intbuffer[50];
    	sprintf(intbuffer,"%Lu",fileOffset);
    	message.append(intbuffer);
		myLog.print(logError,message);
    	exit(0);
    }
}
string coopDB::mysqlRealEscapeString(string s)
{
	char tname[20000];
	string returnValue;

	mysql_real_escape_string(&dbaseConnection, tname, s.c_str(), s.length());
	returnValue.append(tname, strlen(tname));
	return(returnValue);
}

long coopDB::addClientDirectoryEntry()	//artist must be set before calling function
{
	string SQLStmt;
	string myFileSize;
	char buffer [500];
	string message;

	sprintf (buffer, "%llu",fileSize);
	myFileSize.append(buffer);
	SQLStmt.append("INSERT into coopData.fileInformation (md5Hash, direntry, fileSize, type, ipaddress, MachineName, status) values ('");
	SQLStmt.append(md5Hash + "','" + clientDirectoryEntry + "','" + myFileSize + "','" + directoryEntryType + "','" + ipaddress + "','"+MachineName+"','"+ clientStatus +"');");
	message.clear();
	message.append("coopDB::addClientDirectoryEntry SQLStmt:");
	message.append(SQLStmt);
	myLog.print(logDebug,message);
	if (mysql_query(&dbaseConnection, SQLStmt.c_str()))
	{
		message.append("coopDB::addClientDirectoryEntry error: ");
		message.append(mysql_error(&dbaseConnection));
		myLog.print(logError,message);
		return(0);
	}
	return(1);
}

long coopDB::addHashFile()	//artist must be set before calling function
{
	string SQLStmt;
	string myFileSize;
	char buffer [500];
	string message;

	sprintf (buffer, "%llu",fileSize);
	myFileSize.append(buffer);
	SQLStmt.append("INSERT into coopData.hashFile (md5Hash,direntry,status,fileSize,connection) values ('");
	SQLStmt.append(md5Hash + "','" + serverDirectoryEntry + "','" + serverStatus + "','" + myFileSize + "','");
	SQLStmt.append(serverConnection +"');");
	message.clear();
	message.append("coopDB::addHashFile SQLStmt:");
	message.append(SQLStmt);
	myLog.print(logDebug,message);
	if (mysql_query(&dbaseConnection, SQLStmt.c_str()))
	{
		message.append("coopDB::addHashFile error: ");
		message.append(mysql_error(&dbaseConnection));
		myLog.print(logError,message);
		return(0);
	}
	return(1);
}

long coopDB::updateHashFileStatus()	//artist must be set before calling function
{
	string SQLStmt;
	string myFileOffset;
	char buffer [500];
	string message;

	sprintf (buffer, "%llu",fileOffset);
	myFileOffset.clear();
	myFileOffset.append(buffer);
	SQLStmt.append("update coopData.hashFile set status='" + serverStatus + "', fileOffset='" + myFileOffset + "' where md5Hash='" + md5Hash + "'");
	message.clear();
	message.append("coopDB::updateHashFileStatus SQLStmt: ");
	message.append(SQLStmt);
	myLog.print(logDebug,message);
	if (mysql_query(&dbaseConnection, SQLStmt.c_str()))
	{
		message.clear();
		message.append("coopDB::updateHashFileStatus SQLStmt: " + SQLStmt + " " + mysql_error(&dbaseConnection));
		myLog.print(logError,message);
		return(0);
	}
	return(1);
}

long coopDB::updateHashFileFileOffset()
{
	string SQLStmt;
	string stringFileOffset;
	string myFileOffset;
	char buffer [500];
	string message;

	sprintf (buffer, "%llu",fileOffset);
	myFileOffset.clear();
	myFileOffset.append(buffer);
	SQLStmt.append("update coopData.hashFile set fileOffset='" + myFileOffset + "' where md5Hash='" + md5Hash + "'");
	message.clear();
	message.append("coopDB::updateHashFileFileOffset SQLStmt: ");
	message.append(SQLStmt);
	myLog.print(logDebug,message);
	if (mysql_query(&dbaseConnection, SQLStmt.c_str()))
	{
		message.append("coopDB::updateHashFileFileOffset error: ");
		message.append(mysql_error(&dbaseConnection));
		myLog.print(logError,message);
		return(0);
	}
	return(1);
}

long coopDB::updateHashFileDirectoryEntry()
{
	string SQLStmt;
	string message;

	SQLStmt.append("update coopData.hashFile set direntry='" + serverDirectoryEntry + "' where md5Hash='" + md5Hash + "'");
	message.clear();
	message.append("coopDB::updateHashFileFileOffset SQLStmt: ");
	message.append(SQLStmt);
	myLog.print(logDebug,message);
	if (mysql_query(&dbaseConnection, SQLStmt.c_str()))
	{
		message.clear();
		message.append("coopDB::updateHashFileDirectoryEntry error: ");
		message.append(mysql_error(&dbaseConnection));
		myLog.print(logError,message);
		return(0);
	}
	return(1);
}
long coopDB::updateHashFileStatusWithError()	//artist must be set before calling function
{
	string SQLStmt;
	string message;

	SQLStmt.append("update coopData.hashFile set status='" + serverStatus + "',errorMessage='"+clientError+"' where md5Hash='" + md5Hash + "'");
	message.clear();
	message.append("coopDB::updateHashFileStatusWithError SQLStmt: ");
	message.append(SQLStmt);
	myLog.print(logDebug,message);
	if (mysql_query(&dbaseConnection, SQLStmt.c_str()))
	{
		message.clear();
		message.append("coopDB::updateHashFileStatusWithError error: ");
		message.append(mysql_error(&dbaseConnection));
		myLog.print(logError,message);
		return(0);
	}
	return(1);
}

int coopDB::doesHashExistInDataBase(const char* hash)
{
	char queryStmt[2000];
	MYSQL_ROW row;
	MYSQL_RES *duplicateHashResult;
	int returnValue = 0;
	string message;

	sprintf(queryStmt,"SELECT * FROM coopData.hashFile where md5Hash = '%s'", mysqlRealEscapeString(hash).c_str());
	message.clear();
	message.append("coopDB::doesHashExistInDataBase SQLStmt: ");
	message.append(queryStmt);
	myLog.print(logDebug,message);
	if(mysql_real_query(&dbaseConnection,queryStmt,strlen(queryStmt)))
	{
		message.append("coopDB::doesHashExistInDataBase error: ");
		message.append(mysql_error(&dbaseConnection));
		myLog.print(logError,message);
		message.append("coopDB::doesHashExistInDataBase SQL Query: ");
		message.append(queryStmt);
		myLog.print(logError,message);
		exit(1);
	}
	duplicateHashResult = mysql_store_result(&dbaseConnection);
	if ((row = mysql_fetch_row(duplicateHashResult)) != NULL)
	{
		returnValue = 1;
	}
	/* Release memory used to store results and close connection */
	mysql_free_result(duplicateHashResult);
    return(returnValue);
}

string coopDB::getHashFileStatus(const char* hash)
{

	string SQLStmt;
	string returnValue;
	string message;
	MYSQL_ROW row;
	MYSQL_RES *duplicateHashResult;

	SQLStmt.append("select status from coopData.hashFile where md5Hash='" + mysqlRealEscapeString(hash)+ "'");
	message.clear();
	message.append("coopDB::getHashFileStatus SQLStmt: ");
	message.append(SQLStmt);
	myLog.print(logDebug,message);
	if(mysql_real_query(&dbaseConnection,SQLStmt.c_str(),SQLStmt.length()))
	{
		message = "SQL error";
		myLog.print(logError,message);
		exit(1);
	}

	duplicateHashResult = mysql_store_result(&dbaseConnection);
	if ((row = mysql_fetch_row(duplicateHashResult)) != NULL)
	{
		returnValue.clear();
		returnValue.append(row[0]);
	}
	else
	{
		returnValue.clear();
		returnValue.append("");
	}
	mysql_free_result(duplicateHashResult); // this should be run after every mysql_store_result to free up memory
	return (returnValue);
}


uint64_t coopDB::getFileSize(const char* hash)
{

	string SQLStmt;
	uint64_t returnValue;
	MYSQL_ROW row;
	MYSQL_RES *duplicateHashResult;
	string message;

	SQLStmt.append("select fileSize from coopData.hashFile where md5Hash='" + mysqlRealEscapeString(hash)+ "'");
	message.clear();
	message.append("coopDB::getFileSize SQL Query: ");
	message.append(SQLStmt);
	myLog.print(logDebug,message);
	if(mysql_real_query(&dbaseConnection,SQLStmt.c_str(),SQLStmt.length()))
	{
		message = "coopDB::getFileSize SQL error";
		myLog.print(logError,message);
		exit(1);
	}

	duplicateHashResult = mysql_store_result(&dbaseConnection);
	if ((row = mysql_fetch_row(duplicateHashResult)) != NULL)
	{

		message.append("coopDB::getFileSize row[0]:");
		message.append(row[0]);
		myLog.print(logError,message);
	    if (sscanf(row[0], "%Lu", &fileSize) < 1)
	    {
	    	message = "coopDB::getFileSize attribute error";
			myLog.print(logError,message);
	    	returnValue = -1;
	    }
	    returnValue = fileSize;
	}
	else
	{
	    fileSize = -1;
	    returnValue = fileSize;
	}
	mysql_free_result(duplicateHashResult); // this should be run after every mysql_store_result to free up memory
	return (returnValue);
}

off64_t coopDB::getActiveFile(int connection)
{

	string SQLStmt;
	char myInt[1000];
	string serverDirectoryEntry;
	MYSQL_ROW row;
	MYSQL_RES *duplicateHashResult;
	off64_t returnValue;
	string message;

	sprintf(myInt,"%d",connection);
	SQLStmt.clear();
	SQLStmt.append("select direntry, fileOffset, md5Hash, status, fileSize from coopData.hashFile where ((connection = ");
	SQLStmt.append( myInt);
	SQLStmt.append( " ) and (status = 'active') )");
	message.append("coopDB::getActiveFile SQLStmt:");
	message.append(SQLStmt);
	myLog.print(logDebug,message);
	if(mysql_real_query(&dbaseConnection,SQLStmt.c_str(),SQLStmt.length()))
	{
		message = "coopDB::getActiveFile SQL error";
		myLog.print(logError,message);
		exit(1);
	}

	duplicateHashResult = mysql_store_result(&dbaseConnection);
	if ((row = mysql_fetch_row(duplicateHashResult)) != NULL) // if there is a current active file then do this.
	{
		setServerDirectoryEntry(row[0]);
		message.clear();
		message.append("coopDB::getActiveFile serverDirectoryEntry:");
		message.append(serverDirectoryEntry);
		message.append("fileOffset: ");
		message.append(row[1]);
		myLog.print(logDebug,message);
	    if (sscanf(row[1], "%Lu", &fileOffset) < 1)
	    {
	    	message = "coopDB::getActiveFile attribute error";
	    	myLog.print( logError,message);
	    	return(-1);
	    }
	    setMD5Hash(row[2]);
	    setServerStatus(row[3]);
	    setFileSize(row[4]);
		returnValue = fileOffset;
	}
	else
	{
		returnValue = getCopyFile(connection);
		if (returnValue != -1)
		{
			setServerStatus("active");
			updateHashFileStatus();
		}
	}
//	now get client information
	if (returnValue != -1)
	{
		if (getClientFileName(md5Hash) == -1)
		{
			message = "coopDB::getActiveFile getClientFileName error";
			myLog.print(logError,message);
			exit(1);
		}
	}
	mysql_free_result(duplicateHashResult); // this should be run after every mysql_store_result to free up memory
	return (returnValue);
}

int coopDB::getClientFileName(string hash)
{
	return (getClientFileName(hash.c_str()));
}

int coopDB::getClientFileName(const char* hash)
{
	string SQLStmt;
	int returnValue;
	MYSQL_ROW row;
	MYSQL_RES *duplicateHashResult;
	string message;

	SQLStmt.append("select direntry from coopData.fileInformation where md5Hash='" + mysqlRealEscapeString(hash)+ "'");
	message.clear();
	message.append("coopDB::getClientFileName SQLStmt:");
	message.append(SQLStmt);
	myLog.print(logDebug,message);
	if(mysql_real_query(&dbaseConnection,SQLStmt.c_str(),SQLStmt.length()))
	{
		message = "coopDB::getClientFileName SQL error";
		myLog.print(logError,message);
		exit(1);
	}

	duplicateHashResult = mysql_store_result(&dbaseConnection);
	if ((row = mysql_fetch_row(duplicateHashResult)) != NULL)
	{
		setClientDirectoryEntry(row[0]);
		returnValue = 1;
	}
	else
	{
		returnValue = -1;
	}
	mysql_free_result(duplicateHashResult); // this should be run after every mysql_store_result to free up memory
	return (returnValue);
}

off64_t coopDB::getCopyFile(int connection)
{

	string SQLStmt;
	char myInt[10];
	string serverDirectoryEntry;
	MYSQL_ROW row;
	MYSQL_RES *duplicateHashResult;
	off64_t returnValue;
	string message;

	sprintf(myInt,"%d",connection);
	SQLStmt.clear();
	SQLStmt.append("select direntry, fileOffset, md5Hash, status, fileSize from coopData.hashFile where ((connection = ");
	SQLStmt.append( myInt);
	SQLStmt.append( " ) and (status = 'copy') )");

	if(mysql_real_query(&dbaseConnection,SQLStmt.c_str(),SQLStmt.length()))
	{
		message = "coopDB::getCopyFile SQL error";
		myLog.print(logError,message);
		exit(1);
	}
	duplicateHashResult = mysql_store_result(&dbaseConnection);
	if ((row = mysql_fetch_row(duplicateHashResult)) != NULL) // if there is a current active file then do this.
	{
		setServerDirectoryEntry(row[0]);
	    if (sscanf(row[1], "%Lu", &fileOffset) < 1)
	    {
	    	message = "coopDB::getCopyFile attribute error";
	    	myLog.print(logError,message);
			returnValue = -1;
	    }
	    else
	    {
	    	returnValue = fileOffset;
	    }
	    setMD5Hash(row[2]);
	    setServerStatus(row[3]);
	    setFileSize(row[4]);
	}
	else
	{
		returnValue = -1;
	}
	mysql_free_result(duplicateHashResult); // this should be run after every mysql_store_result to free up memory
	return (returnValue);
}


off64_t coopDB::getServerFile(char *serverFileHash)
{

	string SQLStmt;
	string serverDirectoryEntry;
	MYSQL_ROW row;
	MYSQL_RES *duplicateHashResult;
	off64_t returnValue;
	string message;

	SQLStmt.clear();
	SQLStmt.append("select direntry, fileOffset, md5Hash, status, fileSize from coopData.hashFile where md5Hash = '");
	SQLStmt.append( serverFileHash);
	SQLStmt.append( "'");

	if(mysql_real_query(&dbaseConnection,SQLStmt.c_str(),SQLStmt.length()))
	{
		message = "coopDB::getServerFile SQL error";
		myLog.print(logError,message);
		exit(1);
	}
	duplicateHashResult = mysql_store_result(&dbaseConnection);
	if ((row = mysql_fetch_row(duplicateHashResult)) != NULL) // if there is a current active file then do this.
	{
		setServerDirectoryEntry(row[0]);
	    if (sscanf(row[1], "%Lu", &fileOffset) < 1)
	    {
	    	message = "coopDB::getServerFile attribute error";
	    	myLog.print(logError,message);
			returnValue = -1;
	    }
	    else
	    {
	    	returnValue = fileOffset;
	    }
	    setMD5Hash(row[2]);
	    setServerStatus(row[3]);
	    setFileSize(row[4]);
	}
	else
	{
		returnValue = -1;
	}
	mysql_free_result(duplicateHashResult); // this should be run after every mysql_store_result to free up memory
	return (returnValue);
}
int coopDB::doesMachineFileExistInDataBase(const char* hash,const char* machineName,const char* direntry,const char* fileType)
{
	string queryStmt;
	MYSQL_ROW row;
	MYSQL_RES *duplicateHashResult;
	int returnValue = 0;
	string message;

	queryStmt.append("SELECT * FROM coopData.fileInformation where ");
	if(strcmp( fileType,"file")==0)
	{
		queryStmt.append("md5Hash = '");
		queryStmt.append(mysqlRealEscapeString(hash));
		queryStmt.append("' and ");
	}

	queryStmt.append("MachineName = '");
	queryStmt.append(mysqlRealEscapeString(machineName));
	queryStmt.append("' and direntry = '");
	queryStmt.append(mysqlRealEscapeString(direntry));
	queryStmt.append("' and type = '");
	queryStmt.append(mysqlRealEscapeString(fileType));
	queryStmt.append("'");

	if(mysql_real_query(&dbaseConnection,queryStmt.c_str(),queryStmt.length()))
	{
		message.append("coopDB::doesMachineFileExistInDataBase SQL Query: " + queryStmt);
    	myLog.print(logError,message);
		exit(1);
	}

	duplicateHashResult = mysql_store_result(&dbaseConnection);
	if ((row = mysql_fetch_row(duplicateHashResult)) != NULL)
	{
		returnValue = 1;
	}
	/* Release memory used to store results and close connection */
	mysql_free_result(duplicateHashResult); // this should be run after every mysql_store_result to free up memory
    return(returnValue);
}

int asciiToUtf8(char *myString, size_t maxLen)
{
	iconv_t it;
	size_t inLength=16;
	size_t outLength=maxLen;
	char *toCode="UTF8";
	char *fromCode="ISO-8859-1";
	int returnValue;
	char *outString;
	size_t c;
	char *source=myString;
	char *result;
	string message;

	outString = (char *)calloc(outLength +10,sizeof (char));
	if (outString == NULL)
	{
		message.clear();
		message.append("coopDB::asciiToUtf8 OUT OF MEMORY!!");
		myLog.print(logError,message);
		exit(0);
		returnValue = -2; //out of Memory
		return(returnValue);
	}
	result=outString;
	it=iconv_open(toCode, fromCode);
	if(it!=(iconv_t)-1)
	{

		inLength=strlen(myString);

		if(iconv(it, (char **)&myString, &inLength, &outString, &outLength)!=-1)
		{
			strcpy (source, result);
			returnValue = 1; //Success
		}
		else
		{
			returnValue = 0; //Failure
		}
		iconv_close(it);
	}
	else
	{
		returnValue = -1; //iconv Open Failure
	}
	free (result);
	return(returnValue);
}

long coopDB::deleteEverything()
{
	string SQLStmt;
	string message;

	SQLStmt.append("delete from coopData.hashFile");
	if (mysql_query(&dbaseConnection, SQLStmt.c_str()))
	{
		message.append("coopDB::deleteEverything coopData.hashFile");
		myLog.print(logError,message);
		message.clear();
		message.append(mysql_error(&dbaseConnection));
		myLog.print(logError,message);
		return(0);
	}

	SQLStmt.clear();
	SQLStmt.append("delete from coopData.fileInformation");
	if (mysql_query(&dbaseConnection, SQLStmt.c_str()))
	{
		message.clear();
		message.append("coopDB::deleteEverything coopData.fileInformation");
		myLog.print(logError,message);
		message.clear();
		message.append(mysql_error(&dbaseConnection));
		myLog.print(logError,message);
		return(0);
	}
	return(1);
}
