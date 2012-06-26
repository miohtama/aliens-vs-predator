#include "sprchunk.hpp"
#include "mishchnk.hpp"

#if cencon
#include "fnamefnc.hpp"
#include "bmpnames.hpp"
#include "shpchunk.hpp"
#endif

#ifdef cencon
#define new my_new
#endif
//macro for helping to force inclusion of chunks when using libraries
FORCE_CHUNK_INCLUDE_IMPLEMENT(sprchunk)

extern Chunk* Parent_File;

RIF_IMPLEMENT_DYNCREATE("SPRACTIO",Sprite_Action_Chunk)

Sprite_Action_Chunk::Sprite_Action_Chunk(Chunk_With_Children* parent,const char* data,size_t /*datasize*/)
: Chunk(parent, "SPRACTIO")
{
	Action=*((int*)data);
	data+=4;
	NumYaw=*((int*)data);
	data+=4;
	NumPitch=*((int*)data);
	data+=4;
	NumFrames=*((int*)data);
	data+=4;
	Flags=*((int*)data);
	data+=4;
	FrameTime=*((int*)data);
	data+=4;
	FrameList=new Frame**[NumYaw];
	for(int i=0;i<NumYaw;i++)
	{
		FrameList[i]=new Frame*[NumPitch];
		for(int j=0;j<NumPitch;j++)
		{
			FrameList[i][j]=new Frame[NumFrames];
			for(int k=0;k<NumFrames;k++)
			{
				Frame* f=&FrameList[i][j][k];
				f->Texture=*((int*)data);
				data+=4;
				f->CentreX=*((int*)data);
				data+=4;
				f->CentreY=*((int*)data);
				data+=4;
				for(int l=0;l<4;l++)
				{
					f->UVCoords[l][0]=*((int*)data);		
					data+=4;
					f->UVCoords[l][1]=*((int*)data);		
					data+=4;
				}
			}
		}
	}
}
Sprite_Action_Chunk::Sprite_Action_Chunk(Chunk_With_Children* parent)
: Chunk(parent, "SPRACTIO")
{
	Action=-1;
	NumPitch=0;
	NumYaw=0;
	NumFrames=0;
	FrameList=0;
   	Flags=0;
	FrameTime=200;
}
size_t Sprite_Action_Chunk::size_chunk()
{
	chunk_size=36+NumFrames*NumPitch*NumYaw*44;
	return chunk_size;	
}

Sprite_Action_Chunk::~Sprite_Action_Chunk()
{
	for(int i=0;i<NumYaw;i++)
	{
		for(int j=0;j<NumPitch;j++)
		{
			delete [] FrameList[i][j];
		}
		delete[]  FrameList[i];
	}
	delete [] FrameList;
}


BOOL Sprite_Action_Chunk::output_chunk (HANDLE &hand)
{
	unsigned long junk;
	BOOL ok;
	char * data_block;

	data_block = this->make_data_block_from_chunk();

	ok = WriteFile (hand, (long *) data_block, (unsigned long) chunk_size, &junk, 0);

	delete [] data_block;

	if (!ok) return FALSE;

	return TRUE;
}

void Sprite_Action_Chunk::fill_data_block(char* data_start)
{
	strncpy (data_start, identifier, 8);

	data_start += 8;

	*((int *) data_start) = chunk_size;

	data_start += 4;
	
	*(int*)data_start=Action;
	data_start += 4;
	*(int*)data_start=NumYaw;
	data_start += 4;
	*(int*)data_start=NumPitch;
	data_start += 4;
	*(int*)data_start=NumFrames;
	data_start += 4;
	*(int*)data_start=Flags;
	data_start += 4;
	*(int*)data_start=FrameTime;
	data_start += 4; 
	
	for(int i=0;i<NumYaw;i++)
	{
		for(int j=0;j<NumPitch;j++)
		{
			for(int k=0;k<NumFrames;k++)
			{
				Frame* f=&FrameList[i][j][k];
				*(int*)data_start=f->Texture;
				data_start += 4;
				*(int*)data_start=f->CentreX;
				data_start +=4;
				*(int*)data_start=f->CentreY;
				data_start +=4;
				for(int l=0;l<4;l++)
				{
					*(int*)data_start=f->UVCoords[l][0];
					data_start+=4;
					*(int*)data_start=f->UVCoords[l][1];
					data_start+=4;
				}
   			}
		}
	}
}

