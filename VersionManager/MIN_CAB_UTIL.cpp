#include "stdafx.h"
#include "MIN_CAB_UTIL.h"

namespace CAB_UTIL_MIN
{
	/*
	* Memory allocation function
	*/
	FNFCIALLOC(mem_alloc)
	{
		return malloc(cb);
	}


	/*
	* Memory free function
	*/
	FNFCIFREE(mem_free)
	{
		free(memory);
	}


	/*
	* File i/o functions
	*/
	FNFCIOPEN(fci_open)
	{
		int result;

		result = _open(pszFile, oflag, pmode);

		if (result == -1)
			*err = errno;

		return result;
	}

	FNFCIREAD(fci_read)
	{
		unsigned int result;

		result = (unsigned int) _read((int) hf, (void *) memory, (int) cb);

		if (result != cb)
			*err = errno;

		return result;
	}

	FNFCIWRITE(fci_write)
	{
		unsigned int result;

		result = (unsigned int) _write((int) hf, (void *) memory, (int) cb);

		if (result != cb)
			*err = errno;

		return result;
	}

	FNFCICLOSE(fci_close)
	{
		int result;

		result = _close((int) hf);

		if (result != 0)
			*err = errno;

		return result;
	}

	FNFCISEEK(fci_seek)
	{
		long result;

		result = _lseek((int) hf, (long) dist, (int) seektype);

		if (result == -1)
			*err = errno;

		return result;
	}

	FNFCIDELETE(fci_delete)
	{
		int result;

		result = remove(pszFile);

		if (result != 0)
			*err = errno;

		return result;
	}


	/*
	* File placed function called when a file has been committed
	* to a cabinet
	*/
	FNFCIFILEPLACED(file_placed)
	{
		printf(
			"   placed file '%s' (size %d) on cabinet '%s'\n",
			pszFile, 
			cbFile, 
			pccab->szCab
		);

		if (fContinuation)
			printf("      (Above file is a later segment of a continued file)\n");

		return 0;
	}


	/*
	* Function to obtain temporary files
	*/
	FNFCIGETTEMPFILE(get_temp_file)
	{
		char    *psz;

		psz = _tempnam("","xx");            // Get a name
		if ((psz != NULL) && (strlen(psz) < (unsigned)cbTempName)) {
			::strcpy(pszTempName,psz);        // Copy to caller's buffer
			::free(psz);                      // Free temporary name buffer
			return TRUE;                    // Success
		}
		//** Failed
		if (psz) {
			free(psz);
		}

		return FALSE;
	}


	/*
	* Progress function
	*/
	FNFCISTATUS(progress)
	{
		client_state	*cs;

		cs = (client_state *) pv;

		if (typeStatus == statusFile)
		{
			cs->total_compressed_size += cb1;
			cs->total_uncompressed_size += cb2;

			/*
			* Compressing a block into a folder
			*
			* cb2 = uncompressed size of block
			*/
			printf(
				"Compressing: %9ld -> %9ld             \r",
				cs->total_uncompressed_size,
				cs->total_compressed_size
			);
			
			fflush(stdout);
		}
		else if (typeStatus == statusFolder)
		{
			int	percentage;

			/*
			* Adding a folder to a cabinet
			*
			* cb1 = amount of folder copied to cabinet so far
			* cb2 = total size of folder
			*/
			percentage = get_percentage(cb1, cb2);

			printf("Copying folder to cabinet: %d%%      \r", percentage);
			fflush(stdout);
		}

		return 0;
	}


	void store_cab_name(char *cabname, int iCab)
	{
		sprintf(cabname, "TEST%d.CAB", iCab);
	}


	FNFCIGETNEXTCABINET(get_next_cabinet)
	{
		/*
		* Cabinet counter has been incremented already by FCI
		*/

		/*
		* Store next cabinet name
		*/
		store_cab_name(pccab->szCab, pccab->iCab);

		/*
		* You could change the disk name here too, if you wanted
		*/

		return TRUE;
	}


