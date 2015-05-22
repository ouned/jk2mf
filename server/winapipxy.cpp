/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#include <windows.h>
#include <shlobj.h>
#include "winapipxy.h"
#include "main.h"

#ifdef WINAPIPRX

HMODULE winmmdll; // handles original winmm.dll
FARPROC pointers[192]; // pointers to original addresses

// ==============
// DllMain
//
// Main entrypoint for Windows hosts
// ==============
BOOL WINAPI DllMain(HANDLE hDll, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
			InitWinApiProxy();
			Init();
			break;

        case DLL_PROCESS_DETACH:
            DetachWinApiProxy();
            break;
    }
    
	return TRUE;
}

// ==============
// InitWinApiProxy & DetachWinApiProxy
//
// Gets addresses of the original winmm.dll
// DetachWinApiProxy gets called when winmm gets unloaded
// ==============
void InitWinApiProxy()
{
	char winmmDir[MAX_PATH];

	SHGetFolderPathA(NULL, CSIDL_SYSTEMX86, NULL, 0, winmmDir);
	strcat_s(winmmDir, "\\winmm.dll");
	winmmdll = LoadLibraryA(winmmDir);

	pointers[0] = GetProcAddress(winmmdll, "CloseDriver");
	pointers[1] = GetProcAddress(winmmdll, "DefDriverProc");
	pointers[2] = GetProcAddress(winmmdll, "DriverCallback");
	pointers[3] = GetProcAddress(winmmdll, "DrvGetModuleHandle");
	pointers[4] = GetProcAddress(winmmdll, "GetDriverModuleHandle");
	pointers[5] = GetProcAddress(winmmdll, "NotifyCallbackData");
	pointers[6] = GetProcAddress(winmmdll, "OpenDriver");
	pointers[7] = GetProcAddress(winmmdll, "PlaySound");
	pointers[8] = GetProcAddress(winmmdll, "PlaySoundA");
	pointers[9] = GetProcAddress(winmmdll, "PlaySoundW");
	pointers[10] = GetProcAddress(winmmdll, "SendDriverMessage");
	pointers[11] = GetProcAddress(winmmdll, "WOW32DriverCallback");
	pointers[12] = GetProcAddress(winmmdll, "WOW32ResolveMultiMediaHandle");
	pointers[13] = GetProcAddress(winmmdll, "WOWAppExit");
	pointers[14] = GetProcAddress(winmmdll, "aux32Message");
	pointers[15] = GetProcAddress(winmmdll, "auxGetDevCapsA");
	pointers[16] = GetProcAddress(winmmdll, "auxGetDevCapsW");
	pointers[17] = GetProcAddress(winmmdll, "auxGetNumDevs");
	pointers[18] = GetProcAddress(winmmdll, "auxGetVolume");
	pointers[19] = GetProcAddress(winmmdll, "auxOutMessage");
	pointers[20] = GetProcAddress(winmmdll, "auxSetVolume");
	pointers[21] = GetProcAddress(winmmdll, "joy32Message");
	pointers[22] = GetProcAddress(winmmdll, "joyConfigChanged");
	pointers[23] = GetProcAddress(winmmdll, "joyGetDevCapsA");
	pointers[24] = GetProcAddress(winmmdll, "joyGetDevCapsW");
	pointers[25] = GetProcAddress(winmmdll, "joyGetNumDevs");
	pointers[26] = GetProcAddress(winmmdll, "joyGetPos");
	pointers[27] = GetProcAddress(winmmdll, "joyGetPosEx");
	pointers[28] = GetProcAddress(winmmdll, "joyGetThreshold");
	pointers[29] = GetProcAddress(winmmdll, "joyReleaseCapture");
	pointers[30] = GetProcAddress(winmmdll, "joySetCapture");
	pointers[31] = GetProcAddress(winmmdll, "joySetThreshold");
	pointers[32] = GetProcAddress(winmmdll, "mci32Message");
	pointers[33] = GetProcAddress(winmmdll, "mciDriverNotify");
	pointers[34] = GetProcAddress(winmmdll, "mciDriverYield");
	pointers[35] = GetProcAddress(winmmdll, "mciExecute");
	pointers[36] = GetProcAddress(winmmdll, "mciFreeCommandResource");
	pointers[37] = GetProcAddress(winmmdll, "mciGetCreatorTask");
	pointers[38] = GetProcAddress(winmmdll, "mciGetDeviceIDA");
	pointers[39] = GetProcAddress(winmmdll, "mciGetDeviceIDFromElementIDA");
	pointers[40] = GetProcAddress(winmmdll, "mciGetDeviceIDFromElementIDW");
	pointers[41] = GetProcAddress(winmmdll, "mciGetDeviceIDW");
	pointers[42] = GetProcAddress(winmmdll, "mciGetDriverData");
	pointers[43] = GetProcAddress(winmmdll, "mciGetErrorStringA");
	pointers[44] = GetProcAddress(winmmdll, "mciGetErrorStringW");
	pointers[45] = GetProcAddress(winmmdll, "mciGetYieldProc");
	pointers[46] = GetProcAddress(winmmdll, "mciLoadCommandResource");
	pointers[47] = GetProcAddress(winmmdll, "mciSendCommandA");
	pointers[48] = GetProcAddress(winmmdll, "mciSendCommandW");
	pointers[49] = GetProcAddress(winmmdll, "mciSendStringA");
	pointers[50] = GetProcAddress(winmmdll, "mciSendStringW");
	pointers[51] = GetProcAddress(winmmdll, "mciSetDriverData");
	pointers[52] = GetProcAddress(winmmdll, "mciSetYieldProc");
	pointers[53] = GetProcAddress(winmmdll, "mid32Message");
	pointers[54] = GetProcAddress(winmmdll, "midiConnect");
	pointers[55] = GetProcAddress(winmmdll, "midiDisconnect");
	pointers[56] = GetProcAddress(winmmdll, "midiInAddBuffer");
	pointers[57] = GetProcAddress(winmmdll, "midiInClose");
	pointers[58] = GetProcAddress(winmmdll, "midiInGetDevCapsA");
	pointers[59] = GetProcAddress(winmmdll, "midiInGetDevCapsW");
	pointers[60] = GetProcAddress(winmmdll, "midiInGetErrorTextA");
	pointers[61] = GetProcAddress(winmmdll, "midiInGetErrorTextW");
	pointers[62] = GetProcAddress(winmmdll, "midiInGetID");
	pointers[63] = GetProcAddress(winmmdll, "midiInGetNumDevs");
	pointers[64] = GetProcAddress(winmmdll, "midiInMessage");
	pointers[65] = GetProcAddress(winmmdll, "midiInOpen");
	pointers[66] = GetProcAddress(winmmdll, "midiInPrepareHeader");
	pointers[67] = GetProcAddress(winmmdll, "midiInReset");
	pointers[68] = GetProcAddress(winmmdll, "midiInStart");
	pointers[69] = GetProcAddress(winmmdll, "midiInStop");
	pointers[70] = GetProcAddress(winmmdll, "midiInUnprepareHeader");
	pointers[71] = GetProcAddress(winmmdll, "midiOutCacheDrumPatches");
	pointers[72] = GetProcAddress(winmmdll, "midiOutCachePatches");
	pointers[73] = GetProcAddress(winmmdll, "midiOutClose");
	pointers[74] = GetProcAddress(winmmdll, "midiOutGetDevCapsA");
	pointers[75] = GetProcAddress(winmmdll, "midiOutGetDevCapsW");
	pointers[76] = GetProcAddress(winmmdll, "midiOutGetErrorTextA");
	pointers[77] = GetProcAddress(winmmdll, "midiOutGetErrorTextW");
	pointers[78] = GetProcAddress(winmmdll, "midiOutGetID");
	pointers[79] = GetProcAddress(winmmdll, "midiOutGetNumDevs");
	pointers[80] = GetProcAddress(winmmdll, "midiOutGetVolume");
	pointers[81] = GetProcAddress(winmmdll, "midiOutLongMsg");
	pointers[82] = GetProcAddress(winmmdll, "midiOutMessage");
	pointers[83] = GetProcAddress(winmmdll, "midiOutOpen");
	pointers[84] = GetProcAddress(winmmdll, "midiOutPrepareHeader");
	pointers[85] = GetProcAddress(winmmdll, "midiOutReset");
	pointers[86] = GetProcAddress(winmmdll, "midiOutSetVolume");
	pointers[87] = GetProcAddress(winmmdll, "midiOutShortMsg");
	pointers[88] = GetProcAddress(winmmdll, "midiOutUnprepareHeader");
	pointers[89] = GetProcAddress(winmmdll, "midiStreamClose");
	pointers[90] = GetProcAddress(winmmdll, "midiStreamOpen");
	pointers[91] = GetProcAddress(winmmdll, "midiStreamOut");
	pointers[92] = GetProcAddress(winmmdll, "midiStreamPause");
	pointers[93] = GetProcAddress(winmmdll, "midiStreamPosition");
	pointers[94] = GetProcAddress(winmmdll, "midiStreamProperty");
	pointers[95] = GetProcAddress(winmmdll, "midiStreamRestart");
	pointers[96] = GetProcAddress(winmmdll, "midiStreamStop");
	pointers[97] = GetProcAddress(winmmdll, "mixerClose");
	pointers[98] = GetProcAddress(winmmdll, "mixerGetControlDetailsA");
	pointers[99] = GetProcAddress(winmmdll, "mixerGetControlDetailsW");
	pointers[100] = GetProcAddress(winmmdll, "mixerGetDevCapsA");
	pointers[101] = GetProcAddress(winmmdll, "mixerGetDevCapsW");
	pointers[102] = GetProcAddress(winmmdll, "mixerGetID");
	pointers[103] = GetProcAddress(winmmdll, "mixerGetLineControlsA");
	pointers[104] = GetProcAddress(winmmdll, "mixerGetLineControlsW");
	pointers[105] = GetProcAddress(winmmdll, "mixerGetLineInfoA");
	pointers[106] = GetProcAddress(winmmdll, "mixerGetLineInfoW");
	pointers[107] = GetProcAddress(winmmdll, "mixerGetNumDevs");
	pointers[108] = GetProcAddress(winmmdll, "mixerMessage");
	pointers[109] = GetProcAddress(winmmdll, "mixerOpen");
	pointers[110] = GetProcAddress(winmmdll, "mixerSetControlDetails");
	pointers[111] = GetProcAddress(winmmdll, "mmDrvInstall");
	pointers[112] = GetProcAddress(winmmdll, "mmGetCurrentTask");
	pointers[113] = GetProcAddress(winmmdll, "mmTaskBlock");
	pointers[114] = GetProcAddress(winmmdll, "mmTaskCreate");
	pointers[115] = GetProcAddress(winmmdll, "mmTaskSignal");
	pointers[116] = GetProcAddress(winmmdll, "mmTaskYield");
	pointers[117] = GetProcAddress(winmmdll, "mmioAdvance");
	pointers[118] = GetProcAddress(winmmdll, "mmioAscend");
	pointers[119] = GetProcAddress(winmmdll, "mmioClose");
	pointers[120] = GetProcAddress(winmmdll, "mmioCreateChunk");
	pointers[121] = GetProcAddress(winmmdll, "mmioDescend");
	pointers[122] = GetProcAddress(winmmdll, "mmioFlush");
	pointers[123] = GetProcAddress(winmmdll, "mmioGetInfo");
	pointers[124] = GetProcAddress(winmmdll, "mmioInstallIOProcA");
	pointers[125] = GetProcAddress(winmmdll, "mmioInstallIOProcW");
	pointers[126] = GetProcAddress(winmmdll, "mmioOpenA");
	pointers[127] = GetProcAddress(winmmdll, "mmioOpenW");
	pointers[128] = GetProcAddress(winmmdll, "mmioRead");
	pointers[129] = GetProcAddress(winmmdll, "mmioRenameA");
	pointers[130] = GetProcAddress(winmmdll, "mmioRenameW");
	pointers[131] = GetProcAddress(winmmdll, "mmioSeek");
	pointers[132] = GetProcAddress(winmmdll, "mmioSendMessage");
	pointers[133] = GetProcAddress(winmmdll, "mmioSetBuffer");
	pointers[134] = GetProcAddress(winmmdll, "mmioSetInfo");
	pointers[135] = GetProcAddress(winmmdll, "mmioStringToFOURCCA");
	pointers[136] = GetProcAddress(winmmdll, "mmioStringToFOURCCW");
	pointers[137] = GetProcAddress(winmmdll, "mmioWrite");
	pointers[138] = GetProcAddress(winmmdll, "mmsystemGetVersion");
	pointers[139] = GetProcAddress(winmmdll, "mod32Message");
	pointers[140] = GetProcAddress(winmmdll, "mxd32Message");
	pointers[141] = GetProcAddress(winmmdll, "sndPlaySoundA");
	pointers[142] = GetProcAddress(winmmdll, "sndPlaySoundW");
	pointers[143] = GetProcAddress(winmmdll, "tid32Message");
	pointers[144] = GetProcAddress(winmmdll, "timeBeginPeriod");
	pointers[145] = GetProcAddress(winmmdll, "timeEndPeriod");
	pointers[146] = GetProcAddress(winmmdll, "timeGetDevCaps");
	pointers[147] = GetProcAddress(winmmdll, "timeGetSystemTime");
	pointers[148] = GetProcAddress(winmmdll, "timeGetTime");
	pointers[149] = GetProcAddress(winmmdll, "timeKillEvent");
	pointers[150] = GetProcAddress(winmmdll, "timeSetEvent");
	pointers[151] = GetProcAddress(winmmdll, "waveInAddBuffer");
	pointers[152] = GetProcAddress(winmmdll, "waveInClose");
	pointers[153] = GetProcAddress(winmmdll, "waveInGetDevCapsA");
	pointers[154] = GetProcAddress(winmmdll, "waveInGetDevCapsW");
	pointers[155] = GetProcAddress(winmmdll, "waveInGetErrorTextA");
	pointers[156] = GetProcAddress(winmmdll, "waveInGetErrorTextW");
	pointers[157] = GetProcAddress(winmmdll, "waveInGetID");
	pointers[158] = GetProcAddress(winmmdll, "waveInGetNumDevs");
	pointers[159] = GetProcAddress(winmmdll, "waveInGetPosition");
	pointers[160] = GetProcAddress(winmmdll, "waveInMessage");
	pointers[161] = GetProcAddress(winmmdll, "waveInOpen");
	pointers[162] = GetProcAddress(winmmdll, "waveInPrepareHeader");
	pointers[163] = GetProcAddress(winmmdll, "waveInReset");
	pointers[164] = GetProcAddress(winmmdll, "waveInStart");
	pointers[165] = GetProcAddress(winmmdll, "waveInStop");
	pointers[166] = GetProcAddress(winmmdll, "waveInUnprepareHeader");
	pointers[167] = GetProcAddress(winmmdll, "waveOutBreakLoop");
	pointers[168] = GetProcAddress(winmmdll, "waveOutClose");
	pointers[169] = GetProcAddress(winmmdll, "waveOutGetDevCapsA");
	pointers[170] = GetProcAddress(winmmdll, "waveOutGetDevCapsW");
	pointers[171] = GetProcAddress(winmmdll, "waveOutGetErrorTextA");
	pointers[172] = GetProcAddress(winmmdll, "waveOutGetErrorTextW");
	pointers[173] = GetProcAddress(winmmdll, "waveOutGetID");
	pointers[174] = GetProcAddress(winmmdll, "waveOutGetNumDevs");
	pointers[175] = GetProcAddress(winmmdll, "waveOutGetPitch");
	pointers[176] = GetProcAddress(winmmdll, "waveOutGetPlaybackRate");
	pointers[177] = GetProcAddress(winmmdll, "waveOutGetPosition");
	pointers[178] = GetProcAddress(winmmdll, "waveOutGetVolume");
	pointers[179] = GetProcAddress(winmmdll, "waveOutMessage");
	pointers[180] = GetProcAddress(winmmdll, "waveOutOpen");
	pointers[181] = GetProcAddress(winmmdll, "waveOutPause");
	pointers[182] = GetProcAddress(winmmdll, "waveOutPrepareHeader");
	pointers[183] = GetProcAddress(winmmdll, "waveOutReset");
	pointers[184] = GetProcAddress(winmmdll, "waveOutRestart");
	pointers[185] = GetProcAddress(winmmdll, "waveOutSetPitch");
	pointers[186] = GetProcAddress(winmmdll, "waveOutSetPlaybackRate");
	pointers[187] = GetProcAddress(winmmdll, "waveOutSetVolume");
	pointers[188] = GetProcAddress(winmmdll, "waveOutUnprepareHeader");
	pointers[189] = GetProcAddress(winmmdll, "waveOutWrite");
	pointers[190] = GetProcAddress(winmmdll, "wid32Message");
	pointers[191] = GetProcAddress(winmmdll, "wod32Message");
}

