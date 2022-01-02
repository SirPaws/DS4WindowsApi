#pragma once
#ifndef DUALSHOCK4_HEADER
#define DUALSHOCK4_HEADER
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DUALSHOCK4_API_VERSION 4
#define DUALSHOCK4_MAX_USERS 4
#define DUALSHOCK4_WIRED 0
#define DUALSHOCK4_WIRELESS 1

#define DUALSHOCK4_SUCCEEDED 1
#define DUALSHOCK4_ERROR_INVALID_PARAMETER	0x0001
#define DUALSHOCK4_ERROR_DISCONNECTED		0x0002
#define DUALSHOCK4_ERROR_SEND_FAILED			0x0004
#define DUALSHOCK4_ERROR_INVALID_USER_INDEX	0x0008

#define DUALSHOCK4_ANALOG_RANGE 32767
#define DUALSHOCK4_ANALOG_DEADZONE 7849
#define DUALSHOCK4_TRIGGER_RANGE 255
#define DUALSHOCK4_GYRO_RANGE 3000
#define DUALSHOCK4_TOUCH_WIDTH 1920
#define DUALSHOCK4_TOUCH_HEIGHT 932


// Directional Pad 
#define DUALSHOCK4_DPAD_UP			0
#define DUALSHOCK4_DPAD_UP_RIGHT		1
#define DUALSHOCK4_DPAD_RIGHT		2
#define DUALSHOCK4_DPAD_DOWN_RIGHT	3
#define DUALSHOCK4_DPAD_DOWN			4
#define DUALSHOCK4_DPAD_DOWN_LEFT	5
#define DUALSHOCK4_DPAD_LEFT			6
#define DUALSHOCK4_DPAD_UP_LEFT		7

//buttons
//test
#define DUALSHOCK4_BUTTON_UP 0x00000001
#define DUALSHOCK4_BUTTON_RIGHT 0x00000002
#define DUALSHOCK4_BUTTON_DOWN 0x00000004
#define DUALSHOCK4_BUTTON_LEFT 0x00000008

// Gamepad Buttons
#define DUALSHOCK4_BUTTON_SQUARE 0x00000010
#define DUALSHOCK4_BUTTON_CROSS 0x00000020
#define DUALSHOCK4_BUTTON_CIRCLE 0x00000040
#define DUALSHOCK4_BUTTON_TRIANGLE 0x00000080
#define DUALSHOCK4_BUTTON_L1 0x00000100
#define DUALSHOCK4_BUTTON_R1 0x00000200
#define DUALSHOCK4_BUTTON_L2 0x00000400
#define DUALSHOCK4_BUTTON_R2 0x00000800
#define DUALSHOCK4_BUTTON_SHARE 0x00001000
#define DUALSHOCK4_BUTTON_OPTION 0x00002000
#define DUALSHOCK4_BUTTON_L3 0x00004000
#define DUALSHOCK4_BUTTON_R3 0x00008000
#define DUALSHOCK4_BUTTON_PS 0x00010000
#define DUALSHOCK4_BUTTON_TOUCHPAD 0x00100000


// DualShock 4 controller data
typedef struct DUALSHOCK4_GAMEPAD_
{
	// Digital buttons
	uint16_t Pad;
	uint32_t Buttons;

	// Triggers
	uint8_t LeftTrigger;
	uint8_t RightTrigger;

	// Analog sticks
	int16_t StickLeftX;
	int16_t StickLeftY;
	int16_t StickRightX;
	int16_t StickRightY;
	
	// Gyroscope
	int16_t GyroPitch;
	int16_t GyroYaw;
	int16_t GyroRoll;

	// Accelerometer
	int16_t AccelPitch;
	int16_t AccelYaw;
	int16_t AccelRoll;

	// Touchpad
	uint16_t TouchX[2];
	uint16_t TouchY[2];
	uint16_t TouchTrackId[2];
	uint8_t	 TouchCount;
	
}DUALSHOCK4_GAMEPAD;


typedef struct DUALSHOCK4_STATE_ {
	uint32_t PackageId;
	DUALSHOCK4_GAMEPAD Gamepad;
}DUALSHOCK4_STATE;

typedef struct DUALSHOCK4_VIBRATION_
{
	uint8_t LeftMotorSpeed;
	uint8_t RightMotorSpeed;
}DUALSHOCK4_VIBRATION;

