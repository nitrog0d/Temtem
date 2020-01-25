#include <Windows.h>
#include <iostream>
#include <stdlib.h>

#define GAME_EXECUTABLE "Temtem.exe"
#define PAYLOAD_NAMESPACE "TemtemCheat"
#define PAYLOAD_CLASS "CheatLoader"
#define PAYLOAD_MAIN "Inject"
#define MONO_DLL "mono-2.0-bdwgc.dll"

#pragma region Mono
typedef VOID MonoObject;
typedef VOID MonoDomain;
typedef VOID MonoAssembly;
typedef VOID MonoImage;
typedef VOID MonoClass;
typedef VOID MonoMethod;
typedef VOID MonoImageOpenStatus;

// typedefs and fields for required mono functions
typedef void(__cdecl* t_mono_thread_attach)(MonoDomain*);
t_mono_thread_attach fnThreadAttach;
typedef  MonoDomain* (__cdecl* t_mono_get_root_domain)(void);
t_mono_get_root_domain fnGetRootDomain;
typedef MonoAssembly* (__cdecl* t_mono_assembly_open)(const char*, MonoImageOpenStatus*);
t_mono_assembly_open fnAssemblyOpen;
typedef MonoImage* (__cdecl* t_mono_assembly_get_image)(MonoAssembly*);
t_mono_assembly_get_image fnAssemblyGetImage;
typedef MonoClass* (__cdecl* t_mono_class_from_name)(MonoImage*, const char*, const char*);
t_mono_class_from_name fnClassFromName;
typedef MonoMethod* (__cdecl* t_mono_class_get_method_from_name)(MonoClass*, const char*, int);
t_mono_class_get_method_from_name fnMethodFromName;
typedef MonoObject* (__cdecl* t_mono_runtime_invoke)(MonoMethod*, void*, void**, MonoObject**);
t_mono_runtime_invoke fnRuntimeInvoke;
typedef const char* (__cdecl* t_mono_assembly_getrootdir)(void);
t_mono_assembly_getrootdir fnGetRootDir;

void initMonoFunctions(HMODULE mono) {
	fnThreadAttach = (t_mono_thread_attach)GetProcAddress(mono, "mono_thread_attach");
	fnGetRootDomain = (t_mono_get_root_domain)GetProcAddress(mono, "mono_get_root_domain");
	fnAssemblyOpen = (t_mono_assembly_open)GetProcAddress(mono, "mono_assembly_open");
	fnAssemblyGetImage = (t_mono_assembly_get_image)GetProcAddress(mono, "mono_assembly_get_image");
	fnClassFromName = (t_mono_class_from_name)GetProcAddress(mono, "mono_class_from_name");
	fnMethodFromName = (t_mono_class_get_method_from_name)GetProcAddress(mono, "mono_class_get_method_from_name");
	fnRuntimeInvoke = (t_mono_runtime_invoke)GetProcAddress(mono, "mono_runtime_invoke");
	fnGetRootDir = (t_mono_assembly_getrootdir)GetProcAddress(mono, "mono_assembly_getrootdir");
}

WCHAR dllpath[46];

bool InjectMonoAssembly() {
	std::string assemblyDir;

	HMODULE mono;
	MonoDomain* domain;
	MonoAssembly* assembly;
	MonoImage* image;
	MonoClass* klass;
	MonoMethod* method;

	/* grab the mono dll module */
	mono = LoadLibrary(MONO_DLL);
	/* initialize mono functions */
	initMonoFunctions(mono);
	/* grab the root domain */
	domain = fnGetRootDomain();
	fnThreadAttach(domain);
	/* Grab our root directory*/
	assemblyDir.append(fnGetRootDir());
	assemblyDir.append("\\");
	std::wstring ws(dllpath);
	assemblyDir.append(std::string(ws.begin(), ws.end()));
	/* open payload assembly */
	assembly = fnAssemblyOpen(assemblyDir.c_str(), 0);
	if (!assembly) return false;
	/* get image from assembly */
	image = fnAssemblyGetImage(assembly);
	if (!image) return false;
	/* grab the class */
	klass = fnClassFromName(image, PAYLOAD_NAMESPACE, PAYLOAD_CLASS);
	if (!klass) return false;
	/* grab the hack entrypoint */
	method = fnMethodFromName(klass, PAYLOAD_MAIN, 0);
	if (!method) return false;
	/* call our entrypoint */
	fnRuntimeInvoke(method, 0, 0, 0);
	return true;
}
#pragma endregion

