# Microsoft Developer Studio Project File - Name="AvP" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AvP - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AvP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AvP.mak" CFG="AvP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AvP - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AvP - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "AvP - Win32 Release For Fox" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AvP - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Zp4 /MT /W3 /GX /O2 /I "c:\mssdk\include" /I "3dc" /I "3dc\avp" /I "3dc\avp\support" /I "3dc\avp\win95" /I "3dc\avp\win95\frontend" /I "3dc\avp\win95\gadgets" /I "3dc\include" /I "3dc\win95" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D engine=1 /D "__STDC__" /D "AVP_DEBUG_VERSION" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dsound.lib dplayx.lib dinput.lib smackw32.lib binkw32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"avpprog.exe" /libpath:"c:\mssdk\lib" /libpath:"3dc"

!ELSEIF  "$(CFG)" == "AvP - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp4 /MTd /W3 /Gm /GX /ZI /Od /I "c:\mssdk\include" /I "3dc" /I "3dc\avp" /I "3dc\avp\support" /I "3dc\avp\win95" /I "3dc\avp\win95\frontend" /I "3dc\avp\win95\gadgets" /I "3dc\include" /I "3dc\win95" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D engine=1 /D "__STDC__" /D "AVP_DEBUG_VERSION" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dsound.lib dplayx.lib dinput.lib smackw32.lib binkw32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"debug_AvP.exe" /pdbtype:sept /libpath:"c:\mssdk\lib" /libpath:"3dc"

!ELSEIF  "$(CFG)" == "AvP - Win32 Release For Fox"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AvP___Win32_Release_For_Fox"
# PROP BASE Intermediate_Dir "AvP___Win32_Release_For_Fox"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "AvP___Win32_Release_For_Fox"
# PROP Intermediate_Dir "AvP___Win32_Release_For_Fox"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Zp4 /MT /W3 /GX /O2 /I "c:\mssdk\include" /I "3dc" /I "3dc\avp" /I "3dc\avp\support" /I "3dc\avp\win95" /I "3dc\avp\win95\frontend" /I "3dc\avp\win95\gadgets" /I "3dc\include" /I "3dc\win95" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D engine=1 /D "__STDC__" /YX /FD /c
# ADD CPP /nologo /Zp4 /MT /W3 /GX /O2 /I "c:\mssdk\include" /I "3dc" /I "3dc\avp" /I "3dc\avp\support" /I "3dc\avp\win95" /I "3dc\avp\win95\frontend" /I "3dc\avp\win95\gadgets" /I "3dc\include" /I "3dc\win95" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D engine=1 /D "__STDC__" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dsound.lib dplayx.lib dinput.lib smackw32.lib binkw32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"AvP.exe" /libpath:"c:\mssdk\lib" /libpath:"3dc"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ddraw.lib dsound.lib dplayx.lib dinput.lib smackw32.lib binkw32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /out:"AvP.exe" /libpath:"c:\mssdk\lib" /libpath:"3dc"

!ENDIF 

# Begin Target

