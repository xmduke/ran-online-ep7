// Class to read and write to Excel and text delimited spreadsheet
//
// Created by Yap Chun Wei
// December 2001
// 
// Version 1.1
// Updates: Fix bug in ReadRow() which prevent reading of single column spreadsheet
//
//	Note : ¼öÁ¤(2002.11.23) JDH
//
#pragma once

#include <odbcinst.h>
#include <afxdb.h>

class CSpreadSheet
{
public:
	// Open spreadsheet for reading and writing
	CSpreadSheet ( CString File, CString SheetOrSeparator, bool Backup = true );

	// Perform some cleanup functions
	~CSpreadSheet ();

	// Add header row to spreadsheet
	bool AddHeaders ( CStringArray &FieldNames, bool replace = false );

	// Clear text delimited file content
	bool DeleteSheet ();

	// Clear entire Excel spreadsheet content. The sheet itself is not deleted
	bool DeleteSheet ( CString SheetName );
	
	// Insert or replace a row into spreadsheet. Default is add new row. 
	bool AddRow ( CStringArray &RowValues, long row = 0, bool replace = false );
	
	// Replace or add a cell into Excel spreadsheet using header row or column alphabet.
	// Default is add cell into new row. Set Auto to false if want to force column to be used as header name
	bool AddCell ( CString CellValue, CString column, long row = 0, bool Auto = true );

	// Replace or add a cell into spreadsheet using column number. Default is add cell into new row. 
	bool AddCell ( CString CellValue, short column, long row = 0 );

	// Search and replace rows in Excel spreadsheet
	bool ReplaceRows ( CStringArray &NewRowValues, CStringArray &OldRowValues );

	// Read a row from spreadsheet. Default is read the next row
	bool ReadRow ( CStringArray &RowValues, long row = 0 );
	
	// Read a column from Excel spreadsheet using header row or column alphabet. Set Auto to false if want to force column to be used as header name
	bool ReadColumn ( CStringArray &ColumnValues, CString column, bool Auto = true );

	// Read a column from spreadsheet using column number
	bool ReadColumn ( CStringArray &ColumnValues, short column );
	
	// Read a cell from Excel spreadsheet using header row or column alphabet. Default is read the next cell in next row. Set Auto to false if want to force column to be used as header name
	bool ReadCell ( CString &CellValue, CString column, long row = 0, bool Auto = true );
	
	// Read a cell from spreadsheet using column number. Default is read the next cell in next row.
	bool ReadCell ( CString &CellValue, short column, long row = 0 );
	
	// Begin transaction
	void BeginTransaction ();

	// Save changes to spreadsheet
	bool Commit ();

	// Undo changes to spreadsheet
	bool RollBack ();
	bool Convert ( CString SheetOrSeparator );

	// Get the header row from spreadsheet
	inline void GetFieldNames ( CStringArray &FieldNames )	{ FieldNames.RemoveAll(); FieldNames.Copy(m_aFieldNames); }

	// Get total number of rows in  spreadsheet
	inline long GetTotalRows ()								{ return m_dTotalRows; }

	// Get total number of columns in  spreadsheet
	inline short GetTotalColumns ()							{ return m_dTotalColumns; }

	// Get the currently selected row in  spreadsheet
	inline long GetCurrentRow ()							{ return m_dCurrentRow; }
	
	// Get status of backup. True if backup is successful, False if spreadsheet is not backup
	inline bool GetBackupStatus ()							{ return m_bBackup; }
	
	// Get status of Transaction. True if Transaction is started, False if Transaction is not started or has error in starting
	inline bool GetTransactionStatus ()						{ return m_bTransaction; }

	// Get last error message
	inline CString GetLastError ()							{ return m_sLastError; }

private:
	// Open a text delimited file for reading or writing
	bool Open ();

	// Get the name of the Excel-ODBC driver
	void GetExcelDriver();
	
	// Convert Excel column in alphabet into column number
	short CalculateColumnNumber ( CString column, bool Auto );

	// Internal flag to denote newly created spreadsheet or previously created spreadsheet
	bool m_bAppend;

	// Internal flag to denote status of Backup
	bool m_bBackup;

	// Internal flag to denote whether file is Excel spreadsheet or text delimited spreadsheet
	bool m_bExcel;

	// Internal flag to denote status of Transaction
	bool m_bTransaction;

	// Index of current row, starting from 1
	long m_dCurrentRow;

	// Total number of rows in spreadsheet
	long m_dTotalRows;
	
	// Total number of columns in Excel spreadsheet. Largest number of columns in text delimited spreadsheet
	short m_dTotalColumns;

	// SQL statement to open Excel spreadsheet for reading
	CString m_sSql;
	
	// DSN string to open Excel spreadsheet for reading and writing
	CString m_sDsn;
	
	// Temporary string for SQL statements or for use by functions
	CString m_stempSql;

	// Temporary string for use by functions
	CString m_stempString;

	// Sheet name of Excel spreadsheet
	CString m_sSheetName;

	// Name of Excel Driver
	CString m_sExcelDriver;

	// Spreadsheet file name
	CString m_sFile;

	// Separator in text delimited spreadsheet
	CString m_sSeparator;

	// Last error message
	CString m_sLastError;

	// Temporary array for use by functions
	CStringArray m_atempArray;
	
	// Header row in spreadsheet
	CStringArray m_aFieldNames;

	// Content of all the rows in spreadsheet
	CStringArray m_aRows;

	// Database variable for Excel spreadsheet
	CDatabase *m_Database;

	// Recordset for Excel spreadsheet
	CRecordset *m_rSheet;
};