typedef struct DUALSHOCK4_LIGHT_
{
	uint8_t Red;
	uint8_t Green;
	uint8_t Blue;
	uint8_t BlinkTime;
}DUALSHOCK4_LIGHT;

typedef struct DUALSHOCK4_BATTERY_INFORMATION_
{
	uint8_t Wired;
	uint8_t Level;
}DUALSHOCK4_BATTERY_INFORMATION;

//
//
//
uint32_t DualShock4Init();
void 	 DualShock4Shutdown();
uint32_t DualShock4GetVersion();
uint32_t DualShock4GetNumDevices();
uint32_t DualShock4DisconnectOnEmptyBattery(uint8_t enable);
uint32_t DualShock4GetState( uint32_t user_index, DUALSHOCK4_STATE* state );
uint32_t DualShock4GetDefaultLight( uint32_t user_index, DUALSHOCK4_LIGHT* light);
uint32_t DualShock4GetBatteryInfo( uint32_t user_index, DUALSHOCK4_BATTERY_INFORMATION* battery_info );
uint32_t DualShock4SetVibration( uint32_t user_index, DUALSHOCK4_VIBRATION* vib);
uint32_t DualShock4SetLight(uint32_t user_index, DUALSHOCK4_LIGHT* light);
uint32_t DualShock4SetLightBlink( uint32_t user_index, uint8_t time);

// these are only used if you handle Win32 loop yourself
#ifdef STRICT
int DualShock4InputRegister(struct HWND__ *hwnd);
int DualShock4InputUnregister(struct HWND__ *hwnd);
int DualShock4OnDeviceAdded(void *hDevice);
int DualShock4OnDeviceRemoved(void *hDevice);
int DualShock4OnDeviceInput(struct HRAWINPUT__**RawDataInputPtr);
#else
int DualShock4InputRegister(void *hwnd);
int DualShock4InputUnregister(void *hwnd);
int DualShock4OnDeviceAdded(void *hDevice);
int DualShock4OnDeviceRemoved(void *hDevice);
int DualShock4OnDeviceInput(void **RawDataInputPtr);
#endif

void DualShock4DisconnectAll(void);


#ifdef __cplusplus
}
#endif
#endif // DUALSHOCK4_HEADER
#if defined (DUALSHOCK4_IMPLEMENTATION)
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <hidsdi.h>
#include <hidusage.h>
#include <hidpi.h>
#include <hidclass.h>

#define CLAMP(a,b,x) (x < a)? a:(x > b) ? b: x;

// Device ids
#define DUALSHOCK4_DEVICE_PRODUCT_ID 1476
#define DUALSHOCK4V2_DEVICE_PRODUCT_ID 2508
#define DUALSHOCK4_DEVICE_VENDOR_ID 1356 

// Device flags
#define DUALSHOCK4_DEVICE_FLAG_CONNECTED 0x0001
#define DUALSHOCK4_DEVICE_FLAG_TYPE_USB	 0x0010
#define DUALSHOCK4_DEVICE_FLAG_TYPE_BT	 0x0020

//
//	Device Report
//
typedef struct DUALSHOCK4_DEVICE_INPUT_REPORT_INFO_
{
	uint8_t type;
	uint8_t offset;
	uint8_t pack_size;
	uint8_t unused;
}DUALSHOCK4_DEVICE_INPUT_REPORT_INFO;

//
//	Device Report Data
//
typedef struct DUALSHOCK4_DEVICE_REPORT_OUT_
{
	uint8_t id;
	uint8_t data[128];
	
}DUALSHOCK4_DEVICE_REPORT_OUT;


//
//	Device Info, this is a local backup of the device state
//
typedef struct DUALSHOCK4_DEVICE_INFO_
{
	uint16_t Flags;
	DUALSHOCK4_STATE State;
	DUALSHOCK4_LIGHT Light;
	DUALSHOCK4_VIBRATION Vibration;
	DUALSHOCK4_BATTERY_INFORMATION BatteryInfo;
	DUALSHOCK4_DEVICE_INPUT_REPORT_INFO ReportInfo;
}DUALSHOCK4_DEVICE_INFO;

//
//	Rawinput state, the state of the application
// this is the application interface
//
typedef struct RAWINPUT_STATE_
{
	uint32_t Initialized;
	HWND WindowHandle;
	HANDLE ThreadHandle;
	BYTE Buffer[8192];
	uint8_t DiconnectOnEmptyBattery;
	uint8_t DeviceConnectedNum;
	HANDLE DeviceHandle[DUALSHOCK4_MAX_USERS];
	HANDLE HIDHandle[DUALSHOCK4_MAX_USERS];
	DUALSHOCK4_DEVICE_INFO DeviceInfo[DUALSHOCK4_MAX_USERS];// might change this to DeviceData, makes more sense
	uint32_t ReportID[DUALSHOCK4_MAX_USERS];
}RAWINPUT_STATE;

