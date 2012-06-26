#include <string.h>

#define TRY_OLD_DIRS 0 // obsolete
#define ALLOW_LOAD_ORIGINAL 0

#if TRY_OLD_DIRS // temporary until all textures go into subshps directory
#include "ffstdio.h"
#endif

#include "chnkimag.hpp"

#include "list_tem.hpp"
#include "envchunk.hpp"
#include "chunkpal.hpp"
#include "bmpnames.hpp"

#include "chnkload.hpp"

char const * CL_RIFFImage::game_mode = 0;

// useful filename handling functions

// returns pointer into string pointing to filename without dirname
template <class C> // C can be char or char const
static C * strip_path(C * n)
{
	C * rm = strrchr(n,':');
	if (rm) n = rm+1;
	rm = strrchr(n,'/');
	if (rm) n = rm+1;
	rm = strrchr(n,'\\');
	if (rm) n = rm+1;

	return n;
}

// removes any .extension from filename by inserting null character
static void strip_file_extension(char * n)
{
	char * dotpos = strrchr(n,'.');
	if (dotpos) *dotpos = 0;
}

static char * strip_file_extension(char const * n)
{
	char * nn = new char[strlen(n)+1];
	strcpy(nn,n);
	strip_file_extension(nn);
	return nn;
}

////////////////////////
//
// riff file interface
//
////////////////////////


// get the directory associated with the riff - free with delete[]
static char * riff_basename(Chunk_With_Children * envd)
{
	RIF_Name_Chunk * rnc = 0;

	List<Chunk *> chlst = envd->lookup_child("RIFFNAME");

	if (chlst.size())
	{
		rnc = (RIF_Name_Chunk *)chlst.first_entry();
		const char * rif_name = strip_path(rnc->rif_name);

		char * basename = new char[strlen(rif_name)+1];
		strcpy(basename,rif_name);
		strip_file_extension(basename);

		return basename;
	}
	const char * deflt = "empty";
	char * basename = new char [strlen(deflt)+1];
	strcpy(basename,deflt);

	return basename;
}

#if OUTPUT_LOG
#define _LOGID	CL_LogFile.lprintf("%s:%d :: ",__FILE__,__LINE__)
#define _LOGPUT(s) _LOGID,CL_LogFile.lputs(s)
#define _LOGPRINT(args) _LOGID,CL_LogFile.lprintf args
#else
#define _LOGPUT(s) (void)0
#define _LOGPRINT(args) (void)0
#endif

