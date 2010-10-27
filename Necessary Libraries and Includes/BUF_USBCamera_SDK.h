#ifndef _buf_usb_camera_sdk_h
#define  _buf_usb_camera_sdk_h

#include <windows.h>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MTUSBDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MTUSBDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
typedef int SDK_RETURN_CODE;
typedef unsigned int DEV_HANDLE;

#ifdef SDK_EXPORTS
#define SDK_API extern "C" __declspec(dllexport) SDK_RETURN_CODE _cdecl
#define SDK_HANDLE_API extern "C" __declspec(dllexport) DEV_HANDLE _cdecl
#define SDK_POINTER_API extern "C" __declspec(dllexport) unsigned char * _cdecl
#else
#define SDK_API __declspec(dllimport) SDK_RETURN_CODE _cdecl
#define SDK_HANDLE_API __declspec(dllimport) DEV_HANDLE _cdecl
#define SDK_POINTER_API  __declspec(dllimport) unsigned char * _cdecl
#endif

#define GRAB_FRAME_FOREVER	0x8888

#pragma pack(1)
#ifdef __cplusplus
 extern "C" {
 #endif
typedef struct {
    int Revision;
    // For Image Capture
    int Resolution;
    int ExposureTime;
    // GPIO Control
    unsigned char GpioConfigByte; // Config for Input/Output for each pin.
    unsigned char GpioCurrentSet; // For output Pins only.
} TImageControl;

typedef struct {
    int CameraID;
    int Row;
    int Column;
    int Bin;
    int XStart;
    int YStart;
    int ExposureTime;
    int RedGain;
    int GreenGain;
    int BlueGain;
    int TimeStamp;
    int TriggerOccurred;
    int TriggerEventCount;
    int ProcessFrameType;
    int FilterAcceptForFile;
} TProcessedDataProperty;

#pragma pack()

typedef TImageControl *PImageCtl;

typedef void (* DeviceFaultCallBack)( int DeviceType );
typedef void (* FrameDataCallBack)( TProcessedDataProperty* Attributes, unsigned char *BytePtr );

// Import functions:
SDK_API BUFUSB_InitDevice( void );
SDK_API BUFUSB_UnInitDevice( void );
SDK_API BUFUSB_GetModuleNoSerialNo( int DeviceID, char *ModuleNo, char *SerialNo);
SDK_API BUFUSB_AddDeviceToWorkingSet( int DeviceID );
SDK_API BUFUSB_RemoveDeviceFromWorkingSet( int DeviceID );
SDK_API BUFUSB_ActiveDeviceInWorkingSet( int DeviceID, int Active );
SDK_API BUFUSB_StartCameraEngine( HWND ParentHandle, int CameraBitOption );
SDK_API BUFUSB_StopCameraEngine( void );
SDK_API BUFUSB_SetCameraWorkMode( int DeviceID, int WorkMode );
SDK_API BUFUSB_StartFrameGrab( int TotalFrames );
SDK_API BUFUSB_StopFrameGrab( void );
SDK_API BUFUSB_ShowFactoryControlPanel( int DeviceID, char *passWord );
SDK_API BUFUSB_HideFactoryControlPanel( void );
SDK_API BUFUSB_SetResolution( int deviceID, int Resolution, int Bin, int BufferCnt );
SDK_API BUFUSB_SetCustomizedResolution( int deviceID, int RowSize, int ColSize, int Bin, int BufferCnt );
SDK_API BUFUSB_SetExposureTime( int DeviceID, int exposureTime );
SDK_API BUFUSB_SetFrameTime( int DeviceID, int frameTime );
SDK_API BUFUSB_SetXYStart( int deviceID, int XStart, int YStart );
SDK_API BUFUSB_SetGains( int deviceID, int RedGain, int GreenGain, int BlueGain );
SDK_API BUFUSB_SetGamma( int Gamma, int Contrast, int Bright, int Sharp );
SDK_API BUFUSB_SetBWMode( int BWMode, int H_Mirror, int V_Flip );
SDK_API BUFUSB_InstallFrameHooker( int FrameType, FrameDataCallBack FrameHooker );
SDK_API BUFUSB_InstallUSBDeviceHooker( DeviceFaultCallBack USBDeviceHooker );
//SDK_POINTER_API BUFUSB_GetCurrentFrame( int FrameType, int deviceID, byte* &FramePtr );
SDK_API BUFUSB_GetDevicesErrorState();
SDK_API BUFUSB_SetGPIOConifg( int DeviceID, unsigned char ConfigByte );
SDK_API BUFUSB_SetGPIOInOut( int DeviceID, unsigned char OutputByte,
                             unsigned char *InputBytePtr );

#ifdef __cplusplus
 }
 #endif
#endif