# Name "AvP - Win32 Release"
# Name "AvP - Win32 Debug"
# Name "AvP - Win32 Release For Fox"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\3dc\Afont.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\ahudgadg.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\AI_Sight.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\alt_tab.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Animchnk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\animobs.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_EnvInfo.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_Intro.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_MenuData.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_MenuGfx.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_Menus.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_MP_Config.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_UserProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Avpchunk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\AvpReg.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Avpview.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\awBmpLd.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\awIffLd.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\awPnmLd.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\awTexLd.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_agun.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_ais.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_alien.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_binsw.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_cable.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_corpse.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_deathvol.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_debri.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_dummy.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_fan.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_far.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_fhug.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_gener.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_ldoor.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_lift.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_light.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_lnksw.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_ltfx.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_marin.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_mission.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_near.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_pargen.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_plachier.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_plift.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_pred.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_queen.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_RubberDuck.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_selfdest.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_snds.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_spcl.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_swdor.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_track.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_types.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_videoscreen.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_waypt.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_weap.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_xeno.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\bink.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Bmpnames.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\BonusAbilities.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\cconvars.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\CD_player.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\CDTrackSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Cheat.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\CheatModes.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\chnkload.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Chnktexi.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Chnktype.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\chtcodes.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Chunk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Chunkpal.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\comp_map.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Comp_shp.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\consbind.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\consbtch.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\conscmnd.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\ConsoleLog.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\conssym.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\consvar.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\Coordstr.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\shapes\Cube.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\d3_func.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\d3d_hud.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\d3d_render.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\Daemon.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\davehook.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\db.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Dd_func.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Ddplat.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\deaths.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Debuglog.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\decal.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\DetailLevels.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Di_func.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\DirectPlay.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Dp_func.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\dplayext.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\DummyObjectChunk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\dx_proj.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Dxlog.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Dynamics.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Dynblock.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\endianio.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Enumchnk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Enumsch.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Envchunk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Equipmnt.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\equiputl.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\extents.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\fail.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Ffread.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Ffstdio.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\fragchnk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\frustrum.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\gadget.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Game.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\game_statistics.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\gamecmds.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\gameflow.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\gamevars.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\GammaControl.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gflwplat.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Gsprchnk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\hierchnk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\hierplace.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\HModel.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Hud.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\hudgadg.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\huffman.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\iff.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\iff_ILBM.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\ILBM_ext.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\indexfnt.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\intro.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Inventry.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\io.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\iofocus.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\jsndsup.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\Kshape.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Kzsort.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Langplat.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Language.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Lighting.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\list_tem.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\load_shp.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\los.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Ltchunk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\Map.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Maps.c
# End Source File
# Begin Source File

SOURCE=.\3dc\Maths.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\md5.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\media.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\mem3dc.c
# End Source File
# Begin Source File

SOURCE=.\3dc\Mem3dcpp.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\mempool.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\MessageHistory.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Mishchnk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\missions.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\modcmds.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\Module.c
# End Source File
# Begin Source File

SOURCE=.\3dc\Morph.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\MouseCentreing.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\movement.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\mp_launch.c
# End Source File
# Begin Source File

SOURCE=.\3dc\Mslhand.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Npcsetup.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Obchunk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\Object.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Objsetup.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\OEChunk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Our_mem.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Paintball.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\particle.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\PathChnk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Pcmenus.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Pfarlocs.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Pheromon.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\plat_shp.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Platsup.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Player.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Pldghost.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Pldnet.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\plspecfn.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Pmove.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\progress_bar.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Projload.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Psnd.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Psndplat.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Psndproj.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Pvisible.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\r2base.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\r2pos666.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\reflist.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\refobj.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\rentrntq.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\rootgadg.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\savegame.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\scream.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Scrshot.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\scstring.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\SecStats.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\sfx.c
# End Source File
# Begin Source File

SOURCE=.\3dc\shpanim.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Shpchunk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\smacker.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Sndchunk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\sphere.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Sprchunk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Strachnk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Stratdef.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\String.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\strtab.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\strutil.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\system.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\t_ingadg.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\Tables.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\tallfont.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\targeting.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\teletype.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Texio.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\textexp.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\textin.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Toolchnk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\track.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\trepgadg.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\trig666.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Triggers.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Txioctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Usr_io.c
# End Source File
# Begin Source File

SOURCE=.\3dc\Vdb.c
# End Source File
# Begin Source File

