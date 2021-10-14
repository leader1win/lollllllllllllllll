
//Basic includes
#include <iostream>
#include <Windows.h>
#include <thread>
#include <string>
#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <vector>
#include <math.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <tchar.h>
#include <winioctl.h>
#include <Uxtheme.h>
#include <dwmapi.h>
#include <WinInet.h>
#include <Mmsystem.h>
#include <mciapi.h>
#include <shobjidl_core.h>
#include <direct.h>
#include <urlmon.h>

//Other includes
#include "cheat/comm.hpp"
#include "cheat/utils.hpp"
#include "api/KeyAuth.hpp"
#include "xorstr.hpp"
#include "cheat/ImGui/imgui.h"
#include "cheat/ImGui/imgui_impl_dx9.h"
#include "cheat/ImGui/imgui_impl_win32.h"

//Lib includes
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Winmm.lib")

#define FG_RED "\033[31m"
#define FG_PURPLE "\033[35m"
#define FG_GREEN "\033[32m"
#define FG_YELLOW "\033[33m"
#define FG_WHITE "\033[0m"
#define FG_BLUE "\033[0;34m"

//Setup
HWND MyWnd = NULL;
HWND GameWnd = NULL;
RECT GameRect = { NULL };
MSG Message = { NULL };
IDirect3D9Ex* p_Object = NULL;
IDirect3DDevice9Ex* p_Device = NULL;
D3DPRESENT_PARAMETERS p_Params = { NULL };
#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))
#define ReadUint64(base, offset) (*(uintptr_t**)(((PBYTE)base + offset)))
#define ReadBool(base, offset) (*(bool *)(((PBYTE)base + offset)))
#define ReadVector2D(base, offset) (*(FVector2D *)(((PBYTE)base + offset)))
#define ReadFVector(base, offset) (*(FVector *)(((PBYTE)base + offset)))
#define ReadInt(base, offset) (*(int *)(((PBYTE)base + offset)))
#define ReadFloat(base, offset) (*(float *)(((PBYTE)base + offset)))
#define ReadDWORD(base, offset) (*(PDWORD)(((PBYTE)base + offset)))
#define ReadBYTE(base, offset) (*(((PBYTE)base + offset)))

//Check if object is in rectangle
boolean isInRectangle(double centerX, double centerY, double radius, double x, double y)
{
	return x >= centerX - radius && x <= centerX + radius &&
		y >= centerY - radius && y <= centerY + radius;
}

//string to wstring converter
std::wstring s2ws(const std::string& s) {
	std::string curLocale = setlocale(LC_ALL, "");
	const char* _Source = s.c_str();
	size_t _Dsize = mbstowcs(NULL, _Source, 0) + 1;
	wchar_t* _Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest, _Source, _Dsize);
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

//String to char converter
char* StringToChar(std::string string)
{
	return _strdup(string.c_str());
}

//Message logger
template <class T>
void msg(T msg)
{
	std::cout << msg << std::endl;
}

//Check if a file exists
bool FileExists(const std::string& fileName)
{
	struct stat buffer;
	return (stat(fileName.c_str(), &buffer) == 0);
}

//Write data to .ini file
void WriteStringToIni(std::string string, std::string file, std::string app, std::string key)
{
	WritePrivateProfileStringA(app.c_str(), key.c_str(), string.c_str(), file.c_str());
}

//Read data from .ini file
std::string ReadStringFromIni(std::string file, std::string app, std::string key)
{
	char buf[100];
	GetPrivateProfileStringA(app.c_str(), key.c_str(), XorStr("NULL").c_str(), buf, 100, file.c_str());
	return (std::string)buf;
}

double return_float_value(float val69)
{

	return (float)val69;
}


//values
float new_crosshairx = 1;
float new_crosshairy = 1;
float new_crosshairt = 1;
float new_aimfov = 1;
float new_aimsmooth = 1;
float new_aimdistance = 1;


std::string ReadStringFromIni_Float(std::string file, std::string app, std::string key, int float_code)
{
	char buf[100];
	GetPrivateProfileStringA(app.c_str(), key.c_str(), XorStr("NULL").c_str(), buf, 100, file.c_str());

	if (float_code == 0)
	{
		system(XorStr("cls").c_str());
		std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("ERR - 14").c_str() << FG_WHITE << XorStr("] - Contact Support.").c_str();
		Sleep(5000);
		exit(0);
	}
	else if (float_code == 1) //crosshairx
	{
		new_crosshairx = std::stof(buf);
	}
	else if (float_code == 2) //crosshairy
	{
		new_crosshairy = std::stof(buf);
	}
	else if (float_code == 3) //crosshairthinkness
	{
		new_crosshairt = std::stof(buf);
	}
	else if (float_code == 4) //aimfov
	{
		new_aimfov = std::stof(buf);
	}
	else if (float_code == 5) //aimsmooth
	{
		new_aimsmooth = std::stof(buf);
	}
	else if (float_code == 6) //aimdistance
	{
		new_aimdistance = std::stof(buf);
	}
	return (std::string)buf;
}

//WinProc (required for ImGui menu)
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam)) {
		return true;
	}
	switch (Message)
	{
	case WM_DESTROY:

		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (p_Device != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Params.BackBufferWidth = LOWORD(lParam);
			p_Params.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = p_Device->Reset(&p_Params);

			if (hr == D3DERR_INVALIDCALL) {
				IM_ASSERT(0);
			}

			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

//Initialize Window, DirectX and ImGui style
HRESULT DirectXInit(HWND hWnd) {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object))) {
		exit(3);
	}
	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Settings::MajorValues::Width;
	p_Params.BackBufferHeight = Settings::MajorValues::Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
	{
		p_Object->Release();
		exit(4);
	}
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig font_config;
	font_config.OversampleH = 1;
	font_config.OversampleV = 1;
	font_config.PixelSnapH = true;
	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF,
		0x0400, 0x044F,
		0,
	};
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(p_Device);
	//ImGui::StyleColorsClassic();
	ImGuiStyle& style = ImGui::GetStyle();
			style.WindowPadding = ImVec2(8.00f, 8.00f);
			style.FramePadding = ImVec2(4.00f, 3.00f);
			style.ItemSpacing = ImVec2(8.00f, 4.00f);
			style.ItemInnerSpacing = ImVec2(4.00f, 4.00f);
			style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
			style.IndentSpacing = 21.00f;
			style.ScrollbarSize = 14.00f;
			style.GrabMinSize = 10.00f;
			style.WindowBorderSize = 1.00f;
			style.ChildBorderSize = 1.00f;
			style.PopupBorderSize = 1.00f;
			style.FrameBorderSize = 0.00f;
			style.TabBorderSize = 0.00f;
			style.WindowRounding = 4.00f;
			style.ChildRounding = 4.00f;
			style.FrameRounding = 4.00f;
			style.PopupRounding = 4.00f;
			style.ScrollbarRounding = 10.00f;
			style.GrabRounding = 4.00f;
			style.TabRounding = 4.00f;
			style.WindowTitleAlign = ImVec2(0.00f, 0.50f);
			style.ButtonTextAlign = ImVec2(0.50f, 0.50f);
			style.DisplaySafeAreaPadding = ImVec2(3.00f, 3.00f);



			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.27f, 0.48f, 1.00f, 0.54f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.27f, 0.48f, 1.00f, 0.82f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.27f, 0.48f, 1.00f, 0.82f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.27f, 0.48f, 1.00f, 0.82f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.99f, 0.99f, 0.99f, 1.00f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.58f, 0.75f, 0.96f, 1.00f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.27f, 0.48f, 1.00f, 0.82f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.34f, 0.99f, 0.80f, 0.76f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.34f, 0.99f, 0.80f, 0.76f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			style.Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
			style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
			style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
			style.Colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
			style.Colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			style.Colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
			style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
			style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
			style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
			style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
			style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	io.Fonts->AddFontFromFileTTF(XorStr("C:\\Infinity\\font.otf").c_str(), 14);
	p_Object->Release();
	return S_OK;
}

