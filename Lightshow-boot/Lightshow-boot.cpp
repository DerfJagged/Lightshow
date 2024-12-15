//--------------------------------------------------------------------------------------
// Lightshow-boot.cpp
//
// A DashLaunch plugin to play Ring of Light lightshows at bootup.
//
// Credit to Xenomega for their plugin/symlink code:
// https://github.com/Xenomega/ArchLoader/tree/master
//--------------------------------------------------------------------------------------
#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smc.h"
#include "Lightshow-boot.h"

#define MAX_LINE_LENGTH 256
HANDLE moduleHandle;
LEDState New_SMC_State[4];
unsigned char Power_LED_State = POWER_LED_ON;

static void Main01(){
    PlayLightshow();
}

BOOL APIENTRY DllMain(HANDLE hInstDLL, DWORD reason, LPVOID lpReserved){
	if(reason == DLL_PROCESS_ATTACH){
		HANDLE    hThread;
        DWORD    hThreadId;
		ExCreateThread(&hThread, 0, &hThreadId, (VOID*)XapiThreadStartup , (LPTHREAD_START_ROUTINE)Main01, NULL, 0x2);
		XSetThreadProcessor(hThread, 4); ResumeThread(hThread); CloseHandle(hThread);
	}
 	return TRUE;
}

LEDState StringToLEDState(const char* color) {
    if (strcmp(color, "OFF") == 0) return OFF;
    if (strcmp(color, "RED") == 0) return RED;
    if (strcmp(color, "GREEN") == 0) return GREEN;
    if (strcmp(color, "ORANGE") == 0) return ORANGE;
    return OFF;
}

void ProcessLine(const char* line) {
    char color1[16], color2[16], color3[16], color4[16], power[16];
    int duration = 0;
	
	int num_fields = sscanf(line, "%15[^,],%15[^,],%15[^,],%15[^,],%d", color1, color2, color3, color4, &duration);
	if (num_fields > 3) {
        //printf("Colors: %s, %s, %s, %s | Duration: %d\n", color1, color2, color3, color4, duration);
		New_SMC_State[0] = StringToLEDState(color1);
		New_SMC_State[1] = StringToLEDState(color2);
		New_SMC_State[2] = StringToLEDState(color3);
		New_SMC_State[3] = StringToLEDState(color4);
		SetLEDs();
		if (duration) {
			Sleep(duration);
		}
    } else {
		num_fields = sscanf(line, "%15[^,],%d", power, &duration);
		
		if (num_fields > 0) {
			if (strcmp(power, "POWER_OFF") == 0) {
				Power_LED_State = POWER_LED_OFF;
			}
			else if (strcmp(power, "POWER_ON") == 0) {
				Power_LED_State = POWER_LED_ON;
			}
			else if (strcmp(power, "POWER_BLINK") == 0) {
				Power_LED_State = POWER_LED_BLINK;
			}
			SetPowerLED(Power_LED_State, false);
			//printf("Line: %s, Duration grabbed: %d\n", line, duration);
			if (duration) {
				Sleep(duration);
			}		
		}
		else {
			printf("Invalid line format: %s\n", line);
		}
	}
}

HRESULT CreateSymbolicLink(CHAR* szDrive, CHAR* szDeviceName, BOOL System) {
	// Setup our path
	CHAR szDestinationDrive[MAX_PATH];
	if(System)
		sprintf_s(szDestinationDrive, MAX_PATH, "\\System??\\%s", szDrive);
	else
		sprintf_s(szDestinationDrive, MAX_PATH, "\\??\\%s", szDrive);

	// Setup our strings
	ANSI_STRING linkname, devicename;
	RtlInitAnsiString(&linkname, szDestinationDrive);
	RtlInitAnsiString(&devicename, szDeviceName);

	// Create finally
	NTSTATUS status = ObCreateSymbolicLink(&linkname, &devicename);
	if (status >= 0)
		return S_OK;
	return S_FALSE;
}

int PlayLightshow() {
	CreateSymbolicLink("HDD:", "\\Device\\Harddisk0\\Partition1", TRUE);
	const char* filename = "HDD:\\Apps\\Lightshow\\lightshow.conf";
	FILE* file = fopen(filename, "r");

    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        ProcessLine(line);
    }

    fclose(file);
	return 0;
}

void PrepareBuffers() {
	ZeroMemory( m_SMCMessage, sizeof(m_SMCMessage) );
	ZeroMemory( m_SMCReturn, sizeof(m_SMCReturn) );
}

void SetPowerLED(unsigned char command, bool animate) {
    PrepareBuffers();
    m_SMCMessage[0] = 0x8c;
    m_SMCMessage[1] = command;
    m_SMCMessage[2] = (animate ? 0x01 : 0x00);
	HalSendSMCMessage(m_SMCMessage, NULL);
}

void SetSolidRing(LEDState color) {
	New_SMC_State[0] = color;
	New_SMC_State[1] = color;
	New_SMC_State[2] = color;
	New_SMC_State[3] = color;
	SetLEDs();
}

void SetLEDs() {
	LEDState sTopLeft = New_SMC_State[0];
	LEDState sTopRight = New_SMC_State[1];
	LEDState sBottomLeft = New_SMC_State[2];
	LEDState sBottomRight = New_SMC_State[3];
	PrepareBuffers();
    m_SMCMessage[0] = 0x99;
    m_SMCMessage[1] = 0x01;
	m_SMCMessage[2] = ((unsigned char)(sTopLeft>>3) | (unsigned char)(sTopRight>>2) | (unsigned char)(sBottomLeft>>1) | (unsigned char)(sBottomRight));
	HalSendSMCMessage(m_SMCMessage, NULL);
}