//////////////////////////////////////////////////
//Class Sprite_Header_Chunk
RIF_IMPLEMENT_DYNCREATE("SPRIHEAD",Sprite_Header_Chunk)
CHUNK_WITH_CHILDREN_LOADER("SPRIHEAD",Sprite_Header_Chunk)

/*
Children for Sprite_Header_Chunk :

"SPRITVER"		Sprite_Version_Number_Chunk 
"SPRITEPC"		PC_Sprite_Chunk 
"SPRITEPS"		Playstation_Sprite_Chunk 
"SPRITESA"		Saturn_Sprite_Chunk 
"BMPLSTST"		Bitmap_List_Store_Chunk 
"BMNAMVER"		BMP_Names_Version_Chunk 
"BMNAMEXT"		BMP_Names_ExtraData_Chunk 
"RIFFNAME"		RIF_Name_Chunk 
"SHPEXTFN"		Shape_External_Filename_Chunk 
"SPRISIZE"		Sprite_Size_Chunk 
"BMPMD5ID"		Bitmap_MD5_Chunk 
"SPRBMPSC"		Sprite_Bitmap_Scale_Chunk 
"SPRBMPCE"		Sprite_Bitmap_Centre_Chunk 
"SPREXTEN"		Sprite_Extent_Chunk 
*/


Sprite_Header_Chunk::Sprite_Header_Chunk(const char * file_name, Chunk_With_Children * parent)
: Chunk_With_Children(parent,"SPRIHEAD")
{
// Load in whole chunk and traverse
	HANDLE rif_file;
	DWORD file_size;
	DWORD file_size_from_file;
	unsigned long bytes_read;
	char * buffer;
	char * buffer_ptr;
	char id_buffer[9];

	Parent_File = this;

	error_code = 0;


	rif_file = CreateFileA (file_name, GENERIC_READ, 0, 0, OPEN_EXISTING, 
					FILE_FLAG_RANDOM_ACCESS, 0);

	if (rif_file == INVALID_HANDLE_VALUE) {
		return;
	}

	file_size = GetFileSize (rif_file, NULL);	

	
	if (!ReadFile(rif_file, id_buffer, 8, &bytes_read, 0)) {
		error_code = CHUNK_FAILED_ON_LOAD;
		CloseHandle (rif_file);
		return;
	}	

	if (strncmp (id_buffer, "SPRIHEAD", 8)) {
		error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
		CloseHandle (rif_file);
		return;
	}	

	if (!ReadFile(rif_file, &file_size_from_file, 4, &bytes_read, 0)) {
		error_code = CHUNK_FAILED_ON_LOAD;
		CloseHandle (rif_file);
		return;
	}	

	if (file_size != file_size_from_file) {
		error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
		CloseHandle (rif_file);
		return;
	}	


	buffer = new char [file_size];

	if (!ReadFile(rif_file, buffer, (file_size-12), &bytes_read, 0)) {
		error_code = CHUNK_FAILED_ON_LOAD;
		CloseHandle (rif_file);
		return;
	}

	// Process the file

	buffer_ptr = buffer;

	
	// The start of the first chunk

	while ((buffer_ptr-buffer)< ((signed) file_size-12) && !error_code) {

		if ((*(int *)(buffer_ptr + 8)) + (buffer_ptr-buffer) > ((signed) file_size-12)) {
			error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
			break;
		}

		DynCreate(buffer_ptr);
		buffer_ptr += *(int *)(buffer_ptr + 8);

	}

	delete [] buffer;

	CloseHandle (rif_file);

}