//Create Window
void SetupWindow()
{
	GameWnd = FindWindowW(NULL, TEXT("Fortnite  "));
	if (GameWnd)
	{
		GetClientRect(GameWnd, &GameRect);
		POINT xy = { 0 };
		ClientToScreen(GameWnd, &xy);
		GameRect.left = xy.x;
		GameRect.top = xy.y;
		Settings::MajorValues::Width = GameRect.right;
		Settings::MajorValues::Height = GameRect.bottom;
	}
	else {
		exit(2);
	}
	WNDCLASSEX overlayWindowClass;
	ZeroMemory(&overlayWindowClass, sizeof(WNDCLASSEX));
	overlayWindowClass.cbClsExtra = NULL;
	overlayWindowClass.cbWndExtra = NULL;
	overlayWindowClass.cbSize = sizeof(WNDCLASSEX);
	overlayWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	overlayWindowClass.lpfnWndProc = WinProc;
	overlayWindowClass.hInstance = NULL;
	overlayWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	overlayWindowClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	overlayWindowClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	overlayWindowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	overlayWindowClass.lpszClassName = L"mnfgdnjsah4678zvsdgtzuiztnehtzui5ezu5e4bjdfbjnfgb4frc76e48be4utdhubtdug4w7b7e4b74wbw48bw48w48bw4b8w7b8g6rubetzubet78hb4wehvs87w95eg6vnmshfenmzgi8";
	overlayWindowClass.lpszMenuName = L"mnfgdnjsah4678zvsdgtzuiztnehtzui5ezu5e4bjdfbjnfgb4frc76e48be4utdhubtdug4w7b7e4b74wbw48bw48w48bw4b8w7b8g6rubetzubet78hb4wehvs87w95eg6vnmshfenmzgi8";
	RegisterClassEx(&overlayWindowClass);
	MyWnd = CreateWindowEx(NULL, L"mnfgdnjsah4678zvsdgtzuiztnehtzui5ezu5e4bjdfbjnfgb4frc76e48be4utdhubtdug4w7b7e4b74wbw48bw48w48bw4b8w7b8g6rubetzubet78hb4wehvs87w95eg6vnmshfenmzgi8", L"mnfgdnjsah4678zvsdgtzuiztnehtzui5ezu5e4bjdfbjnfgb4frc76e48be4utdhubtdug4w7b7e4b74wbw48bw48w48bw4b8w7b8g6rubetzubet78hb4wehvs87w95eg6vnmshfenmzgi8", WS_POPUP | WS_VISIBLE, GameRect.left, GameRect.top, Settings::MajorValues::Width, Settings::MajorValues::Height, NULL, NULL, NULL, NULL);
	MARGINS margin = { GameRect.left, GameRect.top, Settings::MajorValues::Width, Settings::MajorValues::Height };
	DwmExtendFrameIntoClientArea(MyWnd, &margin);
	SetWindowLong(MyWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
	ShowWindow(MyWnd, SW_SHOW);
	ITaskbarList* pTaskList = NULL;
	HRESULT initRet = CoInitialize(NULL);
	HRESULT createRet = CoCreateInstance(CLSID_TaskbarList,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ITaskbarList,
		(LPVOID*)&pTaskList);
	if (createRet == S_OK)
	{
		pTaskList->DeleteTab(FindWindowA(NULL, XorStr("mnfgdnjsah4678zvsdgtzuiztnehtzui5ezu5e4bjdfbjnfgb4frc76e48be4utdhubtdug4w7b7e4b74wbw48bw48w48bw4b8w7b8g6rubetzubet78hb4wehvs87w95eg6vnmshfenmzgi8").c_str()));

		pTaskList->Release();
	}
	CoUninitialize();
	//::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	UpdateWindow(MyWnd);
}

//KeyAuth setup
using namespace KeyAuth;
std::string name = XorStr("Infinity testing").c_str();
std::string ownerid = XorStr("wbUW2r8opA").c_str();
std::string secret = XorStr("fdc223842f73ae527628e6f600c2ad1682a24509eed7926bc6bdfa1fc6230777").c_str();
std::string version = XorStr("1.0").c_str();
std::string key;
api KeyAuthApp(name, ownerid, secret, version);

//Options, Settings
struct
{
	bool ShowMenu = false;
	bool FirstUse = true;
	int MenuTab = 0;
	float Width;
	float Height;
	struct
	{
		bool Enable = true;
		bool DrawFOV = true;
		bool autofiretrigger = false;
		bool ClosestByCrosshair = true;
		bool ClosestByDistance = false;
		bool AimPos_Head = true;
		bool AimPos_Body = false;
		float AimbotFOV = 50.0f;
		float AimbotSmooth = 2.0f;
		float AimbotMaximalDistance = 100.0f;
		bool AimKey_RMB = true;
		bool AimKey_SHIFT = false;
		int AimKey;
	} Aimbot;
	struct {
		bool Boxes = true;
		bool corneredbox = false;
		bool skel = false;
		bool filledboxes = false;
		bool Snaplines = true;
		bool Distance = true;
		bool HeadDot= false;
		bool Nickname;
		bool ActiveItemOrWeapon;
		bool esppreview = true;
		bool PlayerESP = true;
	    bool enabled = true;
		float visdist = 300;
		bool Lock_Line = true;
	} Visuals;
	struct
	{
		bool FakeKeyboardSounds = false;
		bool Crosshair = false;
		bool WhiteCross = false;
		float CrosshairX = 10.0f;
		float CrosshairY = 10.0f;
		float CrosshairThickness = 1.0f;
		int Fovshape = 20.f;
	} Misc;

} Options;

//Find closest 3D Object to 2D point
using namespace std;
int getClosest(int val1, int val2,
	int target)
{
	if (target - val1 >= val2 - target)
		return val2;
	else
		return val1;
}
int findClosest(int arr[], int n, int target)
{
	if (target <= arr[0])
		return arr[0];
	if (target >= arr[n - 1])
		return arr[n - 1];
	int i = 0, j = n, mid = 0;
	while (i < j) {
		mid = (i + j) / 2;
		if (arr[mid] == target)
			return arr[mid];
		if (target < arr[mid]) {
			if (mid > 0 && target > arr[mid - 1])
				return getClosest(arr[mid - 1],
					arr[mid], target);
			j = mid;
		}
		else {
			if (mid < n - 1 && target < arr[mid + 1])
				return getClosest(arr[mid],
					arr[mid + 1], target);
			i = mid + 1;
		}
	}
	return arr[mid];
}

//Definition of game addresses
DWORD_PTR Uworld;
DWORD_PTR LocalPawn;
DWORD_PTR Localplayer;
DWORD_PTR Rootcomp;
DWORD_PTR PlayerController;
DWORD_PTR Ulevel;
DWORD_PTR entityx;
int localplayerID;
extern Vector3 CameraEXT(0, 0, 0);
float FovAngle;
Vector3 localactorpos;
Vector3 Localcam;
bool isaimbotting = false;
//WorldToScreen - Convert a 3D Object to 2D Screen x, y position
Vector3 ProjectWorldToScreen69(Vector3 WorldLocation, Vector3 camrot)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);
	Vector3 Camera;

	auto chain69 = read<uintptr_t>(Localplayer + 0xa8);
	uint64_t chain699 = read<uintptr_t>(chain69 + 8);

	Camera.x = read<float>(chain699 + 0x7F8);  //camera pitch  watch out for x and y swapped 4u
	Camera.y = read<float>(Rootcomp + 0x12C);  //camera yaw

	float test = asin(Camera.x);
	float degrees = test * (180.0 / M_PI);
	Camera.x = degrees;

	if (Camera.y < 0)
		Camera.y = 360 + Camera.y;

	D3DMATRIX tempMatrix = Matrix(Camera);
	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	uint64_t chain = read<uint64_t>(Localplayer + 0x70);
	uint64_t chain1 = read<uint64_t>(chain + 0x98);
	uint64_t chain2 = read<uint64_t>(chain1 + 0x140);

	Vector3 vDelta = WorldLocation - read<Vector3>(chain2 + 0x10); //camera location credits for Object9999
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float zoom = read<float>(chain699 + 0x590);

	FovAngle = 80.0f / (zoom / 1.19f);
	float ScreenCenterX = Settings::MajorValues::ScreenCenterX;
	float ScreenCenterY = Settings::MajorValues::ScreenCenterY;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	CameraEXT = Camera;

	return Screenlocation;
}
double GetCrossDistance(double x1, double y1, double x2, double y2)
{
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

bool GetClosestPlayerToCrossHair(Vector3 Pos, float& max, float aimfov, DWORD_PTR entity)
{
	if (entity)
	{
		float Dist = GetCrossDistance(Pos.x, Pos.y, Settings::MajorValues::ScreenCenterX, Settings::MajorValues::ScreenCenterY);

		if (Dist < max)
		{
			max = Dist;
			entityx = entity;
			Options.Aimbot.AimbotFOV = aimfov;
		}
	}
	return false;
}
void AIms(DWORD_PTR entity, Vector3 Localcam)
{
	float max = 100.f;

	uint64_t currentactormesh = read<uint64_t>(entity + 0x280); 

	Vector3 rootHead = GetBoneWithRotation(currentactormesh, 66);
	Vector3 rootHeadOut = ProjectWorldToScreen69(rootHead, Vector3(Localcam.y, Localcam.x, Localcam.z));

	if (GetClosestPlayerToCrossHair(rootHeadOut, max, Options.Aimbot.AimbotFOV, entity))
		entityx = entity;
}
void aimbot(float x, float y)
{
	float ScreenCenterX = Settings::MajorValues::ScreenCenterX;
	float ScreenCenterY = Settings::MajorValues::ScreenCenterY;
	int AimSpeed = Options.Aimbot.AimbotSmooth;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);

	if (Options.Aimbot.autofiretrigger) {
		mouse_event(MOUSEEVENTF_LEFTDOWN, DWORD(NULL), DWORD(NULL), DWORD(0x0002), ULONG_PTR(NULL));
		mouse_event(MOUSEEVENTF_LEFTUP, DWORD(NULL), DWORD(NULL), DWORD(0x0004), ULONG_PTR(NULL));
	}

	return;
}
int Width;
int Height;
int Depth;
#include <Xinput.h>
void AimAt(DWORD_PTR entity, Vector3 Localcam)
{


	{
		uint64_t currentactormesh = read<uint64_t>(entity + 0x280);
		auto rootHead = GetBoneWithRotation(currentactormesh, 66);
		Vector3 rootHeadOut = ProjectWorldToScreen69(rootHead, Vector3(Localcam.y, Localcam.x, Localcam.z));
		Vector3 Headpos = GetBoneWithRotation(currentactormesh, 66);
		Vector3 HeadposW2s = ProjectWorldToScreen69(Headpos, Vector3(Localcam.y, Localcam.x, Localcam.z));

		if (rootHeadOut.x != 0 || rootHeadOut.y != 0)
		{
			if ((GetCrossDistance(rootHeadOut.x, rootHeadOut.y, Settings::MajorValues::ScreenCenterX, Settings::MajorValues::ScreenCenterY) <= Options.Aimbot.AimbotFOV * 8) || isaimbotting)
				if (Options.Visuals.Lock_Line) {
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(rootHeadOut.x, rootHeadOut.y), IM_COL32(40, 40, 40, 180), Thickness);
			{
				

				



			}
		}
	}


}

