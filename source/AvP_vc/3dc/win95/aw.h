#ifndef _INCLUDED_AW_H_
#define _INCLUDED_AW_H_

#include <d3d.h>

typedef IDirectDraw DDObject;
typedef IDirect3DDevice D3DDevice;
typedef IDirect3DTexture D3DTexture;
typedef IDirectDrawSurface DDSurface;
typedef IDirectDrawPalette DDPalette;

#define GUID_D3D_TEXTURE IID_IDirect3DTexture
#define GUID_DD_SURFACE IID_IDirectDrawSurface

typedef DDSURFACEDESC DD_SURFACE_DESC;
typedef DDSCAPS DD_S_CAPS;

struct AwBackupTexture;
typedef struct AwBackupTexture * AW_BACKUPTEXTUREHANDLE;

#endif /* _INCLUDED_AW_H_ */