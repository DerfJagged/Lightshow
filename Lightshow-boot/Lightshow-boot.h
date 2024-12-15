//IIRC this should be right and work, call as SetLEDs(Color1,Color2,Color3,Color4);

//Thanks to www.free60.org/SMC for helping me get front LEDs right
#pragma once
#ifndef _LIGHTSHOWBOOT_H
#define _LIGHTSHOWBOOT_H

LEDState StringToLEDState(const char* color);
void ProcessLine(const char* line);
int PlayLightshow();
HRESULT CreateSymbolicLink(CHAR* szDrive, CHAR* szDeviceName, BOOL System);
typedef long NTSTATUS;
typedef struct _ANSI_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PCHAR Buffer;
} ANSI_STRING, *PANSI_STRING;

EXTERN_C { 
	DWORD ExCreateThread(PHANDLE pHandle, DWORD dwStackSize, LPDWORD lpThreadId, 
	VOID* apiThreadStartup, LPTHREAD_START_ROUTINE lpStartAddress, 
	LPVOID lpParameter, DWORD dwCreationFlagsMod);
	VOID  XapiThreadStartup(VOID (__cdecl *StartRoutine)(VOID*), VOID* StartContext);
	DWORD XexLoadImage(CHAR* ImagePath, DWORD LoadFlags, DWORD Version, HMODULE* ModuleHandle);
	NTSTATUS ObCreateSymbolicLink(PANSI_STRING, PANSI_STRING);
	NTSTATUS ObDeleteSymbolicLink(PANSI_STRING);
	void RtlInitAnsiString(PANSI_STRING DestinationString, const char* SourceString);
}

#endif
