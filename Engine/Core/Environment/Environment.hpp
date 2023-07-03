//
// Created by loyus on 7/2/2023.
//

#pragma once

void
InitializeEnvironment();

void
ShutdownEnvironment();

inline char DefaultEditorImGuiLayout[] = "[Window][DockSpaceViewport_11111111]\nPos=0,19\nSize=1920,1061\nCollapsed=0\n[Window][Console]\nPos=0,838\nSize=1645,242\nCollapsed=0\nDockId=0x00000006,0\n[Window][Hierarchy]\nPos=0,19\nSize=273,817\nCollapsed=0\nDockId=0x00000004,0\n[Window][Details]\nPos=1647,19\nSize=273,1061\nCollapsed=0\nDockId=0x00000003,0\n[Docking][Data]\nDockSpace       ID=0x8B93E3BD Window=0xA787BDB4 Pos=0,19 Size=1920,1061 Split=X\nDockNode      ID=0x00000002 Parent=0x8B93E3BD SizeRef=1645,1061 Split=Y\nDockNode    ID=0x00000001 Parent=0x00000002 SizeRef=1920,817 Split=X\nDockNode  ID=0x00000004 Parent=0x00000001 SizeRef=273,1061 Selected=0x29EABFBD\nDockNode  ID=0x00000005 Parent=0x00000001 SizeRef=1269,1061 CentralNode=1 Selected=0xFC3EA205\nDockNode    ID=0x00000006 Parent=0x00000002 SizeRef=1920,242 Selected=0x49278EEE\nDockNode      ID=0x00000003 Parent=0x8B93E3BD SizeRef=273,1061 Selected=0xFC3EA205";
