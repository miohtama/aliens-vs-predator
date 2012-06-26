/* KJL 15:17:31 10/12/98 - user profile stuff */
#include "list_tem.hpp"
extern "C"
{
#include "3dc.h"
#include "inline.h"
#include "module.h"
#include "stratdef.h"

#include "AvP_UserProfile.h"
#include "language.h"
#include "GammaControl.h"
#include "psnd.h"
#include "cd_player.h"

#define UseLocalAssert Yes
#include "ourasert.h"

 // Edmond
 #include "pldnet.h"
 #include "dp_func.h"
 #include <time.h>

static int LoadUserProfiles(void);

static void EmptyUserProfilesList(void);
static int MakeNewUserProfile(void);
static void InsertProfileIntoList(AVP_USER_PROFILE *profilePtr);
static int ProfileIsMoreRecent(AVP_USER_PROFILE *profilePtr, AVP_USER_PROFILE *profileToTestAgainstPtr);
static void SetDefaultProfileOptions(AVP_USER_PROFILE *profilePtr);

extern int SmackerSoundVolume;
extern int EffectsSoundVolume;
extern int MoviesAreActive;
extern int IntroOutroMoviesAreActive;
extern char MP_PlayerName[];
extern int AutoWeaponChangeOn;


List<AVP_USER_PROFILE *> UserProfilesList;
static AVP_USER_PROFILE DefaultUserProfile = 
{
	"",
};
static AVP_USER_PROFILE *CurrentUserProfilePtr;

extern void ExamineSavedUserProfiles(void)
{
	// delete any existing profiles
	EmptyUserProfilesList();
	
//	UserProfilesList.add_entry(profilePtr);
//	SaveUserProfile(profilePtr);
	if (LoadUserProfiles())
	{
	
	}
	else /* No user profile found. We'll have to make one */
	{
	//	MakeNewUserProfile();
	}

	AVP_USER_PROFILE *profilePtr = new AVP_USER_PROFILE;
	*profilePtr = DefaultUserProfile;

	GetLocalTime(&profilePtr->TimeLastUpdated);
	SystemTimeToFileTime(&profilePtr->TimeLastUpdated,&profilePtr->FileTime);

	strncpy(profilePtr->Name,GetTextString(TEXTSTRING_USERPROFILE_NEW),MAX_SIZE_OF_USERS_NAME);
	profilePtr->Name[MAX_SIZE_OF_USERS_NAME]=0;
	SetDefaultProfileOptions(profilePtr);

	InsertProfileIntoList(profilePtr);

}

extern int NumberOfUserProfiles(void)
{
	int n = UserProfilesList.size();

	LOCALASSERT(n>0);

	return n-1;
}

extern AVP_USER_PROFILE *GetFirstUserProfile(void)
{
	CurrentUserProfilePtr=UserProfilesList.first_entry();
	return CurrentUserProfilePtr;
}

extern AVP_USER_PROFILE *GetNextUserProfile(void)
{
	if (CurrentUserProfilePtr == UserProfilesList.last_entry())
	{
		CurrentUserProfilePtr = UserProfilesList.first_entry();
	}
	else
	{
		CurrentUserProfilePtr = UserProfilesList.next_entry(CurrentUserProfilePtr);
	}
	return CurrentUserProfilePtr;
}

static void EmptyUserProfilesList(void)
{
	while (UserProfilesList.size())
	{
		delete UserProfilesList.first_entry();
		UserProfilesList.delete_first_entry();
	}
}

extern int SaveUserProfile(AVP_USER_PROFILE *profilePtr)
{
	char *filename = new char [strlen(USER_PROFILES_PATH)+strlen(profilePtr->Name)+strlen(USER_PROFILES_SUFFIX)+1];
	strcpy(filename,USER_PROFILES_PATH);
	strcat(filename,profilePtr->Name);
	strcat(filename,USER_PROFILES_SUFFIX);

	FILE* file=fopen(filename,"wb");
	delete [] filename;
	if(!file) return 0;
	
	SaveSettingsToUserProfile(profilePtr);
	
	fwrite(profilePtr,sizeof(AVP_USER_PROFILE),1,file);
	fclose(file);

	return 1;

}

extern void DeleteUserProfile(int number)
{
	AVP_USER_PROFILE *profilePtr = GetFirstUserProfile();

	for (int i=0; i<number; i++) profilePtr = GetNextUserProfile();

	char *filename = new char [strlen(USER_PROFILES_PATH)+strlen(profilePtr->Name)+strlen(USER_PROFILES_SUFFIX)+1];
	strcpy(filename,USER_PROFILES_PATH);
	strcat(filename,profilePtr->Name);
	strcat(filename,USER_PROFILES_SUFFIX);

	DeleteFile(filename);

	delete [] filename;
	{
		int i;
		filename = new char [100];

		for (i=0; i<NUMBER_OF_SAVE_SLOTS; i++)
		{
			sprintf(filename,"%s%s_%d.sav",USER_PROFILES_PATH,profilePtr->Name,i+1);
			DeleteFile(filename);
		}
		delete [] filename;
	}

}



static void InsertProfileIntoList(AVP_USER_PROFILE *profilePtr)
{
	if (UserProfilesList.size())
	{
		AVP_USER_PROFILE *profileInListPtr = GetFirstUserProfile();

		for (int i=0; i<UserProfilesList.size(); i++, profileInListPtr = GetNextUserProfile())
		{
			if (ProfileIsMoreRecent(profilePtr,profileInListPtr))
			{
				UserProfilesList.add_entry_before(profilePtr,profileInListPtr);
				return;
			}
		}
	}
	UserProfilesList.add_entry(profilePtr);
}
static int ProfileIsMoreRecent(AVP_USER_PROFILE *profilePtr, AVP_USER_PROFILE *profileToTestAgainstPtr)
{
	if (CompareFileTime(&profilePtr->FileTime,&profileToTestAgainstPtr->FileTime)==1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
static int LoadUserProfiles(void)
{

	const char* load_name=USER_PROFILES_WILDCARD_NAME;
	// allow a wildcard search
	WIN32_FIND_DATA wfd;

	HANDLE hFindFile = ::FindFirstFile(load_name,&wfd);

	if (INVALID_HANDLE_VALUE == hFindFile)
	{
//		printf("File Not Found: <%s>\n",load_name);
		return 0;
	}

	// get any path in the load_name
	int nPathLen = 0;
	char * pColon = strrchr(load_name,':');
	if (pColon) nPathLen = pColon - load_name + 1;
	char * pBackSlash = strrchr(load_name,'\\');
	if (pBackSlash)
	{
		int nLen = pBackSlash - load_name + 1;
		if (nLen > nPathLen) nPathLen = nLen;
	}
	char * pSlash = strrchr(load_name,'/');
	if (pSlash)
	{
		int nLen = pSlash - load_name + 1;
		if (nLen > nPathLen) nPathLen = nLen;
	}

	do
	{
		if
		(
			!(wfd.dwFileAttributes &
				(FILE_ATTRIBUTE_DIRECTORY
				|FILE_ATTRIBUTE_SYSTEM
				|FILE_ATTRIBUTE_HIDDEN
				|FILE_ATTRIBUTE_READONLY))
				// not a directory, hidden or system file
		)
		{
			char * pszFullPath = new char [nPathLen+strlen(wfd.cFileName)+1];
			strncpy(pszFullPath,load_name,nPathLen);
			strcpy(pszFullPath+nPathLen,wfd.cFileName);
			
			
			//make sure the file is a rif file
			HANDLE rif_file;
			rif_file = CreateFile (pszFullPath, GENERIC_READ, 0, 0, OPEN_EXISTING, 
					FILE_FLAG_RANDOM_ACCESS, 0);
			if(rif_file==INVALID_HANDLE_VALUE)
			{
//				printf("couldn't open %s\n",pszFullPath);
				delete[] pszFullPath;
				continue;
			}

			AVP_USER_PROFILE *profilePtr = new AVP_USER_PROFILE;
			unsigned long bytes_read;
			
			if (!ReadFile(rif_file, profilePtr, sizeof(AVP_USER_PROFILE), &bytes_read, 0))
			{
	       		CloseHandle (rif_file);
				delete[] pszFullPath;
				delete profilePtr;
				continue;
			}
			FILETIME ftLocal;
			FileTimeToLocalFileTime(&wfd.ftLastWriteTime,&ftLocal);
			FileTimeToSystemTime(&ftLocal,&profilePtr->TimeLastUpdated);
			profilePtr->FileTime = ftLocal;
			InsertProfileIntoList(profilePtr);
			CloseHandle (rif_file);
			delete[] pszFullPath;

		}

	}
	while (::FindNextFile(hFindFile,&wfd));


	if (ERROR_NO_MORE_FILES != GetLastError())
	{
	   //	printf("Error finding next file\n");
	}

	::FindClose(hFindFile);

	return 1;
}



static void SetDefaultProfileOptions(AVP_USER_PROFILE *profilePtr)
{
	// set Gamma
	RequestedGammaSetting = 128;
	
	// controls
	MarineInputPrimaryConfig = DefaultMarineInputPrimaryConfig;
	MarineInputSecondaryConfig = DefaultMarineInputSecondaryConfig;
	PredatorInputPrimaryConfig = DefaultPredatorInputPrimaryConfig;
	PredatorInputSecondaryConfig = DefaultPredatorInputSecondaryConfig;
	AlienInputPrimaryConfig = DefaultAlienInputPrimaryConfig;
	AlienInputSecondaryConfig = DefaultAlienInputSecondaryConfig;
	ControlMethods = DefaultControlMethods;
	JoystickControlMethods = DefaultJoystickControlMethods;
	
	SmackerSoundVolume = ONE_FIXED/512;
	EffectsSoundVolume = VOLUME_DEFAULT;
	CDPlayerVolume = CDDA_VOLUME_DEFAULT;
	MoviesAreActive = 1;
	IntroOutroMoviesAreActive = 1; 
	AutoWeaponChangeOn = TRUE;
	
 
 	// Edmond to add in network name
 	srand(time(NULL));
 	switch(rand()%11)
 	{
 		case 0:
 			strcpy(MP_PlayerName, "DogMeat");
 			break;
 		case 1:
 			strcpy(MP_PlayerName, "FreshMeat");
 			break;
 		case 2:
 			strcpy(MP_PlayerName, "RancidMeat");
 			break;
 		case 3:
 			strcpy(MP_PlayerName, "HorseMeat");
 			break;
 		case 4:
 			strcpy(MP_PlayerName, "RawMeat");
 			break;
 		case 5:
 			strcpy(MP_PlayerName, "LiveMeat");
 			break;
 		case 6:
 			strcpy(MP_PlayerName, "M-m-m-meat");
 			break;
 		case 7:
 			strcpy(MP_PlayerName, "LlamaMeat");
 			break;
 		case 8:
 			strcpy(MP_PlayerName, "JustMeat");
 			break;
 		case 9:
 			strcpy(MP_PlayerName, "TastyMeat");
 			break;
 		case 10:
 			strcpy(MP_PlayerName, "MonkeyMeat");
 			break;
 	}
	strcpy(MP_PlayerName,"DeadMeat");

	SetToDefaultDetailLevels();
	
	{
		int a,b;

		for (a=0; a<I_MaxDifficulties; a++) {
			for (b=0; b<AVP_ENVIRONMENT_END_OF_LIST; b++) {
				profilePtr->PersonalBests[a][b]=DefaultLevelGameStats;
			}
		}
	}

	SaveSettingsToUserProfile(profilePtr);
}




			
extern void GetSettingsFromUserProfile(void)
{
	RequestedGammaSetting = UserProfilePtr->GammaSetting;

	MarineInputPrimaryConfig = 		UserProfilePtr->MarineInputPrimaryConfig;
	MarineInputSecondaryConfig = 	UserProfilePtr->MarineInputSecondaryConfig;
	PredatorInputPrimaryConfig = 	UserProfilePtr->PredatorInputPrimaryConfig;
	PredatorInputSecondaryConfig = 	UserProfilePtr->PredatorInputSecondaryConfig;
	AlienInputPrimaryConfig = 		UserProfilePtr->AlienInputPrimaryConfig;
	AlienInputSecondaryConfig = 	UserProfilePtr->AlienInputSecondaryConfig;
	ControlMethods = 				UserProfilePtr->ControlMethods;
	JoystickControlMethods = 		UserProfilePtr->JoystickControlMethods;
	MenuDetailLevelOptions = 		UserProfilePtr->DetailLevelSettings;
	SmackerSoundVolume =			UserProfilePtr->SmackerSoundVolume;
	EffectsSoundVolume =			UserProfilePtr->EffectsSoundVolume;
	CDPlayerVolume = 				UserProfilePtr->CDPlayerVolume;
	MoviesAreActive =				UserProfilePtr->MoviesAreActive;
	IntroOutroMoviesAreActive =		UserProfilePtr->IntroOutroMoviesAreActive;
	AutoWeaponChangeOn = 			!UserProfilePtr->AutoWeaponChangeDisabled;
   	strncpy(MP_PlayerName,UserProfilePtr->MultiplayerCallsign,15);

	SetDetailLevelsFromMenu();
}


extern void SaveSettingsToUserProfile(AVP_USER_PROFILE *profilePtr)
{
	profilePtr->GammaSetting = RequestedGammaSetting;

	profilePtr->MarineInputPrimaryConfig =		MarineInputPrimaryConfig;
	profilePtr->MarineInputSecondaryConfig =	MarineInputSecondaryConfig;
	profilePtr->PredatorInputPrimaryConfig =	PredatorInputPrimaryConfig;
	profilePtr->PredatorInputSecondaryConfig =	PredatorInputSecondaryConfig;
	profilePtr->AlienInputPrimaryConfig =		AlienInputPrimaryConfig;
	profilePtr->AlienInputSecondaryConfig =		AlienInputSecondaryConfig;
	profilePtr->ControlMethods =				ControlMethods;
	profilePtr->JoystickControlMethods =		JoystickControlMethods;
	profilePtr->DetailLevelSettings =			MenuDetailLevelOptions;
	profilePtr->SmackerSoundVolume =			SmackerSoundVolume;	
	profilePtr->EffectsSoundVolume =			EffectsSoundVolume;
	profilePtr->CDPlayerVolume = 				CDPlayerVolume;
	profilePtr->MoviesAreActive =				MoviesAreActive;
	profilePtr->IntroOutroMoviesAreActive =		IntroOutroMoviesAreActive;
	profilePtr->AutoWeaponChangeDisabled=   !AutoWeaponChangeOn;
   	strncpy(profilePtr->MultiplayerCallsign,MP_PlayerName,15);
}

extern void FixCheatModesInUserProfile(AVP_USER_PROFILE *profilePtr)
{
	int a;

	for (a=0; a<MAX_NUMBER_OF_CHEATMODES; a++) {
		if (profilePtr->CheatMode[a]==2) {
			profilePtr->CheatMode[a]=1;
		}
	}

}

}; // extern "C"