Sprite_Header_Chunk::write_file(const char* fname)
{
	HANDLE rif_file;

	rif_file = CreateFileA (fname, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 
					FILE_FLAG_RANDOM_ACCESS, 0);

	if (rif_file == INVALID_HANDLE_VALUE) {
		return CHUNK_FAILED_ON_WRITE;
	}

	size_chunk();

	if (!(this->output_chunk(rif_file)))
		return CHUNK_FAILED_ON_WRITE;

	CloseHandle (rif_file);

	return 0;
}

Sprite_Header_Chunk::output_chunk(HANDLE & hand)
{
	unsigned long junk;
	BOOL ok;
	char * data_block;

	data_block = this->make_data_block_from_chunk();

	ok = WriteFile (hand, (long *) data_block, (unsigned long) chunk_size, &junk, 0);

	delete [] data_block;

	if (!ok) return FALSE;

	return TRUE;
}

#if cencon
void Sprite_Header_Chunk::post_input_processing()
{
	if (parent)
	{
		List<Chunk *> chlst;
		GetRootChunk()->lookup_child("REBENVDT",chlst);
		Lockable_Chunk_With_Children * envd = 0;
		if (chlst.size()) envd = (Lockable_Chunk_With_Children *) chlst.first_entry();
		
		BOOL fixpal = IsFixedPalette(envd);

		lookup_child("SHPEXTFN",chlst);
		
		if (chlst.size())
		{
			Shape_External_Filename_Chunk * sefc = (Shape_External_Filename_Chunk *)chlst.first_entry();
			
			#if cencon
			twprintf("Locating %s\n",sefc->file_name);
			char * locatedfile = FindExistingFileInPath_PreferWriteable(CWnd::GetActiveWindow(),sefc->file_name,"RifSearchPath");
			twprintf("Loading %s\n",locatedfile ? locatedfile : sefc->file_name);
			Sprite_Header_Chunk rfc(locatedfile ? locatedfile : sefc->file_name);
			if (locatedfile)
			{
				delete[] locatedfile;
			}
			#else
			Sprite_Header_Chunk rfc(sefc->file_name);
			#endif

			if (rfc.error_code != 0)
			{
				return;
			}

			lookup_child("SPRITVER",chlst);
			int myver = -2;
			if (chlst.size())
			{
				Sprite_Version_Number_Chunk * svnc = (Sprite_Version_Number_Chunk *)chlst.first_entry();
				myver = svnc->version_num;
			}
			rfc.lookup_child("SPRITVER",chlst);
			int yourver = -1;
			if (chlst.size())
			{
				Sprite_Version_Number_Chunk * svnc = (Sprite_Version_Number_Chunk *)chlst.first_entry();
				yourver = svnc->version_num;
			}

			if (yourver != myver)
			{
				#define NOT_A_BITMAP_RELATED_CHUNK \
					strncmp ("BMPLSTST", child_ptr->identifier, 8) && \
					strncmp ("BMNAMVER", child_ptr->identifier, 8) && \
					strncmp ("BMNAMEXT", child_ptr->identifier, 8) && \
					strncmp ("RIFFNAME", child_ptr->identifier, 8) && \
					strncmp ("SHPEXTFN", child_ptr->identifier, 8) && \
					strncmp ("BMPMD5ID", child_ptr->identifier, 8)
				
				Chunk * child_ptr = children;
				List <Chunk *> chunks_to_delete;
				while (child_ptr != NULL)
				{
					if (NOT_A_BITMAP_RELATED_CHUNK)
					{
						chunks_to_delete.add_entry(child_ptr);
					}
					child_ptr = child_ptr->next;
				}
				for (LIF<Chunk *> delchunks(&chunks_to_delete); !delchunks.done(); delchunks.next())
				{
					delete delchunks();
				}
				
				child_ptr = rfc.children;
				while (child_ptr != NULL)
				{
					if (NOT_A_BITMAP_RELATED_CHUNK)
					{
						#define CREATE_CHUNK_END(_datablock) \
							new Miscellaneous_Chunk(this,_datablock,_datablock+12,*(int *)(_datablock+8)-12);

						#define CREATE_CHUNK_FOR(_datablock,_id,_chunkclass) \
							if (!strncmp(_datablock,_id,8)) { \
								new _chunkclass(this,_datablock+12,*(int *)(_datablock+8)-12); \
							} else

						child_ptr->prepare_for_output();
						char * datablock = child_ptr->make_data_block_from_chunk();

						CREATE_CHUNK_FOR(datablock,"SPRITVER",Sprite_Version_Number_Chunk)
						CREATE_CHUNK_FOR(datablock,"SPRITEPC",PC_Sprite_Chunk)
						CREATE_CHUNK_FOR(datablock,"SPRITEPS",Playstation_Sprite_Chunk)
						CREATE_CHUNK_FOR(datablock,"SPRITESA",Saturn_Sprite_Chunk)
						CREATE_CHUNK_FOR(datablock,"SPRISIZE",Sprite_Size_Chunk)
						CREATE_CHUNK_FOR(datablock,"SPRBMPSC",Sprite_Bitmap_Scale_Chunk)
						CREATE_CHUNK_FOR(datablock,"SPRBMPCE",Sprite_Bitmap_Centre_Chunk)
						CREATE_CHUNK_FOR(datablock,"SPREXTEN",Sprite_Extent_Chunk)
						CREATE_CHUNK_END(datablock)

						delete[] datablock;
					}
					child_ptr = child_ptr->next;
				}
				
			}

			Chunk_With_BMPs * blsc = 0;
			Chunk_With_BMPs * gbnc = 0;

			rfc.lookup_child("BMPLSTST",chlst);
			if (chlst.size())
			{
				gbnc = (Chunk_With_BMPs *) chlst.first_entry();
				if (!gbnc->bmps.size()) gbnc = 0;
			}
			
			List<Chunk *> oldlst;
			lookup_child("BMPLSTST",oldlst);
			assert (oldlst.size()<2);
			
			if (oldlst.size())
			{
				blsc = (Bitmap_List_Store_Chunk *)oldlst.first_entry();
			}
			else
			{
				if (gbnc) blsc = new Bitmap_List_Store_Chunk(this);
			}

			BMP_Names_ExtraData * extended = 0;
			if (blsc)
			{
				extended = blsc->GetExtendedData();
				if (fixpal)
					extended->flags = (GlobalBMPFlags)(extended->flags | GBF_FIXEDPALETTE);
				else
					extended->flags = (GlobalBMPFlags)(extended->flags & ~GBF_FIXEDPALETTE);
			}
			if (gbnc)
			{
				if ((gbnc->get_version_num()!=blsc->get_version_num()) || (gbnc->bmps.size() != blsc->bmps.size()))
				{  // other checks could be done as well
					if (blsc->bmps.size())
					{
						BOOL neednewpalette = FALSE;
						
						List<BMP_Name> newlist = gbnc->bmps;
						for (LIF<BMP_Name> newLIF(&newlist); !newLIF.done(); newLIF.next())
						{
							BMP_Name newcur = newLIF();
							newcur.flags = (BMPN_Flags) (newcur.flags & ~(COMPLETED_BMPN_FLAGS | ChunkBMPFlag_FixedPalette));
							if (fixpal) newcur.flags = (BMPN_Flags) (newcur.flags | ChunkBMPFlag_FixedPalette);
							for (LIF<BMP_Name> oldLIF(&blsc->bmps); !oldLIF.done(); oldLIF.next())
							{
								BMP_Name oldcur = oldLIF();
								if (newcur == oldcur)
								{
									// do we need to requantize?
									if ((oldcur.flags ^ newcur.flags) & CHECKMODIFY_BMPN_FLAGS
										|| newcur.flags & ChunkBMPFlag_UsesTransparency
										   && !(newcur.flags & ChunkBMPFlag_IFF)
										   && !(oldcur.flags & ChunkBMPFlag_IFF)
										   && oldcur.DifferentTransparencyColour(newcur))
										oldcur.flags = (BMPN_Flags)(oldcur.flags & ~COMPLETED_BMPN_FLAGS);
									// keep some of the old flags - the ones that can differ
									newcur.flags = (BMPN_Flags)(newcur.flags & COPY_BMPN_FLAGS);
									newcur.flags = (BMPN_Flags)(newcur.flags | oldcur.flags & ~COPY_BMPN_FLAGS);
									if (oldcur.version_num != newcur.version_num)
									{
										neednewpalette = TRUE;
										newcur.flags = (BMPN_Flags)(newcur.flags & ~ChunkBMPFlag_HistogramExists);
										extended->flags = (GlobalBMPFlags)(extended->flags & ~GBF_HISTOGRAMEXISTS);
									}
									break;
								}
							}
							if (oldLIF.done())
							{
							   	// reset palette up to date flag
								neednewpalette = TRUE;
								newcur.flags = (BMPN_Flags)(newcur.flags & ~(ChunkBMPFlag_HistogramExists | COMPLETED_BMPN_FLAGS));
								extended->flags = (GlobalBMPFlags)(extended->flags & ~GBF_HISTOGRAMEXISTS);
							}
							newLIF.change_current(newcur);
						}

						// check if any bitmaps have been removed
						for (LIF<BMP_Name> bli(&blsc->bmps); !bli.done(); bli.next())
						{
							if (!newlist.contains(bli()))
							{
								// delete assoc files
								neednewpalette = TRUE;
								extended->flags = (GlobalBMPFlags)(extended->flags & ~GBF_HISTOGRAMEXISTS);
							}
						}

						if (neednewpalette)
						{
							Palette_Outdated(envd);
							if (fixpal) FixedPalette_Outdated(envd);
							envd->updated = TRUE;
						}
						blsc->bmps = newlist;
					}
					else
					{
						blsc->bmps = gbnc->bmps;
						for (LIF<BMP_Name> flagresetLIF(&blsc->bmps); !flagresetLIF.done(); flagresetLIF.next())
						{
							BMP_Name current = flagresetLIF();
							current.flags = (BMPN_Flags)(current.flags & (COPY_BMPN_FLAGS & ~ChunkBMPFlag_FixedPalette));
							if (fixpal) current.flags = (BMPN_Flags) (current.flags | ChunkBMPFlag_FixedPalette);
							flagresetLIF.change_current(current);
						}
						// reset palette up to date flag
						extended->flags = (GlobalBMPFlags)(extended->flags & ~GBF_HISTOGRAMEXISTS);
						Palette_Outdated(envd);
						if (fixpal) FixedPalette_Outdated(envd);
						envd->updated = TRUE;
					}
					blsc->max_index = gbnc->max_index;
					blsc->set_version_num(gbnc->get_version_num());
					assert (!strcmp("RSPRITES",parent->identifier));
					((Lockable_Chunk_With_Children *)parent)->updated = TRUE;
				}
			}
			else
			{
				if (blsc)
				{
					if (blsc->bmps.size())
					{
						// reset palette up to date flag
						extended->flags = (GlobalBMPFlags)(extended->flags & ~GBF_HISTOGRAMEXISTS);
						Palette_Outdated(envd);
						if (fixpal) FixedPalette_Outdated(envd);
						envd->updated = TRUE;
						assert (!strcmp("RSPRITES",parent->identifier));
						((Lockable_Chunk_With_Children *)parent)->updated = TRUE;
					}
					delete blsc;
				}
			}
			
		}
		
	}
	Chunk_With_Children::post_input_processing();
}
#endif
RIF_IMPLEMENT_DYNCREATE_DECLARE_PARENT("SPRITEPC",PC_Sprite_Chunk,"SPRIHEAD",Sprite_Header_Chunk)