void aimbot2(Vector3 Localcam)
{
	if (entityx != 0)
	{
		if (GetAsyncKeyState(hotkeys::aimkey))
		{
			AimAt(entityx, Localcam);
		}
		else
		{
			isaimbotting = false;
		}
	}
}
int NewPlayerLocationX;
int NewPlayerLocationY;
Vector3 Headposw2s;
bool aimbottestnigga = false;
bool aimbottestnigga2 = false;

//Offsets
#define offset_uworld 0x9c9f3b0
#define offset_gameinstance 0x188
#define offset_localplayers 0x38
#define offset_playercontroller 0x30
#define offset_localpawn 0x2A0
#define offset_rootcomp 0x130
#define offset_playerid 0x18
#define offset_plevel 0x30
#define offset_playerstate 0x240
#define offset_actorcount 0xA0
#define offset_aactors 0x98

//ActorLoop
void ActorLoop()
{
	Uworld = read<DWORD_PTR>(m_base + offset_uworld);
	DWORD_PTR Gameinstance = read<DWORD_PTR>(Uworld + offset_gameinstance);
	if (Gameinstance == (DWORD_PTR)nullptr)
		return;
	DWORD_PTR LocalPlayers = read<DWORD_PTR>(Gameinstance + offset_localplayers);
	if (LocalPlayers == (DWORD_PTR)nullptr)
		return;
	Localplayer = read<DWORD_PTR>(LocalPlayers);
	if (Localplayer == (DWORD_PTR)nullptr)
		return;
	PlayerController = read<DWORD_PTR>(Localplayer + offset_playercontroller);
	if (PlayerController == (DWORD_PTR)nullptr)
		return;
	LocalPawn = read<uint64_t>(PlayerController + offset_localpawn);
	if (LocalPawn == (DWORD_PTR)nullptr)
		return;
	Rootcomp = read<uint64_t>(LocalPawn + offset_rootcomp);
	if (Rootcomp == (DWORD_PTR)nullptr)
		return;
	if (LocalPawn != 0) {
		localplayerID = read<int>(LocalPawn + offset_playerid);
	}
	Ulevel = read<DWORD_PTR>(Uworld + offset_plevel);
	if (Ulevel == (DWORD_PTR)nullptr)
		return;
	DWORD64 PlayerState = read<DWORD64>(LocalPawn + offset_playerstate);
	if (PlayerState == (DWORD_PTR)nullptr)
		return;
	DWORD ActorCount = read<DWORD>(Ulevel + offset_actorcount);
	DWORD_PTR AActors = read<DWORD_PTR>(Ulevel + offset_aactors);
	if (AActors == (DWORD_PTR)nullptr)
		return;
	for (int i = 0; i < ActorCount; i++)
	{
		uint64_t CurrentActor = read<uint64_t>(AActors + i * 0x8);
		int niggaid = read<int>(CurrentActor + offset_playerid);
		if (niggaid == localplayerID)
		{
			uint64_t CurrentActorRootComponent = read<uint64_t>(CurrentActor + 0x130);
			uint64_t currentactormesh = read<uint64_t>(CurrentActor + 0x280);
			int MyTeamId = read<int>(PlayerState + 0xED8);
			DWORD64 otherPlayerState = read<uint64_t>(CurrentActor + 0x240);
			int ActorTeamId = read<int>(otherPlayerState + 0xED8);
			Vector3 Headpos = GetBoneWithRotation(currentactormesh, 66);
			Localcam = CameraEXT;
			localactorpos = read<Vector3>(Rootcomp + 0x11C);
			float distance = localactorpos.Distance(Headpos) / 100.f;
			Vector3 CirclePOS = GetBoneWithRotation(currentactormesh, 2);
			Vector3 bone0 = GetBoneWithRotation(currentactormesh, 0);
			Vector3 bottom = ProjectWorldToScreen69(bone0, Vector3(Localcam.y, Localcam.x, Localcam.z));
			Headposw2s = ProjectWorldToScreen69(Headpos, Vector3(Localcam.y, Localcam.x, Localcam.z));
			aimbottestnigga = true;
			Vector3 rootOut = GetBoneWithRotation(currentactormesh, 0);
			Vector3 RootBone = GetBoneWithRotation(currentactormesh, 0);
			Vector3 RootBoneOut = ProjectWorldToScreen69(RootBone, Vector3(Localcam.y, Localcam.x, Localcam.z));
			Vector3 Out = ProjectWorldToScreen69(rootOut, Vector3(Localcam.y, Localcam.x, Localcam.z));
			if (MyTeamId != ActorTeamId)
				if (distance < Options.Visuals.visdist)
			{
				if (Options.Visuals.Distance)
				{
					char name[64];
					sprintf_s(name, "Player | %2.fM", distance);
					DrawString(14, Headposw2s.x, Headposw2s.y - 15, &Col.red_, true, true, name);
				}

				if (Options.Visuals.Snaplines, bottom.x, bottom.y)
				{
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(960, 1060), ImVec2(bottom.x, bottom.y), ImColor(255, 0, 0, 255), 2.0f);
				}

				if (Options.Visuals.corneredbox)
				{
					float boxsize = (float)(Out.y - Headposw2s.y);
					float boxwidth = boxsize / 3.0f;
					float BoxHeight = (float)(Out.y - Headposw2s.y);
					float BoxWidth = BoxHeight * 0.230f;
					float dwpleftx = (float)Out.x - (BoxWidth / 1);
					float dwplefty = (float)Out.y;
					float CornerHeight = abs(Out.y - Headposw2s.y);
					float CornerWidth = CornerHeight * 0.5; //0.5
					DrawCorneredBox(Headposw2s.x - (CornerWidth / 2), Headposw2s.y, CornerWidth, CornerHeight, IM_COL32(255, 0, 0, 255), 2);
				}
				if (Options.Aimbot.Enable && (GetAsyncKeyState(VK_SHIFT) || GetAsyncKeyState(0x02)) && isInRectangle(Settings::MajorValues::ScreenCenterX, Settings::MajorValues::ScreenCenterY, Options.Aimbot.AimbotFOV, Headposw2s.x, Headposw2s.y) && distance < Options.Aimbot.AimbotMaximalDistance)
				{
					aimbottestnigga2 = true;
					float AimbotLocationX; float AimbotLocationY; int AllPlayerLocationX = Headposw2s.x; int AllPlayerLocationY = Headposw2s.y; int array69_x[] = { AllPlayerLocationX }; int n_x = sizeof(array69_x) / sizeof(array69_x[0]); int array69_y[] = { AllPlayerLocationY }; int n_y = sizeof(array69_y) / sizeof(array69_y[0]); int targetX = Settings::MajorValues::ScreenCenterX; int targetY = Settings::MajorValues::ScreenCenterY; NewPlayerLocationX = findClosest(array69_x, n_x, targetX); NewPlayerLocationY = findClosest(array69_y, n_y, targetY); AimbotLocationX = NewPlayerLocationX; AimbotLocationY = NewPlayerLocationY;
				}
				if (Options.Visuals.filledboxes) {
					float boxsize = (float)(Out.y - Headposw2s.y);
					float boxwidth = boxsize / 3.0f;
					float BoxHeight = (float)(Out.y - Headposw2s.y);
					float BoxWidth = BoxHeight * 0.224f;
					float dwpleftx = (float)Out.x - (BoxWidth / 1);
					float dwplefty = (float)Out.y;
					float CornerHeight = abs(Out.y - Headposw2s.y);
					float CornerWidth = CornerHeight * 0.5; //0.5
					ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(dwpleftx, dwplefty), ImVec2(Headposw2s.x + boxwidth, Headposw2s.y + 5.0f), IM_COL32(40, 40, 40, 180));
				}

			}
		}
	}
	if (auth_check_nigga_shit != XorStr("true").c_str())
	{
		std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("ERR - 4").c_str() << FG_WHITE << XorStr("] - Contact Support.").c_str();
		Sleep(-1);
		exit(0);
	}
}

