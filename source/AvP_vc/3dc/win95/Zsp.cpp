#include <math.h>

#if cencon
#include "AFXWIN.H"
#include "list_tem.hpp"

#include "Vectors.hpp"
#include "environs.hpp"
#include "shpchunk.hpp"
#elif shpedit
#include "list_tem.hpp"
#include "Vectors.hpp"
#else
#include "list_tem.hpp"
#endif

#include "zsp.hpp"

#ifdef cencon
#define new my_new
#endif

#if shpedit
#define twprintf printf
#endif

// ZSP creation functions

#if cencon
void generate_zsp_data (int num_cubes, CWnd * pWnd)
{
	Shape_Chunk * shpch = Sel_Obj->get_assoc_shape();

	if (!shpch->lock_chunk(*(Main_Env.environment)))
	{
		char message[300];
		char * locker = "Unknown";
		
		Shape_Header_Chunk * head = shpch->get_header();
		if (head)
			locker = head->lock_user;
		
		sprintf (message, "Shape locked by %s\n can not continue", locker);
		
		pWnd->MessageBox (message, "Tools Control Area", 
			MB_ICONHAND + MB_OK + MB_TASKMODAL);
		return;		
	}
	start_text_window(pWnd);	

	
	ZSP_Data * zsp = new ZSP_Data (shpch->shape_data, num_cubes);
	new Shape_ZSP_Data_Chunk (shpch, *zsp);

	shpch->unlock_chunk (*(Main_Env.environment), TRUE);
	stop_text_window();

	delete zsp;
}
#endif

#if cencon || shpedit

ZSP_Data::ZSP_Data (const ChunkShape & shp, int num_cubes)
{
	double xsize,ysize,zsize;
	double max_size;
	Vector cube_vector;
	int num_subshapes;
	
	xsize = fabs(shp.max.x) + fabs(shp.min.x);
	ysize = fabs(shp.max.y) + fabs(shp.min.y);
	zsize = fabs(shp.max.z) + fabs(shp.min.z);
	
	max_size=max(xsize,ysize);
	max_size=max(max_size,zsize);
	
	cube_size = max_size / num_cubes;
	
	int nc = num_cubes+2;
	
	num_x_cubes = nc-(int) ( nc-(xsize / cube_size) );
	num_y_cubes = nc-(int) ( nc-(ysize / cube_size) );
	num_z_cubes = nc-(int) ( nc-(zsize / cube_size) );
	
	cube_vector.x = cube_size / 2;
	cube_vector.y = cube_size / 2;
	cube_vector.z = cube_size / 2;
	cube_radius = mod(cube_vector);
	
	num_subshapes = num_x_cubes * num_y_cubes * num_z_cubes;

	int i,j,k;
	double xstart, ystart, zstart;
	double xstart_tmp, ystart_tmp, zstart_tmp;
	

	xstart = shp.min.x;
	ystart = shp.min.y;
	zstart = shp.min.z;

	xstart_tmp = xstart;
	
	twprintf ("Number of zones is \n",num_subshapes);

	for(i = 0; i < num_x_cubes; i++) {

		twprintf ("x = %d\n",i);
		
		ystart_tmp = ystart;

		for(j = 0; j < num_y_cubes; j++) {

			twprintf ("y = %d\nz = ",j);
			
			zstart_tmp = zstart;

			for(k = 0; k < num_z_cubes; k++) {

				twprintf ("%d ",k);
			
				/* Process the subshape and update the ZSP pointer array */

				ZSP_zone temp_zone(shp, xstart_tmp, ystart_tmp, 
											zstart_tmp, cube_size);
											
				zone_array.add_entry(temp_zone);		

				zstart_tmp += cube_size;

			}

			twprintf ("\n");

			ystart_tmp += cube_size;

		}

		xstart_tmp += cube_size;

	}


	
}

	

static int zone_polygons[20000];
static unsigned char vertex_outcode [20000];
	