SOURCE=.\3dc\version.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\VideoModes.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Vision.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Vramtime.c
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Weapons.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\win_func.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\win_proj.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\winmain.c
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\wpchunk.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\wrapstr.cpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Zsp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\3dc\win95\advwin32.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\ahudgadg.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\AI_Sight.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\alt_tab.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Animchnk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\animobs.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_EnvInfo.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_MenuGfx.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_Menus.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_MP_Config.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Frontend\AvP_UserProfile.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Avpchunk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\avpitems.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\avppages.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\AvpReg.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Avpview.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\aw.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\awTexLd.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\awTexLd.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_agun.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_ais.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_alien.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_binsw.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_cable.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_corpse.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_deathvol.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_debri.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_dummy.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_fan.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_far.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_fhug.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_gener.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_ldoor.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_lift.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_light.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_lnksw.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_ltfx.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_marin.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_mission.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_near.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_paq.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_pargen.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_plachier.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_plift.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_pred.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_queen.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_RubberDuck.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_selfdest.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_snds.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_spcl.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_swdor.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_track.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_types.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_videoscreen.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_waypt.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\bh_weap.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Bh_xeno.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\bink.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\bink_Rad.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Bmp2.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Bmpnames.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\BonusAbilities.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\CD_player.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\CDTrackSelection.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Cheat.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Chnkload.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Chnkload.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Chnktexi.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Chnktype.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Chunk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Chunkpal.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\command.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Comp_shp.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\consbind.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\consbtch.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\conscmnd.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\ConsoleLog.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\conssym.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\consvar.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\Coordstr.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\d3_func.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\d3d_hud.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\d3d_render.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\d3dmacs.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\daemon.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Database.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\datatype.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\davehook.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Db.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\dbdefs.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\dcontext.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Debuglog.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Debuglog.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\decal.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\DetailLevels.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Dp_func.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\dp_Sprh.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\dplayext.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\DummyObjectChunk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Dxlog.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Dynamics.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Dynblock.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Eax.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\endianio.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Enumchnk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Enumsch.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Envchunk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Equates.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Equipmnt.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\equiputl.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\expvar.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\extents.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\fail.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Ffread.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Ffstdio.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\font.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\fragchnk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\frustrum.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\gadget.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\game_statistics.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Gamedef.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\gameflow.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Gameplat.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\GammaControl.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Gsprchnk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Hash_tem.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Heap_tem.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\hierchnk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\hierplace.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\hmodel.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Hud_data.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\HUD_layout.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Hud_map.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Huddefs.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\hudgadg.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Hudgfx.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\huffman.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Ia3d.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\iff.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\iff_ILBM.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\ILBM_ext.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\indexfnt.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Inline.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\intro.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Inventry.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\iofocus.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\jsndsup.h
# End Source File
# Begin Source File

SOURCE=.\3dc\Kshape.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Kzsort.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\langenum.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Language.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Lighting.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\list_tem.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\load_shp.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\los.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Ltchunk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\ltfx_exp.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Macro.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\md5.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\media.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\mempool.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\menudefs.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\menugfx.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Mishchnk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\missions.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Mmx_math.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\modcmds.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\Mslhand.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Multmenu.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Npcsetup.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Obchunk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\objedit.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Objsetup.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\OEChunk.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Ourasert.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\ourbool.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Paintball.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\particle.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\PathChnk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Pcmenus.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Pentime.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Pfarlocs.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Pheromon.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\plat_shp.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\platform.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Pldghost.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Pldnet.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Pmove.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\progress_bar.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\projfont.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Projload.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\projmenu.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\projtext.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Psnd.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Psndplat.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Psndproj.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Pvisible.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\r2base.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\r2pos666.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Rad.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\rebitems.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\rebmenus.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\reflist.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\refobj.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\rentrntq.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\rootgadg.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\savegame.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\scream.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Scrshot.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\scstring.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\sequnces.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\sfx.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\ShowCmds.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\shpanim.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Shpchunk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Smack.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\smacker.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Smsopt.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Sndchunk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\sphere.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Sprchunk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\statpane.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Strachnk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Stratdef.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\String.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\strtab.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\strutil.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\System.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\t_ingadg.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\tallfont.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\targeting.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\teletype.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\textexp.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\textin.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Toolchnk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\track.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\gadgets\trepgadg.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\trig666.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Triggers.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Txioctrl.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Usr_io.h
# End Source File
# Begin Source File

SOURCE=.\3dc\version.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\VideoModes.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Vision.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\win95\Vmanpset.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Vramtime.h
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\Weapons.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\wpchunk.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\avp\support\wrapstr.hpp
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Zmouse.h
# End Source File
# Begin Source File

SOURCE=.\3dc\win95\Zsp.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