PC_Sprite_Chunk::PC_Sprite_Chunk(Sprite_Header_Chunk* parent,const char* data,size_t datasize)
:Chunk_With_Children(parent,"SPRITEPC")
{
	const char * buffer_ptr = data;

	
	while ((data-buffer_ptr)< (signed) datasize) {

		if ((*(int *)(data + 8)) + (data-buffer_ptr) > (signed) datasize) {
			Parent_File->error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
			break;
		}

		if (!strncmp(data, "SPRACTIO",8)) {
			new Sprite_Action_Chunk (this, (data + 12), (*(int *) (data + 8))-12);
			data += *(int *)(data + 8);
		}
		else {
			new Miscellaneous_Chunk (this, data, (data + 12), (*(int *) (data + 8)) -12 );
			data += *(int *)(data + 8);
		}

	}
}
/////////////////////////////////////////
RIF_IMPLEMENT_DYNCREATE_DECLARE_PARENT("SPRITESA",Saturn_Sprite_Chunk,"SPRIHEAD",Sprite_Header_Chunk)


Saturn_Sprite_Chunk::Saturn_Sprite_Chunk(Sprite_Header_Chunk* parent,const char* data,size_t datasize)
:Chunk_With_Children(parent,"SPRITESA")
{
	const char * buffer_ptr = data;

	
	while ((data-buffer_ptr)< (signed) datasize) {

		if ((*(int *)(data + 8)) + (data-buffer_ptr) > (signed) datasize) {
			Parent_File->error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
			break;
		}

		if (!strncmp(data, "SPRACTIO",8)) {
			new Sprite_Action_Chunk (this, (data + 12), (*(int *) (data + 8))-12);
			data += *(int *)(data + 8);
		}
		else {
			new Miscellaneous_Chunk (this, data, (data + 12), (*(int *) (data + 8)) -12 );
			data += *(int *)(data + 8);
		}

	}
}
/////////////////////////////////////////////
RIF_IMPLEMENT_DYNCREATE_DECLARE_PARENT("SPRITEPS",Playstation_Sprite_Chunk,"SPRIHEAD",Sprite_Header_Chunk)