extern RAWINPUT_STATE sRawInputState;

// put cod below inside a c unit file
#define REPORT_TYPE_USB 0x05
#define REPORT_TYPE_BT 0x11



// global instance of the app state
RAWINPUT_STATE sRawInputState = { 0 };


/*===============================
	DualShock 4 Lights Default
===============================*/
DUALSHOCK4_LIGHT sDualShockDefaultLightData[] =
{
	{ 0x00, 0x00, 0xFF, 0x00, },	// BLUE
	{ 0xFF, 0x00, 0x00, 0x00, },	// RED
	{ 0xFF, 0xFF, 0x00, 0x00, },	// GREEN
	{ 0xFF, 0x80, 0x80, 0x00, },	// PINK
};

DUALSHOCK4_LIGHT sDualShockDefaultLightOffData[] =
{
	{ 0x00, 0x00, 0x0a, 0x00, },	// BLUE
	{ 0x0a, 0x00, 0x00, 0x00, },	// RED
	{ 0x00, 0x0a, 0x00, 0x00, },	// GREEN
	{ 0x0f, 0x0a, 0x0a, 0x00, },	// PINK
};

/*===============================
	 DualShock 4 Event functions
===============================*/
int DualShock4OnDeviceAdded(HANDLE hDevice) 
{
	RID_DEVICE_INFO device_info = { 0 };
	unsigned int SizeInBytes = sizeof(RID_DEVICE_INFO);

	// acquire vendor and product ids
	GetRawInputDeviceInfo(hDevice, RIDI_DEVICEINFO, &device_info, &SizeInBytes);

	// this part belongs to the dualshock internaly stuff
	if (device_info.hid.dwVendorId == DUALSHOCK4_DEVICE_VENDOR_ID &&
		(	device_info.hid.dwProductId == DUALSHOCK4_DEVICE_PRODUCT_ID ||
			device_info.hid.dwProductId == DUALSHOCK4V2_DEVICE_PRODUCT_ID))
	{
		//int controller_index = -1;/
		BOOL device_found = FALSE;
		uint32_t device_index = 0;
		for (device_index = 0; device_index < DUALSHOCK4_MAX_USERS; device_index++)
		{
			if (sRawInputState.DeviceInfo[device_index].Flags == 0) 
			{
				device_found = TRUE;
				break;
			}
		}
		if (!device_found) 
		{
			return FALSE;
		}

		// get device name for connection
		wchar_t DeviceName[250] = L"";
		unsigned int DeviceNameLenght = 250;
		GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, &DeviceName, &DeviceNameLenght);

		// create device connection
		HANDLE HIDHandle = CreateFile(DeviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		sRawInputState.DeviceHandle[device_index] = hDevice;
		sRawInputState.HIDHandle[device_index] = HIDHandle;
		sRawInputState.DeviceConnectedNum++;
		// clear the device info
		memset(&sRawInputState.DeviceInfo[device_index], 0, sizeof(DUALSHOCK4_STATE));

		// check if connection is wired or wireless
		sRawInputState.DeviceInfo[device_index].Flags = DUALSHOCK4_DEVICE_FLAG_CONNECTED;
		sRawInputState.DeviceInfo[device_index].Flags |= (DeviceName[8] == '{') ? DUALSHOCK4_DEVICE_FLAG_TYPE_BT : DUALSHOCK4_DEVICE_FLAG_TYPE_USB;

		if (sRawInputState.DeviceInfo[device_index].Flags & DUALSHOCK4_DEVICE_FLAG_TYPE_BT)
		{
			sRawInputState.DeviceInfo[device_index].ReportInfo.type = 0x11;
			sRawInputState.DeviceInfo[device_index].ReportInfo.offset = 2;
			sRawInputState.DeviceInfo[device_index].ReportInfo.pack_size = 78;
			sRawInputState.DeviceInfo[device_index].BatteryInfo.Wired = FALSE;
		}
		else if (sRawInputState.DeviceInfo[device_index].Flags & DUALSHOCK4_DEVICE_FLAG_TYPE_USB)
		{
			sRawInputState.DeviceInfo[device_index].ReportInfo.type = 0x05;
			sRawInputState.DeviceInfo[device_index].ReportInfo.offset = 0;
			sRawInputState.DeviceInfo[device_index].ReportInfo.pack_size = 64;
			sRawInputState.DeviceInfo[device_index].BatteryInfo.Wired = TRUE;
		}
		DualShock4SetLight(device_index, &sDualShockDefaultLightData[device_index]);
		DualShock4SetLightBlink(device_index, 0);
		sRawInputState.ReportID[device_index] = 0;
		return TRUE;
	}

	return FALSE;
}
int DualShock4OnDeviceRemoved(HANDLE hDevice) 
{
	// look for the device with the same handle as the device that is disconnected

	for (unsigned int i = 0; i < 4; i++) {
		if (sRawInputState.DeviceHandle[i] == hDevice) {
			DUALSHOCK4_VIBRATION nullVib = { 0 };
			DUALSHOCK4_LIGHT nullLight = { 0 };
			DualShock4SetLight(i, &nullLight);
			DualShock4SetLightBlink(i, 255);
			DualShock4SetVibration(i, &nullVib);
			sRawInputState.DeviceHandle[i] = 0;
			CloseHandle(sRawInputState.HIDHandle[i]);
			sRawInputState.HIDHandle[i] = NULL;
			memset(&sRawInputState.DeviceInfo[i], 0, sizeof(DUALSHOCK4_STATE));
			sRawInputState.DeviceConnectedNum--;
			return TRUE;
		}
	}

	// device was not found
	return FALSE;
}
int DualShock4OnDeviceInput(HRAWINPUT* RawDataInputPtr)
{
	RID_DEVICE_INFO device_info = { 0 };
	unsigned int SizeInBytes = sizeof(RID_DEVICE_INFO);
	uint32_t device_index = UINT32_MAX;
	DUALSHOCK4_STATE* device_state_ptr = 0;
	uint32_t offset = 0;	
	RAWINPUT* pRaw = (RAWINPUT*)sRawInputState.Buffer;
	uint8_t* RawDataPtr = (uint8_t*)pRaw->data.hid.bRawData;
	UINT dwSize = 0;
	UINT readSize = 0;
	
	// acquire rawinput hid device data
	GetRawInputData(*RawDataInputPtr, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	memset(&sRawInputState.Buffer, 0, sizeof(sRawInputState.Buffer));
	readSize = GetRawInputData((HRAWINPUT)*RawDataInputPtr, RID_INPUT, sRawInputState.Buffer, &dwSize, sizeof(RAWINPUTHEADER));
	if (readSize != dwSize) 
	{
		return FALSE;
	}

	if (pRaw->header.dwType == RIM_TYPEHID)
	{

		// find the controller that is related to the handle
		for (device_index = 0; device_index < 4; device_index++)
		{
			if (sRawInputState.DeviceHandle[device_index] == pRaw->header.hDevice) // && sRawInputState.DeviceInfo[i].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED
			{
				device_state_ptr = &sRawInputState.DeviceInfo[device_index].State;
				offset = sRawInputState.DeviceInfo[device_index].ReportInfo.offset;
				break;
			}
		}


		if (device_state_ptr == 0)
		{
			// wrong device or just not found
			return FALSE;
		}



		// get package id
		sRawInputState.ReportID[device_index]++;
		sRawInputState.ReportID[device_index] = sRawInputState.ReportID[device_index] % _UI32_MAX;
		device_state_ptr->PackageId = sRawInputState.ReportID[device_index];// (DWORD)RawDataPtr[offset + 7];

		// get dpad data
		device_state_ptr->Gamepad.Pad = (WORD)RawDataPtr[offset + 5] % 16;//? why %16

		// get buttons data
		device_state_ptr->Gamepad.Buttons = 0;
		device_state_ptr->Gamepad.Buttons |= (RawDataPtr[offset + 5] & 0xf0);
		device_state_ptr->Gamepad.Buttons |= RawDataPtr[offset + 6] << 8;
		device_state_ptr->Gamepad.Buttons |= RawDataPtr[offset + 7] & 0x1 == 1 ? DUALSHOCK4_BUTTON_PS : 0;
		device_state_ptr->Gamepad.Buttons |= ((RawDataPtr[offset + 7] & 0x2) == 2) ? DUALSHOCK4_BUTTON_TOUCHPAD : 0;

		// get analog stick data
		device_state_ptr->Gamepad.StickLeftX = (-128 + RawDataPtr[offset + 1]) * 255;
		device_state_ptr->Gamepad.StickLeftY = -(-128 + RawDataPtr[offset + 2]) * 255;
		device_state_ptr->Gamepad.StickRightX = (-128 + RawDataPtr[offset + 3]) * 255;
		device_state_ptr->Gamepad.StickRightY = -(-128 + RawDataPtr[offset + 4]) * 255;

		// get gyroscope data
		device_state_ptr->Gamepad.GyroPitch = (*(short*)&RawDataPtr[offset + 23]);
		device_state_ptr->Gamepad.GyroYaw = (*(short*)&RawDataPtr[offset + 21]);
		device_state_ptr->Gamepad.GyroRoll = (*(short*)&RawDataPtr[offset + 19]);

		// get accelerometer data
		device_state_ptr->Gamepad.AccelPitch = *(short*)&RawDataPtr[offset + 13];
		device_state_ptr->Gamepad.AccelYaw = *(short*)&RawDataPtr[offset + 15];
		device_state_ptr->Gamepad.AccelRoll = *(short*)&RawDataPtr[offset + 17];

		// get trigger data
		device_state_ptr->Gamepad.LeftTrigger = RawDataPtr[offset + 8];
		device_state_ptr->Gamepad.RightTrigger = RawDataPtr[offset + 9];


		// its a messy way to set the flags for dpag states so that multiple buttons can be teste at once
		// for example DUALSHOCK4_BUTTON_UP & DUALSHOCK4_BUTTON_CROSS
		if (device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_UP ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_UP_LEFT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_UP_RIGHT)
		{
			device_state_ptr->Gamepad.Buttons |= DUALSHOCK4_BUTTON_UP;
		}

		if (device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_RIGHT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_DOWN_RIGHT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_UP_RIGHT)
		{
			device_state_ptr->Gamepad.Buttons |= DUALSHOCK4_BUTTON_RIGHT;
		}

		if (device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_DOWN ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_DOWN_LEFT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_DOWN_RIGHT)
		{
			device_state_ptr->Gamepad.Buttons |= DUALSHOCK4_BUTTON_DOWN;
		}

		if (device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_LEFT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_UP_LEFT ||
			device_state_ptr->Gamepad.Pad == DUALSHOCK4_DPAD_DOWN_LEFT)
		{
			device_state_ptr->Gamepad.Buttons |= DUALSHOCK4_BUTTON_LEFT;
		}
		//__u8 index = rd[30] <= 5 ? rd[30] : 5;
		sRawInputState.DeviceInfo[device_index].BatteryInfo.Level = RawDataPtr[offset + 30];// <= 5 ? RawDataPtr[offset + batcap_offset] : 5;//  (BYTE)RawDataPtr[offset + 12];


		//
		// get touchpad information
		device_state_ptr->Gamepad.TouchX[0] = RawDataPtr[offset + 36] | ((RawDataPtr[offset + 37] & 0x0f) << 8);
		device_state_ptr->Gamepad.TouchY[0] = (RawDataPtr[offset + 38]) << 4 | (RawDataPtr[offset + 37] & 0x0f);
		device_state_ptr->Gamepad.TouchX[1] = RawDataPtr[offset + 40] | ((RawDataPtr[offset + 41] & 0x0f) << 8);
		device_state_ptr->Gamepad.TouchY[1] = (RawDataPtr[offset + 42]) << 4 | (RawDataPtr[offset + 41] & 0x0f);
		device_state_ptr->Gamepad.TouchTrackId[0] = RawDataPtr[offset + 35] & 0x7F;
		device_state_ptr->Gamepad.TouchTrackId[1] = RawDataPtr[offset + 39] & 0x7F;
		device_state_ptr->Gamepad.TouchCount = 0;
		device_state_ptr->Gamepad.TouchCount += (RawDataPtr[offset + 35] >> 7) == 0 ? 1 : 0;
		device_state_ptr->Gamepad.TouchCount += (RawDataPtr[offset + 39] >> 7) == 0 ? 1 : 0;		
		

		// battery is empty
		if(sRawInputState.DiconnectOnEmptyBattery){
			if (sRawInputState.DeviceInfo[device_index].BatteryInfo.Level == 0) {
				sRawInputState.DeviceHandle[device_index] = 0;
				CloseHandle(sRawInputState.HIDHandle[device_index]);
				sRawInputState.HIDHandle[device_index] = NULL;
				memset(&sRawInputState.DeviceInfo[device_index], 0, sizeof(DUALSHOCK4_STATE));
				sRawInputState.DeviceConnectedNum--;
				memset(device_state_ptr, 0, sizeof(DUALSHOCK4_STATE));
			}
		}

		return TRUE;
	}

	return FALSE;
}
int DualShock4InputRegister(HWND hwnd) {

	
	memset(&sRawInputState, 0, sizeof(sRawInputState));
	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x05;
	Rid[0].dwFlags = RIDEV_DEVNOTIFY;
	Rid[0].hwndTarget = hwnd;
	if (!RegisterRawInputDevices(Rid, 1, sizeof(RAWINPUTDEVICE))) {
		return FALSE;
	}

	sRawInputState.Initialized = TRUE;
	sRawInputState.DiconnectOnEmptyBattery = FALSE;
	return TRUE;
}

int DualShock4InputUnregister(HWND hwnd) {
	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x05;
	Rid[0].dwFlags = RIDEV_REMOVE;
	Rid[0].hwndTarget = 0;
	RegisterRawInputDevices(Rid, 1, sizeof(RAWINPUTDEVICE));
	sRawInputState.Initialized = FALSE;
	memset(&sRawInputState, 0, sizeof(sRawInputState));
	return TRUE;
}

//
LRESULT CALLBACK DualShock4MsgCallback(HWND hWnd, UINT MessageID, WPARAM wParam, LPARAM lParam) 
{
	switch (MessageID) {
	default:break;
	case WM_CREATE:
		DualShock4InputRegister(hWnd);
		break;
	case WM_DESTROY:
		DualShock4InputUnregister(hWnd);
		return 1;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_INPUT:
		DualShock4OnDeviceInput((HRAWINPUT*)&lParam);
		break;
	case WM_INPUT_DEVICE_CHANGE:
		if (wParam == GIDC_ARRIVAL)
		{
			DualShock4OnDeviceAdded((HANDLE)lParam);
		}
		else if (wParam == GIDC_REMOVAL)
		{
			DualShock4OnDeviceRemoved((HANDLE)lParam);
		}
		break;
	}
	return DefWindowProc(hWnd, MessageID, wParam, lParam);
}

//
int CreateDualShock4InputWindow()
{
	ATOM result = 0;
	WNDCLASSEXA wc;
	
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = GetModuleHandle(0);

	wc.lpszClassName = "DS4WND_V3";
	wc.lpfnWndProc = DualShock4MsgCallback;

 	result = RegisterClassExA(&wc);
	if (!result) {
		return FALSE;
	}
	sRawInputState.WindowHandle = CreateWindowExA(0, wc.lpszClassName, "", 0, 0, 0, 0, 0, NULL, 0, GetModuleHandle(0), 0);
	if (!sRawInputState.WindowHandle) {
		return FALSE;
	}
	UpdateWindow(sRawInputState.WindowHandle);
	ShowWindow(sRawInputState.WindowHandle, SW_HIDE);
	return TRUE;
}

//
DWORD WINAPI DualShock4InputThread(LPVOID pParameter)
{
	MSG msg = { 0 };
	if (!CreateDualShock4InputWindow()) {
		return FALSE;
	}
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		//Sleep(0);
	}
	return (int)msg.wParam;
}