//Aimbot loop
void AimbotLoop()
{
	while (1)
	{
		if (Options.Aimbot.Enable && aimbottestnigga && aimbottestnigga2)
		{
			if (Options.Aimbot.AimKey_RMB && GetAsyncKeyState(0x02))
			{
				aimbot(NewPlayerLocationX, NewPlayerLocationY);
			}
			else if (Options.Aimbot.AimKey_SHIFT && GetAsyncKeyState(VK_SHIFT))
			{
				aimbot(NewPlayerLocationX, NewPlayerLocationY);
			}
		}
		aimbottestnigga = false;
		aimbottestnigga2 = false;
		break;
	}
}

//Reset menu settings
void resetmenu()
{
	Options.Aimbot.Enable = true;
	Options.Aimbot.DrawFOV = true;
	Options.Aimbot.ClosestByCrosshair = true;
	Options.Aimbot.ClosestByDistance = false;
	Options.Aimbot.AimPos_Head = true;
	Options.Aimbot.AimPos_Body = false;
	Options.Aimbot.AimbotFOV = 50.0f;
	Options.Aimbot.AimbotSmooth = 2.0f;
	Options.Aimbot.AimbotMaximalDistance = 100.0f;
	Options.Aimbot.AimKey;
	Options.Visuals.Boxes = true;
	Options.Visuals.filledboxes = false;
	Options.Visuals.Snaplines = true;
	Options.Visuals.Distance = true;
	Options.Visuals.HeadDot;
	Options.Visuals.Nickname;
	Options.Visuals.ActiveItemOrWeapon;
	Options.Misc.FakeKeyboardSounds = false;
	Options.Misc.Crosshair = false;
	Options.Misc.WhiteCross = false;
	Options.Misc.CrosshairX = 10.0f;
	Options.Misc.CrosshairY = 10.0f;
	Options.Misc.CrosshairThickness = 1.0f;
	Sleep(200);
	Options.ShowMenu = true;
	Options.MenuTab = 0;
}

//Menu colors
void rendercolors()
{
	if (auth_check_nigga_shit != XorStr("true").c_str())
	{
		std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("ERR - 4").c_str() << FG_WHITE << XorStr("] - Contact Support.").c_str();
		Sleep(-1);
		exit(0);
	}
}