	FNFCIGETOPENINFO(get_open_info)
	{
		BY_HANDLE_FILE_INFORMATION	finfo;
		FILETIME					filetime;
		HANDLE						handle;
		DWORD                       attrs;
		int                         hf;

		/*
		* Need a Win32 type handle to get file date/time
		* using the Win32 APIs, even though the handle we
		* will be returning is of the type compatible with
		* _open
		*/
		handle = CreateFile(
			pszName,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL
		);
	   
		if (handle == INVALID_HANDLE_VALUE)
		{
			return -1;
		}

		if (GetFileInformationByHandle(handle, &finfo) == FALSE)
		{
			CloseHandle(handle);
			return -1;
		}
	   
		FileTimeToLocalFileTime(
			&finfo.ftLastWriteTime, 
			&filetime
		);

		FileTimeToDosDateTime(
			&filetime,
			pdate,
			ptime
		);

		attrs = GetFileAttributes(pszName);

		if (attrs == 0xFFFFFFFF)
		{
			/* failure */
			*pattribs = 0;
		}
		else
		{
			/*
			* Mask out all other bits except these four, since other
			* bits are used by the cabinet format to indicate a
			* special meaning.
			*/
			*pattribs = (int) (attrs & (_A_RDONLY | _A_SYSTEM | _A_HIDDEN | _A_ARCH));
		}

		CloseHandle(handle);


		/*
		* Return handle using _open
		*/
		hf = _open( pszName, _O_RDONLY | _O_BINARY );

		if (hf == -1)
			return -1; // abort on error
	   
		return hf;
	}


	void set_cab_parameters(PCCAB cab_parms)
	{
		memset(cab_parms, 0, sizeof(CCAB));

		cab_parms->cb = MEDIA_SIZE;
		cab_parms->cbFolderThresh = FOLDER_THRESHOLD;

		/*
		* Don't reserve space for any extensions
		*/
		cab_parms->cbReserveCFHeader = 0;
		cab_parms->cbReserveCFFolder = 0;
		cab_parms->cbReserveCFData   = 0;

		/*
		* We use this to create the cabinet name
		*/
		cab_parms->iCab = 1;

		/*
		* If you want to use disk names, use this to
		* count disks
		*/
		cab_parms->iDisk = 0;

		/*
		* Choose your own number
		*/
		cab_parms->setID = 12345;

		/*
		* Only important if CABs are spanning multiple
		* disks, in which case you will want to use a
		* real disk name.
		*
		* Can be left as an empty string.
		*/
		strcpy(cab_parms->szDisk, "MyDisk");

		/* where to store the created CAB files */
		strcpy(cab_parms->szCabPath, "c:\\");

		/* store name of first CAB file */
		store_cab_name(cab_parms->szCab, cab_parms->iCab);
	}

	void MinSetCabParameters(PCCAB cab_parms, const char* szTargetPath)
	{
		memset(cab_parms, 0, sizeof(CCAB));

		cab_parms->cb = MEDIA_SIZE;
		cab_parms->cbFolderThresh = FOLDER_THRESHOLD;

		/*
		* Don't reserve space for any extensions
		*/
		cab_parms->cbReserveCFHeader = 0;
		cab_parms->cbReserveCFFolder = 0;
		cab_parms->cbReserveCFData   = 0;

		/*
		* We use this to create the cabinet name
		*/
		cab_parms->iCab = 1;

		/*
		* If you want to use disk names, use this to
		* count disks
		*/
		cab_parms->iDisk = 0;

		/*
		* Choose your own number
		*/
		cab_parms->setID = 12345;

		/*
		* Only important if CABs are spanning multiple
		* disks, in which case you will want to use a
		* real disk name.
		*
		* Can be left as an empty string.
		*/
		strcpy(cab_parms->szDisk, "MyDisk");

		/* where to store the created CAB files */
		strcpy(cab_parms->szCabPath, szTargetPath);

		/* store name of first CAB file */
		store_cab_name(cab_parms->szCab, cab_parms->iCab);	
	}

	BOOL MinMakeCab( const char* szSrcFile, const char* szTargetPath)
	{
		HFCI			hfci;
		ERF				erf;
		CCAB			cab_parameters;
		client_state	cs;

		/*  
		* Initialise our internal state
		*/
		cs.total_compressed_size = 0;
		cs.total_uncompressed_size = 0;

		MinSetCabParameters(&cab_parameters, szTargetPath);
		
		char stripped_name[256] = {0};

		strip_path(szSrcFile, stripped_name);
		
		CString strCabName;
		strCabName.Format(_T("%s"), stripped_name);
		strCabName = strCabName + ".cab";

		strcpy(cab_parameters.szCab, strCabName.GetString());

		hfci = FCICreate(
			&erf,
			file_placed,
			mem_alloc,
			mem_free,
			fci_open,
			fci_read,
			fci_write,
			fci_close,
			fci_seek,
			fci_delete,
			get_temp_file,
			&cab_parameters,
			&cs
		);

		if (hfci == NULL)
		{
			return FALSE;
		}
		
		/*
			* Flush the folder?
			*/
		if (!strcmp(szSrcFile, "+"))
		{
			if (FALSE == FCIFlushFolder(
				hfci,
				get_next_cabinet,
				progress))
			{
				(void) FCIDestroy(hfci);
				return FALSE;
			}
		}

		/*
			* Don't store the path name in the cabinet file!
			*/
		

		if (FALSE == FCIAddFile(
			hfci,
			(char*)szSrcFile,  /* file to add */
			stripped_name, /* file name in cabinet file */
			FALSE, /* file is not executable */
			get_next_cabinet,
			progress,
			get_open_info,
			COMPRESSION_TYPE))
		{
			(void) FCIDestroy(hfci);
			return FALSE;
		}

		/*
		* This will automatically flush the folder first
		*/
		if (FALSE == FCIFlushCabinet(
			hfci,
			FALSE,
			get_next_cabinet,
			progress))
		{
			(void) FCIDestroy(hfci);
			return FALSE;
		}

		if (FCIDestroy(hfci) != TRUE)
		{
			return FALSE;
		}
		return TRUE;
	}

