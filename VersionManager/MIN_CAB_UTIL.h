#ifndef MIN_CAB_UTIL_H_
#define MIN_CAB_UTIL_H_

#include <io.h>
#include <fcntl.h>
#include <string.h>

#include "Fci.h"
#include "Fdi.h"

namespace CAB_UTIL_MIN
{
	/*
	* When a CAB file reaches this size, a new CAB will be created
	* automatically.  This is useful for fitting CAB files onto disks.
	*
	* If you want to create just one huge CAB file with everything in
	* it, change this to a very very large number.
	*/
	#define MEDIA_SIZE			300000000

	/*
	* When a folder has this much compressed data inside it,
	* automatically flush the folder.
	*
	* Flushing the folder hurts compression a little bit, but
	* helps random access significantly.
	*/
	#define FOLDER_THRESHOLD	900000


	/*
	* Compression type to use
	*/

	#define COMPRESSION_TYPE    tcompTYPE_MSZIP


	/*
	* Our internal state
	*
	* The FCI APIs allow us to pass back a state pointer of our own
	*/
	typedef struct
	{
		long    total_compressed_size;      /* total compressed size so far */
		long	total_uncompressed_size;	/* total uncompressed size so far */
	} client_state;


	/*
	* Function prototypes 
	*/
	void    store_cab_name(char *cabname, int iCab);
	void    set_cab_parameters(PCCAB cab_parms);
	
	BOOL	test_fci(int num_files, char *file_list[]);
	BOOL    test_fci(int num_files, char *file_list[]);

	// void    strip_path(char *filename, char *stripped_name);
	void    strip_path(const char *filename, char *stripped_name);

	int		get_percentage(unsigned long a, unsigned long b);
	char    *return_fci_error_string(FCIERROR err);

	BOOL	MinMakeCab( const char* szSrcFile, const char* szTargetPath);

	/*
	* Memory allocation function
	*/
	FNFCIALLOC(mem_alloc);	
    /*
	* Memory free function
	*/
	FNFCIFREE(mem_free);
	/*
	* File i/o functions
	*/
	FNFCIOPEN(fci_open);
	FNFCIREAD(fci_read);
	FNFCIWRITE(fci_write);
	FNFCICLOSE(fci_close);
	FNFCISEEK(fci_seek);
	FNFCIDELETE(fci_delete);
	/*
	* File placed function called when a file has been committed
	* to a cabinet
	*/
	FNFCIFILEPLACED(file_placed);
	/*
	* Function to obtain temporary files
	*/
	FNFCIGETTEMPFILE(get_temp_file);
	/*
	* Progress function
	*/
	FNFCISTATUS(progress);
	void store_cab_name(char *cabname, int iCab);
	FNFCIGETNEXTCABINET(get_next_cabinet);
	FNFCIGETOPENINFO(get_open_info);

	void set_cab_parameters(PCCAB cab_parms);
	void MinSetCabParameters(PCCAB cab_parms, const char* szTargetPath);
	BOOL MinMakeCab(const char* szSrcFile, const char* szTargetPath);	
	
	int get_percentage(unsigned long a, unsigned long b);
	char *return_fci_error_string(FCIERROR err);
}

#endif // MIN_CAB_UTIL_H_