Playstation_Sprite_Chunk::Playstation_Sprite_Chunk(Sprite_Header_Chunk* parent,const char* data,size_t datasize)
:Chunk_With_Children(parent,"SPRITEPS")
{
	const char * buffer_ptr = data;

	
	while ((data-buffer_ptr)< (signed) datasize) {

		if ((*(int *)(data + 8)) + (data-buffer_ptr) > (signed) datasize) {
			Parent_File->error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
			break;
		}

		if (!strncmp(data, "SPRACTIO",8)) {
			new Sprite_Action_Chunk (this, (data + 12), (*(int *) (data + 8))-12);
			data += *(int *)(data + 8);
		}
		else {
			new Miscellaneous_Chunk (this, data, (data + 12), (*(int *) (data + 8)) -12 );
			data += *(int *)(data + 8);
		}

	}
}



////////////////////////////////////////////////////////////
RIF_IMPLEMENT_DYNCREATE("SPRISIZE",Sprite_Size_Chunk)


Sprite_Size_Chunk::Sprite_Size_Chunk(Chunk_With_Children* parent,const char* data,size_t /*datasize*/)
: Chunk(parent, "SPRISIZE")
{
	scale=*(double*)data;
	data+=8;
	maxy=*(double*)data;
	data+=8;
	maxx=*(double*)data;
	data+=8;
	radius=*(int*)data;
	data+=4;
	Flags=*(int*)data;
	data+=4;
}