	BOOL test_fci(int num_files, char *file_list[])
	{
		HFCI			hfci;
		ERF				erf;
		CCAB			cab_parameters;
		int				i;
		client_state	cs;

		/*  
		* Initialise our internal state
		*/
		cs.total_compressed_size = 0;
		cs.total_uncompressed_size = 0;

		set_cab_parameters(&cab_parameters);

		hfci = FCICreate(
			&erf,
			file_placed,
			mem_alloc,
			mem_free,
			fci_open,
			fci_read,
			fci_write,
			fci_close,
			fci_seek,
			fci_delete,
			get_temp_file,
			&cab_parameters,
			&cs
		);

		if (hfci == NULL)
		{
			//printf("FCICreate() failed: code %d [%s]\n", erf.erfOper, return_fci_error_string(erf.erfOper));

			return FALSE;
		}

		for (i = 0; i < num_files; i++)
		{
			char	stripped_name[256];

			/*
			* Flush the folder?
			*/
			if (!strcmp(file_list[i], "+"))
			{
				if (FALSE == FCIFlushFolder(
					hfci,
					get_next_cabinet,
					progress))
				{
					//printf("FCIFlushFolder() failed: code %d [%s]\n",
					//	erf.erfOper, return_fci_error_string(erf.erfOper));

					(void) FCIDestroy(hfci);

					return FALSE;
				}

				continue;
			}

			/*
			* Don't store the path name in the cabinet file!
			*/
			strip_path(file_list[i], stripped_name);

			if (FALSE == FCIAddFile(
				hfci,
				file_list[i],  /* file to add */
				stripped_name, /* file name in cabinet file */
				FALSE, /* file is not executable */
				get_next_cabinet,
				progress,
				get_open_info,
				COMPRESSION_TYPE))
			{
			/*	printf("FCIAddFile() failed: code %d [%s]\n",
					erf.erfOper, return_fci_error_string(erf.erfOper)
				);*/

				(void) FCIDestroy(hfci);

				return FALSE;
			}
		}		

		/*
		* This will automatically flush the folder first
		*/
		if (FALSE == FCIFlushCabinet(
			hfci,
			FALSE,
			get_next_cabinet,
			progress))
		{
		/*	printf("FCIFlushCabinet() failed: code %d [%s]\n",
				erf.erfOper, return_fci_error_string(erf.erfOper)
			);*/

			(void) FCIDestroy(hfci);

			return FALSE;
		}

		if (FCIDestroy(hfci) != TRUE)
		{
			/*printf("FCIDestroy() failed: code %d [%s]\n",
				erf.erfOper, return_fci_error_string(erf.erfOper)
			);*/

			return FALSE;
		}

		printf("                                                 \r");

		return TRUE;
	}


	void strip_path (const char *filename, char *stripped_name)
	{
		char	*p;

		p = strrchr(filename, '\\');

		if (p == NULL)
			strcpy(stripped_name, filename);
		else
			strcpy(stripped_name, p+1);
	}


	int get_percentage(unsigned long a, unsigned long b)
	{
		while (a > 10000000)
		{
			a >>= 3;
			b >>= 3;
		}

		if (b == 0)
			return 0;

		return ((a*100)/b);
	}


	char *return_fci_error_string(FCIERROR err)
	{
		switch (err)
		{
			case FCIERR_NONE:
				return "No error";

			case FCIERR_OPEN_SRC:
				return "Failure opening file to be stored in cabinet";
			
			case FCIERR_READ_SRC:
				return "Failure reading file to be stored in cabinet";
			
			case FCIERR_ALLOC_FAIL:
				return "Insufficient memory in FCI";

			case FCIERR_TEMP_FILE:
				return "Could not create a temporary file";

			case FCIERR_BAD_COMPR_TYPE:
				return "Unknown compression type";

			case FCIERR_CAB_FILE:
				return "Could not create cabinet file";

			case FCIERR_USER_ABORT:
				return "Client requested abort";

			case FCIERR_MCI_FAIL:
				return "Failure compressing data";

			default:
				return "Unknown error";
		}
	} 
}