//
// Global api initializer
uint32_t DualShock4Init()
{
	// only initialize if api is not initialized already
	if(!sRawInputState.Initialized)
	{
		sRawInputState.ThreadHandle = CreateThread(0, 0, DualShock4InputThread, 0, 0, 0);
		if (!sRawInputState.ThreadHandle)
		{
			return FALSE;
		}
        while (!sRawInputState.Initialized) {
            (void)sRawInputState;
        }
	}
	return TRUE;
}

//
// Global api shutdown
void DualShock4Shutdown() 
{
	if(sRawInputState.Initialized)
	{		
		// close thread
		CloseHandle(sRawInputState.ThreadHandle);
		WaitForSingleObject(sRawInputState.ThreadHandle, INFINITE);
		
        DualShock4DisconnectAll();

		// unregister rawinput listener
		//DualShock4InputUnregister(sRawInputState.WindowHandle);

		// close window
		SendMessage(sRawInputState.WindowHandle, WM_CLOSE, 0, 0);
		UpdateWindow(sRawInputState.WindowHandle);


	}
}

void DualShock4DisconnectAll(void) {
    // disconenct all disconnect devices(or just turn down the light a bit
    DUALSHOCK4_VIBRATION nullVib = { 0 };
    for (UINT i = 0; i < DUALSHOCK4_MAX_USERS; i++)
    {
        //DualShock4SetLight(i, &sDualShockDefaultLightOffData[i]);
        //DualShock4SetVibration(i, &nullVib);

        DUALSHOCK4_VIBRATION nullVib = { 0 };
        DUALSHOCK4_LIGHT nullLight = { 0 };
        DualShock4SetLightBlink(i, 0);
        DualShock4SetLight(i, &sDualShockDefaultLightOffData[i]);

        DualShock4SetVibration(i, &nullVib);
        sRawInputState.DeviceHandle[i] = 0;
        CloseHandle(sRawInputState.HIDHandle[i]);
        sRawInputState.HIDHandle[i] = NULL;
        memset(&sRawInputState.DeviceInfo[i], 0, sizeof(DUALSHOCK4_STATE));
        sRawInputState.DeviceConnectedNum--;
    }
}

