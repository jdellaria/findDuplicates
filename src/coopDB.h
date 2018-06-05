/*
 * coopDB.h
 *
 *  Created on: Oct 3, 2010
 *      Author: jdellaria
 */

#ifndef COOPDB_H_
#define COOPDB_H_

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <mysql/mysql.h>
#include <inttypes.h>


using namespace std;

class coopDB {
public:
	coopDB();
	virtual ~coopDB();
	MYSQL	OpenConnection();
	void	CloseConnection();
	long addClientDirectoryEntry();
	long addHashFile();
	void setMD5Hash(string s);
	void setClientDirectoryEntry(string s);
	void setServerDirectoryEntry(string s);
	void setDirectoryEntryType(string s);
	void setIpaddress(string s);
	void setMachineName(string s);
	void setClientStatus(string s);
	void setClientError(string s);
	void setServerStatus(string s);
	void setFileSize(uint64_t s);
	void setFileSize(string s);
	void setFileSize(const char *s);
	void setServerConnection(int connection);
	string mysqlRealEscapeString(string s);
	int doesHashExistInDataBase(const char* hash);
	int doesMachineFileExistInDataBase(const char* hash,const char* machineName,const char* direntry,const char* fileType);
	long updateHashFileStatus();
	long updateHashFileFileOffset();
	long updateHashFileStatusWithError();
	long updateHashFileDirectoryEntry();
	off64_t getServerFile(char *serverFileHash);
	long deleteEverything();

	string getHashFileStatus(const char* hash);
	off64_t getActiveFile(int connection);
	off64_t getCopyFile(int connection);
	uint64_t getFileSize(const char* hash);
	void setFileOffset(uint64_t s);
	void setFileOffset(string s);
	void setFileOffset(const char *s);
	int getClientFileName(const char* hash);
	int getClientFileName(string hash);

	MYSQL	dbaseConnection;
	std::string md5Hash;
	std::string ipaddress;
	std::string MachineName;
	std::string clientStatus;
	std::string clientDirectoryEntry;
	std::string directoryEntryType;
	std::string serverDirectoryEntry;
	std::string serverStatus;
	std::string clientError;
	off64_t fileSize;
	off64_t fileOffset;
	int serverConnection;

};

#endif /* COOPDB_H_ */
