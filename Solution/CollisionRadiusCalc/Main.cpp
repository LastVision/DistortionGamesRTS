/*
* An example demonstrating recursive directory traversal.
*
* Compile this file with Visual Studio 2008 project vs2008.sln and run
* the produced command in console with a directory name argument.  For
* example, command
*
*     find "C:\Program Files"
*
* might produce a listing with thousands of entries such as
*
*     c:\Program Files/7-Zip/7-zip.chm
*     c:\Program Files/7-Zip/7-zip.dll
*     c:\Program Files/7-Zip/7z.dll
*     c:\Program Files/Adobe/Reader 10.0/Reader/logsession.dll
*     c:\Program Files/Adobe/Reader 10.0/Reader/LogTransport2.exe
*     c:\Program Files/Windows NT/Accessories/wordpad.exe
*     c:\Program Files/Windows NT/Accessories/write.wpc
*
* The find command provided by this file is only an example.  That is,
* the command does not provide options to restrict the output to certain
* files as the Linux version does.
*
* Copyright (C) 2006-2012 Toni Ronkko
* This file is part of dirent.  Dirent may be freely distributed
* under the MIT license.  For all details and documentation, see
* https://github.com/tronkko/dirent
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dirent.h"
#include "Reader.h"
#include <TimerManager.h>
#include <DL_Debug.h>

static int find_directory(const char *dirname, Reader& aReader);


int main()
{
	DL_Debug::Debug::Create();
	CU::TimerManager::Create();
	
	Reader reader;
	find_directory("Data", reader);

	CU::TimerManager::Destroy();
	DL_Debug::Debug::Destroy();
	return EXIT_SUCCESS;
}

/* Find files and subdirectories recursively */
static int find_directory(const char *dirname, Reader& aReader)
{
	DIR *dir;
	char buffer[PATH_MAX + 2];
	char *p = buffer;
	const char *src;
	char *end = &buffer[PATH_MAX];
	int ok;

	/* Copy directory name to buffer */
	src = dirname;
	while (p < end  &&  *src != '\0') {
		*p++ = *src++;
	}
	*p = '\0';

	/* Open directory stream */
	dir = opendir(dirname);
	if (dir != NULL) {
		struct dirent *ent;

		/* Print all files and directories within the directory */
		while ((ent = readdir(dir)) != NULL) {
			char *q = p;
			char c;

			/* Get final character of directory name */
			if (buffer < q) {
				c = q[-1];
			}
			else {
				c = ':';
			}

			/* Append directory separator if not already there */
			if (c != ':'  &&  c != '/'  &&  c != '\\') {
				*q++ = '/';
			}

			/* Append file name */
			src = ent->d_name;
			while (q < end  &&  *src != '\0') {
				*q++ = *src++;
			}
			*q = '\0';

			/* Decide what to do with the directory entry */
			switch (ent->d_type) {
			case DT_LNK:
			case DT_REG:
				/* Output file name with directory */
				//printf("%s\n", buffer);
				aReader.ReadFile(buffer);
				break;

			case DT_DIR:
				/* Scan sub-directory recursively */
				if (strcmp(ent->d_name, ".") != 0
					&& strcmp(ent->d_name, "..") != 0) {
					find_directory(buffer, aReader);
				}
				break;

			default:
				/* Ignore device entries */
				/*NOP*/;
			}

		}

		closedir(dir);
		ok = 1;

	}
	else {
		/* Could not open directory */
		printf("Cannot open directory %s\n", dirname);
		ok = 0;
	}

	return ok;
}