#define VIB_MASK 0xf1
#define LED_MASK ((1 << 1) | (1 << 3) |(1 << 5) | (1 << 7))
#define LED_BLINK_MASK (1 << 2)//((1 << 1) | (1 << 2) |(1 << 5) | (1 << 7))

#pragma comment(lib, "Setupapi.lib" )
#pragma comment(lib, "Hid.lib" )

/*-----------------------------
	DualShock api functions
--------------------------------*/
uint32_t DualShock4GetVersion()
{
	return DUALSHOCK4_API_VERSION;
}

uint32_t DualShock4GetNumDevices()
{
	return sRawInputState.DeviceConnectedNum;
}

uint32_t DualShock4GetState(uint32_t user_index, DUALSHOCK4_STATE* state)
{
	if (user_index >= DUALSHOCK4_MAX_USERS)
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (state == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_PARAMETER;
	}
	if (sRawInputState.DeviceInfo[user_index].Flags == 0)
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}
	memcpy(state, &sRawInputState.DeviceInfo[user_index].State, sizeof(DUALSHOCK4_STATE));
	return DUALSHOCK4_SUCCEEDED;
}

uint32_t DualShock4GetBatteryInfo(uint32_t user_index, DUALSHOCK4_BATTERY_INFORMATION* battery_info)
{
	if (user_index >= DUALSHOCK4_MAX_USERS) 
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (battery_info == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_PARAMETER;
	}
	if (!(sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED))
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}
	battery_info->Wired = sRawInputState.DeviceInfo[user_index].BatteryInfo.Wired;
	battery_info->Level = sRawInputState.DeviceInfo[user_index].BatteryInfo.Level;
	return DUALSHOCK4_SUCCEEDED;
}