#pragma region Proxy
struct version_dll {
	HMODULE dll;
	FARPROC oGetFileVersionInfoA;
	FARPROC oGetFileVersionInfoByHandle;
	FARPROC oGetFileVersionInfoExA;
	FARPROC oGetFileVersionInfoExW;
	FARPROC oGetFileVersionInfoSizeA;
	FARPROC oGetFileVersionInfoSizeExA;
	FARPROC oGetFileVersionInfoSizeExW;
	FARPROC oGetFileVersionInfoSizeW;
	FARPROC oGetFileVersionInfoW;
	FARPROC oVerFindFileA;
	FARPROC oVerFindFileW;
	FARPROC oVerInstallFileA;
	FARPROC oVerInstallFileW;
	FARPROC oVerLanguageNameA;
	FARPROC oVerLanguageNameW;
	FARPROC oVerQueryValueA;
	FARPROC oVerQueryValueW;
} version;

extern "C" {
	FARPROC PA;
	int runASM();

	void fGetFileVersionInfoA() { PA = version.oGetFileVersionInfoA; runASM(); }
	void fGetFileVersionInfoByHandle() { PA = version.oGetFileVersionInfoByHandle; runASM(); }
	void fGetFileVersionInfoExA() { PA = version.oGetFileVersionInfoExA; runASM(); }
	void fGetFileVersionInfoExW() { PA = version.oGetFileVersionInfoExW; runASM(); }
	void fGetFileVersionInfoSizeA() { PA = version.oGetFileVersionInfoSizeA; runASM(); }
	void fGetFileVersionInfoSizeExA() { PA = version.oGetFileVersionInfoSizeExA; runASM(); }
	void fGetFileVersionInfoSizeExW() { PA = version.oGetFileVersionInfoSizeExW; runASM(); }
	void fGetFileVersionInfoSizeW() { PA = version.oGetFileVersionInfoSizeW; runASM(); }
	void fGetFileVersionInfoW() { PA = version.oGetFileVersionInfoW; runASM(); }
	void fVerFindFileA() { PA = version.oVerFindFileA; runASM(); }
	void fVerFindFileW() { PA = version.oVerFindFileW; runASM(); }
	void fVerInstallFileA() { PA = version.oVerInstallFileA; runASM(); }
	void fVerInstallFileW() { PA = version.oVerInstallFileW; runASM(); }
	void fVerLanguageNameA() { PA = version.oVerLanguageNameA; runASM(); }
	void fVerLanguageNameW() { PA = version.oVerLanguageNameW; runASM(); }
	void fVerQueryValueA() { PA = version.oVerQueryValueA; runASM(); }
	void fVerQueryValueW() { PA = version.oVerQueryValueW; runASM(); }
}

void setupFunctions() {
	version.oGetFileVersionInfoA = GetProcAddress(version.dll, "GetFileVersionInfoA");
	version.oGetFileVersionInfoByHandle = GetProcAddress(version.dll, "GetFileVersionInfoByHandle");
	version.oGetFileVersionInfoExA = GetProcAddress(version.dll, "GetFileVersionInfoExA");
	version.oGetFileVersionInfoExW = GetProcAddress(version.dll, "GetFileVersionInfoExW");
	version.oGetFileVersionInfoSizeA = GetProcAddress(version.dll, "GetFileVersionInfoSizeA");
	version.oGetFileVersionInfoSizeExA = GetProcAddress(version.dll, "GetFileVersionInfoSizeExA");
	version.oGetFileVersionInfoSizeExW = GetProcAddress(version.dll, "GetFileVersionInfoSizeExW");
	version.oGetFileVersionInfoSizeW = GetProcAddress(version.dll, "GetFileVersionInfoSizeW");
	version.oGetFileVersionInfoW = GetProcAddress(version.dll, "GetFileVersionInfoW");
	version.oVerFindFileA = GetProcAddress(version.dll, "VerFindFileA");
	version.oVerFindFileW = GetProcAddress(version.dll, "VerFindFileW");
	version.oVerInstallFileA = GetProcAddress(version.dll, "VerInstallFileA");
	version.oVerInstallFileW = GetProcAddress(version.dll, "VerInstallFileW");
	version.oVerLanguageNameA = GetProcAddress(version.dll, "VerLanguageNameA");
	version.oVerLanguageNameW = GetProcAddress(version.dll, "VerLanguageNameW");
	version.oVerQueryValueA = GetProcAddress(version.dll, "VerQueryValueA");
	version.oVerQueryValueW = GetProcAddress(version.dll, "VerQueryValueW");
}
#pragma endregion