void DetachWinApiProxy()
{
	FreeLibrary(winmmdll);
}

// ==============
// Exported (faked) WINAPI Procedures
// ==============
// CloseDriver
extern "C" __declspec(naked) void __stdcall __E__0__()
{
	__asm jmp pointers[0*4];
}
// DefDriverProc
extern "C" __declspec(naked) void __stdcall __E__1__()
{
	__asm jmp pointers[1*4];
}
// DriverCallback
extern "C" __declspec(naked) void __stdcall __E__2__()
{
	__asm jmp pointers[2*4];
}
// DrvGetModuleHandle
extern "C" __declspec(naked) void __stdcall __E__3__()
{
	__asm jmp pointers[3*4];
}
// GetDriverModuleHandle
extern "C" __declspec(naked) void __stdcall __E__4__()
{
	__asm jmp pointers[4*4];
}
// NotifyCallbackData
extern "C" __declspec(naked) void __stdcall __E__5__()
{
	__asm jmp pointers[5*4];
}
// OpenDriver
extern "C" __declspec(naked) void __stdcall __E__6__()
{
	__asm jmp pointers[6*4];
}
// PlaySound
extern "C" __declspec(naked) void __stdcall __E__7__()
{
	__asm jmp pointers[7*4];
}
// PlaySoundA
extern "C" __declspec(naked) void __stdcall __E__8__()
{
	__asm jmp pointers[8*4];
}
// PlaySoundW
extern "C" __declspec(naked) void __stdcall __E__9__()
{
	__asm jmp pointers[9*4];
}
// SendDriverMessage
extern "C" __declspec(naked) void __stdcall __E__10__()
{
	__asm jmp pointers[10*4];
}
// WOW32DriverCallback
extern "C" __declspec(naked) void __stdcall __E__11__()
{
	__asm jmp pointers[11*4];
}
// WOW32ResolveMultiMediaHandle
extern "C" __declspec(naked) void __stdcall __E__12__()
{
	__asm jmp pointers[12*4];
}
// WOWAppExit
extern "C" __declspec(naked) void __stdcall __E__13__()
{
	__asm jmp pointers[13*4];
}
// aux32Message
extern "C" __declspec(naked) void __stdcall __E__14__()
{
	__asm jmp pointers[14*4];
}
// auxGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__15__()
{
	__asm jmp pointers[15*4];
}
// auxGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__16__()
{
	__asm jmp pointers[16*4];
}
// auxGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__17__()
{
	__asm jmp pointers[17*4];
}
// auxGetVolume
extern "C" __declspec(naked) void __stdcall __E__18__()
{
	__asm jmp pointers[18*4];
}
// auxOutMessage
extern "C" __declspec(naked) void __stdcall __E__19__()
{
	__asm jmp pointers[19*4];
}
// auxSetVolume
extern "C" __declspec(naked) void __stdcall __E__20__()
{
	__asm jmp pointers[20*4];
}
// joy32Message
extern "C" __declspec(naked) void __stdcall __E__21__()
{
	__asm jmp pointers[21*4];
}
// joyConfigChanged
extern "C" __declspec(naked) void __stdcall __E__22__()
{
	__asm jmp pointers[22*4];
}
// joyGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__23__()
{
	__asm jmp pointers[23*4];
}
// joyGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__24__()
{
	__asm jmp pointers[24*4];
}
// joyGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__25__()
{
	__asm jmp pointers[25*4];
}
// joyGetPos
extern "C" __declspec(naked) void __stdcall __E__26__()
{
	__asm jmp pointers[26*4];
}
// joyGetPosEx
extern "C" __declspec(naked) void __stdcall __E__27__()
{
	__asm jmp pointers[27*4];
}
// joyGetThreshold
extern "C" __declspec(naked) void __stdcall __E__28__()
{
	__asm jmp pointers[28*4];
}
// joyReleaseCapture
extern "C" __declspec(naked) void __stdcall __E__29__()
{
	__asm jmp pointers[29*4];
}
// joySetCapture
extern "C" __declspec(naked) void __stdcall __E__30__()
{
	__asm jmp pointers[30*4];
}
// joySetThreshold
extern "C" __declspec(naked) void __stdcall __E__31__()
{
	__asm jmp pointers[31*4];
}
// mci32Message
extern "C" __declspec(naked) void __stdcall __E__32__()
{
	__asm jmp pointers[32*4];
}
// mciDriverNotify
extern "C" __declspec(naked) void __stdcall __E__33__()
{
	__asm jmp pointers[33*4];
}
// mciDriverYield
extern "C" __declspec(naked) void __stdcall __E__34__()
{
	__asm jmp pointers[34*4];
}
// mciExecute
extern "C" __declspec(naked) void __stdcall __E__35__()
{
	__asm jmp pointers[35*4];
}
// mciFreeCommandResource
extern "C" __declspec(naked) void __stdcall __E__36__()
{
	__asm jmp pointers[36*4];
}
// mciGetCreatorTask
extern "C" __declspec(naked) void __stdcall __E__37__()
{
	__asm jmp pointers[37*4];
}
// mciGetDeviceIDA
extern "C" __declspec(naked) void __stdcall __E__38__()
{
	__asm jmp pointers[38*4];
}
// mciGetDeviceIDFromElementIDA
extern "C" __declspec(naked) void __stdcall __E__39__()
{
	__asm jmp pointers[39*4];
}
// mciGetDeviceIDFromElementIDW
extern "C" __declspec(naked) void __stdcall __E__40__()
{
	__asm jmp pointers[40*4];
}
// mciGetDeviceIDW
extern "C" __declspec(naked) void __stdcall __E__41__()
{
	__asm jmp pointers[41*4];
}
// mciGetDriverData
extern "C" __declspec(naked) void __stdcall __E__42__()
{
	__asm jmp pointers[42*4];
}
// mciGetErrorStringA
extern "C" __declspec(naked) void __stdcall __E__43__()
{
	__asm jmp pointers[43*4];
}
// mciGetErrorStringW
extern "C" __declspec(naked) void __stdcall __E__44__()
{
	__asm jmp pointers[44*4];
}
// mciGetYieldProc
extern "C" __declspec(naked) void __stdcall __E__45__()
{
	__asm jmp pointers[45*4];
}
// mciLoadCommandResource
extern "C" __declspec(naked) void __stdcall __E__46__()
{
	__asm jmp pointers[46*4];
}
// mciSendCommandA
extern "C" __declspec(naked) void __stdcall __E__47__()
{
	__asm jmp pointers[47*4];
}
// mciSendCommandW
extern "C" __declspec(naked) void __stdcall __E__48__()
{
	__asm jmp pointers[48*4];
}
// mciSendStringA
extern "C" __declspec(naked) void __stdcall __E__49__()
{
	__asm jmp pointers[49*4];
}
// mciSendStringW
extern "C" __declspec(naked) void __stdcall __E__50__()
{
	__asm jmp pointers[50*4];
}
// mciSetDriverData
extern "C" __declspec(naked) void __stdcall __E__51__()
{
	__asm jmp pointers[51*4];
}
// mciSetYieldProc
extern "C" __declspec(naked) void __stdcall __E__52__()
{
	__asm jmp pointers[52*4];
}
// mid32Message
extern "C" __declspec(naked) void __stdcall __E__53__()
{
	__asm jmp pointers[53*4];
}
// midiConnect
extern "C" __declspec(naked) void __stdcall __E__54__()
{
	__asm jmp pointers[54*4];
}
// midiDisconnect
extern "C" __declspec(naked) void __stdcall __E__55__()
{
	__asm jmp pointers[55*4];
}
// midiInAddBuffer
extern "C" __declspec(naked) void __stdcall __E__56__()
{
	__asm jmp pointers[56*4];
}
// midiInClose
extern "C" __declspec(naked) void __stdcall __E__57__()
{
	__asm jmp pointers[57*4];
}
// midiInGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__58__()
{
	__asm jmp pointers[58*4];
}
// midiInGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__59__()
{
	__asm jmp pointers[59*4];
}
// midiInGetErrorTextA
extern "C" __declspec(naked) void __stdcall __E__60__()
{
	__asm jmp pointers[60*4];
}
// midiInGetErrorTextW
extern "C" __declspec(naked) void __stdcall __E__61__()
{
	__asm jmp pointers[61*4];
}
// midiInGetID
extern "C" __declspec(naked) void __stdcall __E__62__()
{
	__asm jmp pointers[62*4];
}
// midiInGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__63__()
{
	__asm jmp pointers[63*4];
}
// midiInMessage
extern "C" __declspec(naked) void __stdcall __E__64__()
{
	__asm jmp pointers[64*4];
}
// midiInOpen
extern "C" __declspec(naked) void __stdcall __E__65__()
{
	__asm jmp pointers[65*4];
}
// midiInPrepareHeader
extern "C" __declspec(naked) void __stdcall __E__66__()
{
	__asm jmp pointers[66*4];
}
// midiInReset
extern "C" __declspec(naked) void __stdcall __E__67__()
{
	__asm jmp pointers[67*4];
}
// midiInStart
extern "C" __declspec(naked) void __stdcall __E__68__()
{
	__asm jmp pointers[68*4];
}
// midiInStop
extern "C" __declspec(naked) void __stdcall __E__69__()
{
	__asm jmp pointers[69*4];
}
// midiInUnprepareHeader
extern "C" __declspec(naked) void __stdcall __E__70__()
{
	__asm jmp pointers[70*4];
}
// midiOutCacheDrumPatches
extern "C" __declspec(naked) void __stdcall __E__71__()
{
	__asm jmp pointers[71*4];
}
// midiOutCachePatches
extern "C" __declspec(naked) void __stdcall __E__72__()
{
	__asm jmp pointers[72*4];
}
// midiOutClose
extern "C" __declspec(naked) void __stdcall __E__73__()
{
	__asm jmp pointers[73*4];
}
// midiOutGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__74__()
{
	__asm jmp pointers[74*4];
}
// midiOutGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__75__()
{
	__asm jmp pointers[75*4];
}
// midiOutGetErrorTextA
extern "C" __declspec(naked) void __stdcall __E__76__()
{
	__asm jmp pointers[76*4];
}
// midiOutGetErrorTextW
extern "C" __declspec(naked) void __stdcall __E__77__()
{
	__asm jmp pointers[77*4];
}
// midiOutGetID
extern "C" __declspec(naked) void __stdcall __E__78__()
{
	__asm jmp pointers[78*4];
}
// midiOutGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__79__()
{
	__asm jmp pointers[79*4];
}
// midiOutGetVolume
extern "C" __declspec(naked) void __stdcall __E__80__()
{
	__asm jmp pointers[80*4];
}
// midiOutLongMsg
extern "C" __declspec(naked) void __stdcall __E__81__()
{
	__asm jmp pointers[81*4];
}
// midiOutMessage
extern "C" __declspec(naked) void __stdcall __E__82__()
{
	__asm jmp pointers[82*4];
}
// midiOutOpen
extern "C" __declspec(naked) void __stdcall __E__83__()
{
	__asm jmp pointers[83*4];
}
// midiOutPrepareHeader
extern "C" __declspec(naked) void __stdcall __E__84__()
{
	__asm jmp pointers[84*4];
}
// midiOutReset
extern "C" __declspec(naked) void __stdcall __E__85__()
{
	__asm jmp pointers[85*4];
}
// midiOutSetVolume
extern "C" __declspec(naked) void __stdcall __E__86__()
{
	__asm jmp pointers[86*4];
}
// midiOutShortMsg
extern "C" __declspec(naked) void __stdcall __E__87__()
{
	__asm jmp pointers[87*4];
}
// midiOutUnprepareHeader
extern "C" __declspec(naked) void __stdcall __E__88__()
{
	__asm jmp pointers[88*4];
}
// midiStreamClose
extern "C" __declspec(naked) void __stdcall __E__89__()
{
	__asm jmp pointers[89*4];
}
// midiStreamOpen
extern "C" __declspec(naked) void __stdcall __E__90__()
{
	__asm jmp pointers[90*4];
}
// midiStreamOut
extern "C" __declspec(naked) void __stdcall __E__91__()
{
	__asm jmp pointers[91*4];
}
// midiStreamPause
extern "C" __declspec(naked) void __stdcall __E__92__()
{
	__asm jmp pointers[92*4];
}
// midiStreamPosition
extern "C" __declspec(naked) void __stdcall __E__93__()
{
	__asm jmp pointers[93*4];
}
// midiStreamProperty
extern "C" __declspec(naked) void __stdcall __E__94__()
{
	__asm jmp pointers[94*4];
}
// midiStreamRestart
extern "C" __declspec(naked) void __stdcall __E__95__()
{
	__asm jmp pointers[95*4];
}
// midiStreamStop
extern "C" __declspec(naked) void __stdcall __E__96__()
{
	__asm jmp pointers[96*4];
}
// mixerClose
extern "C" __declspec(naked) void __stdcall __E__97__()
{
	__asm jmp pointers[97*4];
}
// mixerGetControlDetailsA
extern "C" __declspec(naked) void __stdcall __E__98__()
{
	__asm jmp pointers[98*4];
}
// mixerGetControlDetailsW
extern "C" __declspec(naked) void __stdcall __E__99__()
{
	__asm jmp pointers[99*4];
}
// mixerGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__100__()
{
	__asm jmp pointers[100*4];
}
// mixerGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__101__()
{
	__asm jmp pointers[101*4];
}
// mixerGetID
extern "C" __declspec(naked) void __stdcall __E__102__()
{
	__asm jmp pointers[102*4];
}
// mixerGetLineControlsA
extern "C" __declspec(naked) void __stdcall __E__103__()
{
	__asm jmp pointers[103*4];
}
// mixerGetLineControlsW
extern "C" __declspec(naked) void __stdcall __E__104__()
{
	__asm jmp pointers[104*4];
}
// mixerGetLineInfoA
extern "C" __declspec(naked) void __stdcall __E__105__()
{
	__asm jmp pointers[105*4];
}
// mixerGetLineInfoW
extern "C" __declspec(naked) void __stdcall __E__106__()
{
	__asm jmp pointers[106*4];
}
// mixerGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__107__()
{
	__asm jmp pointers[107*4];
}
// mixerMessage
extern "C" __declspec(naked) void __stdcall __E__108__()
{
	__asm jmp pointers[108*4];
}
// mixerOpen
extern "C" __declspec(naked) void __stdcall __E__109__()
{
	__asm jmp pointers[109*4];
}
// mixerSetControlDetails
extern "C" __declspec(naked) void __stdcall __E__110__()
{
	__asm jmp pointers[110*4];
}
// mmDrvInstall
extern "C" __declspec(naked) void __stdcall __E__111__()
{
	__asm jmp pointers[111*4];
}
// mmGetCurrentTask
extern "C" __declspec(naked) void __stdcall __E__112__()
{
	__asm jmp pointers[112*4];
}
// mmTaskBlock
extern "C" __declspec(naked) void __stdcall __E__113__()
{
	__asm jmp pointers[113*4];
}
// mmTaskCreate
extern "C" __declspec(naked) void __stdcall __E__114__()
{
	__asm jmp pointers[114*4];
}
// mmTaskSignal
extern "C" __declspec(naked) void __stdcall __E__115__()
{
	__asm jmp pointers[115*4];
}
// mmTaskYield
extern "C" __declspec(naked) void __stdcall __E__116__()
{
	__asm jmp pointers[116*4];
}
// mmioAdvance
extern "C" __declspec(naked) void __stdcall __E__117__()
{
	__asm jmp pointers[117*4];
}
// mmioAscend
extern "C" __declspec(naked) void __stdcall __E__118__()
{
	__asm jmp pointers[118*4];
}
// mmioClose
extern "C" __declspec(naked) void __stdcall __E__119__()
{
	__asm jmp pointers[119*4];
}
// mmioCreateChunk
extern "C" __declspec(naked) void __stdcall __E__120__()
{
	__asm jmp pointers[120*4];
}
// mmioDescend
extern "C" __declspec(naked) void __stdcall __E__121__()
{
	__asm jmp pointers[121*4];
}
// mmioFlush
extern "C" __declspec(naked) void __stdcall __E__122__()
{
	__asm jmp pointers[122*4];
}
// mmioGetInfo
extern "C" __declspec(naked) void __stdcall __E__123__()
{
	__asm jmp pointers[123*4];
}
// mmioInstallIOProcA
extern "C" __declspec(naked) void __stdcall __E__124__()
{
	__asm jmp pointers[124*4];
}
// mmioInstallIOProcW
extern "C" __declspec(naked) void __stdcall __E__125__()
{
	__asm jmp pointers[125*4];
}
// mmioOpenA
extern "C" __declspec(naked) void __stdcall __E__126__()
{
	__asm jmp pointers[126*4];
}
// mmioOpenW
extern "C" __declspec(naked) void __stdcall __E__127__()
{
	__asm jmp pointers[127*4];
}
// mmioRead
extern "C" __declspec(naked) void __stdcall __E__128__()
{
	__asm jmp pointers[128*4];
}
// mmioRenameA
extern "C" __declspec(naked) void __stdcall __E__129__()
{
	__asm jmp pointers[129*4];
}
// mmioRenameW
extern "C" __declspec(naked) void __stdcall __E__130__()
{
	__asm jmp pointers[130*4];
}
// mmioSeek
extern "C" __declspec(naked) void __stdcall __E__131__()
{
	__asm jmp pointers[131*4];
}
// mmioSendMessage
extern "C" __declspec(naked) void __stdcall __E__132__()
{
	__asm jmp pointers[132*4];
}
// mmioSetBuffer
extern "C" __declspec(naked) void __stdcall __E__133__()
{
	__asm jmp pointers[133*4];
}
// mmioSetInfo
extern "C" __declspec(naked) void __stdcall __E__134__()
{
	__asm jmp pointers[134*4];
}
// mmioStringToFOURCCA
extern "C" __declspec(naked) void __stdcall __E__135__()
{
	__asm jmp pointers[135*4];
}
// mmioStringToFOURCCW
extern "C" __declspec(naked) void __stdcall __E__136__()
{
	__asm jmp pointers[136*4];
}
// mmioWrite
extern "C" __declspec(naked) void __stdcall __E__137__()
{
	__asm jmp pointers[137*4];
}
// mmsystemGetVersion
extern "C" __declspec(naked) void __stdcall __E__138__()
{
	__asm jmp pointers[138*4];
}
// mod32Message
extern "C" __declspec(naked) void __stdcall __E__139__()
{
	__asm jmp pointers[139*4];
}
// mxd32Message
extern "C" __declspec(naked) void __stdcall __E__140__()
{
	__asm jmp pointers[140*4];
}
// sndPlaySoundA
extern "C" __declspec(naked) void __stdcall __E__141__()
{
	__asm jmp pointers[141*4];
}
// sndPlaySoundW
extern "C" __declspec(naked) void __stdcall __E__142__()
{
	__asm jmp pointers[142*4];
}
// tid32Message
extern "C" __declspec(naked) void __stdcall __E__143__()
{
	__asm jmp pointers[143*4];
}
// timeBeginPeriod
extern "C" __declspec(naked) void __stdcall __E__144__()
{
	__asm jmp pointers[144*4];
}
// timeEndPeriod
extern "C" __declspec(naked) void __stdcall __E__145__()
{
	__asm jmp pointers[145*4];
}
// timeGetDevCaps
extern "C" __declspec(naked) void __stdcall __E__146__()
{
	__asm jmp pointers[146*4];
}
// timeGetSystemTime
extern "C" __declspec(naked) void __stdcall __E__147__()
{
	__asm jmp pointers[147*4];
}
// timeGetTime
extern "C" __declspec(naked) void __stdcall __E__148__()
{
	__asm jmp pointers[148*4];
}
// timeKillEvent
extern "C" __declspec(naked) void __stdcall __E__149__()
{
	__asm jmp pointers[149*4];
}
// timeSetEvent
extern "C" __declspec(naked) void __stdcall __E__150__()
{
	__asm jmp pointers[150*4];
}
// waveInAddBuffer
extern "C" __declspec(naked) void __stdcall __E__151__()
{
	__asm jmp pointers[151*4];
}
// waveInClose
extern "C" __declspec(naked) void __stdcall __E__152__()
{
	__asm jmp pointers[152*4];
}
// waveInGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__153__()
{
	__asm jmp pointers[153*4];
}
// waveInGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__154__()
{
	__asm jmp pointers[154*4];
}
// waveInGetErrorTextA
extern "C" __declspec(naked) void __stdcall __E__155__()
{
	__asm jmp pointers[155*4];
}
// waveInGetErrorTextW
extern "C" __declspec(naked) void __stdcall __E__156__()
{
	__asm jmp pointers[156*4];
}
// waveInGetID
extern "C" __declspec(naked) void __stdcall __E__157__()
{
	__asm jmp pointers[157*4];
}
// waveInGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__158__()
{
	__asm jmp pointers[158*4];
}
// waveInGetPosition
extern "C" __declspec(naked) void __stdcall __E__159__()
{
	__asm jmp pointers[159*4];
}
// waveInMessage
extern "C" __declspec(naked) void __stdcall __E__160__()
{
	__asm jmp pointers[160*4];
}
// waveInOpen
extern "C" __declspec(naked) void __stdcall __E__161__()
{
	__asm jmp pointers[161*4];
}
// waveInPrepareHeader
extern "C" __declspec(naked) void __stdcall __E__162__()
{
	__asm jmp pointers[162*4];
}
// waveInReset
extern "C" __declspec(naked) void __stdcall __E__163__()
{
	__asm jmp pointers[163*4];
}
// waveInStart
extern "C" __declspec(naked) void __stdcall __E__164__()
{
	__asm jmp pointers[164*4];
}
// waveInStop
extern "C" __declspec(naked) void __stdcall __E__165__()
{
	__asm jmp pointers[165*4];
}
// waveInUnprepareHeader
extern "C" __declspec(naked) void __stdcall __E__166__()
{
	__asm jmp pointers[166*4];
}
// waveOutBreakLoop
extern "C" __declspec(naked) void __stdcall __E__167__()
{
	__asm jmp pointers[167*4];
}
// waveOutClose
extern "C" __declspec(naked) void __stdcall __E__168__()
{
	__asm jmp pointers[168*4];
}
// waveOutGetDevCapsA
extern "C" __declspec(naked) void __stdcall __E__169__()
{
	__asm jmp pointers[169*4];
}
// waveOutGetDevCapsW
extern "C" __declspec(naked) void __stdcall __E__170__()
{
	__asm jmp pointers[170*4];
}
// waveOutGetErrorTextA
extern "C" __declspec(naked) void __stdcall __E__171__()
{
	__asm jmp pointers[171*4];
}
// waveOutGetErrorTextW
extern "C" __declspec(naked) void __stdcall __E__172__()
{
	__asm jmp pointers[172*4];
}
// waveOutGetID
extern "C" __declspec(naked) void __stdcall __E__173__()
{
	__asm jmp pointers[173*4];
}
// waveOutGetNumDevs
extern "C" __declspec(naked) void __stdcall __E__174__()
{
	__asm jmp pointers[174*4];
}
// waveOutGetPitch
extern "C" __declspec(naked) void __stdcall __E__175__()
{
	__asm jmp pointers[175*4];
}
// waveOutGetPlaybackRate
extern "C" __declspec(naked) void __stdcall __E__176__()
{
	__asm jmp pointers[176*4];
}
// waveOutGetPosition
extern "C" __declspec(naked) void __stdcall __E__177__()
{
	__asm jmp pointers[177*4];
}
// waveOutGetVolume
extern "C" __declspec(naked) void __stdcall __E__178__()
{
	__asm jmp pointers[178*4];
}
// waveOutMessage
extern "C" __declspec(naked) void __stdcall __E__179__()
{
	__asm jmp pointers[179*4];
}
// waveOutOpen
extern "C" __declspec(naked) void __stdcall __E__180__()
{
	__asm jmp pointers[180*4];
}
// waveOutPause
extern "C" __declspec(naked) void __stdcall __E__181__()
{
	__asm jmp pointers[181*4];
}
// waveOutPrepareHeader
extern "C" __declspec(naked) void __stdcall __E__182__()
{
	__asm jmp pointers[182*4];
}
// waveOutReset
extern "C" __declspec(naked) void __stdcall __E__183__()
{
	__asm jmp pointers[183*4];
}
// waveOutRestart
extern "C" __declspec(naked) void __stdcall __E__184__()
{
	__asm jmp pointers[184*4];
}
// waveOutSetPitch
extern "C" __declspec(naked) void __stdcall __E__185__()
{
	__asm jmp pointers[185*4];
}
// waveOutSetPlaybackRate
extern "C" __declspec(naked) void __stdcall __E__186__()
{
	__asm jmp pointers[186*4];
}
// waveOutSetVolume
extern "C" __declspec(naked) void __stdcall __E__187__()
{
	__asm jmp pointers[187*4];
}
// waveOutUnprepareHeader
extern "C" __declspec(naked) void __stdcall __E__188__()
{
	__asm jmp pointers[188*4];
}
// waveOutWrite
extern "C" __declspec(naked) void __stdcall __E__189__()
{
	__asm jmp pointers[189*4];
}
// wid32Message
extern "C" __declspec(naked) void __stdcall __E__190__()
{
	__asm jmp pointers[190*4];
}
// wod32Message
extern "C" __declspec(naked) void __stdcall __E__191__()
{
	__asm jmp pointers[191*4];
}

#endif