uint32_t DualShock4GetDefaultLight(uint32_t user_index, _Out_ DUALSHOCK4_LIGHT* light) 
{
	if (user_index >= DUALSHOCK4_MAX_USERS || light == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (light == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_PARAMETER;
	}
	if (!(sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED))
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}
	memcpy(light, &sRawInputState.DeviceInfo[user_index].Light, sizeof(DUALSHOCK4_LIGHT));
	return DUALSHOCK4_SUCCEEDED;
}


//
uint32_t DualShock4SetVibration(uint32_t user_index, _Out_ DUALSHOCK4_VIBRATION* vib)
{
	DUALSHOCK4_DEVICE_REPORT_OUT* report_ptr = 0;
	DWORD byte_written = 0;
	uint32_t offset = 0;
	if (user_index >= DUALSHOCK4_MAX_USERS || vib == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (vib == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_PARAMETER;
	}
	if (!(sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED))
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}
	
	DUALSHOCK4_DEVICE_REPORT_OUT reportOut = { 0 };
	report_ptr = &reportOut;
	offset = sRawInputState.DeviceInfo[user_index].ReportInfo.offset;

	report_ptr->id = sRawInputState.DeviceInfo[user_index].ReportInfo.type;
	report_ptr->data[offset] = VIB_MASK;
	report_ptr->data[offset + 4] = vib->LeftMotorSpeed;
	report_ptr->data[offset + 3] = vib->RightMotorSpeed;
	
	if (sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_TYPE_BT) 
	{
		// if bluetooth device the first byte needs to be a different value
		report_ptr->data[0] = 128;
		if (!HidD_SetOutputReport(sRawInputState.HIDHandle[user_index], report_ptr, sizeof(DUALSHOCK4_DEVICE_REPORT_OUT)))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}
	else 
	{
		if (!WriteFile(sRawInputState.HIDHandle[user_index], report_ptr, sizeof(DUALSHOCK4_DEVICE_REPORT_OUT), &byte_written, NULL))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}

	return DUALSHOCK4_SUCCEEDED;
}

