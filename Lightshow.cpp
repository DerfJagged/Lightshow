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

LEDState New_SMC_State[4];
bool Power_LED_State;

class Sample : public ATG::Application {
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
    Sample atgApp;
    ATG::GetVideoSettings( &atgApp.m_d3dpp.BackBufferWidth, &atgApp.m_d3dpp.BackBufferHeight );
    atgApp.m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    atgApp.Run();
}

HRESULT Sample::Initialize() {
    if( FAILED( m_Font16.Create( "game:\\Media\\Fonts\\Arial_16.xpr" ) ) )
        return ATGAPPERR_MEDIANOTFOUND;
    if( FAILED( m_Font12.Create( "game:\\Media\\Fonts\\Arial_12.xpr" ) ) )
        return ATGAPPERR_MEDIANOTFOUND;

    m_Font12.SetWindow( ATG::GetTitleSafeArea() );
    m_Font16.SetWindow( ATG::GetTitleSafeArea() );
    return S_OK;
}

HRESULT Sample::Update() {
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

	// Set selected quadrant color
	if (quadrant != -1) {
		if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_A ) ) {
			New_SMC_State[quadrant] = GREEN;
			SetLEDS();
		}
		else if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_B ) ) {
			New_SMC_State[quadrant] = RED;
			SetLEDS();
		}
		else if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_X ) )  {
			New_SMC_State[quadrant] = OFF;
			SetLEDS();
		}
		else if( ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_Y ) ) {
			New_SMC_State[quadrant] = ORANGE;
			SetLEDS();
		}
	}

	// Set center LED
	if ( m_pGamepad->wPressedButtons & XINPUT_GAMEPAD_START ) {
		Power_LED_State = !Power_LED_State;
		if (Power_LED_State) {
			SetPowerLED(POWER_LED_ON, false);
		}
		else {
			SetPowerLED(POWER_LED_OFF, false);
		}
	}

	return S_OK;
}

HRESULT Sample::DrawTextContent() {
    D3DRECT rc = m_Font16.m_rcWindow;

    m_Font16.Begin();
    m_Font16.DrawText( ( rc.x2 - rc.x1 ) / 2.0f, 0, 0xffffffff, L"Lightshow", ATGFONT_CENTER_X );
    m_Font16.End();

    m_Font12.Begin();
	m_Font12.DrawText( ( rc.x2 - rc.x1 ) / 2.0f - 120, 40, 0xffffffff, L"Use Left Stick to select an LED\nPress (A) to set to green\nPress (B) to set to red\nPress (Y) to set to yellow\nPress (X) to turn off\nPress (Start) to toggle center\n\nIf all LEDs off, system sets Q1 to green.", ATGFONT_LEFT );
	m_Font12.DrawText( rc.x1 - 200.0f, rc.y2 - 100.0f, 0xffffffff, L"Made by Derf\nConsoleMods.org", ATGFONT_LEFT );
    m_Font12.End();

    return S_OK;
}

HRESULT Sample::Render() {
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

void SetLEDS() {
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
