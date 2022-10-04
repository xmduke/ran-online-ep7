/*
 * This is a part of the BugTrap package.
 * Copyright (c) 2005-2007 IntelleSoft.
 * All rights reserved.
 *
 * Description: Get Windows version.
 * Author: Maksim Pyatkovskiy.
 *
 * This source code is only intended as a supplement to the
 * BugTrap package reference and related electronic documentation
 * provided with the product. See these sources for detailed
 * information regarding the BugTrap package.
 */

#pragma once

class CWinVersion
{
public:
	/// Return true if current OS platform is NT-based.
	static bool IsWinNT();
	/// Return true if current OS platform is Win2K-based.
	static bool IsWin2K();
	/// Get current windows version.
	static OSVERSIONINFO& GetVersionInfo();

private:
	/// Cached Windows version info.
	static OSVERSIONINFO m_osvi;
};

/**
 * @return true if current OS platform is NT-based.
 */
inline bool CWinVersion::IsWinNT()
{
	return (CWinVersion::GetVersionInfo().dwPlatformId >= VER_PLATFORM_WIN32_NT);
}

/**
 * @return true if current OS platform is Win2K-based.
 */
inline bool CWinVersion::IsWin2K()
{
	return (CWinVersion::GetVersionInfo().dwMajorVersion >= 5);
}