uint32_t DualShock4SetLight(uint32_t user_index, DUALSHOCK4_LIGHT* light) {
	DUALSHOCK4_DEVICE_REPORT_OUT* report_ptr = 0;
	DWORD byte_written = 0;
	uint32_t offset = 0;

	if (user_index >= DUALSHOCK4_MAX_USERS) 
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (light == 0)
	{
		return DUALSHOCK4_ERROR_INVALID_PARAMETER;
	}
	if (!(sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED))
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}

	DUALSHOCK4_DEVICE_REPORT_OUT reportOut = { 0 };
	report_ptr = &reportOut;
	offset = sRawInputState.DeviceInfo[user_index].ReportInfo.offset;
	report_ptr->id = sRawInputState.DeviceInfo[user_index].ReportInfo.type;
	report_ptr->data[offset] = LED_MASK ;// light->BlinkTime > 1 ? LED_BLINK_MASK : LED_MASK;

	report_ptr->data[offset + 5] = light->Red;
	report_ptr->data[offset + 6] = light->Green;
	report_ptr->data[offset + 7] = light->Blue;
	
	//report_ptr->data[offset + 8] = light->BlinkTime;
	//report_ptr->data[offset + 9] = light->BlinkTime;


	if (sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_TYPE_BT)
	{
		// if bluetooth device the first byte needs to be a different value
		report_ptr->data[0] = 128;// 0x0a | 0x02;
		uint32_t size = sRawInputState.DeviceInfo[user_index].ReportInfo.pack_size;
		if (!HidD_SetOutputReport(sRawInputState.HIDHandle[user_index], report_ptr, size))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}
	else 
	{
		uint32_t size = sRawInputState.DeviceInfo[user_index].ReportInfo.pack_size;
		if (!WriteFile(sRawInputState.HIDHandle[user_index], report_ptr, size, &byte_written, NULL))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}

	return DUALSHOCK4_SUCCEEDED;
}
uint32_t DualShock4DisconnectOnEmptyBattery(uint8_t enable){
	return sRawInputState.DiconnectOnEmptyBattery = enable;
}