//Render cheat addons
void RenderAddons()
{
	
	//Aimbot FOV circle
	if (Options.Aimbot.DrawFOV) { ImGui::GetOverlayDrawList()->AddCircle(ImVec2(Settings::MajorValues::ScreenCenterX, Settings::MajorValues::ScreenCenterY), Options.Aimbot.AimbotFOV, ImGui::GetColorU32({ 0.8f, 0.8f, 0.8f, 0.9f }), Options.Misc.Fovshape, 1.0f);}
	
	//Crosshair
	if (Options.Misc.Crosshair){ImGui::GetOverlayDrawList()->AddLine(ImVec2(Settings::MajorValues::ScreenCenterX - Options.Misc.CrosshairX, Settings::MajorValues::ScreenCenterY), ImVec2(Settings::MajorValues::ScreenCenterX + Options.Misc.CrosshairX, Settings::MajorValues::ScreenCenterY), ImGui::GetColorU32({ 0.8f, 0.0f, 0.0f, 0.9f }), Options.Misc.CrosshairThickness);ImGui::GetOverlayDrawList()->AddLine(ImVec2(Settings::MajorValues::ScreenCenterX, Settings::MajorValues::ScreenCenterY - Options.Misc.CrosshairY), ImVec2(Settings::MajorValues::ScreenCenterX, Settings::MajorValues::ScreenCenterY + Options.Misc.CrosshairY), ImGui::GetColorU32({ 0.8f, 0.0f, 0.0f, 0.9f }), Options.Misc.CrosshairThickness);}
}

//Save config
void SaveConfig()
{
	
	//Aimbot
	if (Options.Aimbot.Enable)
		WriteStringToIni(XorStr("1").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("aimbot").c_str());
	else
		WriteStringToIni(XorStr("0").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("aimbot").c_str());

	//Draw FOV
	if (Options.Aimbot.DrawFOV)
		WriteStringToIni(XorStr("1").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("drawfov").c_str());
	else
		WriteStringToIni(XorStr("0").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("drawfov").c_str());

	//Snaplines
	if (Options.Visuals.Snaplines)
		WriteStringToIni(XorStr("1").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("snaplines").c_str());
	else
		WriteStringToIni(XorStr("0").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("snaplines").c_str());

	//Distance
	if (Options.Visuals.Distance)
		WriteStringToIni(XorStr("1").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("distance").c_str());
	else
		WriteStringToIni(XorStr("0").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("distance").c_str());

	//Boxes
	if (Options.Visuals.Boxes)
		WriteStringToIni(XorStr("1").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("boxes").c_str());
	else
		WriteStringToIni(XorStr("0").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("boxes").c_str());
	// filed boxes
	if (Options.Visuals.filledboxes)
		WriteStringToIni(XorStr("1").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("filledboxes").c_str());
	else
		WriteStringToIni(XorStr("0").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("filledboxes").c_str());
		if (Options.Misc.Crosshair)
			WriteStringToIni(XorStr("1").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("crosshair").c_str());
		else
			WriteStringToIni(XorStr("0").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("crosshair").c_str());
		
	//AimFOV
	WriteStringToIni(std::to_string(Options.Aimbot.AimbotFOV), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("aimfov").c_str());

	//Smooth
	WriteStringToIni(std::to_string(Options.Aimbot.AimbotSmooth), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("smooth").c_str());

	//Distance
	WriteStringToIni(std::to_string(Options.Aimbot.AimbotMaximalDistance), XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("distanceslider").c_str());
}

//Load config
void LoadConfig()
{
	
	//Aimbot
	if (ReadStringFromIni(XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("aimbot").c_str()) == XorStr("1").c_str())
		Options.Aimbot.Enable = true;
	else
		Options.Aimbot.Enable = false;

	//Draw FOV
	if (ReadStringFromIni(XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("drawfov").c_str()) == XorStr("1").c_str())
		Options.Aimbot.DrawFOV = true;
	else
		Options.Aimbot.DrawFOV = false;

	//Snaplines
	if (ReadStringFromIni(XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("snaplines").c_str()) == XorStr("1").c_str())
		Options.Visuals.Snaplines = true;
	else
		Options.Visuals.Snaplines = false;

	//Distance
	if (ReadStringFromIni(XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("distance").c_str()) == XorStr("1").c_str())
		Options.Visuals.Distance = true;
	else
		Options.Visuals.Distance = false;

	//Boxes
	if (ReadStringFromIni(XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("boxes").c_str()) == XorStr("1").c_str())
		Options.Visuals.Boxes = true;
	else
		Options.Visuals.Boxes = false;
	// filledboxes
	if (ReadStringFromIni(XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("filledboxes").c_str()) == XorStr("1").c_str())
		Options.Visuals.filledboxes = true;
	else
		Options.Visuals.filledboxes = false;
	// crosshair
	if (ReadStringFromIni(XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("crosshair").c_str()) == XorStr("1").c_str())
		Options.Misc.Crosshair = true;
	else
		Options.Misc.Crosshair = false;


	//AimFOV
	ReadStringFromIni_Float(XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("aimfov").c_str(), 4);
	Options.Aimbot.AimbotFOV = new_aimfov;

	//AimSmooth
	ReadStringFromIni_Float(XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("smooth").c_str(), 5);
	Options.Aimbot.AimbotSmooth = new_aimsmooth;

	//AimDistance
	ReadStringFromIni_Float(XorStr("C:\\Infinity\\config.ini").c_str(), XorStr("options").c_str(), XorStr("distanceslider").c_str(), 6);
	Options.Aimbot.AimbotMaximalDistance = new_aimdistance;
	
}

