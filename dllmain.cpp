// test_dll.cpp : Defines the exported functions for the DLL application.
//

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <stdlib.h>

extern "C" void __declspec (dllexport) foo()
{
	MessageBoxA(0, "Hello World!", "Hello Title", 1);
}

void ChangeMessage1(void); // Works
void SetNumberWins(void); // Works
void WinGameOnNextMove(void); // Works, but does not check that move was valid
void WinGameOnNextValidMove(void); // Partially Works
void ScrewWithKeyBindings(void); // Not working

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBoxA(0, "Hello World!", "Hello Title", 1);
		SetNumberWins();
		ChangeMessage1();
		WinGameOnNextMove();
		WinGameOnNextValidMove();
		ScrewWithKeyBindings();

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


typedef void(*__stdcall NEW_GAME)(int, int);

void handle_solitaire(void)
{
	// Req 1: Deal a new game
	NEW_GAME new_game_fcn = (NEW_GAME)0x1001468;
	new_game_fcn(1, 0);
}

////////////////////////////////////////////////////////////


void ChangeMessage1(void) // Done.
{		
	// Create MEssage
	const size_t length = 42;
	wchar_t message[length] = L"Not in this game :(";

	// Get Address == 0x01010C04
	wchar_t* DataAddress = (wchar_t*)0x01010C04;

	// Mess with permisions
	DWORD previous_setting;
	VirtualProtect(DataAddress, length, PAGE_EXECUTE_READWRITE, &previous_setting);
	memcpy(DataAddress, message, length); // opy new buffer
	VirtualProtect(DataAddress, length, previous_setting, nullptr);
}

void SetNumberWins(void) 
{			
	DWORD Number_of_wins = 9001;
	HKEY hkey;

	// Open/Get Key
	RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Applets\\FreeCell", 0, KEY_ALL_ACCESS, &hkey);
	
	// Set value of or at key?
	RegSetValueEx(hkey, L"won", 0, REG_DWORD, (LPBYTE)&Number_of_wins, sizeof(DWORD));

	RegCloseKey(hkey); // lose Key
}

void WinGameOnNextMove(void) {
	// address of chear var (0x1007130) from process (re)named "Cheat" in IDA (highlighted green)
	*(unsigned int *)0x1007130 = 2; // Works but also on invalid moves
}

void WinGameOnNextValidMove(void) 
{

}

//#include "resource.h"
//
//IDR_ACCEL1 ACCELERATORS
//{
//	0x4D, ID_TOGGLE_MODE, VIRTKEY, CONTROL    // ctrl-M
//	0x70, ID_DRAW_MODE, VIRTKEY               // F1
//	0x71, ID_SELECT_MODE, VIRTKEY             // F2
//}
//HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL1));

void ScrewWithKeyBindings(void) 
{
	HINSTANCE hinst;
	HACCEL oldH = LoadAccelerators(hinst, L"FREEMENU");

	ACCEL newHotKeys[9];
	CopyAcceleratorTable(oldH, newHotKeys, 8);

	newHotKeys[7] = { 0x0D, 114, VK_F6 };
	newHotKeys[8] = { 0x0D, 116, VK_F2 };

	DestroyAcceleratorTable(oldH);

	HACCEL newH = LoadAccelerators(hinst, MAKEINTRESOURCE(newHotKeys));
}








//unsigned int old_val, new_val;
//old_val = *(unsigned int *)0x100702c;
//*(unsigned int *)0x100702c = 4;
//new_val = *(unsigned int *)0x100702c;

// PatchByte(0x100702c, 0x04)