ZSP_zone::ZSP_zone (const ChunkShape & shp, double xstart, double ystart, 
				double zstart, double cube_size)
{
	double xplane0, xplane1;
	double yplane0, yplane1;
	double zplane0, zplane1;
	int i,j;
	int outcode_or, outcode_and;
	int vert_outcode;

	num_z_polys = 0;
	num_z_verts = 0;

	/* Bounding planes in terms of axis limits */

	xplane0 = xstart;
	xplane1 = xstart + cube_size;

	yplane0 = ystart;
	yplane1 = ystart + cube_size;

	zplane0 = zstart;
	zplane1 = zstart + cube_size;

	for (i=0; i<shp.num_polys; i++)
	{
		for (j=0; j<shp.poly_list[i].num_verts; j++)
		{
			vert_outcode = 0;
			
			int point_no = shp.poly_list[i].vert_ind[j];
			
			if(shp.v_list[point_no].x < xplane0) vert_outcode |= rsp_oc_x0;
			if(shp.v_list[point_no].x > xplane1) vert_outcode |= rsp_oc_x1;
			if(shp.v_list[point_no].y < yplane0) vert_outcode |= rsp_oc_y0;
			if(shp.v_list[point_no].y > yplane1) vert_outcode |= rsp_oc_y1;
			if(shp.v_list[point_no].z < zplane0) vert_outcode |= rsp_oc_z0;
			if(shp.v_list[point_no].z > zplane1) vert_outcode |= rsp_oc_z1;

			if(j==0) {

				outcode_or  = vert_outcode;
				outcode_and = vert_outcode;

			}

			else {

				outcode_or  |= vert_outcode;
				outcode_and &= vert_outcode;

			}
			
		}
		
		if (outcode_and == 0)
		{
			zone_polygons[num_z_polys] = i;
			num_z_polys++;
		}
		
	}
	
	if (!num_z_polys)
	{
		z_poly_list=0;
		z_vert_list=0;
		return;
	}
	
	z_poly_list = new int [num_z_polys];
	for (i=0; i<num_z_polys; i++)
	{
		z_poly_list[i] = zone_polygons[i];
	}
	
	for (i=0; i<shp.num_verts; i++)
	{
		vertex_outcode[i] = 0;
	}
	
	for (i=0; i<num_z_polys; i++)
	{
		for (j=0; j<shp.poly_list[z_poly_list[i]].num_verts; j++)
		{
			vertex_outcode[shp.poly_list[z_poly_list[i]].vert_ind[j]] = 1;
		}
	}

	for (i=0; i<shp.num_verts; i++)
	{
		if (vertex_outcode[i]) num_z_verts++;
	}
	
	z_vert_list = new int[num_z_verts];
	
	j=0;
	
	for (i=0; i<shp.num_verts; i++)
	{
		if (vertex_outcode[i]) 
			z_vert_list[j++] = i;
	}

}

#endif


ZSP_Data::~ZSP_Data()
{
	while (zone_array.size())
	{
		zone_array.delete_first_entry();
	}
}

ZSP_Data::ZSP_Data (const char * zdata, size_t /*zsize*/)
{
	const char * ptr = zdata;

	num_x_cubes = *((int *) ptr);
	ptr += 4;

	num_y_cubes = *((int *) ptr);
	ptr += 4;

	num_z_cubes = *((int *) ptr);
	ptr += 4;

	cube_size = *((double *) ptr);
	ptr += 8;

	cube_radius = *((double *) ptr);
	ptr += 8;
	
	int i,j;
	
	int numzones = num_x_cubes*num_y_cubes*num_z_cubes;
	
	for (i=0; i<numzones; i++)
	{
		ZSP_zone tmpzone;
	
		tmpzone.num_z_polys = *((int *) ptr);
		ptr += 4;

		tmpzone.num_z_verts = *((int *) ptr);
		ptr += 4;
		
		if (tmpzone.num_z_polys)
			tmpzone.z_poly_list = new int [tmpzone.num_z_polys];
		
		for (j=0; j<tmpzone.num_z_polys; j++)
		{
			tmpzone.z_poly_list[j] = *((int *) ptr);
			ptr += 4;
		}
		
		if (tmpzone.num_z_verts)
			tmpzone.z_vert_list = new int [tmpzone.num_z_verts];
		
		for (j=0; j<tmpzone.num_z_verts; j++)
		{
			tmpzone.z_vert_list[j] = *((int *) ptr);
			ptr += 4;
		}
		
		zone_array.add_entry(tmpzone);
	}

}	