//Renderer
void renderLoopCall() {
	//Render setup
	ImGui_ImplDX9_NewFrame(); ImGui_ImplWin32_NewFrame(); ImGui::NewFrame();
	RECT rect = { 0 }; if (GetWindowRect(GameWnd, &rect)) { Settings::MajorValues::Width = rect.right - rect.left; Settings::MajorValues::Height = rect.bottom - rect.top; } Settings::MajorValues::ScreenCenterX = (Settings::MajorValues::Width / 2.0f), Settings::MajorValues::ScreenCenterY = (Settings::MajorValues::Height / 2.0f);


	//Render cheat addons
	RenderAddons();

	//Start the ActorLoop (ESP, etc.)
	ActorLoop();

	//Start the Aimbot loop
	AimbotLoop();

	//Toggle menu: F8
	if (GetAsyncKeyState(VK_INSERT) & 1) { mciSendString(XorStr(L"play C:\\Infinity\\buttonclick.mp3").c_str(), NULL, 0, 0); Options.ShowMenu = !Options.ShowMenu; }

	//Toggle visuals: F6
	if (GetAsyncKeyState(VK_F6) & 1) { mciSendString(XorStr(L"play C:\\Infinity\\buttonclick.mp3").c_str(), NULL, 0, 0); Options.Aimbot.DrawFOV = false; Options.Visuals.ActiveItemOrWeapon = false; Options.Visuals.Boxes = false; Options.Visuals.Distance = false; Options.Visuals.HeadDot = false; Options.Visuals.Nickname = false; Options.Visuals.Snaplines = false; Options.Misc.Crosshair = false; }

	//Exit: F2
	if (GetAsyncKeyState(VK_F2) & 1) { mciSendString(XorStr(L"play C:\\Infinity\\buttonclick.mp3").c_str(), NULL, 0, 0); Sleep(200); exit(0); }

	//Check if it's the first use
	if (Options.FirstUse)
	{
		////Set ImGui window position
		ImGui::SetWindowPos(ImVec2(700, 550));

		//Load config
		LoadConfig();

		//Initialize menu colors
		rendercolors();

		//Disable first use
		Options.FirstUse = false;
	}

	//Check if Options.ShowMenu is enabled
	if (Options.ShowMenu)
	{

		//Begin menu drawing
		if (ImGui::Begin(XorStr("Infinity.CC").c_str(), 0, ImGuiWindowFlags_NoResize))
		{
			ImGui::SetNextWindowSize({ 511.f,283.f });

			ImGui::Begin(XorStr("Infinity.CC").c_str());
			ImGui::SetCursorPos({ 14.f,30.f });
			if (ImGui::Button(XorStr("Softaim").c_str(), { 80.f,30.f })) 
			{
				mciSendString(XorStr(L"play C:\\Infinity\\buttonclick.mp3").c_str(), NULL, 0, 0);
				Options.MenuTab = 1;
			}
			ImGui::SetCursorPos({ 14.f,235.f });
			if (ImGui::Button(XorStr("Exit").c_str(), { 80.f,30.f }))
			{
				mciSendString(XorStr(L"play C:\\Infinity\\buttonclick.mp3").c_str(), NULL, 0, 0);
				exit(0);
			}
			ImGui::SetCursorPos({ 14.f,110.f });
			if (ImGui::Button(XorStr("Misc").c_str(), { 80.f,30.f }))
			{
				mciSendString(XorStr(L"play C:\\Infinity\\buttonclick.mp3").c_str(), NULL, 0, 0);
				Options.MenuTab = 3;
			}
			ImGui::SetCursorPos({ 14.f,70.f });
			if (ImGui::Button(XorStr("Visuals").c_str(), { 80.f,30.f }))
			{
				mciSendString(XorStr(L"play C:\\Infinity\\buttonclick.mp3").c_str(), NULL, 0, 0);
				Options.MenuTab = 2;
			}
			ImGui::SetCursorPos({ 16.f,212.f });
			ImGui::PushItemWidth(91.000000);
			ImGui::Text(XorStr("Status : F-UD").c_str());
			ImGui::PopItemWidth();
			ImGui::SetCursorPos({ 16.f,190.f });
			ImGui::PushItemWidth(84.000000);
			ImGui::Text(XorStr("09/24/2021").c_str());
			ImGui::PopItemWidth();

			if (Options.MenuTab == 0)
			{
				ImGui::SetCursorPos({ 125.f,36.f });
				ImGui::PushItemWidth(294.000000);
				ImGui::Text(XorStr("Infnity.CC | BUILD : 2.0 Beta | 10/2/2021").c_str());
				ImGui::PopItemWidth();
				ImGui::SetCursorPos({ 125.f,67.f });
				ImGui::PushItemWidth(273.000000);
				ImGui::Text(XorStr("Infnity.CC | Battle Eye UD | Up to date").c_str());
				ImGui::PopItemWidth();
				ImGui::SetCursorPos({ 196.f,235.f });
				if (ImGui::Button(XorStr("Discord").c_str(), { 178.f,30.f }))
				{
					system(XorStr("start https://discord.gg/kEqpQFdmPu").c_str());
				}
				ImGui::SetCursorPos({ 125.f,98.f });
				ImGui::PushItemWidth(357.000000);
				ImGui::Text(XorStr("Infinity.CC | Menu : INS | Visuals : F6 | Exit : F2").c_str());
				ImGui::PopItemWidth();
			}

			if (Options.MenuTab == 1)
			{
				ImGui::SetCursorPos({ 136.f,34.f });
				ImGui::Checkbox(XorStr("Enable").c_str(), &Options.Aimbot.Enable);
				ImGui::SetCursorPos({ 135.f,65.f });
				ImGui::Checkbox(XorStr("Show FOV").c_str(), &Options.Aimbot.DrawFOV);
				ImGui::SetCursorPos({ 136.f,96.f });
				ImGui::PushItemWidth(275.000000);
				ImGui::SliderFloat(XorStr("FOV").c_str(), &Options.Aimbot.AimbotFOV, 025.f, 1000.f, XorStr("%.2f").c_str());
				ImGui::PopItemWidth();
				ImGui::SetCursorPos({ 136.f,127.f });
				ImGui::PushItemWidth(275.000000);
				ImGui::SliderFloat(XorStr("Smooth").c_str(), &Options.Aimbot.AimbotSmooth, 1.f, 10.f, XorStr("%.2f").c_str());
				ImGui::PopItemWidth();
				ImGui::SetCursorPos({ 136.f,158.f });
				ImGui::PushItemWidth(275.000000);
				ImGui::SliderFloat(XorStr("Distance").c_str(), &Options.Aimbot.AimbotMaximalDistance, 15.f, 300.f, XorStr("%.2f").c_str());
				ImGui::PopItemWidth();
				ImGui::SetCursorPos({ 134.f,193.f });
				auto clickedH = ImGui::Checkbox(XorStr("Shift").c_str(), &Options.Aimbot.AimKey_SHIFT);
				if (clickedH && Options.Aimbot.AimKey_SHIFT) {
					Options.Aimbot.AimKey_RMB = false;
				}
				else if (clickedH && !Options.Aimbot.AimKey_RMB) {
					Options.Aimbot.AimKey_SHIFT = true;
				}
				ImGui::SetCursorPos({ 221.f,193.f });
				auto clickedB = ImGui::Checkbox(XorStr("Right Mouse").c_str(), &Options.Aimbot.AimKey_RMB);
				if (clickedB && Options.Aimbot.AimKey_RMB) {
					Options.Aimbot.AimKey_SHIFT = false;

				}
				else if (clickedB && Options.Aimbot.AimKey_RMB)
				{
					Options.Aimbot.AimKey_RMB = true;
				}
			}

			if (Options.MenuTab == 2)
			{
				ImGui::SetCursorPos({ 136.f,34.f });
				ImGui::Checkbox(XorStr("Cornerd Box ESP").c_str(), &Options.Visuals.corneredbox);
				ImGui::SetCursorPos({ 135.f,64.f });
				ImGui::Checkbox(XorStr("Snapline ESP").c_str(), &Options.Visuals.Snaplines);
				ImGui::SetCursorPos({ 136.f,94.f });
				ImGui::Checkbox(XorStr("Distance ESP").c_str(), &Options.Visuals.Distance);
				ImGui::PushItemWidth(266.000000);
				ImGui::SetCursorPos({ 136.f,124.f });
				ImGui::SliderFloat(XorStr("Vis dist").c_str(), &Options.Visuals.visdist, 25.f, 300.f, XorStr("%.2f").c_str());
			}

			if (Options.MenuTab == 3)
			{
				ImGui::SetCursorPos({ 136.f,34.f });
				ImGui::Checkbox(XorStr("Crosshair").c_str(), &Options.Misc.Crosshair);
				ImGui::SetCursorPos({ 136.f,65.f });
				ImGui::PushItemWidth(266.000000);
				ImGui::SliderFloat(XorStr("Horizontal").c_str(), &Options.Misc.CrosshairX, 1.f, 20.f, XorStr("%.2f").c_str());
				ImGui::PopItemWidth();
				ImGui::SetCursorPos({ 136.f,96.f });
				ImGui::PushItemWidth(266.000000);
				ImGui::SliderFloat(XorStr("Vertical").c_str(), &Options.Misc.CrosshairY, 1.f, 20.f, XorStr("%.2f").c_str());
				ImGui::PopItemWidth();
				ImGui::SetCursorPos({ 136.f,127.f });
				ImGui::PushItemWidth(266.000000);
				ImGui::SliderFloat(XorStr("Thickness").c_str(), &Options.Misc.CrosshairThickness, 0.5f, 5.f, XorStr("%.2f").c_str());
				ImGui::SetCursorPos({ 136.f,218 });
				ImGui::SliderInt(XorStr("Fov Shape").c_str(), &Options.Misc.Fovshape, 2.f, 30.f, XorStr("%.2f").c_str());
				ImGui::PopItemWidth();
				ImGui::SetCursorPos({ 136.f,158.f });
				if (ImGui::Button(XorStr("Save Config").c_str(), { 102.f,19.f }))
				{
					mciSendString(XorStr(L"play C:\\Infinity\\buttonclick.mp3").c_str(), NULL, 0, 0);
					Sleep(500); SaveConfig();
				}
				ImGui::SetCursorPos({ 251.f,158.f });
				if (ImGui::Button(XorStr("Load Config").c_str(), { 102.f,19.f }))
				{
					mciSendString(XorStr(L"play C:\\Infinity\\buttonclick.mp3").c_str(), NULL, 0, 0);
					Sleep(500); LoadConfig();
				}
				ImGui::SetCursorPos({ 252.f,189.f });
				if (ImGui::Button(XorStr("Panic").c_str(), { 102.f,19.f }))
				{
					mciSendString(XorStr(L"play C:\\Infinity\\buttonclick.mp3").c_str(), NULL, 0, 0);
					exit(0);
				}
				ImGui::SetCursorPos({ 136.f,189.f });
				if (ImGui::Button(XorStr("Reset Config").c_str(), { 102.f,19.f }))
				{
					mciSendString(XorStr(L"play C:\\Infinity\\buttonclick.mp3").c_str(), NULL, 0, 0);
					Sleep(500); resetmenu();
				}
				
			}

			ImGui::End();
		}
		ImGui::End();
	}

	//End ImGui Frame
	ImGui::EndFrame();

	//Render, clear
	p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (p_Device->BeginScene() >= 0){ImGui::Render();ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());p_Device->EndScene();}

	//Result
	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);
	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{ImGui_ImplDX9_InvalidateDeviceObjects();p_Device->Reset(&p_Params);ImGui_ImplDX9_CreateDeviceObjects();}
}

