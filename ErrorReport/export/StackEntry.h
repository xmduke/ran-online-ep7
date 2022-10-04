#pragma	once

struct CStackEntry
{
	/// Module name.
	CString m_strModule;
	/// Address.
	CString m_strAddress;
	/// Function name.
	CString m_strFunctionName;
	/// Function offset.
	CString m_strFunctionOffset;
	/// Textual function description.
	CString m_strFunctionInfo;
	/// Source file name.
	CString m_strSourceFile;
	/// Line number.
	CString m_strLineNumber;
	/// Line offset.
	CString m_strLineOffset;
	/// Textual line information.
	CString m_strLineInfo;
};