void CL_RIFFImage::GetPath(ImageDescriptor const & idsc, Environment_Data_Chunk * envd, BMPN_Flags bflags)
{
	if (fname) delete[] fname;
	fname = 0; 
	
	// set the name
	if (name) delete[] name;
	char * nptr = strip_path(idsc.filename);
	name = new char[strlen(nptr)+1];
	strcpy(name,nptr);
	#if 0
	char orig_ext[32];
	char const * oeP = strrchr(name,'.');
	if (!oeP) eoP = "";
	strcpy(orig_ext,oeP);
	#endif
	strip_file_extension(name);
					
	// load this image
	char const * pg0ext = ".PG0";
	switch (imode)
	{
		case CLM_16BIT:
		case CLM_24BIT:
		case CLM_32BIT:
		#if ALLOW_LOAD_ORIGINAL
		{
			char const * dir2 = idsc.flags & IDSCF_SPRITE ? "Sprites\\" : "";
			char * riffname = riff_basename(envd);
			char const * dir3 = idsc.flags & IDSCF_INCLUDED ? idsc.rifname : riffname;
			fname = new char[strlen(ToolsTex_Directory)+strlen(dir2)+strlen(dir3)+1+strlen(name)+5];
			strcpy(fname,ToolsTex_Directory);
			strcat(fname,dir2);
			strcat(fname,dir3);
			strcat(fname,"\\");
			strcat(fname,name);
			strcat(fname,".PP0");
			delete[] riffname;
			break;
		}
		#endif
		case CLM_ATTACHEDPALETTE:
		{
			char const * dir2 = idsc.flags & IDSCF_SPRITE ? "Sprites\\" : idsc.flags & IDSCF_SUBSHAPE ? "SubShps\\All\\" : "";
			char * riffname = riff_basename(envd);
			char const * dir3 = idsc.flags & IDSCF_INCLUDED ? idsc.rifname : riffname;
			fname = new char[strlen(GenTex_Directory)+strlen(dir2)+strlen(dir3)+1+strlen(name)+5];
			strcpy(fname,GenTex_Directory);
			strcat(fname,dir2);
			strcat(fname,dir3);
			strcat(fname,"\\");
			strcat(fname,name);
			strcat(fname,".BM0");
			delete[] riffname;
			#if TRY_OLD_DIRS // temporary until all textures go into subshps directory
			FFILE * ftest = ffopen(fname,"rb");
			if (ftest) ffclose(ftest);
			else
			{
				_LOGPUT("WARNING! Not found in SubShps directory\n");
				char const * dir2 = idsc.flags & IDSCF_SPRITE ? "Sprites\\" : "";
				char * riffname = riff_basename(envd);
				char const * dir3 = idsc.flags & IDSCF_INCLUDED ? idsc.rifname : riffname;
				delete[] fname;
				fname = new char[strlen(GenTex_Directory)+strlen(dir2)+strlen(dir3)+1+strlen(name)+5];
				strcpy(fname,GenTex_Directory);
				strcat(fname,dir2);
				strcat(fname,dir3);
				strcat(fname,"\\");
				strcat(fname,name);
				strcat(fname,".BM0");
				delete[] riffname;
			}
			#endif
			break;
		}
		case CLM_TLTPALETTE:
			if (!(bflags & ChunkBMPFlag_NotLit))
			{
				pg0ext = ".PW0";
				flags.tltpalette = 1;
			}
		case CLM_GLOBALPALETTE:
		{
			if (idsc.flags & IDSCF_FIXEDPALETTE)
			{
				char const * dir2 = idsc.fixrifname ? *idsc.fixrifname ? idsc.fixrifname : 0 : 0;
				char const * dir3 = idsc.flags & IDSCF_SPRITE ? "Sprites\\" : "";
				char * riffname = riff_basename(envd);
				char const * dir4 = idsc.flags & IDSCF_INCLUDED ? idsc.rifname : riffname;
				fname = new char[strlen(FixTex_Directory)+(dir2 ? strlen(dir2)+1 : 0)+strlen(dir3)+strlen(dir4)+1+strlen(name)+5];
				strcpy(fname,FixTex_Directory);
				if (dir2)
				{
					strcat(fname,dir2);
					strcat(fname,"\\");
				}
				strcat(fname,dir3);
				strcat(fname,dir4);
				strcat(fname,"\\");
				strcat(fname,name);
				strcat(fname,pg0ext);
				delete[] riffname;
			}
			else
			{
				char const * dir1 = game_mode ? GameTex_Directory : ToolsTex_Directory;
				char * dir2 = riff_basename(envd);
				char const * dir4 = idsc.flags & IDSCF_SPRITE ? "Sprites\\" : "";
				char const * dir5 = idsc.flags & IDSCF_INCLUDED ? idsc.rifname : 0;
				fname = new char[strlen(dir1)+strlen(dir2)+1+(game_mode ? strlen(game_mode)+1 : 0)+strlen(dir4)+(dir5 ? strlen(dir5)+1 : 0)+strlen(name)+5];
				strcpy(fname,dir1);
				strcat(fname,dir2);
				strcat(fname,"\\");
				if (game_mode)
				{
					strcat(fname,game_mode);
					strcat(fname,"\\");
				}
				strcat(fname,dir4);
				if (dir5)
				{
					strcat(fname,dir5);
					strcat(fname,"\\");
				}
				strcat(fname,name);
				strcat(fname,pg0ext);
				delete[] dir2;
			}
			break;
		}
	}

	if (!fname)
	{
		_LOGPUT("WARNING! GetPath returning NULL pointer\n");
	}
	else
		_LOGPRINT(("file expected to be %s\n",fname));
}