WPARAM MainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		if (auth_check_nigga_shit != XorStr("true").c_str())
		{
			std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("ERR - 4").c_str() << FG_WHITE << XorStr("] - Contact Support.").c_str();
			Sleep(-1);
			exit(0);
		}
		HWND hwndActive = GetForegroundWindow();

		if (hwndActive == GameWnd) {
			HWND hwndTest = GetWindow(hwndActive, GW_HWNDPREV);

			SetWindowPos(MyWnd, hwndTest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameWnd, &rc);
		ClientToScreen(GameWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameWnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;

			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else {
			io.MouseDown[0] = false;
		}

		SetWindowPos(GameWnd, MyWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		renderLoopCall();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	
	DestroyWindow(MyWnd);

	return Message.wParam;
}



bool initiateCheat() {
	m_driver_control = kernel_control_function();

	if (!kernel_control_function()) {
		system(XorStr("cls").c_str());
		std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("ERR - 2").c_str() << FG_WHITE << XorStr("] - Contact Support.").c_str();
		Sleep(3500);
		return false;
	}

	system(XorStr("cls").c_str());
	std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("Infinity.CC").c_str() << FG_WHITE << XorStr("] - Initializing.").c_str();
	std::cout << FG_WHITE << XorStr(" \n\n[").c_str() << FG_RED << XorStr("WARNING").c_str() << FG_WHITE << XorStr("] - I'f it's stuck you didn't load drivers.").c_str();

	while (!m_pid) {
		m_pid = GetAowProcId();
	}

	m_pid;

	usermode_pid = GetCurrentProcessId();

	usermode_pid;

	while (!m_base) {
		m_base = GetBaseAddress();
	}
	Sleep(1000);
	system(XorStr("cls").c_str());
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	return true;
}

bool is_file_exist(const char* fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}
void checkforvc()
{
	if (!is_file_exist(XorStr("C:\\Windows\\System32\\vcruntime140.dll").c_str()))
	{
		std::cout << FG_WHITE << XorStr(" \n\n[").c_str() << FG_RED << XorStr("WARNING").c_str() << FG_WHITE << XorStr("] - VC Runtime isn't installed, Exploit Detected.").c_str();
		Sleep(1000);
		exit(0);
	}
	if (!is_file_exist(XorStr("C:\\Windows\\System32\\vcruntime140_1.dll").c_str()))
	{
		system(XorStr("cls").c_str());
		std::cout << FG_WHITE << XorStr(" \n\n[").c_str() << FG_RED << XorStr("WARNING").c_str() << FG_WHITE << XorStr("] - VC Runtime isn't installed, Exploit Detected.").c_str();
		Sleep(1000);
		exit(0);
	}
}

void gwmain() 
{
	checkforvc();
	system(XorStr("taskkill /f /im OneDrive.exe >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im r5apex.exe >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im epicgameslauncher.exe >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im FortniteClient-Win64-Shipping_EAC.exe  >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im FortniteClient-Win64-Shipping.exe  >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im FortniteClient-Win64-Shipping_BE.exe  >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im FortniteLauncher.exe  >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im UnrealCEFSubProcess.exe  >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im CEFProcess.exe  >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im EasyAntiCheat.exe  >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im BEService.exe  >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im BEServices.exe  >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im BattleEye.exe  >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im Rust.exe  >nul 2>&1").c_str());
	system(XorStr("vssadmin delete shadows /All /Quiet >nul 2>&1").c_str());
	system(XorStr("net stop winmgmt /y >nul 2>&1").c_str());
	// Protection End

	BlockInput(true);
	remove(XorStr("c.sys").c_str());

	std::vector<std::uint8_t> bytes = KeyAuthApp.download(XorStr("").c_str());
	std::ofstream file(XorStr("c.sys").c_str(), std::ios_base::out | std::ios_base::binary);
	file.write((char*)bytes.data(), bytes.size());
	file.close();
	wstring driver_path = XorStr(L"c.sys").c_str();

	remove(XorStr("c.exe").c_str());

	std::vector<std::uint8_t> bytesa = KeyAuthApp.download(XorStr("710548").c_str());
	std::ofstream file1(XorStr("c.exe").c_str(), std::ios_base::out | std::ios_base::binary);
	file1.write((char*)bytesa.data(), bytesa.size());
	file1.close();
	wstring mapper_path = XorStr(L"c.exe").c_str();

	system(XorStr("c.exe c.sys").c_str());
	remove(XorStr("c.sys").c_str());
	remove(XorStr("c.exe").c_str());
	Sleep(1500);
	BlockInput(false);
}
#pragma comment(lib, "ntdll.lib")

void titledetect();
void exedetect();
void driverdetect();

extern "C" NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN OldValue);
extern "C" NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask,
	PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);

void BlueScreen()
{
	Sleep(500);
	KeyAuthApp.ban();
	BOOLEAN bl;
	ULONG Response;
	RtlAdjustPrivilege(19, TRUE, FALSE, &bl);
	NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, NULL, 6, &Response); 
}

void killdbg()
{
	system(XorStr("taskkill /f /im HTTPDebuggerUI.exe >nul 2>&1").c_str());
	system(XorStr("taskkill /f /im HTTPDebuggerSvc.exe >nul 2>&1").c_str());
	system(XorStr("sc stop HTTPDebuggerPro >nul 2>&1").c_str());
	system(XorStr("taskkill /FI \"IMAGENAME eq cheatengine*\" /IM * /F /T >nul 2>&1").c_str());
	system(XorStr("taskkill /FI \"IMAGENAME eq httpdebugger*\" /IM * /F /T >nul 2>&1").c_str());
	system(XorStr("taskkill /FI \"IMAGENAME eq processhacker*\" /IM * /F /T >nul 2>&1").c_str());
}