WCHAR credits[72];
WCHAR loading[20];
WCHAR inject[76];
WCHAR mononotfound[74];
WCHAR failed[86];

void OnAttach(HMODULE hModule) {
#pragma region "Obfuscated Strings"
	credits[6] = L'0' + 19;
	credits[45] = 113;
	credits[32] = 48;
	credits[38] = 121 - 0;
	credits[41] = L'B' + 49;
	credits[14] = L'T' - 17;
	credits[4] = L']' + 8;
	credits[11] = 67 - 35;
	credits[39] = 107;
	credits[13] = L' ' + 0;
	credits[61] = 113 + 3;
	credits[24] = 32;
	credits[43] = 100 + 22;
	credits[33] = L'P' - 32;
	credits[9] = 111 - 14;
	credits[69] = 102 + 8;
	credits[58] = L'y' - 1;
	credits[71] = 62 + 47;
	credits[25] = 67 + 43;
	credits[0] = L'=' + 23;
	credits[36] = L'Y' - 89;
	credits[15] = 93 + 21;
	credits[53] = 122 - 7;
	credits[37] = 52 + 58;
	credits[48] = 102;
	credits[42] = L'l' - 1;
	credits[56] = L'z' - 17;
	credits[31] = L'#' + 0;
	credits[57] = 114 - 3;
	credits[8] = 106 - 5;
	credits[27] = L'N' + 38;
	credits[16] = 87 + 14;
	credits[55] = L'8' + 66;
	credits[40] = L'z' - 1;
	credits[67] = L'o' - 0;
	credits[19] = 101;
	credits[7] = 106 - 2;
	credits[29] = L'w' - 8;
	credits[66] = 119 - 11;
	credits[51] = 110 + 8;
	credits[23] = 110 + 11;
	credits[12] = 45;
	credits[34] = 61 - 13;
	credits[2] = L'x' - 11;
	credits[26] = L'=' + 44;
	credits[68] = 102;
	credits[52] = L's' - 9;
	credits[50] = L'c' - 2;
	credits[5] = L'o' - 2;
	credits[17] = 96 + 1;
	credits[59] = L'@' + 45;
	credits[44] = 102;
	credits[28] = L'z' - 8;
	credits[1] = L'z' - 21;
	credits[64] = 75 + 26;
	credits[22] = 98;
	credits[21] = L' ' + 0;
	credits[10] = L'h' + 12;
	credits[47] = L's' + 0;
	credits[3] = 116;
	credits[46] = 70 + 39;
	credits[30] = 46 + 0;
	credits[60] = L's' - 15;
	credits[62] = L'v' - 19;
	credits[54] = L']' + 7;
	credits[35] = L'1' + 6;
	credits[65] = L'9' + 60;
	credits[63] = L'^' + 17;
	credits[49] = 119;
	credits[20] = L't' - 16;
	credits[70] = 117 - 1;
	credits[18] = 116;

	loading[14] = L'P' + 31;
	loading[12] = L'Z' + 9;
	loading[11] = 73 + 25;
	loading[9] = L'0' - 2;
	loading[7] = L'Q' - 35;
	loading[10] = 0;
	loading[19] = 99;
	loading[13] = L'K' + 38;
	loading[3] = L'l' - 8;
	loading[1] = L't' - 5;
	loading[5] = L'P' + 30;
	loading[16] = 97;
	loading[15] = 58 + 58;
	loading[8] = 46;
	loading[18] = 119;
	loading[6] = 91 + 12;
	loading[2] = 119 - 22;
	loading[17] = 120 - 16;
	loading[0] = L'Z' - 14;
	loading[4] = 105 + 0;

	inject[8] = 89 + 22;
	inject[11] = L'P' + 30;
	inject[65] = 81 + 21;
	inject[42] = 58 + 61;
	inject[19] = 83 + 18;
	inject[57] = 60 + 53;
	inject[55] = L'n' - 6;
	inject[33] = L'K' + 30;
	inject[58] = 105 + 2;
	inject[14] = 99 + 0;
	inject[66] = L'9' + 44;
	inject[43] = L'z' - 1;
	inject[6] = L'p' - 80;
	inject[52] = 52 + 52;
	inject[69] = 112;
	inject[72] = L';' + 55;
	inject[29] = L'K' + 26;
	inject[64] = L'U' + 29;
	inject[34] = L't' - 0;
	inject[18] = 111 - 7;
	inject[15] = L'v' - 2;
	inject[73] = 120 - 5;
	inject[61] = L'S' + 29;
	inject[24] = 52 + 60;
	inject[30] = 110 - 78;
	inject[45] = 77 + 42;
	inject[23] = 32 + 0;
	inject[12] = 106 + 0;
	inject[7] = 120 - 4;
	inject[54] = L's' - 10;
	inject[27] = 97;
	inject[36] = L'.' + 0;
	inject[70] = 98;
	inject[25] = L'^' + 14;
	inject[1] = 114;
	inject[38] = L'b' - 98;
	inject[46] = 76 + 45;
	inject[67] = 105;
	inject[22] = 103 - 59;
	inject[0] = 84;
	inject[41] = 107;
	inject[28] = 115;
	inject[3] = 105;
	inject[2] = 121;
	inject[59] = L'i' - 1;
	inject[5] = 103;
	inject[50] = L'0' + 55;
	inject[37] = 46 + 0;
	inject[60] = L'z' - 10;
	inject[17] = 48 + 51;
	inject[48] = L'p' - 3;
	inject[44] = L'^' + 20;
	inject[51] = L'N' + 35;
	inject[21] = L'3' + 65;
	inject[10] = L';' + 46;
	inject[56] = 98;
	inject[13] = L'?' + 38;
	inject[9] = 32;
	inject[4] = L's' - 5;
	inject[75] = L'u' - 4;
	inject[32] = 59 + 38;
	inject[16] = L'u' - 85;
	inject[49] = L'7' + 42;
	inject[68] = 79 + 33;
	inject[53] = L']' + 25;
	inject[20] = 97;
	inject[62] = L'm' + 12;
	inject[31] = 118 + 1;
	inject[35] = 46;
	inject[47] = 119;
	inject[63] = L'q' - 2;
	inject[74] = 103;
	inject[26] = L'i' - 4;
	inject[40] = 82 + 20;
	inject[71] = 118 - 18;
	inject[39] = 76 + 24;

	mononotfound[71] = L'q' - 8;
	mononotfound[50] = L'Y' + 32;
	mononotfound[11] = 74 + 42;
	mononotfound[0] = L'0' + 29;
	mononotfound[33] = L'E' + 42;
	mononotfound[31] = L'a' + 4;
	mononotfound[48] = 85 + 31;
	mononotfound[39] = L'w' + 1;
	mononotfound[52] = 114;
	mononotfound[68] = L'0' + 49;
	mononotfound[29] = 32 + 0;
	mononotfound[41] = 109;
	mononotfound[69] = L'M' + 36;
	mononotfound[57] = L'B' + 53;
	mononotfound[49] = 107 + 12;
	mononotfound[13] = 50 + 52;
	mononotfound[72] = 79 + 29;
	mononotfound[27] = L'\\' - 60;
	mononotfound[56] = 57 + 58;
	mononotfound[21] = L'R' + 15;
	mononotfound[60] = 121;
	mononotfound[70] = L'i' + 1;
	mononotfound[53] = 106;
	mononotfound[38] = 112;
	mononotfound[37] = 0;
	mononotfound[42] = 119;
	mononotfound[7] = 99 - 23;
	mononotfound[43] = 122 - 3;
	mononotfound[67] = 49 + 54;
	mononotfound[20] = L'y' - 2;
	mononotfound[24] = L'n' - 5;
	mononotfound[23] = L'z' - 6;
	mononotfound[51] = L'u' - 12;
	mononotfound[59] = 97 + 17;
	mononotfound[4] = 49 - 17;
	mononotfound[26] = 110 - 7;
	mononotfound[10] = L'v' - 7;
	mononotfound[19] = 32;
	mononotfound[45] = L'5' + 67;
	mononotfound[16] = 110;
	mononotfound[18] = 44;
	mononotfound[47] = 103;
	mononotfound[3] = L'g' + 8;
	mononotfound[14] = 111;
	mononotfound[28] = L'K' - 26;
	mononotfound[73] = L'I' + 41;
	mononotfound[22] = 105;
	mononotfound[1] = 103 + 8;
	mononotfound[55] = 112;
	mononotfound[15] = 79 + 38;
	mononotfound[8] = 32 + 0;
	mononotfound[35] = L'n' - 10;
	mononotfound[25] = 110;
	mononotfound[12] = L'g' - 71;
	mononotfound[46] = 114 + 7;
	mononotfound[66] = 63 + 58;
	mononotfound[44] = 101;
	mononotfound[30] = 100 + 15;
	mononotfound[61] = 103;
	mononotfound[58] = 108;
	mononotfound[32] = L'4' + 47;
	mononotfound[64] = 76 + 26;
	mononotfound[2] = 110;
	mononotfound[9] = 51 + 59;
	mononotfound[40] = L'W' + 17;
	mononotfound[63] = L'x' - 10;
	mononotfound[17] = 100;
	mononotfound[34] = 51 + 59;
	mononotfound[36] = 46;
	mononotfound[5] = L'E' - 1;
	mononotfound[6] = 105 - 29;
	mononotfound[54] = 107 - 10;
	mononotfound[62] = 88 + 30;
	mononotfound[65] = 116 - 10;

	failed[33] = L'#' - 3;
	failed[4] = L'P' + 21;
	failed[5] = 119 - 19;
	failed[25] = L'=' + 36;
	failed[24] = L'S' - 51;
	failed[62] = L'z' - 2;
	failed[9] = 32;
	failed[35] = 32 + 0;
	failed[39] = L'>' + 49;
	failed[40] = L's' - 5;
	failed[10] = L'b' + 7;
	failed[32] = 117 - 7;
	failed[2] = 102 + 3;
	failed[83] = 100;
	failed[19] = L'x' - 6;
	failed[8] = 115 - 4;
	failed[74] = 102;
	failed[73] = 115 - 7;
	failed[30] = 32;
	failed[18] = L't' - 0;
	failed[85] = 121 - 1;
	failed[34] = 49;
	failed[82] = L'w' - 16;
	failed[64] = L';' + 41;
	failed[84] = 115 - 11;
	failed[59] = 99;
	failed[36] = L'L' + 39;
	failed[45] = L'j' - 2;
	failed[49] = 93 + 6;
	failed[44] = 115 - 11;
	failed[6] = 77 - 45;
	failed[55] = 90 + 25;
	failed[58] = L'z' - 15;
	failed[31] = L'F' + 35;
	failed[57] = L']' + 12;
	failed[22] = L'o' - 1;
	failed[28] = L'J' + 31;
	failed[47] = L't' + 5;
	failed[56] = L'y' - 5;
	failed[20] = 121;
	failed[12] = 112 - 6;
	failed[78] = 99;
	failed[67] = 113 - 5;
	failed[54] = 61 + 37;
	failed[21] = L'f' + 3;
	failed[72] = L't' - 16;
	failed[29] = 71 + 39;
	failed[79] = 61 + 54;
	failed[76] = L'a' + 6;
	failed[1] = 97;
	failed[16] = 44;
	failed[75] = 119 - 12;
	failed[13] = L'l' - 7;
	failed[23] = L'j' - 3;
	failed[11] = L'p' - 2;
	failed[3] = L'y' - 13;
	failed[61] = 104 + 8;
	failed[71] = 76 + 42;
	failed[17] = 32;
	failed[60] = 98;
	failed[68] = 108 + 13;
	failed[66] = 114;
	failed[80] = L'c' + 16;
	failed[26] = 107 - 4;
	failed[46] = 99;
	failed[43] = 50 - 50;
	failed[38] = 99;
	failed[53] = 88 + 34;
	failed[77] = 63 + 51;
	failed[27] = L'r' - 17;
	failed[48] = L'y' - 0;
	failed[50] = 89 + 26;
	failed[37] = 122 - 21;
	failed[15] = 100 + 16;
	failed[41] = L'9' + 43;
	failed[14] = 99;
	failed[81] = 52 + 60;
	failed[63] = 108 - 0;
	failed[52] = L'n' - 2;
	failed[65] = L'l' - 11;
	failed[51] = 122;
	failed[7] = L'>' + 54;
	failed[70] = 101 + 2;
	failed[69] = L't' - 1;
	failed[0] = L'F' + 0;
	failed[42] = L'.' + 0;

	dllpath[1] = L'\\' - 46;
	dllpath[18] = L'\\' + 24;
	dllpath[44] = L'i' + 7;
	dllpath[21] = 109 - 1;
	dllpath[41] = 111;
	dllpath[39] = 108;
	dllpath[35] = L'X' + 28;
	dllpath[43] = 109;
	dllpath[5] = 46;
	dllpath[36] = 103;
	dllpath[11] = L'H' + 44;
	dllpath[22] = L'6' + 54;
	dllpath[10] = 67 + 42;
	dllpath[40] = 98;
	dllpath[32] = L'v' - 5;
	dllpath[30] = 118 + 2;
	dllpath[14] = L'=' + 6;
	dllpath[8] = 81 + 3;
	dllpath[45] = L'\\' + 23;
	dllpath[0] = 46 + 0;
	dllpath[38] = L'w' - 17;
	dllpath[26] = 100;
	dllpath[37] = L'[' + 8;
	dllpath[16] = L']' + 8;
	dllpath[9] = 93 + 8;
	dllpath[3] = 73 + 19;
	dllpath[33] = 121 - 1;
	dllpath[31] = 76 + 29;
	dllpath[24] = 113;
	dllpath[4] = L'.' + 0;
	dllpath[42] = 109;
	dllpath[12] = 121 - 20;
	dllpath[29] = 86 + 33;
	dllpath[25] = 51 + 65;
	dllpath[7] = 63 + 29;
	dllpath[19] = 90 - 44;
	dllpath[28] = L'S' + 35;
	dllpath[6] = L'e' - 9;
	dllpath[27] = 117 - 6;
	dllpath[2] = 92;
	dllpath[13] = L'K' + 34;
	dllpath[20] = L'Y' + 11;
	dllpath[34] = L'7' + 43;
	dllpath[23] = 0;
	dllpath[15] = 56 + 48;
	dllpath[17] = L'd' - 3;

#pragma endregion
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	SetConsoleCtrlHandler(NULL, true);
	(void)freopen("CONIN$", "r", stdin);
	(void)freopen("CONOUT$", "w", stdout);
	(void)freopen("CONOUT$", "w", stderr);
	SetConsoleTitleW(loading);
	std::wcout << credits << std::endl;
	std::wcout << inject << std::endl;
	while (!GetModuleHandle(MONO_DLL)) {
		std::wcout << mononotfound << std::endl;
		Sleep(1000);
	}
	// This is required, or else the game crashes (I guess because mono isn't ready or something, idrc)
	// 1 second seems to work, just to make sure I'll set it to 5
	Sleep(5000);
	while (!InjectMonoAssembly()) {
		std::wcout << failed << std::endl;
		Sleep(1000);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		char path[MAX_PATH];
		GetModuleFileName(NULL, path, MAX_PATH);
		if (std::string(path).find(GAME_EXECUTABLE) != std::string::npos) {
			char path[MAX_PATH];
			GetWindowsDirectory(path, sizeof(path));

			strcat_s(path, "\\System32\\version.dll");
			version.dll = LoadLibrary(path);
			setupFunctions();

			CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(OnAttach), hModule, 0, nullptr);
		}
		break;
	case DLL_PROCESS_DETACH:
		FreeLibrary(version.dll);
		break;
	}
	return 1;
}
