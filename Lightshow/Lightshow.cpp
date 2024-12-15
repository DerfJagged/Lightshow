//--------------------------------------------------------------------------------------
// Lightshow.cpp
//
// Tool to test Ring of Light (ROL) LEDs. 
//--------------------------------------------------------------------------------------
#include <xtl.h>
#include <AtgApp.h>
#include <AtgFont.h>
#include <AtgInput.h>
#include <AtgUtil.h>
#include "smc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

LEDState New_SMC_State[4];
unsigned char Power_LED_State = POWER_LED_ON;

class Lightshow : public ATG::Application {
    ATG::Font m_Font16; // 16-point font class
    ATG::Font m_Font12; // 12-point font class
	ATG::GAMEPAD* m_pGamepad;
    HRESULT DrawTextContent();

private:
    virtual HRESULT Initialize();
    virtual HRESULT Update();
    virtual HRESULT Render();
};

VOID __cdecl main() {
    Lightshow atgApp;
    ATG::GetVideoSettings( &atgApp.m_d3dpp.BackBufferWidth, &atgApp.m_d3dpp.BackBufferHeight );
    atgApp.m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    atgApp.Run();
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

int PlayLightshow() {
    const char* filename = "game:\\lightshow.conf";
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

HRESULT Lightshow::Initialize() {
    if( FAILED( m_Font16.Create( "game:\\Media\\Fonts\\Arial_16.xpr" ) ) )
        return ATGAPPERR_MEDIANOTFOUND;
    if( FAILED( m_Font12.Create( "game:\\Media\\Fonts\\Arial_12.xpr" ) ) )
        return ATGAPPERR_MEDIANOTFOUND;

    m_Font12.SetWindow( ATG::GetTitleSafeArea() );
    m_Font16.SetWindow( ATG::GetTitleSafeArea() );
    return S_OK;
}

HRESULT Lightshow::Update() {
    // Get the current gamepad status
    m_pGamepad = ATG::Input::GetMergedInput();

	// Get selected quadrant
	int quadrant = -1;
	if (m_pGamepad->sThumbLY > m_pGamepad->LEFT_THUMB_DEADZONE) {
		if (m_pGamepad->sThumbLX < -m_pGamepad->LEFT_THUMB_DEADZONE) { // Up-left
			quadrant = 0;
		}
		else if (m_pGamepad->sThumbLX > m_pGamepad->LEFT_THUMB_DEADZONE) { // Up-right
			quadrant = 1;
		}
	}
	else if (m_pGamepad->sThumbLY < -m_pGamepad->LEFT_THUMB_DEADZONE) {
		if (m_pGamepad->sThumbLX < -m_pGamepad->LEFT_THUMB_DEADZONE) { // Down-left
			quadrant = 2;
		}
		else if (m_pGamepad->sThumbLX > m_pGamepad->LEFT_THUMB_DEADZONE) { // Down-right
			quadrant = 3;
		}
	}

	if (quadrant != -1) {
		// Set selected quadrant color
		if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_A ) ) {
			New_SMC_State[quadrant] = GREEN;
			SetLEDs();
		}
		else if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_B ) ) {
			New_SMC_State[quadrant] = RED;
			SetLEDs();
		}
		else if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_X ) )  {
			New_SMC_State[quadrant] = OFF;
			SetLEDs();
		}
		else if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_Y ) ) {
			New_SMC_State[quadrant] = ORANGE;
			SetLEDs();
		}
	}
	else {
		// Set whole ring color
		if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_A ) ) {
			SetSolidRing(GREEN);
		}
		else if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_B ) ) {
			SetSolidRing(RED);
		}
		else if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_X ) )  {
			SetSolidRing(OFF);
		}
		else if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_Y ) ) {
			SetSolidRing(ORANGE);
		}
	}

	// Set center LED
	if ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_BACK ) {
		if (Power_LED_State == POWER_LED_OFF) {
			perror("POWER LED ON");
			Power_LED_State = POWER_LED_ON;
		}
		else if (Power_LED_State == POWER_LED_ON) {
			perror("POWER LED BLINK");
			Power_LED_State = POWER_LED_BLINK;
		}
		else if (Power_LED_State == POWER_LED_BLINK) {
			perror("POWER LED OFF");
			Power_LED_State = POWER_LED_OFF;
		}
		SetPowerLED(Power_LED_State, false);
	}

	// Play lightshow.conf
	if ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_START ) {
		PlayLightshow();
	}

	return S_OK;
}

HRESULT Lightshow::DrawTextContent() {
    D3DRECT rc = m_Font16.m_rcWindow;

    m_Font16.Begin();
    m_Font16.DrawText( ( rc.x2 - rc.x1 ) / 2.0f, 0, 0xffffffff, L"Lightshow", ATGFONT_CENTER_X );
    m_Font16.End();

    m_Font12.Begin();
	m_Font12.DrawText( ( rc.x2 - rc.x1 ) / 2.0f - 120, 40, 0xffffffff, L"Use Left Stick to select an LED\nPress (A) to set to green\nPress (B) to set to red\nPress (Y) to set to yellow\nPress (X) to turn off\nPress (Back) to toggle center\nPress (Start) to play lightshow.conf\n\nIf all LEDs off, system sets Q1 to green.", ATGFONT_LEFT );
	m_Font12.DrawText( rc.x1 - 200.0f, rc.y2 - 100.0f, 0xffffffff, L"Made by Derf\nConsoleMods.org", ATGFONT_LEFT );
    m_Font12.End();

    return S_OK;
}

HRESULT Lightshow::Render() {
    ATG::RenderBackground( 0x00000000, 0x00000000 ); //Black
    DrawTextContent();
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    return S_OK;
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