Sprite_Size_Chunk::Sprite_Size_Chunk(Chunk_With_Children* parent)
: Chunk(parent, "SPRISIZE")
{
	scale=15.625;
	maxy=1000;
	maxx=500;
	radius=0;
	Flags=0;
}

BOOL Sprite_Size_Chunk::output_chunk (HANDLE &hand)
{
	unsigned long junk;
	BOOL ok;
	char * data_block;

	data_block = this->make_data_block_from_chunk();

	ok = WriteFile (hand, (long *) data_block, (unsigned long) chunk_size, &junk, 0);

	delete [] data_block;

	if (!ok) return FALSE;

	return TRUE;
}

size_t Sprite_Size_Chunk::size_chunk()
{
	chunk_size=44;
	return chunk_size;	
}

void Sprite_Size_Chunk::fill_data_block(char* data_start)
{
	strncpy (data_start, identifier, 8);

	data_start += 8;

	*((int *) data_start) = chunk_size;

	data_start += 4;
	
	*(double*)data_start=scale;
	data_start+=8;
	*(double*)data_start=maxy;
	data_start+=8;
	*(double*)data_start=maxx;
	data_start+=8;
	*(int*)data_start=radius;
	data_start+=4;
	*(int*)data_start=Flags;
	data_start+=4;
}
///////////////////////////////////////////////
RIF_IMPLEMENT_DYNCREATE("SPRITVER",Sprite_Version_Number_Chunk)

