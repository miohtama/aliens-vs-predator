// Launches mplayer.com into a specific URL with LaunchMplayer()
// By Rich Rice --rich@mpath.com
// Modified by Edmond Meinfelder (edmond@mpath.com) for use with AvP

#include <windows.h>
#include <stdio.h>
#include <sys/stat.h>

#ifdef _MSC_VER
   #define stat _stat
#endif

// Execute executes whatever the text in cmdline is.
// If an executable is successfully launched as a 
// result, TRUE is returned. False is returned if 
// nothing happens.

static int
Execute(char *cmdline)
{
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;

	memset (&StartupInfo, 0, sizeof StartupInfo);
	StartupInfo.cb          = sizeof (StartupInfo);
	StartupInfo.dwFlags     = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_SHOWNORMAL;

	if (CreateProcess(NULL, cmdline, NULL, NULL, FALSE,
			CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
			&StartupInfo, &ProcessInfo) == 0)
		return FALSE;

	return TRUE;
}

// Mplayer stores it installed location in the Windows Registry
// under HKEY_LOCAL_MACHINE\Software\Mplayer\Main\Root Directory
// This function returns the location stored in that string value.
// Upon failure, FALSE is returned and, upon success, TRUE is
// returned.
// The location of the Mplayer's install path is returned in
// the parameter mplayer_directory.

static int 
GetMplayerDirectory(char *mplayer_directory)
{
	HKEY hkey;
	HKEY key = HKEY_LOCAL_MACHINE;
	char subkey[]="software\\mpath\\mplayer\\main";
	char valuename[]="root directory";
	char buffer[MAX_PATH];
	DWORD dwType, dwSize;

	
	if (RegOpenKeyEx(key, subkey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		dwSize = MAX_PATH;
		if (RegQueryValueEx(hkey, valuename, 0, &dwType, (LPBYTE) buffer,
			&dwSize) == ERROR_SUCCESS)
		{
			sprintf(mplayer_directory, "%s", buffer);
			return TRUE;
		}
		RegCloseKey(hkey);
	}

	return FALSE;
}

// If the named file exists, this function
// returns TRUE, otherwise FALSE is returned.

static int 
FileExists(char *file)
{
	struct stat buf;
	int result;

	// Get data associated with 'file'
	result = stat( file, &buf );

	// Check if we have a file
	if( result == 0 )
	{
	   return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// This is the only public function, it launches Mplayer's user interface.
// This function returns FALSE upon failure and TRUE on success.

int
LaunchMplayer()
{
	char mplayer_url[]="http://www.mplayer.com/mic/avp.html";
	char cmdline[MAX_PATH], mplaunch_exe[MAX_PATH], mplayer_directory[MAX_PATH];

	if (GetMplayerDirectory(mplayer_directory))
	{
		sprintf(mplaunch_exe, "%s\\programs\\mplaunch.exe", mplayer_directory);
		if (FileExists(mplaunch_exe))
		{
			sprintf(cmdline, "%s AvP.mpi", mplaunch_exe);
			return Execute(cmdline);
		}
	}

	sprintf(cmdline, "rundll32.exe url.dll,FileProtocolHandler %s", mplayer_url);
	return Execute(cmdline);
}