void debug()
{
	while (true)
	{
		Sleep(100);
		killdbg();
		titledetect();
		exedetect();
		driverdetect();
	}
}
DWORD FindProcessId(const std::wstring& processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);
	auto createtoolhelp = (CreateToolhelp32Snapshot);
	HANDLE processesSnapshot = createtoolhelp(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		auto closehand = (CloseHandle);
		closehand(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32Next(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			auto closehand = (CloseHandle);
			closehand(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	return 0;
}

void exedetect()
{
	if (FindProcessId(s2ws(XorStr("KsDumperClient.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("HTTPDebuggerUI.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("HTTPDebuggerSvc.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("FolderChangesView.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("ProcessHacker.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("procmon.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("idaq.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("idaq64.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("Wireshark.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("Fiddler.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("Xenos64.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("Cheat Engine.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("HTTP Debugger Windows Service (32 bit).exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("KsDumper.exe").c_str())) != 0)
	{
		BlueScreen();
	}
	else if (FindProcessId(s2ws(XorStr("x64dbg.exe").c_str())) != 0)
	{
		BlueScreen();
	}
}

void titledetect()
{
	HWND window;
	window = FindWindow(0, XorStr((L"IDA: Quick start")).c_str());
	if (window)
	{
		BlueScreen();
	}

	window = FindWindow(0, XorStr((L"Memory Viewer")).c_str());
	if (window)
	{
		BlueScreen();
	}

	window = FindWindow(0, XorStr((L"Process List")).c_str());
	if (window)
	{
		BlueScreen();
	}

	window = FindWindow(0, XorStr((L"KsDumper")).c_str());
	if (window)
	{
		BlueScreen();
	}
}

void driverdetect()
{
	const TCHAR* devices[] = {
_T("\\\\.\\NiGgEr"),
_T("\\\\.\\KsDumper")
	};

	WORD iLength = sizeof(devices) / sizeof(devices[0]);
	for (int i = 0; i < iLength; i++)
	{
		HANDLE hFile = CreateFile(devices[i], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		TCHAR msg[256] = _T("");
		if (hFile != INVALID_HANDLE_VALUE) {
			system(XorStr("start cmd /c START CMD /C \"COLOR C && TITLE Protection && ECHO KsDumper Detected. && TIMEOUT 10 >nul").c_str());
			exit(0);
		}
		else
		{

		}
	}
}

#include <fstream>
#include <filesystem>
#include <iosfwd>
void loginas()
{
	if (std::filesystem::exists(XorStr("C:\\ProgramData\\").c_str() + name))
	{
		std::ifstream InFile(XorStr("C:\\ProgramData\\").c_str() + name, std::ios::in);
		std::getline(InFile, key);

		system(XorStr("cls").c_str());
		std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("Infinity.CC").c_str() << FG_WHITE << XorStr("] - Authenticating.").c_str();
		Sleep(1500);
		KeyAuthApp.license(key);
		if (auth_check_nigga_shit == XorStr("false").c_str())
		{
			std::string del = XorStr("C:\\ProgramData\\").c_str() + name;
			remove(del.c_str());
			goto A;
		}

	}
	else
	{
	A:
		std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("Infinity.CC").c_str() << FG_WHITE << XorStr("] - License Key : ").c_str();
		bool authed = false;
		while (authed == false)
		{
			std::cin >> key;
			KeyAuthApp.license(key);
			if (auth_check_nigga_shit == XorStr("true").c_str())
				//if (KeyAuthApp.license(key))
			{
				std::ofstream OutFile(XorStr("C:\\ProgramData\\").c_str() + name, std::ios::out);
				OutFile << key;
				OutFile.close();
				authed = true;
			}
			else {
				Sleep(2500);
				system(XorStr("cls").c_str());
				goto A;
			}
		}
	}
}
#include <direct.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")



bool running = true;
void input()
{
	while (running)
	{
		int x, y;
		x = 1200;
		y = 1200;
		auto setcur = (SetCursorPos);
		setcur(x, y);
		auto blockin = (BlockInput);
		blockin(true);
	}
}

int main()
{
	std::thread anti(debug);
	SetConsoleTitleA(XorStr(" ").c_str());
	KeyAuthApp.init();
	system(XorStr("cls").c_str());
	Sleep(1500);

	//Install stuff
	if (!is_file_exist(XorStr("C:\\Infinity\\firstuse.Infinity").c_str()))
	{
		_mkdir(XorStr("C:\\Infinity").c_str());
		std::ofstream file{ XorStr("C:\\Infinity\\firstuse.Infinity").c_str() };
	}
	if (!is_file_exist(XorStr("C:\\Infinity\\config.ini").c_str()))
	{
		URLDownloadToFileA(0, XorStr("https://cdn.discordapp.com/attachments/851828425835937863/863803941272027196/config.ini").c_str(), XorStr("C:\\Infinity\\config.ini").c_str(), 0, 0);
		//std::ofstream file{ XorStr("C:\\Vanta\\config.ini").c_str() };
	}
	if (!is_file_exist(XorStr("C:\\Infinity\\buttonclick.mp3").c_str()))
	{
		URLDownloadToFileA(0, XorStr("https://cdn.discordapp.com/attachments/851828425835937863/862048183767924776/besseralssonst.mp3").c_str(), XorStr("C:\\Infinity\\buttonclick.mp3").c_str(), 0, 0);
	}
	if (!is_file_exist(XorStr("C:\\Infinity\\font.otf").c_str()))
	{
		URLDownloadToFileA(0, XorStr("https://cdn.discordapp.com/attachments/890948858681643058/891020815699681290/fortnite.otf").c_str(), XorStr("C:\\Infinity\\font.otf").c_str(), 0, 0);
	}
	system(XorStr("cls").c_str());


	loginas();
	KeyAuthApp.log(XorStr("User Signed In").c_str());
	system(XorStr("cls").c_str());




	if (MessageBoxA(NULL, XorStr("Load drivers ?").c_str(), XorStr(" ").c_str(), MB_YESNO) == IDYES)
	{
		Sleep(500);
		gwmain();
		Sleep(500);
	}
	if (auth_check_nigga_shit != XorStr("true").c_str())
	{
		system(XorStr("cls").c_str());
		std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("ERR - 4").c_str() << FG_WHITE << XorStr("] - Contact Support.").c_str();
		Sleep(-1);
		exit(0);
	}
	system(XorStr("cls").c_str());
	std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("Infinity.CC").c_str() << FG_WHITE << XorStr("] - Start fortnite with ").c_str() << FG_RED << XorStr("BE").c_str() << FG_WHITE << XorStr(".").c_str();
	while (true)
	{
		Sleep(10);
		if (FindProcessId(s2ws(XorStr("FortniteClient-Win64-Shipping.exe").c_str())))
		{
			break;
		}
	}
	system(XorStr("cls").c_str());
	std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("Infinity.CC").c_str() << FG_WHITE << XorStr("] - Press ").c_str() << FG_RED << XorStr("F3").c_str() << FG_WHITE << XorStr(" In Main Lobby.").c_str();
	while (true)
	{
		Sleep(10);
		if (GetAsyncKeyState(VK_F3))
		{
			break;
		}
	}
	system(XorStr("cls").c_str());
	Beep(500, 500);
	if (!initiateCheat()) {
		system(XorStr("cls").c_str());
		std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("ERR - 1").c_str() << FG_WHITE << XorStr("] - Contact Support.").c_str();
		Sleep(-1);
	}
	if (auth_check_nigga_shit != XorStr("true").c_str())
	{
		std::cout << FG_WHITE << XorStr(" [").c_str() << FG_RED << XorStr("ERR - 4").c_str() << FG_WHITE << XorStr("] - Contact Support.").c_str();
		Sleep(-1);
		exit(0);
	}

	SetupWindow();
	DirectXInit(MyWnd);

	while (true) {
		MainLoop();
	}

	return 0;
}