uint32_t DualShock4SetLightBlink(uint32_t user_index, uint8_t time){
	DUALSHOCK4_DEVICE_REPORT_OUT* report_ptr = 0;
	DWORD byte_written = 0;
	uint32_t offset = 0;

	if (user_index >= DUALSHOCK4_MAX_USERS)
	{
		return DUALSHOCK4_ERROR_INVALID_USER_INDEX;
	}
	if (!(sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_CONNECTED))
	{
		return DUALSHOCK4_ERROR_DISCONNECTED;
	}

	DUALSHOCK4_DEVICE_REPORT_OUT reportOut = { 0 };
	report_ptr = &reportOut;
	offset = sRawInputState.DeviceInfo[user_index].ReportInfo.offset;
	report_ptr->id = sRawInputState.DeviceInfo[user_index].ReportInfo.type;
	report_ptr->data[offset] = LED_BLINK_MASK;
	report_ptr->data[offset + 8] = time;
	report_ptr->data[offset + 9] = time;


	if (sRawInputState.DeviceInfo[user_index].Flags & DUALSHOCK4_DEVICE_FLAG_TYPE_BT)
	{
		// if bluetooth device the first byte needs to be a different value
		report_ptr->data[0] = 128;// 0x0a | 0x02;
		uint32_t size = sRawInputState.DeviceInfo[user_index].ReportInfo.pack_size;
		if (!HidD_SetOutputReport(sRawInputState.HIDHandle[user_index], report_ptr, size))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}
	else
	{
		uint32_t size = sRawInputState.DeviceInfo[user_index].ReportInfo.pack_size;
		if (!WriteFile(sRawInputState.HIDHandle[user_index], report_ptr, size, &byte_written, NULL))
		{
			return DUALSHOCK4_ERROR_SEND_FAILED;
		}
	}

	return DUALSHOCK4_SUCCEEDED;
}
#endif
