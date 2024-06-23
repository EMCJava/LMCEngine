//
// Created by loyus on 7/2/2023.
//

#pragma once

void
InitializeEnvironment( );

void
ShutdownEnvironment( );

inline char DefaultEditorImGuiLayout[] = "[Window][DockSpaceViewport_11111111]\nPos=0,19\nSize=1280,701\nCollapsed=0\n[Window][Console]\nPos=0,838\nSize=1608,242\nCollapsed=0\nDockId=0x00000006,0\n[Window][Hierarchy]\nPos=0,19\nSize=306,817\nCollapsed=0\nDockId=0x00000008,0\n[Window][Details]\nPos=1610,19\nSize=310,1061\nCollapsed=0\nDockId=0x00000004,0\n[Window][Main viewport]\nPos=308,19\nSize=1300,817\nCollapsed=0\nDockId=0x00000009,0\n[Window][WindowOverViewport_11111111]\nPos=0,19\nSize=1920,1061\nCollapsed=0\n[Window][Debug##Default]\nPos=60,60\nSize=400,400\nCollapsed=0\n[Docking][Data]\nDockSpace       ID=0x7C6B3D9B Window=0xA87D555D Pos=0,19 Size=1920,1061 Split=X\nDockNode      ID=0x00000003 Parent=0x7C6B3D9B SizeRef=1608,1061 Split=Y\nDockNode    ID=0x00000005 Parent=0x00000003 SizeRef=1920,817 Split=X\nDockNode  ID=0x00000008 Parent=0x00000005 SizeRef=306,1061 Selected=0x29EABFBD\nDockNode  ID=0x00000009 Parent=0x00000005 SizeRef=1300,1061 CentralNode=1 Selected=0xA13C9A01\nDockNode    ID=0x00000006 Parent=0x00000003 SizeRef=1920,242 Selected=0x49278EEE\nDockNode      ID=0x00000004 Parent=0x7C6B3D9B SizeRef=310,1061 Selected=0xFC3EA205\nDockSpace       ID=0x8B93E3BD Pos=112,154 Size=1280,701 CentralNode=1 HiddenTabBar=1";