CL_Error CL_RIFFImage::Locate(char const * iname, int const enum_id)
{
	_LOGPRINT(("RIF File image finder called for %s, id %d\n",iname,enum_id));
	
	if (!Env_Chunk)
		_LOGPUT("WARNING! no .RIF file loaded\n");
	
	if (!Env_Chunk) return CLE_RIFFERROR;

	switch (imode)
	{
		case CLM_ATTACHEDPALETTE:
		case CLM_16BIT:
		case CLM_24BIT:
		case CLM_32BIT:
		case CLM_GLOBALPALETTE:
		case CLM_TLTPALETTE:
			break;
		default:
			_LOGPUT("WARNING! undefined video mode\n");
			return CLE_INVALIDDXMODE;
	}

	// remove projectsubdirectory from start of image name if it is there
	unsigned int const psdirlen = strlen(projectsubdirectory);
	if (!strncmp(projectsubdirectory,iname,psdirlen))
	iname += psdirlen;
	
	List<Chunk *> envdl (Env_Chunk->lookup_child("REBENVDT"));

	if (!envdl.size())
	{
		_LOGPUT("WARNING! no environment data chunk\n");
		return CLE_RIFFERROR;
	}

	Environment_Data_Chunk * envd = (Environment_Data_Chunk *) envdl.first_entry();

	CL_Error retval = CLE_OK;
	Environment_Game_Mode_Chunk * egmc = 0;
	if (game_mode)
	{
		if (*game_mode)
		{
			List<Chunk *> egmcl (envd->lookup_child("GAMEMODE"));

			for (LIF<Chunk *> egmci(&egmcl); !egmci.done(); egmci.next())
			{
				Environment_Game_Mode_Chunk * egmcm = (Environment_Game_Mode_Chunk *) egmci();
				if (egmcm->id_equals(game_mode))
				{
					egmc = egmcm;
					break;
				}
			}

			if (!egmc) retval = CLE_INVALIDGAMEMODE;
				// only returns this error if the game mode cannot be found *and* the image is not listed
		}
	}

	if (name)
	{
		delete[] name;
		name = 0;
	}
	if (iname) name = strip_file_extension(strip_path(iname));

	char * rcname = 0;
	if (iname)
	{
		if (strchr(iname,'\\'))
		{
			rcname = new char[strlen(iname)+1];
			strcpy(rcname,iname);
			*strchr(rcname,'\\')=0;
		}
		else if (strchr(iname,'/'))
		{
			rcname = new char[strlen(iname)+1];
			strcpy(rcname,iname);
			*strchr(rcname,'/')=0;
		}
	}

	if (egmc)
	{
		int shapefoundingm = rcname ? 0 : 1;
		// Get the matching image 'Processor' chunk
		List<Chunk *> micl = egmc->lookup_child("MATCHIMG");

		Matching_Images_Chunk * mic = 0;
		if (micl.size()) mic = (Matching_Images_Chunk *)micl.first_entry();
		
		List<Chunk *> rcl = egmc->lookup_child("RIFCHILD");

		for (LIF<Chunk *> rci(&rcl); !rci.done(); rci.next())
		{
			RIF_Child_Chunk * rcm = (RIF_Child_Chunk *) rci();

			if (rcname)
			{
				if (_stricmp(rcname,rcm->rifname) && (*rcname || *rcm->filename))
					continue;
				shapefoundingm = 1;
			}

			for (LIF<BMP_Flags> bmpfi(&rcm->bmps); !bmpfi.done(); bmpfi.next())
			{
				BMP_Flags bmpft(bmpfi());
				strip_file_extension(bmpft.filename);

				if (iname ? !_stricmp(name,strip_path(bmpft.filename)) : enum_id == bmpft.enum_id)
				{
					// select image descriptor
					ImageDescriptor const idsc
						(
							*rcm->filename ? 
								(IDscFlags)((bmpft.flags & ChunkBMPFlag_FixedPalette ?
									IDSCF_FIXEDPALETTE
								:
									IDSCF_0)
								|IDSCF_INCLUDED)
							:
								IDSCF_0,
							bmpfi().filename,
							*rcm->filename ? rcm->rifname : 0
						);
					ImageDescriptor const * p_idsc = &idsc;

					if (mic) p_idsc = &mic->GetLoadImage(idsc);
					else _LOGPRINT(("WARNING! no rule to find matching images in game mode %s\n",egmc->header->mode_identifier));

					// load this image
					GetPath(*p_idsc,envd,bmpft.flags);

					if (fname)
					{
						if (rcname)
						{
							delete[] rcname;
							rcname = 0;
						}
						flags.located = 1;
						return CLE_OK;
					}
				}
			}
		}
		
		List<Chunk *> ssc = egmc->lookup_child("SHBMPNAM");

		for (LIF<Chunk *> ssi(&ssc); !ssi.done(); ssi.next())
		{
			External_Shape_BMPs_Store_Chunk * ss = (External_Shape_BMPs_Store_Chunk *) ssi();

			if (rcname)
				if (_stricmp(rcname,ss->shapename) && *rcname)
					continue;

			for (LIF<BMP_Name> bmpfi(&ss->bmps); !bmpfi.done(); bmpfi.next())
			{
				BMP_Name bmpft(bmpfi());
				strip_file_extension(bmpft.filename);

				if (iname ? !_stricmp(name,strip_path(bmpft.filename)) : enum_id == bmpft.enum_id)
				{

					// select image descriptor
					ImageDescriptor const idsc
						(
							(IDscFlags)((bmpft.flags & ChunkBMPFlag_FixedPalette ?
								IDSCF_FIXEDPALETTE
							:
								IDSCF_0)
							|(ss->GetExtendedData()->flags & GBF_SPRITE ?
								IDSCF_SPRITE
							:
								IDSCF_SUBSHAPE)
							|IDSCF_INCLUDED),
							bmpfi().filename,
							ss->shapename,
							bmpft.flags & ChunkBMPFlag_FixedPalette ? ss->rifname : 0
						);
					ImageDescriptor const * p_idsc = &idsc;

					if (mic) p_idsc = &mic->GetLoadImage(idsc);
					else _LOGPRINT(("WARNING! no rule to find matching images in game mode %s\n",egmc->header->mode_identifier));

					#if TRY_OLD_DIRS // temporary until all textures move to SubShps/All directory
					if (*p_idsc == idsc)
					{
						// select image descriptor
						ImageDescriptor const idsc2
							(
								(IDscFlags)((bmpft.flags & ChunkBMPFlag_FixedPalette ?
									IDSCF_FIXEDPALETTE
								:
									IDSCF_0)
								|(ss->GetExtendedData()->flags & GBF_SPRITE ?
									IDSCF_SPRITE
								:
									IDSCF_0)
								|IDSCF_INCLUDED),
								bmpfi().filename,
								ss->shapename,
								bmpft.flags & ChunkBMPFlag_FixedPalette ? ss->rifname : 0
							);
						ImageDescriptor const * p_idsc2 = &idsc2;

						if (mic) p_idsc2 = &mic->GetLoadImage(idsc2);
						else _LOGPRINT(("WARNING! no rule to find matching images in game mode %s\n",egmc->header->mode_identifier));

						if (*p_idsc2 != idsc2)
						{
							_LOGPUT("WARNING! Not listed as in SubShps directory\n");
							p_idsc = p_idsc2;
						}
					}
					#endif

					// load this image
					GetPath(*p_idsc,envd,bmpft.flags);
					
					if (fname)
					{
						if (rcname)
						{
							delete[] rcname;
							rcname = 0;
						}
						flags.located = 1;
						return CLE_OK;
					}
				}
			}
		}
		
		if (rcname)
		{
			if (!shapefoundingm)
				_LOGPRINT(("WARNING! shape/sprite %s not found in this RIF file\n",rcname));
			else
				_LOGPRINT(("WARNING! shape/sprite %s does not appear to list %s\n",rcname,name));
		}

	}

	List<Chunk *> micl = envd->lookup_child("MATCHIMG");
	
	Matching_Images_Chunk * mic_fix = 0;
	Matching_Images_Chunk * mic_nrm = 0;
	
	for (LIF<Chunk *> mici(&micl); !mici.done(); mici.next())
	{
		Matching_Images_Chunk * mic = (Matching_Images_Chunk *)mici();
		if (mic->flags & MICF_FIXEDPALETTE)
			mic_fix = mic;
		else
			mic_nrm = mic;
	}
		
	List<Chunk_With_Children *> shapesandsprites;
	
	List<Chunk *> shlst = Env_Chunk->lookup_child("REBSHAPE");
	for (LIF<Chunk *> shLIF(&shlst); !shLIF.done(); shLIF.next())
	{	
		List<Chunk *> shxflst = ((Shape_Chunk *)shLIF())->lookup_child("SHPEXTFL");
		if (shxflst.size())
		{
			shapesandsprites.add_entry( (Shape_External_File_Chunk *)shxflst.first_entry() );
		}
	}
	shlst = Env_Chunk->lookup_child("RSPRITES");
	if (shlst.size())
	{
		List<Chunk *> splst = ((Chunk_With_Children *)shlst.first_entry())->lookup_child("SPRIHEAD");

		for (LIF<Chunk *> spLIF(&splst); !spLIF.done(); spLIF.next())
		{	
			shapesandsprites.add_entry( (Chunk_With_Children *)spLIF() );
		}
	}

	int shapefound = rcname ? 0 : 1;
	
	for (LIF<Chunk_With_Children *> sasLIF(&shapesandsprites); !sasLIF.done(); sasLIF.next())
	{	
		char * subrifname = riff_basename(sasLIF());

		if (rcname)
		{
			if (_stricmp(subrifname,rcname)) // must match shapes name exactly
			{
				delete[] subrifname;
				continue;
			}
			shapefound = 1;
		}
		
		List<Chunk *> blsclst = sasLIF()->lookup_child("BMPLSTST");
		if (blsclst.size())
		{
			Bitmap_List_Store_Chunk * gbnc = (Bitmap_List_Store_Chunk *) blsclst.first_entry();

			for (LIF<BMP_Name> bmpni(&gbnc->bmps); !bmpni.done(); bmpni.next())
			{
				BMP_Name bmpnt(bmpni());
				strip_file_extension(bmpnt.filename);

				if (iname ? !_stricmp(name,strip_path(bmpnt.filename)) : enum_id == bmpnt.enum_id)
				{
				
					// select image descriptor
					char * riffname = riff_basename(envd);
					ImageDescriptor const idsc
						(
							(IDscFlags)((bmpnt.flags & ChunkBMPFlag_FixedPalette ?
								IDSCF_FIXEDPALETTE
							:
								IDSCF_0)
							|(gbnc->GetExtendedData()->flags & GBF_SPRITE ?
								IDSCF_SPRITE
							:
								IDSCF_SUBSHAPE)
							|IDSCF_INCLUDED),
							bmpni().filename,
							subrifname,
							bmpnt.flags & ChunkBMPFlag_FixedPalette ? riffname : 0
						);
					ImageDescriptor const * p_idsc = &idsc;
					delete[] riffname;

					if (bmpnt.flags & ChunkBMPFlag_FixedPalette)
					{
						if (mic_fix) p_idsc = &mic_fix->GetLoadImage(idsc);
						else _LOGPUT("WARNING! no rule to find fixed palette matching images in environment data\n");
					}
					else
					{
						if (mic_nrm) p_idsc = &mic_nrm->GetLoadImage(idsc);
						else _LOGPUT("WARNING! no rule to find matching images in environment data (interface engine?)\n");
					}

					#if TRY_OLD_DIRS // temporary until all textures move to SubShps/All directory
					if (*p_idsc == idsc)
					{
						// select image descriptor
						char * riffname = riff_basename(envd);
						ImageDescriptor const idsc2
							(
								(IDscFlags)((bmpnt.flags & ChunkBMPFlag_FixedPalette ?
									IDSCF_FIXEDPALETTE
								:
									IDSCF_0)
								|(gbnc->GetExtendedData()->flags & GBF_SPRITE ?
									IDSCF_SPRITE
								:
									IDSCF_0)
								|IDSCF_INCLUDED),
								bmpni().filename,
								subrifname,
								bmpnt.flags & ChunkBMPFlag_FixedPalette ? riffname : 0
							);
						ImageDescriptor const * p_idsc2 = &idsc2;
						delete[] riffname;

						if (bmpnt.flags & ChunkBMPFlag_FixedPalette)
						{
							if (mic_fix) p_idsc2 = &mic_fix->GetLoadImage(idsc2);
							else _LOGPUT("WARNING! no rule to find fixed palette matching images in environment data\n");
						}
						else
						{
							if (mic_nrm) p_idsc2 = &mic_nrm->GetLoadImage(idsc2);
							else _LOGPUT("WARNING! no rule to find matching images in environment data (interface engine?)\n");
						}
						if (*p_idsc2 != idsc2)
						{
							_LOGPUT("WARNING! Not listed as in SubShps directory\n");
							p_idsc = p_idsc2;
						}
					}
					#endif

					// load this image
					GetPath(*p_idsc,envd,bmpnt.flags);
					
					if (fname)
					{
						delete[] subrifname;
						if (rcname)
						{
							delete[] rcname;
							rcname = 0;
						}
						flags.located = 1;
						return CLE_OK;
					}
				}
			}
		}
		delete[] subrifname;
	}

	if (rcname)
	{
		if (!shapefound)
			_LOGPRINT(("WARNING! shape/sprite %s not found in this RIF file\n",rcname));
		else
			_LOGPRINT(("WARNING! shape/sprite %s does not appear to list %s\n",rcname,name));
		delete[] rcname;
		rcname = 0;
	}
	
	// not found in game textures, so look in default
	
	else // but only if there is no virtual shape directory
	{
		List<Chunk *> gbncl = envd->lookup_child("BMPNAMES");
		if (gbncl.size())
		{
			Global_BMP_Name_Chunk * gbnc = (Global_BMP_Name_Chunk *) gbncl.first_entry();

			for (LIF<BMP_Name> bmpni(&gbnc->bmps); !bmpni.done(); bmpni.next())
			{
				BMP_Name bmpnt(bmpni());
				strip_file_extension(bmpnt.filename);

				if (iname ? !_stricmp(name,strip_path(bmpnt.filename)) : enum_id == bmpnt.enum_id)
				{
					// select image descriptor
					ImageDescriptor const idsc (bmpnt.flags & ChunkBMPFlag_FixedPalette ? IDSCF_FIXEDPALETTE : IDSCF_0, bmpni().filename);
					ImageDescriptor const * p_idsc = &idsc;

					if (bmpnt.flags & ChunkBMPFlag_FixedPalette)
					{
						if (mic_fix) p_idsc = &mic_fix->GetLoadImage(idsc);
						else _LOGPUT("WARNING! no rule to find fixed palette matching images in environment data\n");
					}
					else
					{
						if (mic_nrm) p_idsc = &mic_nrm->GetLoadImage(idsc);
						else _LOGPUT("WARNING! no rule to find matching images in environment data (interface engine?)\n");
					}

					// load this image
					GetPath(*p_idsc,envd,bmpnt.flags);
					
					if (fname)
					{
						flags.located = 1;
						return CLE_OK;
					}
				}
			}
		}
	}

	if (retval != CLE_OK) return retval;
	return CLE_FINDERROR;
}



