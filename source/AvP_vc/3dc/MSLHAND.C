#include "3dc.h"

#include "mslhand.h"

#if PSX
#include "psx_inc.h"
#endif

#define UseLocalAssert 1
#include "ourasert.h"

extern SHAPEHEADER * mainshapelist[];

static int begins[ MSLS_MAXSECTIONS ];
static int ends[ MSLS_MAXSECTIONS ];

static int autodelete[ maxobjects ];

#define MSS_FREE (-1)

static void EnsureInitialized(void)
{
	static int initialized = 0;

	if (!initialized)
	{
		int i;

		initialized = 1;
		
		for (i=0; i<MSLS_MAXSECTIONS; ++i)
		{
			begins[i]=MSS_FREE;
			ends[i]=MSS_FREE;
		}
	}
}

int GetNextMSLEntry(MSL_SECTION sect, unsigned int num_shapes, int auto_delete)
{
	int i, blockstart, maxend = maxobjects;
	/* I assume that the size of the global array 'mainshapelist' will be 'maxobjects' */

	EnsureInitialized();
	
	for (i=sect+1; i<MSLS_MAXSECTIONS; ++i)
	{
		if (MSS_FREE != begins[i])
		{
			maxend = begins[i];
			break;
		}
	}

	for (i=sect-1; MSS_FREE == begins[sect]; --i)
	{
		ends[sect] = begins[sect] = i>=0 ? ends[i] : 0;
	}

	blockstart = ends[sect];
	/* can use a local assert here to fail on a mainshapelist overflow, otherwise return error code */
	LOCALASSERT(ends[sect] + num_shapes <= maxend);
	if (ends[sect] + num_shapes > maxend ) return MSL_OVERFLOW;
	ends[sect] += num_shapes;

	for (i=blockstart; i<ends[sect]; ++i)
	{
		mainshapelist[i] = 0;
		autodelete[i] = auto_delete;
	}

	return blockstart;
}

void FlushMSLSection(MSL_SECTION sect)
{

#if StandardShapeLanguage

	int i;
	
	EnsureInitialized();

	if (MSS_FREE == begins[sect] || MSS_FREE == ends[sect]) return;

	for (i=begins[sect]; i<ends[sect]; ++i)
	{
		if (autodelete[i] && mainshapelist[i])
		{
			SHAPEHEADER * shp = mainshapelist[i];

			int max_num_texs = 0;
			int i;

			if (shp->points)
			{
				if (*shp->points) free(*shp->points);
				free(shp->points);
			}
			if (shp->sh_normals)
			{
				if (*shp->sh_normals) free(*shp->sh_normals);
				free(shp->sh_normals);
			}
			if (shp->sh_vnormals)
			{
				if (*shp->sh_vnormals) free(*shp->sh_vnormals);
				free(shp->sh_vnormals);
			}
			if (shp->sh_extraitemdata)
				free(shp->sh_extraitemdata);
			/* the items are allocated in one big bunch
			// 9 int's per item (to make bsp simple)
			// this should be changed if it is to be done 
			// a different way
			*/
			if (shp->items)
			{
				for (i=0; i<shp->numitems; i++)
				{
					if (shp->items[i][0] == 5 || shp->items[i][0] == 6)
					{
						int UVIndex =  (shp->items[i][3] &0xffff0000) >> 16;
						max_num_texs = max (max_num_texs, shp->items[i][3] &0x7fff);
						if(shp->items[i][2]& iflag_txanim)
						{
							int j;
							TXANIMHEADER** thlist=(TXANIMHEADER**)shp->sh_textures[UVIndex];
							for(j=1;thlist[j]!=0;j++)
							{
								int k;
								TXANIMHEADER* th=thlist[j];
								for(k=0;k<th->txa_numframes;k++)
								{
									free(th->txa_framedata[k].txf_uvdata);
								}
								free (th->txa_framedata);
								free (th);
							}
							free (thlist);
							shp->sh_textures[UVIndex]=0;
						}
						else
						{
							free(shp->sh_textures[UVIndex]);
						}
					}
				}

				free (*shp->items);
				free (shp->items);
			}

			if (shp->sh_textures)
			{
				free (shp->sh_textures);
			}

			if (shp->sh_localtextures)
			{
				for (i=0; i<(max_num_texs+1); i++)
				{
					free (shp->sh_localtextures[i]);
				}
				free (shp->sh_localtextures);
			}

			if (shp->sh_instruction)
				free(shp->sh_instruction);

			free(shp);
		}
		mainshapelist[i]=0;
		autodelete[i]=0;
	}

	begins[sect]=MSS_FREE;
	ends[sect]=MSS_FREE;

#else 

printf("\nDo NOT call this function!\n");
//GLOBALASSERT(1==0);

#endif

}