Sprite_Version_Number_Chunk::Sprite_Version_Number_Chunk(Chunk_With_Children* parent,const char* data,size_t /*datasize*/)
: Chunk(parent, "SPRITVER")
{
	version_num=*(int*)data;
	data+=4;
}

Sprite_Version_Number_Chunk::Sprite_Version_Number_Chunk(Chunk_With_Children* parent)
: Chunk(parent, "SPRITVER")
{
	version_num=0;
}

BOOL Sprite_Version_Number_Chunk::output_chunk (HANDLE &hand)
{
	unsigned long junk;
	BOOL ok;
	char * data_block;

	data_block = this->make_data_block_from_chunk();

	ok = WriteFile (hand, (long *) data_block, (unsigned long) chunk_size, &junk, 0);

	delete [] data_block;

	if (!ok) return FALSE;

	return TRUE;
}

size_t Sprite_Version_Number_Chunk::size_chunk()
{
	chunk_size=16;
	return chunk_size;	
}

void Sprite_Version_Number_Chunk::fill_data_block(char* data_start)
{
	strncpy (data_start, identifier, 8);

	data_start += 8;

	*((int *) data_start) = chunk_size;

	data_start += 4;
	
	*(int*)data_start=version_num;
}


//////////////////////////////////////////////////////////
RIF_IMPLEMENT_DYNCREATE("SPRBMPSC",Sprite_Bitmap_Scale_Chunk)


Sprite_Bitmap_Scale_Chunk::Sprite_Bitmap_Scale_Chunk(Chunk_With_Children* parent,const char* data,size_t /*datasize*/)
: Chunk(parent, "SPRBMPSC")
{
	NumBitmaps=*(int*)data;
	data+=4;
	if(NumBitmaps)
	{
		Scale=new float[NumBitmaps];
		for(int i=0;i<NumBitmaps;i++)
		{
			Scale[i]=*(float*)data;
			data+=sizeof(float);
		}
	}
	else
		Scale=0;
}
Sprite_Bitmap_Scale_Chunk::Sprite_Bitmap_Scale_Chunk(Chunk_With_Children* parent)
: Chunk(parent, "SPRBMPSC")
{
	NumBitmaps=0;
	Scale=0;
}
Sprite_Bitmap_Scale_Chunk::~Sprite_Bitmap_Scale_Chunk()
{
	delete [] Scale;
}
size_t Sprite_Bitmap_Scale_Chunk::size_chunk()
{
	chunk_size=16+sizeof(float)*NumBitmaps;
	return chunk_size;	
}

void Sprite_Bitmap_Scale_Chunk::fill_data_block(char* data_start)
{
	strncpy (data_start, identifier, 8);

	data_start += 8;

	*((int *) data_start) = chunk_size;

	data_start += 4;
	
	*(int*)data_start=NumBitmaps;
	data_start+=4;
	for(int i=0;i<NumBitmaps;i++)
	{
		*(float*)data_start=Scale[i];
		data_start+=sizeof(float);
	}
}
/////////////////////////////////////////////////////
RIF_IMPLEMENT_DYNCREATE("SPRBMPCE",Sprite_Bitmap_Centre_Chunk)

