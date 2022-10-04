# Microsoft Developer Studio Project File - Name="enginelib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=enginelib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "enginelib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "enginelib.mak" CFG="enginelib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "enginelib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "enginelib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Smtm/enginelib", YACAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "enginelib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../_RBuildData/EngineLib"
# PROP Intermediate_Dir "../_RBuildData/EngineLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "./" /I "./Common" /I "./Meshs" /I "./Workspace" /I "./Dxframe" /I "./DxCommon" /I "./DxOctree" /I "./DxEffect" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\_RLib\enginelib.lib"

!ELSEIF  "$(CFG)" == "enginelib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../_DBuildData/EngineLib"
# PROP Intermediate_Dir "../_DBuildData/EngineLib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "./" /I "./Common" /I "./Meshs" /I "./Workspace" /I "./Dxframe" /I "./DxCommon" /I "./DxOctree" /I "./DxEffect" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\_DLib\enginelib.lib"

!ENDIF 

# Begin Target

# Name "enginelib - Win32 Release"
# Name "enginelib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Meshs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Meshs\DxAniKeys.h
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxAnimationSaveLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxFrameMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxFrameMesh.h
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxFrameMeshCollision.cpp
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxFrameMeshDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxFrameMeshMethod.cpp
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxFrameSaveLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxMeshesSaveLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxSkinMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxSkinMesh.h
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxSkinMeshDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Meshs\DxSkinMeshLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\Meshs\ReplaceContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\Meshs\ReplaceContainer.h
# End Source File
# End Group
# Begin Group "DxFrame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dxframe\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=.\dxframe\d3dapp.h
# End Source File
# Begin Source File

SOURCE=.\dxframe\d3dfile.cpp
# End Source File
# Begin Source File

SOURCE=.\dxframe\d3dfile.h
# End Source File
# Begin Source File

SOURCE=.\dxframe\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=.\dxframe\d3dfont.h
# End Source File
# Begin Source File

SOURCE=.\dxframe\d3dres.h
# End Source File
# Begin Source File

SOURCE=.\dxframe\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dxframe\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\dxframe\DxObject.h
# End Source File
# Begin Source File

SOURCE=.\dxframe\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dxframe\dxutil.h
# End Source File
# Begin Source File

SOURCE=.\dxframe\xrmxftmpl.cpp
# End Source File
# Begin Source File

SOURCE=.\dxframe\xrmxftmpl.h
# End Source File
# End Group
# Begin Group "DxCommon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DxCommon\Collision.cpp
# End Source File
# Begin Source File

SOURCE=.\DxCommon\Collision.h
# End Source File
# Begin Source File

SOURCE=.\DxCommon\DebugSet.cpp
# End Source File
# Begin Source File

SOURCE=.\DxCommon\DebugSet.h
# End Source File
# Begin Source File

SOURCE=.\DxCommon\DxInputDevice.cpp
# End Source File
# Begin Source File

SOURCE=.\DxCommon\DxInputDevice.h
# End Source File
# Begin Source File

SOURCE=.\DxCommon\DxViewPort.cpp
# End Source File
# Begin Source File

SOURCE=.\DxCommon\DxViewPort.h
# End Source File
# Begin Source File

SOURCE=.\DxCommon\ScanCode.h
# End Source File
# Begin Source File

SOURCE=.\DxCommon\TextureManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DxCommon\TextureManager.h
# End Source File
# End Group
# Begin Group "DxOctree"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DxOctree\CustomVertex.h
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxAnimationManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxAnimationManager.h
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxAnimationManagerSaveLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctree.cpp
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctree.h
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctreeBoxCreate.cpp
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctreeBoxRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctreeCollision.cpp
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctreeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctreeManager.h
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctreeManagerSaveLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctreeMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctreeMesh.h
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctreeMeshSaveLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\DxOctree\DxOctreeSaveLoad.cpp
# End Source File
# End Group
# Begin Group "WorkSpace"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WorkSpace\DxDecal.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkSpace\DxDecal.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Common\CBitArray.h
# End Source File
# Begin Source File

SOURCE=.\Common\CBitArray2d.h
# End Source File
# Begin Source File

SOURCE=.\Common\CTwoBitArray.h
# End Source File
# Begin Source File

SOURCE=.\Common\SerialFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Common\SerialFile.h
# End Source File
# End Group
# Begin Group "DxEffect"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DxEffect\DxEffect.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectAniso.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectAniso.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectBlur.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectBlur.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectBlurC.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectBlurC.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectBumpWater.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectBumpWater.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectCloudCover.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectCloudCover.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectDot3.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectDot3.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectFishEye.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectFishEye.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectFrame.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectHalo.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectHalo.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectMan.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectMan.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectMemb.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectMemb.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectRain.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectRain.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectRainDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectRainDrop.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectReflect.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectReflect.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectShadow.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectShadow.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectShadowHW.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectShadowHW.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectToon.cpp
# End Source File
# Begin Source File

SOURCE=.\DxEffect\DxEffectToon.h
# End Source File
# Begin Source File

SOURCE=.\DxEffect\ShaderConstant.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