ZSP_zone::ZSP_zone ()
{
	num_z_polys = 0;
	z_poly_list = 0;
	num_z_verts = 0;
	z_vert_list = 0;
}	

ZSP_zone::~ZSP_zone ()
{
	if (num_z_polys)
		delete [] z_poly_list;
	if (num_z_verts)
		delete [] z_vert_list;
}	

ZSP_zone::ZSP_zone (const ZSP_zone &zz)
{
	if (zz.num_z_polys)	
	{
		num_z_polys = zz.num_z_polys;
		z_poly_list = new int [num_z_polys];
		
		int i;
		
		for (i=0; i<num_z_polys; i++)
		{
			z_poly_list[i] = zz.z_poly_list[i];
		}
	}
	else
	{
		z_poly_list = 0;
		num_z_polys = 0;
	}


	if (zz.num_z_verts)	
	{
		num_z_verts = zz.num_z_verts;
		z_vert_list = new int [num_z_verts];
		
		int i;
		
		for (i=0; i<num_z_verts; i++)
		{
			z_vert_list[i] = zz.z_vert_list[i];
		}
	}
	else
	{
		z_vert_list = 0;
		num_z_verts = 0;
	}
	
}

ZSP_zone & ZSP_zone::operator=(const ZSP_zone &zz)
{
	
	if (num_z_polys)
		delete [] z_poly_list;
	if (num_z_verts)
		delete [] z_vert_list;

	if (zz.num_z_polys)	
	{
		num_z_polys = zz.num_z_polys;
		z_poly_list = new int [num_z_polys];
		
		int i;
		
		for (i=0; i<num_z_polys; i++)
		{
			z_poly_list[i] = zz.z_poly_list[i];
		}
	}
	else
	{
		z_poly_list = 0;
		num_z_polys = 0;
	}


	if (zz.num_z_verts)	
	{
		num_z_verts = zz.num_z_verts;
		z_vert_list = new int [num_z_verts];
		
		int i;
		
		for (i=0; i<num_z_verts; i++)
		{
			z_vert_list[i] = zz.z_vert_list[i];
		}
	}
	else
	{
		z_vert_list = 0;
		num_z_verts = 0;
	}
	
	return(*this);
	
}
	

unsigned char operator==(const ZSP_zone &z1, const ZSP_zone &z2)
{
	return(&z1 == &z2);
}	

unsigned char operator!=(const ZSP_zone &z1, const ZSP_zone &z2)
{
	return(&z1 != &z2);
}	



/////////////////////////////////////////

// Class Shape_ZSP_Data_Chunk functions

size_t Shape_ZSP_Data_Chunk::size_chunk ()
{
	int sz = 12 + 12 + 16;

	ZSP_Data * zdata = (ZSP_Data *)(&zspdata);

	for (LIF<ZSP_zone> znl(&zdata->zone_array); !znl.done(); znl.next())
	{
		sz += 8 + (znl().num_z_polys * 4) + (znl().num_z_verts * 4);
	}

	return (chunk_size = sz);
}

void Shape_ZSP_Data_Chunk::fill_data_block ( char * data_start)
{
	strncpy (data_start, identifier, 8);

	data_start += 8;

	*((int *) data_start) = chunk_size;

	data_start += 4;

	*((int *) data_start) = zspdata.num_x_cubes;
	data_start += 4;
	
	*((int *) data_start) = zspdata.num_y_cubes;
	data_start += 4;

	*((int *) data_start) = zspdata.num_z_cubes;
	data_start += 4;

	*((double *) data_start) = zspdata.cube_size;
	data_start += 8;

	*((double *) data_start) = zspdata.cube_radius;
	data_start += 8;

	ZSP_Data * zdata = (ZSP_Data *)(&zspdata);

	for (LIF<ZSP_zone> znl(&zdata->zone_array); !znl.done(); znl.next())
	{
		*((int *) data_start) = znl().num_z_polys;
		data_start += 4;

		*((int *) data_start) = znl().num_z_verts;
		data_start += 4;

		int i;
		for (i=0; i<znl().num_z_polys; i++)
		{
			*((int *) data_start) = znl().z_poly_list[i];
			data_start += 4;
		}

		for (i=0; i<znl().num_z_verts; i++)
		{
			*((int *) data_start) = znl().z_vert_list[i];
			data_start += 4;
		}
	}
}	