Sprite_Bitmap_Centre_Chunk::Sprite_Bitmap_Centre_Chunk(Chunk_With_Children* parent,const char* data,size_t /*datasize*/)
: Chunk(parent, "SPRBMPCE")
{
	NumBitmaps=*(int*)data;
	data+=4;
	if(NumBitmaps)
	{
		CentreX=new int[NumBitmaps];
		CentreY=new int[NumBitmaps];
		OffsetX=new int[NumBitmaps];
		OffsetY=new int[NumBitmaps];

		for(int i=0;i<NumBitmaps;i++)
		{
			CentreX[i]=*(int*)data;
			data+=4;
			CentreY[i]=*(int*)data;
			data+=4;
			OffsetX[i]=*(int*)data;
			data+=4;
			OffsetY[i]=*(int*)data;
			data+=4;
		}
	}
	else
	{
		CentreX=0;
		CentreY=0;
		OffsetX=0;
		OffsetY=0;
	}
	spare=*(int*)data;
}
Sprite_Bitmap_Centre_Chunk::Sprite_Bitmap_Centre_Chunk(Chunk_With_Children* parent)
: Chunk(parent, "SPRBMPCE")
{
	NumBitmaps=0;
	CentreX=CentreY=0;
	OffsetX=OffsetY=0;
	spare=0;
}
Sprite_Bitmap_Centre_Chunk::~Sprite_Bitmap_Centre_Chunk()
{
	delete [] CentreX;
	delete [] CentreY;
	delete [] OffsetX;
	delete [] OffsetY;
}
size_t Sprite_Bitmap_Centre_Chunk::size_chunk()
{
	chunk_size=20+16*NumBitmaps;
	return chunk_size;	
}

void Sprite_Bitmap_Centre_Chunk::fill_data_block(char* data_start)
{
	strncpy (data_start, identifier, 8);

	data_start += 8;

	*((int *) data_start) = chunk_size;

	data_start += 4;
	
	*(int*)data_start=NumBitmaps;
	data_start+=4;
	for(int i=0;i<NumBitmaps;i++)
	{
		*(int*)data_start=CentreX[i];
		data_start+=4;
		*(int*)data_start=CentreY[i];
		data_start+=4;
		*(int*)data_start=OffsetX[i];
		data_start+=4;
		*(int*)data_start=OffsetY[i];
		data_start+=4;
	}
	*(int*)data_start=spare;
}
/////////////////////////////////////////////////////
RIF_IMPLEMENT_DYNCREATE("SPREXTEN",Sprite_Extent_Chunk)

Sprite_Extent_Chunk::Sprite_Extent_Chunk(Chunk_With_Children* parent,const char* data,size_t /*datasize*/)
: Chunk(parent, "SPREXTEN")
{
	minx=*(double*)data;
	data+=8;
	maxx=*(double*)data;
	data+=8;
	miny=*(double*)data;
	data+=8;
	maxy=*(double*)data;
	data+=8;
	spare1=*(int*)data;
	data+=4;
	spare2=*(int*)data;
	data+=4;
}
Sprite_Extent_Chunk::Sprite_Extent_Chunk(Chunk_With_Children* parent)
: Chunk(parent, "SPREXTEN")
{
	minx=miny=-1000;
	maxx=maxy=1000;
	spare1=spare2=0;
}
size_t Sprite_Extent_Chunk::size_chunk()
{
	chunk_size=12+40;
	return chunk_size;	
}

void Sprite_Extent_Chunk::fill_data_block(char* data_start)
{
	strncpy (data_start, identifier, 8);

	data_start += 8;

	*((int *) data_start) = chunk_size;

	data_start += 4;
	
	*(double*)data_start=minx;
	data_start+=8;	
	*(double*)data_start=maxx;
	data_start+=8;	
	*(double*)data_start=miny;
	data_start+=8;	
	*(double*)data_start=maxy;
	data_start+=8;	

	*(int*)data_start=spare1;
	data_start+=4;
	*(int*)data_start=spare2;
	data_start+=4;

}
