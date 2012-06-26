/***************************************************************************/
/*                           Creation of Z-Buffer                          */
/***************************************************************************/
/*
 * D3DAppICreateZBuffer
 * Create a Z-Buffer of the appropriate depth and attach it to the back
 * buffer.
 */
BOOL
D3DAppICreateZBuffer(int w, int h, int driver)
{
    DDSURFACEDESC ddsd;
    DWORD devDepth;
    /*
     * Release any Z-Buffer that might be around just in case.
     */
    RELEASE(d3dappi.lpZBuffer);
    
    /*
     * If this driver does not do z-buffering, don't create a z-buffer
     */
    if (!d3dappi.Driver[driver].bDoesZBuffer)
        return TRUE;

    memset(&ddsd, 0 ,sizeof(DDSURFACEDESC));
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS |
                   DDSD_ZBUFFERBITDEPTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
    ddsd.dwHeight = h;
    ddsd.dwWidth = w;
    /*
     * If this is a hardware D3D driver, the Z-Buffer MUST end up in video
     * memory.  Otherwise, it MUST end up in system memory.
     */
    if (d3dappi.Driver[driver].bIsHardware)
        ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
    else
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
    /*
     * Get the Z buffer bit depth from this driver's D3D device description
     */
    devDepth = d3dappi.Driver[driver].Desc.dwDeviceZBufferBitDepth;
    if (devDepth & DDBD_32)
        ddsd.dwZBufferBitDepth = 32;
    else if (devDepth & DDBD_24)
        ddsd.dwZBufferBitDepth = 24;
    else if (devDepth & DDBD_16)
        ddsd.dwZBufferBitDepth = 16;
    else if (devDepth & DDBD_8)
        ddsd.dwZBufferBitDepth = 8;
    else {
        D3DAppISetErrorString("Unsupported Z-buffer depth requested by device.\n");
        return FALSE;
    }
    LastError = d3dappi.lpDD->lpVtbl->CreateSurface(d3dappi.lpDD, &ddsd,
                                                    &d3dappi.lpZBuffer,
                                                    NULL);
    if(LastError != DD_OK) {
        D3DAppISetErrorString("CreateSurface for Z-buffer failed.\n%s",
                              D3DAppErrorToString(LastError));
        goto exit_with_error;
    }
    /*
     * Attach the Z-buffer to the back buffer so D3D will find it
     */
    LastError =
       d3dappi.lpBackBuffer->lpVtbl->AddAttachedSurface(d3dappi.lpBackBuffer,
                                                        d3dappi.lpZBuffer);
    if(LastError != DD_OK) {
        D3DAppISetErrorString("AddAttachedBuffer failed for Z-Buffer.\n%s",
                              D3DAppErrorToString(LastError));
        goto exit_with_error;
    }
    /*
     * Find out if it ended up in video memory (FYI).
     */
    LastError = D3DAppIGetSurfDesc(&ddsd, d3dappi.lpZBuffer);
    if (LastError != DD_OK) {
        D3DAppISetErrorString("Failed to get surface description of Z buffer.\n%s",
                              D3DAppErrorToString(LastError));
        goto exit_with_error;
    }
    d3dappi.bZBufferInVideo =
                  (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) ? TRUE : FALSE;
    return TRUE;

exit_with_error:
    RELEASE(d3dappi.lpZBuffer);
    return FALSE;
}
