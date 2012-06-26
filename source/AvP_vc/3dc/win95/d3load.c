/***************************************************************************/
/*                    Loading a PPM file into a surface                    */
/***************************************************************************/
/*
 * LoadSurface
 * Loads a ppm file into a texture map DD surface of the given format.  The
 * memory flag specifies DDSCAPS_SYSTEMMEMORY or DDSCAPS_VIDEOMEMORY.
 */
LPDIRECTDRAWSURFACE
D3DAppILoadSurface(LPDIRECTDRAW lpDD, LPCSTR lpName,
                   LPDDSURFACEDESC lpFormat, DWORD memoryflag)
{
    LPDIRECTDRAWSURFACE lpDDS;
    DDSURFACEDESC ddsd, format;
    D3DCOLOR colors[256];
    D3DCOLOR c;
    DWORD dwWidth, dwHeight;
    int i, j;
    FILE *fp;
    char *lpC;
    CHAR buf[100];
    LPDIRECTDRAWPALETTE lpDDPal;
    PALETTEENTRY ppe[256];
    int psize;
    DWORD pcaps;
    int color_count;
    BOOL bQuant = FALSE;
    HRESULT ddrval;

    /*
     * Find the image file and open it
     */
    fp = D3DAppIFindFile(lpName, "rb");
    if (fp == NULL) {
        D3DAppISetErrorString("Cannot find %s.\n", lpName);
        return NULL;
    }
    /*
     * Is it a PPM file?
     */
    fgets(buf, sizeof buf, fp);
    if (lstrcmp(buf, "P6\n")) {
        fclose(fp);
        D3DAppISetErrorString("%s is not a PPM file.\n", lpName);
        return NULL;
    }
    /*
     * Skip any comments
     */
    do {
        fgets(buf, sizeof buf, fp);
    } while (buf[0] == '#');
    /*
     * Read the width and height
     */
    sscanf(buf, "%d %d\n", &dwWidth, &dwHeight);
    fgets(buf, sizeof buf, fp); /* skip next line */
    /*
     * Create a surface of the given format using the dimensions of the PPM
     * file.
     */
    memcpy(&format, lpFormat, sizeof(DDSURFACEDESC));
    if (format.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) {
        bQuant = TRUE;
        psize = 256;
        pcaps = DDPCAPS_8BIT | DDPCAPS_ALLOW256;
    } else if (format.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4) {
        bQuant = TRUE;
        psize = 16;
        pcaps = DDPCAPS_4BIT;
    }
    memcpy(&ddsd, &format, sizeof(DDSURFACEDESC));
    ddsd.dwSize = sizeof(DDSURFACEDESC);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | memoryflag;
    ddsd.dwHeight = dwHeight;
    ddsd.dwWidth = dwWidth;

    ddrval = lpDD->lpVtbl->CreateSurface(lpDD, &ddsd, &lpDDS, NULL);
    if (ddrval != DD_OK) {
        D3DAppISetErrorString("CreateSurface for texture failed (loadtex).\n%s",
                              D3DAppErrorToString(ddrval));
        return NULL;
    }
    /*
     * Lock the surface so it can be filled with the PPM file
     */
    memset(&ddsd, 0, sizeof(DDSURFACEDESC));
    ddsd.dwSize = sizeof(DDSURFACEDESC);
    ddrval = lpDDS->lpVtbl->Lock(lpDDS, NULL, &ddsd, 0, NULL);
    if (ddrval != DD_OK) {
        lpDDS->lpVtbl->Release(lpDDS);
        D3DAppISetErrorString("Lock failed while loading surface (loadtex).\n%s",
                              D3DAppErrorToString(ddrval));
        return NULL;
    }
    /*
     * The method of loading depends on the pixel format of the dest surface
     */
    if (!bQuant) {
        /*
         * The texture surface is not palettized
         */
        unsigned long* lpLP;
        unsigned short* lpSP;
        unsigned char* lpCP;
        unsigned long m;
        int s;
        int red_shift, red_scale;
        int green_shift, green_scale;
        int blue_shift, blue_scale;
        /*
         * Determine the red, green and blue masks' shift and scale.
         */
        for (s = 0, m = format.ddpfPixelFormat.dwRBitMask; !(m & 1);
                                                               s++, m >>= 1);
        red_shift = s;
        red_scale = 255 / (format.ddpfPixelFormat.dwRBitMask >> s);
        for (s = 0, m = format.ddpfPixelFormat.dwGBitMask; !(m & 1);
                                                               s++, m >>= 1);
        green_shift = s;
        green_scale = 255 / (format.ddpfPixelFormat.dwGBitMask >> s);
        for (s = 0, m = format.ddpfPixelFormat.dwBBitMask; !(m & 1);
                                                               s++, m >>= 1);
        blue_shift = s;
        blue_scale = 255 / (format.ddpfPixelFormat.dwBBitMask >> s);
        /*
         * Each RGB bit count requires different pointers
         */
        switch (format.ddpfPixelFormat.dwRGBBitCount) {
            case 32 :
                for (j = 0; j < (int)dwHeight; j++) {
                    /*
                     * Point to next row in texture surface
                     */
                    lpLP = (unsigned long*)(((char*)ddsd.lpSurface) +
                                                            ddsd.lPitch * j);
                    for (i = 0; i < (int)dwWidth; i++) {
                        int r, g, b;
                        /*
                         * Read each value, scale it and shift it into position
                         */
                        r = getc(fp) / red_scale;
                        g = getc(fp) / green_scale;
                        b = getc(fp) / blue_scale;
                        *lpLP = (r << red_shift) | (g << green_shift) |
                                (b << blue_shift);
                        lpLP++;
                    }
                }
                break;
            case 16 :
                for (j = 0; j < (int)dwHeight; j++) {
                    lpSP = (unsigned short*)(((char*)ddsd.lpSurface) +
                                                            ddsd.lPitch * j);
                    for (i = 0; i < (int)dwWidth; i++) {
                        int r, g, b;
                        r = getc(fp) / red_scale;
                        g = getc(fp) / green_scale;
                        b = getc(fp) / blue_scale;
                        *lpSP = (r << red_shift) | (g << green_shift) |
                                (b << blue_shift);
                        lpSP++;
                    }
                }
                break;
            case 8:
                for (j = 0; j < (int)dwHeight; j++) {
                    lpCP = (unsigned char*)(((char*)ddsd.lpSurface) +
                                                            ddsd.lPitch * j);
                    for (i = 0; i < (int)dwWidth; i++) {
                        int r, g, b;
                        r = getc(fp) / red_scale;
                        g = getc(fp) / green_scale;
                        b = getc(fp) / blue_scale;
                        *lpCP = (r << red_shift) | (g << green_shift) | 
                                (b << blue_shift);
                        lpCP++;
                    }
                }
                break;
            default:
                /*
                 * This wasn't a format I recognize
                 */
                lpDDS->lpVtbl->Unlock(lpDDS, NULL);
                fclose(fp);
                lpDDS->lpVtbl->Release(lpDDS);
                D3DAppISetErrorString("Unknown pixel format (loadtex).");
                return NULL;
        }
        /*
         * Unlock the texture and return the surface pointer
         */
        lpDDS->lpVtbl->Unlock(lpDDS, NULL);
        fclose(fp);
        return (lpDDS);
    }

    /*
     * We assume the 8-bit palettized case
     */
    color_count = 0;    /* number of colors in the texture */
    for (j = 0; j < (int)dwHeight; j++) {
        /*
         * Point to next row in surface
         */
        lpC = ((char*)ddsd.lpSurface) + ddsd.lPitch * j;
        for (i = 0; i < (int)dwWidth; i++) {
            int r, g, b, k;
            /*
             * Get the next red, green and blue values and turn them into a
             * D3DCOLOR
             */
            r = getc(fp);
            g = getc(fp);
            b = getc(fp);
            c = RGB_MAKE(r, g, b);
            /*
             * Search for this color in a table of colors in this texture
             */
            for (k = 0; k < color_count; k++)
                if (c == colors[k]) break;
            if (k == color_count) {
                /*
                 * This is a new color, so add it to the list
                 */
                color_count++;
                /*
                 * More than 256 and we fail (8-bit) 
                 */
                if (color_count > psize) {
                    color_count--;
                    k = color_count - 1;
                    //goto burst_colors;
                }
                colors[k] = c;
            }
            /*
             * Set the "pixel" value on the surface to be the index into the
             * color table
             */
            if (psize == 16) {
                if ((i & 1) == 0)
                    *lpC = k & 0xf;
                else {
                    *lpC |= (k & 0xf) << 4;
                    lpC++;
                }
            } else {
                *lpC = (char)k;
                lpC++;
            }
        }
    }
    /*
     * Close the file and unlock the surface
     */
    fclose(fp);
    lpDDS->lpVtbl->Unlock(lpDDS, NULL);

//burst_colors:
    if (color_count > psize) {
        /*
         * If there are more than 256 colors, we overran our palette
         */
        lpDDS->lpVtbl->Unlock(lpDDS, NULL);
        lpDDS->lpVtbl->Release(lpDDS);
        D3DAppISetErrorString("Palette burst. (loadtex).\n");
        return (NULL);
    }

    /*
     * Create a palette with the colors in our color table
     */
    memset(ppe, 0, sizeof(PALETTEENTRY) * 256);
    for (i = 0; i < color_count; i++) {
        ppe[i].peRed = (unsigned char)RGB_GETRED(colors[i]);
        ppe[i].peGreen = (unsigned char)RGB_GETGREEN(colors[i]);
        ppe[i].peBlue = (unsigned char)RGB_GETBLUE(colors[i]);
    }
    /*
     * Set all remaining entry flags to D3DPAL_RESERVED, which are ignored by
     * the renderer.
     */
    for (; i < 256; i++)
        ppe[i].peFlags = D3DPAL_RESERVED;
    /*
     * Create the palette with the DDPCAPS_ALLOW256 flag because we want to
     * have access to all entries.
     */
    ddrval = lpDD->lpVtbl->CreatePalette(lpDD,
                                         DDPCAPS_INITIALIZE | pcaps,
                                         ppe, &lpDDPal, NULL);
    if (ddrval != DD_OK) {
        lpDDS->lpVtbl->Release(lpDDS);
        D3DAppISetErrorString("Create palette failed while loading surface (loadtex).\n%s",
                              D3DAppErrorToString(ddrval));
        return (NULL);
    }
    /*
     * Finally, bind the palette to the surface
     */
    ddrval = lpDDS->lpVtbl->SetPalette(lpDDS, lpDDPal);
    if (ddrval != DD_OK) {
        lpDDS->lpVtbl->Release(lpDDS);
        lpDDPal->lpVtbl->Release(lpDDPal);
        D3DAppISetErrorString("SetPalette failed while loading surface (loadtex).\n%s",
                              D3DAppErrorToString(ddrval));
        return (NULL);
    }

    lpDDPal->lpVtbl->Release(lpDDPal);

    return lpDDS;
}
