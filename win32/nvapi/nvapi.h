#include"nvapi_lite_salstart.h"
#include"nvapi_lite_common.h"
#include"nvapi_lite_surround.h"

 /************************************************************************************************************************************\
|*                                                                                                                                    *|
|*     Copyright © 2012 NVIDIA Corporation.  All rights reserved.                                                                     *|
|*                                                                                                                                    *|
|*  NOTICE TO USER:                                                                                                                   *|
|*                                                                                                                                    *|
|*  This software is subject to NVIDIA ownership rights under U.S. and international Copyright laws.                                  *|
|*                                                                                                                                    *|
|*  This software and the information contained herein are PROPRIETARY and CONFIDENTIAL to NVIDIA                                     *|
|*  and are being provided solely under the terms and conditions of an NVIDIA software license agreement.                             *|
|*  Otherwise, you have no rights to use or access this software in any manner.                                                       *|
|*                                                                                                                                    *|
|*  If not covered by the applicable NVIDIA software license agreement:                                                               *|
|*  NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.                                            *|
|*  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.                                                           *|
|*  NVIDIA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,                                                                     *|
|*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.                       *|
|*  IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,                               *|
|*  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT,                         *|
|*  NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.            *|
|*                                                                                                                                    *|
|*  U.S. Government End Users.                                                                                                        *|
|*  This software is a "commercial item" as that term is defined at 48 C.F.R. 2.101 (OCT 1995),                                       *|
|*  consisting  of "commercial computer  software"  and "commercial computer software documentation"                                  *|
|*  as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995) and is provided to the U.S. Government only as a commercial end item.     *|
|*  Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through 227.7202-4 (JUNE 1995),                                          *|
|*  all U.S. Government End Users acquire the software with only those rights set forth herein.                                       *|
|*                                                                                                                                    *|
|*  Any use of this software in individual and commercial software must include,                                                      *|
|*  in the user documentation and internal comments to the code,                                                                      *|
|*  the above Disclaimer (as applicable) and U.S. Government End Users Notice.                                                        *|
|*                                                                                                                                    *|
 \************************************************************************************************************************************/
 

///////////////////////////////////////////////////////////////////////////////
//
// Date: Dec 10, 2020 
// File: nvapi.h
//
// NvAPI provides an interface to NVIDIA devices. This file contains the 
// interface constants, structure definitions and function prototypes.
//
//   Target Profile: developer
//  Target Platform: windows
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _NVAPI_H
#define _NVAPI_H

#pragma pack(push,8) // Make sure we have consistent structure packings

#ifdef __cplusplus
extern "C" {
#endif
// ====================================================
// Universal NvAPI Definitions
// ====================================================
#ifndef _WIN32
#define __cdecl
#endif



//! @}


//!   \ingroup nvapistatus
#define NVAPI_API_NOT_INTIALIZED        NVAPI_API_NOT_INITIALIZED       //!< Fix typo in error code

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Initialize
//
//! This function initializes the NvAPI library (if not already initialized) but always increments the ref-counter.
//! This must be called before calling other NvAPI_ functions.
//! Note: It is now mandatory to call NvAPI_Initialize before calling any other NvAPI.
//! NvAPI_Unload should be called to unload the NVAPI Library.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \return      This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!              specific meaning for this API, they are listed below.
//! \retval      NVAPI_LIBRARY_NOT_FOUND  Failed to load the NVAPI support library
//! \sa nvapistatus
//! \ingroup nvapifunctions
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Initialize();


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Unload
//
//!   DESCRIPTION: Decrements the ref-counter and when it reaches ZERO, unloads NVAPI library.
//!                This must be called in pairs with NvAPI_Initialize.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//!        If the client wants unload functionality, it is recommended to always call NvAPI_Initialize and NvAPI_Unload in pairs.
//!
//!  Unloading NvAPI library is not supported when the library is in a resource locked state.
//!  Some functions in the NvAPI library initiates an operation or allocates certain resources
//!  and there are corresponding functions available, to complete the operation or free the
//!  allocated resources. All such function pairs are designed to prevent unloading NvAPI library.
//!
//!  For example, if NvAPI_Unload is called after NvAPI_XXX which locks a resource, it fails with
//!  NVAPI_ERROR. Developers need to call the corresponding NvAPI_YYY to unlock the resources,
//!  before calling NvAPI_Unload again.
//!
//! \return      This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!              specific meaning for this API, they are listed below.
//! \retval      NVAPI_API_IN_USE       Atleast an API is still being called hence cannot unload requested driver.
//!
//! \ingroup nvapifunctions
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Unload();

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetErrorMessage
//
//! This function converts an NvAPI error code into a null terminated string.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \param nr      The error code to convert
//! \param szDesc  The string corresponding to the error code
//!
//! \return NULL terminated string (always, never NULL)
//! \ingroup nvapifunctions
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetErrorMessage(NvAPI_Status nr,NvAPI_ShortString szDesc);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetInterfaceVersionString
//
//! This function returns a string describing the version of the NvAPI library.
//!               The contents of the string are human readable.  Do not assume a fixed
//!                format.
//!
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \param  szDesc User readable string giving NvAPI version information
//!
//! \return See \ref nvapistatus for the list of possible return values.
//! \ingroup nvapifunctions
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetInterfaceVersionString(NvAPI_ShortString szDesc);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetInterfaceVersionStringEx
//
//! This function returns a string describing the version of the NvAPI library.
//!               The contents of the string are human readable.  Do not assume a fixed  format.
//!
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 450
//!
//! \param  szDesc User readable string giving NvAPI version information
//!
//! \return See \ref nvapistatus for the list of possible return values.
//! \ingroup nvapifunctions
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetInterfaceVersionStringEx(NvAPI_ShortString szDesc);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//              All display port related data types definition starts
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This category is intentionally added before the #ifdef. The #endif should also be in the same scope

#ifndef DISPLAYPORT_STRUCTS_DEFINED
#define DISPLAYPORT_STRUCTS_DEFINED

//! \ingroup dispcontrol
//! Used in NV_DISPLAY_PORT_INFO.
typedef enum _NV_DP_LINK_RATE
{
    NV_DP_1_62GBPS            = 6,
    NV_DP_2_70GBPS            = 0xA,
    NV_DP_5_40GBPS            = 0x14,
    NV_DP_8_10GBPS            = 0x1E,
// Recommended Intermidiate Link Transfer Rates
    NV_EDP_2_16GBPS           = 8,
    NV_EDP_2_43GBPS           = 9,
    NV_EDP_3_24GBPS           = 0xC,
    NV_EDP_4_32GBPS           = 0x10
} NV_DP_LINK_RATE;


//! \ingroup dispcontrol
//! Used in NV_DISPLAY_PORT_INFO.
typedef enum _NV_DP_LANE_COUNT
{
    NV_DP_1_LANE              = 1,
    NV_DP_2_LANE              = 2,
    NV_DP_4_LANE              = 4,
} NV_DP_LANE_COUNT;


//! \ingroup dispcontrol
//! Used in NV_DISPLAY_PORT_INFO.
typedef enum _NV_DP_COLOR_FORMAT
{
    NV_DP_COLOR_FORMAT_RGB     = 0,
    NV_DP_COLOR_FORMAT_YCbCr422,
    NV_DP_COLOR_FORMAT_YCbCr444,
} NV_DP_COLOR_FORMAT;


//! \ingroup dispcontrol
//! Used in NV_DISPLAY_PORT_INFO.
typedef enum _NV_DP_COLORIMETRY
{
    NV_DP_COLORIMETRY_RGB     = 0,
    NV_DP_COLORIMETRY_YCbCr_ITU601,
    NV_DP_COLORIMETRY_YCbCr_ITU709,
} NV_DP_COLORIMETRY;


//! \ingroup dispcontrol
//! Used in NV_DISPLAY_PORT_INFO.
typedef enum _NV_DP_DYNAMIC_RANGE
{
    NV_DP_DYNAMIC_RANGE_VESA  = 0,
    NV_DP_DYNAMIC_RANGE_CEA,
} NV_DP_DYNAMIC_RANGE;


//! \ingroup dispcontrol
//! Used in NV_DISPLAY_PORT_INFO.
typedef enum _NV_DP_BPC
{
    NV_DP_BPC_DEFAULT         = 0,
    NV_DP_BPC_6,
    NV_DP_BPC_8,
    NV_DP_BPC_10,
    NV_DP_BPC_12,
    NV_DP_BPC_16,
} NV_DP_BPC;


#endif  //#ifndef DISPLAYPORT_STRUCTS_DEFINED

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//              All display port related data types definitions end
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetEDID
//
//! \fn NvAPI_GPU_GetEDID(NvPhysicalGpuHandle hPhysicalGpu, NvU32 displayOutputId, NV_EDID *pEDID)
//!  This function returns the EDID data for the specified GPU handle and connection bit mask.
//!  displayOutputId should have exactly 1 bit set to indicate a single display. See \ref handles.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 85
//!
//! \retval    NVAPI_INVALID_ARGUMENT              pEDID is NULL; displayOutputId has 0 or > 1 bits set
//! \retval    NVAPI_OK                           *pEDID contains valid data.
//! \retval    NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//! \retval    NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle.
//! \retval    NVAPI_DATA_NOT_FOUND                The requested display does not contain an EDID.
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup gpu
//! @{

#define NV_EDID_V1_DATA_SIZE   256

#define NV_EDID_DATA_SIZE      NV_EDID_V1_DATA_SIZE

typedef struct
{
    NvU32   version;        //structure version
    NvU8    EDID_Data[NV_EDID_DATA_SIZE];
} NV_EDID_V1;

//! Used in NvAPI_GPU_GetEDID()
typedef struct
{
    NvU32   version;        //!< Structure version
    NvU8    EDID_Data[NV_EDID_DATA_SIZE];
    NvU32   sizeofEDID;
} NV_EDID_V2;

//! Used in NvAPI_GPU_GetEDID()
typedef struct
{
    NvU32   version;        //!< Structure version
    NvU8    EDID_Data[NV_EDID_DATA_SIZE];
    NvU32   sizeofEDID;
    NvU32   edidId;     //!< ID which always returned in a monotonically increasing counter.
                       //!< Across a split-EDID read we need to verify that all calls returned the same edidId.
                       //!< This counter is incremented if we get the updated EDID.
    NvU32   offset;    //!< Which 256-byte page of the EDID we want to read. Start at 0.
                       //!< If the read succeeds with edidSize > NV_EDID_DATA_SIZE,
                       //!< call back again with offset+256 until we have read the entire buffer
} NV_EDID_V3;

typedef NV_EDID_V3    NV_EDID;

#define NV_EDID_VER1    MAKE_NVAPI_VERSION(NV_EDID_V1,1)
#define NV_EDID_VER2    MAKE_NVAPI_VERSION(NV_EDID_V2,2)
#define NV_EDID_VER3    MAKE_NVAPI_VERSION(NV_EDID_V3,3)
#define NV_EDID_VER   NV_EDID_VER3

//! @}

//! \ingroup gpu
NVAPI_INTERFACE NvAPI_GPU_GetEDID(NvPhysicalGpuHandle hPhysicalGpu, NvU32 displayOutputId, NV_EDID *pEDID);

//! \ingroup gpu
//! Used in NV_GPU_CONNECTOR_DATA
typedef enum _NV_GPU_CONNECTOR_TYPE
{
    NVAPI_GPU_CONNECTOR_VGA_15_PIN                      = 0x00000000,
    NVAPI_GPU_CONNECTOR_TV_COMPOSITE                    = 0x00000010,
    NVAPI_GPU_CONNECTOR_TV_SVIDEO                       = 0x00000011,
    NVAPI_GPU_CONNECTOR_TV_HDTV_COMPONENT               = 0x00000013,
    NVAPI_GPU_CONNECTOR_TV_SCART                        = 0x00000014,
    NVAPI_GPU_CONNECTOR_TV_COMPOSITE_SCART_ON_EIAJ4120  = 0x00000016,
    NVAPI_GPU_CONNECTOR_TV_HDTV_EIAJ4120                = 0x00000017,
    NVAPI_GPU_CONNECTOR_PC_POD_HDTV_YPRPB               = 0x00000018,
    NVAPI_GPU_CONNECTOR_PC_POD_SVIDEO                   = 0x00000019,
    NVAPI_GPU_CONNECTOR_PC_POD_COMPOSITE                = 0x0000001A,
    NVAPI_GPU_CONNECTOR_DVI_I_TV_SVIDEO                 = 0x00000020,
    NVAPI_GPU_CONNECTOR_DVI_I_TV_COMPOSITE              = 0x00000021,
    NVAPI_GPU_CONNECTOR_DVI_I                           = 0x00000030,
    NVAPI_GPU_CONNECTOR_DVI_D                           = 0x00000031,
    NVAPI_GPU_CONNECTOR_ADC                             = 0x00000032,
    NVAPI_GPU_CONNECTOR_LFH_DVI_I_1                     = 0x00000038,
    NVAPI_GPU_CONNECTOR_LFH_DVI_I_2                     = 0x00000039,
    NVAPI_GPU_CONNECTOR_SPWG                            = 0x00000040,
    NVAPI_GPU_CONNECTOR_OEM                             = 0x00000041,
    NVAPI_GPU_CONNECTOR_DISPLAYPORT_EXTERNAL            = 0x00000046,
    NVAPI_GPU_CONNECTOR_DISPLAYPORT_INTERNAL            = 0x00000047,
    NVAPI_GPU_CONNECTOR_DISPLAYPORT_MINI_EXT            = 0x00000048,
    NVAPI_GPU_CONNECTOR_HDMI_A                          = 0x00000061,
    NVAPI_GPU_CONNECTOR_HDMI_C_MINI                     = 0x00000063,
    NVAPI_GPU_CONNECTOR_LFH_DISPLAYPORT_1               = 0x00000064,
    NVAPI_GPU_CONNECTOR_LFH_DISPLAYPORT_2               = 0x00000065,
    NVAPI_GPU_CONNECTOR_VIRTUAL_WFD                     = 0x00000070,
    NVAPI_GPU_CONNECTOR_USB_C                           = 0x00000071,
    NVAPI_GPU_CONNECTOR_UNKNOWN                         = 0xFFFFFFFF,
} NV_GPU_CONNECTOR_TYPE;

////////////////////////////////////////////////////////////////////////////////
//
// NvAPI_TVOutput Information
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup tvapi
//! Used in NV_DISPLAY_TV_OUTPUT_INFO
typedef enum _NV_DISPLAY_TV_FORMAT
{
    NV_DISPLAY_TV_FORMAT_NONE               = 0,
    NV_DISPLAY_TV_FORMAT_SD_NTSCM           = 0x00000001,
    NV_DISPLAY_TV_FORMAT_SD_NTSCJ           = 0x00000002,
    NV_DISPLAY_TV_FORMAT_SD_PALM            = 0x00000004,
    NV_DISPLAY_TV_FORMAT_SD_PALBDGH         = 0x00000008,
    NV_DISPLAY_TV_FORMAT_SD_PALN            = 0x00000010,
    NV_DISPLAY_TV_FORMAT_SD_PALNC           = 0x00000020,
    NV_DISPLAY_TV_FORMAT_SD_576i            = 0x00000100,
    NV_DISPLAY_TV_FORMAT_SD_480i            = 0x00000200,
    NV_DISPLAY_TV_FORMAT_ED_480p            = 0x00000400,
    NV_DISPLAY_TV_FORMAT_ED_576p            = 0x00000800,
    NV_DISPLAY_TV_FORMAT_HD_720p            = 0x00001000,
    NV_DISPLAY_TV_FORMAT_HD_1080i           = 0x00002000,
    NV_DISPLAY_TV_FORMAT_HD_1080p           = 0x00004000,
    NV_DISPLAY_TV_FORMAT_HD_720p50          = 0x00008000,
    NV_DISPLAY_TV_FORMAT_HD_1080p24         = 0x00010000,
    NV_DISPLAY_TV_FORMAT_HD_1080i50         = 0x00020000,
    NV_DISPLAY_TV_FORMAT_HD_1080p50         = 0x00040000,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp30          = 0x00080000,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp30_3840     = NV_DISPLAY_TV_FORMAT_UHD_4Kp30,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp25          = 0x00100000,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp25_3840     = NV_DISPLAY_TV_FORMAT_UHD_4Kp25,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp24          = 0x00200000,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp24_3840     = NV_DISPLAY_TV_FORMAT_UHD_4Kp24,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp24_SMPTE    = 0x00400000,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp50_3840     = 0x00800000,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp60_3840     = 0x00900000,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp30_4096     = 0x00A00000,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp25_4096     = 0x00B00000,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp24_4096     = 0x00C00000,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp50_4096     = 0x00D00000,
    NV_DISPLAY_TV_FORMAT_UHD_4Kp60_4096     = 0x00E00000,
    NV_DISPLAY_TV_FORMAT_UHD_8Kp24_7680     = 0x01000000,
    NV_DISPLAY_TV_FORMAT_UHD_8Kp25_7680     = 0x02000000,
    NV_DISPLAY_TV_FORMAT_UHD_8Kp30_7680     = 0x04000000,
    NV_DISPLAY_TV_FORMAT_UHD_8Kp48_7680     = 0x08000000,
    NV_DISPLAY_TV_FORMAT_UHD_8Kp50_7680     = 0x09000000,
    NV_DISPLAY_TV_FORMAT_UHD_8Kp60_7680     = 0x0A000000,
    NV_DISPLAY_TV_FORMAT_UHD_8Kp100_7680    = 0x0B000000,
    NV_DISPLAY_TV_FORMAT_UHD_8Kp120_7680    = 0x0C000000,


    NV_DISPLAY_TV_FORMAT_SD_OTHER           = 0x10000000,
    NV_DISPLAY_TV_FORMAT_ED_OTHER           = 0x20000000,
    NV_DISPLAY_TV_FORMAT_HD_OTHER           = 0x40000000,

    NV_DISPLAY_TV_FORMAT_ANY                = 0x80000000,

} NV_DISPLAY_TV_FORMAT;


//! \ingroup dispcontrol
//! @{
#define NVAPI_MAX_VIEW_TARGET  2
#define NVAPI_ADVANCED_MAX_VIEW_TARGET 4

#ifndef _NV_TARGET_VIEW_MODE_
#define _NV_TARGET_VIEW_MODE_

//! Used in NvAPI_SetView().
typedef enum _NV_TARGET_VIEW_MODE
{
    NV_VIEW_MODE_STANDARD  = 0,
    NV_VIEW_MODE_CLONE     = 1,
    NV_VIEW_MODE_HSPAN     = 2,
    NV_VIEW_MODE_VSPAN     = 3,
    NV_VIEW_MODE_DUALVIEW  = 4,
    NV_VIEW_MODE_MULTIVIEW = 5,
} NV_TARGET_VIEW_MODE;
#endif

//! @}

// Following definitions are used in NvAPI_SetViewEx.

//! Scaling modes - used in NvAPI_SetViewEx().
//! \ingroup dispcontrol
typedef enum _NV_SCALING
{
    NV_SCALING_DEFAULT          = 0,        //!< No change

    // New Scaling Declarations
    NV_SCALING_GPU_SCALING_TO_CLOSEST                   = 1,  //!< Balanced  - Full Screen
    NV_SCALING_GPU_SCALING_TO_NATIVE                    = 2,  //!< Force GPU - Full Screen
    NV_SCALING_GPU_SCANOUT_TO_NATIVE                    = 3,  //!< Force GPU - Centered\No Scaling
    NV_SCALING_GPU_SCALING_TO_ASPECT_SCANOUT_TO_NATIVE  = 5,  //!< Force GPU - Aspect Ratio
    NV_SCALING_GPU_SCALING_TO_ASPECT_SCANOUT_TO_CLOSEST = 6,  //!< Balanced  - Aspect Ratio
    NV_SCALING_GPU_SCANOUT_TO_CLOSEST                   = 7,  //!< Balanced  - Centered\No Scaling
    NV_SCALING_GPU_INTEGER_ASPECT_SCALING               = 8,  //!< Force GPU - Integer Scaling

    // Legacy Declarations
    NV_SCALING_MONITOR_SCALING                          = NV_SCALING_GPU_SCALING_TO_CLOSEST,
    NV_SCALING_ADAPTER_SCALING                          = NV_SCALING_GPU_SCALING_TO_NATIVE,
    NV_SCALING_CENTERED                                 = NV_SCALING_GPU_SCANOUT_TO_NATIVE,
    NV_SCALING_ASPECT_SCALING                           = NV_SCALING_GPU_SCALING_TO_ASPECT_SCANOUT_TO_NATIVE,

    NV_SCALING_CUSTOMIZED       = 255       //!< For future use
} NV_SCALING;

//! Rotate modes- used in NvAPI_SetViewEx().

//! \ingroup dispcontrol
typedef enum _NV_ROTATE
{
    NV_ROTATE_0           = 0,
    NV_ROTATE_90          = 1,
    NV_ROTATE_180         = 2,
    NV_ROTATE_270         = 3,
    NV_ROTATE_IGNORED     = 4,
} NV_ROTATE;

//! Color formats- used in NvAPI_SetViewEx().
//! \ingroup dispcontrol
#define NVFORMAT_MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                         ((NvU32)(NvU8)(ch0) | ((NvU32)(NvU8)(ch1) << 8) |   \
                     ((NvU32)(NvU8)(ch2) << 16) | ((NvU32)(NvU8)(ch3) << 24 ))



//! Color formats- used in NvAPI_SetViewEx().
//! \ingroup dispcontrol
typedef enum _NV_FORMAT
{
    NV_FORMAT_UNKNOWN           =  0,       //!< unknown. Driver will choose one as following value.
    NV_FORMAT_P8                = 41,       //!< for 8bpp mode
    NV_FORMAT_R5G6B5            = 23,       //!< for 16bpp mode
    NV_FORMAT_A8R8G8B8          = 21,       //!< for 32bpp mode
    NV_FORMAT_A16B16G16R16F     = 113,      //!< for 64bpp(floating point) mode.

} NV_FORMAT;

// TV standard

typedef struct
{
    float x;    //!<  x-coordinate of the viewport top-left point
    float y;    //!<  y-coordinate of the viewport top-left point
    float w;    //!<  Width of the viewport
    float h;    //!<  Height of the viewport
} NV_VIEWPORTF;



//! \ingroup dispcontrol
//! The timing override is not supported yet; must be set to _AUTO. \n


typedef enum _NV_TIMING_OVERRIDE
{
    NV_TIMING_OVERRIDE_CURRENT = 0,          //!< get the current timing
    NV_TIMING_OVERRIDE_AUTO,                 //!< the timing the driver will use based the current policy
    NV_TIMING_OVERRIDE_EDID,                 //!< EDID timing
    NV_TIMING_OVERRIDE_DMT,                  //!< VESA DMT timing
    NV_TIMING_OVERRIDE_DMT_RB,               //!< VESA DMT timing with reduced blanking
    NV_TIMING_OVERRIDE_CVT,                  //!< VESA CVT timing
    NV_TIMING_OVERRIDE_CVT_RB,               //!< VESA CVT timing with reduced blanking
    NV_TIMING_OVERRIDE_GTF,                  //!< VESA GTF timing
    NV_TIMING_OVERRIDE_EIA861,               //!< EIA 861x pre-defined timing
    NV_TIMING_OVERRIDE_ANALOG_TV,            //!< analog SD/HDTV timing
    NV_TIMING_OVERRIDE_CUST,                 //!< NV custom timings
    NV_TIMING_OVERRIDE_NV_PREDEFINED,        //!< NV pre-defined timing (basically the PsF timings)
    NV_TIMING_OVERRIDE_NV_PSF                = NV_TIMING_OVERRIDE_NV_PREDEFINED,
    NV_TIMING_OVERRIDE_NV_ASPR,
    NV_TIMING_OVERRIDE_SDI,                  //!< Override for SDI timing

    NV_TIMING_OVRRIDE_MAX,
}NV_TIMING_OVERRIDE;


#ifndef NV_TIMING_STRUCTS_DEFINED
#define NV_TIMING_STRUCTS_DEFINED

//***********************
// The Timing Structure
//***********************
//
//! \ingroup dispcontrol
//!  NVIDIA-specific timing extras \n
//! Used in NV_TIMING.
typedef struct tagNV_TIMINGEXT
{
    NvU32   flag;          //!< Reserved for NVIDIA hardware-based enhancement, such as double-scan.
    NvU16   rr;            //!< Logical refresh rate to present
    NvU32   rrx1k;         //!< Physical vertical refresh rate in 0.001Hz
    NvU32   aspect;        //!< Display aspect ratio Hi(aspect):horizontal-aspect, Low(aspect):vertical-aspect
    NvU16   rep;           //!< Bit-wise pixel repetition factor: 0x1:no pixel repetition; 0x2:each pixel repeats twice horizontally,..
    NvU32   status;        //!< Timing standard
    NvU8    name[40];      //!< Timing name
}NV_TIMINGEXT;



//! \ingroup dispcontrol
//!The very basic timing structure based on the VESA standard:
//! \code
//!            |<----------------------------htotal--------------------------->|
//!             ---------"active" video-------->|<-------blanking------>|<-----
//!            |<-------hvisible-------->|<-hb->|<-hfp->|<-hsw->|<-hbp->|<-hb->|
//! --------- -+-------------------------+      |       |       |       |      |
//!   A      A |                         |      |       |       |       |      |
//!   :      : |                         |      |       |       |       |      |
//!   :      : |                         |      |       |       |       |      |
//!   :vertical|    addressable video    |      |       |       |       |      |
//!   : visible|                         |      |       |       |       |      |
//!   :      : |                         |      |       |       |       |      |
//!   :      : |                         |      |       |       |       |      |
//! vertical V |                         |      |       |       |       |      |
//!  total   --+-------------------------+      |       |       |       |      |
//!   :      vb         border                  |       |       |       |      |
//!   :      -----------------------------------+       |       |       |      |
//!   :      vfp        front porch                     |       |       |      |
//!   :      -------------------------------------------+       |       |      |
//!   :      vsw        sync width                              |       |      |
//!   :      ---------------------------------------------------+       |      |
//!   :      vbp        back porch                                      |      |
//!   :      -----------------------------------------------------------+      |
//!   V      vb         border                                                 |
//! ---------------------------------------------------------------------------+
//! \endcode
typedef struct _NV_TIMING
{
    // VESA scan out timing parameters:
    NvU16 HVisible;         //!< horizontal visible
    NvU16 HBorder;          //!< horizontal border
    NvU16 HFrontPorch;      //!< horizontal front porch
    NvU16 HSyncWidth;       //!< horizontal sync width
    NvU16 HTotal;           //!< horizontal total
    NvU8  HSyncPol;         //!< horizontal sync polarity: 1-negative, 0-positive

    NvU16 VVisible;         //!< vertical visible
    NvU16 VBorder;          //!< vertical border
    NvU16 VFrontPorch;      //!< vertical front porch
    NvU16 VSyncWidth;       //!< vertical sync width
    NvU16 VTotal;           //!< vertical total
    NvU8  VSyncPol;         //!< vertical sync polarity: 1-negative, 0-positive

    NvU16 interlaced;       //!< 1-interlaced, 0-progressive
    NvU32 pclk;             //!< pixel clock in 10 kHz

    //other timing related extras
    NV_TIMINGEXT etc;
}NV_TIMING;
#endif //NV_TIMING_STRUCTS_DEFINED


//! \addtogroup dispcontrol
//! Timing-related constants
//! @{
#define NV_TIMING_H_SYNC_POSITIVE                             0
#define NV_TIMING_H_SYNC_NEGATIVE                             1
#define NV_TIMING_H_SYNC_DEFAULT                              NV_TIMING_H_SYNC_NEGATIVE
//
#define NV_TIMING_V_SYNC_POSITIVE                             0
#define NV_TIMING_V_SYNC_NEGATIVE                             1
#define NV_TIMING_V_SYNC_DEFAULT                              NV_TIMING_V_SYNC_POSITIVE
//
#define NV_TIMING_PROGRESSIVE                                 0
#define NV_TIMING_INTERLACED                                  1
#define NV_TIMING_INTERLACED_EXTRA_VBLANK_ON_FIELD2           1
#define NV_TIMING_INTERLACED_NO_EXTRA_VBLANK_ON_FIELD2        2
//! @}

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_SetView
//
//! \fn NvAPI_SetView(NvDisplayHandle hNvDisplay, NV_VIEW_TARGET_INFO *pTargetInfo, NV_TARGET_VIEW_MODE targetView)
//!  This function lets the caller modify the target display arrangement of the selected source display handle in any nView mode.
//!  It can also modify or extend the source display in Dualview mode.
//!  \note Maps the selected source to the associated target Ids.
//!  \note Display PATH with this API is limited to single GPU. DUALVIEW across GPUs cannot be enabled with this API.
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_DISP_SetDisplayConfig.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 90
//!
//! \param [in]  hNvDisplay       NVIDIA Display selection. #NVAPI_DEFAULT_HANDLE is not allowed, it has to be a handle enumerated with NvAPI_EnumNVidiaDisplayHandle().
//! \param [in]  pTargetInfo      Pointer to array of NV_VIEW_TARGET_INFO, specifying device properties in this view.
//!                               The first device entry in the array is the physical primary.
//!                               The device entry with the lowest source id is the desktop primary.
//! \param [in]  targetCount      Count of target devices specified in pTargetInfo.
//! \param [in]  targetView       Target view selected from NV_TARGET_VIEW_MODE.
//!
//! \retval  NVAPI_OK               Completed request
//! \retval  NVAPI_ERROR            Miscellaneous error occurred
//! \retval  NVAPI_INVALID_ARGUMENT Invalid input parameter.
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup dispcontrol
//! Used in NvAPI_SetView() and NvAPI_GetView()
typedef struct
{
    NvU32 version;     //!< (IN) structure version
    NvU32 count;       //!< (IN) target count
    struct
    {
        NvU32 deviceMask;    //!< (IN/OUT) Device mask
        NvU32 sourceId;      //!< (IN/OUT) Source ID - values will be based on the number of heads exposed per GPU.
        NvU32 bPrimary:1;    //!< (OUT) Indicates if this is the GPU's primary view target. This is not the desktop GDI primary.
                             //!< NvAPI_SetView automatically selects the first target in NV_VIEW_TARGET_INFO index 0 as the GPU's primary view.
        NvU32 bInterlaced:1; //!< (IN/OUT) Indicates if the timing being used on this monitor is interlaced.
        NvU32 bGDIPrimary:1; //!< (IN/OUT) Indicates if this is the desktop GDI primary.
        NvU32 bForceModeSet:1;//!< (IN) Used only on Win7 and higher during a call to NvAPI_SetView(). Turns off optimization & forces OS to set supplied mode.
    } target[NVAPI_MAX_VIEW_TARGET];
} NV_VIEW_TARGET_INFO;

//! \ingroup dispcontrol
#define NV_VIEW_TARGET_INFO_VER  MAKE_NVAPI_VERSION(NV_VIEW_TARGET_INFO,2)


//! \ingroup dispcontrol
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_DISP_SetDisplayConfig.")
NVAPI_INTERFACE NvAPI_SetView(NvDisplayHandle hNvDisplay, NV_VIEW_TARGET_INFO *pTargetInfo, NV_TARGET_VIEW_MODE targetView);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_SetViewEx
//
//!  \fn NvAPI_SetViewEx(NvDisplayHandle hNvDisplay, NV_DISPLAY_PATH_INFO *pPathInfo, NV_TARGET_VIEW_MODE displayView)
//!  This function lets caller to modify the display arrangement for selected source display handle in any of the nview modes.
//!  It also allows to modify or extend the source display in dualview mode.
//!   \note Maps the selected source to the associated target Ids.
//!   \note Display PATH with this API is limited to single GPU. DUALVIEW across GPUs cannot be enabled with this API.
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_DISP_SetDisplayConfig.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 95
//!
//! \param [in]  hNvDisplay   NVIDIA Display selection. #NVAPI_DEFAULT_HANDLE is not allowed, it has to be a handle enumerated with
//!                           NvAPI_EnumNVidiaDisplayHandle().
//! \param [in]  pPathInfo    Pointer to array of NV_VIEW_PATH_INFO, specifying device properties in this view.
//!                           The first device entry in the array is the physical primary.
//!                           The device entry with the lowest source id is the desktop primary.
//! \param [in]  pathCount    Count of paths specified in pPathInfo.
//! \param [in]  displayView  Display view selected from NV_TARGET_VIEW_MODE.
//!
//! \retval  NVAPI_OK                Completed request
//! \retval  NVAPI_ERROR             Miscellaneous error occurred
//! \retval  NVAPI_INVALID_ARGUMENT  Invalid input parameter.
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup dispcontrol
#define NVAPI_MAX_DISPLAY_PATH  NVAPI_MAX_VIEW_TARGET

//! \ingroup dispcontrol
#define NVAPI_ADVANCED_MAX_DISPLAY_PATH  NVAPI_ADVANCED_MAX_VIEW_TARGET



//! \ingroup dispcontrol
//! Used in NV_DISPLAY_PATH_INFO.
typedef struct
{
    NvU32                   deviceMask;     //!< (IN) Device mask
    NvU32                   sourceId;       //!< (IN) Values will be based on the number of heads exposed per GPU(0, 1?)
    NvU32                   bPrimary:1;     //!< (IN/OUT) Indicates if this is the GPU's primary view target. This is not the desktop GDI primary.
                                            //!< NvAPI_SetViewEx() automatically selects the first target in NV_DISPLAY_PATH_INFO index 0 as the GPU's primary view.
    NV_GPU_CONNECTOR_TYPE   connector;      //!< (IN) Specify connector type. For TV only.

    // source mode information
    NvU32                   width;          //!< (IN) Width of the mode
    NvU32                   height;         //!< (IN) Height of the mode
    NvU32                   depth;          //!< (IN) Depth of the mode
    NV_FORMAT               colorFormat;    //!<      Color format if it needs to be specified. Not used now.

    //rotation setting of the mode
    NV_ROTATE               rotation;       //!< (IN) Rotation setting.

    // the scaling mode
    NV_SCALING              scaling;        //!< (IN) Scaling setting

    // Timing info
    NvU32                   refreshRate;    //!< (IN) Refresh rate of the mode
    NvU32                   interlaced:1;   //!< (IN) Interlaced mode flag

    NV_DISPLAY_TV_FORMAT    tvFormat;       //!< (IN) To choose the last TV format set this value to NV_DISPLAY_TV_FORMAT_NONE

    // Windows desktop position
    NvU32                   posx;           //!< (IN/OUT) X-offset of this display on the Windows desktop
    NvU32                   posy;           //!< (IN/OUT) Y-offset of this display on the Windows desktop
    NvU32                   bGDIPrimary:1;  //!< (IN/OUT) Indicates if this is the desktop GDI primary.

    NvU32                   bForceModeSet:1;//!< (IN) Used only on Win7 and higher during a call to NvAPI_SetViewEx(). Turns off optimization & forces OS to set supplied mode.
    NvU32                   bFocusDisplay:1;//!< (IN) If set, this display path should have the focus after the GPU topology change
    NvU32                   gpuId:24;       //!< (IN) the physical display/target Gpu id which is the owner of the scan out (for SLI multimon, display from the slave Gpu)

} NV_DISPLAY_PATH;

//! \ingroup dispcontrol
//! Used in NvAPI_SetViewEx() and NvAPI_GetViewEx().
typedef struct
{
    NvU32 version;     //!< (IN) Structure version
    NvU32 count;       //!< (IN) Path count
    NV_DISPLAY_PATH path[NVAPI_MAX_DISPLAY_PATH];
} NV_DISPLAY_PATH_INFO_V3;

//! \ingroup dispcontrol
//! Used in NvAPI_SetViewEx() and NvAPI_GetViewEx().
typedef struct
{
    NvU32 version;     //!< (IN) Structure version
    NvU32 count;       //!< (IN) Path count
    NV_DISPLAY_PATH path[NVAPI_ADVANCED_MAX_DISPLAY_PATH];
} NV_DISPLAY_PATH_INFO;

//! \addtogroup dispcontrol
//! Macro for constructing the version fields of NV_DISPLAY_PATH_INFO
//! @{
#define NV_DISPLAY_PATH_INFO_VER  NV_DISPLAY_PATH_INFO_VER4
#define NV_DISPLAY_PATH_INFO_VER4 MAKE_NVAPI_VERSION(NV_DISPLAY_PATH_INFO,4)
#define NV_DISPLAY_PATH_INFO_VER3 MAKE_NVAPI_VERSION(NV_DISPLAY_PATH_INFO,3)
#define NV_DISPLAY_PATH_INFO_VER2 MAKE_NVAPI_VERSION(NV_DISPLAY_PATH_INFO,2)
#define NV_DISPLAY_PATH_INFO_VER1 MAKE_NVAPI_VERSION(NV_DISPLAY_PATH_INFO,1)
//! @}
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_SetViewEx
//
//!  \fn NvAPI_SetViewEx(NvDisplayHandle hNvDisplay, NV_DISPLAY_PATH_INFO *pPathInfo, NV_TARGET_VIEW_MODE displayView)
//!  This function lets caller to modify the display arrangement for selected source display handle in any of the nview modes.
//!  It also allows to modify or extend the source display in dualview mode.
//!   \note Maps the selected source to the associated target Ids.
//!   \note Display PATH with this API is limited to single GPU. DUALVIEW across GPUs cannot be enabled with this API.
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_DISP_SetDisplayConfig.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 95
//!
//! \param [in]  hNvDisplay   NVIDIA Display selection. #NVAPI_DEFAULT_HANDLE is not allowed, it has to be a handle enumerated with
//!                           NvAPI_EnumNVidiaDisplayHandle().
//! \param [in]  pPathInfo    Pointer to array of NV_VIEW_PATH_INFO, specifying device properties in this view.
//!                           The first device entry in the array is the physical primary.
//!                           The device entry with the lowest source id is the desktop primary.
//! \param [in]  pathCount    Count of paths specified in pPathInfo.
//! \param [in]  displayView  Display view selected from NV_TARGET_VIEW_MODE.
//!
//! \retval  NVAPI_OK                Completed request
//! \retval  NVAPI_ERROR             Miscellaneous error occurred
//! \retval  NVAPI_INVALID_ARGUMENT  Invalid input parameter.
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup dispcontrol
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_DISP_SetDisplayConfig.")
NVAPI_INTERFACE NvAPI_SetViewEx(NvDisplayHandle hNvDisplay, NV_DISPLAY_PATH_INFO *pPathInfo, NV_TARGET_VIEW_MODE displayView);



///////////////////////////////////////////////////////////////////////////////
// SetDisplayConfig/GetDisplayConfig
///////////////////////////////////////////////////////////////////////////////
//! \ingroup dispcontrol

typedef struct _NV_POSITION
{
    NvS32   x;
    NvS32   y;
} NV_POSITION;

//! \ingroup dispcontrol
typedef struct _NV_RESOLUTION
{
    NvU32   width;
    NvU32   height;
    NvU32   colorDepth;
} NV_RESOLUTION;

//! \ingroup dispcontrol
typedef struct _NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO_V1
{
    NvU32                   version;

    // Rotation and Scaling
    NV_ROTATE               rotation;       //!< (IN) rotation setting.
    NV_SCALING              scaling;        //!< (IN) scaling setting.

    // Refresh Rate
    NvU32                   refreshRate1K;  //!< (IN) Non-interlaced Refresh Rate of the mode, multiplied by 1000, 0 = ignored
                                            //!< This is the value which driver reports to the OS.
    // Flags
    NvU32                   interlaced:1;   //!< (IN) Interlaced mode flag, ignored if refreshRate == 0
    NvU32                   primary:1;      //!< (IN) Declares primary display in clone configuration. This is *NOT* GDI Primary.
                                            //!< Only one target can be primary per source. If no primary is specified, the first
                                            //!< target will automatically be primary.
#ifdef NV_PAN_AND_SCAN_DEFINED
    NvU32                   isPanAndScanTarget:1; //!< Whether on this target Pan and Scan is enabled or has to be enabled. Valid only
                                                  //!< when the target is part of clone topology.
#else
    NvU32                   reservedBit1:1;
#endif
    NvU32                   disableVirtualModeSupport:1;
    NvU32                   isPreferredUnscaledTarget:1;
    NvU32                   reserved:27;
    // TV format information
    NV_GPU_CONNECTOR_TYPE   connector;      //!< Specify connector type. For TV only, ignored if tvFormat == NV_DISPLAY_TV_FORMAT_NONE
    NV_DISPLAY_TV_FORMAT    tvFormat;       //!< (IN) to choose the last TV format set this value to NV_DISPLAY_TV_FORMAT_NONE
                                            //!< In case of NvAPI_DISP_GetDisplayConfig(), this field will indicate the currently applied TV format;
                                            //!< if no TV format is applied, this field will have NV_DISPLAY_TV_FORMAT_NONE value.
                                            //!< In case of NvAPI_DISP_SetDisplayConfig(), this field should only be set in case of TVs;
                                            //!< for other displays this field will be ignored and resolution & refresh rate specified in input will be used to apply the TV format.

    // Backend (raster) timing standard
    NV_TIMING_OVERRIDE      timingOverride;     //!< Ignored if timingOverride == NV_TIMING_OVERRIDE_CURRENT
    NV_TIMING               timing;             //!< Scan out timing, valid only if timingOverride == NV_TIMING_OVERRIDE_CUST
                                                //!< The value NV_TIMING::NV_TIMINGEXT::rrx1k is obtained from the EDID. The driver may
                                                //!< tweak this value for HDTV, stereo, etc., before reporting it to the OS.
} NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO_V1;

//! \ingroup dispcontrol
typedef NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO_V1 NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO;

//! \ingroup dispcontrol
#define NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO_VER1     MAKE_NVAPI_VERSION(NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO_V1,1)

//! \ingroup dispcontrol
#define NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO_VER      NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO_VER1

//! \ingroup dispcontrol
typedef struct _NV_DISPLAYCONFIG_PATH_TARGET_INFO_V1
{
    NvU32                                           displayId;  //!< Display ID
    NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO*     details;    //!< May be NULL if no advanced settings are required. NULL for Non-NVIDIA Display.
} NV_DISPLAYCONFIG_PATH_TARGET_INFO_V1;

//! \ingroup dispcontrol
typedef struct _NV_DISPLAYCONFIG_PATH_TARGET_INFO_V2
{
    NvU32                                           displayId;  //!< Display ID
    NV_DISPLAYCONFIG_PATH_ADVANCED_TARGET_INFO*     details;    //!< May be NULL if no advanced settings are required
    NvU32                                           targetId;   //!< Windows CCD target ID. Must be present only for non-NVIDIA adapter, for NVIDIA adapter this parameter is ignored.
} NV_DISPLAYCONFIG_PATH_TARGET_INFO_V2;


//! \ingroup dispcontrol
//! As version is not defined for this structure, we will be using version of NV_DISPLAYCONFIG_PATH_INFO
typedef NV_DISPLAYCONFIG_PATH_TARGET_INFO_V2 NV_DISPLAYCONFIG_PATH_TARGET_INFO;


//! \ingroup dispcontrol
typedef enum _NV_DISPLAYCONFIG_SPANNING_ORIENTATION
{
    NV_DISPLAYCONFIG_SPAN_NONE          = 0,
    NV_DISPLAYCONFIG_SPAN_HORIZONTAL    = 1,
    NV_DISPLAYCONFIG_SPAN_VERTICAL      = 2,
} NV_DISPLAYCONFIG_SPANNING_ORIENTATION;

//! \ingroup dispcontrol
typedef struct _NV_DISPLAYCONFIG_SOURCE_MODE_INFO_V1
{
    NV_RESOLUTION                           resolution;
    NV_FORMAT                               colorFormat;                //!< Ignored at present, must be NV_FORMAT_UNKNOWN (0)
    NV_POSITION                             position;                   //!< Is all positions are 0 or invalid, displays will be automatically
                                                                        //!< positioned from left to right with GDI Primary at 0,0, and all
                                                                        //!< other displays in the order of the path array.
    NV_DISPLAYCONFIG_SPANNING_ORIENTATION   spanningOrientation;        //!< Spanning is only supported on XP
    NvU32                                   bGDIPrimary : 1;
    NvU32                                   bSLIFocus : 1;
    NvU32                                   reserved : 30;              //!< Must be 0
} NV_DISPLAYCONFIG_SOURCE_MODE_INFO_V1;



//! \ingroup dispcontrol
typedef struct _NV_DISPLAYCONFIG_PATH_INFO_V1
{
    NvU32                                   version;
    NvU32                                   reserved_sourceId;     	//!< This field is reserved. There is ongoing debate if we need this field.
                                                                        //!< Identifies sourceIds used by Windows. If all sourceIds are 0,
                                                                        //!< these will be computed automatically.
    NvU32                                   targetInfoCount;            //!< Number of elements in targetInfo array
    NV_DISPLAYCONFIG_PATH_TARGET_INFO_V1*   targetInfo;
    NV_DISPLAYCONFIG_SOURCE_MODE_INFO_V1*   sourceModeInfo;             //!< May be NULL if mode info is not important
} NV_DISPLAYCONFIG_PATH_INFO_V1;

//! \ingroup dispcontrol
//! This define is temporary and must be removed once DVS failure is fixed.
#define _NV_DISPLAYCONFIG_PATH_INFO_V2 _NV_DISPLAYCONFIG_PATH_INFO

//! \ingroup dispcontrol
typedef struct _NV_DISPLAYCONFIG_PATH_INFO_V2
{
    NvU32                                   version;
    union {
        NvU32                                   sourceId;            	//!< Identifies sourceId used by Windows CCD. This can be optionally set.
        NvU32                                   reserved_sourceId;      //!< Only for compatibility
    };

    NvU32                                   targetInfoCount;            //!< Number of elements in targetInfo array
    NV_DISPLAYCONFIG_PATH_TARGET_INFO_V2*   targetInfo;
    NV_DISPLAYCONFIG_SOURCE_MODE_INFO_V1*   sourceModeInfo;             //!< May be NULL if mode info is not important
    NvU32                                   IsNonNVIDIAAdapter : 1;     //!< True for non-NVIDIA adapter.
    NvU32                                   reserved : 31;              //!< Must be 0
    void                                    *pOSAdapterID;              //!< Used by Non-NVIDIA adapter for pointer to OS Adapter of LUID
                                                                        //!< type, type casted to void *.
} NV_DISPLAYCONFIG_PATH_INFO_V2;

//! \ingroup dispcontrol
#define NV_DISPLAYCONFIG_PATH_INFO_VER1                 MAKE_NVAPI_VERSION(NV_DISPLAYCONFIG_PATH_INFO_V1,1)

//! \ingroup dispcontrol
#define NV_DISPLAYCONFIG_PATH_INFO_VER2                 MAKE_NVAPI_VERSION(NV_DISPLAYCONFIG_PATH_INFO_V2,2)

#ifndef NV_DISPLAYCONFIG_PATH_INFO_VER

typedef NV_DISPLAYCONFIG_PATH_INFO_V2 NV_DISPLAYCONFIG_PATH_INFO;

#define NV_DISPLAYCONFIG_PATH_INFO_VER                  NV_DISPLAYCONFIG_PATH_INFO_VER2

typedef NV_DISPLAYCONFIG_SOURCE_MODE_INFO_V1 NV_DISPLAYCONFIG_SOURCE_MODE_INFO;

#endif


//! \ingroup dispcontrol
typedef enum _NV_DISPLAYCONFIG_FLAGS
{
    NV_DISPLAYCONFIG_VALIDATE_ONLY          = 0x00000001,
    NV_DISPLAYCONFIG_SAVE_TO_PERSISTENCE    = 0x00000002,
    NV_DISPLAYCONFIG_DRIVER_RELOAD_ALLOWED  = 0x00000004,               //!< Driver reload is permitted if necessary
    NV_DISPLAYCONFIG_FORCE_MODE_ENUMERATION = 0x00000008,               //!< Refresh OS mode list.
    NV_FORCE_COMMIT_VIDPN                   = 0x00000010,               //!< Tell OS to avoid optimizing CommitVidPn call during a modeset
} NV_DISPLAYCONFIG_FLAGS;


#define NVAPI_UNICODE_STRING_MAX                             2048
#define NVAPI_BINARY_DATA_MAX                                4096

typedef NvU16 NvAPI_UnicodeString[NVAPI_UNICODE_STRING_MAX];
typedef const NvU16 *NvAPI_LPCWSTR;

// Common



//! \ingroup gpuclock
//! @{
#define NVAPI_MAX_GPU_CLOCKS            32
#define NVAPI_MAX_GPU_PUBLIC_CLOCKS     32
#define NVAPI_MAX_GPU_PERF_CLOCKS       32
#define NVAPI_MAX_GPU_PERF_VOLTAGES     16
#define NVAPI_MAX_GPU_PERF_PSTATES      16
//! @}

//! \ingroup gpuclock
typedef enum _NV_GPU_PERF_VOLTAGE_INFO_DOMAIN_ID
{
    NVAPI_GPU_PERF_VOLTAGE_INFO_DOMAIN_CORE      = 0,
    NVAPI_GPU_PERF_VOLTAGE_INFO_DOMAIN_UNDEFINED = NVAPI_MAX_GPU_PERF_VOLTAGES,
} NV_GPU_PERF_VOLTAGE_INFO_DOMAIN_ID;

//! \ingroup gpuclock
typedef enum _NV_GPU_PUBLIC_CLOCK_ID
{
    NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS  = 0,
    NVAPI_GPU_PUBLIC_CLOCK_MEMORY    = 4,
    NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR = 7,
    NVAPI_GPU_PUBLIC_CLOCK_VIDEO     = 8,
    NVAPI_GPU_PUBLIC_CLOCK_UNDEFINED = NVAPI_MAX_GPU_PUBLIC_CLOCKS,
} NV_GPU_PUBLIC_CLOCK_ID;


//! \addtogroup gpupstate
//! @{

typedef enum _NV_GPU_PERF_PSTATE_ID
{
    NVAPI_GPU_PERF_PSTATE_P0 = 0,
    NVAPI_GPU_PERF_PSTATE_P1,
    NVAPI_GPU_PERF_PSTATE_P2,
    NVAPI_GPU_PERF_PSTATE_P3,
    NVAPI_GPU_PERF_PSTATE_P4,
    NVAPI_GPU_PERF_PSTATE_P5,
    NVAPI_GPU_PERF_PSTATE_P6,
    NVAPI_GPU_PERF_PSTATE_P7,
    NVAPI_GPU_PERF_PSTATE_P8,
    NVAPI_GPU_PERF_PSTATE_P9,
    NVAPI_GPU_PERF_PSTATE_P10,
    NVAPI_GPU_PERF_PSTATE_P11,
    NVAPI_GPU_PERF_PSTATE_P12,
    NVAPI_GPU_PERF_PSTATE_P13,
    NVAPI_GPU_PERF_PSTATE_P14,
    NVAPI_GPU_PERF_PSTATE_P15,
    NVAPI_GPU_PERF_PSTATE_UNDEFINED = NVAPI_MAX_GPU_PERF_PSTATES,
    NVAPI_GPU_PERF_PSTATE_ALL,

} NV_GPU_PERF_PSTATE_ID;

//! @}



//! \addtogroup gpupstate
//! @{

#define NVAPI_MAX_GPU_PSTATE20_PSTATES          16
#define NVAPI_MAX_GPU_PSTATE20_CLOCKS           8
#define NVAPI_MAX_GPU_PSTATE20_BASE_VOLTAGES    4

//! Used to identify clock type
typedef enum
{
    //! Clock domains that use single frequency value within given pstate
    NVAPI_GPU_PERF_PSTATE20_CLOCK_TYPE_SINGLE = 0,

    //! Clock domains that allow range of frequency values within given pstate
    NVAPI_GPU_PERF_PSTATE20_CLOCK_TYPE_RANGE,
} NV_GPU_PERF_PSTATE20_CLOCK_TYPE_ID;

//! Used to describe both voltage and frequency deltas
typedef struct
{
    //! Value of parameter delta (in respective units [kHz, uV])
    NvS32       value;

    struct
    {
        //! Min value allowed for parameter delta (in respective units [kHz, uV])
        NvS32   min;

        //! Max value allowed for parameter delta (in respective units [kHz, uV])
        NvS32   max;
    } valueRange;
} NV_GPU_PERF_PSTATES20_PARAM_DELTA;

//! Used to describe single clock entry
typedef struct
{
    //! ID of the clock domain
    NV_GPU_PUBLIC_CLOCK_ID                      domainId;

    //! Clock type ID
    NV_GPU_PERF_PSTATE20_CLOCK_TYPE_ID          typeId;
    NvU32                                       bIsEditable:1;

    //! These bits are reserved for future use (must be always 0)
    NvU32                                       reserved:31;

    //! Current frequency delta from nominal settings in (kHz)
    NV_GPU_PERF_PSTATES20_PARAM_DELTA           freqDelta_kHz;

    //! Clock domain type dependant information
    union
    {
        struct
        {
            //! Clock frequency within given pstate in (kHz)
            NvU32                               freq_kHz;
        } single;

        struct
        {
            //! Min clock frequency within given pstate in (kHz)
            NvU32                               minFreq_kHz;

            //! Max clock frequency within given pstate in (kHz)
            NvU32                               maxFreq_kHz;

            //! Voltage domain ID and value range in (uV) required for this clock
            NV_GPU_PERF_VOLTAGE_INFO_DOMAIN_ID  domainId;
            NvU32                               minVoltage_uV;
            NvU32                               maxVoltage_uV;
        } range;
    } data;
} NV_GPU_PSTATE20_CLOCK_ENTRY_V1;

//! Used to describe single base voltage entry
typedef struct
{
    //! ID of the voltage domain
    NV_GPU_PERF_VOLTAGE_INFO_DOMAIN_ID  domainId;
    NvU32                               bIsEditable:1;

    //! These bits are reserved for future use (must be always 0)
    NvU32                               reserved:31;

    //! Current base voltage settings in [uV]
    NvU32                               volt_uV;

    NV_GPU_PERF_PSTATES20_PARAM_DELTA   voltDelta_uV; // Current base voltage delta from nominal settings in [uV]
} NV_GPU_PSTATE20_BASE_VOLTAGE_ENTRY_V1;

//! Used in NvAPI_GPU_GetPstates20() interface call.

typedef struct
{
    //! Version info of the structure (NV_GPU_PERF_PSTATES20_INFO_VER<n>)
    NvU32   version;

    NvU32   bIsEditable:1;

    //! These bits are reserved for future use (must be always 0)
    NvU32   reserved:31;

    //! Number of populated pstates
    NvU32   numPstates;

    //! Number of populated clocks (per pstate)
    NvU32   numClocks;

    //! Number of populated base voltages (per pstate)
    NvU32   numBaseVoltages;

    //! Performance state (P-State) settings
    //! Valid index range is 0 to numPstates-1
    struct
    {
    //! ID of the P-State
        NV_GPU_PERF_PSTATE_ID                   pstateId;

        NvU32                                   bIsEditable:1;

        //! These bits are reserved for future use (must be always 0)
        NvU32                                   reserved:31;

        //! Array of clock entries
        //! Valid index range is 0 to numClocks-1
        NV_GPU_PSTATE20_CLOCK_ENTRY_V1          clocks[NVAPI_MAX_GPU_PSTATE20_CLOCKS];

        //! Array of baseVoltage entries
        //! Valid index range is 0 to numBaseVoltages-1
        NV_GPU_PSTATE20_BASE_VOLTAGE_ENTRY_V1   baseVoltages[NVAPI_MAX_GPU_PSTATE20_BASE_VOLTAGES];
    } pstates[NVAPI_MAX_GPU_PSTATE20_PSTATES];
} NV_GPU_PERF_PSTATES20_INFO_V1;

//! Used in NvAPI_GPU_GetPstates20() interface call.

typedef struct _NV_GPU_PERF_PSTATES20_INFO_V2
{
    //! Version info of the structure (NV_GPU_PERF_PSTATES20_INFO_VER<n>)
    NvU32   version;

    NvU32   bIsEditable:1;

    //! These bits are reserved for future use (must be always 0)
    NvU32   reserved:31;

    //! Number of populated pstates
    NvU32   numPstates;

    //! Number of populated clocks (per pstate)
    NvU32   numClocks;

    //! Number of populated base voltages (per pstate)
    NvU32   numBaseVoltages;

    //! Performance state (P-State) settings
    //! Valid index range is 0 to numPstates-1
    struct
    {
    //! ID of the P-State
        NV_GPU_PERF_PSTATE_ID                   pstateId;

        NvU32                                   bIsEditable:1;

        //! These bits are reserved for future use (must be always 0)
        NvU32                                   reserved:31;

        //! Array of clock entries
        //! Valid index range is 0 to numClocks-1
        NV_GPU_PSTATE20_CLOCK_ENTRY_V1          clocks[NVAPI_MAX_GPU_PSTATE20_CLOCKS];

        //! Array of baseVoltage entries
        //! Valid index range is 0 to numBaseVoltages-1
        NV_GPU_PSTATE20_BASE_VOLTAGE_ENTRY_V1   baseVoltages[NVAPI_MAX_GPU_PSTATE20_BASE_VOLTAGES];
    } pstates[NVAPI_MAX_GPU_PSTATE20_PSTATES];

    //! OV settings - Please refer to NVIDIA over-volting recommendation to understand impact of this functionality
    //! Valid index range is 0 to numVoltages-1
    struct
    {
        //! Number of populated voltages
        NvU32                                 numVoltages;

        //! Array of voltage entries
        //! Valid index range is 0 to numVoltages-1
        NV_GPU_PSTATE20_BASE_VOLTAGE_ENTRY_V1 voltages[NVAPI_MAX_GPU_PSTATE20_BASE_VOLTAGES];
    } ov;
} NV_GPU_PERF_PSTATES20_INFO_V2;

typedef NV_GPU_PERF_PSTATES20_INFO_V2   NV_GPU_PERF_PSTATES20_INFO;

//! Macro for constructing the version field of NV_GPU_PERF_PSTATES20_INFO_V1
#define NV_GPU_PERF_PSTATES20_INFO_VER1 MAKE_NVAPI_VERSION(NV_GPU_PERF_PSTATES20_INFO_V1,1)

//! Macro for constructing the version field of NV_GPU_PERF_PSTATES20_INFO_V2
#define NV_GPU_PERF_PSTATES20_INFO_VER2 MAKE_NVAPI_VERSION(NV_GPU_PERF_PSTATES20_INFO_V2,2)

//! Macro for constructing the version field of NV_GPU_PERF_PSTATES20_INFO_V2
#define NV_GPU_PERF_PSTATES20_INFO_VER3 MAKE_NVAPI_VERSION(NV_GPU_PERF_PSTATES20_INFO_V2,3)

//! Macro for constructing the version field of NV_GPU_PERF_PSTATES20_INFO
#define NV_GPU_PERF_PSTATES20_INFO_VER  NV_GPU_PERF_PSTATES20_INFO_VER3

//! @}

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetDisplayDriverVersion
//! \fn NvAPI_GetDisplayDriverVersion(NvDisplayHandle hNvDisplay, NV_DISPLAY_DRIVER_VERSION *pVersion)
//! This function returns a struct that describes aspects of the display driver
//!                build.
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_SYS_GetDriverAndBranchVersion.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \param [in]  hNvDisplay NVIDIA display handle.
//! \param [out] pVersion Pointer to NV_DISPLAY_DRIVER_VERSION struc
//!
//! \retval NVAPI_ERROR
//! \retval NVAPI_OK
///////////////////////////////////////////////////////////////////////////////

//! \ingroup driverapi
//! Used in NvAPI_GetDisplayDriverVersion()
typedef struct 
{
    NvU32              version;             // Structure version
    NvU32              drvVersion;           
    NvU32              bldChangeListNum;     
    NvAPI_ShortString  szBuildBranchString; 
    NvAPI_ShortString  szAdapterString;
} NV_DISPLAY_DRIVER_VERSION;

//! \ingroup driverapi
#define NV_DISPLAY_DRIVER_VERSION_VER  MAKE_NVAPI_VERSION(NV_DISPLAY_DRIVER_VERSION,1)


//! \ingroup driverapi
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_SYS_GetDriverAndBranchVersion.")
NVAPI_INTERFACE NvAPI_GetDisplayDriverVersion(NvDisplayHandle hNvDisplay, NV_DISPLAY_DRIVER_VERSION *pVersion);




///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_OGL_ExpertModeSet[Get]
//
//! \name NvAPI_OGL_ExpertModeSet[Get] Functions
//@{
//!  This function configures OpenGL Expert Mode, an API usage feedback and
//!  advice reporting mechanism. The effects of this call are
//!  applied only to the current context, and are reset to the
//!  defaults when the context is destroyed.
//!
//!  \note  This feature is valid at runtime only when GLExpert
//!         functionality has been built into the OpenGL driver
//!         installed on the system. All Windows Vista OpenGL
//!         drivers provided by NVIDIA have this instrumentation
//!         included by default. Windows XP, however, requires a
//!         special display driver available with the NVIDIA
//!         PerfSDK found at developer.nvidia.com.
//!
//!  \note These functions are valid only for the current OpenGL
//!        context. Calling these functions prior to creating a
//!        context and calling MakeCurrent with it will result
//!        in errors and undefined behavior.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \param   expertDetailMask  Mask made up of NVAPI_OGLEXPERT_DETAIL bits,
//!                            this parameter specifies the detail level in
//!                            the feedback stream.
//!
//! \param   expertReportMask  Mask made up of NVAPI_OGLEXPERT_REPORT bits,
//!                            this parameter specifies the areas of
//!                            functional interest.
//!
//! \param   expertOutputMask  Mask made up of NVAPI_OGLEXPERT_OUTPUT bits,
//!                            this parameter specifies the feedback output
//!                            location.
//!
//! \param   expertCallback    Used in conjunction with OUTPUT_TO_CALLBACK,
//!                            this is a simple callback function the user
//!                            may use to obtain the feedback stream. The
//!                            function will be called once per fully
//!                            qualified feedback stream extry.
//!
//! \retval  NVAPI_API_NOT_INTIALIZED          NVAPI not initialized
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND     No NVIDIA GPU found
//! \retval  NVAPI_OPENGL_CONTEXT_NOT_CURRENT  No NVIDIA OpenGL context
//!                                            which supports GLExpert
//!                                            has been made current
//! \retval  NVAPI_ERROR                       OpenGL driver failed to load properly
//! \retval  NVAPI_OK                          Success
//
///////////////////////////////////////////////////////////////////////////////

//! \addtogroup oglapi
//! @{
#define NVAPI_OGLEXPERT_DETAIL_NONE                 0x00000000
#define NVAPI_OGLEXPERT_DETAIL_ERROR                0x00000001
#define NVAPI_OGLEXPERT_DETAIL_SWFALLBACK           0x00000002
#define NVAPI_OGLEXPERT_DETAIL_BASIC_INFO           0x00000004
#define NVAPI_OGLEXPERT_DETAIL_DETAILED_INFO        0x00000008
#define NVAPI_OGLEXPERT_DETAIL_PERFORMANCE_WARNING  0x00000010
#define NVAPI_OGLEXPERT_DETAIL_QUALITY_WARNING      0x00000020
#define NVAPI_OGLEXPERT_DETAIL_USAGE_WARNING        0x00000040
#define NVAPI_OGLEXPERT_DETAIL_ALL                  0xFFFFFFFF

#define NVAPI_OGLEXPERT_REPORT_NONE                 0x00000000
#define NVAPI_OGLEXPERT_REPORT_ERROR                0x00000001
#define NVAPI_OGLEXPERT_REPORT_SWFALLBACK           0x00000002
#define NVAPI_OGLEXPERT_REPORT_PIPELINE_VERTEX      0x00000004
#define NVAPI_OGLEXPERT_REPORT_PIPELINE_GEOMETRY    0x00000008
#define NVAPI_OGLEXPERT_REPORT_PIPELINE_XFB         0x00000010
#define NVAPI_OGLEXPERT_REPORT_PIPELINE_RASTER      0x00000020
#define NVAPI_OGLEXPERT_REPORT_PIPELINE_FRAGMENT    0x00000040
#define NVAPI_OGLEXPERT_REPORT_PIPELINE_ROP         0x00000080
#define NVAPI_OGLEXPERT_REPORT_PIPELINE_FRAMEBUFFER 0x00000100
#define NVAPI_OGLEXPERT_REPORT_PIPELINE_PIXEL       0x00000200
#define NVAPI_OGLEXPERT_REPORT_PIPELINE_TEXTURE     0x00000400
#define NVAPI_OGLEXPERT_REPORT_OBJECT_BUFFEROBJECT  0x00000800
#define NVAPI_OGLEXPERT_REPORT_OBJECT_TEXTURE       0x00001000
#define NVAPI_OGLEXPERT_REPORT_OBJECT_PROGRAM       0x00002000
#define NVAPI_OGLEXPERT_REPORT_OBJECT_FBO           0x00004000
#define NVAPI_OGLEXPERT_REPORT_FEATURE_SLI          0x00008000
#define NVAPI_OGLEXPERT_REPORT_ALL                  0xFFFFFFFF


#define NVAPI_OGLEXPERT_OUTPUT_TO_NONE       0x00000000
#define NVAPI_OGLEXPERT_OUTPUT_TO_CONSOLE    0x00000001
#define NVAPI_OGLEXPERT_OUTPUT_TO_DEBUGGER   0x00000004
#define NVAPI_OGLEXPERT_OUTPUT_TO_CALLBACK   0x00000008
#define NVAPI_OGLEXPERT_OUTPUT_TO_ALL        0xFFFFFFFF

//! @}

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION TYPE: NVAPI_OGLEXPERT_CALLBACK
//
//!   DESCRIPTION: Used in conjunction with OUTPUT_TO_CALLBACK, this is a simple 
//!                callback function the user may use to obtain the feedback 
//!                stream. The function will be called once per fully qualified 
//!                feedback stream entry.
//!
//!   \param   categoryId    Contains the bit from the NVAPI_OGLEXPERT_REPORT 
//!                          mask that corresponds to the current message
//!   \param   messageId     Unique ID for the current message
//!   \param   detailLevel   Contains the bit from the NVAPI_OGLEXPERT_DETAIL
//!                          mask that corresponds to the current message
//!   \param   objectId      Unique ID of the object that corresponds to the
//!                          current message
//!   \param   messageStr    Text string from the current message
//!
//!   \ingroup oglapi
///////////////////////////////////////////////////////////////////////////////
typedef void (* NVAPI_OGLEXPERT_CALLBACK) (unsigned int categoryId, unsigned int messageId, unsigned int detailLevel, int objectId, const char *messageStr);



//! \ingroup oglapi
//! SUPPORTED OS:  Windows 7 and higher
//!
NVAPI_INTERFACE NvAPI_OGL_ExpertModeSet(NvU32 expertDetailLevel,
                                        NvU32 expertReportMask,
                                        NvU32 expertOutputMask,
                     NVAPI_OGLEXPERT_CALLBACK expertCallback);

//! \addtogroup oglapi
//! SUPPORTED OS:  Windows 7 and higher
//!
NVAPI_INTERFACE NvAPI_OGL_ExpertModeGet(NvU32 *pExpertDetailLevel,
                                        NvU32 *pExpertReportMask,
                                        NvU32 *pExpertOutputMask,
                     NVAPI_OGLEXPERT_CALLBACK *pExpertCallback);

//@}
///////////////////////////////////////////////////////////////////////////////
//
//! \name NvAPI_OGL_ExpertModeDefaultsSet[Get] Functions
//!
//@{
//!  This function configures OpenGL Expert Mode global defaults. These settings
//!  apply to any OpenGL application which starts up after these
//!  values are applied (i.e. these settings *do not* apply to
//!  currently running applications).
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \param   expertDetailLevel Value which specifies the detail level in
//!                            the feedback stream. This is a mask made up
//!                            of NVAPI_OGLEXPERT_LEVEL bits.
//!
//! \param   expertReportMask  Mask made up of NVAPI_OGLEXPERT_REPORT bits,
//!                            this parameter specifies the areas of
//!                            functional interest.
//!
//! \param   expertOutputMask  Mask made up of NVAPI_OGLEXPERT_OUTPUT bits,
//!                            this parameter specifies the feedback output
//!                            location. Note that using OUTPUT_TO_CALLBACK
//!                            here is meaningless and has no effect, but
//!                            using it will not cause an error.
//!
//! \return  ::NVAPI_ERROR or ::NVAPI_OK
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup oglapi
//! SUPPORTED OS:  Windows 7 and higher
//!
NVAPI_INTERFACE NvAPI_OGL_ExpertModeDefaultsSet(NvU32 expertDetailLevel,
                                                NvU32 expertReportMask,
                                                NvU32 expertOutputMask);

//! \addtogroup oglapi
//! SUPPORTED OS:  Windows 7 and higher
//!
NVAPI_INTERFACE NvAPI_OGL_ExpertModeDefaultsGet(NvU32 *pExpertDetailLevel,
                                                NvU32 *pExpertReportMask,
                                                NvU32 *pExpertOutputMask);
//@}




///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_EnumTCCPhysicalGPUs
//
//! This function returns an array of physical GPU handles that are in TCC Mode.
//! Each handle represents a physical GPU present in the system in TCC Mode.
//! That GPU may not be visible to the OS directly.
//!
//! The array nvGPUHandle will be filled with physical GPU handle values. The returned
//! gpuCount determines how many entries in the array are valid.
//!
//! NOTE: Handles enumerated by this API are only valid for NvAPIs that are tagged as TCC_SUPPORTED
//!       If handle is passed to any other API, it will fail with NVAPI_INVALID_HANDLE
//!
//!       For WDDM GPU handles please use NvAPI_EnumPhysicalGPUs()
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//!
//! \param [out]   nvGPUHandle      Physical GPU array that will contain all TCC Physical GPUs
//! \param [out]   pGpuCount        count represent the number of valid entries in nvGPUHandle
//!
//!
//! \retval NVAPI_INVALID_ARGUMENT         nvGPUHandle or pGpuCount is NULL
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_EnumTCCPhysicalGPUs( NvPhysicalGpuHandle nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS], NvU32 *pGpuCount);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_EnumLogicalGPUs
//
//! This function returns an array of logical GPU handles.
//!
//! Each handle represents one or more GPUs acting in concert as a single graphics device.
//!
//! At least one GPU must be present in the system and running an NVIDIA display driver.
//!
//! The array nvGPUHandle will be filled with logical GPU handle values.  The returned
//! gpuCount determines how many entries in the array are valid.
//!
//! \note All logical GPUs handles get invalidated on a GPU topology change, so the calling
//!       application is required to renum the logical GPU handles to get latest physical handle
//!       mapping after every GPU topology change activated by a call to NvAPI_SetGpuTopologies().
//!
//! To detect if SLI rendering is enabled, use NvAPI_D3D_GetCurrentSLIState().
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \retval NVAPI_INVALID_ARGUMENT         nvGPUHandle or pGpuCount is NULL
//! \retval NVAPI_OK                       One or more handles were returned
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND  No NVIDIA GPU driving a display was found
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_EnumLogicalGPUs(NvLogicalGpuHandle nvGPUHandle[NVAPI_MAX_LOGICAL_GPUS], NvU32 *pGpuCount);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetPhysicalGPUsFromDisplay
//
//! This function returns an array of physical GPU handles associated with the specified display.
//!
//! At least one GPU must be present in the system and running an NVIDIA display driver.
//!
//! The array nvGPUHandle will be filled with physical GPU handle values.  The returned
//! gpuCount determines how many entries in the array are valid.
//!
//! If the display corresponds to more than one physical GPU, the first GPU returned
//! is the one with the attached active output.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \retval NVAPI_INVALID_ARGUMENT         hNvDisp is not valid; nvGPUHandle or pGpuCount is NULL
//! \retval NVAPI_OK                       One or more handles were returned
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND  no NVIDIA GPU driving a display was found
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetPhysicalGPUsFromDisplay(NvDisplayHandle hNvDisp, NvPhysicalGpuHandle nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS], NvU32 *pGpuCount);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetPhysicalGPUFromUnAttachedDisplay
//
//! This function returns a physical GPU handle associated with the specified unattached display.
//! The source GPU is a physical render GPU which renders the frame buffer but may or may not drive the scan out.
//!
//! At least one GPU must be present in the system and running an NVIDIA display driver.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \retval NVAPI_INVALID_ARGUMENT         hNvUnAttachedDisp is not valid or pPhysicalGpu is NULL.
//! \retval NVAPI_OK                       One or more handles were returned
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND  No NVIDIA GPU driving a display was found
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetPhysicalGPUFromUnAttachedDisplay(NvUnAttachedDisplayHandle hNvUnAttachedDisp, NvPhysicalGpuHandle *pPhysicalGpu);



///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetLogicalGPUFromDisplay
//
//! This function returns the logical GPU handle associated with the specified display.
//! At least one GPU must be present in the system and running an NVIDIA display driver.
//! hNvDisp can be NVAPI_DEFAULT_HANDLE or a handle enumerated from NvAPI_EnumNVidiaDisplayHandle().
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \retval NVAPI_INVALID_ARGUMENT         hNvDisp is not valid; pLogicalGPU is NULL
//! \retval NVAPI_OK                       One or more handles were returned
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND  No NVIDIA GPU driving a display was found
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetLogicalGPUFromDisplay(NvDisplayHandle hNvDisp, NvLogicalGpuHandle *pLogicalGPU);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetLogicalGPUFromPhysicalGPU
//
//! This function returns the logical GPU handle associated with specified physical GPU handle.
//! At least one GPU must be present in the system and running an NVIDIA display driver.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \retval NVAPI_INVALID_ARGUMENT         hPhysicalGPU is not valid; pLogicalGPU is NULL
//! \retval NVAPI_OK                       One or more handles were returned
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND  No NVIDIA GPU driving a display was found
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetLogicalGPUFromPhysicalGPU(NvPhysicalGpuHandle hPhysicalGPU, NvLogicalGpuHandle *pLogicalGPU);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetPhysicalGPUsFromLogicalGPU
//
//!  This function returns the physical GPU handles associated with the specified logical GPU handle.
//!  At least one GPU must be present in the system and running an NVIDIA display driver.
//!
//!  The array hPhysicalGPU will be filled with physical GPU handle values.  The returned
//!  gpuCount determines how many entries in the array are valid.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \retval NVAPI_INVALID_ARGUMENT             hLogicalGPU is not valid; hPhysicalGPU is NULL
//! \retval NVAPI_OK                           One or more handles were returned
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND      No NVIDIA GPU driving a display was found
//! \retval NVAPI_EXPECTED_LOGICAL_GPU_HANDLE  hLogicalGPU was not a logical GPU handle
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetPhysicalGPUsFromLogicalGPU(NvLogicalGpuHandle hLogicalGPU,NvPhysicalGpuHandle hPhysicalGPU[NVAPI_MAX_PHYSICAL_GPUS], NvU32 *pGpuCount);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetShaderSubPipeCount
//
//!   DESCRIPTION: This function retrieves the number of Shader SubPipes on the GPU
//!                On newer architectures, this corresponds to the number of SM units
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 170
//!
//! RETURN STATUS: NVAPI_INVALID_ARGUMENT: pCount is NULL
//!                NVAPI_OK: *pCount is set
//!                NVAPI_NVIDIA_DEVICE_NOT_FOUND: no NVIDIA GPU driving a display was found
//!                NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE: hPhysicalGpu was not a physical GPU handle
//!
//! \ingroup   gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetShaderSubPipeCount(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pCount);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetGpuCoreCount
//
//!   DESCRIPTION: Retrieves the total number of cores defined for a GPU.
//!                Returns 0 on architectures that don't define GPU cores.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \retval ::NVAPI_INVALID_ARGUMENT              pCount is NULL
//! \retval ::NVAPI_OK                            *pCount is set
//! \retval ::NVAPI_NVIDIA_DEVICE_NOT_FOUND       no NVIDIA GPU driving a display was found
//! \retval ::NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle
//! \retval ::NVAPI_NOT_SUPPORTED                 API call is not supported on current architecture
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetGpuCoreCount(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pCount);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetAllOutputs
//
//!  This function returns set of all GPU-output identifiers as a bitmask.
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_GPU_GetAllDisplayIds.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 85
//!
//! \retval   NVAPI_INVALID_ARGUMENT              hPhysicalGpu or pOutputsMask is NULL.
//! \retval   NVAPI_OK                           *pOutputsMask contains a set of GPU-output identifiers.
//! \retval   NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//! \retval   NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle.
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_GPU_GetAllDisplayIds.")
NVAPI_INTERFACE NvAPI_GPU_GetAllOutputs(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pOutputsMask);



///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetConnectedOutputs
//
//! This function is the same as NvAPI_GPU_GetAllOutputs() but returns only the set of GPU output
//! identifiers that are connected to display devices.
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_GPU_GetConnectedDisplayIds.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \retval   NVAPI_INVALID_ARGUMENT              hPhysicalGpu or pOutputsMask is NULL.
//! \retval   NVAPI_OK                           *pOutputsMask contains a set of GPU-output identifiers.
//! \retval   NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//! \retval   NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle.
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_GPU_GetConnectedDisplayIds.")
NVAPI_INTERFACE NvAPI_GPU_GetConnectedOutputs(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pOutputsMask);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetConnectedSLIOutputs
//
//!   DESCRIPTION: This function is the same as NvAPI_GPU_GetConnectedOutputs() but returns only the set of GPU-output
//!                identifiers that can be selected in an SLI configuration.
//!                 NOTE: This function matches NvAPI_GPU_GetConnectedOutputs()
//!                 - On systems which are not SLI capable.
//!                 - If the queried GPU is not part of a valid SLI group.
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_GPU_GetConnectedDisplayIds.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 170
//!
//! \retval   NVAPI_INVALID_ARGUMENT              hPhysicalGpu or pOutputsMask is NULL
//! \retval   NVAPI_OK                           *pOutputsMask contains a set of GPU-output identifiers
//! \retval   NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found
//! \retval   NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE: hPhysicalGpu was not a physical GPU handle
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_GPU_GetConnectedDisplayIds.")
NVAPI_INTERFACE NvAPI_GPU_GetConnectedSLIOutputs(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pOutputsMask);




//! \ingroup gpu
typedef enum
{
    NV_MONITOR_CONN_TYPE_UNINITIALIZED = 0,
    NV_MONITOR_CONN_TYPE_VGA,
    NV_MONITOR_CONN_TYPE_COMPONENT,
    NV_MONITOR_CONN_TYPE_SVIDEO,
    NV_MONITOR_CONN_TYPE_HDMI,
    NV_MONITOR_CONN_TYPE_DVI,
    NV_MONITOR_CONN_TYPE_LVDS,
    NV_MONITOR_CONN_TYPE_DP,
    NV_MONITOR_CONN_TYPE_COMPOSITE,
    NV_MONITOR_CONN_TYPE_UNKNOWN =  -1
} NV_MONITOR_CONN_TYPE;


//! \addtogroup gpu
//! @{
#define NV_GPU_CONNECTED_IDS_FLAG_UNCACHED          NV_BIT(0) //!< Get uncached connected devices
#define NV_GPU_CONNECTED_IDS_FLAG_SLI               NV_BIT(1) //!< Get devices such that those can be selected in an SLI configuration
#define NV_GPU_CONNECTED_IDS_FLAG_LIDSTATE          NV_BIT(2) //!< Get devices such that to reflect the Lid State
#define NV_GPU_CONNECTED_IDS_FLAG_FAKE              NV_BIT(3) //!< Get devices that includes the fake connected monitors
#define NV_GPU_CONNECTED_IDS_FLAG_EXCLUDE_MST       NV_BIT(4) //!< Excludes devices that are part of the multi stream topology.

//! @}

//! \ingroup gpu
typedef struct _NV_GPU_DISPLAYIDS
{
    NvU32    version;
    NV_MONITOR_CONN_TYPE connectorType;     //!< out: vga, tv, dvi, hdmi and dp. This is reserved for future use and clients should not rely on this information. Instead get the
                                            //!< GPU connector type from NvAPI_GPU_GetConnectorInfo/NvAPI_GPU_GetConnectorInfoEx
    NvU32    displayId;                     //!< this is a unique identifier for each device

    NvU32    isDynamic              : 1;    //!< if bit is set then this display is part of MST topology and it's a dynamic
    NvU32    isMultiStreamRootNode  : 1;    //!< if bit is set then this displayID belongs to a multi stream enabled connector(root node). Note that when multi stream is enabled and
                                            //!< a single multi stream capable monitor is connected to it, the monitor will share the display id with the RootNode.
                                            //!< When there is more than one monitor connected in a multi stream topology, then the root node will have a separate displayId.
    NvU32    isActive               : 1;    //!< if bit is set then this display is being actively driven
    NvU32    isCluster              : 1;    //!< if bit is set then this display is the representative display
    NvU32    isOSVisible            : 1;    //!< if bit is set, then this display is reported to the OS
    NvU32    isWFD                  : 1;    //!< if bit is set, then this display is wireless
    NvU32    isConnected            : 1;    //!< if bit is set, then this display is connected

    NvU32    reservedInternal       :10;    //!< Do not use
    NvU32    isPhysicallyConnected  : 1;    //!< if bit is set, then this display is a phycially connected display; Valid only when isConnected bit is set
    NvU32    reserved               : 14;   //!< must be zero
} NV_GPU_DISPLAYIDS;

//! \ingroup gpu
//! Macro for constructing the version field of ::_NV_GPU_DISPLAYIDS
#define NV_GPU_DISPLAYIDS_VER1          MAKE_NVAPI_VERSION(NV_GPU_DISPLAYIDS,1)
#define NV_GPU_DISPLAYIDS_VER2          MAKE_NVAPI_VERSION(NV_GPU_DISPLAYIDS,3)

#define NV_GPU_DISPLAYIDS_VER NV_GPU_DISPLAYIDS_VER2

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetConnectedDisplayIds
//
//! \code
//!   DESCRIPTION: Due to space limitation NvAPI_GPU_GetConnectedOutputs can return maximum 32 devices, but
//!                this is no longer true for DPMST. NvAPI_GPU_GetConnectedDisplayIds will return all
//!                the connected display devices in the form of displayIds for the associated hPhysicalGpu.
//!                This function can accept set of flags to request cached, uncached, sli and lid to get the connected devices.
//!                Default value for flags will be cached .
//! HOW TO USE: 1) for each PhysicalGpu, make a call to get the number of connected displayId's
//!                using NvAPI_GPU_GetConnectedDisplayIds by passing the pDisplayIds as NULL
//!                On call success:
//!             2) If pDisplayIdCount is greater than 0, allocate memory based on pDisplayIdCount. Then make a call NvAPI_GPU_GetConnectedDisplayIds to populate DisplayIds.
//!                However, if pDisplayIdCount is 0, do not make this call.
//! SUPPORTED OS:  Windows 7 and higher
//!
//! PARAMETERS:     hPhysicalGpu (IN)  - GPU selection
//!                 flags        (IN)  - One or more defines from NV_GPU_CONNECTED_IDS_FLAG_* as valid flags.
//!                 pDisplayIds  (IN/OUT) - Pointer to an NV_GPU_DISPLAYIDS struct, each entry represents a one displayID and its attributes
//!                 pDisplayIdCount(OUT)- Number of displayId's.
//!
//! RETURN STATUS: NVAPI_INVALID_ARGUMENT: hPhysicalGpu or pDisplayIds or pDisplayIdCount is NULL
//!                NVAPI_OK: *pDisplayIds contains a set of GPU-output identifiers
//!                NVAPI_NVIDIA_DEVICE_NOT_FOUND: no NVIDIA GPU driving a display was found
//!                NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE: hPhysicalGpu was not a physical GPU handle
//! \endcode
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetConnectedDisplayIds(__in NvPhysicalGpuHandle hPhysicalGpu,  __inout_ecount_part_opt(*pDisplayIdCount, *pDisplayIdCount) NV_GPU_DISPLAYIDS* pDisplayIds, __inout NvU32* pDisplayIdCount, __in NvU32 flags);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetAllDisplayIds
//
//!   DESCRIPTION: This API returns display IDs for all possible outputs on the GPU.
//!                For DPMST connector, it will return display IDs for all the video sinks in the topology. \n
//! HOW TO USE: 1. The first call should be made to get the all display ID count. To get the display ID count, send in \n
//!                  a) hPhysicalGpu    - a valid GPU handle(enumerated using NvAPI_EnumPhysicalGPUs()) as input,      \n
//!                  b) pDisplayIds     - NULL, as we just want to get the display ID count.                           \n
//!                  c) pDisplayIdCount - a valid pointer to NvU32, whose value is set to ZERO.                        \n
//!                If all parameters are correct and this call is successful, this call will return the display ID's count. \n
//!             2. To get the display ID array, make the second call to NvAPI_GPU_GetAllDisplayIds() with              \n
//!                  a) hPhysicalGpu    - should be same value which was sent in first call,                           \n
//!                  b) pDisplayIds     - pointer to the display ID array allocated by caller based on display ID count,    \n
//!                                       eg. malloc(sizeof(NV_GPU_DISPLAYIDS) * pDisplayIdCount).                     \n
//!                  c) pDisplayIdCount - a valid pointer to NvU32. This indicates for how many display IDs            \n
//!                                       the memory is allocated(pDisplayIds) by the caller.                          \n
//!                If all parameters are correct and this call is successful, this call will return the display ID array and actual
//!                display ID count (which was obtained in the first call to NvAPI_GPU_GetAllDisplayIds). If the input display ID count is
//!                less than the actual display ID count, it will overwrite the input and give the pDisplayIdCount as actual count and the
//!                API will return NVAPI_INSUFFICIENT_BUFFER.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     hPhysicalGpu         GPU selection.
//! \param [in,out] DisplayIds           Pointer to an array of NV_GPU_DISPLAYIDS structures, each entry represents one displayID
//!                                      and its attributes.
//! \param [in,out] pDisplayIdCount      As input, this parameter indicates the number of display's id's for which caller has
//!                                      allocated the memory. As output, it will return the actual number of display IDs.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!          specific meaning for this API, they are listed below.
//!
//! \retval  NVAPI_INSUFFICIENT_BUFFER  When the input buffer(pDisplayIds) is less than the actual number of display IDs, this API
//!                                     will return NVAPI_INSUFFICIENT_BUFFER.
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetAllDisplayIds(__in NvPhysicalGpuHandle hPhysicalGpu, __inout_ecount_part_opt(*pDisplayIdCount, *pDisplayIdCount) NV_GPU_DISPLAYIDS* pDisplayIds, __inout NvU32* pDisplayIdCount);




///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetConnectedOutputsWithLidState
//
//!  This function is similar to NvAPI_GPU_GetConnectedOutputs(), and returns the connected display identifiers that are connected
//!  as an output mask but unlike NvAPI_GPU_GetConnectedOutputs() this API "always" reflects the Lid State in the output mask.
//!  Thus if you expect the LID close state to be available in the connection mask use this API.
//!  - If LID is closed then this API will remove the LID panel from the connected display identifiers.
//!  - If LID is open then this API will reflect the LID panel in the connected display identifiers.
//!
//! \note This API should be used on notebook systems and on systems where the LID state is required in the connection
//!       output mask. On desktop systems the returned identifiers will match NvAPI_GPU_GetConnectedOutputs().
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_GPU_GetConnectedDisplayIds.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 95
//!
//! \retval  NVAPI_INVALID_ARGUMENT              hPhysicalGpu or pOutputsMask is NULL
//! \retval  NVAPI_OK                           *pOutputsMask contains a set of GPU-output identifiers
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found
//! \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_GPU_GetConnectedDisplayIds.")
NVAPI_INTERFACE NvAPI_GPU_GetConnectedOutputsWithLidState(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pOutputsMask);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetConnectedSLIOutputsWithLidState
//
//!   DESCRIPTION: This function is the same as NvAPI_GPU_GetConnectedOutputsWithLidState() but returns only the set
//!                of GPU-output identifiers that can be selected in an SLI configuration. With SLI disabled,
//!                this function matches NvAPI_GPU_GetConnectedOutputsWithLidState().
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_GPU_GetConnectedDisplayIds.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 170
//!
//! \retval  NVAPI_INVALID_ARGUMENT              hPhysicalGpu or pOutputsMask is NULL
//! \retval  NVAPI_OK                           *pOutputsMask contains a set of GPU-output identifiers
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found
//! \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_GPU_GetConnectedDisplayIds.")
NVAPI_INTERFACE NvAPI_GPU_GetConnectedSLIOutputsWithLidState(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pOutputsMask);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetSystemType
//
//! \fn NvAPI_GPU_GetSystemType(NvPhysicalGpuHandle hPhysicalGpu, NV_SYSTEM_TYPE *pSystemType)
//!  This function identifies whether the GPU is a notebook GPU or a desktop GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 95
//!
//! \retval  NVAPI_INVALID_ARGUMENT              hPhysicalGpu or pOutputsMask is NULL
//! \retval  NVAPI_OK                           *pSystemType contains the GPU system type
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found
//! \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE: hPhysicalGpu was not a physical GPU handle
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup gpu
//! Used in NvAPI_GPU_GetSystemType()
typedef enum
{
    NV_SYSTEM_TYPE_UNKNOWN = 0,
    NV_SYSTEM_TYPE_LAPTOP  = 1,
    NV_SYSTEM_TYPE_DESKTOP = 2,

} NV_SYSTEM_TYPE;



//! \ingroup gpu
NVAPI_INTERFACE NvAPI_GPU_GetSystemType(NvPhysicalGpuHandle hPhysicalGpu, NV_SYSTEM_TYPE *pSystemType);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetActiveOutputs
//
//!  This function is the same as NvAPI_GPU_GetAllOutputs but returns only the set of GPU output
//!  identifiers that are actively driving display devices.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 85
//!
//! \retval    NVAPI_INVALID_ARGUMENT              hPhysicalGpu or pOutputsMask is NULL.
//! \retval    NVAPI_OK                           *pOutputsMask contains a set of GPU-output identifiers.
//! \retval    NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//! \retval    NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle.
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetActiveOutputs(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pOutputsMask);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_SetEDID
//
//!  Thus function sets the EDID data for the specified GPU handle and connection bit mask.
//!  User can either send (Gpu handle & output id)  or only display Id in variable displayOutputId parameter & hPhysicalGpu parameter can be default handle (0).
//!  \note The EDID will be cached across the boot session and will be enumerated to the OS in this call.
//!        To remove the EDID set sizeofEDID to zero.
//!        OS and NVAPI connection status APIs will reflect the newly set or removed EDID dynamically.
//!
//!                This feature will NOT be supported on the following boards:
//!                - GeForce
//!                - Quadro VX
//!                - Tesla
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 100
//!
//! \retval  NVAPI_INVALID_ARGUMENT              pEDID is NULL; displayOutputId has 0 or > 1 bits set
//! \retval  NVAPI_OK                           *pEDID data was applied to the requested displayOutputId.
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//! \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE: hPhysicalGpu was not a physical GPU handle.
//! \retval  NVAPI_NOT_SUPPORTED                 For the above mentioned GPUs
//! \retval  NVAPI_INVALID_USER_PRIVILEGE        The application will require Administrator privileges to access this API.
//!                                              The application can be elevated to a higher permission level by selecting "Run as Administrator".
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_SetEDID(NvPhysicalGpuHandle hPhysicalGpu, NvU32 displayOutputId, NV_EDID *pEDID);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetOutputType
//
//! \fn NvAPI_GPU_GetOutputType(NvPhysicalGpuHandle hPhysicalGpu, NvU32 outputId, NV_GPU_OUTPUT_TYPE *pOutputType)
//!  This function returns the output type. User can either specify both 'physical GPU handle and outputId (exactly 1 bit set - see \ref handles)' or
//!  a valid displayId in the outputId parameter.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \Version Earliest supported ForceWare version: 82.61
//!
//! \retval     NVAPI_INVALID_ARGUMENT              outputId, pOutputType is NULL; or if outputId parameter is not displayId and either it has > 1 bit set or hPhysicalGpu is NULL.
//! \retval     NVAPI_OK                           *pOutputType contains a NvGpuOutputType value
//! \retval     NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found
//! \retval     NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup gpu
//! used in NvAPI_GPU_GetOutputType()
typedef enum _NV_GPU_OUTPUT_TYPE
{
    NVAPI_GPU_OUTPUT_UNKNOWN  = 0,
    NVAPI_GPU_OUTPUT_CRT      = 1,     //!<  CRT display device
    NVAPI_GPU_OUTPUT_DFP      = 2,     //!<  Digital Flat Panel display device
    NVAPI_GPU_OUTPUT_TV       = 3,     //!<  TV display device
} NV_GPU_OUTPUT_TYPE;




//! \ingroup gpu
NVAPI_INTERFACE NvAPI_GPU_GetOutputType(NvPhysicalGpuHandle hPhysicalGpu, NvU32 outputId, NV_GPU_OUTPUT_TYPE *pOutputType);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_ValidateOutputCombination
//
//!  This function determines if a set of GPU outputs can be active
//!  simultaneously.  While a GPU may have <n> outputs, typically they cannot
//!  all be active at the same time due to internal resource sharing.
//!
//!  Given a physical GPU handle and a mask of candidate outputs, this call
//!  will return NVAPI_OK if all of the specified outputs can be driven
//!  simultaneously.  It will return NVAPI_INVALID_COMBINATION if they cannot.
//!
//!  Use NvAPI_GPU_GetAllOutputs() to determine which outputs are candidates.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 85
//!
//! \retval  NVAPI_OK                            Combination of outputs in outputsMask are valid (can be active simultaneously).
//! \retval  NVAPI_INVALID_COMBINATION           Combination of outputs in outputsMask are NOT valid.
//! \retval  NVAPI_INVALID_ARGUMENT              hPhysicalGpu or outputsMask does not have at least 2 bits set.
//! \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle.
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_ValidateOutputCombination(NvPhysicalGpuHandle hPhysicalGpu, NvU32 outputsMask);




///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetFullName
//
//!  This function retrieves the full GPU name as an ASCII string - for example, "Quadro FX 1400".
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \return  NVAPI_ERROR or NVAPI_OK
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetFullName(NvPhysicalGpuHandle hPhysicalGpu, NvAPI_ShortString szName);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetPCIIdentifiers
//
//!  This function returns the PCI identifiers associated with this GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \param   DeviceId      The internal PCI device identifier for the GPU.
//! \param   SubSystemId   The internal PCI subsystem identifier for the GPU.
//! \param   RevisionId    The internal PCI device-specific revision identifier for the GPU.
//! \param   ExtDeviceId   The external PCI device identifier for the GPU.
//!
//! \retval  NVAPI_INVALID_ARGUMENT              hPhysicalGpu or an argument is NULL
//! \retval  NVAPI_OK                            Arguments are populated with PCI identifiers
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found
//! \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetPCIIdentifiers(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pDeviceId,NvU32 *pSubSystemId,NvU32 *pRevisionId,NvU32 *pExtDeviceId);




//! \ingroup gpu
//! Used in NvAPI_GPU_GetGPUType().
typedef enum _NV_GPU_TYPE
{
    NV_SYSTEM_TYPE_GPU_UNKNOWN     = 0,
    NV_SYSTEM_TYPE_IGPU            = 1, //!< Integrated GPU
    NV_SYSTEM_TYPE_DGPU            = 2, //!< Discrete GPU
} NV_GPU_TYPE;


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetGPUType
//
//!  DESCRIPTION: This function returns the GPU type (integrated or discrete).
//!               See ::NV_GPU_TYPE.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 173
//!
//! \retval  NVAPI_INVALID_ARGUMENT              hPhysicalGpu
//! \retval  NVAPI_OK                           *pGpuType contains the GPU type
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found
//! \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE: hPhysicalGpu was not a physical GPU handle
//!
//!  \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetGPUType(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_GPU_TYPE *pGpuType);




//! \ingroup gpu
//! Used in NvAPI_GPU_GetBusType()
typedef enum _NV_GPU_BUS_TYPE
{
    NVAPI_GPU_BUS_TYPE_UNDEFINED    = 0,
    NVAPI_GPU_BUS_TYPE_PCI          = 1,
    NVAPI_GPU_BUS_TYPE_AGP          = 2,
    NVAPI_GPU_BUS_TYPE_PCI_EXPRESS  = 3,
    NVAPI_GPU_BUS_TYPE_FPCI         = 4,
    NVAPI_GPU_BUS_TYPE_AXI          = 5,
} NV_GPU_BUS_TYPE;
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetBusType
//
//!  This function returns the type of bus associated with this GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \return      This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!              specific meaning for this API, they are listed below.
//! \retval      NVAPI_INVALID_ARGUMENT             hPhysicalGpu or pBusType is NULL.
//! \retval      NVAPI_OK                          *pBusType contains bus identifier.
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetBusType(NvPhysicalGpuHandle hPhysicalGpu,NV_GPU_BUS_TYPE *pBusType);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetBusId
//
//!   DESCRIPTION: Returns the ID of the bus associated with this GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 167
//!
//!  \retval  NVAPI_INVALID_ARGUMENT              hPhysicalGpu or pBusId is NULL.
//!  \retval  NVAPI_OK                           *pBusId contains the bus ID.
//!  \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//!  \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle.
//!
//!  \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetBusId(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pBusId);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetBusSlotId
//
//!   DESCRIPTION: Returns the ID of the bus slot associated with this GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 167
//!
//!  \retval  NVAPI_INVALID_ARGUMENT              hPhysicalGpu or pBusSlotId is NULL.
//!  \retval  NVAPI_OK                           *pBusSlotId contains the bus slot ID.
//!  \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//!  \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle.
//!
//!  \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetBusSlotId(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pBusSlotId);



///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetIRQ
//
//!  This function returns the interrupt number associated with this GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \retval  NVAPI_INVALID_ARGUMENT              hPhysicalGpu or pIRQ is NULL.
//! \retval  NVAPI_OK                           *pIRQ contains interrupt number.
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//! \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle.
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetIRQ(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pIRQ);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetVbiosRevision
//
//!  This function returns the revision of the video BIOS associated with this GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \retval    NVAPI_INVALID_ARGUMENT               hPhysicalGpu or pBiosRevision is NULL.
//! \retval    NVAPI_OK                            *pBiosRevision contains revision number.
//! \retval    NVAPI_NVIDIA_DEVICE_NOT_FOUND        No NVIDIA GPU driving a display was found.
//! \retval    NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE   hPhysicalGpu was not a physical GPU handle.
//! \ingroup   gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetVbiosRevision(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pBiosRevision);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetVbiosOEMRevision
//
//!  This function returns the OEM revision of the video BIOS associated with this GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \retval    NVAPI_INVALID_ARGUMENT              hPhysicalGpu or pBiosRevision is NULL
//! \retval    NVAPI_OK                           *pBiosRevision contains revision number
//! \retval    NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found
//! \retval    NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle
//! \ingroup   gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetVbiosOEMRevision(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pBiosRevision);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetVbiosVersionString
//
//!  This function returns the full video BIOS version string in the form of xx.xx.xx.xx.yy where
//!  - xx numbers come from NvAPI_GPU_GetVbiosRevision() and
//!  - yy comes from NvAPI_GPU_GetVbiosOEMRevision().
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \retval   NVAPI_INVALID_ARGUMENT              hPhysicalGpu is NULL.
//! \retval   NVAPI_OK                            szBiosRevision contains version string.
//! \retval   NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//! \retval   NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle.
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetVbiosVersionString(NvPhysicalGpuHandle hPhysicalGpu,NvAPI_ShortString szBiosRevision);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetAGPAperture
//
//!  This function returns the AGP aperture in megabytes.
//!
//! \deprecated  Do not use this function - it is deprecated in release 455.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \retval   NVAPI_NOT_SUPPORTED
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 455.")
NVAPI_INTERFACE NvAPI_GPU_GetAGPAperture(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pSize);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetCurrentAGPRate
//
//!  This function returns the current AGP Rate (0 = AGP not present, 1 = 1x, 2 = 2x, etc.).
//!
//! \deprecated  Do not use this function - it is deprecated in release 455.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \retval   NVAPI_NOT_SUPPORTED
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 455.")
NVAPI_INTERFACE NvAPI_GPU_GetCurrentAGPRate(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pRate);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetCurrentPCIEDownstreamWidth
//
//!  This function returns the number of PCIE lanes being used for the PCIE interface
//!  downstream from the GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \retval  NVAPI_INVALID_ARGUMENT              pWidth is NULL.
//! \retval  NVAPI_OK                            Call successful.
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//! \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle.
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetCurrentPCIEDownstreamWidth(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pWidth);



///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetPhysicalFrameBufferSize
//
//!   This function returns the physical size of framebuffer in KB.  This does NOT include any
//!   system RAM that may be dedicated for use by the GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \retval  NVAPI_INVALID_ARGUMENT              pSize is NULL
//! \retval  NVAPI_OK                            Call successful
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found
//! \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetPhysicalFrameBufferSize(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pSize);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetVirtualFrameBufferSize
//
//!  This function returns the virtual size of framebuffer in KB.  This includes the physical RAM plus any
//!  system RAM that has been dedicated for use by the GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 90
//!
//! \retval  NVAPI_INVALID_ARGUMENT              pSize is NULL.
//! \retval  NVAPI_OK                            Call successful.
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND       No NVIDIA GPU driving a display was found.
//! \retval  NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu was not a physical GPU handle.
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetVirtualFrameBufferSize(NvPhysicalGpuHandle hPhysicalGpu,NvU32 *pSize);



///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetQuadroStatus
//
//!  This function retrieves the Quadro status for the GPU (1 if Quadro, 0 if GeForce)
//!
//!  This API is deprecated and it is recommended to instead query 
//!  NV_GPU_WORKSTATION_FEATURE_TYPE_PROVIZ support from the API
//!  NvAPI_GPU_QueryWorkstationFeatureSupport.
//! \deprecated  Do not use this function - it is deprecated in release 460.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \return  NVAPI_ERROR or NVAPI_OK
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 460.")
NVAPI_INTERFACE NvAPI_GPU_GetQuadroStatus(NvPhysicalGpuHandle hPhysicalGpu, NvU32 *pStatus);


//! \ingroup gpu
typedef struct _NV_BOARD_INFO
{
    NvU32 version;                   //!< structure version
    NvU8 BoardNum[16];               //!< Board Serial Number

}NV_BOARD_INFO_V1;

#define NV_BOARD_INFO_VER1  MAKE_NVAPI_VERSION(NV_BOARD_INFO_V1,1)
#ifndef NV_BOARD_INFO_VER
//! \ingroup gpu
typedef NV_BOARD_INFO_V1    NV_BOARD_INFO;
//! \ingroup gpu
//! \ingroup gpu
#define NV_BOARD_INFO_VER   NV_BOARD_INFO_VER1
#endif

//! SUPPORTED OS:  Windows 7 and higher
//!
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetBoardInfo
//
//!   DESCRIPTION: This API Retrieves the Board information (a unique GPU Board Serial Number) stored in the InfoROM.
//!
//! \param [in]      hPhysicalGpu       Physical GPU Handle.
//! \param [in,out]  NV_BOARD_INFO      Board Information.
//!
//! TCC_SUPPORTED
//!
//! \retval ::NVAPI_OK                     completed request
//! \retval ::NVAPI_ERROR                  miscellaneous error occurred
//! \retval ::NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  handle passed is not a physical GPU handle
//! \retval ::NVAPI_API_NOT_INTIALIZED            NVAPI not initialized
//! \retval ::NVAPI_INVALID_POINTER               pBoardInfo is NULL
//! \retval ::NVAPI_INCOMPATIBLE_STRUCT_VERSION   the version of the INFO struct is not supported
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetBoardInfo(NvPhysicalGpuHandle hPhysicalGpu, NV_BOARD_INFO *pBoardInfo);




///////////////////////////////////////////////////////////////////////////////////
//  I2C API
//  Provides ability to read or write data using I2C protocol.
//  These APIs allow I2C access only to DDC monitors


//! \addtogroup i2capi
//! @{
#define NVAPI_MAX_SIZEOF_I2C_DATA_BUFFER    4096
#define NVAPI_MAX_SIZEOF_I2C_REG_ADDRESS       4
#define NVAPI_DISPLAY_DEVICE_MASK_MAX         24
#define NVAPI_I2C_SPEED_DEPRECATED        0xFFFF

typedef enum
{
    NVAPI_I2C_SPEED_DEFAULT,    //!< Set i2cSpeedKhz to I2C_SPEED_DEFAULT if default I2C speed is to be chosen, ie.use the current frequency setting.
    NVAPI_I2C_SPEED_3KHZ,
    NVAPI_I2C_SPEED_10KHZ,
    NVAPI_I2C_SPEED_33KHZ,
    NVAPI_I2C_SPEED_100KHZ,
    NVAPI_I2C_SPEED_200KHZ,
    NVAPI_I2C_SPEED_400KHZ,
} NV_I2C_SPEED;

//! Used in NvAPI_I2CRead() and NvAPI_I2CWrite()
typedef struct
{
    NvU32                   version;            //!< The structure version.
    NvU32                   displayMask;        //!< The Display Mask of the concerned display.
    NvU8                    bIsDDCPort;         //!< This flag indicates either the DDC port (TRUE) or the communication port
                                                //!< (FALSE) of the concerned display.
    NvU8                    i2cDevAddress;      //!< The address of the I2C slave.  The address should be shifted left by one.  For
                                                //!< example, the I2C address 0x50, often used for reading EDIDs, would be stored
                                                //!< here as 0xA0.  This matches the position within the byte sent by the master, as
                                                //!< the last bit is reserved to specify the read or write direction.
    NvU8*                   pbI2cRegAddress;    //!< The I2C target register address.  May be NULL, which indicates no register
                                                //!< address should be sent.
    NvU32                   regAddrSize;        //!< The size in bytes of target register address.  If pbI2cRegAddress is NULL, this
                                                //!< field must be 0.
    NvU8*                   pbData;             //!< The buffer of data which is to be read or written (depending on the command).
    NvU32                   cbSize;             //!< The size of the data buffer, pbData, to be read or written.
    NvU32                   i2cSpeed;           //!< The target speed of the transaction (between 28Kbps to 40Kbps; not guaranteed).
} NV_I2C_INFO_V1;

//! Used in NvAPI_I2CRead() and NvAPI_I2CWrite()
typedef struct
{
    NvU32                   version;            //!< The structure version.
    NvU32                   displayMask;        //!< The Display Mask of the concerned display.
    NvU8                    bIsDDCPort;         //!< This flag indicates either the DDC port (TRUE) or the communication port
                                                //!< (FALSE) of the concerned display.
    NvU8                    i2cDevAddress;      //!< The address of the I2C slave.  The address should be shifted left by one.  For
                                                //!< example, the I2C address 0x50, often used for reading EDIDs, would be stored
                                                //!< here as 0xA0.  This matches the position within the byte sent by the master, as
                                                //!< the last bit is reserved to specify the read or write direction.
    NvU8*                   pbI2cRegAddress;    //!< The I2C target register address.  May be NULL, which indicates no register
                                                //!< address should be sent.
    NvU32                   regAddrSize;        //!< The size in bytes of target register address.  If pbI2cRegAddress is NULL, this
                                                //!< field must be 0.
    NvU8*                   pbData;             //!< The buffer of data which is to be read or written (depending on the command).
    NvU32                   cbSize;             //!< The size of the data buffer, pbData, to be read or written.
    NvU32                   i2cSpeed;           //!< Deprecated, Must be set to NVAPI_I2C_SPEED_DEPRECATED.
    NV_I2C_SPEED            i2cSpeedKhz;        //!< The target speed of the transaction in (kHz) (Chosen from the enum NV_I2C_SPEED).
} NV_I2C_INFO_V2;

//! Used in NvAPI_I2CRead() and NvAPI_I2CWrite()
typedef struct
{
    NvU32                   version;            //!< The structure version.
    NvU32                   displayMask;        //!< The Display Mask of the concerned display.
    NvU8                    bIsDDCPort;         //!< This flag indicates either the DDC port (TRUE) or the communication port
                                                //!< (FALSE) of the concerned display.
    NvU8                    i2cDevAddress;      //!< The address of the I2C slave.  The address should be shifted left by one.  For
                                                //!< example, the I2C address 0x50, often used for reading EDIDs, would be stored
                                                //!< here as 0xA0.  This matches the position within the byte sent by the master, as
                                                //!< the last bit is reserved to specify the read or write direction.
    NvU8*                   pbI2cRegAddress;    //!< The I2C target register address.  May be NULL, which indicates no register
                                                //!< address should be sent.
    NvU32                   regAddrSize;        //!< The size in bytes of target register address.  If pbI2cRegAddress is NULL, this
                                                //!< field must be 0.
    NvU8*                   pbData;             //!< The buffer of data which is to be read or written (depending on the command).
    NvU32                   cbSize;             //!< The size of the data buffer, pbData, to be read or written.
    NvU32                   i2cSpeed;           //!< Deprecated, Must be set to NVAPI_I2C_SPEED_DEPRECATED.
    NV_I2C_SPEED            i2cSpeedKhz;        //!< The target speed of the transaction in (kHz) (Chosen from the enum NV_I2C_SPEED).
    NvU8                    portId;             //!< The portid on which device is connected (remember to set bIsPortIdSet if this value is set)
                                                //!< Optional for pre-Kepler
    NvU32                   bIsPortIdSet;       //!< set this flag on if and only if portid value is set
} NV_I2C_INFO_V3;

typedef NV_I2C_INFO_V3                     NV_I2C_INFO;

#define NV_I2C_INFO_VER3  MAKE_NVAPI_VERSION(NV_I2C_INFO_V3,3)
#define NV_I2C_INFO_VER2  MAKE_NVAPI_VERSION(NV_I2C_INFO_V2,2)
#define NV_I2C_INFO_VER1  MAKE_NVAPI_VERSION(NV_I2C_INFO_V1,1)

#define NV_I2C_INFO_VER  NV_I2C_INFO_VER3
//! @}

/***********************************************************************************/


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  NvAPI_I2CRead
//
//!  This function reads the data buffer from the I2C port.
//!                 The I2C request must be for a DDC port: pI2cInfo->bIsDDCPort = 1.
//!
//!                 A data buffer size larger than 16 bytes may be rejected if a register address is specified.  In such a case,
//!                 NVAPI_ARGUMENT_EXCEED_MAX_SIZE would be returned.
//!
//!                 If a register address is specified (i.e. regAddrSize is positive), then the transaction will be performed in
//!                 the combined format described in the I2C specification.  The register address will be written, followed by
//!                 reading into the data buffer.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 85
//!
//! \param [in]   hPhysicalGPU     GPU selection.
//! \param [out]  NV_I2C_INFO     *pI2cInfo The I2C data input structure
//!
//! \retval   NVAPI_OK                            Completed request
//! \retval   NVAPI_ERROR                         Miscellaneous error occurred.
//! \retval   NVAPI_HANDLE_INVALIDATED            Handle passed has been invalidated (see user guide).
//! \retval   NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  Handle passed is not a physical GPU handle.
//! \retval   NVAPI_INCOMPATIBLE_STRUCT_VERSION   Structure version is not supported.
//! \retval   NVAPI_INVALID_ARGUMENT - argument does not meet specified requirements
//! \retval   NVAPI_ARGUMENT_EXCEED_MAX_SIZE - an argument exceeds the maximum
//!
//! \ingroup i2capi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_I2CRead(NvPhysicalGpuHandle hPhysicalGpu, NV_I2C_INFO *pI2cInfo);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:  NvAPI_I2CWrite
//
//!  This function writes the data buffer to the I2C port.
//!
//!                 The I2C request must be for a DDC port: pI2cInfo->bIsDDCPort = 1.
//!
//!                 A data buffer size larger than 16 bytes may be rejected if a register address is specified.  In such a case,
//!                 NVAPI_ARGUMENT_EXCEED_MAX_SIZE would be returned.
//!
//!                 If a register address is specified (i.e. regAddrSize is positive), then the register address will be written
//!                 and the data buffer will immediately follow without a restart.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 85
//!
//! \param [in]   hPhysicalGPU     GPU selection.
//! \param [in]   pI2cInfo         The I2C data input structure
//!
//! \retval   NVAPI_OK                            Completed request
//! \retval   NVAPI_ERROR                         Miscellaneous error occurred.
//! \retval   NVAPI_HANDLE_INVALIDATED            Handle passed has been invalidated (see user guide).
//! \retval   NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  Handle passed is not a physical GPU handle.
//! \retval   NVAPI_INCOMPATIBLE_STRUCT_VERSION    Structure version is not supported.
//! \retval   NVAPI_INVALID_ARGUMENT              Argument does not meet specified requirements
//! \retval   NVAPI_ARGUMENT_EXCEED_MAX_SIZE      Argument exceeds the maximum
//!
//! \ingroup i2capi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_I2CWrite(NvPhysicalGpuHandle hPhysicalGpu, NV_I2C_INFO *pI2cInfo);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_WorkstationFeatureSetup
//
//! \fn NvAPI_GPU_WorkstationFeatureSetup(NvPhysicalGpuHandle hPhysicalGpu, NvU32 featureEnableMask, NvU32 featureDisableMask)
//!   DESCRIPTION: This API configures the driver for a set of workstation features.
//!                The driver can allocate the memory resources accordingly.
//!
//! SUPPORTED OS:  Windows 7
//!
//!
//! \param [in]   hPhysicalGpu       Physical GPU Handle of the display adapter to be configured. GPU handles may be retrieved
//!                                  using NvAPI_EnumPhysicalGPUs. A value of NULL is permitted and applies the same operation
//!                                  to all GPU handles enumerated by NvAPI_EnumPhysicalGPUs.
//! \param [in]   featureEnableMask  Mask of features the caller requests to enable for use
//! \param [in]   featureDisableMask Mask of features the caller requests to disable
//!
//!                As a general rule, features in the enable and disable masks are expected to be disjoint, although the disable
//!                mask has precedence and a feature flagged in both masks will be disabled.
//!
//! \retval ::NVAPI_OK                            configuration request succeeded
//! \retval ::NVAPI_ERROR                         configuration request failed
//! \retval ::NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu is not a physical GPU handle.
//! \retval ::NVAPI_GPU_WORKSTATION_FEATURE_INCOMPLETE  requested feature set does not have all resources allocated for completeness.
//! \retval ::NVAPI_NO_IMPLEMENTATION             OS below Win7, implemented only for Win7 but returns NVAPI_OK on OS above Win7 to
//!                                               keep compatibility with apps written against Win7.
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup gpu
typedef enum
{
    NVAPI_GPU_WORKSTATION_FEATURE_MASK_SWAPGROUP     = 0x00000001,
    NVAPI_GPU_WORKSTATION_FEATURE_MASK_STEREO        = 0x00000010,
    NVAPI_GPU_WORKSTATION_FEATURE_MASK_WARPING       = 0x00000100,
    NVAPI_GPU_WORKSTATION_FEATURE_MASK_PIXINTENSITY  = 0x00000200,
    NVAPI_GPU_WORKSTATION_FEATURE_MASK_GRAYSCALE     = 0x00000400,
    NVAPI_GPU_WORKSTATION_FEATURE_MASK_BPC10         = 0x00001000
} NVAPI_GPU_WORKSTATION_FEATURE_MASK;

//! \ingroup gpu
NVAPI_INTERFACE NvAPI_GPU_WorkstationFeatureSetup(__in NvPhysicalGpuHandle hPhysicalGpu, __in NvU32 featureEnableMask, __in NvU32 featureDisableMask);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_WorkstationFeatureQuery
//
//!   DESCRIPTION: This API queries the current set of workstation features.
//!
//! SUPPORTED OS:  Windows 7
//!
//!
//! \param [in]   hPhysicalGpu       Physical GPU Handle of the display adapter to be configured. GPU handles may be retrieved
//!                                  using NvAPI_EnumPhysicalGPUs.
//! \param [out]  pConfiguredFeatureMask  Mask of features requested for use by client drivers
//! \param [out]  pConsistentFeatureMask  Mask of features that have all resources allocated for completeness.
//!
//! \retval ::NVAPI_OK                            configuration request succeeded
//! \retval ::NVAPI_ERROR                         configuration request failed
//! \retval ::NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  hPhysicalGpu is not a physical GPU handle.
//! \retval ::NVAPI_NO_IMPLEMENTATION             OS below Win7, implemented only for Win7 but returns NVAPI_OK on OS above Win7 to
//!                                               keep compatibility with apps written against Win7.
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup gpu
NVAPI_INTERFACE NvAPI_GPU_WorkstationFeatureQuery(__in NvPhysicalGpuHandle hPhysicalGpu, __out_opt NvU32 *pConfiguredFeatureMask, __out_opt NvU32 *pConsistentFeatureMask);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetHDCPSupportStatus
//
//! \fn NvAPI_GPU_GetHDCPSupportStatus(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_GET_HDCP_SUPPORT_STATUS *pGetHDCPSupportStatus)
//! DESCRIPTION: This function returns a GPU's HDCP support status.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 175
//!
//!  \retval ::NVAPI_OK
//!  \retval ::NVAPI_ERROR
//!  \retval ::NVAPI_INVALID_ARGUMENT
//!  \retval ::NVAPI_HANDLE_INVALIDATED
//!  \retval ::NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE
//!  \retval ::NVAPI_INCOMPATIBLE_STRUCT_VERSION
//
////////////////////////////////////////////////////////////////////////////////


//! \addtogroup gpu
//! @{


//! HDCP fuse states - used in NV_GPU_GET_HDCP_SUPPORT_STATUS
typedef enum _NV_GPU_HDCP_FUSE_STATE
{
    NV_GPU_HDCP_FUSE_STATE_UNKNOWN  = 0,
    NV_GPU_HDCP_FUSE_STATE_DISABLED = 1,
    NV_GPU_HDCP_FUSE_STATE_ENABLED  = 2,
} NV_GPU_HDCP_FUSE_STATE;


//! HDCP key sources - used in NV_GPU_GET_HDCP_SUPPORT_STATUS
typedef enum _NV_GPU_HDCP_KEY_SOURCE
{
    NV_GPU_HDCP_KEY_SOURCE_UNKNOWN    = 0,
    NV_GPU_HDCP_KEY_SOURCE_NONE       = 1,
    NV_GPU_HDCP_KEY_SOURCE_CRYPTO_ROM = 2,
    NV_GPU_HDCP_KEY_SOURCE_SBIOS      = 3,
    NV_GPU_HDCP_KEY_SOURCE_I2C_ROM    = 4,
    NV_GPU_HDCP_KEY_SOURCE_FUSES      = 5,
} NV_GPU_HDCP_KEY_SOURCE;


//! HDCP key source states - used in NV_GPU_GET_HDCP_SUPPORT_STATUS
typedef enum _NV_GPU_HDCP_KEY_SOURCE_STATE
{
    NV_GPU_HDCP_KEY_SOURCE_STATE_UNKNOWN = 0,
    NV_GPU_HDCP_KEY_SOURCE_STATE_ABSENT  = 1,
    NV_GPU_HDCP_KEY_SOURCE_STATE_PRESENT = 2,
} NV_GPU_HDCP_KEY_SOURCE_STATE;


//! HDPC support status - used in NvAPI_GPU_GetHDCPSupportStatus()
typedef struct
{
    NvU32                        version;               //! Structure version constucted by macro #NV_GPU_GET_HDCP_SUPPORT_STATUS
    NV_GPU_HDCP_FUSE_STATE       hdcpFuseState;         //! GPU's HDCP fuse state
    NV_GPU_HDCP_KEY_SOURCE       hdcpKeySource;         //! GPU's HDCP key source
    NV_GPU_HDCP_KEY_SOURCE_STATE hdcpKeySourceState;    //! GPU's HDCP key source state
} NV_GPU_GET_HDCP_SUPPORT_STATUS;


//! Macro for constructing the version for structure NV_GPU_GET_HDCP_SUPPORT_STATUS
#define NV_GPU_GET_HDCP_SUPPORT_STATUS_VER MAKE_NVAPI_VERSION(NV_GPU_GET_HDCP_SUPPORT_STATUS,1)


//! @}


//!  \ingroup gpu
NVAPI_INTERFACE NvAPI_GPU_GetHDCPSupportStatus(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_GET_HDCP_SUPPORT_STATUS *pGetHDCPSupportStatus);



///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetTachReading
//
//!   DESCRIPTION: This API retrieves the fan speed tachometer reading for the specified physical GPU.
//!
//!   HOW TO USE:
//!                 - NvU32 Value = 0;
//!                 - ret = NvAPI_GPU_GetTachReading(hPhysicalGpu, &Value);
//!                 - On call success:
//!                 - Value contains the tachometer reading
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \param [in]    hPhysicalGpu   GPU selection.
//! \param [out]   pValue         Pointer to a variable to get the tachometer reading
//!
//! \retval ::NVAPI_OK - completed request
//! \retval ::NVAPI_ERROR - miscellaneous error occurred
//! \retval ::NVAPI_NOT_SUPPORTED - functionality not supported
//! \retval ::NVAPI_API_NOT_INTIALIZED - nvapi not initialized
//! \retval ::NVAPI_INVALID_ARGUMENT - invalid argument passed
//! \retval ::NVAPI_HANDLE_INVALIDATED - handle passed has been invalidated (see user guide)
//! \retval ::NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE - handle passed is not a physical GPU handle
//!
//! \ingroup gpucooler
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetTachReading(NvPhysicalGpuHandle hPhysicalGPU, NvU32 *pValue);




///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GPU_GetECCStatusInfo
//
//! \fn NvAPI_GPU_GetECCStatusInfo(NvPhysicalGpuHandle hPhysicalGpu,
//!                                           NV_GPU_ECC_STATUS_INFO *pECCStatusInfo);
//! DESCRIPTION:     This function returns ECC memory status information.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \param [in]      hPhysicalGpu      A handle identifying the physical GPU for which ECC
//!                                    status information is to be retrieved.
//! \param [out]     pECCStatusInfo    A pointer to an ECC status structure.
//!
//! \retval ::NVAPI_OK                  The request was completed successfully.
//! \retval ::NVAPI_ERROR               An unknown error occurred.
//! \retval ::NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE The provided GPU handle is not a physical GPU handle.
//! \retval ::NVAPI_INVALID_HANDLE      The provided GPU handle is invalid.
//! \retval ::NVAPI_HANDLE_INVALIDATED  The provided GPU handle is no longer valid.
//! \retval ::NVAPI_INVALID_POINTER     An invalid argument pointer was provided.
//! \retval ::NVAPI_NOT_SUPPORTED       The request is not supported.
//! \retval ::NVAPI_API_NOT_INTIALIZED  NvAPI was not yet initialized.
//
///////////////////////////////////////////////////////////////////////////////

//! \addtogroup gpuecc
//! Used in NV_GPU_ECC_STATUS_INFO.
typedef enum _NV_ECC_CONFIGURATION
{
    NV_ECC_CONFIGURATION_NOT_SUPPORTED = 0,
    NV_ECC_CONFIGURATION_DEFERRED,           //!< Changes require a POST to take effect
    NV_ECC_CONFIGURATION_IMMEDIATE,          //!< Changes can optionally be made to take effect immediately
} NV_ECC_CONFIGURATION;

//! \ingroup gpuecc
//! Used in NvAPI_GPU_GetECCStatusInfo().
typedef struct
{
    NvU32                 version;               //!< Structure version
    NvU32                 isSupported : 1;       //!< ECC memory feature support
    NV_ECC_CONFIGURATION  configurationOptions;  //!< Supported ECC memory feature configuration options
    NvU32                 isEnabled : 1;         //!< Active ECC memory setting
} NV_GPU_ECC_STATUS_INFO;

//! \ingroup gpuecc
//! Macro for constructing the version field of NV_GPU_ECC_STATUS_INFO
#define NV_GPU_ECC_STATUS_INFO_VER MAKE_NVAPI_VERSION(NV_GPU_ECC_STATUS_INFO,1)

//! \ingroup gpuecc
NVAPI_INTERFACE NvAPI_GPU_GetECCStatusInfo(NvPhysicalGpuHandle hPhysicalGpu,
                                           NV_GPU_ECC_STATUS_INFO *pECCStatusInfo);

//! \ingroup gpuecc
//! Used in NvAPI_GPU_GetECCErrorInfo()/
typedef struct
{
    NvU32   version;             //!< Structure version
    struct
    {
        NvU64  singleBitErrors;  //!< Number of single-bit ECC errors detected since last boot
        NvU64  doubleBitErrors;  //!< Number of double-bit ECC errors detected since last boot
    } current;
    struct
    {
        NvU64  singleBitErrors;  //!< Number of single-bit ECC errors detected since last counter reset
        NvU64  doubleBitErrors;  //!< Number of double-bit ECC errors detected since last counter reset
    } aggregate;
} NV_GPU_ECC_ERROR_INFO;

//! \ingroup gpuecc
//! Macro for constructing the version field of NV_GPU_ECC_ERROR_INFO
#define NV_GPU_ECC_ERROR_INFO_VER MAKE_NVAPI_VERSION(NV_GPU_ECC_ERROR_INFO,1)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GPU_GetECCErrorInfo
//
//! \fn NvAPI_GPU_GetECCErrorInfo(NvPhysicalGpuHandle hPhysicalGpu,
//!                                          NV_GPU_ECC_ERROR_INFO *pECCErrorInfo);
//!
//! DESCRIPTION:     This function returns ECC memory error information.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \param [in]      hPhysicalGpu  A handle identifying the physical GPU for
//!                                which ECC error information is to be
//!                                retrieved.
//! \param [out]     pECCErrorInfo A pointer to an ECC error structure.
//!
//! \retval ::NVAPI_OK  The request was completed successfully.
//! \retval ::NVAPI_ERROR  An unknown error occurred.
//! \retval ::NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  The provided GPU handle is not a physical GPU handle.
//! \retval ::NVAPI_INVALID_ARGUMENT  incorrect param value
//! \retval ::NVAPI_INVALID_POINTER  An invalid argument pointer was provided.
//! \retval ::NVAPI_INCOMPATIBLE_STRUCT_VERSION  structure version is not supported, initialize to NV_GPU_ECC_ERROR_INFO_VER.
//! \retval ::NVAPI_HANDLE_INVALIDATED  The provided GPU handle is no longer valid.
//! \retval ::NVAPI_NOT_SUPPORTED  The request is not supported.
//! \retval ::NVAPI_API_NOT_INTIALIZED  NvAPI was not yet initialized.
//
///////////////////////////////////////////////////////////////////////////////
//! \ingroup gpuecc

NVAPI_INTERFACE NvAPI_GPU_GetECCErrorInfo(NvPhysicalGpuHandle hPhysicalGpu,
                                          NV_GPU_ECC_ERROR_INFO *pECCErrorInfo);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GPU_ResetECCErrorInfo
//
//! DESCRIPTION:     This function resets ECC memory error counters.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \requires Administrator privileges since release 430.39
//!
//! \param [in]     hPhysicalGpu     A handle identifying the physical GPU for
//!                                  which ECC error information is to be
//!                                  cleared.
//! \param [in]     bResetCurrent    Reset the current ECC error counters.
//! \param [in]     bResetAggregate  Reset the aggregate ECC error counters.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!          specific meaning for this API, they are listed below.
//!
//! \retval ::NVAPI_INVALID_USER_PRIVILEGE       - The application will require Administrator privileges to access this API.
//!                                                The application can be elevated to a higher permission level by selecting "Run as Administrator".
//!
//! \ingroup gpuecc
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_ResetECCErrorInfo(NvPhysicalGpuHandle hPhysicalGpu, NvU8 bResetCurrent,
                                            NvU8 bResetAggregate);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GPU_GetECCConfigurationInfo
//
//! \fn NvAPI_GPU_GetECCConfigurationInfo(NvPhysicalGpuHandle hPhysicalGpu,
//!                             NV_GPU_ECC_CONFIGURATION_INFO *pECCConfigurationInfo);
//! DESCRIPTION:     This function returns ECC memory configuration information.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \param [in]      hPhysicalGpu  A handle identifying the physical GPU for
//!                                which ECC configuration information
//!                               is to be retrieved.
//! \param [out]     pECCConfigurationInfo  A pointer to an ECC
//!                                                configuration structure.
//!
//! \retval ::NVAPI_OK  The request was completed successfully.
//! \retval ::NVAPI_ERROR  An unknown error occurred.
//! \retval ::NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  The provided GPU handle is not a physical GPU handle.
//! \retval ::NVAPI_INVALID_HANDLE  The provided GPU handle is invalid.
//! \retval ::NVAPI_HANDLE_INVALIDATED  The provided GPU handle is no longer valid.
//! \retval ::NVAPI_INVALID_POINTER  An invalid argument pointer was provided.
//! \retval ::NVAPI_NOT_SUPPORTED  The request is not supported.
//! \retval ::NVAPI_API_NOT_INTIALIZED  NvAPI was not yet initialized.
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup gpuecc
//! Used in NvAPI_GPU_GetECCConfigurationInfo().
typedef struct
{
    NvU32  version;                 //! Structure version
    NvU32  isEnabled : 1;           //! Current ECC configuration stored in non-volatile memory
    NvU32  isEnabledByDefault : 1;  //! Factory default ECC configuration (static)
} NV_GPU_ECC_CONFIGURATION_INFO;

//! \ingroup gpuecc
//! Macro for consstructing the verion field of NV_GPU_ECC_CONFIGURATION_INFO
#define NV_GPU_ECC_CONFIGURATION_INFO_VER MAKE_NVAPI_VERSION(NV_GPU_ECC_CONFIGURATION_INFO,1)

//! \ingroup gpuecc
NVAPI_INTERFACE NvAPI_GPU_GetECCConfigurationInfo(NvPhysicalGpuHandle hPhysicalGpu,
                                                  NV_GPU_ECC_CONFIGURATION_INFO *pECCConfigurationInfo);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GPU_SetECCConfiguration
//
//! DESCRIPTION:     This function updates the ECC memory configuration setting.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \requires Administrator privileges since release 430.39
//!
//! \param [in]      hPhysicalGpu    A handle identifying the physical GPU for
//!                                  which to update the ECC configuration
//!                                  setting.
//! \param [in]      bEnable         The new ECC configuration setting.
//! \param [in]      bEnableImmediately   Request that the new setting take effect immediately.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!          specific meaning for this API, they are listed below.
//!
//! \retval ::NVAPI_INVALID_CONFIGURATION        - Possibly SLI is enabled. Disable SLI and retry.
//! \retval ::NVAPI_INVALID_USER_PRIVILEGE       - The application will require Administrator privileges to access this API.
//!                                                The application can be elevated to a higher permission level by selecting "Run as Administrator".
//!
//! \ingroup gpuecc
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_SetECCConfiguration(NvPhysicalGpuHandle hPhysicalGpu, NvU8 bEnable,
                                              NvU8 bEnableImmediately);



//! \ingroup gpu
typedef enum _NV_GPU_WORKSTATION_FEATURE_TYPE
{
    NV_GPU_WORKSTATION_FEATURE_TYPE_NVIDIA_RTX_VR_READY  = 1,  //!< NVIDIA RTX VR Ready
    NV_GPU_WORKSTATION_FEATURE_TYPE_QUADRO_VR_READY  = NV_GPU_WORKSTATION_FEATURE_TYPE_NVIDIA_RTX_VR_READY,  //!< DEPRECATED name - do not use
    NV_GPU_WORKSTATION_FEATURE_TYPE_PROVIZ = 2
} NV_GPU_WORKSTATION_FEATURE_TYPE;


///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_GPU_QueryWorkstationFeatureSupport
//
//! \fn NvAPI_GPU_QueryWorkstationFeatureSupport(NvPhysicalGpuHandle physicalGpu, NV_GPU_WORKSTATION_FEATURE_TYPE gpuWorkstationFeature)
//! \code
//! DESCRIPTION:     Indicates whether a queried workstation feature is supported by the requested GPU.
//!
//! SUPPORTED OS:  Windows 10 and higher
//!
//! \since Release: 440
//!
//! DESCRIPTION:     This API, when called with a valid physical gpu handle as Input, lets caller know whether the given workstation feature is supported by this GPU.
//!
//! PARAMETERS:      physicalGpu(IN)            : The handle of the GPU for the which caller wants to get the support information.
//!                  gpuWorkstationFeature(IN ) : The feature for the GPU in question. One of the values from enum NV_GPU_WORKSTATION_FEATURE_TYPE.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status listed below
//!
//! \retval ::NVAPI_OK the queried workstation feature is supported on the given GPU.
//! \retval ::NVAPI_NO_IMPLEMENTATION the current driver doesn't support this interface.
//! \retval ::NVAPI_INVALID_HANDLE the incoming physicalGpu handle is invalid.
//! \retval ::NVAPI_NOT_SUPPORTED the requested gpuWorkstationFeature is not supported in the selected GPU.
//! \retval ::NVAPI_SETTING_NOT_FOUND the requested gpuWorkstationFeature is unknown to the current driver version.
//!
//! \endcode
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_QueryWorkstationFeatureSupport(NvPhysicalGpuHandle physicalGpu, NV_GPU_WORKSTATION_FEATURE_TYPE gpuWorkstationFeature);



//! \ingroup gpu
typedef struct
{
    NvU32  version;              //!< version of this structure
    NvU32  width;                //!< width of the input texture
    NvU32  height;               //!< height of the input texture
    float* blendingTexture;      //!< array of floating values building an intensity RGB texture
} NV_SCANOUT_INTENSITY_DATA_V1;

//! \ingroup gpu
typedef struct
{
    NvU32  version;              //!< version of this structure
    NvU32  width;                //!< width of the input texture
    NvU32  height;               //!< height of the input texture
    float* blendingTexture;      //!< array of floating values building an intensity RGB texture
    float* offsetTexture;        //!< array of floating values building an offset texture
    NvU32  offsetTexChannels;    //!< number of channels per pixel in the offset texture
} NV_SCANOUT_INTENSITY_DATA_V2;

typedef NV_SCANOUT_INTENSITY_DATA_V2 NV_SCANOUT_INTENSITY_DATA;

//! \ingroup gpu
#define NV_SCANOUT_INTENSITY_DATA_VER1    MAKE_NVAPI_VERSION(NV_SCANOUT_INTENSITY_DATA_V1, 1)
#define NV_SCANOUT_INTENSITY_DATA_VER2    MAKE_NVAPI_VERSION(NV_SCANOUT_INTENSITY_DATA_V2, 2)
#define NV_SCANOUT_INTENSITY_DATA_VER      NV_SCANOUT_INTENSITY_DATA_VER2

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: NvAPI_GPU_SetScanoutIntensity
//
//!   DESCRIPTION: This API enables and sets up per-pixel intensity feature on the specified display.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]   displayId              combined physical display and GPU identifier of the display to apply the intensity control.
//! \param [in]   scanoutIntensityData   the intensity texture info.
//! \param [out]  pbSticky(OUT)           indicates whether the settings will be kept over a reboot.
//!
//! \retval ::NVAPI_INVALID_ARGUMENT Invalid input parameters.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI not initialized.
//! \retval ::NVAPI_NOT_SUPPORTED Interface not supported by the driver used, or only supported on selected GPUs
//! \retval ::NVAPI_INVALID_ARGUMENT Invalid input data.
//! \retval ::NVAPI_INCOMPATIBLE_STRUCT_VERSION NV_SCANOUT_INTENSITY_DATA structure version mismatch.
//! \retval ::NVAPI_OK Feature enabled.
//! \retval ::NVAPI_ERROR Miscellaneous error occurred.
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_SetScanoutIntensity(NvU32 displayId, NV_SCANOUT_INTENSITY_DATA* scanoutIntensityData, int *pbSticky);


//! \ingroup gpu
typedef struct _NV_SCANOUT_INTENSITY_STATE_DATA
{
    NvU32  version;                                 //!< version of this structure
    NvU32  bEnabled;                                //!< intensity is enabled or not
} NV_SCANOUT_INTENSITY_STATE_DATA;

//! \ingroup gpu
#define NV_SCANOUT_INTENSITY_STATE_VER    MAKE_NVAPI_VERSION(NV_SCANOUT_INTENSITY_STATE_DATA, 1)

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: NvAPI_GPU_GetScanoutIntensityState
//
//!   DESCRIPTION: This API queries current state of the intensity feature on the specified display.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     displayId                       combined physical display and GPU identifier of the display to query the configuration.
//! \param [in,out] scanoutIntensityStateData       intensity state data.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!          specific meaning for this API, they are listed below.
//!
//! \retval ::NVAPI_INVALID_ARGUMENT Invalid input parameters.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI not initialized.
//! \retval ::NVAPI_NOT_SUPPORTED Interface not supported by the driver used, or only supported on selected GPUs.
//! \retval ::NVAPI_OK Feature enabled.
//! \retval ::NVAPI_ERROR Miscellaneous error occurred.
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetScanoutIntensityState(__in NvU32 displayId, __inout NV_SCANOUT_INTENSITY_STATE_DATA* scanoutIntensityStateData);


//! \ingroup gpu
typedef enum
{
    NV_GPU_WARPING_VERTICE_FORMAT_TRIANGLESTRIP_XYUVRQ = 0,
    NV_GPU_WARPING_VERTICE_FORMAT_TRIANGLES_XYUVRQ     = 1,
} NV_GPU_WARPING_VERTICE_FORMAT;

//! \ingroup gpu
typedef struct
{
    NvU32  version;                                 //!< version of this structure
    float* vertices;                                //!< width of the input texture
    NV_GPU_WARPING_VERTICE_FORMAT vertexFormat;     //!< format of the input vertices
    int    numVertices;                             //!< number of the input vertices
    NvSBox* textureRect;                            //!< rectangle in desktop coordinates describing the source area for the warping
} NV_SCANOUT_WARPING_DATA;

//! \ingroup gpu
#define NV_SCANOUT_WARPING_VER    MAKE_NVAPI_VERSION(NV_SCANOUT_WARPING_DATA, 1)


///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: NvAPI_GPU_SetScanoutWarping
//
//!   DESCRIPTION: This API enables and sets up the warping feature on the specified display.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]    displayId               Combined physical display and GPU identifier of the display to apply the intensity control
//! \param [in]    scanoutWarpingData      The warping data info
//! \param [out]   pbSticky                Indicates whether the settings will be kept over a reboot.
//!
//! \retval ::NVAPI_INVALID_ARGUMENT Invalid input parameters.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI not initialized.
//! \retval ::NVAPI_NOT_SUPPORTED Interface not supported by the driver used, or only supported on selected GPUs
//! \retval ::NVAPI_INVALID_ARGUMENT Invalid input data.
//! \retval ::NVAPI_INCOMPATIBLE_STRUCT_VERSION NV_SCANOUT_WARPING_DATA structure version mismatch.
//! \retval ::NVAPI_OK Feature enabled.
//! \retval ::NVAPI_ERROR Miscellaneous error occurred.
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_GPU_SetScanoutWarping(NvU32 displayId, NV_SCANOUT_WARPING_DATA* scanoutWarpingData, int* piMaxNumVertices, int* pbSticky);


//! \ingroup gpu
typedef struct _NV_SCANOUT_WARPING_STATE_DATA
{
    NvU32  version;                                  //!< version of this structure
    NvU32  bEnabled;                                 //!< warping is enabled or not
} NV_SCANOUT_WARPING_STATE_DATA;

//! \ingroup gpu
#define NV_SCANOUT_WARPING_STATE_VER    MAKE_NVAPI_VERSION(NV_SCANOUT_WARPING_STATE_DATA, 1)

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: NvAPI_GPU_GetScanoutWarpingState
//
//!   DESCRIPTION: This API queries current state of the warping feature on the specified display.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     displayId                      combined physical display and GPU identifier of the display to query the configuration.
//! \param [in,out] scanoutWarpingStateData        warping state data.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!          specific meaning for this API, they are listed below.
//!
//! \retval ::NVAPI_INVALID_ARGUMENT Invalid input parameters.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI not initialized.
//! \retval ::NVAPI_NOT_SUPPORTED Interface not supported by the driver used, or only supported on selected GPUs.
//! \retval ::NVAPI_OK Feature enabled.
//! \retval ::NVAPI_ERROR Miscellaneous error occurred.
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetScanoutWarpingState(__in NvU32 displayId, __inout NV_SCANOUT_WARPING_STATE_DATA* scanoutWarpingStateData);

typedef enum
{
    NV_GPU_SCANOUT_COMPOSITION_PARAMETER_WARPING_RESAMPLING_METHOD      = 0
} NV_GPU_SCANOUT_COMPOSITION_PARAMETER;

//! This enum defines a collection of possible scanout composition values that can be used to configure
//! possible scanout composition settings.  (Currently the only parameter defined is the WARPING_RESAMPLING_METHOD).
typedef enum
{
    NV_GPU_SCANOUT_COMPOSITION_PARAMETER_SET_TO_DEFAULT = 0,                                  // Set parameter to default value.
    // WARPING_RESAMPLING_METHOD section:
    NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE_WARPING_RESAMPLING_METHOD_BILINEAR                       = 0x100,
    NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE_WARPING_RESAMPLING_METHOD_BICUBIC_TRIANGULAR             = 0x101,
    NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE_WARPING_RESAMPLING_METHOD_BICUBIC_BELL_SHAPED            = 0x102,
    NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE_WARPING_RESAMPLING_METHOD_BICUBIC_BSPLINE                = 0x103,
    NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE_WARPING_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_TRIANGULAR    = 0x104,
    NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE_WARPING_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_BELL_SHAPED   = 0x105,
    NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE_WARPING_RESAMPLING_METHOD_BICUBIC_ADAPTIVE_BSPLINE       = 0x106
} NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE;

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: NvAPI_GPU_SetScanoutCompositionParameter
//
//!   DESCRIPTION: This API sets various parameters that configure the scanout composition feature on the specified display.
//!                (currently there is only one configurable parameter defined: WARPING_RESAMPLING_METHOD,
//!                 but this function is designed to support the addition of parameters as needed.)
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]    displayId               Combined physical display and GPU identifier of the display to apply the intensity control
//! \param [in]    parameter               The scanout composition parameter to be set
//! \param [in]    parameterValue          The data to be set for the specified parameter
//! \param [in]    pContainer              Additional container for data associated with the specified parameter
//!
//! \retval ::NVAPI_INVALID_ARGUMENT Invalid input parameters.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI not initialized.
//! \retval ::NVAPI_NOT_SUPPORTED Interface not supported by the driver used, or only supported on selected GPUs
//! \retval ::NVAPI_INVALID_ARGUMENT Invalid input data.
//! \retval ::NVAPI_OK Feature enabled.
//! \retval ::NVAPI_ERROR Miscellaneous error occurred.
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_GPU_SetScanoutCompositionParameter(NvU32 displayId, NV_GPU_SCANOUT_COMPOSITION_PARAMETER parameter,
                                                         NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE parameterValue, float *pContainer);


///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: NvAPI_GPU_GetScanoutCompositionParameter
//
//!   DESCRIPTION: This API queries current state of one of the various scanout composition parameters on the specified display.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     displayId           combined physical display and GPU identifier of the display to query the configuration.
//! \param [in]     parameter           scanout composition parameter to by queried.
//! \param [out] parameterData          scanout composition parameter data.
//! \param [out]   pContainer           Additional container for returning data associated with the specified parameter
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!          specific meaning for this API, they are listed below.
//!
//! \retval ::NVAPI_INVALID_ARGUMENT Invalid input parameters.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI not initialized.
//! \retval ::NVAPI_NOT_SUPPORTED Interface not supported by the driver used, or only supported on selected GPUs.
//! \retval ::NVAPI_OK Feature enabled.
//! \retval ::NVAPI_ERROR Miscellaneous error occurred.
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetScanoutCompositionParameter(__in NvU32 displayId, __in NV_GPU_SCANOUT_COMPOSITION_PARAMETER parameter,
                                                         __out NV_GPU_SCANOUT_COMPOSITION_PARAMETER_VALUE *parameterData, __out float *pContainer);


///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: NvAPI_GPU_GetScanoutConfiguration
//
//!   DESCRIPTION: This API queries the desktop and scanout portion of the specified display.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     displayId          combined physical display and GPU identifier of the display to query the configuration.
//! \param [in,out] desktopRect        desktop area of the display in desktop coordinates.
//! \param [in,out] scanoutRect        scanout area of the display relative to desktopRect.
//!
//! \retval ::NVAPI_INVALID_ARGUMENT Invalid input parameters.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI not initialized.
//! \retval ::NVAPI_NOT_SUPPORTED Interface not supported by the driver used, or only supported on selected GPUs.
//! \retval ::NVAPI_OK Feature enabled.
//! \retval ::NVAPI_ERROR Miscellaneous error occurred.
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetScanoutConfiguration(NvU32 displayId, NvSBox* desktopRect, NvSBox* scanoutRect);



//! \ingroup gpu
//! Used in NvAPI_GPU_GetScanoutConfigurationEx().
typedef struct _NV_SCANOUT_INFORMATION
{
    NvU32      version;                 //!< Structure version, needs to be initialized with NV_SCANOUT_INFORMATION_VER.

    NvSBox     sourceDesktopRect;       //!< Operating system display device rect in desktop coordinates displayId is scanning out from.
    NvSBox     sourceViewportRect;      //!< Area inside the sourceDesktopRect which is scanned out to the display.
    NvSBox     targetViewportRect;      //!< Area inside the rect described by targetDisplayWidth/Height sourceViewportRect is scanned out to.
    NvU32      targetDisplayWidth;      //!< Horizontal size of the active resolution scanned out to the display.
    NvU32      targetDisplayHeight;     //!< Vertical size of the active resolution scanned out to the display.
    NvU32      cloneImportance;         //!< If targets are cloned views of the sourceDesktopRect the cloned targets have an importance assigned (0:primary,1 secondary,...).
    NV_ROTATE  sourceToTargetRotation;  //!< Rotation performed between the sourceViewportRect and the targetViewportRect.
} NV_SCANOUT_INFORMATION;

#define NV_SCANOUT_INFORMATION_VER  MAKE_NVAPI_VERSION(NV_SCANOUT_INFORMATION,1)

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME: NvAPI_GPU_GetScanoutConfigurationEx
//
//!   DESCRIPTION: This API queries the desktop and scanout portion of the specified display.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//! \since Release: 331
//!
//! \param [in]     displayId            combined physical display and GPU identifier of the display to query the configuration.
//! \param [in,out] pScanoutInformation  desktop area to displayId mapping information.
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status.
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetScanoutConfigurationEx(__in NvU32 displayId, __inout NV_SCANOUT_INFORMATION *pScanoutInformation);


//! \ingroup gpu
typedef enum _NV_VIRTUALIZATION_MODE
{
    NV_VIRTUALIZATION_MODE_NONE       = 0,   //!< Represents Bare Metal GPU
    NV_VIRTUALIZATION_MODE_NMOS       = 1,   //!< Represents GPU-Passthrough
    NV_VIRTUALIZATION_MODE_VGX        = 2,   //!< Represents vGPU inside virtual machine
    NV_VIRTUALIZATION_MODE_HOST_VGPU  = 3    //!< Represents VGX hypervisor in vGPU mode
} NV_VIRTUALIZATION_MODE;

//! \ingroup gpu
typedef struct _NV_GPU_VIRTUALIZATION_INFO
{
    NvU32                    version;                      //!< Structure version

    NV_VIRTUALIZATION_MODE   virtualizationMode;           //!< one of NV_VIRTUALIZATION_MODE.
    NvU32                    reserved;                     //!< reserved for future use. Should be set to ZERO.
} NV_GPU_VIRTUALIZATION_INFO_V1;

//! \ingroup gpu
typedef NV_GPU_VIRTUALIZATION_INFO_V1     NV_GPU_VIRTUALIZATION_INFO;

//! \ingroup gpu
#define NV_GPU_VIRTUALIZATION_INFO_VER1   MAKE_NVAPI_VERSION(NV_GPU_VIRTUALIZATION_INFO_V1,1)

//! \ingroup gpu
#define NV_GPU_VIRTUALIZATION_INFO_VER    NV_GPU_VIRTUALIZATION_INFO_VER1


///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_GPU_GetVirtualizationInfo
//!
//!   DESCRIPTION: This API returns virtualization information of the GPU
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 367
//!
//! \param [in,out] pVirtualizationInfo         Pointer to NV_GPU_VIRTUALIZATION_INFO structure.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!          specific meaning for this API, they are listed below.
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetVirtualizationInfo(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_GPU_VIRTUALIZATION_INFO *pVirtualizationInfo);


//! \ingroup gpu
typedef struct _NV_LOGICAL_GPU_DATA_V1
{
    NvU32 version;                                                   //!< [in]  Structure version.
    void *pOSAdapterId;                                              //!< [out] Returns OS-AdapterId. User must send memory buffer of size atleast equal to the size of LUID structure before calling the NVAPI.
    NvU32 physicalGpuCount;                                          //!< [out] Number of physical GPU handles associated with the specified logical GPU handle.

    NvPhysicalGpuHandle physicalGpuHandles[NVAPI_MAX_PHYSICAL_GPUS]; //!< [out] This array will be filled with physical GPU handles associated with the given logical GPU handle.
                                                                     //!< The array index refers to the Physical Gpu Index (Idx).
                                                                     //!< Idx value is the same as D3D11 MultiGPUDevice GPU index, D3D12 node index, OpenGL GL_NV_gpu_multicast GPU index.
                                                                     //!< When converted to a bit mask (1 << Idx), it matches:
                                                                     //!<    1. Vulkan deviceNodeMask in VkPhysicalDeviceIDProperties
                                                                     //!<    2. CUDA deviceNodeMask returned by cuDeviceGetLuid
    NvU32 reserved[8];                                               //!< Reserved for future use. Should be set to ZERO.
} NV_LOGICAL_GPU_DATA_V1;

//! \ingroup gpu
typedef NV_LOGICAL_GPU_DATA_V1     NV_LOGICAL_GPU_DATA;
#define NV_LOGICAL_GPU_DATA_VER1   MAKE_NVAPI_VERSION(NV_LOGICAL_GPU_DATA_V1,1)
#define NV_LOGICAL_GPU_DATA_VER    NV_LOGICAL_GPU_DATA_VER1

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetLogicalGpuInfo
//
//!  This function is used to query Logical GPU information.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 421
//!
//! \param [in]    hLogicalGpu            logical GPU Handle.
//! \param [inout] pLogicalGpuData        Pointer to NV_LOGICAL_GPU_DATA structure.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!          specific meaning for this API, they are listed below.
//!
//! \ingroup gpu
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetLogicalGpuInfo(__in NvLogicalGpuHandle hLogicalGpu, __inout NV_LOGICAL_GPU_DATA *pLogicalGpuData);


//! \addtogroup gridlicense
//! @{

//! Maximum  number of supported Feature License
#define NV_LICENSE_MAX_COUNT                              3

//! Signature length for GRID License
#define NV_LICENSE_SIGNATURE_SIZE                         (128)

//! Length for Grid License
#define NV_LICENSE_INFO_MAX_LENGTH                         (128)

//! License string
typedef char NvAPI_LicenseString[NV_LICENSE_INFO_MAX_LENGTH];

//! Used in NV_LICENSE_FEATURE_DETAILS
typedef enum _NV_LICENSE_FEATURE_TYPE
{
    NV_LICENSE_FEATURE_UNKNOWN   = 0,
    NV_LICENSE_FEATURE_VGPU      = 1,
    NV_LICENSE_FEATURE_NVIDIA_RTX = 2,
    NV_LICENSE_FEATURE_QUADRO    = NV_LICENSE_FEATURE_NVIDIA_RTX, //!< DEPRECATED name - do not use
    NV_LICENSE_FEATURE_GAMING    = 3,
} NV_LICENSE_FEATURE_TYPE;

//! Used in NV_LICENSABLE_FEATURES
typedef struct _NV_LICENSE_FEATURE_DETAILS_V1
{
    NvU32                           version;                //!< IN - Structure version.
    NvU32                           isEnabled  : 1;         //!< The current state of the licensed feature, true=enabled, false=disabled.
    NvU32                           reserved   : 31;        //!< Reserved
    NV_LICENSE_FEATURE_TYPE         featureCode;            //!< Feature code that corresponds to the licensable feature.
    NvAPI_LicenseString             licenseInfo;            //!< Deprecated
}NV_LICENSE_FEATURE_DETAILS_V1;

typedef struct _NV_LICENSE_FEATURE_DETAILS_V2
{
    NvU32                           version;                //!< Unused.
    NvU32                           isEnabled  : 1;         //!< The current state of the licensed feature, true=enabled, false=disabled.
    NvU32                           reserved   : 31;        //!< Reserved
    NV_LICENSE_FEATURE_TYPE         featureCode;            //!< Feature code that corresponds to the licensable feature.
    NvAPI_LicenseString             licenseInfo;            //!< Deprecated
    NvAPI_LicenseString             productName;            //!< Nvidia Grid licensable product name.
}NV_LICENSE_FEATURE_DETAILS_V2;

typedef struct _NV_LICENSE_FEATURE_DETAILS_V3
{
    NvU32                           version;                //!< Unused.
    NvU32                           isEnabled        : 1;   //!< The current state of license, true=licensed, false=unlicensed.
    NvU32                           isFeatureEnabled : 1;   //!< The current state of feature, true=enabled, false=disabled.
    NvU32                           reserved         : 30;  //!< Reserved
    NV_LICENSE_FEATURE_TYPE         featureCode;            //!< Feature code that corresponds to the licensable feature.
    NvAPI_LicenseString             licenseInfo;            //!< Deprecated
    NvAPI_LicenseString             productName;            //!< Nvidia Grid licensable product name.
}NV_LICENSE_FEATURE_DETAILS_V3;

typedef NV_LICENSE_FEATURE_DETAILS_V1     NV_LICENSE_FEATURE_DETAILS;

#define NV_LICENSE_FEATURE_DETAILS_VER1   MAKE_NVAPI_VERSION(NV_LICENSE_FEATURE_DETAILS_V1, 1)
#define NV_LICENSE_FEATURE_DETAILS_VER    NV_LICENSE_FEATURE_DETAILS_VER1

//! Licensable features
typedef struct _NV_LICENSABLE_FEATURES_V1
{
    NvU32                              version;                                         //!< IN - Structure version.
    NvU32                              isLicenseSupported  : 1;                         //!< True if Software Licensing is supported.
    NvU32                              reserved            : 31;                        //!< Reserved
    NvU32                              licensableFeatureCount;                          //!< The number of licensable features.
    NvU8                               signature[NV_LICENSE_SIGNATURE_SIZE];            //!< Dynamic signature required for Authentication of the components,
                                                                                        //!< signature length limited to NV_LICENSE_SIGNATURE_SIZE
    NV_LICENSE_FEATURE_DETAILS_V1      licenseDetails[NV_LICENSE_MAX_COUNT];            //!< Array of licensable features
}NV_LICENSABLE_FEATURES_V1;

typedef struct _NV_LICENSABLE_FEATURES_V2
{
    NvU32                              version;                                         //!< IN - Structure version.
    NvU32                              isLicenseSupported  : 1;                         //!< True if Software Licensing is supported.
    NvU32                              reserved            : 31;                        //!< Reserved
    NvU32                              licensableFeatureCount;                          //!< The number of licensable features.
    NvU8                               signature[NV_LICENSE_SIGNATURE_SIZE];            //!< Dynamic signature required for Authentication of the components,
                                                                                        //!< signature length limited to NV_LICENSE_SIGNATURE_SIZE
    NV_LICENSE_FEATURE_DETAILS_V2      licenseDetails[NV_LICENSE_MAX_COUNT];            //!< Array of licensable features
}NV_LICENSABLE_FEATURES_V2;

typedef struct _NV_LICENSABLE_FEATURES_V3
{
    NvU32                              version;                                         //!< IN - Structure version.
    NvU32                              isLicenseSupported  : 1;                         //!< True if Software Licensing is supported.
    NvU32                              reserved            : 31;                        //!< Reserved
    NvU32                              licensableFeatureCount;                          //!< The number of licensable features.
    NvU8                               signature[NV_LICENSE_SIGNATURE_SIZE];            //!< Dynamic signature required for Authentication of the components,
                                                                                        //!< signature length limited to NV_LICENSE_SIGNATURE_SIZE
    NV_LICENSE_FEATURE_DETAILS_V3      licenseDetails[NV_LICENSE_MAX_COUNT];            //!< Array of licensable features
}NV_LICENSABLE_FEATURES_V3;

typedef NV_LICENSABLE_FEATURES_V3     NV_LICENSABLE_FEATURES;

#define NV_LICENSABLE_FEATURES_VER1   MAKE_NVAPI_VERSION(NV_LICENSABLE_FEATURES_V1, 1)
#define NV_LICENSABLE_FEATURES_VER2   MAKE_NVAPI_VERSION(NV_LICENSABLE_FEATURES_V2, 2)
#define NV_LICENSABLE_FEATURES_VER3   MAKE_NVAPI_VERSION(NV_LICENSABLE_FEATURES_V3, 3)
#define NV_LICENSABLE_FEATURES_VER    NV_LICENSABLE_FEATURES_VER3

//! @}

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GPU_GetLicensableFeatures
//
//!   DESCRIPTION:   This function call identifies whether licenses are supported on this system and if
//!                  they are supported, returns the details of the features that can be licensed.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hPhysicalGpu                   GPU selection
//! \param [in,out] pLicensableFeatures         Licensable features information.
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status.
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup gridlicense
/////////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetLicensableFeatures(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_LICENSABLE_FEATURES *pLicensableFeatures);

//! Used in NvAPI_GPU_GetPerfDecreaseInfo.
//! Bit masks for knowing the exact reason for performance decrease
typedef enum _NVAPI_GPU_PERF_DECREASE
{
    NV_GPU_PERF_DECREASE_NONE                        = 0,          //!< No Slowdown detected
    NV_GPU_PERF_DECREASE_REASON_THERMAL_PROTECTION   = 0x00000001, //!< Thermal slowdown/shutdown/POR thermal protection
    NV_GPU_PERF_DECREASE_REASON_POWER_CONTROL        = 0x00000002, //!< Power capping / pstate cap
    NV_GPU_PERF_DECREASE_REASON_AC_BATT              = 0x00000004, //!< AC->BATT event
    NV_GPU_PERF_DECREASE_REASON_API_TRIGGERED        = 0x00000008, //!< API triggered slowdown
    NV_GPU_PERF_DECREASE_REASON_INSUFFICIENT_POWER   = 0x00000010, //!< Power connector missing
    NV_GPU_PERF_DECREASE_REASON_UNKNOWN              = 0x80000000, //!< Unknown reason
} NVAPI_GPU_PERF_DECREASE;


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetPerfDecreaseInfo
//
//! DESCRIPTION:   This function retrieves - in NvU32 variable - reasons for the current performance decrease.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//! \param [in]      hPhysicalGPU    (IN)    - GPU for which performance decrease is to be evaluated.
//! \param [out]  pPerfDecrInfo    (OUT)    - Pointer to a NvU32 variable containing performance decrease info
//!
//! \return      This API can return any of the error codes enumerated in #NvAPI_Status.
//!
//! \ingroup gpuPerf
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetPerfDecreaseInfo(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NvU32 *pPerfDecrInfo);


//! \ingroup gpupstate
//! Used in NvAPI_GPU_GetPstatesInfoEx()
typedef struct
{
    NvU32   version;
    NvU32   flags;           //!< - bit 0 indicates if perfmon is enabled or not
                             //!< - bit 1 indicates if dynamic Pstate is capable or not
                             //!< - bit 2 indicates if dynamic Pstate is enable or not
                             //!< - all other bits must be set to 0
    NvU32   numPstates;      //!< The number of available p-states
    NvU32   numClocks;       //!< The number of clock domains supported by each P-State
    struct
    {
        NV_GPU_PERF_PSTATE_ID   pstateId; //!< ID of the p-state.
        NvU32                   flags;    //!< - bit 0 indicates if the PCIE limit is GEN1 or GEN2
                                          //!< - bit 1 indicates if the Pstate is overclocked or not
                                          //!< - bit 2 indicates if the Pstate is overclockable or not
                                          //!< - all other bits must be set to 0
        struct
        {
            NV_GPU_PUBLIC_CLOCK_ID           domainId;  //!< ID of the clock domain
            NvU32                               flags;  //!< Reserved. Must be set to 0
            NvU32                                freq;  //!< Clock frequency in kHz

        } clocks[NVAPI_MAX_GPU_PERF_CLOCKS];
    } pstates[NVAPI_MAX_GPU_PERF_PSTATES];

} NV_GPU_PERF_PSTATES_INFO_V1;


//! \ingroup gpupstate
typedef struct
{
    NvU32   version;
    NvU32   flags;             //!< - bit 0 indicates if perfmon is enabled or not
                               //!< - bit 1 indicates if dynamic Pstate is capable or not
                               //!< - bit 2 indicates if dynamic Pstate is enable or not
                               //!< - all other bits must be set to 0
    NvU32   numPstates;        //!< The number of available p-states
    NvU32   numClocks;         //!< The number of clock domains supported by each P-State
    NvU32   numVoltages;
    struct
    {
        NV_GPU_PERF_PSTATE_ID   pstateId;  //!< ID of the p-state.
        NvU32                   flags;     //!< - bit 0 indicates if the PCIE limit is GEN1 or GEN2
                                           //!< - bit 1 indicates if the Pstate is overclocked or not
                                           //!< - bit 2 indicates if the Pstate is overclockable or not
                                           //!< - all other bits must be set to 0
        struct
        {
            NV_GPU_PUBLIC_CLOCK_ID            domainId;
            NvU32                                flags; //!< bit 0 indicates if this clock is overclockable
                                                        //!< all other bits must be set to 0
            NvU32                                 freq;

        } clocks[NVAPI_MAX_GPU_PERF_CLOCKS];
        struct
        {
            NV_GPU_PERF_VOLTAGE_INFO_DOMAIN_ID domainId; //!< ID of the voltage domain, containing flags and mvolt info
            NvU32                       flags;           //!< Reserved for future use. Must be set to 0
            NvU32                       mvolt;           //!< Voltage in mV

        } voltages[NVAPI_MAX_GPU_PERF_VOLTAGES];

    } pstates[NVAPI_MAX_GPU_PERF_PSTATES];  //!< Valid index range is 0 to numVoltages-1

} NV_GPU_PERF_PSTATES_INFO_V2;

//! \ingroup gpupstate
typedef  NV_GPU_PERF_PSTATES_INFO_V2 NV_GPU_PERF_PSTATES_INFO;


//! \ingroup gpupstate
//! @{

//! Macro for constructing the version field of NV_GPU_PERF_PSTATES_INFO_V1
#define NV_GPU_PERF_PSTATES_INFO_VER1  MAKE_NVAPI_VERSION(NV_GPU_PERF_PSTATES_INFO_V1,1)

//! Macro for constructing the version field of NV_GPU_PERF_PSTATES_INFO_V2
#define NV_GPU_PERF_PSTATES_INFO_VER2  MAKE_NVAPI_VERSION(NV_GPU_PERF_PSTATES_INFO_V2,2)

//! Macro for constructing the version field of NV_GPU_PERF_PSTATES_INFO_V2
#define NV_GPU_PERF_PSTATES_INFO_VER3  MAKE_NVAPI_VERSION(NV_GPU_PERF_PSTATES_INFO_V2,3)

//! Macro for constructing the version field of NV_GPU_PERF_PSTATES_INFO
#define NV_GPU_PERF_PSTATES_INFO_VER   NV_GPU_PERF_PSTATES_INFO_VER3

//! @}

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GPU_GetPstatesInfoEx
//
//! DESCRIPTION:     This API retrieves all performance states (P-States) information. This is the same as
//!                  NvAPI_GPU_GetPstatesInfo(), but supports an input flag for various options.
//!
//!                  P-States are GPU active/executing performance capability and power consumption states.
//!
//!                  P-States ranges from P0 to P15, with P0 being the highest performance/power state, and
//!                  P15 being the lowest performance/power state. Each P-State, if available, maps to a
//!                  performance level. Not all P-States are available on a given system. The definitions
//!                  of each P-State are currently as follows: \n
//!                  - P0/P1 - Maximum 3D performance
//!                  - P2/P3 - Balanced 3D performance-power
//!                  - P8 - Basic HD video playback
//!                  - P10 - DVD playback
//!                  - P12 - Minimum idle power consumption
//!
//! \deprecated  Do not use this function - it is deprecated in release 304. Instead, use NvAPI_GPU_GetPstates20.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     hPhysicalGPU       GPU selection.
//! \param [out]    pPerfPstatesInfo   P-States information retrieved, as detailed below: \n
//!                  - flags is reserved for future use.
//!                  - numPstates is the number of available P-States
//!                  - numClocks is the number of clock domains supported by each P-State
//!                  - pstates has valid index range from 0 to numPstates - 1
//!                  - pstates[i].pstateId is the ID of the P-State,
//!                      containing the following info:
//!                    - pstates[i].flags containing the following info:
//!                        - bit 0 indicates if the PCIE limit is GEN1 or GEN2
//!                        - bit 1 indicates if the Pstate is overclocked or not
//!                        - bit 2 indicates if the Pstate is overclockable or not
//!                    - pstates[i].clocks has valid index range from 0 to numClocks -1
//!                    - pstates[i].clocks[j].domainId is the public ID of the clock domain,
//!                        containing the following info:
//!                      - pstates[i].clocks[j].flags containing the following info:
//!                          bit 0 indicates if the clock domain is overclockable or not
//!                      - pstates[i].clocks[j].freq is the clock frequency in kHz
//!                    - pstates[i].voltages has a valid index range from 0 to numVoltages - 1
//!                    - pstates[i].voltages[j].domainId is the ID of the voltage domain,
//!                        containing the following info:
//!                      - pstates[i].voltages[j].flags is reserved for future use.
//!                      - pstates[i].voltages[j].mvolt is the voltage in mV
//!                  inputFlags(IN)   - This can be used to select various options:
//!                    - if bit 0 is set, pPerfPstatesInfo would contain the default settings
//!                        instead of the current, possibily overclocked settings.
//!                    - if bit 1 is set, pPerfPstatesInfo would contain the maximum clock
//!                        frequencies instead of the nominal frequencies.
//!                    - if bit 2 is set, pPerfPstatesInfo would contain the minimum clock
//!                        frequencies instead of the nominal frequencies.
//!                    - all other bits must be set to 0.
//!
//! \retval ::NVAPI_OK                            Completed request
//! \retval ::NVAPI_ERROR                         Miscellaneous error occurred
//! \retval ::NVAPI_HANDLE_INVALIDATED            Handle passed has been invalidated (see user guide)
//! \retval ::NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE  Handle passed is not a physical GPU handle
//! \retval ::NVAPI_INCOMPATIBLE_STRUCT_VERSION   The version of the NV_GPU_PERF_PSTATES struct is not supported
//!
//! \ingroup gpupstate
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 304. Instead, use NvAPI_GPU_GetPstates20.")
NVAPI_INTERFACE NvAPI_GPU_GetPstatesInfoEx(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_PERF_PSTATES_INFO *pPerfPstatesInfo, NvU32 inputFlags);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GPU_GetPstates20
//
//! DESCRIPTION:    This API retrieves all performance states (P-States) 2.0 information.
//!
//!                 P-States are GPU active/executing performance capability states.
//!                 They range from P0 to P15, with P0 being the highest performance state,
//!                 and P15 being the lowest performance state. Each P-State, if available,
//!                 maps to a performance level. Not all P-States are available on a given system.
//!                 The definition of each P-States are currently as follow:
//!                 - P0/P1 - Maximum 3D performance
//!                 - P2/P3 - Balanced 3D performance-power
//!                 - P8 - Basic HD video playback
//!                 - P10 - DVD playback
//!                 - P12 - Minimum idle power consumption
//!
//! TCC_SUPPORTED
//!
//! \since Release: 295
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]   hPhysicalGPU  GPU selection
//! \param [out]  pPstatesInfo  P-States information retrieved, as documented in declaration above
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status.
//!          If there are return error codes with specific meaning for this API,
//!          they are listed below.
//!
//! \ingroup gpupstate
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetPstates20(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_GPU_PERF_PSTATES20_INFO *pPstatesInfo);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GPU_GetCurrentPstate
//
//! DESCRIPTION:     This function retrieves the current performance state (P-State).
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 165
//!
//! TCC_SUPPORTED
//!
//! \param [in]      hPhysicalGPU     GPU selection
//! \param [out]     pCurrentPstate   The ID of the current P-State of the GPU - see \ref NV_GPU_PERF_PSTATES.
//!
//! \retval    NVAPI_OK                             Completed request
//! \retval    NVAPI_ERROR                          Miscellaneous error occurred.
//! \retval    NVAPI_HANDLE_INVALIDATED             Handle passed has been invalidated (see user guide).
//! \retval    NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE   Handle passed is not a physical GPU handle.
//! \retval    NVAPI_NOT_SUPPORTED                  P-States is not supported on this setup.
//!
//! \ingroup   gpupstate
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetCurrentPstate(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_PERF_PSTATE_ID *pCurrentPstate);




//! \ingroup gpupstate
#define NVAPI_MAX_GPU_UTILIZATIONS 8



//! \ingroup gpupstate
//! Used in NvAPI_GPU_GetDynamicPstatesInfoEx().
typedef struct
{
    NvU32       version;        //!< Structure version
    NvU32       flags;          //!< bit 0 indicates if the dynamic Pstate is enabled or not
    struct
    {
        NvU32   bIsPresent:1;   //!< Set if this utilization domain is present on this GPU
        NvU32   percentage;     //!< Percentage of time where the domain is considered busy in the last 1 second interval
    } utilization[NVAPI_MAX_GPU_UTILIZATIONS];
} NV_GPU_DYNAMIC_PSTATES_INFO_EX;

//! \ingroup gpupstate
//! Macro for constructing the version field of NV_GPU_DYNAMIC_PSTATES_INFO_EX
#define NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER MAKE_NVAPI_VERSION(NV_GPU_DYNAMIC_PSTATES_INFO_EX,1)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetDynamicPstatesInfoEx
//
//! DESCRIPTION:   This API retrieves the NV_GPU_DYNAMIC_PSTATES_INFO_EX structure for the specified physical GPU.
//!                Each domain's info is indexed in the array.  For example:
//!                - pDynamicPstatesInfo->utilization[NVAPI_GPU_UTILIZATION_DOMAIN_GPU] holds the info for the GPU domain. \p
//!                There are currently 4 domains for which GPU utilization and dynamic P-State thresholds can be retrieved:
//!                   graphic engine (GPU), frame buffer (FB), video engine (VID), and bus interface (BUS).
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//!
//! TCC_SUPPORTED
//! \since Release: 185
//!
//! \retval ::NVAPI_OK
//! \retval ::NVAPI_ERROR
//! \retval ::NVAPI_INVALID_ARGUMENT  pDynamicPstatesInfo is NULL
//! \retval ::NVAPI_HANDLE_INVALIDATED
//! \retval ::NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE
//! \retval ::NVAPI_INCOMPATIBLE_STRUCT_VERSION The version of the INFO struct is not supported
//!
//! \ingroup gpupstate
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetDynamicPstatesInfoEx(NvPhysicalGpuHandle hPhysicalGpu, NV_GPU_DYNAMIC_PSTATES_INFO_EX *pDynamicPstatesInfoEx);
///////////////////////////////////////////////////////////////////////////////////
//  Thermal API
//  Provides ability to get temperature levels from the various thermal sensors associated with the GPU

//! \ingroup gputhermal
#define NVAPI_MAX_THERMAL_SENSORS_PER_GPU 3

//! \ingroup gputhermal
//! Used in NV_GPU_THERMAL_SETTINGS
typedef enum
{
    NVAPI_THERMAL_TARGET_NONE          = 0,
    NVAPI_THERMAL_TARGET_GPU           = 1,     //!< GPU core temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_MEMORY        = 2,     //!< GPU memory temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_POWER_SUPPLY  = 4,     //!< GPU power supply temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_BOARD         = 8,     //!< GPU board ambient temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_VCD_BOARD     = 9,     //!< Visual Computing Device Board temperature requires NvVisualComputingDeviceHandle
    NVAPI_THERMAL_TARGET_VCD_INLET     = 10,    //!< Visual Computing Device Inlet temperature requires NvVisualComputingDeviceHandle
    NVAPI_THERMAL_TARGET_VCD_OUTLET    = 11,    //!< Visual Computing Device Outlet temperature requires NvVisualComputingDeviceHandle

    NVAPI_THERMAL_TARGET_ALL           = 15,
    NVAPI_THERMAL_TARGET_UNKNOWN       = -1,
} NV_THERMAL_TARGET;

//! \ingroup gputhermal
//! Used in NV_GPU_THERMAL_SETTINGS
typedef enum
{
    NVAPI_THERMAL_CONTROLLER_NONE = 0,
    NVAPI_THERMAL_CONTROLLER_GPU_INTERNAL,
    NVAPI_THERMAL_CONTROLLER_ADM1032,
    NVAPI_THERMAL_CONTROLLER_MAX6649,
    NVAPI_THERMAL_CONTROLLER_MAX1617,
    NVAPI_THERMAL_CONTROLLER_LM99,
    NVAPI_THERMAL_CONTROLLER_LM89,
    NVAPI_THERMAL_CONTROLLER_LM64,
    NVAPI_THERMAL_CONTROLLER_ADT7473,
    NVAPI_THERMAL_CONTROLLER_SBMAX6649,
    NVAPI_THERMAL_CONTROLLER_VBIOSEVT,
    NVAPI_THERMAL_CONTROLLER_OS,
    NVAPI_THERMAL_CONTROLLER_UNKNOWN = -1,
} NV_THERMAL_CONTROLLER;

//! \ingroup gputhermal
//! Used in NvAPI_GPU_GetThermalSettings()
typedef struct
{
    NvU32   version;                //!< structure version
    NvU32   count;                  //!< number of associated thermal sensors
    struct
    {
        NV_THERMAL_CONTROLLER       controller;        //!< internal, ADM1032, MAX6649...
        NvU32                       defaultMinTemp;    //!< The min default temperature value of the thermal sensor in degree Celsius
        NvU32                       defaultMaxTemp;    //!< The max default temperature value of the thermal sensor in degree Celsius
        NvU32                       currentTemp;       //!< The current temperature value of the thermal sensor in degree Celsius
        NV_THERMAL_TARGET           target;            //!< Thermal sensor targeted @ GPU, memory, chipset, powersupply, Visual Computing Device, etc.
    } sensor[NVAPI_MAX_THERMAL_SENSORS_PER_GPU];

} NV_GPU_THERMAL_SETTINGS_V1;

//! \ingroup gputhermal
typedef struct
{
    NvU32   version;                //!< structure version
    NvU32   count;                  //!< number of associated thermal sensors
    struct
    {
        NV_THERMAL_CONTROLLER       controller;         //!< internal, ADM1032, MAX6649...
        NvS32                       defaultMinTemp;     //!< Minimum default temperature value of the thermal sensor in degree Celsius
        NvS32                       defaultMaxTemp;     //!< Maximum default temperature value of the thermal sensor in degree Celsius
        NvS32                       currentTemp;        //!< Current temperature value of the thermal sensor in degree Celsius
        NV_THERMAL_TARGET           target;             //!< Thermal sensor targeted - GPU, memory, chipset, powersupply, Visual Computing Device, etc
    } sensor[NVAPI_MAX_THERMAL_SENSORS_PER_GPU];

} NV_GPU_THERMAL_SETTINGS_V2;

//! \ingroup gputhermal
typedef NV_GPU_THERMAL_SETTINGS_V2  NV_GPU_THERMAL_SETTINGS;

//! \ingroup gputhermal
//! @{

//! Macro for constructing the version field of NV_GPU_THERMAL_SETTINGS_V1
#define NV_GPU_THERMAL_SETTINGS_VER_1   MAKE_NVAPI_VERSION(NV_GPU_THERMAL_SETTINGS_V1,1)

//! Macro for constructing the version field of NV_GPU_THERMAL_SETTINGS_V2
#define NV_GPU_THERMAL_SETTINGS_VER_2   MAKE_NVAPI_VERSION(NV_GPU_THERMAL_SETTINGS_V2,2)

//! Macro for constructing the version field of NV_GPU_THERMAL_SETTINGS
#define NV_GPU_THERMAL_SETTINGS_VER     NV_GPU_THERMAL_SETTINGS_VER_2
//! @}




///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GPU_GetThermalSettings
//
//!  This function retrieves the thermal information of all thermal sensors or specific thermal sensor associated with the selected GPU.
//!  Thermal sensors are indexed 0 to NVAPI_MAX_THERMAL_SENSORS_PER_GPU-1.
//!
//!  - To retrieve specific thermal sensor info, set the sensorIndex to the required thermal sensor index.
//!  - To retrieve info for all sensors, set sensorIndex to NVAPI_THERMAL_TARGET_ALL.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 85
//!
//! \param [in]   hPhysicalGPU      GPU selection.
//! \param [in]   sensorIndex       Explicit thermal sensor index selection.
//! \param [out]  pThermalSettings  Array of thermal settings.
//!
//! \retval   NVAPI_OK                           Completed request
//! \retval   NVAPI_ERROR                        Miscellaneous error occurred.
//! \retval   NVAPI_INVALID_ARGUMENT             pThermalInfo is NULL.
//! \retval   NVAPI_HANDLE_INVALIDATED           Handle passed has been invalidated (see user guide).
//! \retval   NVAPI_EXPECTED_PHYSICAL_GPU_HANDLE Handle passed is not a physical GPU handle.
//! \retval   NVAPI_INCOMPATIBLE_STRUCT_VERSION  The version of the INFO struct is not supported.
//! \ingroup gputhermal
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetThermalSettings(NvPhysicalGpuHandle hPhysicalGpu, NvU32 sensorIndex, NV_GPU_THERMAL_SETTINGS *pThermalSettings);



//! \ingroup gpuclock
//! Used in NvAPI_GPU_GetAllClockFrequencies()
typedef struct
{
    NvU32   version;    //!< Structure version
    NvU32   reserved;   //!< These bits are reserved for future use.
    struct
    {
        NvU32 bIsPresent:1;         //!< Set if this domain is present on this GPU
        NvU32 reserved:31;          //!< These bits are reserved for future use.
        NvU32 frequency;            //!< Clock frequency (kHz)
    }domain[NVAPI_MAX_GPU_PUBLIC_CLOCKS];
} NV_GPU_CLOCK_FREQUENCIES_V1;

#ifndef NV_GPU_MAX_CLOCK_FREQUENCIES
    #define NV_GPU_MAX_CLOCK_FREQUENCIES 3
#endif

//! \ingroup gpuclock
//! Used in NvAPI_GPU_GetAllClockFrequencies()
typedef enum
{
    NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ                = 0,
    NV_GPU_CLOCK_FREQUENCIES_BASE_CLOCK                  = 1,
    NV_GPU_CLOCK_FREQUENCIES_BOOST_CLOCK                 = 2,
    NV_GPU_CLOCK_FREQUENCIES_CLOCK_TYPE_NUM              = NV_GPU_MAX_CLOCK_FREQUENCIES
}   NV_GPU_CLOCK_FREQUENCIES_CLOCK_TYPE;

//! \ingroup gpuclock
//! Used in NvAPI_GPU_GetAllClockFrequencies()
typedef struct
{
    NvU32   version;        //!< Structure version
    NvU32   ClockType:4;    //!< One of NV_GPU_CLOCK_FREQUENCIES_CLOCK_TYPE. Used to specify the type of clock to be returned.
    NvU32   reserved:20;    //!< These bits are reserved for future use. Must be set to 0.
    NvU32   reserved1:8;    //!< These bits are reserved.
    struct
    {
        NvU32 bIsPresent:1;         //!< Set if this domain is present on this GPU
        NvU32 reserved:31;          //!< These bits are reserved for future use.
        NvU32 frequency;            //!< Clock frequency (kHz)
    }domain[NVAPI_MAX_GPU_PUBLIC_CLOCKS];
} NV_GPU_CLOCK_FREQUENCIES_V2;

//! \ingroup gpuclock
//! Used in NvAPI_GPU_GetAllClockFrequencies()
typedef NV_GPU_CLOCK_FREQUENCIES_V2 NV_GPU_CLOCK_FREQUENCIES;

//! \addtogroup gpuclock
//! @{
#define NV_GPU_CLOCK_FREQUENCIES_VER_1    MAKE_NVAPI_VERSION(NV_GPU_CLOCK_FREQUENCIES_V1,1)
#define NV_GPU_CLOCK_FREQUENCIES_VER_2    MAKE_NVAPI_VERSION(NV_GPU_CLOCK_FREQUENCIES_V2,2)
#define NV_GPU_CLOCK_FREQUENCIES_VER_3    MAKE_NVAPI_VERSION(NV_GPU_CLOCK_FREQUENCIES_V2,3)
#define NV_GPU_CLOCK_FREQUENCIES_VER      NV_GPU_CLOCK_FREQUENCIES_VER_3
//! @}

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GPU_GetAllClockFrequencies
//
//!   This function retrieves the NV_GPU_CLOCK_FREQUENCIES structure for the specified physical GPU.
//!
//!   For each clock domain:
//!      - bIsPresent is set for each domain that is present on the GPU
//!      - frequency is the domain's clock freq in kHz
//!
//!   Each domain's info is indexed in the array.  For example:
//!   clkFreqs.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY] holds the info for the MEMORY domain.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 295
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status.
//!          If there are return error codes with specific meaning for this API,
//!          they are listed below.
//! \retval  NVAPI_INVALID_ARGUMENT     pClkFreqs is NULL.
//! \ingroup gpuclock
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_GetAllClockFrequencies(__in NvPhysicalGpuHandle hPhysicalGPU, __inout NV_GPU_CLOCK_FREQUENCIES *pClkFreqs);


///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_GPU_QueryIlluminationSupport
//!
//! \fn NvAPI_GPU_QueryIlluminationSupport(__inout  NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM *pIlluminationSupportInfo)
//! DESCRIPTION:   This function reports if the specified illumination attribute is supported.
//!
//! \note Only a single GPU can manage an given attribute on a given HW element,
//!       regardless of how many are attatched. I.E. only one GPU will be used to control
//!       the brightness of the LED on an SLI bridge, regardless of how many are physicaly attached.
//!       You should enumerate thru the GPUs with this call to determine which GPU is managing the attribute.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 300.05
//!
//! \param [in]  hPhysicalGpu        Physical GPU handle
//! \param       Attribute           An enumeration value specifying the Illumination attribute to be querried
//! \param [out] pSupported          A boolean indicating if the attribute is supported.
//!
//! \return See \ref nvapistatus for the list of possible return values.
//
//////////////////////////////////////////////////////////////////////////////

//! \ingroup gpu
typedef enum _NV_GPU_ILLUMINATION_ATTRIB
{
    NV_GPU_IA_LOGO_BRIGHTNESS  = 0,
    NV_GPU_IA_SLI_BRIGHTNESS   = 1,
} NV_GPU_ILLUMINATION_ATTRIB;

//! \ingroup gpu
typedef struct _NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM_V1 {

    // IN
    NvU32   version;                        //!< Version of this structure
    NvPhysicalGpuHandle hPhysicalGpu;       //!< The handle of the GPU that you are checking for the specified attribute.
                                            //!< note that this is the GPU that is managing the attribute.
                                            //!< Only a single GPU can manage an given attribute on a given HW element,
                                            //!< regardless of how many are attatched.
                                            //!< I.E. only one GPU will be used to control the brightness of the LED on an SLI bridge,
                                            //!< regardless of how many are physicaly attached.
                                            //!< You enumerate thru the GPUs with this call to determine which GPU is managing the attribute.
    NV_GPU_ILLUMINATION_ATTRIB Attribute;   //!< An enumeration value specifying the Illumination attribute to be querried.
                                            //!<     refer to enum \ref NV_GPU_ILLUMINATION_ATTRIB.

    // OUT
    NvU32    bSupported;                    //!< A boolean indicating if the attribute is supported.

} NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM_V1;

//! \ingroup gpu
typedef NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM_V1      NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM;
//! \ingroup gpu
#define NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM_VER_1   MAKE_NVAPI_VERSION(NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM_V1,1)
//! \ingroup gpu
#define NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM_VER     NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM_VER_1

//! \ingroup gpu
NVAPI_INTERFACE NvAPI_GPU_QueryIlluminationSupport(__inout NV_GPU_QUERY_ILLUMINATION_SUPPORT_PARM *pIlluminationSupportInfo);




///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_GPU_GetIllumination
//!
//! \fn NvAPI_GPU_GetIllumination(NV_GPU_GET_ILLUMINATION_PARM *pIlluminationInfo)
//! DESCRIPTION:   This function reports value of the specified illumination attribute.
//!
//! \note Only a single GPU can manage an given attribute on a given HW element,
//!       regardless of how many are attatched. I.E. only one GPU will be used to control
//!       the brightness of the LED on an SLI bridge, regardless of how many are physicaly attached.
//!       You should enumerate thru the GPUs with the \ref NvAPI_GPU_QueryIlluminationSupport call to
//!       determine which GPU is managing the attribute.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 300.05
//!
//! \param [in]  hPhysicalGpu        Physical GPU handle
//! \param       Attribute           An enumeration value specifying the Illumination attribute to be querried
//! \param [out] Value               A DWORD containing the current value for the specified attribute.
//!                                  This is specified as a percentage of the full range of the attribute
//!                                  (0-100; 0 = off, 100 = full brightness)
//!
//! \return See \ref nvapistatus for the list of possible return values. Return values of special interest are:
//!             NVAPI_INVALID_ARGUMENT The specified attibute is not known to the driver.
//!             NVAPI_NOT_SUPPORTED:   The specified attribute is not supported on the specified GPU
//
//////////////////////////////////////////////////////////////////////////////

//! \ingroup gpu
typedef struct _NV_GPU_GET_ILLUMINATION_PARM_V1 {

    // IN
    NvU32   version;                        //!< Version of this structure
    NvPhysicalGpuHandle hPhysicalGpu;       //!< The handle of the GPU that you are checking for the specified attribute.
                                            //!< Note that this is the GPU that is managing the attribute.
                                            //!< Only a single GPU can manage an given attribute on a given HW element,
                                            //!< regardless of how many are attatched.
                                            //!< I.E. only one GPU will be used to control the brightness of the LED on an SLI bridge,
                                            //!< regardless of how many are physicaly attached.
                                            //!< You enumerate thru the GPUs with this call to determine which GPU is managing the attribute.
    NV_GPU_ILLUMINATION_ATTRIB Attribute;   //!< An enumeration value specifying the Illumination attribute to be querried.
                                            //!< refer to enum \ref NV_GPU_ILLUMINATION_ATTRIB.

    // OUT
    NvU32    Value;                         //!< A DWORD that will contain the current value of the specified attribute.
                                            //! This is specified as a percentage of the full range of the attribute
                                            //! (0-100; 0 = off, 100 = full brightness)

} NV_GPU_GET_ILLUMINATION_PARM_V1;

//! \ingroup gpu
typedef NV_GPU_GET_ILLUMINATION_PARM_V1      NV_GPU_GET_ILLUMINATION_PARM;
//! \ingroup gpu
#define NV_GPU_GET_ILLUMINATION_PARM_VER_1   MAKE_NVAPI_VERSION(NV_GPU_GET_ILLUMINATION_PARM_V1,1)
//! \ingroup gpu
#define NV_GPU_GET_ILLUMINATION_PARM_VER     NV_GPU_GET_ILLUMINATION_PARM_VER_1

//! \ingroup gpu
NVAPI_INTERFACE NvAPI_GPU_GetIllumination(NV_GPU_GET_ILLUMINATION_PARM *pIlluminationInfo);




///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_GPU_SetIllumination
//!
//! \fn NvAPI_GPU_SetIllumination(NV_GPU_SET_ILLUMINATION_PARM *pIlluminationInfo)
//! DESCRIPTION:   This function sets the value of the specified illumination attribute.
//!
//! \note Only a single GPU can manage an given attribute on a given HW element,
//!       regardless of how many are attatched. I.E. only one GPU will be used to control
//!       the brightness of the LED on an SLI bridge, regardless of how many are physicaly attached.
//!       You should enumerate thru the GPUs with the \ref NvAPI_GPU_QueryIlluminationSupport call to
//!       determine which GPU is managing the attribute.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 300.05
//!
//! \param [in]  hPhysicalGpu        Physical GPU handle
//! \param       Attribute           An enumeration value specifying the Illumination attribute to be set
//! \param       Value               The new value for the specified attribute.
//!                                  This should be specified as a percentage of the full range of the attribute
//!                                  (0-100; 0 = off, 100 = full brightness)
//!                                  If a value is specified outside this range, NVAPI_INVALID_ARGUMENT will be returned.
//!
//! \return See \ref nvapistatus for the list of possible return values. Return values of special interest are:
//!             NVAPI_INVALID_ARGUMENT  The specified attibute is not known to the driver, or the specified value is out of range.
//!             NVAPI_NOT_SUPPORTED     The specified attribute is not supported on the specified GPU.
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup gpu
typedef struct _NV_GPU_SET_ILLUMINATION_PARM_V1 {

    // IN
    NvU32   version;                        //!< Version of this structure
    NvPhysicalGpuHandle hPhysicalGpu;       //!< The handle of the GPU that you are checking for the specified attribute.
                                            //!< Note that this is the GPU that is managing the attribute.
                                            //!< Only a single GPU can manage an given attribute on a given HW element,
                                            //!< regardless of how many are attatched.
                                            //!< I.E. only one GPU will be used to control the brightness of the LED on an SLI bridge,
                                            //!< regardless of how many are physicaly attached.
                                            //!< You enumerate thru the GPUs with this call to determine which GPU is managing the attribute.
    NV_GPU_ILLUMINATION_ATTRIB Attribute;   //!< An enumeration value specifying the Illumination attribute to be querried.
                                            //!< refer to enum \ref NV_GPU_ILLUMINATION_ATTRIB.
    NvU32    Value;                         //!< A DWORD containing the new value for the specified attribute.
                                            //!< This should be specified as a percentage of the full range of the attribute
                                            //!< (0-100; 0 = off, 100 = full brightness)
                                            //!< If a value is specified outside this range, NVAPI_INVALID_ARGUMENT will be returned.

    // OUT

} NV_GPU_SET_ILLUMINATION_PARM_V1;

//! \ingroup gpu
typedef NV_GPU_SET_ILLUMINATION_PARM_V1      NV_GPU_SET_ILLUMINATION_PARM;
//! \ingroup gpu
#define NV_GPU_SET_ILLUMINATION_PARM_VER_1   MAKE_NVAPI_VERSION(NV_GPU_SET_ILLUMINATION_PARM_V1,1)
//! \ingroup gpu
#define NV_GPU_SET_ILLUMINATION_PARM_VER     NV_GPU_SET_ILLUMINATION_PARM_VER_1

//! \ingroup gpu
NVAPI_INTERFACE NvAPI_GPU_SetIllumination(NV_GPU_SET_ILLUMINATION_PARM *pIlluminationInfo);



/*!
 * Enumeration of control modes that can be applied to Illumination Zones.
 */
typedef enum
{
    NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGB = 0,       // deprecated
    NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_RGB, // deprecated

    NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL = 0,        
    NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR,

    // Strictly add new control modes above this.
    NV_GPU_CLIENT_ILLUM_CTRL_MODE_INVALID = 0xFF,
} NV_GPU_CLIENT_ILLUM_CTRL_MODE;

/*!
 * Enumeration of locations where an Illumination Zone might be present.
 * Encoding used -
 *   1:0 - Number specifier (0)
 *   4:2 - Location (TOP)
 *   7:5 - Type (GPU/SLI)
 */
typedef enum
{
    NV_GPU_CLIENT_ILLUM_ZONE_LOCATION_GPU_TOP_0   = 0x00,
    NV_GPU_CLIENT_ILLUM_ZONE_LOCATION_GPU_FRONT_0 = 0x08,
    NV_GPU_CLIENT_ILLUM_ZONE_LOCATION_GPU_BACK_0  = 0x0C,
    NV_GPU_CLIENT_ILLUM_ZONE_LOCATION_SLI_TOP_0   = 0x20,
    NV_GPU_CLIENT_ILLUM_ZONE_LOCATION_INVALID     = 0xFFFFFFFF,
} NV_GPU_CLIENT_ILLUM_ZONE_LOCATION;

/*!
 * Enumeration of ILLUM_DEVICEs.
 */
typedef enum
{
    NV_GPU_CLIENT_ILLUM_DEVICE_TYPE_INVALID = 0,
    NV_GPU_CLIENT_ILLUM_DEVICE_TYPE_MCUV10,
    NV_GPU_CLIENT_ILLUM_DEVICE_TYPE_GPIO_PWM_RGBW_V10,
    NV_GPU_CLIENT_ILLUM_DEVICE_TYPE_GPIO_PWM_SINGLE_COLOR_V10,
} NV_GPU_CLIENT_ILLUM_DEVICE_TYPE;

/*!
 * Enumeration of ILLUM_ZONEs.
 */
typedef enum
{
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE_INVALID = 0,
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB,
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED,
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGBW,
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE_SINGLE_COLOR,
} NV_GPU_CLIENT_ILLUM_ZONE_TYPE;

/*!
 * Number of color points for the piecewise linear control mode.
 */
#define NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_COLOR_ENDPOINTS           2

/*!
 * Enumeration of Cycle types for piecewise linear control mode.
 */
typedef enum
{
    NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_HALF_HALT = 0,
    NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_FULL_HALT,
    NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_FULL_REPEAT,
    NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_INVALID = 0xFF,
} NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_TYPE;

#define NV_GPU_CLIENT_ILLUM_DEVICE_NUM_DEVICES_MAX 32
 
/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_DEVICE_INFO_V1
 * Describes the static information of illumination device type MCUV10.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_DEVICE_INFO_DATA_MCUV10
{
    /*!
     * I2C Device Index: Pointing to the illumination device in I2C Devices Table.
     */
    NvU8 i2cDevIdx;
} NV_GPU_CLIENT_ILLUM_DEVICE_INFO_DATA_MCUV10;

/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_DEVICE_INFO_V1
 * Describes the static information of illum device type GPIO_PWM_RGBW.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_DEVICE_INFO_DATA_GPIO_PWM_RGBW
{
    /*!
     * Red drive GPIO pin.
     */
    NvU8 gpioPinRed;

    /*!
     * Green drive GPIO pin.
     */
    NvU8 gpioPinGreen;

    /*!
     * Blue drive GPIO pin.
     */
    NvU8 gpioPinBlue;

    /*!
     * White drive GPIO pin.
     */
    NvU8 gpioPinWhite;
} NV_GPU_CLIENT_ILLUM_DEVICE_INFO_DATA_GPIO_PWM_RGBW;
/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_DEVICE_INFO_V1
 * Describes the static information of illum device type GPIO_PWM_SINGLE_COLOR.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_DEVICE_INFO_DATA_GPIO_PWM_SINGLE_COLOR
{
    /*!
     * Single Color GPIO pin.
     */
    NvU8 gpioPinSingleColor;
} NV_GPU_CLIENT_ILLUM_DEVICE_INFO_DATA_GPIO_PWM_SINGLE_COLOR;

/*!
 */
typedef struct _NV_GPU_CLIENT_ILLUM_DEVICE_INFO_V1
{
    /*!
     * Type of the illumination device.
     */
    NV_GPU_CLIENT_ILLUM_DEVICE_TYPE    type;
 
    /*!
     * Supported control modes for this illumination device.
     */
    NvU32                              ctrlModeMask;
 
    /*!
     * Union of illumination device info data. Interpreted as per
     * @ref NV_GPU_CLIENT_ILLUM_DEVICE_INFO_V1::type
     */
    union
    {
        //
        // Need to be careful when add/expanding types in this union. If any type
        // exceeds sizeof(rsvd) then rsvd has failed its purpose.
        //
        NV_GPU_CLIENT_ILLUM_DEVICE_INFO_DATA_MCUV10                 mcuv10;
        NV_GPU_CLIENT_ILLUM_DEVICE_INFO_DATA_GPIO_PWM_RGBW          gpioPwmRgbwv10;
        NV_GPU_CLIENT_ILLUM_DEVICE_INFO_DATA_GPIO_PWM_SINGLE_COLOR  gpioPwmSingleColorv10;

        /*!
         * Reserved bytes for possible future extension of this struct.
         */
        NvU8                                           rsvd[64];
    } data;
 
    /*!
     * Reserved for future.
     */
    NvU8    rsvd[64];
} NV_GPU_CLIENT_ILLUM_DEVICE_INFO_V1;
 
/*!
 */
typedef struct _NV_GPU_CLIENT_ILLUM_DEVICE_INFO_PARAMS_V1
{
    /*!
     * Version of structure. Must always be first member.
     */
    NvU32       version;

    /*!
     * Number of illumination devices present.
     */
    NvU32       numIllumDevices;

    /*!
     * Reserved bytes for possible future extension of this struct.
     */
    NvU8        rsvd[64];
 
    /*!
     */
    NV_GPU_CLIENT_ILLUM_DEVICE_INFO_V1 devices[NV_GPU_CLIENT_ILLUM_DEVICE_NUM_DEVICES_MAX];
} NV_GPU_CLIENT_ILLUM_DEVICE_INFO_PARAMS_V1;
 
#define NV_GPU_CLIENT_ILLUM_DEVICE_INFO_PARAMS_VER_1 MAKE_NVAPI_VERSION(NV_GPU_CLIENT_ILLUM_DEVICE_INFO_PARAMS_V1, 1)
#define NV_GPU_CLIENT_ILLUM_DEVICE_INFO_PARAMS_VER   NV_GPU_CLIENT_ILLUM_DEVICE_INFO_PARAMS_VER_1
typedef NV_GPU_CLIENT_ILLUM_DEVICE_INFO_PARAMS_V1    NV_GPU_CLIENT_ILLUM_DEVICE_INFO_PARAMS;
 
///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_GPU_ClientIllumDevicesGetInfo
//!
//! DESCRIPTION:   This API returns static information about illumination devices on the
//!                given GPU.
//
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 400
//! \param [in]  hPhysicalGpu       The physical GPU handle
//! \param [out] pIllumDevicesInfo  Pointer to structure containing static
//!                                 information about illumination devices.
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status.
//!          If there are return error codes with specific meaning for this API,
//!          they are listed below.
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_ClientIllumDevicesGetInfo(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_GPU_CLIENT_ILLUM_DEVICE_INFO_PARAMS *pIllumDevicesInfo);


/*!
 * Structure representing the data required for synchronization.
 */
typedef struct
{
    /*!
     * Boolean representing the need for synchronization.
     */
    NvBool bSync;

    /*!
     * Time stamp value required for synchronization.
     */
    NvU64  timeStampms;

    /*!
     * Reserved for future.
     */
    NvU8    rsvd[64];
} NV_GPU_CLIENT_ILLUM_DEVICE_SYNC_V1;

/*!
 * Structure representing the device control parameters of each ILLUM_DEVICE.
 */
typedef struct
{
    /*!
     * Type of the illum device.
     */
    NV_GPU_CLIENT_ILLUM_DEVICE_TYPE    type;

    /*!
     * Structure containing the synchronization data for the illumination device.
     */
    NV_GPU_CLIENT_ILLUM_DEVICE_SYNC_V1    syncData;

    /*!
     * Reserved for future.
     */
    NvU8    rsvd[64];
} NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_V1;

typedef NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_V1    NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL;

/*!
 * Structure representing the control parameters of ILLUM_DEVICE-s.
 */
typedef struct
{
    /*!
     * Version of structure. Must always be first member.
     */
    NvU32   version;

    /*!
     * Number of illumination devices present.
     */
    NvU32   numIllumDevices;

    /*!
     * Reserved bytes for possible future extension of this struct.
     */
    NvU8    rsvd[64];

    /*!
     */
    NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_V1 devices[NV_GPU_CLIENT_ILLUM_DEVICE_NUM_DEVICES_MAX];
} NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_PARAMS_V1;

#define NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_PARAMS_VER_1 MAKE_NVAPI_VERSION(NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_PARAMS_V1, 1)
#define NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_PARAMS_VER   NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_PARAMS_VER_1
typedef NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_PARAMS_V1    NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_PARAMS;

///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_GPU_ClientIllumDevicesGetControl
//!
//! DESCRIPTION:   This API gets control parameters about illumination devices on the
//!                given GPU.
//
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 400
//! \param [in]  hPhysicalGpu          The physical GPU handle
//! \param [inout] pIllumDevicesControl  Pointer to structure containing control
//!                                 information about illum devices.
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status.
//!          If there are return error codes with specific meaning for this API,
//!          they are listed below.
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_ClientIllumDevicesGetControl(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_PARAMS *pClientIllumDevicesControl);

///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_GPU_ClientIllumDevicesSetControl
//!
//! DESCRIPTION:   This API sets control parameters about illumination devices on the
//!                given GPU.
//
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 400
//! \param [in]  hPhysicalGpu          The physical GPU handle
//! \param [inout] pClientIllumDevicesControl  Pointer to structure containing control
//!                                 information about illum devices.
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status.
//!          If there are return error codes with specific meaning for this API,
//!          they are listed below.
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_ClientIllumDevicesSetControl(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_GPU_CLIENT_ILLUM_DEVICE_CONTROL_PARAMS *pClientIllumDevicesControl);

 
#define NV_GPU_CLIENT_ILLUM_ZONE_NUM_ZONES_MAX 32
 
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_INFO_DATA_RGB
{
    NvU8 rsvd;
} NV_GPU_CLIENT_ILLUM_ZONE_INFO_DATA_RGB;

/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_ZONE_INFO_V1
 * Describes the static information of illum zone type RGBW.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_INFO_DATA_RGBW
{
    NvU8 rsvd;
} NV_GPU_CLIENT_ILLUM_ZONE_INFO_DATA_RGBW;

/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_ZONE_INFO_V1
 * Describes the static information of illum zone type SINGLE_COLOR.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_INFO_DATA_SINGLE_COLOR
{
    NvU8 rsvd;
} NV_GPU_CLIENT_ILLUM_ZONE_INFO_DATA_SINGLE_COLOR;

typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_INFO_V1
{
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE      type;

    /*!
     * Index pointing to an Illumination Device that controls this zone.
     */
    NvU8                               illumDeviceIdx;

    /*!
     * Provider index for representing logical to physical zone mapping.
     */
    NvU8                                provIdx;

    /*!
     * Location of the zone on the board.
     */
    NV_GPU_CLIENT_ILLUM_ZONE_LOCATION  zoneLocation;

    union
    {
        //
        // Need to be careful when add/expanding types in this union. If any type
        // exceeds sizeof(rsvd) then rsvd has failed its purpose.
        //
        NV_GPU_CLIENT_ILLUM_ZONE_INFO_DATA_RGB          rgb;
        NV_GPU_CLIENT_ILLUM_ZONE_INFO_DATA_RGBW         rgbw;
        NV_GPU_CLIENT_ILLUM_ZONE_INFO_DATA_SINGLE_COLOR singleColor;

        /*!
         * Reserved bytes for possible future extension of this struct.
         */
        NvU8                                      rsvd[64];
    } data;
 
    NvU8    rsvd[64];
} NV_GPU_CLIENT_ILLUM_ZONE_INFO_V1;
 
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_INFO_PARAMS_V1
{
    /*!
     * Version of structure. Must always be first member.
     */
    NvU32   version;

    /*!
     * Number of illumination zones present.
     */
    NvU32   numIllumZones;

    /*!
     * Reserved bytes for possible future extension of this struct.
     */
    NvU8    rsvd[64];
    NV_GPU_CLIENT_ILLUM_ZONE_INFO_V1 zones[NV_GPU_CLIENT_ILLUM_ZONE_NUM_ZONES_MAX];
} NV_GPU_CLIENT_ILLUM_ZONE_INFO_PARAMS_V1;
 
#define NV_GPU_CLIENT_ILLUM_ZONE_INFO_PARAMS_VER_1 MAKE_NVAPI_VERSION(NV_GPU_CLIENT_ILLUM_ZONE_INFO_PARAMS_V1, 1)
#define NV_GPU_CLIENT_ILLUM_ZONE_INFO_PARAMS_VER   NV_GPU_CLIENT_ILLUM_ZONE_INFO_PARAMS_VER_1
typedef NV_GPU_CLIENT_ILLUM_ZONE_INFO_PARAMS_V1    NV_GPU_CLIENT_ILLUM_ZONE_INFO_PARAMS;
 
///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_GPU_ClientIllumZonesGetInfo
//!
//! DESCRIPTION:   This API returns static information about illumination zones on the
//!                given GPU.
//
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 400
//! \param [in]  hPhysicalGpu     The physical GPU handle
//! \param [out] pIllumZonesInfo  Pointer to structure containing static
//!                               information about illumination devices.
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status.
//!          If there are return error codes with specific meaning for this API,
//!          they are listed below.
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_ClientIllumZonesGetInfo(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_GPU_CLIENT_ILLUM_ZONE_INFO_PARAMS *pIllumZonesInfo);
 

/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB
 * Parameters required to represent control mode of type
 * \ref NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGB.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB_PARAMS
{
    /*!
     * Red compenent of color applied to the zone.
     */
    NvU8 colorR;

    /*!
     * Green compenent of color applied to the zone.
     */
    NvU8 colorG;

    /*!
     * Blue compenent of color applied to the zone.
     */
    NvU8 colorB;

    /*!
     * Brightness perecentage value of the zone.
     */
    NvU8 brightnessPct;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB_PARAMS;

/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGB
 * Data required to represent control mode of type
 * \ref NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGB.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB
{
    /*!
     * Parameters required to represent control mode of type
     * \ref NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGB.
     */
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB_PARAMS rgbParams;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB;

/*!
 * Used in \ref NV_GPU_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_RGB
 * Data required to represent control mode of type
 * \ref NV_GPU_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_RGB.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR
{
    /*!
     * Type of cycle effect to apply.
     */
    NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_TYPE    cycleType;

    /*!
     * Number of times to repeat function within group period.
     */
    NvU8    grpCount;

    /*!
     * Time in ms to transition from color A to color B.
     */
    NvU16   riseTimems;

    /*!
     * Time in ms to transition from color B to color A.
     */
    NvU16   fallTimems;

    /*!
     * Time in ms to remain at color A before color A to color B transition.
     */
    NvU16   ATimems;

    /*!
     * Time in ms to remain at color B before color B to color A transition.
     */
    NvU16   BTimems;

    /*!
     * Time in ms to remain idle before next group of repeated function cycles.
     */
    NvU16   grpIdleTimems;

    /*!
     * Time in ms to offset the cycle relative to other zones.
     */
    NvU16   phaseOffsetms;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR;

/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGB
 * Data required to represent control mode of type
 * \ref NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_RGB.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_RGB
{
    /*!
     * Parameters required to represent control mode of type
     * \ref NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_RGB.
     */
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB_PARAMS rgbParams[NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_COLOR_ENDPOINTS];

    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR  piecewiseLinearData;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_RGB;

/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_V1
 * Describes the control data for illumination zone of type
 * \ref NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGB
{
    /*!
     * Union of illumination zone control data for zone of type NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB.
     * Interpreted as per ctrlMode.
     */
    union
    {
        //
        // Need to be careful when add/expanding types in this union. If any type
        // exceeds sizeof(rsvd) then rsvd has failed its purpose.
        //
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB            manualRGB;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_RGB  piecewiseLinearRGB;

        /*!
         * Reserved bytes for possible future extension of this struct.
         */
        NvU8                                         rsvd[64];
    } data;

    /*!
     * Reserved for future.
     */
    NvU8    rsvd[64];
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGB;

/*!
 * Used in \ref NV_GPU_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED
 * Parameters required to represent control mode of type
 * \ref NV_GPU_ILLUM_CTRL_MODE_MANUAL_RGB.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED_PARAMS
{
    /*!
     * Brightness percentage value of the zone.
     */
    NvU8 brightnessPct;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED_PARAMS;

/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_COLOR_FIXED
 * Data required to represent control mode of type
 * \ref NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGB.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED
{
    /*!
     * Parameters required to represent control mode of type
     * \ref NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGB.
     */
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED_PARAMS colorFixedParams;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED;

/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_COLOR_FIXED
 * Data required to represent control mode of type
 * \ref NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_RGB.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_COLOR_FIXED
{
    /*!
     * Parameters required to represent control mode of type
     * \ref NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_RGB.
     */
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED_PARAMS colorFixedParams[NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_COLOR_ENDPOINTS];

    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR          piecewiseLinearData;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_COLOR_FIXED;

/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_V1
 * Describes the control data for illum zone of type
 * \ref NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_COLOR_FIXED
{
    /*!
     * Union of illum zone control data for zone of type NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED.
     * Interpreted as per ctrlMode.
     */
    union
    {
        //
        // Need to be careful when add/expanding types in this union. If any type
        // exceeds sizeof(rsvd) then rsvd has failed its purpose.
        //
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED           manualColorFixed;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_COLOR_FIXED piecewiseLinearColorFixed;
        /*!
         * Reserved bytes for possible future extension of this struct.
         */
        NvU8                                         rsvd[64];
    } data;

    /*!
     * Reserved for future.
     */
    NvU8    rsvd[64];
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_COLOR_FIXED;

/*!
 * Used in \ref NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGBW
 * Parameters required to represent control mode of type
 * \ref NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGBW.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGBW_PARAMS
{
    /*!
     * Red component of color applied to the zone.
     */
    NvU8 colorR;
  
    /*!
     * Green component of color applied to the zone.
     */
    NvU8 colorG;
  
    /*!
     * Blue component of color applied to the zone.
     */
    NvU8 colorB;
  
    /*!
     * White component of color applied to the zone.
     */
    NvU8 colorW;
  
    /*!
     * Brightness percentage value of the zone.
     */
    NvU8 brightnessPct;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGBW_PARAMS;

/*!
 * Used in \ref NV_GPU_ILLUM_ZONE_CONTROL_DATA_RGBW
 * Data required to represent control mode of type
 * \ref NV_GPU_ILLUM_CTRL_MODE_MANUAL_RGBW.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGBW
{
    /*!
     * Parameters required to represent control mode of type
     * \ref NV_GPU_ILLUM_CTRL_MODE_MANUAL_RGBW.
     */
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGBW_PARAMS rgbwParams;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGBW;

/*!
 * Used in \ref NV_GPU_ILLUM_ZONE_CONTROL_DATA_RGBW
 * Data required to represent control mode of type
 * \ref NV_GPU_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_RGBW.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_RGBW
{
    /*!
     * Parameters required to represent control mode of type
     * \ref NV_GPU_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_RGBW.
     */
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGBW_PARAMS rgbwParams[NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_COLOR_ENDPOINTS];
  
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR  piecewiseLinearData;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_RGBW;

/*!
 * Used in \ref NV_GPU_ILLUM_ZONE_CONTROL_V1
 * Describes the control data for illum zone of type
 * \ref NV_GPU_ILLUM_ZONE_TYPE_RGBW.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGBW
{
    /*!
     * Union of illum zone control data for zone of type NV_GPU_ILLUM_ZONE_TYPE_RGBW.
     * Interpreted as per ctrlMode.
     */
    union
    {
        //
        // Need to be careful when add/expanding types in this union. If any type
        // exceeds sizeof(rsvd) then rsvd has failed its purpose.
        //
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGBW           manualRGBW;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_RGBW piecewiseLinearRGBW;
        /*!
         * Reserved bytes for possible future extension of this struct.
         */
        NvU8                                         rsvd[64];
    } data;
  
    /*!
     * Reserved for future.
     */
    NvU8    rsvd[64];
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGBW;

/*!
 * Used in \ref NV_GPU_ILLUM_ZONE_CONTROL_DATA_MANUAL_SINGLE_COLOR
 * Parameters required to represent control mode of type
 * \ref NV_GPU_ILLUM_CTRL_MODE_MANUAL_SINGLE_COLOR.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_SINGLE_COLOR_PARAMS
{
    /*!
     * Brightness percentage value of the zone.
     */
    NvU8 brightnessPct;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_SINGLE_COLOR_PARAMS;

/*!
 * Used in \ref NV_GPU_ILLUM_ZONE_CONTROL_DATA_SINGLE_COLOR
 * Data required to represent control mode of type
 * \ref NV_GPU_ILLUM_CTRL_MODE_MANUAL_SINGLE_COLOR.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_SINGLE_COLOR
{
    /*!
     * Parameters required to represent control mode of type
     * \ref NV_GPU_ILLUM_CTRL_MODE_MANUAL_SINGLE_COLOR.
     */
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_SINGLE_COLOR_PARAMS singleColorParams;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_SINGLE_COLOR;

/*!
 * Used in \ref NV_GPU_ILLUM_ZONE_CONTROL_DATA_SINGLE_COLOR
 * Data required to represent control mode of type
 * \ref NV_GPU_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_SINGLE_COLOR.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_SINGLE_COLOR
{
    /*!
     * Parameters required to represent control mode of type
     * \ref NV_GPU_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_SINGLE_COLOR.
     */
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_SINGLE_COLOR_PARAMS singleColorParams[NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_COLOR_ENDPOINTS];
  
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR  piecewiseLinearData;
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_SINGLE_COLOR;

/*!
 * Used in \ref NV_GPU_ILLUM_ZONE_CONTROL_V1
 * Describes the control data for illum zone of type
 * \ref NV_GPU_ILLUM_ZONE_TYPE_SINGLE_COLOR.
 */
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_SINGLE_COLOR
{
    /*!
     * Union of illum zone control data for zone of type NV_GPU_ILLUM_ZONE_TYPE_SINGLE_COLOR.
     * Interpreted as per ctrlMode.
     */
    union
    {
        //
        // Need to be careful when add/expanding types in this union. If any type
        // exceeds sizeof(rsvd) then rsvd has failed its purpose.
        //
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_SINGLE_COLOR           manualSingleColor;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_SINGLE_COLOR piecewiseLinearSingleColor;
        /*!
         * Reserved bytes for possible future extension of this struct.
         */
        NvU8                                         rsvd[64];
    } data;
  
    /*!
     * Reserved for future.
     */
    NvU8    rsvd[64];
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_SINGLE_COLOR;

typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_V1
{
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE  type;
    NV_GPU_CLIENT_ILLUM_CTRL_MODE  ctrlMode;
    union
    {
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGB           rgb;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_COLOR_FIXED   colorFixed;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGBW          rgbw;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_SINGLE_COLOR  singleColor;        
        NvU8                                                rsvd[64];
    } data;
    NvU8    rsvd[64];
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_V1;
 
typedef struct _NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS_V1
{
    NvU32                          version;
 
    /*!
     * Bit field specifying the set of values to retrieve or set
     * - default (NV_TRUE)
     * - currently active (NV_FALSE).
     */
    NvU32                          bDefault : 1;
    NvU32                          rsvdField : 31;

    /*!
     * Number of illumination zones present.
     */
    NvU32   numIllumZonesControl;

    /*!
     * Reserved bytes for possible future extension of this struct.
     */
    NvU8                           rsvd[64];
  
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_V1 zones[NV_GPU_CLIENT_ILLUM_ZONE_NUM_ZONES_MAX];
} NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS_V1;
 
#define NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS_VER_1 MAKE_NVAPI_VERSION(NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS_V1, 1)
#define NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS_VER   NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS_VER_1
typedef NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS_V1    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS;
 
///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_GPU_ClientIllumZonesGetControl
//!
//! DESCRIPTION:    Accessor for control information about illumination zones on the
//!                 given GPU.
//
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 400
//! \param [in]  hPhysicalGpu        The physical GPU handle
//! \param [out] pIllumZonesControl  Pointer to structure containing control
//!                                  information about illumination zones.
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status.
//!          If there are return error codes with specific meaning for this API,
//!          they are listed below.
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_ClientIllumZonesGetControl(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS *pIllumZonesControl);
 
///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_GPU_ClientIllumZonesSetControl
//!
//! DESCRIPTION:   Mutator for control information about illumination zones on the
//!                given GPU.
//
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! TCC_SUPPORTED
//!
//! \since Release: 400
//! \param [in]  hPhysicalGpu        The physical GPU handle
//! \param [out] pIllumZonesControl  Pointer to structure containing control
//!                                  information about illumination zones.
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status.
//!          If there are return error codes with specific meaning for this API,
//!          they are listed below.
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GPU_ClientIllumZonesSetControl(__in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS *pIllumZonesControl);
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_EnumNvidiaDisplayHandle
//
//! This function returns the handle of the NVIDIA display specified by the enum
//!                index (thisEnum). The client should keep enumerating until it
//!                returns error.
//!
//!                Note: Display handles can get invalidated on a modeset, so the calling applications need to
//!                renum the handles after every modeset.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \param [in]  thisEnum      The index of the NVIDIA display.
//! \param [out] pNvDispHandle Pointer to the NVIDIA display handle.
//!
//! \retval NVAPI_INVALID_ARGUMENT        Either the handle pointer is NULL or enum index too big
//! \retval NVAPI_OK                      Return a valid NvDisplayHandle based on the enum index
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND No NVIDIA device found in the system
//! \retval NVAPI_END_ENUMERATION         No more display device to enumerate
//! \ingroup disphandle
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_EnumNvidiaDisplayHandle(NvU32 thisEnum, NvDisplayHandle *pNvDispHandle);





///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_EnumNvidiaUnAttachedDisplayHandle
//
//! This function returns the handle of the NVIDIA unattached display specified by the enum
//!                index (thisEnum). The client should keep enumerating until it
//!                returns error.
//!                Note: Display handles can get invalidated on a modeset, so the calling applications need to
//!                renum the handles after every modeset.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \param [in]  thisEnum                  The index of the NVIDIA display.
//! \param [out] pNvUnAttachedDispHandle   Pointer to the NVIDIA display handle of the unattached display.
//!
//! \retval NVAPI_INVALID_ARGUMENT         Either the handle pointer is NULL or enum index too big
//! \retval NVAPI_OK                       Return a valid NvDisplayHandle based on the enum index
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND  No NVIDIA device found in the system
//! \retval NVAPI_END_ENUMERATION          No more display device to enumerate.
//! \ingroup disphandle
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_EnumNvidiaUnAttachedDisplayHandle(NvU32 thisEnum, NvUnAttachedDisplayHandle *pNvUnAttachedDispHandle);



///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_CreateDisplayFromUnAttachedDisplay
//
//! This function converts the unattached display handle to an active attached display handle.
//!
//! At least one GPU must be present in the system and running an NVIDIA display driver.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \retval NVAPI_INVALID_ARGUMENT         hNvUnAttachedDisp is not valid or pNvDisplay is NULL.
//! \retval NVAPI_OK                       One or more handles were returned
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND  No NVIDIA GPU driving a display was found
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_CreateDisplayFromUnAttachedDisplay(NvUnAttachedDisplayHandle hNvUnAttachedDisp, NvDisplayHandle *pNvDisplay);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetAssociatedNVidiaDisplayHandle
//
//!  This function returns the handle of the NVIDIA display that is associated
//!  with the given display "name" (such as "\\.\DISPLAY1").
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \retval NVAPI_INVALID_ARGUMENT         Either argument is NULL
//! \retval NVAPI_OK                      *pNvDispHandle is now valid
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND  No NVIDIA device maps to that display name
//! \ingroup disphandle
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetAssociatedNvidiaDisplayHandle(const char *szDisplayName, NvDisplayHandle *pNvDispHandle);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DISP_GetAssociatedUnAttachedNvidiaDisplayHandle
//
//!   DESCRIPTION: This function returns the handle of an unattached NVIDIA display that is
//!                associated with the given display name (such as "\\DISPLAY1").
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 185
//!
//! \retval ::NVAPI_INVALID_ARGUMENT          Either argument is NULL.
//! \retval ::NVAPI_OK                       *pNvUnAttachedDispHandle is now valid.
//! \retval ::NVAPI_NVIDIA_DEVICE_NOT_FOUND   No NVIDIA device maps to that display name.
//!
//! \ingroup disphandle
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DISP_GetAssociatedUnAttachedNvidiaDisplayHandle(const char *szDisplayName, NvUnAttachedDisplayHandle *pNvUnAttachedDispHandle);



///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetAssociatedNVidiaDisplayName
//
//!  For a given NVIDIA display handle, this function returns a string (such as "\\.\DISPLAY1") to identify the display.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \retval NVAPI_INVALID_ARGUMENT          Either argument is NULL
//! \retval NVAPI_OK                       *pNvDispHandle is now valid
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND   No NVIDIA device maps to that display name
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetAssociatedNvidiaDisplayName(NvDisplayHandle NvDispHandle, NvAPI_ShortString szDisplayName);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetUnAttachedAssociatedDisplayName
//
//!  This function returns the display name given, for example, "\\DISPLAY1", using the unattached NVIDIA display handle
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 95
//!
//! \retval NVAPI_INVALID_ARGUMENT          Either argument is NULL
//! \retval NVAPI_OK                       *pNvDispHandle is now valid
//! \retval NVAPI_NVIDIA_DEVICE_NOT_FOUND   No NVIDIA device maps to that display name
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetUnAttachedAssociatedDisplayName(NvUnAttachedDisplayHandle hNvUnAttachedDisp, NvAPI_ShortString szDisplayName);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_EnableHWCursor
//
//!  This function enables hardware cursor support
//!
//! SUPPORTED OS:  Windows XP
//!
//!
//!
//! \since Release: 80
//!
//! \return NVAPI_ERROR or NVAPI_OK
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_EnableHWCursor(NvDisplayHandle hNvDisplay);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DisableHWCursor
//
//! This function disables hardware cursor support
//!
//! SUPPORTED OS:  Windows XP
//!
//!
//! \since Release: 80
//!
//! \return  NVAPI_ERROR or NVAPI_OK
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DisableHWCursor(NvDisplayHandle hNvDisplay);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetVBlankCounter
//
//!  This function gets the V-blank counter
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 80
//!
//! \return NVAPI_ERROR or NVAPI_OK
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetVBlankCounter(NvDisplayHandle hNvDisplay, NvU32 *pCounter);

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:  NvAPI_SetRefreshRateOverride
//
//!  This function overrides the refresh rate on the given display/outputsMask.
//!  The new refresh rate can be applied right away in this API call or deferred to be applied with the
//!  next OS modeset. The override is good for only one modeset (regardless whether it's deferred or immediate).
//!
//!
//! SUPPORTED OS:  Windows XP
//!
//!
//! \since Release: 80
//!
//!  \param [in] hNvDisplay    The NVIDIA display handle. It can be NVAPI_DEFAULT_HANDLE or a handle
//!                           enumerated from NvAPI_EnumNVidiaDisplayHandle().
//!  \param [in] outputsMask  A set of bits that identify all target outputs which are associated with the NVIDIA
//!                           display handle to apply the refresh rate override. When SLI is enabled, the
//!                           outputsMask only applies to the GPU that is driving the display output.
//!  \param [in] refreshRate  The override value. "0.0" means cancel the override.
//!  \param [in] bSetDeferred
//!              - "0": Apply the refresh rate override immediately in this API call.\p
//!              - "1": Apply refresh rate at the next OS modeset.
//!
//!  \retval  NVAPI_INVALID_ARGUMENT hNvDisplay or outputsMask is invalid
//!  \retval  NVAPI_OK               The refresh rate override is correct set
//!  \retval  NVAPI_ERROR            The operation failed
//!  \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_SetRefreshRateOverride(NvDisplayHandle hNvDisplay, NvU32 outputsMask, float refreshRate, NvU32 bSetDeferred);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetAssociatedDisplayOutputId
//
//! This function gets the active outputId associated with the display handle.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 90
//!
//! \param [in]  hNvDisplay  NVIDIA Display selection. It can be NVAPI_DEFAULT_HANDLE or a handle enumerated from NvAPI_EnumNVidiaDisplayHandle().
//! \param [out] outputId    The active display output ID associated with the selected display handle hNvDisplay.
//!                          The outputid will have only one bit set. In the case of Clone or Span mode, this will indicate the
//!                          display outputId of the primary display that the GPU is driving. See \ref handles.
//!
//! \retval  NVAPI_OK                      Call successful.
//! \retval  NVAPI_NVIDIA_DEVICE_NOT_FOUND No NVIDIA GPU driving a display was found.
//! \retval  NVAPI_EXPECTED_DISPLAY_HANDLE hNvDisplay is not a valid display handle.
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetAssociatedDisplayOutputId(NvDisplayHandle hNvDisplay, NvU32 *pOutputId);


//! \ingroup dispcontrol
//! Used in NvAPI_GetDisplayPortInfo().
typedef struct _NV_DISPLAY_PORT_INFO_V1
{
    NvU32               version;                                  //!< Structure version
    NvU32               dpcd_ver;                                 //!< DPCD version of the monitor
    NV_DP_LINK_RATE     maxLinkRate;                              //!< Maximum supported link rate
    NV_DP_LANE_COUNT    maxLaneCount;                             //!< Maximum supported lane count
    NV_DP_LINK_RATE     curLinkRate;                              //!< Current link rate
    NV_DP_LANE_COUNT    curLaneCount;                             //!< Current lane count
    NV_DP_COLOR_FORMAT  colorFormat;                              //!< Current color format
    NV_DP_DYNAMIC_RANGE dynamicRange;                             //!< Dynamic range
    NV_DP_COLORIMETRY   colorimetry;                              //!< Ignored in RGB space
    NV_DP_BPC           bpc;                                      //!< Current bit-per-component
    NvU32               isDp                                : 1;  //!< If the monitor is driven by a DisplayPort
    NvU32               isInternalDp                        : 1;  //!< If the monitor is driven by an NV Dp transmitter
    NvU32               isColorCtrlSupported                : 1;  //!< If the color format change is supported
    NvU32               is6BPCSupported                     : 1;  //!< If 6 bpc is supported
    NvU32               is8BPCSupported                     : 1;  //!< If 8 bpc is supported
    NvU32               is10BPCSupported                    : 1;  //!< If 10 bpc is supported
    NvU32               is12BPCSupported                    : 1;  //!< If 12 bpc is supported
    NvU32               is16BPCSupported                    : 1;  //!< If 16 bpc is supported
    NvU32               isYCrCb420Supported                 : 1;  //!< If YCrCb420 is supported
    NvU32               isYCrCb422Supported                 : 1;  //!< If YCrCb422 is supported
    NvU32               isYCrCb444Supported                 : 1;  //!< If YCrCb444 is supported
    NvU32               isRgb444SupportedOnCurrentMode      : 1;  //!< If Rgb444 is supported on the current mode
    NvU32               isYCbCr444SupportedOnCurrentMode    : 1;  //!< If YCbCr444 is supported on the current mode
    NvU32               isYCbCr422SupportedOnCurrentMode    : 1;  //!< If YCbCr422 is supported on the current mode
    NvU32               isYCbCr420SupportedOnCurrentMode    : 1;  //!< If YCbCr420 is supported on the current mode
    NvU32               is6BPCSupportedOnCurrentMode        : 1;  // if 6 bpc is supported On Current Mode
    NvU32               is8BPCSupportedOnCurrentMode        : 1;  // if 8 bpc is supported On Current Mode
    NvU32               is10BPCSupportedOnCurrentMode       : 1;  // if 10 bpc is supported On Current Mode
    NvU32               is12BPCSupportedOnCurrentMode       : 1;  // if 12 bpc is supported On Current Mode
    NvU32               is16BPCSupportedOnCurrentMode       : 1;  // if 16 bpc is supported On Current Mode
    NvU32               isMonxvYCC601Capable                : 1;  // if xvYCC 601 extended colorimetry is supported
    NvU32               isMonxvYCC709Capable                : 1;  // if xvYCC 709 extended colorimetry is supported
    NvU32               isMonsYCC601Capable                 : 1;  // if sYCC601 extended colorimetry is supported
    NvU32               isMonAdobeYCC601Capable             : 1;  // if AdobeYCC601 extended colorimetry is supported
    NvU32               isMonAdobeRGBCapable                : 1;  // if AdobeRGB extended colorimetry is supported
    NvU32               isMonBT2020RGBCapable               : 1;  // if BT2020 RGB extended colorimetry is supported
    NvU32               isMonBT2020YCCCapable               : 1;  // if BT2020 Y'CbCr extended colorimetry is supported
    NvU32               isMonBT2020cYCCCapable              : 1;  // if BT2020 cYCbCr (constant luminance) extended colorimetry is supported

    NvU32               reserved                            : 4;  //!< reserved
 } NV_DISPLAY_PORT_INFO_V1;

 typedef NV_DISPLAY_PORT_INFO_V1 NV_DISPLAY_PORT_INFO;

//! Macro for constructing the version field of NV_DISPLAY_PORT_INFO.
#define NV_DISPLAY_PORT_INFO_VER1  MAKE_NVAPI_VERSION(NV_DISPLAY_PORT_INFO,1)
#define NV_DISPLAY_PORT_INFO_VER2  MAKE_NVAPI_VERSION(NV_DISPLAY_PORT_INFO,2)
#define NV_DISPLAY_PORT_INFO_VER   NV_DISPLAY_PORT_INFO_VER2

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_GetDisplayPortInfo
//
//! \fn NvAPI_GetDisplayPortInfo(__in_opt NvDisplayHandle hNvDisplay, __in NvU32 outputId, __inout NV_DISPLAY_PORT_INFO *pInfo)
//! DESCRIPTION:     This function returns the current DisplayPort-related information on the specified device (monitor).
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 165
//!
//! \param [in]     hvDisplay     NVIDIA Display selection. It can be NVAPI_DEFAULT_HANDLE or a handle enumerated from NvAPI_EnumNVidiaDisplayHandle().
//!                               This parameter is ignored when the outputId is a NvAPI displayId.
//! \param [in]     outputId      This can either be the connection bit mask or the NvAPI displayId. When the legacy connection bit mask is passed,
//!                               it should have exactly 1 bit set to indicate a single display. If it's "0" then the default outputId from
//!                               NvAPI_GetAssociatedDisplayOutputId() will be used. See \ref handles.
//! \param [out]    pInfo         The DisplayPort information
//!
//! \retval         NVAPI_OK                Completed request
//! \retval         NVAPI_ERROR             Miscellaneous error occurred
//! \retval         NVAPI_INVALID_ARGUMENT  Invalid input parameter.
//
///////////////////////////////////////////////////////////////////////////////
//! \ingroup        dispcontrol
NVAPI_INTERFACE NvAPI_GetDisplayPortInfo(__in_opt NvDisplayHandle hNvDisplay, __in NvU32 outputId, __inout NV_DISPLAY_PORT_INFO *pInfo);

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_SetDisplayPort
//
//! \fn NvAPI_SetDisplayPort(NvDisplayHandle hNvDisplay, NvU32 outputId, NV_DISPLAY_PORT_CONFIG *pCfg)
//! DESCRIPTION:     This function sets up DisplayPort-related configurations.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release:   165
//!
//! \param [in]       hNvDisplay  NVIDIA display handle. It can be NVAPI_DEFAULT_HANDLE or a handle enumerated from
//!                               NvAPI_EnumNVidiaDisplayHandle().
//! \param [in]       outputId    This display output ID, when it's "0" it means the default outputId generated from the return of
//!                               NvAPI_GetAssociatedDisplayOutputId(). See \ref handles.
//! \param [in]       pCfg        The display port config structure. If pCfg is NULL, it means to use the driver's default value to setup.
//!
//! \retval           NVAPI_OK                Completed request
//! \retval           NVAPI_ERROR             Miscellaneous error occurred
//! \retval           NVAPI_INVALID_ARGUMENT  Invalid input parameter
///////////////////////////////////////////////////////////////////////////////


//! \ingroup dispcontrol
//! DisplayPort configuration settings - used in NvAPI_SetDisplayPort().
typedef struct
{
    NvU32               version;                     //!< Structure version - 2 is the latest
    NV_DP_LINK_RATE     linkRate;                    //!< Link rate
    NV_DP_LANE_COUNT    laneCount;                   //!< Lane count
    NV_DP_COLOR_FORMAT  colorFormat;                 //!< Color format to set
    NV_DP_DYNAMIC_RANGE dynamicRange;                //!< Dynamic range
    NV_DP_COLORIMETRY   colorimetry;                 //!< Ignored in RGB space
    NV_DP_BPC           bpc;                         //!< Bit-per-component
    NvU32               isHPD               : 1;     //!< If the control panel is making this call due to HPD
    NvU32               isSetDeferred       : 1;     //!< Requires an OS modeset to finalize the setup if set
    NvU32               isChromaLpfOff      : 1;     //!< Force the chroma low_pass_filter to be off
    NvU32               isDitherOff         : 1;     //!< Force to turn off dither
    NvU32               testLinkTrain       : 1;     //!< If testing mode, skip validation
    NvU32               testColorChange     : 1;     //!< If testing mode, skip validation

} NV_DISPLAY_PORT_CONFIG;

//! \addtogroup dispcontrol
//! @{
//! Macro for constructing the version field of NV_DISPLAY_PORT_CONFIG
#define NV_DISPLAY_PORT_CONFIG_VER   MAKE_NVAPI_VERSION(NV_DISPLAY_PORT_CONFIG,2)
//! Macro for constructing the version field of NV_DISPLAY_PORT_CONFIG
#define NV_DISPLAY_PORT_CONFIG_VER_1 MAKE_NVAPI_VERSION(NV_DISPLAY_PORT_CONFIG,1)
//! Macro for constructing the version field of NV_DISPLAY_PORT_CONFIG
#define NV_DISPLAY_PORT_CONFIG_VER_2 MAKE_NVAPI_VERSION(NV_DISPLAY_PORT_CONFIG,2)
//! @}


//! \ingroup          dispcontrol
NVAPI_INTERFACE NvAPI_SetDisplayPort(NvDisplayHandle hNvDisplay, NvU32 outputId, NV_DISPLAY_PORT_CONFIG *pCfg);




//! \ingroup dispcontrol
//! Used in NvAPI_GetHDMISupportInfo().
typedef struct _NV_HDMI_SUPPORT_INFO_V1
{
    NvU32      version;                     //!< Structure version

    NvU32      isGpuHDMICapable       : 1;  //!< If the GPU can handle HDMI
    NvU32      isMonUnderscanCapable  : 1;  //!< If the monitor supports underscan
    NvU32      isMonBasicAudioCapable : 1;  //!< If the monitor supports basic audio
    NvU32      isMonYCbCr444Capable   : 1;  //!< If YCbCr 4:4:4 is supported
    NvU32      isMonYCbCr422Capable   : 1;  //!< If YCbCr 4:2:2 is supported
    NvU32      isMonxvYCC601Capable   : 1;  //!< If xvYCC 601 is supported
    NvU32      isMonxvYCC709Capable   : 1;  //!< If xvYCC 709 is supported
    NvU32      isMonHDMI              : 1;  //!< If the monitor is HDMI (with IEEE's HDMI registry ID)
    NvU32      reserved               : 24; //!< Reserved.

    NvU32      EDID861ExtRev;               //!< Revision number of the EDID 861 extension
 } NV_HDMI_SUPPORT_INFO_V1;

typedef struct _NV_HDMI_SUPPORT_INFO_V2
{
    NvU32      version;                      //!< Structure version

    NvU32      isGpuHDMICapable        : 1;  //!< If the GPU can handle HDMI
    NvU32      isMonUnderscanCapable   : 1;  //!< If the monitor supports underscan
    NvU32      isMonBasicAudioCapable  : 1;  //!< If the monitor supports basic audio
    NvU32      isMonYCbCr444Capable    : 1;  //!< If YCbCr 4:4:4 is supported
    NvU32      isMonYCbCr422Capable    : 1;  //!< If YCbCr 4:2:2 is supported
    NvU32      isMonxvYCC601Capable    : 1;  //!< If xvYCC extended colorimetry 601 is supported
    NvU32      isMonxvYCC709Capable    : 1;  //!< If xvYCC extended colorimetry 709 is supported
    NvU32      isMonHDMI               : 1;  //!< If the monitor is HDMI (with IEEE's HDMI registry ID)
    NvU32      isMonsYCC601Capable     : 1;  //!< if sYCC601 extended colorimetry is supported
    NvU32      isMonAdobeYCC601Capable : 1;  //!< if AdobeYCC601 extended colorimetry is supported
    NvU32      isMonAdobeRGBCapable    : 1;  //!< if AdobeRGB extended colorimetry is supported
    NvU32      reserved                : 21; //!< Reserved.

    NvU32      EDID861ExtRev;                //!< Revision number of the EDID 861 extension
 } NV_HDMI_SUPPORT_INFO_V2;

#define NV_HDMI_SUPPORT_INFO_VER1  MAKE_NVAPI_VERSION(NV_HDMI_SUPPORT_INFO_V1, 1)
#define NV_HDMI_SUPPORT_INFO_VER2  MAKE_NVAPI_VERSION(NV_HDMI_SUPPORT_INFO_V2, 2)



#ifndef NV_HDMI_SUPPORT_INFO_VER

typedef NV_HDMI_SUPPORT_INFO_V2    NV_HDMI_SUPPORT_INFO;
#define NV_HDMI_SUPPORT_INFO_VER   NV_HDMI_SUPPORT_INFO_VER2

#endif


//! SUPPORTED OS:  Windows 7 and higher
//!
///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_GetHDMISupportInfo
//
//! \fn NvAPI_GetHDMISupportInfo(__in_opt NvDisplayHandle hNvDisplay, __in NvU32 outputId, __inout NV_HDMI_SUPPORT_INFO *pInfo)
//!   This API returns the current infoframe data on the specified device(monitor).
//!
//! \since Release: 95
//!
//! \param [in]  hvDisplay  NVIDIA Display selection. It can be NVAPI_DEFAULT_HANDLE or a handle enumerated from NvAPI_EnumNVidiaDisplayHandle().
//!                         This parameter is ignored when the outputId is a NvAPI displayId.
//! \param [in]  outputId   This can either be the connection bit mask or the NvAPI displayId. When the legacy connection bit mask is passed,
//!                         it should have exactly 1 bit set to indicate a single display. If it's "0" then the default outputId from
//!                         NvAPI_GetAssociatedDisplayOutputId() will be used. See \ref handles.
//! \param [out] pInfo      The monitor and GPU's HDMI support info
//!
//! \retval  NVAPI_OK                Completed request
//! \retval  NVAPI_ERROR             Miscellaneous error occurred
//! \retval  NVAPI_INVALID_ARGUMENT  Invalid input parameter.
///////////////////////////////////////////////////////////////////////////////


//! \ingroup dispcontrol
NVAPI_INTERFACE NvAPI_GetHDMISupportInfo(__in_opt NvDisplayHandle hNvDisplay, __in NvU32 outputId, __inout NV_HDMI_SUPPORT_INFO *pInfo);


//! \ingroup dispcontrol

typedef enum
{
    NV_INFOFRAME_CMD_GET_DEFAULT = 0,     //!< Returns the fields in the infoframe with values set by the manufacturer - NVIDIA/OEM.
    NV_INFOFRAME_CMD_RESET,               //!< Sets the fields in the infoframe to auto, and infoframe to the default infoframe for use in a set.
    NV_INFOFRAME_CMD_GET,                 //!< Get the current infoframe state.
    NV_INFOFRAME_CMD_SET,                 //!< Set the current infoframe state (flushed to the monitor), the values are one time and do not persist.
    NV_INFOFRAME_CMD_GET_OVERRIDE,        //!< Get the override infoframe state, non-override fields will be set to value = AUTO, overridden fields will have the current override values.
    NV_INFOFRAME_CMD_SET_OVERRIDE,        //!< Set the override infoframe state, non-override fields will be set to value = AUTO, other values indicate override; persist across modeset/reboot
    NV_INFOFRAME_CMD_GET_PROPERTY,        //!< get properties associated with infoframe (each of the infoframe type will have properties)
    NV_INFOFRAME_CMD_SET_PROPERTY,        //!< set properties associated with infoframe
} NV_INFOFRAME_CMD;


typedef enum
{
    NV_INFOFRAME_PROPERTY_MODE_AUTO           = 0, //!< Driver determines whether to send infoframes.
    NV_INFOFRAME_PROPERTY_MODE_ENABLE,             //!< Driver always sends infoframe.
    NV_INFOFRAME_PROPERTY_MODE_DISABLE,            //!< Driver never sends infoframe.
    NV_INFOFRAME_PROPERTY_MODE_ALLOW_OVERRIDE,     //!< Driver only sends infoframe when client requests it via infoframe escape call.
} NV_INFOFRAME_PROPERTY_MODE;


//! Returns whether the current monitor is in blacklist or force this monitor to be in blacklist.
typedef enum
{
    NV_INFOFRAME_PROPERTY_BLACKLIST_FALSE = 0,
    NV_INFOFRAME_PROPERTY_BLACKLIST_TRUE,
} NV_INFOFRAME_PROPERTY_BLACKLIST;

typedef struct
{
    NvU32 mode      :  4;
    NvU32 blackList :  2;
    NvU32 reserved  : 10;
    NvU32 version   :  8;
    NvU32 length    :  8;
} NV_INFOFRAME_PROPERTY;

//! Byte1 related
typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_SCANINFO_NODATA    = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_SCANINFO_OVERSCAN,
    NV_INFOFRAME_FIELD_VALUE_AVI_SCANINFO_UNDERSCAN,
    NV_INFOFRAME_FIELD_VALUE_AVI_SCANINFO_FUTURE,
    NV_INFOFRAME_FIELD_VALUE_AVI_SCANINFO_AUTO      = 7
} NV_INFOFRAME_FIELD_VALUE_AVI_SCANINFO;


typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_BARDATA_NOT_PRESENT         = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_BARDATA_VERTICAL_PRESENT,
    NV_INFOFRAME_FIELD_VALUE_AVI_BARDATA_HORIZONTAL_PRESENT,
    NV_INFOFRAME_FIELD_VALUE_AVI_BARDATA_BOTH_PRESENT,
    NV_INFOFRAME_FIELD_VALUE_AVI_BARDATA_AUTO                = 7
} NV_INFOFRAME_FIELD_VALUE_AVI_BARDATA;

typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_AFI_ABSENT   = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_AFI_PRESENT,
    NV_INFOFRAME_FIELD_VALUE_AVI_AFI_AUTO     = 3
} NV_INFOFRAME_FIELD_VALUE_AVI_ACTIVEFORMATINFO;


typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_COLORFORMAT_RGB      = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_COLORFORMAT_YCbCr422,
    NV_INFOFRAME_FIELD_VALUE_AVI_COLORFORMAT_YCbCr444,
    NV_INFOFRAME_FIELD_VALUE_AVI_COLORFORMAT_FUTURE,
    NV_INFOFRAME_FIELD_VALUE_AVI_COLORFORMAT_AUTO     = 7
} NV_INFOFRAME_FIELD_VALUE_AVI_COLORFORMAT;

typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_F17_FALSE = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_F17_TRUE,
    NV_INFOFRAME_FIELD_VALUE_AVI_F17_AUTO = 3
} NV_INFOFRAME_FIELD_VALUE_AVI_F17;

//! Byte2 related
typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_NO_AFD           = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_RESERVE01,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_RESERVE02,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_RESERVE03,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_LETTERBOX_GT16x9,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_RESERVE05,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_RESERVE06,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_RESERVE07,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_EQUAL_CODEDFRAME = 8,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_CENTER_4x3,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_CENTER_16x9,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_CENTER_14x9,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_RESERVE12,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_4x3_ON_14x9,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_16x9_ON_14x9,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_16x9_ON_4x3,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION_AUTO             = 31,
} NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOACTIVEPORTION;


typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOCODEDFRAME_NO_DATA = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOCODEDFRAME_4x3,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOCODEDFRAME_16x9,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOCODEDFRAME_FUTURE,
    NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOCODEDFRAME_AUTO    = 7
} NV_INFOFRAME_FIELD_VALUE_AVI_ASPECTRATIOCODEDFRAME;

typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_COLORIMETRY_NO_DATA                   = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_COLORIMETRY_SMPTE_170M,
    NV_INFOFRAME_FIELD_VALUE_AVI_COLORIMETRY_ITUR_BT709,
    NV_INFOFRAME_FIELD_VALUE_AVI_COLORIMETRY_USE_EXTENDED_COLORIMETRY,
    NV_INFOFRAME_FIELD_VALUE_AVI_COLORIMETRY_AUTO                      = 7
} NV_INFOFRAME_FIELD_VALUE_AVI_COLORIMETRY;

//! Byte 3 related
typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_NONUNIFORMPICTURESCALING_NO_DATA    = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_NONUNIFORMPICTURESCALING_HORIZONTAL,
    NV_INFOFRAME_FIELD_VALUE_AVI_NONUNIFORMPICTURESCALING_VERTICAL,
    NV_INFOFRAME_FIELD_VALUE_AVI_NONUNIFORMPICTURESCALING_BOTH,
    NV_INFOFRAME_FIELD_VALUE_AVI_NONUNIFORMPICTURESCALING_AUTO       = 7
} NV_INFOFRAME_FIELD_VALUE_AVI_NONUNIFORMPICTURESCALING;

typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_RGBQUANTIZATION_DEFAULT       = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_RGBQUANTIZATION_LIMITED_RANGE,
    NV_INFOFRAME_FIELD_VALUE_AVI_RGBQUANTIZATION_FULL_RANGE,
    NV_INFOFRAME_FIELD_VALUE_AVI_RGBQUANTIZATION_RESERVED,
    NV_INFOFRAME_FIELD_VALUE_AVI_RGBQUANTIZATION_AUTO          = 7
} NV_INFOFRAME_FIELD_VALUE_AVI_RGBQUANTIZATION;

typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_EXTENDEDCOLORIMETRY_XVYCC601     = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_EXTENDEDCOLORIMETRY_XVYCC709,
    NV_INFOFRAME_FIELD_VALUE_AVI_EXTENDEDCOLORIMETRY_SYCC601,
    NV_INFOFRAME_FIELD_VALUE_AVI_EXTENDEDCOLORIMETRY_ADOBEYCC601,
    NV_INFOFRAME_FIELD_VALUE_AVI_EXTENDEDCOLORIMETRY_ADOBERGB,
    NV_INFOFRAME_FIELD_VALUE_AVI_EXTENDEDCOLORIMETRY_RESERVED05,
    NV_INFOFRAME_FIELD_VALUE_AVI_EXTENDEDCOLORIMETRY_RESERVED06,
    NV_INFOFRAME_FIELD_VALUE_AVI_EXTENDEDCOLORIMETRY_RESERVED07,
    NV_INFOFRAME_FIELD_VALUE_AVI_EXTENDEDCOLORIMETRY_AUTO         = 15
} NV_INFOFRAME_FIELD_VALUE_AVI_EXTENDEDCOLORIMETRY;

typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_ITC_VIDEO_CONTENT = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_ITC_ITCONTENT,
    NV_INFOFRAME_FIELD_VALUE_AVI_ITC_AUTO          = 3
} NV_INFOFRAME_FIELD_VALUE_AVI_ITC;

//! Byte 4 related
typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_NONE = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_X02,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_X03,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_X04,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_X05,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_X06,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_X07,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_X08,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_X09,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_X10,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_RESERVED10,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_RESERVED11,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_RESERVED12,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_RESERVED13,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_RESERVED14,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_RESERVED15,
    NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION_AUTO         = 31
} NV_INFOFRAME_FIELD_VALUE_AVI_PIXELREPETITION;


typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_CONTENTTYPE_GRAPHICS = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_CONTENTTYPE_PHOTO,
    NV_INFOFRAME_FIELD_VALUE_AVI_CONTENTTYPE_CINEMA,
    NV_INFOFRAME_FIELD_VALUE_AVI_CONTENTTYPE_GAME,
    NV_INFOFRAME_FIELD_VALUE_AVI_CONTENTTYPE_AUTO     = 7
} NV_INFOFRAME_FIELD_VALUE_AVI_CONTENTTYPE;

typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AVI_YCCQUANTIZATION_LIMITED_RANGE = 0,
    NV_INFOFRAME_FIELD_VALUE_AVI_YCCQUANTIZATION_FULL_RANGE,
    NV_INFOFRAME_FIELD_VALUE_AVI_YCCQUANTIZATION_RESERVED02,
    NV_INFOFRAME_FIELD_VALUE_AVI_YCCQUANTIZATION_RESERVED03,
    NV_INFOFRAME_FIELD_VALUE_AVI_YCCQUANTIZATION_AUTO          = 7
} NV_INFOFRAME_FIELD_VALUE_AVI_YCCQUANTIZATION;

//! Adding an Auto bit to each field
typedef struct
{
    NvU32 vic                     : 8;
    NvU32 pixelRepeat             : 5;
    NvU32 colorSpace              : 3;
    NvU32 colorimetry             : 3;
    NvU32 extendedColorimetry     : 4;
    NvU32 rgbQuantizationRange    : 3;
    NvU32 yccQuantizationRange    : 3;
    NvU32 itContent               : 2;
    NvU32 contentTypes            : 3;
    NvU32 scanInfo                : 3;
    NvU32 activeFormatInfoPresent : 2;
    NvU32 activeFormatAspectRatio : 5;
    NvU32 picAspectRatio          : 3;
    NvU32 nonuniformScaling       : 3;
    NvU32 barInfo                 : 3;
    NvU32 top_bar                 : 17;
    NvU32 bottom_bar              : 17;
    NvU32 left_bar                : 17;
    NvU32 right_bar               : 17;
    NvU32 Future17                : 2;
    NvU32 Future47                : 2;
} NV_INFOFRAME_VIDEO;

//! Byte 1 related
typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELCOUNT_IN_HEADER = 0,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELCOUNT_2,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELCOUNT_3,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELCOUNT_4,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELCOUNT_5,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELCOUNT_6,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELCOUNT_7,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELCOUNT_8,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELCOUNT_AUTO      = 15
} NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELCOUNT;

typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_IN_HEADER                  = 0,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_PCM,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_AC3,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_MPEG1,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_MP3,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_MPEG2,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_AACLC,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_DTS,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_ATRAC,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_DSD,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_EAC3,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_DTSHD,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_MLP,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_DST,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_WMAPRO,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_USE_CODING_EXTENSION_TYPE,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE_AUTO                      = 31
} NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGTYPE;

//! Byte 2 related
typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLESIZE_IN_HEADER = 0,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLESIZE_16BITS,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLESIZE_20BITS,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLESIZE_24BITS,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLESIZE_AUTO      = 7
} NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLESIZE;

typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLEFREQUENCY_IN_HEADER = 0,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLEFREQUENCY_32000HZ,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLEFREQUENCY_44100HZ,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLEFREQUENCY_48000HZ,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLEFREQUENCY_88200KHZ,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLEFREQUENCY_96000KHZ,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLEFREQUENCY_176400KHZ,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLEFREQUENCY_192000KHZ,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLEFREQUENCY_AUTO      = 15
} NV_INFOFRAME_FIELD_VALUE_AUDIO_SAMPLEFREQUENCY;



//! Byte 3 related
typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_USE_CODING_TYPE = 0,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_HEAAC,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_HEAACV2,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_MPEGSURROUND,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE04,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE05,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE06,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE07,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE08,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE09,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE10,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE11,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE12,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE13,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE14,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE15,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE16,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE17,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE18,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE19,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE20,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE21,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE22,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE23,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE24,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE25,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE26,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE27,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE28,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE29,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE30,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_RESERVE31,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE_AUTO           = 63
} NV_INFOFRAME_FIELD_VALUE_AUDIO_CODINGEXTENSIONTYPE;


//! Byte 4 related
typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_X_X_X_X_FR_FL           =0,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_X_X_X_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_X_X_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_X_X_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_X_RC_X_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_X_RC_X_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_X_RC_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_X_RC_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_RR_RL_X_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_RR_RL_X_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_RR_RL_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_X_RR_RL_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_RC_RR_RL_X_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_RC_RR_RL_X_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_RC_RR_RL_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_RC_RR_RL_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_RRC_RLC_RR_RL_X_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_RRC_RLC_RR_RL_X_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_RRC_RLC_RR_RL_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_RRC_RLC_RR_RL_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_X_X_X_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_X_X_X_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_X_X_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_X_X_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_X_RC_X_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_X_RC_X_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_X_RC_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_X_RC_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_RR_RL_X_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_RR_RL_X_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_RR_RL_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRC_FLC_RR_RL_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_FCH_RR_RL_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_X_FCH_RR_RL_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_TC_X_RR_RL_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_TC_X_RR_RL_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRH_FLH_RR_RL_X_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRH_FLH_RR_RL_X_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRW_FLW_RR_RL_X_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRW_FLW_RR_RL_X_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_TC_RC_RR_RL_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_TC_RC_RR_RL_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FCH_RC_RR_RL_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FCH_RC_RR_RL_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_TC_FCH_RR_RL_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_TC_FCH_RR_RL_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRH_FLH_RR_RL_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRH_FLH_RR_RL_FC_LFE_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRW_FLW_RR_RL_FC_X_FR_FL,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_FRW_FLW_RR_RL_FC_LFE_FR_FL  = 0X31,
    // all other values should default to auto
    NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION_AUTO                        = 0x1FF
} NV_INFOFRAME_FIELD_VALUE_AUDIO_CHANNELALLOCATION;

//! Byte 5 related
typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LFEPLAYBACKLEVEL_NO_DATA    = 0,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LFEPLAYBACKLEVEL_0DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LFEPLAYBACKLEVEL_PLUS10DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LFEPLAYBACKLEVEL_RESERVED03,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LFEPLAYBACKLEVEL_AUTO       = 7
} NV_INFOFRAME_FIELD_VALUE_AUDIO_LFEPLAYBACKLEVEL;

typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_0DB  = 0,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_1DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_2DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_3DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_4DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_5DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_6DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_7DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_8DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_9DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_10DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_11DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_12DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_13DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_14DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_15DB,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES_AUTO = 31
} NV_INFOFRAME_FIELD_VALUE_AUDIO_LEVELSHIFTVALUES;


typedef enum
{
    NV_INFOFRAME_FIELD_VALUE_AUDIO_DOWNMIX_PERMITTED  = 0,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_DOWNMIX_PROHIBITED,
    NV_INFOFRAME_FIELD_VALUE_AUDIO_DOWNMIX_AUTO       = 3
} NV_INFOFRAME_FIELD_VALUE_AUDIO_DOWNMIX;

typedef struct
{
    NvU32 codingType          : 5;
    NvU32 codingExtensionType : 6;
    NvU32 sampleSize          : 3;
    NvU32 sampleRate          : 4;
    NvU32 channelCount        : 4;
    NvU32 speakerPlacement    : 9;
    NvU32 downmixInhibit      : 2;
    NvU32 lfePlaybackLevel    : 3;
    NvU32 levelShift          : 5;
    NvU32 Future12            : 2;
    NvU32 Future2x            : 4;
    NvU32 Future3x            : 4;
    NvU32 Future52            : 2;
    NvU32 Future6             : 9;
    NvU32 Future7             : 9;
    NvU32 Future8             : 9;
    NvU32 Future9             : 9;
    NvU32 Future10            : 9;
} NV_INFOFRAME_AUDIO;

typedef struct
{
    NvU32 version; //!< version of this structure
    NvU16 size;    //!< size of this structure
    NvU8  cmd;     //!< The actions to perform from NV_INFOFRAME_CMD
    NvU8  type;    //!< type of infoframe

    union
    {
        NV_INFOFRAME_PROPERTY     property;  //!< This is NVIDIA-specific and corresponds to the property cmds and associated infoframe.
        NV_INFOFRAME_AUDIO        audio;
        NV_INFOFRAME_VIDEO        video;
    } infoframe;
} NV_INFOFRAME_DATA;

//! Macro for constructing the version field of ::NV_INFOFRAME_DATA
#define NV_INFOFRAME_DATA_VER   MAKE_NVAPI_VERSION(NV_INFOFRAME_DATA,1)

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_Disp_InfoFrameControl
//
//! DESCRIPTION:     This API controls the InfoFrame values.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     displayId         Monitor Identifier
//! \param [in,out] pInfoframeData    Contains data corresponding to InfoFrame
//!
//! \return    This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!            specific meaning for this API, they are listed below.
//!
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Disp_InfoFrameControl(__in NvU32 displayId, __inout NV_INFOFRAME_DATA *pInfoframeData);






//! \ingroup dispcontrol
//! @{
///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_Disp_ColorControl
//
//! \fn NvAPI_Disp_ColorControl(NvU32 displayId, NV_COLOR_DATA *pColorData)
//! DESCRIPTION:    This API controls the Color values.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     displayId         Monitor Identifier
//! \param [in,out] pColorData        Contains data corresponding to color information
//!
//! \return  RETURN STATUS:
//! ::NVAPI_OK,
//! ::NVAPI_ERROR,
//! ::NVAPI_INVALID_ARGUMENT
//
///////////////////////////////////////////////////////////////////////////////

typedef enum
{
    NV_COLOR_CMD_GET                 = 1,
    NV_COLOR_CMD_SET,
    NV_COLOR_CMD_IS_SUPPORTED_COLOR,
    NV_COLOR_CMD_GET_DEFAULT
} NV_COLOR_CMD;

//!  See Table 14 of CEA-861E.  Not all of this is supported by the GPU.
typedef enum
{
    NV_COLOR_FORMAT_RGB             = 0,
    NV_COLOR_FORMAT_YUV422,
    NV_COLOR_FORMAT_YUV444,
    NV_COLOR_FORMAT_YUV420,

    NV_COLOR_FORMAT_DEFAULT         = 0xFE,
    NV_COLOR_FORMAT_AUTO            = 0xFF
} NV_COLOR_FORMAT;



typedef enum
{
    NV_COLOR_COLORIMETRY_RGB             = 0,
    NV_COLOR_COLORIMETRY_YCC601,
    NV_COLOR_COLORIMETRY_YCC709,
    NV_COLOR_COLORIMETRY_XVYCC601,
    NV_COLOR_COLORIMETRY_XVYCC709,
    NV_COLOR_COLORIMETRY_SYCC601,
    NV_COLOR_COLORIMETRY_ADOBEYCC601,
    NV_COLOR_COLORIMETRY_ADOBERGB,
    NV_COLOR_COLORIMETRY_BT2020RGB,
    NV_COLOR_COLORIMETRY_BT2020YCC,
    NV_COLOR_COLORIMETRY_BT2020cYCC,

    NV_COLOR_COLORIMETRY_DEFAULT         = 0xFE,
    NV_COLOR_COLORIMETRY_AUTO            = 0xFF
} NV_COLOR_COLORIMETRY;

typedef enum _NV_DYNAMIC_RANGE
{
    NV_DYNAMIC_RANGE_VESA     = 0x0,
    NV_DYNAMIC_RANGE_CEA      = 0x1,

    NV_DYNAMIC_RANGE_AUTO     = 0xFF
} NV_DYNAMIC_RANGE;

typedef enum _NV_BPC
{
    NV_BPC_DEFAULT         = 0,
    NV_BPC_6               = 1,
    NV_BPC_8               = 2,
    NV_BPC_10              = 3,
    NV_BPC_12              = 4,
    NV_BPC_16              = 5,
} NV_BPC;

typedef enum _NV_COLOR_SELECTION_POLICY
{
     NV_COLOR_SELECTION_POLICY_USER     = 0,     //!< app/nvcpl make decision to select the desire color format
     NV_COLOR_SELECTION_POLICY_BEST_QUALITY = 1, //!< driver/ OS make decision to select the best color format
     NV_COLOR_SELECTION_POLICY_DEFAULT = NV_COLOR_SELECTION_POLICY_BEST_QUALITY,
     NV_COLOR_SELECTION_POLICY_UNKNOWN = 0xFF,
} NV_COLOR_SELECTION_POLICY;

typedef enum _NV_DESKTOP_COLOR_DEPTH
{
  NV_DESKTOP_COLOR_DEPTH_DEFAULT         = 0x0,                                    // set if the current setting should be kept
  NV_DESKTOP_COLOR_DEPTH_8BPC            = 0x1,                                    //8 bit int per color component (8 bit int alpha)
  NV_DESKTOP_COLOR_DEPTH_10BPC           = 0x2,                                    //10 bit int per color component (2 bit int alpha)
  NV_DESKTOP_COLOR_DEPTH_16BPC_FLOAT     = 0x3,                                    //16 bit float per color component (16 bit float alpha)
  NV_DESKTOP_COLOR_DEPTH_16BPC_FLOAT_WCG = 0x4,                                    //16 bit float per color component (16 bit float alpha) wide color gamut
  NV_DESKTOP_COLOR_DEPTH_16BPC_FLOAT_HDR = 0x5,                                    //16 bit float per color component (16 bit float alpha) HDR
  NV_DESKTOP_COLOR_DEPTH_MAX_VALUE       = NV_DESKTOP_COLOR_DEPTH_16BPC_FLOAT_HDR, // must be set to highest enum value
} NV_DESKTOP_COLOR_DEPTH;

typedef struct _NV_COLOR_DATA_V1
{
    NvU32 version; //!< Version of this structure
    NvU16 size;    //!< Size of this structure
    NvU8  cmd;
    struct
    {
        NvU8  colorFormat;    //!< One of NV_COLOR_FORMAT enum values.
        NvU8  colorimetry;    //!< One of NV_COLOR_COLORIMETRY enum values.
    } data;
} NV_COLOR_DATA_V1;

typedef struct _NV_COLOR_DATA_V2
{
    NvU32 version; //!< Version of this structure
    NvU16 size;    //!< Size of this structure
    NvU8  cmd;
    struct
    {
        NvU8  colorFormat;    //!< One of NV_COLOR_FORMAT enum values.
        NvU8  colorimetry;    //!< One of NV_COLOR_COLORIMETRY enum values.
        NvU8  dynamicRange;   //!< One of NV_DYNAMIC_RANGE enum values.
    } data;
} NV_COLOR_DATA_V2;

typedef struct _NV_COLOR_DATA_V3
{
    NvU32 version; //!< Version of this structure
    NvU16 size;    //!< Size of this structure
    NvU8  cmd;
    struct
    {
        NvU8     colorFormat;    //!< One of NV_COLOR_FORMAT enum values.
        NvU8     colorimetry;    //!< One of NV_COLOR_COLORIMETRY enum values.
        NvU8     dynamicRange;   //!< One of NV_DYNAMIC_RANGE enum values.
        NV_BPC   bpc;            //!< One of NV_BPC enum values.
    } data;
} NV_COLOR_DATA_V3;

typedef struct _NV_COLOR_DATA_V4
{
    NvU32 version; //!< Version of this structure
    NvU16 size;    //!< Size of this structure
    NvU8  cmd;
    struct
    {
        NvU8     colorFormat;     //!< One of NV_COLOR_FORMAT enum values.
        NvU8     colorimetry;     //!< One of NV_COLOR_COLORIMETRY enum values.
        NvU8     dynamicRange;    //!< One of NV_DYNAMIC_RANGE enum values.
        NV_BPC   bpc;             //!< One of NV_BPC enum values.
        NV_COLOR_SELECTION_POLICY colorSelectionPolicy; //!< One of the color selection policy
    } data;
} NV_COLOR_DATA_V4;

typedef struct _NV_COLOR_DATA_V5
{
    NvU32 version; //!< Version of this structure
    NvU16 size;    //!< Size of this structure
    NvU8  cmd;
    struct
    {
        NvU8                      colorFormat;          //!< One of NV_COLOR_FORMAT enum values.
        NvU8                      colorimetry;          //!< One of NV_COLOR_COLORIMETRY enum values.
        NvU8                      dynamicRange;         //!< One of NV_DYNAMIC_RANGE enum values.
        NV_BPC                    bpc;                  //!< One of NV_BPC enum values.
        NV_COLOR_SELECTION_POLICY colorSelectionPolicy; //!< One of the color selection policy
        NV_DESKTOP_COLOR_DEPTH    depth;                //!< One of NV_DESKTOP_COLOR_DEPTH enum values.
    } data;
} NV_COLOR_DATA_V5;

typedef NV_COLOR_DATA_V5    NV_COLOR_DATA;

#define NV_COLOR_DATA_VER1  MAKE_NVAPI_VERSION(NV_COLOR_DATA_V1, 1)
#define NV_COLOR_DATA_VER2  MAKE_NVAPI_VERSION(NV_COLOR_DATA_V2, 2)
#define NV_COLOR_DATA_VER3  MAKE_NVAPI_VERSION(NV_COLOR_DATA_V3, 3)
#define NV_COLOR_DATA_VER4  MAKE_NVAPI_VERSION(NV_COLOR_DATA_V4, 4)
#define NV_COLOR_DATA_VER5  MAKE_NVAPI_VERSION(NV_COLOR_DATA_V5, 5)
#define NV_COLOR_DATA_VER   NV_COLOR_DATA_VER5

NVAPI_INTERFACE NvAPI_Disp_ColorControl(NvU32 displayId, NV_COLOR_DATA *pColorData);

//! @}


typedef enum
{
    NV_STATIC_METADATA_TYPE_1 = 0                   //!< Tells the type of structure used to define the Static Metadata Descriptor block.
}NV_STATIC_METADATA_DESCRIPTOR_ID;

typedef struct _NV_HDR_CAPABILITIES_V1
{
    NvU32 version;                                                  //!< Version of this structure

    NvU32 isST2084EotfSupported                 :1;                 //!< HDMI2.0a UHDA HDR with ST2084 EOTF (CEA861.3). Boolean: 0 = not supported, 1 = supported;
    NvU32 isTraditionalHdrGammaSupported        :1;                 //!< HDMI2.0a traditional HDR gamma (CEA861.3). Boolean: 0 = not supported, 1 = supported;
    NvU32 isEdrSupported                        :1;                 //!< Extended Dynamic Range on SDR displays. Boolean: 0 = not supported, 1 = supported;
    NvU32 driverExpandDefaultHdrParameters      :1;                 //!< If set, driver will expand default (=zero) HDR capabilities parameters contained in display's EDID.
                                                                    //!< Boolean: 0 = report actual HDR parameters, 1 = expand default HDR parameters;
    NvU32 isTraditionalSdrGammaSupported        :1;                 //!< HDMI2.0a traditional SDR gamma (CEA861.3). Boolean: 0 = not supported, 1 = supported;
    NvU32 reserved                              :27;

    NV_STATIC_METADATA_DESCRIPTOR_ID static_metadata_descriptor_id; //!< Static Metadata Descriptor Id (0 for static metadata type 1)

    struct                                                          //!< Static Metadata Descriptor Type 1, CEA-861.3, SMPTE ST2086
    {
        NvU16    displayPrimary_x0;                                 //!< x coordinate of color primary 0 (e.g. Red) of the display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y0;                                 //!< y coordinate of color primary 0 (e.g. Red) of the display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayPrimary_x1;                                 //!< x coordinate of color primary 1 (e.g. Green) of the display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y1;                                 //!< y coordinate of color primary 1 (e.g. Green) of the display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayPrimary_x2;                                 //!< x coordinate of color primary 2 (e.g. Blue) of the display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y2;                                 //!< y coordinate of color primary 2 (e.g. Blue) of the display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayWhitePoint_x;                               //!< x coordinate of white point of the display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayWhitePoint_y;                               //!< y coordinate of white point of the display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    desired_content_max_luminance;                     //!< Maximum display luminance = desired max luminance of HDR content ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
        NvU16    desired_content_min_luminance;                     //!< Minimum display luminance = desired min luminance of HDR content ([0x0001-0xFFFF] = [1.0 - 6.55350] cd/m^2)
        NvU16    desired_content_max_frame_average_luminance;       //!< Desired maximum Frame-Average Light Level (MaxFALL) of HDR content ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
    }display_data;
} NV_HDR_CAPABILITIES_V1;

typedef struct _NV_HDR_CAPABILITIES_V2
{
    NvU32 version;                                                  //!< Version of this structure

    NvU32 isST2084EotfSupported                 :1;                 //!< HDMI2.0a UHDA HDR with ST2084 EOTF (CEA861.3). Boolean: 0 = not supported, 1 = supported;
    NvU32 isTraditionalHdrGammaSupported        :1;                 //!< HDMI2.0a traditional HDR gamma (CEA861.3). Boolean: 0 = not supported, 1 = supported;
    NvU32 isEdrSupported                        :1;                 //!< Extended Dynamic Range on SDR displays. Boolean: 0 = not supported, 1 = supported;
    NvU32 driverExpandDefaultHdrParameters      :1;                 //!< If set, driver will expand default (=zero) HDR capabilities parameters contained in display's EDID.
                                                                    //!< Boolean: 0 = report actual HDR parameters, 1 = expand default HDR parameters;
    NvU32 isTraditionalSdrGammaSupported        :1;                 //!< HDMI2.0a traditional SDR gamma (CEA861.3). Boolean: 0 = not supported, 1 = supported;
    NvU32 isDolbyVisionSupported                :1;                 //!< Dolby Vision Support. Boolean: 0 = not supported, 1 = supported;
    NvU32 reserved                              :26;

    NV_STATIC_METADATA_DESCRIPTOR_ID static_metadata_descriptor_id; //!< Static Metadata Descriptor Id (0 for static metadata type 1)

    struct                                                          //!< Static Metadata Descriptor Type 1, CEA-861.3, SMPTE ST2086
    {
        NvU16    displayPrimary_x0;                                 //!< x coordinate of color primary 0 (e.g. Red) of the display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y0;                                 //!< y coordinate of color primary 0 (e.g. Red) of the display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayPrimary_x1;                                 //!< x coordinate of color primary 1 (e.g. Green) of the display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y1;                                 //!< y coordinate of color primary 1 (e.g. Green) of the display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayPrimary_x2;                                 //!< x coordinate of color primary 2 (e.g. Blue) of the display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y2;                                 //!< y coordinate of color primary 2 (e.g. Blue) of the display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayWhitePoint_x;                               //!< x coordinate of white point of the display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayWhitePoint_y;                               //!< y coordinate of white point of the display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    desired_content_max_luminance;                     //!< Maximum display luminance = desired max luminance of HDR content ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
        NvU16    desired_content_min_luminance;                     //!< Minimum display luminance = desired min luminance of HDR content ([0x0001-0xFFFF] = [1.0 - 6.55350] cd/m^2)
        NvU16    desired_content_max_frame_average_luminance;       //!< Desired maximum Frame-Average Light Level (MaxFALL) of HDR content ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
    }display_data;

    struct
    {
        NvU32 VSVDB_version               : 3;                //!< Version of Vendor Data block,Version 0: 25 bytes  Version 1: 14 bytes
        NvU32 dm_version                  : 8;                //!< Upper Nibble represents major version of Display Management(DM) while lower represents minor version of DM
        NvU32 supports_2160p60hz          : 1;                //!< If set sink is capable of 4kx2k @ 60hz
        NvU32 supports_YUV422_12bit       : 1;                //!< If set, sink is capable of YUV422-12 bit
        NvU32 supports_global_dimming     : 1;                //!< Indicates if sink supports global dimming
        NvU32 colorimetry                 : 1;                //!< If set indicates sink supports DCI P3 colorimetry, REc709 otherwise
        NvU32 supports_backlight_control  : 2;                //!< This is set when sink is using lowlatency interface and can control its backlight.
        NvU32 backlt_min_luma             : 2;                //!< It is the level for Backlt min luminance value.
        NvU32 interface_supported_by_sink : 2;                //!< Indicates the interface (standard or low latency) supported by the sink.
        NvU32 supports_10b_12b_444        : 2;                //!< It is set when interface supported is low latency, it tells whether it supports 10 bit or 12 bit RGB 4:4:4 or YCbCr 4:4:4 or both.
        NvU32 reserved                    : 9;                //!< Should be set to zero
                                                              //!< All values below are encoded use DolbyVisionHDMITransmissionSpecification document to decode
        NvU16 target_min_luminance;                           //!< Represents min luminance level of Sink
        NvU16 target_max_luminance;                           //!< Represents max luminance level of sink
        NvU16 cc_red_x;                                       //!< Red primary chromaticity coordinate x
        NvU16 cc_red_y;                                       //!< Red primary chromaticity coordinate y
        NvU16 cc_green_x;                                     //!< Green primary chromaticity coordinate x
        NvU16 cc_green_y;                                     //!< Green primary chromaticity coordinate Y
        NvU16 cc_blue_x;                                      //!< Blue primary chromaticity coordinate x
        NvU16 cc_blue_y;                                      //!< Blue primary chromaticity coordinate y
        NvU16 cc_white_x;                                     //!< White primary chromaticity coordinate x
        NvU16 cc_white_y;                                     //!< White primary chromaticity coordinate y
    }dv_static_metadata;

} NV_HDR_CAPABILITIES_V2;

#define NV_HDR_CAPABILITIES_VER1  MAKE_NVAPI_VERSION(NV_HDR_CAPABILITIES_V1, 1)
#define NV_HDR_CAPABILITIES_VER2  MAKE_NVAPI_VERSION(NV_HDR_CAPABILITIES_V2, 2)
#define NV_HDR_CAPABILITIES_VER   NV_HDR_CAPABILITIES_VER2
typedef NV_HDR_CAPABILITIES_V2    NV_HDR_CAPABILITIES;

//! \ingroup dispcontrol
//! @{
///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_Disp_GetHdrCapabilities
//
//! \fn NvAPI_Disp_GetHdrCapabilities(NvU32 displayId, NV_HDR_CAPABILITIES  *pHdrCapabilities)
//! DESCRIPTION:    This API gets High Dynamic Range (HDR) capabilities of the display.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     displayId           Monitor Identifier
//! \param [in,out] pHdrCapabilities    display's HDR capabilities
//!
//! \return    This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!            specific meaning for this API, they are listed below.
//
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Disp_GetHdrCapabilities(__in NvU32 displayId, __inout NV_HDR_CAPABILITIES *pHdrCapabilities);

//! @}


typedef enum
{
    NV_HDR_CMD_GET = 0,                             //!< Get current HDR output configuration
    NV_HDR_CMD_SET = 1                              //!< Set HDR output configuration
} NV_HDR_CMD;

typedef enum
{
    // Official production-ready HDR modes
    NV_HDR_MODE_OFF                 = 0,            //!< Turn off HDR
    NV_HDR_MODE_UHDA                = 2,            //!< Source: CCCS [a.k.a FP16 scRGB, linear, sRGB primaries, [-65504,0, 65504] range, RGB(1,1,1) = 80nits]  Output : UHDA HDR [a.k.a HDR10, RGB/YCC 10/12bpc ST2084(PQ) EOTF RGB(1,1,1) = 10000 nits, Rec2020 color primaries, ST2086 static HDR metadata]. This is the only supported production HDR mode.

    // Experimental
    NV_HDR_MODE_UHDA_PASSTHROUGH    = 5,            //!< Experimental mode only, not for production! Source: HDR10 RGB 10bpc Output: HDR10 RGB 10 bpc - signal UHDA HDR mode (PQ + Rec2020) to the sink but send source pixel values unmodified (no PQ or Rec2020 conversions) - assumes source is already in HDR10 format.
    NV_HDR_MODE_DOLBY_VISION        = 7,            //!< Experimental mode only, not for production! Source: RGB8 Dolby Vision encoded (12 bpc YCbCr422 packed into RGB8) Output: Dolby Vision encoded : Application is to encoded frames in DV format and embed DV dynamic metadata as described in Dolby Vision specification.

    // Unsupported/obsolete HDR modes
    NV_HDR_MODE_EDR                 = 3,            //!< Do not use! Internal test mode only, to be removed. Source: CCCS (a.k.a FP16 scRGB) Output : EDR (Extended Dynamic Range) - HDR content is tonemapped and gamut mapped to output on regular SDR display set to max luminance ( ~300 nits ).
    NV_HDR_MODE_SDR                 = 4,            //!< Do not use! Internal test mode only, to be removed. Source: any Output: SDR (Standard Dynamic Range), we continuously send SDR EOTF InfoFrame signaling, HDMI compliance testing.
    NV_HDR_MODE_UHDA_NB             = 6,            //!< Do not use! Internal test mode only, to be removed. Source: CCCS (a.k.a FP16 scRGB) Output : notebook HDR
    NV_HDR_MODE_UHDBD               = 2             //!< Do not use! Obsolete, to be removed. NV_HDR_MODE_UHDBD == NV_HDR_MODE_UHDA, reflects obsolete pre-UHDA naming convention.

} NV_HDR_MODE;

typedef struct _NV_HDR_COLOR_DATA_V1
{
    NvU32                             version;                                 //!< Version of this structure
    NV_HDR_CMD                        cmd;                                     //!< Command get/set
    NV_HDR_MODE                       hdrMode;                                 //!< HDR mode
    NV_STATIC_METADATA_DESCRIPTOR_ID  static_metadata_descriptor_id;           //!< Static Metadata Descriptor Id (0 for static metadata type 1)

    struct                                                                    //!< Static Metadata Descriptor Type 1, CEA-861.3, SMPTE ST2086
    {
        NvU16    displayPrimary_x0;                                           //!< x coordinate of color primary 0 (e.g. Red) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y0;                                           //!< y coordinate of color primary 0 (e.g. Red) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayPrimary_x1;                                           //!< x coordinate of color primary 1 (e.g. Green) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y1;                                           //!< y coordinate of color primary 1 (e.g. Green) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayPrimary_x2;                                           //!< x coordinate of color primary 2 (e.g. Blue) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y2;                                           //!< y coordinate of color primary 2 (e.g. Blue) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayWhitePoint_x;                                         //!< x coordinate of white point of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayWhitePoint_y;                                         //!< y coordinate of white point of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    max_display_mastering_luminance;                             //!< Maximum display mastering luminance ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
        NvU16    min_display_mastering_luminance;                             //!< Minimum display mastering luminance ([0x0001-0xFFFF] = [1.0 - 6.55350] cd/m^2)

        NvU16    max_content_light_level;                                     //!< Maximum Content Light level (MaxCLL) ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
        NvU16    max_frame_average_light_level;                               //!< Maximum Frame-Average Light Level (MaxFALL) ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
    } mastering_display_data;
} NV_HDR_COLOR_DATA_V1;

typedef struct _NV_HDR_COLOR_DATA_V2
{
    NvU32                             version;                                 //!< Version of this structure
    NV_HDR_CMD                        cmd;                                     //!< Command get/set
    NV_HDR_MODE                       hdrMode;                                 //!< HDR mode
    NV_STATIC_METADATA_DESCRIPTOR_ID  static_metadata_descriptor_id;           //!< Static Metadata Descriptor Id (0 for static metadata type 1)

    struct                                                                    //!< Static Metadata Descriptor Type 1, CEA-861.3, SMPTE ST2086
    {
        NvU16    displayPrimary_x0;                                           //!< x coordinate of color primary 0 (e.g. Red) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y0;                                           //!< y coordinate of color primary 0 (e.g. Red) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayPrimary_x1;                                           //!< x coordinate of color primary 1 (e.g. Green) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y1;                                           //!< y coordinate of color primary 1 (e.g. Green) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayPrimary_x2;                                           //!< x coordinate of color primary 2 (e.g. Blue) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayPrimary_y2;                                           //!< y coordinate of color primary 2 (e.g. Blue) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    displayWhitePoint_x;                                         //!< x coordinate of white point of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
        NvU16    displayWhitePoint_y;                                         //!< y coordinate of white point of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

        NvU16    max_display_mastering_luminance;                             //!< Maximum display mastering luminance ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
        NvU16    min_display_mastering_luminance;                             //!< Minimum display mastering luminance ([0x0001-0xFFFF] = [1.0 - 6.55350] cd/m^2)

        NvU16    max_content_light_level;                                     //!< Maximum Content Light level (MaxCLL) ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
        NvU16    max_frame_average_light_level;                               //!< Maximum Frame-Average Light Level (MaxFALL) ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
    } mastering_display_data;

    NV_COLOR_FORMAT          hdrColorFormat;                                     //!< Optional, One of NV_COLOR_FORMAT enum values, if set it will apply requested color format for HDR session
    NV_DYNAMIC_RANGE         hdrDynamicRange;                                    //!< Optional, One of NV_DYNAMIC_RANGE enum values, if set it will apply requested dynamic range for HDR session
    NV_BPC                   hdrBpc;                                             //!< Optional, One of NV_BPC enum values, if set it will apply requested color depth
                                                                                 //!< Dolby Vision mode: DV supports specific combinations of colorformat, dynamic range and bpc. Please refer Dolby Vision specification.
                                                                                 //!<                    If invalid or no combination is passed driver will force default combination of RGB format + full range + 8bpc.
                                                                                 //!< HDR mode: These fields are ignored in hdr mode
} NV_HDR_COLOR_DATA_V2;

#define NV_HDR_COLOR_DATA_VER1  MAKE_NVAPI_VERSION(NV_HDR_COLOR_DATA_V1, 1)
#define NV_HDR_COLOR_DATA_VER2  MAKE_NVAPI_VERSION(NV_HDR_COLOR_DATA_V2, 2)

#ifndef NV_HDR_COLOR_DATA_VER
#define NV_HDR_COLOR_DATA_VER   NV_HDR_COLOR_DATA_VER2
typedef NV_HDR_COLOR_DATA_V2    NV_HDR_COLOR_DATA;
#endif

//! \ingroup dispcontrol
//! @{
 ///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_Disp_HdrColorControl
//
//! \fn NvAPI_Disp_HdrColorControl(NvU32 displayId, NV_HDR_COLOR_DATA *pHdrColorData)
//! DESCRIPTION:    This API configures High Dynamic Range (HDR) and Extended Dynamic Range (EDR) output.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     displayId         Monitor Identifier
//! \param [in,out] pHdrColorData     HDR configuration data
//!
//! \return    This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!            specific meaning for this API, they are listed below.
//
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Disp_HdrColorControl(__in NvU32 displayId, __inout NV_HDR_COLOR_DATA *pHdrColorData);

//! @}


//! \ingroup dispcontrol
//! Used in NvAPI_DISP_GetTiming().
typedef struct
{
    NvU32                   isInterlaced   : 4;  //!< To retrieve interlaced/progressive timing
    NvU32                   reserved0      : 12;
    union
    {
        NvU32               tvFormat       : 8;  //!< The actual analog HD/SDTV format. Used when the timing type is
                                                 //!  NV_TIMING_OVERRIDE_ANALOG_TV and width==height==rr==0.
        NvU32               ceaId          : 8;  //!< The EIA/CEA 861B/D predefined short timing descriptor ID.
                                                 //!  Used when the timing type is NV_TIMING_OVERRIDE_EIA861
                                                 //!  and width==height==rr==0.
        NvU32               nvPsfId        : 8;  //!< The NV predefined PsF format Id.
                                                 //!  Used when the timing type is NV_TIMING_OVERRIDE_NV_PREDEFINED.
    };
    NvU32                   scaling        : 8;  //!< Define preferred scaling
}NV_TIMING_FLAG;

//! \ingroup dispcontrol
//! Used in NvAPI_DISP_GetTiming().
typedef struct _NV_TIMING_INPUT
{
    NvU32 version;                      //!< (IN)     structure version

    NvU32 width;                        //!< Visible horizontal size
    NvU32 height;                       //!< Visible vertical size
    float rr;                           //!< Timing refresh rate

    NV_TIMING_FLAG flag;                //!< Flag containing additional info for timing calculation.

    NV_TIMING_OVERRIDE type;            //!< Timing type(formula) to use for calculating the timing
}NV_TIMING_INPUT;

#define NV_TIMING_INPUT_VER   MAKE_NVAPI_VERSION(NV_TIMING_INPUT,1)

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_DISP_GetTiming
//
//! DESCRIPTION:  This function calculates the timing from the visible width/height/refresh-rate and timing type info.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//!
//! \param [in]   displayId     Display ID of the display.
//! \param [in]   timingInput   Inputs used for calculating the timing.
//! \param [out]  pTiming       Pointer to the NV_TIMING structure.
//!
//! \return        This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!                specific meaning for this API, they are listed below.
//!
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DISP_GetTiming( __in NvU32 displayId,__in NV_TIMING_INPUT *timingInput, __out NV_TIMING *pTiming);



///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_DISP_GetMonitorCapabilities
//
//! \fn NvAPI_DISP_GetMonitorCapabilities(NvU32 displayId, NV_MONITOR_CAPABILITIES *pMonitorCapabilities)
//! DESCRIPTION:     This API returns the Monitor capabilities
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]      displayId                Monitor Identifier
//! \param [out]     pMonitorCapabilities     The monitor support info
//!
//! \return ::NVAPI_OK,
//!         ::NVAPI_ERROR,
//!         ::NVAPI_INVALID_ARGUMENT
//
///////////////////////////////////////////////////////////////////////////////

//! \ingroup dispcontrol
//! @{


//! HDMI-related and extended CAPs
typedef enum
{
    // hdmi related caps
    NV_MONITOR_CAPS_TYPE_HDMI_VSDB = 0x1000,
    NV_MONITOR_CAPS_TYPE_HDMI_VCDB = 0x1001,
} NV_MONITOR_CAPS_TYPE;



typedef struct _NV_MONITOR_CAPS_VCDB
{
    NvU8    quantizationRangeYcc         : 1;
    NvU8    quantizationRangeRgb         : 1;
    NvU8    scanInfoPreferredVideoFormat : 2;
    NvU8    scanInfoITVideoFormats       : 2;
    NvU8    scanInfoCEVideoFormats       : 2;
} NV_MONITOR_CAPS_VCDB;


//! See NvAPI_DISP_GetMonitorCapabilities().
typedef struct _NV_MONITOR_CAPS_VSDB
{
    // byte 1
    NvU8    sourcePhysicalAddressB         : 4; //!< Byte 1
    NvU8    sourcePhysicalAddressA         : 4; //!< Byte 1
    // byte 2
    NvU8    sourcePhysicalAddressD         : 4; //!< Byte 2
    NvU8    sourcePhysicalAddressC         : 4; //!< Byte 2
    // byte 3
    NvU8    supportDualDviOperation        : 1; //!< Byte 3
    NvU8    reserved6                      : 2; //!< Byte 3
    NvU8    supportDeepColorYCbCr444       : 1; //!< Byte 3
    NvU8    supportDeepColor30bits         : 1; //!< Byte 3
    NvU8    supportDeepColor36bits         : 1; //!< Byte 3
    NvU8    supportDeepColor48bits         : 1; //!< Byte 3
    NvU8    supportAI                      : 1; //!< Byte 3
    // byte 4
    NvU8    maxTmdsClock;  //!< Bye 4
    // byte 5
    NvU8    cnc0SupportGraphicsTextContent : 1; //!< Byte 5
    NvU8    cnc1SupportPhotoContent        : 1; //!< Byte 5
    NvU8    cnc2SupportCinemaContent       : 1; //!< Byte 5
    NvU8    cnc3SupportGameContent         : 1; //!< Byte 5
    NvU8    reserved8                      : 1; //!< Byte 5
    NvU8    hasVicEntries                  : 1; //!< Byte 5
    NvU8    hasInterlacedLatencyField      : 1; //!< Byte 5
    NvU8    hasLatencyField                : 1; //!< Byte 5
    // byte 6
    NvU8    videoLatency; //!< Byte 6
    // byte 7
    NvU8    audioLatency; //!< Byte 7
    // byte 8
    NvU8    interlacedVideoLatency; //!< Byte 8
    // byte 9
    NvU8    interlacedAudioLatency; //!< Byte 9
    // byte 10
    NvU8    reserved13                     : 7; //!< Byte 10
    NvU8    has3dEntries                   : 1; //!< Byte 10
    // byte 11
    NvU8    hdmi3dLength                   : 5; //!< Byte 11
    NvU8    hdmiVicLength                  : 3; //!< Byte 11
    // Remaining bytes
    NvU8    hdmi_vic[7];  //!< Keeping maximum length for 3 bits
    NvU8    hdmi_3d[31];  //!< Keeping maximum length for 5 bits
} NV_MONITOR_CAPS_VSDB;


//! See NvAPI_DISP_GetMonitorCapabilities().
typedef struct _NV_MONITOR_CAPABILITIES_V1
{
    NvU32    version;
    NvU16    size;
    NvU32    infoType;
    NvU32    connectorType;        //!< Out: VGA, TV, DVI, HDMI, DP
    NvU8     bIsValidInfo : 1;     //!< Boolean : Returns invalid if requested info is not present such as VCDB not present
    union {
        NV_MONITOR_CAPS_VSDB  vsdb;
        NV_MONITOR_CAPS_VCDB  vcdb;
    } data;
} NV_MONITOR_CAPABILITIES_V1;

typedef NV_MONITOR_CAPABILITIES_V1 NV_MONITOR_CAPABILITIES;

//! Macro for constructing the version field of ::NV_MONITOR_CAPABILITIES_V1
#define NV_MONITOR_CAPABILITIES_VER1   MAKE_NVAPI_VERSION(NV_MONITOR_CAPABILITIES_V1,1)
#define NV_MONITOR_CAPABILITIES_VER    NV_MONITOR_CAPABILITIES_VER1

//! @}

//! SUPPORTED OS:  Windows 7 and higher
//!
//! \ingroup dispcontrol
NVAPI_INTERFACE NvAPI_DISP_GetMonitorCapabilities(__in NvU32 displayId, __inout NV_MONITOR_CAPABILITIES *pMonitorCapabilities);

//! \ingroup dispcontrol
typedef struct _NV_MONITOR_COLOR_DATA
{
    NvU32                   version;
// We are only supporting DP monitors for now. We need to extend this to HDMI panels as well
    NV_DP_COLOR_FORMAT      colorFormat;        //!< One of the supported color formats
    NV_DP_BPC               backendBitDepths;   //!< One of the supported bit depths
} NV_MONITOR_COLOR_CAPS_V1;

typedef NV_MONITOR_COLOR_CAPS_V1 NV_MONITOR_COLOR_CAPS;

//! \ingroup dispcontrol
#define NV_MONITOR_COLOR_CAPS_VER1   MAKE_NVAPI_VERSION(NV_MONITOR_COLOR_CAPS_V1,1)
#define NV_MONITOR_COLOR_CAPS_VER    NV_MONITOR_COLOR_CAPS_VER1

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_DISP_GetMonitorColorCapabilities
//
//! DESCRIPTION:    This API returns all the color formats and bit depth values supported by a given DP monitor.
//!
//! USAGE:         Sequence of calls which caller should make to get the information.
//!                1. First call NvAPI_DISP_GetMonitorColorCapabilities() with pMonitorColorCapabilities as NULL to get the count.
//!                2. Allocate memory for color caps(NV_MONITOR_COLOR_CAPS) array.
//!                3. Call NvAPI_DISP_GetMonitorColorCapabilities() again with the pointer to the memory allocated to get all the
//!                   color capabilities.
//!
//!                Note :
//!                1. pColorCapsCount should never be NULL, else the API will fail with NVAPI_INVALID_ARGUMENT.
//!                2. *pColorCapsCount returned from the API will always be the actual count in any/every call.
//!                3. Memory size to be allocated should be (*pColorCapsCount * sizeof(NV_MONITOR_COLOR_CAPS)).
//!                4. If the memory allocated is less than what is required to return all the timings, this API will return the
//!                   amount of information which can fit in user provided buffer and API will return NVAPI_INSUFFICIENT_BUFFER.
//!                5. If the caller specifies a greater value for *pColorCapsCount in second call to NvAPI_DISP_GetMonitorColorCapabilities()
//!                   than what was returned from first call, the API will return only the actual number of elements in the color
//!                   capabilities array and the extra buffer will remain unused.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]      displayId                  Monitor Identifier
//! \param [in, out] pMonitorColorCapabilities  The monitor color capabilities information
//! \param [in, out] pColorCapsCount            - During input, the number of elements allocated for the pMonitorColorCapabilities pointer
//!                                             - During output, the actual number of color data elements the monitor supports
//!
//! \return    This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!            specific meaning for this API, they are listed below.
//!
//! \retval         NVAPI_INSUFFICIENT_BUFFER   The input buffer size is not sufficient to hold the total contents. In this case
//!                                             *pColorCapsCount will hold the required amount of elements.
//! \retval         NVAPI_INVALID_DISPLAY_ID    The input monitor is either not connected or is not a DP panel.
//!
//! \ingroup dispcontrol
//!
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DISP_GetMonitorColorCapabilities(__in NvU32 displayId, __inout_ecount_part_opt(*pColorCapsCount, *pColorCapsCount) NV_MONITOR_COLOR_CAPS *pMonitorColorCapabilities, __inout NvU32 *pColorCapsCount);

//! \ingroup dispcontrol
//! Used in NvAPI_DISP_EnumCustomDisplay() and NvAPI_DISP_TryCustomDisplay().
typedef struct
{
    NvU32                   version;

    // the source mode information
    NvU32                   width;             //!< Source surface(source mode) width
    NvU32                   height;            //!< Source surface(source mode) height
    NvU32                   depth;             //!< Source surface color depth."0" means all 8/16/32bpp
    NV_FORMAT               colorFormat;       //!< Color format (optional)

    NV_VIEWPORTF            srcPartition;      //!< For multimon support, should be set to (0,0,1.0,1.0) for now.

    float                   xRatio;            //!< Horizontal scaling ratio
    float                   yRatio;            //!< Vertical scaling ratio

    NV_TIMING               timing;            //!< Timing used to program TMDS/DAC/LVDS/HDMI/TVEncoder, etc.
    NvU32                   hwModeSetOnly : 1; //!< If set, it means a hardware modeset without OS update

}NV_CUSTOM_DISPLAY;

//! \ingroup dispcontrol
//! Used in NV_CUSTOM_DISPLAY.
#define NV_CUSTOM_DISPLAY_VER  MAKE_NVAPI_VERSION(NV_CUSTOM_DISPLAY,1)

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_DISP_EnumCustomDisplay
//
//! DESCRIPTION:    This API enumerates the custom timing specified by the enum index.
//!                 The client should keep enumerating until it returns NVAPI_END_ENUMERATION.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//! \param [in]     displayId   Dispaly ID of the display.
//! \param [in]     index       Enum index
//! \param [inout]  pCustDisp   Pointer to the NV_CUSTOM_DISPLAY structure
//!
//! \return        This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!                specific meaning for this API, they are listed below.
//! \retval        NVAPI_INVALID_DISPLAY_ID:   Custom Timing is not supported on the Display, whose display id is passed
//!
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DISP_EnumCustomDisplay( __in NvU32 displayId, __in NvU32 index, __inout NV_CUSTOM_DISPLAY *pCustDisp);

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_DISP_TryCustomDisplay
//
//! DESCRIPTION:    This API is used to set up a custom display without saving the configuration on multiple displays.
//!
//! \note
//!  All the members of srcPartition, present in NV_CUSTOM_DISPLAY structure, should have their range in (0.0,1.0).
//!  In clone mode the timings can applied to both the target monitors but only one target at a time. \n
//!  For the secondary target the applied timings works under the following conditions:
//!  - If the secondary monitor EDID supports the selected timing, OR
//!  - If the selected custom timings can be scaled by the secondary monitor for the selected source resolution on the primary, OR
//!  - If the selected custom timings matches the existing source resolution on the primary.
//!  Setting up a custom display on non-active but connected monitors is supported only for Win7 and above.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//!
//! \param [in]    pDisplayIds    Array of the target display Dispaly IDs - See \ref handles.
//! \param [in]    count          Total number of the incoming Display IDs and corresponding NV_CUSTOM_DISPLAY structure. This is for the multi-head support.
//! \param [in]    pCustDisp      Pointer to the NV_CUSTOM_DISPLAY structure array.
//!
//! \return        This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!                specific meaning for this API, they are listed below.
//! \retval        NVAPI_INVALID_DISPLAY_ID:   Custom Timing is not supported on the Display, whose display id is passed
//!
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DISP_TryCustomDisplay( __in_ecount(count) NvU32 *pDisplayIds, __in NvU32 count, __in_ecount(count) NV_CUSTOM_DISPLAY *pCustDisp);

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_DISP_DeleteCustomDisplay
//
//! DESCRIPTION:    This function deletes the custom display configuration, specified from the registry for  all the displays whose display IDs are passed.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//!
//! \param [in]     pDisplayIds     Array of Dispaly IDs on which custom display configuration is to be saved.
//! \param [in]     count           Total number of the incoming Dispaly IDs. This is for the multi-head support.
//! \param [in]     pCustDisp       Pointer to the NV_CUSTOM_DISPLAY structure
//!
//! \return        This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!                specific meaning for this API, they are listed below.
//! \retval        NVAPI_INVALID_DISPLAY_ID:   Custom Timing is not supported on the Display, whose display id is passed
//!
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DISP_DeleteCustomDisplay( __in_ecount(count) NvU32 *pDisplayIds, __in NvU32 count, __in NV_CUSTOM_DISPLAY *pCustDisp);

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_DISP_SaveCustomDisplay
//
//! DESCRIPTION:    This function saves the current hardware display configuration on the specified Display IDs as a custom display configuration.
//!                 This function should be called right after NvAPI_DISP_TryCustomDisplay() to save the custom display from the current
//!                 hardware context. This function will not do anything if the custom display configuration is not tested on the hardware.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//!
//! \param [in]     pDisplayIds         Array of Dispaly IDs on which custom display configuration is to be saved.
//! \param [in]     count               Total number of the incoming Dispaly IDs. This is for the multi-head support.
//! \param [in]     isThisOutputIdOnly  If set, the saved custom display will only be applied on the monitor with the same outputId (see \ref handles).
//! \param [in]     isThisMonitorIdOnly If set, the saved custom display will only be applied on the monitor with the same EDID ID or
//!                                     the same TV connector in case of analog TV.
//!
//! \return        This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!                specific meaning for this API, they are listed below.
//! \retval        NVAPI_INVALID_DISPLAY_ID:   Custom Timing is not supported on the Display, whose display id is passed
//!
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DISP_SaveCustomDisplay( __in_ecount(count) NvU32 *pDisplayIds, __in NvU32 count, __in NvU32 isThisOutputIdOnly, __in NvU32 isThisMonitorIdOnly);

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_DISP_RevertCustomDisplayTrial
//
//! DESCRIPTION:    This API is used to restore the display configuration, that was changed by calling NvAPI_DISP_TryCustomDisplay(). This function
//!                 must be called only after a custom display configuration is tested on the hardware, using NvAPI_DISP_TryCustomDisplay(),
//!                 otherwise no action is taken. On Vista, NvAPI_DISP_RevertCustomDisplayTrial should be called with an active display that
//!                 was affected during the NvAPI_DISP_TryCustomDisplay() call, per GPU.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//!
//! \param [in]    pDisplayIds   Pointer to display Id, of an active display.
//! \param [in]    count         Total number of incoming Display IDs. For future use only. Currently it is expected to be passed as 1.
//!
//! \return        This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!                specific meaning for this API, they are listed below.
//!
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DISP_RevertCustomDisplayTrial( __in_ecount(count) NvU32* pDisplayIds, __in NvU32 count);

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_GetView
//
//! This API lets caller retrieve the target display arrangement for selected source display handle.
//! \note Display PATH with this API is limited to single GPU. DUALVIEW across GPUs will be returned as STANDARD VIEW.
//!       Use NvAPI_SYS_GetDisplayTopologies() to query views across GPUs.
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_DISP_GetDisplayConfig.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 85
//!
//!  \param [in]     hNvDisplay             NVIDIA Display selection. It can be #NVAPI_DEFAULT_HANDLE or a handle enumerated from
//!                                         NvAPI_EnumNVidiaDisplayHandle().
//!  \param [out]    pTargets               User allocated storage to retrieve an array of  NV_VIEW_TARGET_INFO. Can be NULL to retrieve
//!                                         the targetCount.
//!  \param [in,out] targetMaskCount        Count of target device mask specified in pTargetMask.
//!  \param [out]    targetView             Target view selected from NV_TARGET_VIEW_MODE.
//!
//!  \retval         NVAPI_OK               Completed request
//!  \retval         NVAPI_ERROR            Miscellaneous error occurred
//!  \retval         NVAPI_INVALID_ARGUMENT Invalid input parameter.
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_DISP_GetDisplayConfig.")
NVAPI_INTERFACE NvAPI_GetView(NvDisplayHandle hNvDisplay, NV_VIEW_TARGET_INFO *pTargets, NvU32 *pTargetMaskCount, NV_TARGET_VIEW_MODE *pTargetView);







///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_GetViewEx
//
//! DESCRIPTION:    This API lets caller retrieve the target display arrangement for selected source display handle.
//!                 \note Display PATH with this API is limited to single GPU. DUALVIEW across GPUs will be returned as STANDARD VIEW.
//!                       Use NvAPI_SYS_GetDisplayTopologies() to query views across GPUs.
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_DISP_GetDisplayConfig.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 165
//!
//! \param [in]     hNvDisplay       NVIDIA Display selection. #NVAPI_DEFAULT_HANDLE is not allowed, it has to be a handle enumerated with
//!                                  NvAPI_EnumNVidiaDisplayHandle().
//! \param [in,out] pPathInfo        Count field should be set to NVAPI_MAX_DISPLAY_PATH. Can be NULL to retrieve just the pathCount.
//! \param [in,out] pPathCount       Number of elements in array pPathInfo->path.
//! \param [out]    pTargetViewMode  Display view selected from NV_TARGET_VIEW_MODE.
//!
//! \retval         NVAPI_OK                      Completed request
//! \retval         NVAPI_API_NOT_INTIALIZED      NVAPI not initialized
//! \retval         NVAPI_ERROR                   Miscellaneous error occurred
//! \retval         NVAPI_INVALID_ARGUMENT        Invalid input parameter.
//! \retval         NVAPI_EXPECTED_DISPLAY_HANDLE hNvDisplay is not a valid display handle.
//!
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_DISP_GetDisplayConfig.")
NVAPI_INTERFACE NvAPI_GetViewEx(NvDisplayHandle hNvDisplay, NV_DISPLAY_PATH_INFO *pPathInfo, NvU32 *pPathCount, NV_TARGET_VIEW_MODE *pTargetViewMode);

///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_GetSupportedViews
//
//!  This API lets caller enumerate all the supported NVIDIA display views - nView and Dualview modes.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 85
//!
//!  \param [in]     hNvDisplay             NVIDIA Display selection. It can be #NVAPI_DEFAULT_HANDLE or a handle enumerated from
//!                                         NvAPI_EnumNVidiaDisplayHandle().
//!  \param [out]    pTargetViews           Array of supported views. Can be NULL to retrieve the pViewCount first.
//!  \param [in,out] pViewCount             Count of supported views.
//!
//!  \retval         NVAPI_OK               Completed request
//!  \retval         NVAPI_ERROR            Miscellaneous error occurred
//!  \retval         NVAPI_INVALID_ARGUMENT Invalid input parameter.
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetSupportedViews(NvDisplayHandle hNvDisplay, NV_TARGET_VIEW_MODE *pTargetViews, NvU32 *pViewCount);


//! SUPPORTED OS:  Windows 7 and higher
//!
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_DISP_GetDisplayIdByDisplayName
//
//! DESCRIPTION:     This API retrieves the Display Id of a given display by
//!                  display name. The display must be active to retrieve the
//!                  displayId. In the case of clone mode or Surround gaming,
//!                  the primary or top-left display will be returned.
//!
//! \param [in]     displayName  Name of display (Eg: "\\DISPLAY1" to
//!                              retrieve the displayId for.
//! \param [out]    displayId    Display ID of the requested display.
//!
//! retval ::NVAPI_OK:                          Capabilties have been returned.
//! retval ::NVAPI_INVALID_ARGUMENT:            One or more args passed in are invalid.
//! retval ::NVAPI_API_NOT_INTIALIZED:          The NvAPI API needs to be initialized first
//! retval ::NVAPI_NO_IMPLEMENTATION:           This entrypoint not available
//! retval ::NVAPI_ERROR:                       Miscellaneous error occurred
//!
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DISP_GetDisplayIdByDisplayName(const char *displayName, NvU32* displayId);




///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_DISP_GetDisplayConfig
//
//! DESCRIPTION:     This API lets caller retrieve the current global display
//!                  configuration.
//!       USAGE:     The caller might have to call this three times to fetch all the required configuration details as follows:
//!                  First  Pass: Caller should Call NvAPI_DISP_GetDisplayConfig() with pathInfo set to NULL to fetch pathInfoCount.
//!                  Second Pass: Allocate memory for pathInfo with respect to the number of pathInfoCount(from First Pass) to fetch
//!                               targetInfoCount. If sourceModeInfo is needed allocate memory or it can be initialized to NULL.
//!             Third  Pass(Optional, only required if target information is required): Allocate memory for targetInfo with respect
//!                               to number of targetInfoCount(from Second Pass).
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in,out]  pathInfoCount    Number of elements in pathInfo array, returns number of valid topologies, this cannot be null.
//! \param [in,out]  pathInfo         Array of path information
//!
//! \return    This API can return any of the error codes enumerated in #NvAPI_Status. If there are return error codes with
//!            specific meaning for this API, they are listed below.
//!
//! \retval    NVAPI_INVALID_ARGUMENT  -   Invalid input parameter. Following can be the reason for this return value:
//!                                        -# pathInfoCount is NULL.
//!                                        -# *pathInfoCount is 0 and pathInfo is not NULL.
//!                                        -# *pathInfoCount is not 0 and pathInfo is NULL.
//! \retval    NVAPI_DEVICE_BUSY       -   ModeSet has not yet completed. Please wait and call it again.
//!
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DISP_GetDisplayConfig(__inout NvU32 *pathInfoCount, __out_ecount_full_opt(*pathInfoCount) NV_DISPLAYCONFIG_PATH_INFO *pathInfo);




///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_DISP_SetDisplayConfig
//
//
//! DESCRIPTION:     This API lets caller apply a global display configuration
//!                  across multiple GPUs.
//!
//!                  If all sourceIds are zero, then NvAPI will pick up sourceId's based on the following criteria :
//!                  - If user provides sourceModeInfo then we are trying to assign 0th sourceId always to GDIPrimary.
//!                     This is needed since active windows always moves along with 0th sourceId.
//!                  - For rest of the paths, we are incrementally assigning the sourceId per adapter basis.
//!                  - If user doesn't provide sourceModeInfo then NVAPI just picks up some default sourceId's in incremental order.
//!                  Note : NVAPI will not intelligently choose the sourceIDs for any configs that does not need a modeset.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]      pathInfoCount   Number of supplied elements in pathInfo
//! \param [in]      pathInfo        Array of path information
//! \param [in]      flags           Flags for applying settings
//!
//! \retval ::NVAPI_OK - completed request
//! \retval ::NVAPI_API_NOT_INTIALIZED - NVAPI not initialized
//! \retval ::NVAPI_ERROR - miscellaneous error occurred
//! \retval ::NVAPI_INVALID_ARGUMENT - Invalid input parameter.
//!
//! \ingroup dispcontrol
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DISP_SetDisplayConfig(__in NvU32 pathInfoCount, __in_ecount(pathInfoCount) NV_DISPLAYCONFIG_PATH_INFO* pathInfo, __in NvU32 flags);



typedef struct _NV_GET_ADAPTIVE_SYNC_DATA_V1
{
    NvU32  version ;                      //!< [in]    structure version
    NvU32  maxFrameInterval;              //!< [out]   maximum frame interval in micro seconds as set previously using NvAPI_DISP_SetAdaptiveSyncData function.
                                          //!<         If default values from EDID are used, this parameter returns 0.
    NvU32  bDisableAdaptiveSync : 1;      //!< [out]   Indicates if adaptive sync is disabled on the display.
    NvU32  bDisableFrameSplitting : 1;    //!< [out]   Indicates if frame splitting is disabled on the display.
    NvU32  reserved : 30;                 //!< reserved for future use.
    NvU32  lastFlipRefreshCount;          //!< [out]   Number of times the last flip was shown on the screen
    NvU64  lastFlipTimeStamp;             //!< [out]   Timestamp for the lastest flip on the screen
    NvU32  reservedEx[4];                 //!< reserved for future use.
} NV_GET_ADAPTIVE_SYNC_DATA_V1;

#define NV_GET_ADAPTIVE_SYNC_DATA_VER1  MAKE_NVAPI_VERSION(NV_GET_ADAPTIVE_SYNC_DATA_V1,1)
#define NV_GET_ADAPTIVE_SYNC_DATA_VER   NV_GET_ADAPTIVE_SYNC_DATA_VER1

typedef NV_GET_ADAPTIVE_SYNC_DATA_V1  NV_GET_ADAPTIVE_SYNC_DATA;

///////////////////////////////////////////////////////////////////////////////
//
//! \fn NvAPI_DISP_GetAdaptiveSyncData(__in NvU32 displayId, __inout NV_GET_ADAPTIVE_SYNC_DATA *pAdaptiveSyncData)
//! \code
//! DESCRIPTION:    This function is used to get data for the Adaptive Sync Display.
//!
//! SUPPORTED OS:  Windows 10 and higher
//!
//! \since Release: 415
//!
//! \param [in]       displayId            - display id of the display
//! \param [inout]    pAdaptiveSyncData    - A pointer to NV_GET_ADAPTIVE_SYNC_DATA, containing the information about the values of parameters that are to be retrieved on given display.
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status. If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
///////////////////////////////////////////////////////////////////////////////
//! \ingroup dispcontrol
NVAPI_INTERFACE NvAPI_DISP_GetAdaptiveSyncData(__in NvU32 displayId, __inout NV_GET_ADAPTIVE_SYNC_DATA *pAdaptiveSyncData);

typedef struct _NV_SET_ADAPTIVE_SYNC_DATA_V1
{
    NvU32  version ;                      //!< [in]    structure version
    NvU32  maxFrameInterval;              //!< [in]    maximum frame interval in micro seconds.
                                          //!<         If maxFrameInterval is send as 0, default values from EDID will be used.
    NvU32  bDisableAdaptiveSync : 1;      //!< [in]    Indicates if adaptive sync is disabled on the display.
    NvU32  bDisableFrameSplitting : 1;    //!< [in]    Indicates if Frame Splitting should be disabled.
    NvU32  reserved : 30;                 //!< reserved for future use.
    NvU32  reservedEx[7];                 //!< reserved for future use.
} NV_SET_ADAPTIVE_SYNC_DATA_V1;

#define NV_SET_ADAPTIVE_SYNC_DATA_VER1  MAKE_NVAPI_VERSION(NV_SET_ADAPTIVE_SYNC_DATA_V1,1)
#define NV_SET_ADAPTIVE_SYNC_DATA_VER   NV_SET_ADAPTIVE_SYNC_DATA_VER1

typedef NV_SET_ADAPTIVE_SYNC_DATA_V1  NV_SET_ADAPTIVE_SYNC_DATA;

///////////////////////////////////////////////////////////////////////////////
//
//! \fn NvAPI_DISP_SetAdaptiveSyncData(__in NvU32 displayId, __in NV_SET_ADAPTIVE_SYNC_DATA *pAdaptiveSyncData)
//! \code
//! DESCRIPTION:    This function is used to set data for Adaptive Sync Display.
//!
//! SUPPORTED OS:  Windows 10 and higher
//!
//! \since Release: 415
//!
//! \param [in]       displayId            - display id of the display
//! \param [in]       pAdaptiveSyncData    - A pointer to NV_SET_ADAPTIVE_SYNC_DATA, containing the information about the values of parameters that are to be set on given display.
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status. If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
///////////////////////////////////////////////////////////////////////////////
//! \ingroup dispcontrol
NVAPI_INTERFACE NvAPI_DISP_SetAdaptiveSyncData(__in NvU32 displayId, __in NV_SET_ADAPTIVE_SYNC_DATA *pAdaptiveSyncData);




////////////////////////////////////////////////////////////////////////////////////////
//
// MOSAIC allows a multi display target output scanout on a single source. 
//
// SAMPLE of MOSAIC 1x4 topo with 8 pixel horizontal overlap
//
//+-------------------------++-------------------------++-------------------------++-------------------------+
//|                         ||                         ||                         ||                         |
//|                         ||                         ||                         ||                         |
//|                         ||                         ||                         ||                         |
//|        DVI1             ||           DVI2          ||         DVI3            ||          DVI4           |
//|                         ||                         ||                         ||                         |
//|                         ||                         ||                         ||                         |
//|                         ||                         ||                         ||                         |
//|                         ||                         ||                         ||                         |
//+-------------------------++-------------------------++-------------------------++-------------------------+


//! \addtogroup mosaicapi
//! @{

#define NVAPI_MAX_MOSAIC_DISPLAY_ROWS       8
#define NVAPI_MAX_MOSAIC_DISPLAY_COLUMNS    8
//
// These bits are used to describe the validity of a topo.
//
#define NV_MOSAIC_TOPO_VALIDITY_VALID               0x00000000  //!< The topology is valid
#define NV_MOSAIC_TOPO_VALIDITY_MISSING_GPU         0x00000001  //!< Not enough SLI GPUs were found to fill the entire
                                                                //! topology. hPhysicalGPU will be 0 for these.
#define NV_MOSAIC_TOPO_VALIDITY_MISSING_DISPLAY     0x00000002  //!< Not enough displays were found to fill the entire
                                                                //! topology. displayOutputId will be 0 for these.
#define NV_MOSAIC_TOPO_VALIDITY_MIXED_DISPLAY_TYPES 0x00000004  //!< The topoogy is only possible with displays of the same
                                                                //! NV_GPU_OUTPUT_TYPE. Check displayOutputIds to make
                                                                //! sure they are all CRTs, or all DFPs.


//
//! This structure defines the topology details.
typedef struct 
{
    NvU32                version;              //!< Version of this structure
    NvLogicalGpuHandle   hLogicalGPU;          //!< Logical GPU for this topology 
    NvU32                validityMask;         //!< 0 means topology is valid with the current hardware.
                                               //! If not 0, inspect bits against NV_MOSAIC_TOPO_VALIDITY_*.
    NvU32                rowCount;             //!< Number of displays in a row
    NvU32                colCount;             //!< Number of displays in a column

    struct 
    {
        NvPhysicalGpuHandle hPhysicalGPU;      //!< Physical GPU to be used in the topology (0 if GPU missing)
        NvU32               displayOutputId;   //!< Connected display target (0 if no display connected)
        NvS32               overlapX;          //!< Pixels of overlap on left of target: (+overlap, -gap)
        NvS32               overlapY;          //!< Pixels of overlap on top of target: (+overlap, -gap)

    } gpuLayout[NVAPI_MAX_MOSAIC_DISPLAY_ROWS][NVAPI_MAX_MOSAIC_DISPLAY_COLUMNS];

} NV_MOSAIC_TOPO_DETAILS;

//! Macro for constructing te vesion field of NV_MOSAIC_TOPO_DETAILS
#define NVAPI_MOSAIC_TOPO_DETAILS_VER         MAKE_NVAPI_VERSION(NV_MOSAIC_TOPO_DETAILS,1)


//
//! These values refer to the different types of Mosaic topologies that are possible.  When
//! getting the supported Mosaic topologies, you can specify one of these types to narrow down
//! the returned list to only those that match the given type.
typedef enum
{
    NV_MOSAIC_TOPO_TYPE_ALL,                          //!< All mosaic topologies
    NV_MOSAIC_TOPO_TYPE_BASIC,                        //!< Basic Mosaic topologies
    NV_MOSAIC_TOPO_TYPE_PASSIVE_STEREO,               //!< Passive Stereo topologies
    NV_MOSAIC_TOPO_TYPE_SCALED_CLONE,                 //!< Not supported at this time
    NV_MOSAIC_TOPO_TYPE_PASSIVE_STEREO_SCALED_CLONE,  //!< Not supported at this time
    NV_MOSAIC_TOPO_TYPE_MAX,                          //!< Always leave this at end of the enum
} NV_MOSAIC_TOPO_TYPE;


//
//! This is a complete list of supported Mosaic topologies.
//!
//! Using a "Basic" topology combines multiple monitors to create a single desktop.
//!
//! Using a "Passive" topology combines multiples monitors to create a passive stereo desktop.
//! In passive stereo, two identical topologies combine - one topology is used for the right eye and the other identical //! topology (targeting different displays) is used for the left eye.  \n  
//! NOTE: common\inc\nvEscDef.h shadows a couple PASSIVE_STEREO enums.  If this
//!       enum list changes and effects the value of NV_MOSAIC_TOPO_BEGIN_PASSIVE_STEREO
//!       please update the corresponding value in nvEscDef.h
typedef enum
{
    NV_MOSAIC_TOPO_NONE,

    // 'BASIC' topos start here
    //
    // The result of using one of these Mosaic topos is that multiple monitors
    // will combine to create a single desktop.
    //
    NV_MOSAIC_TOPO_BEGIN_BASIC,
    NV_MOSAIC_TOPO_1x2_BASIC = NV_MOSAIC_TOPO_BEGIN_BASIC,
    NV_MOSAIC_TOPO_2x1_BASIC,
    NV_MOSAIC_TOPO_1x3_BASIC,
    NV_MOSAIC_TOPO_3x1_BASIC,
    NV_MOSAIC_TOPO_1x4_BASIC,
    NV_MOSAIC_TOPO_4x1_BASIC,
    NV_MOSAIC_TOPO_2x2_BASIC,
    NV_MOSAIC_TOPO_2x3_BASIC,
    NV_MOSAIC_TOPO_2x4_BASIC,
    NV_MOSAIC_TOPO_3x2_BASIC,
    NV_MOSAIC_TOPO_4x2_BASIC,
    NV_MOSAIC_TOPO_1x5_BASIC,
    NV_MOSAIC_TOPO_1x6_BASIC,
    NV_MOSAIC_TOPO_7x1_BASIC,

    // Add padding for 10 more entries. 6 will be enough room to specify every
    // possible topology with 8 or fewer displays, so this gives us a little
    // extra should we need it.
    NV_MOSAIC_TOPO_END_BASIC = NV_MOSAIC_TOPO_7x1_BASIC + 9,

    // 'PASSIVE_STEREO' topos start here
    //
    // The result of using one of these Mosaic topos is that multiple monitors
    // will combine to create a single PASSIVE STEREO desktop.  What this means is
    // that there will be two topos that combine to create the overall desktop.
    // One topo will be used for the left eye, and the other topo (of the
    // same rows x cols), will be used for the right eye.  The difference between
    // the two topos is that different GPUs and displays will be used.
    //
    NV_MOSAIC_TOPO_BEGIN_PASSIVE_STEREO,    // value shadowed in nvEscDef.h
    NV_MOSAIC_TOPO_1x2_PASSIVE_STEREO = NV_MOSAIC_TOPO_BEGIN_PASSIVE_STEREO,
    NV_MOSAIC_TOPO_2x1_PASSIVE_STEREO,
    NV_MOSAIC_TOPO_1x3_PASSIVE_STEREO,
    NV_MOSAIC_TOPO_3x1_PASSIVE_STEREO,
    NV_MOSAIC_TOPO_1x4_PASSIVE_STEREO,
    NV_MOSAIC_TOPO_4x1_PASSIVE_STEREO,
    NV_MOSAIC_TOPO_2x2_PASSIVE_STEREO,
    NV_MOSAIC_TOPO_END_PASSIVE_STEREO = NV_MOSAIC_TOPO_2x2_PASSIVE_STEREO + 4,


    //
    // Total number of topos.  Always leave this at the end of the enumeration.
    //
    NV_MOSAIC_TOPO_MAX  //! Total number of topologies.

} NV_MOSAIC_TOPO;


//
//! This is a "topology brief" structure.  It tells you what you need to know about
//! a topology at a high level. A list of these is returned when you query for the
//! supported Mosaic information.
//!
//! If you need more detailed information about the topology, call
//! NvAPI_Mosaic_GetTopoGroup() with the topology value from this structure.
typedef struct 
{
    NvU32                        version;            //!< Version of this structure
    NV_MOSAIC_TOPO               topo;               //!< The topology
    NvU32                        enabled;            //!< 1 if topo is enabled, else 0
    NvU32                        isPossible;         //!< 1 if topo *can* be enabled, else 0

} NV_MOSAIC_TOPO_BRIEF;

//! Macro for constructing the version field of NV_MOSAIC_TOPO_BRIEF
#define NVAPI_MOSAIC_TOPO_BRIEF_VER         MAKE_NVAPI_VERSION(NV_MOSAIC_TOPO_BRIEF,1)


//
//! Basic per-display settings that are used in setting/getting the Mosaic mode
typedef struct _NV_MOSAIC_DISPLAY_SETTING_V1
{
    NvU32                        version;            //!< Version of this structure
    NvU32                        width;              //!< Per-display width
    NvU32                        height;             //!< Per-display height
    NvU32                        bpp;                //!< Bits per pixel
    NvU32                        freq;               //!< Display frequency
} NV_MOSAIC_DISPLAY_SETTING_V1;

typedef struct NV_MOSAIC_DISPLAY_SETTING_V2
{
    NvU32                        version;            //!< Version of this structure
    NvU32                        width;              //!< Per-display width
    NvU32                        height;             //!< Per-display height
    NvU32                        bpp;                //!< Bits per pixel
    NvU32                        freq;               //!< Display frequency
    NvU32                        rrx1k;              //!< Display frequency in x1k
} NV_MOSAIC_DISPLAY_SETTING_V2;

typedef NV_MOSAIC_DISPLAY_SETTING_V2 NV_MOSAIC_DISPLAY_SETTING;

//! Macro for constructing the version field of NV_MOSAIC_DISPLAY_SETTING
#define NVAPI_MOSAIC_DISPLAY_SETTING_VER1         MAKE_NVAPI_VERSION(NV_MOSAIC_DISPLAY_SETTING_V1,1)
#define NVAPI_MOSAIC_DISPLAY_SETTING_VER2         MAKE_NVAPI_VERSION(NV_MOSAIC_DISPLAY_SETTING_V2,2)
#define NVAPI_MOSAIC_DISPLAY_SETTING_VER          NVAPI_MOSAIC_DISPLAY_SETTING_VER2


//
// Set a reasonable max number of display settings to support
// so arrays are bound.
//
#define NV_MOSAIC_DISPLAY_SETTINGS_MAX 40  //!< Set a reasonable maximum number of display settings to support
                                           //! so arrays are bound.


//
//! This structure is used to contain a list of supported Mosaic topologies
//! along with the display settings that can be used.
typedef struct _NV_MOSAIC_SUPPORTED_TOPO_INFO_V1
{
    NvU32                          version;                                         //!< Version of this structure
    NvU32                          topoBriefsCount;                                 //!< Number of topologies in below array
    NV_MOSAIC_TOPO_BRIEF           topoBriefs[NV_MOSAIC_TOPO_MAX];                  //!< List of supported topologies with only brief details
    NvU32                          displaySettingsCount;                            //!< Number of display settings in below array
    NV_MOSAIC_DISPLAY_SETTING_V1   displaySettings[NV_MOSAIC_DISPLAY_SETTINGS_MAX]; //!< List of per display settings possible

} NV_MOSAIC_SUPPORTED_TOPO_INFO_V1;

typedef struct _NV_MOSAIC_SUPPORTED_TOPO_INFO_V2
{
    NvU32                          version;                                         //!< Version of this structure
    NvU32                          topoBriefsCount;                                 //!< Number of topologies in below array
    NV_MOSAIC_TOPO_BRIEF           topoBriefs[NV_MOSAIC_TOPO_MAX];                  //!< List of supported topologies with only brief details
    NvU32                          displaySettingsCount;                            //!< Number of display settings in below array
    NV_MOSAIC_DISPLAY_SETTING_V2   displaySettings[NV_MOSAIC_DISPLAY_SETTINGS_MAX]; //!< List of per display settings possible

} NV_MOSAIC_SUPPORTED_TOPO_INFO_V2;

typedef NV_MOSAIC_SUPPORTED_TOPO_INFO_V2 NV_MOSAIC_SUPPORTED_TOPO_INFO;

//! Macro forconstructing  the version field of NV_MOSAIC_SUPPORTED_TOPO_INFO
#define NVAPI_MOSAIC_SUPPORTED_TOPO_INFO_VER1         MAKE_NVAPI_VERSION(NV_MOSAIC_SUPPORTED_TOPO_INFO_V1,1)
#define NVAPI_MOSAIC_SUPPORTED_TOPO_INFO_VER2         MAKE_NVAPI_VERSION(NV_MOSAIC_SUPPORTED_TOPO_INFO_V2,2)
#define NVAPI_MOSAIC_SUPPORTED_TOPO_INFO_VER          NVAPI_MOSAIC_SUPPORTED_TOPO_INFO_VER2


//
// Indices to use to access the topos array within the mosaic topology
#define NV_MOSAIC_TOPO_IDX_DEFAULT       0

#define NV_MOSAIC_TOPO_IDX_LEFT_EYE      0
#define NV_MOSAIC_TOPO_IDX_RIGHT_EYE     1
#define NV_MOSAIC_TOPO_NUM_EYES          2


//
//! This defines the maximum number of topos that can be in a topo group.
//! At this time, it is set to 2 because our largest topo group (passive
//! stereo) only needs 2 topos (left eye and right eye).
//!
//! If a new topo group with more than 2 topos is added above, then this
//! number will also have to be incremented.
#define NV_MOSAIC_MAX_TOPO_PER_TOPO_GROUP 2 


//
//! This structure defines a group of topologies that work together to create one
//! overall layout.  All of the supported topologies are represented with this
//! structure.
//!
//! For example, a 'Passive Stereo' topology would be represented with this
//! structure, and would have separate topology details for the left and right eyes.
//! The count would be 2.  A 'Basic' topology is also represented by this structure,
//! with a count of 1.
//!
//! The structure is primarily used internally, but is exposed to applications in a
//! read-only fashion because there are some details in it that might be useful
//! (like the number of rows/cols, or connected display information).  A user can
//! get the filled-in structure by calling NvAPI_Mosaic_GetTopoGroup().
//!
//! You can then look at the detailed values within the structure.  There are no
//! entrypoints which take this structure as input (effectively making it read-only).
typedef struct 
{
    NvU32                      version;              //!< Version of this structure
    NV_MOSAIC_TOPO_BRIEF       brief;                //!< The brief details of this topo
    NvU32                      count;                //!< Number of topos in array below
    NV_MOSAIC_TOPO_DETAILS     topos[NV_MOSAIC_MAX_TOPO_PER_TOPO_GROUP];

} NV_MOSAIC_TOPO_GROUP;

//! Macro for constructing the version field of NV_MOSAIC_TOPO_GROUP
#define NVAPI_MOSAIC_TOPO_GROUP_VER         MAKE_NVAPI_VERSION(NV_MOSAIC_TOPO_GROUP,1)

//! @}


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_Mosaic_GetSupportedTopoInfo
//
//! DESCRIPTION:     This API returns information on the topologies and display resolutions
//!                  supported by Mosaic mode.
//!
//!                  NOTE: Not all topologies returned can be set immediately.
//!                        See 'OUT' Notes below.
//!
//!                  Once you get the list of supported topologies, you can call
//!                  NvAPI_Mosaic_GetTopoGroup() with one of the Mosaic topologies if you need
//!                  more information about it.
//!
//!     <b>'IN' Notes:</b>  pSupportedTopoInfo->version must be set before calling this function.
//!                  If the specified version is not supported by this implementation,
//!                  an error will be returned (NVAPI_INCOMPATIBLE_STRUCT_VERSION).
//!
//!     <b>'OUT' Notes:</b> Some of the topologies returned might not be valid for one reason or
//!                  another.  It could be due to mismatched or missing displays.  It
//!                  could also be because the required number of GPUs is not found.
//!                  At a high level, you can see if the topology is valid and can be enabled
//!                  by looking at the pSupportedTopoInfo->topoBriefs[xxx].isPossible flag.
//!                  If this is true, the topology can be enabled. If it
//!                  is false, you can find out why it cannot be enabled by getting the
//!                  details of the topology via NvAPI_Mosaic_GetTopoGroup().  From there,
//!                  look at the validityMask of the individual topologies.  The bits can
//!                  be tested against the NV_MOSAIC_TOPO_VALIDITY_* bits.
//!
//!                  It is possible for this function to return NVAPI_OK with no topologies
//!                  listed in the return structure.  If this is the case, it means that
//!                  the current hardware DOES support Mosaic, but with the given configuration
//!                  no valid topologies were found.  This most likely means that SLI was not
//!                  enabled for the hardware. Once enabled, you should see valid topologies
//!                  returned from this function.
//!    
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 185
//!
//!
//! \param [in,out]  pSupportedTopoInfo  Information about what topologies and display resolutions
//!                                      are supported for Mosaic.
//! \param [in]      type                The type of topologies the caller is interested in
//!                                      getting. See NV_MOSAIC_TOPO_TYPE for possible values.
//!
//! \retval ::NVAPI_OK                          No errors in returning supported topologies.
//! \retval ::NVAPI_NOT_SUPPORTED               Mosaic is not supported with the existing hardware.
//! \retval ::NVAPI_INVALID_ARGUMENT            One or more arguments passed in are invalid.
//! \retval ::NVAPI_API_NOT_INTIALIZED          The NvAPI API needs to be initialized first.
//! \retval ::NVAPI_NO_IMPLEMENTATION           This entrypoint not available.
//! \retval ::NVAPI_INCOMPATIBLE_STRUCT_VERSION The version of the structure passed in is not
//                                              compatible with this entry point.
//! \retval ::NVAPI_ERROR:                      Miscellaneous error occurred.
//!
//! \ingroup mosaicapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Mosaic_GetSupportedTopoInfo(NV_MOSAIC_SUPPORTED_TOPO_INFO *pSupportedTopoInfo, NV_MOSAIC_TOPO_TYPE type);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_Mosaic_GetTopoGroup
//
//! DESCRIPTION:     This API returns a structure filled with the details
//!                  of the specified Mosaic topology.
//!
//!                  If the pTopoBrief passed in matches the current topology,
//!                  then information in the brief and group structures
//!                  will reflect what is current. Thus the brief would have
//!                  the current 'enable' status, and the group would have the
//!                  current overlap values. If there is no match, then the
//!                  returned brief has an 'enable' status of FALSE (since it
//!                  is obviously not enabled), and the overlap values will be 0.
//!
//!     <b>'IN' Notes:</b>  pTopoGroup->version must be set before calling this function.
//!                  If the specified version is not supported by this implementation,
//!                  an error will be returned (NVAPI_INCOMPATIBLE_STRUCT_VERSION).
//!
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 185
//!
//! \param [in]     pTopoBrief        The topology for getting the details
//!                                   This must be one of the topology briefs
//!                                   returned from NvAPI_Mosaic_GetSupportedTopoInfo().
//! \param [in,out] pTopoGroup        The topology details matching the brief
//!
//! \retval ::NVAPI_OK                          Details were retrieved successfully.
//! \retval ::NVAPI_NOT_SUPPORTED               Mosaic is not supported with the existing hardware.
//! \retval ::NVAPI_INVALID_ARGUMENT            One or more argumentss passed in are invalid.
//! \retval ::NVAPI_API_NOT_INTIALIZED          The NvAPI API needs to be initialized first.
//! \retval ::NVAPI_NO_IMPLEMENTATION           This entrypoint not available.
//! \retval ::NVAPI_INCOMPATIBLE_STRUCT_VERSION The version of the structure passed in is not
//                                              compatible with this entry point.
//! \retval ::NVAPI_ERROR:                      Miscellaneous error occurred.
//!
//! \ingroup mosaicapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Mosaic_GetTopoGroup(NV_MOSAIC_TOPO_BRIEF *pTopoBrief, NV_MOSAIC_TOPO_GROUP *pTopoGroup);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_Mosaic_GetOverlapLimits
//
//! DESCRIPTION:     This API returns the X and Y overlap limits required if
//!                  the given Mosaic topology and display settings are to be used.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 185
//!
//! \param [in]   pTopoBrief          The topology for getting limits
//!                                   This must be one of the topo briefs
//!                                   returned from NvAPI_Mosaic_GetSupportedTopoInfo().
//! \param [in]   pDisplaySetting     The display settings for getting the limits.
//!                                   This must be one of the settings
//!                                   returned from NvAPI_Mosaic_GetSupportedTopoInfo().
//! \param [out]  pMinOverlapX        X overlap minimum
//! \param [out]  pMaxOverlapX        X overlap maximum
//! \param [out]  pMinOverlapY        Y overlap minimum
//! \param [out]  pMaxOverlapY        Y overlap maximum
//!
//! \retval ::NVAPI_OK                          Details were retrieved successfully.
//! \retval ::NVAPI_NOT_SUPPORTED               Mosaic is not supported with the existing hardware.
//! \retval ::NVAPI_INVALID_ARGUMENT            One or more argumentss passed in are invalid.
//! \retval ::NVAPI_API_NOT_INTIALIZED          The NvAPI API needs to be initialized first.
//! \retval ::NVAPI_NO_IMPLEMENTATION           This entrypoint not available.
//! \retval ::NVAPI_INCOMPATIBLE_STRUCT_VERSION The version of the structure passed in is not
//!                                             compatible with this entry point.
//! \retval ::NVAPI_ERROR                       Miscellaneous error occurred.
//!
//! \ingroup mosaicapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Mosaic_GetOverlapLimits(NV_MOSAIC_TOPO_BRIEF *pTopoBrief, NV_MOSAIC_DISPLAY_SETTING *pDisplaySetting, NvS32 *pMinOverlapX, NvS32 *pMaxOverlapX, NvS32 *pMinOverlapY, NvS32 *pMaxOverlapY);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_Mosaic_SetCurrentTopo
//
//! DESCRIPTION:     This API sets the Mosaic topology and performs a mode switch 
//!                  using the given display settings.
//!
//!                  If NVAPI_OK is returned, the current Mosaic topology was set
//!                  correctly.  Any other status returned means the
//!                  topology was not set, and remains what it was before this
//!                  function was called.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 185
//!
//! \param [in]     pTopoBrief        The topology to set. This must be one of the topologies returned from
//!                                   NvAPI_Mosaic_GetSupportedTopoInfo(), and it must have an isPossible value of 1.
//! \param [in]     pDisplaySetting   The per display settings to be used in the Mosaic mode. This must be one of the
//!                                   settings returned from NvAPI_Mosaic_GetSupportedTopoInfo().
//! \param [in]     overlapX          The pixel overlap to use between horizontal displays (use positive a number for
//!                                   overlap, or a negative number to create a gap.) If the overlap is out of bounds
//!                                   for what is possible given the topo and display setting, the overlap will be clamped.
//! \param [in]     overlapY          The pixel overlap to use between vertical displays (use positive a number for
//!                                   overlap, or a negative number to create a gap.) If the overlap is out of bounds for
//!                                   what is possible given the topo and display setting, the overlap will be clamped.
//! \param [in]     enable            If 1, the topology being set will also be enabled, meaning that the mode set will
//!                                   occur.  \n
//!                                   If 0, you don't want to be in Mosaic mode right now, but want to set the current
//!                                   Mosaic topology so you can enable it later with NvAPI_Mosaic_EnableCurrentTopo().
//!
//! \retval  ::NVAPI_OK                          The Mosaic topology was set.
//! \retval  ::NVAPI_NOT_SUPPORTED               Mosaic is not supported with the existing hardware.
//! \retval  ::NVAPI_INVALID_ARGUMENT            One or more argumentss passed in are invalid.
//! \retval  ::NVAPI_TOPO_NOT_POSSIBLE           The topology passed in is not currently possible.
//! \retval  ::NVAPI_API_NOT_INTIALIZED          The NvAPI API needs to be initialized first.
//! \retval  ::NVAPI_NO_IMPLEMENTATION           This entrypoint not available.
//! \retval  ::NVAPI_INCOMPATIBLE_STRUCT_VERSION The version of the structure passed in is not
//!                                              compatible with this entrypoint.
//! \retval  ::NVAPI_MODE_CHANGE_FAILED          There was an error changing the display mode.
//! \retval  ::NVAPI_ERROR                       Miscellaneous error occurred.
//!
//! \ingroup mosaicapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Mosaic_SetCurrentTopo(NV_MOSAIC_TOPO_BRIEF *pTopoBrief, NV_MOSAIC_DISPLAY_SETTING *pDisplaySetting, NvS32 overlapX, NvS32 overlapY, NvU32 enable);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_Mosaic_GetCurrentTopo
//
//! DESCRIPTION:     This API returns information for the current Mosaic topology.
//!                  This includes topology, display settings, and overlap values.
//!
//!                  You can call NvAPI_Mosaic_GetTopoGroup() with the topology
//!                  if you require more information.
//!
//!                  If there isn't a current topology, then pTopoBrief->topo will
//!                  be NV_MOSAIC_TOPO_NONE.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 185
//!
//! \param [out]     pTopoBrief       The current Mosaic topology
//! \param [out]     pDisplaySetting  The current per-display settings
//! \param [out]     pOverlapX        The pixel overlap between horizontal displays
//! \param [out]     pOverlapY        The pixel overlap between vertical displays
//!
//! \retval ::NVAPI_OK                          Success getting current info.
//! \retval ::NVAPI_NOT_SUPPORTED               Mosaic is not supported with the existing hardware.
//! \retval ::NVAPI_INVALID_ARGUMENT            One or more argumentss passed in are invalid.
//! \retval ::NVAPI_API_NOT_INTIALIZED          The NvAPI API needs to be initialized first.
//! \retval ::NVAPI_NO_IMPLEMENTATION           This entry point not available.
//! \retval ::NVAPI_ERROR                       Miscellaneous error occurred.
//!
//! \ingroup mosaicapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Mosaic_GetCurrentTopo(NV_MOSAIC_TOPO_BRIEF *pTopoBrief, NV_MOSAIC_DISPLAY_SETTING *pDisplaySetting, NvS32 *pOverlapX, NvS32 *pOverlapY);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_Mosaic_EnableCurrentTopo
//
//! DESCRIPTION:     This API enables or disables the current Mosaic topology
//!                  based on the setting of the incoming 'enable' parameter.
//!
//!                  An "enable" setting enables the current (previously set) Mosaic topology.
//!                  Note that when the current Mosaic topology is retrieved, it must have an isPossible value of 1 or
//!                  an error will occur.
//!
//!                  A "disable" setting disables the current Mosaic topology.
//!                  The topology information will persist, even across reboots.
//!                  To re-enable the Mosaic topology, call this function
//!                  again with the enable parameter set to 1.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 185
//!
//! \param [in]   enable               1 to enable the current Mosaic topo, 0 to disable it.
//!
//! \retval ::NVAPI_OK                 The Mosaic topo was enabled/disabled.
//! \retval ::NVAPI_NOT_SUPPORTED      Mosaic is not supported with the existing hardware.
//! \retval ::NVAPI_INVALID_ARGUMENT   One or more arguments passed in are invalid.
//! \retval ::NVAPI_TOPO_NOT_POSSIBLE  The current topology is not currently possible.
//! \retval ::NVAPI_MODE_CHANGE_FAILED There was an error changing the display mode.
//! \retval ::NVAPI_ERROR:             Miscellaneous error occurred.
//!
//! \ingroup mosaicapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Mosaic_EnableCurrentTopo(NvU32 enable);

//! \ingroup mosaicapi
//! @{
typedef struct _NV_MOSAIC_GRID_TOPO_DISPLAY_V1
{
    NvU32                       displayId;              //!< DisplayID of the display
    NvS32                       overlapX;               //!< (+overlap, -gap)
    NvS32                       overlapY;               //!< (+overlap, -gap)
    NV_ROTATE                   rotation;               //!< Rotation of display
    NvU32                       cloneGroup;             //!< Reserved, must be 0
} NV_MOSAIC_GRID_TOPO_DISPLAY_V1;

typedef enum _NV_PIXEL_SHIFT_TYPE
{
    NV_PIXEL_SHIFT_TYPE_NO_PIXEL_SHIFT            = 0,          //!< No pixel shift will be applied to this display.
    NV_PIXEL_SHIFT_TYPE_2x2_TOP_LEFT_PIXELS       = 1,          //!< This display will be used to scanout top left pixels in 2x2 PixelShift configuration
    NV_PIXEL_SHIFT_TYPE_2x2_BOTTOM_RIGHT_PIXELS   = 2,          //!< This display will be used to scanout bottom right pixels in 2x2 PixelShift configuration
    NV_PIXEL_SHIFT_TYPE_2x2_TOP_RIGHT_PIXELS      = 4,          //!< This display will be used to scanout top right pixels in 2x2 PixelShift configuration
    NV_PIXEL_SHIFT_TYPE_2x2_BOTTOM_LEFT_PIXELS    = 8,          //!< This display will be used to scanout bottom left pixels in 2x2 PixelShift configuration
} NV_PIXEL_SHIFT_TYPE;

typedef struct _NV_MOSAIC_GRID_TOPO_DISPLAY_V2
{
    NvU32                               version;                //!< Version of this structure

    NvU32                               displayId;              //!< DisplayID of the display
    NvS32                               overlapX;               //!< (+overlap, -gap)
    NvS32                               overlapY;               //!< (+overlap, -gap)
    NV_ROTATE                           rotation;               //!< Rotation of display
    NvU32                               cloneGroup;             //!< Reserved, must be 0
    NV_PIXEL_SHIFT_TYPE                 pixelShiftType;         //!< Type of the pixel shift enabled display
} NV_MOSAIC_GRID_TOPO_DISPLAY_V2;

#ifndef NV_MOSAIC_GRID_TOPO_DISPLAY_VER

typedef NV_MOSAIC_GRID_TOPO_DISPLAY_V1           NV_MOSAIC_GRID_TOPO_DISPLAY;

#endif

typedef struct _NV_MOSAIC_GRID_TOPO_V1
{
    NvU32                          version;                            //!< Version of this structure
    NvU32                          rows;                               //!< Number of rows
    NvU32                          columns;                            //!< Number of columns
    NvU32                          displayCount;                       //!< Number of display details
    NvU32                          applyWithBezelCorrect : 1;          //!< When enabling and doing the modeset, do we switch to the bezel-corrected resolution
    NvU32                          immersiveGaming : 1;                //!< Enable as immersive gaming instead of Mosaic SLI (for Quadro-boards only)
    NvU32                          baseMosaic : 1;                     //!< Enable as Base Mosaic (Panoramic) instead of Mosaic SLI (for NVS and Quadro-boards only)
    NvU32                          driverReloadAllowed : 1;            //!< If necessary, reloading the driver is permitted (for Vista and above only). Will not be persisted. Value undefined on get.
    NvU32                          acceleratePrimaryDisplay : 1;       //!< Enable SLI acceleration on the primary display while in single-wide mode (For Immersive Gaming only). Will not be persisted. Value undefined on get.
    NvU32                          reserved : 27;                      //!< Reserved, must be 0
    NV_MOSAIC_GRID_TOPO_DISPLAY_V1 displays[NV_MOSAIC_MAX_DISPLAYS];   //!< Displays are done as [(row * columns) + column]
    NV_MOSAIC_DISPLAY_SETTING_V1   displaySettings;                    //!< Display settings
} NV_MOSAIC_GRID_TOPO_V1;

typedef struct _NV_MOSAIC_GRID_TOPO_V2
{
    NvU32                          version;                            //!< Version of this structure
    NvU32                          rows;                               //!< Number of rows
    NvU32                          columns;                            //!< Number of columns
    NvU32                          displayCount;                       //!< Number of display details
    NvU32                          applyWithBezelCorrect : 1;          //!< When enabling and doing the modeset, do we switch to the bezel-corrected resolution
    NvU32                          immersiveGaming : 1;                //!< Enable as immersive gaming instead of Mosaic SLI (for Quadro-boards only)
    NvU32                          baseMosaic : 1;                     //!< Enable as Base Mosaic (Panoramic) instead of Mosaic SLI (for NVS and Quadro-boards only)
    NvU32                          driverReloadAllowed : 1;            //!< If necessary, reloading the driver is permitted (for Vista and above only). Will not be persisted. Value undefined on get.
    NvU32                          acceleratePrimaryDisplay : 1;       //!< Enable SLI acceleration on the primary display while in single-wide mode (For Immersive Gaming only). Will not be persisted. Value undefined on get.
    NvU32                          pixelShift : 1;                     //!< Enable Pixel shift
    NvU32                          reserved : 26;                      //!< Reserved, must be 0
    NV_MOSAIC_GRID_TOPO_DISPLAY_V2 displays[NV_MOSAIC_MAX_DISPLAYS];   //!< Displays are done as [(row * columns) + column]
    NV_MOSAIC_DISPLAY_SETTING_V1   displaySettings;                    //!< Display settings
} NV_MOSAIC_GRID_TOPO_V2;

//! Macro for constructing the version field of ::NV_MOSAIC_GRID_TOPO
#define NV_MOSAIC_GRID_TOPO_VER1         MAKE_NVAPI_VERSION(NV_MOSAIC_GRID_TOPO_V1,1)
#define NV_MOSAIC_GRID_TOPO_VER2         MAKE_NVAPI_VERSION(NV_MOSAIC_GRID_TOPO_V2,2)
#ifndef NV_MOSAIC_GRID_TOPO_VER

typedef NV_MOSAIC_GRID_TOPO_V2           NV_MOSAIC_GRID_TOPO;

//! Macro for constructing the version field of ::NV_MOSAIC_GRID_TOPO
#define NV_MOSAIC_GRID_TOPO_VER          NV_MOSAIC_GRID_TOPO_VER2

#endif

//! @}

//! since Release R290

#define NV_MOSAIC_DISPLAYCAPS_PROBLEM_DISPLAY_ON_INVALID_GPU        NV_BIT(0)
#define NV_MOSAIC_DISPLAYCAPS_PROBLEM_DISPLAY_ON_WRONG_CONNECTOR    NV_BIT(1)
#define NV_MOSAIC_DISPLAYCAPS_PROBLEM_NO_COMMON_TIMINGS             NV_BIT(2)
#define NV_MOSAIC_DISPLAYCAPS_PROBLEM_NO_EDID_AVAILABLE             NV_BIT(3)
#define NV_MOSAIC_DISPLAYCAPS_PROBLEM_MISMATCHED_OUTPUT_TYPE        NV_BIT(4)
#define NV_MOSAIC_DISPLAYCAPS_PROBLEM_NO_DISPLAY_CONNECTED          NV_BIT(5)
#define NV_MOSAIC_DISPLAYCAPS_PROBLEM_NO_GPU_TOPOLOGY               NV_BIT(6)
#define NV_MOSAIC_DISPLAYCAPS_PROBLEM_NOT_SUPPORTED                 NV_BIT(7)
#define NV_MOSAIC_DISPLAYCAPS_PROBLEM_NO_SLI_BRIDGE                 NV_BIT(8)
#define NV_MOSAIC_DISPLAYCAPS_PROBLEM_ECC_ENABLED                   NV_BIT(9)
#define NV_MOSAIC_DISPLAYCAPS_PROBLEM_GPU_TOPOLOGY_NOT_SUPPORTED    NV_BIT(10)




///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_Mosaic_SetDisplayGrids
//
//! DESCRIPTION:     Sets a new display topology, replacing any existing topologies
//!                  that use the same displays.
//!
//!                  This function will look for an SLI configuration that will
//!                  allow the display topology to work.
//!
//!                  To revert to a single display, specify that display as a 1x1
//!                  grid.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]      pGridTopologies    The topology details to set.
//! \param [in]      gridCount          The number of elements in the pGridTopologies array.
//! \param [in]      setTopoFlags       Zero or more of the NVAPI_MOSAIC_SETDISPLAYTOPO_FLAG_*
//!                                     flags.
//!
//!
//! \retval ::NVAPI_OK                          Capabilities have been returned.
//! \retval ::NVAPI_INVALID_ARGUMENT            One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INTIALIZED          The NvAPI API needs to be initialized first
//! \retval ::NVAPI_NO_IMPLEMENTATION           This entrypoint not available
//! \retval ::NVAPI_NO_ACTIVE_SLI_TOPOLOGY      No matching GPU topologies could be found.
//! \retval ::NVAPI_TOPO_NOT_POSSIBLE           One or more of the display grids are not valid.
//! \retval ::NVAPI_ERROR                       Miscellaneous error occurred
//! \ingroup mosaicapi 
///////////////////////////////////////////////////////////////////////////////


//! Do not change the current GPU topology. If the NO_DRIVER_RELOAD bit is not
//! specified, then it may still require a driver reload.
#define NV_MOSAIC_SETDISPLAYTOPO_FLAG_CURRENT_GPU_TOPOLOGY NV_BIT(0)

//! Do not allow a driver reload. That is, stick with the same master GPU as well as the
//! same SLI configuration.
#define NV_MOSAIC_SETDISPLAYTOPO_FLAG_NO_DRIVER_RELOAD     NV_BIT(1)

//! When choosing a GPU topology, choose the topology with the best performance.
//! Without this flag, it will choose the topology that uses the smallest number
//! of GPU's.
#define NV_MOSAIC_SETDISPLAYTOPO_FLAG_MAXIMIZE_PERFORMANCE NV_BIT(2)

//! Do not return an error if no configuration will work with all of the grids.
#define NV_MOSAIC_SETDISPLAYTOPO_FLAG_ALLOW_INVALID        NV_BIT(3)

NVAPI_INTERFACE NvAPI_Mosaic_SetDisplayGrids(__in_ecount(gridCount) NV_MOSAIC_GRID_TOPO *pGridTopologies, __in NvU32 gridCount, __in NvU32 setTopoFlags);


//! \ingroup mosaicapi
//! Indicates that a display's position in the grid is sub-optimal.
#define NV_MOSAIC_DISPLAYTOPO_WARNING_DISPLAY_POSITION            NV_BIT(0)

//! \ingroup mosaicapi
//! Indicates that SetDisplaySettings would need to perform a driver reload.
#define NV_MOSAIC_DISPLAYTOPO_WARNING_DRIVER_RELOAD_REQUIRED      NV_BIT(1)

//! \ingroup mosaicapi
typedef struct
{
    NvU32 version;
    NvU32 errorFlags;            //!< (OUT) Any of the NV_MOSAIC_DISPLAYTOPO_ERROR_* flags.
    NvU32 warningFlags;          //!< (OUT) Any of the NV_MOSAIC_DISPLAYTOPO_WARNING_* flags.

    NvU32 displayCount;          //!< (OUT) The number of valid entries in the displays array.
    struct
    {
        NvU32 displayId;             //!< (OUT) The DisplayID of this display.
        NvU32 errorFlags;            //!< (OUT) Any of the NV_MOSAIC_DISPLAYCAPS_PROBLEM_* flags.
        NvU32 warningFlags;          //!< (OUT) Any of the NV_MOSAIC_DISPLAYTOPO_WARNING_* flags.

        NvU32  supportsRotation : 1; //!< (OUT) This display can be rotated
        NvU32  reserved : 31;        //!< (OUT) reserved
    } displays[NVAPI_MAX_DISPLAYS];
} NV_MOSAIC_DISPLAY_TOPO_STATUS;

//! \ingroup mosaicapi
#define NV_MOSAIC_DISPLAY_TOPO_STATUS_VER         MAKE_NVAPI_VERSION(NV_MOSAIC_DISPLAY_TOPO_STATUS,1)


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_Mosaic_ValidateDisplayGrids
//
//! DESCRIPTION:     Determines if a list of grid topologies is valid. It will choose an SLI
//!                  configuration in the same way that NvAPI_Mosaic_SetDisplayGrids() does.
//!
//!                  On return, each element in the pTopoStatus array will contain any errors or
//!                  warnings about each grid topology. If any error flags are set, then the topology
//!                  is not valid. If any warning flags are set, then the topology is valid, but
//!                  sub-optimal.
//!
//!                  If the ALLOW_INVALID flag is set, then it will continue to validate the grids
//!                  even if no SLI configuration will allow all of the grids. In this case, a grid
//!                  grid with no matching GPU topology will have the error
//!                  flags NO_GPU_TOPOLOGY or NOT_SUPPORTED set.
//!
//!                  If the ALLOW_INVALID flag is not set and no matching SLI configuration is
//!                  found, then it will skip the rest of the validation and return
//!                  NVAPI_NO_ACTIVE_SLI_TOPOLOGY.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]      setTopoFlags       Zero or more of the NVAPI_MOSAIC_SETDISPLAYTOPO_FLAG_*
//!                                     flags.
//! \param [in]      pGridTopologies    The array of grid topologies to verify.
//! \param [in,out]  pTopoStatus        The array of problems and warnings with each grid topology.
//! \param [in]      gridCount          The number of elements in the pGridTopologies and
//!                                     pTopoStatus arrays.
//!
//!
//! \retval ::NVAPI_OK:                          Capabilities have been returned.
//! \retval ::NVAPI_INVALID_ARGUMENT:            One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INTIALIZED:          The NvAPI API needs to be initialized first
//! \retval ::NVAPI_NO_IMPLEMENTATION:           This entrypoint not available
//! \retval ::NVAPI_NO_ACTIVE_SLI_TOPOLOGY:      No matching GPU topologies could be found.
//! \retval ::NVAPI_ERROR:                       Miscellaneous error occurred
//!
//! \ingroup mosaicapi 
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Mosaic_ValidateDisplayGrids(__in NvU32 setTopoFlags,
        __in_ecount(gridCount) NV_MOSAIC_GRID_TOPO *pGridTopologies,
        __inout_ecount_full(gridCount) NV_MOSAIC_DISPLAY_TOPO_STATUS *pTopoStatus,
        __in NvU32 gridCount);



///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_Mosaic_EnumDisplayModes
//
//! DESCRIPTION:     Determines the set of available display modes for a given grid topology.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]      pGridTopology         The grid topology to use.
//! \param [in,out]  pDisplaySettings      A pointer to an array of display settings to populate,
//!                                        or NULL to find out the total number of available modes.
//! \param [in,out]  pDisplayCount         If pDisplaySettings is not NULL, then pDisplayCount
//!                                        should point to the number of elements in the
//!                                        pDisplaySettings array. On return, it will contain the
//!                                        number of modes that were actually returned. If
//!                                        pDisplaySettings is NULL, then pDisplayCount will receive
//!                                        the total number of modes that are available.
//!
//!
//! \retval ::NVAPI_OK                          Capabilities have been returned.
//! \retval ::NVAPI_INVALID_ARGUMENT            One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INTIALIZED          The NvAPI API needs to be initialized first
//! \retval ::NVAPI_NO_IMPLEMENTATION           This entrypoint not available
//! \retval ::NVAPI_ERROR                       Miscellaneous error occurred
//!
//! \ingroup mosaciapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Mosaic_EnumDisplayModes(__in NV_MOSAIC_GRID_TOPO *pGridTopology,
        __inout_ecount_part_opt(*pDisplayCount, *pDisplayCount) NV_MOSAIC_DISPLAY_SETTING *pDisplaySettings,
        __inout NvU32 *pDisplayCount);


//! SUPPORTED OS:  Windows 7 and higher
//!
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_Mosaic_EnumDisplayGrids
//
//! DESCRIPTION:     Enumerates the current active grid topologies. This includes Mosaic, IG, and
//!                  Panoramic topologies, as well as single displays.
//!
//!                  If pGridTopologies is NULL, then pGridCount will be set to the number of active
//!                  grid topologies.
//!
//!                  If pGridTopologies is not NULL, then pGridCount contains the maximum number of
//!                  grid topologies to return. On return, pGridCount will be set to the number of
//!                  grid topologies that were returned.
//!
//! \param [out]     pGridTopologies   The list of active grid topologies.
//! \param [in,out]  pGridCount        A pointer to the number of grid topologies returned.
//!
//! \retval ::NVAPI_OK                          Capabilties have been returned.
//! \retval ::NVAPI_END_ENUMERATION             There are no more topologies to return.
//! \retval ::NVAPI_INVALID_ARGUMENT            One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INTIALIZED          The NvAPI API needs to be initialized first
//! \retval ::NVAPI_NO_IMPLEMENTATION           This entrypoint not available
//! \retval ::NVAPI_ERROR                       Miscellaneous error occurred
//!
//! \ingroup mosaicapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Mosaic_EnumDisplayGrids(__inout_ecount_part_opt(*pGridCount, *pGridCount) NV_MOSAIC_GRID_TOPO *pGridTopologies,
        __inout NvU32 *pGridCount);


////////////////////////////////////////////////////////////////////////////////////////
//
// ###########################################################################
// DELME_RUSS - DELME_RUSS - DELME_RUSS - DELME_RUSS - DELME_RUSS - DELME_RUSS
//
//   Below is the Phase 1 Mosaic stuff, the Phase 2 stuff above is what will remain
//   once Phase 2 is complete.  For a small amount of time, the two will co-exist.  As
//   soon as apps (nvapichk, NvAPITestMosaic, and CPL) are updated to use the Phase 2
//   entrypoints, the code below will be deleted.
//
// DELME_RUSS - DELME_RUSS - DELME_RUSS - DELME_RUSS - DELME_RUSS - DELME_RUSS
// ###########################################################################
//
// Supported topos 1x4, 4x1 and 2x2 to start with.
// 
// Selected scan out targets can be one per GPU or more than one on the same GPU.
//
// SAMPLE of MOSAIC 1x4 SCAN OUT TOPO with 8 pixel horizontal overlap
//
//+-------------------------++-------------------------++-------------------------++-------------------------+
//|                         ||                         ||                         ||                         |
//|                         ||                         ||                         ||                         |
//|                         ||                         ||                         ||                         |
//|        DVI1             ||           DVI2          ||         DVI3            ||          DVI4           |
//|                         ||                         ||                         ||                         |
//|                         ||                         ||                         ||                         |
//|                         ||                         ||                         ||                         |
//|                         ||                         ||                         ||                         |
//+-------------------------++-------------------------++-------------------------++-------------------------+


//! \addtogroup mosaicapi
//! @{

//! Used in NV_MOSAIC_TOPOLOGY.
#define NVAPI_MAX_MOSAIC_DISPLAY_ROWS       8

//! Used in NV_MOSAIC_TOPOLOGY.
#define NVAPI_MAX_MOSAIC_DISPLAY_COLUMNS    8 

//! Used in NV_MOSAIC_TOPOLOGY.
#define NVAPI_MAX_MOSAIC_TOPOS              16

//! Used in NvAPI_GetCurrentMosaicTopology() and NvAPI_SetCurrentMosaicTopology().
typedef struct 
{
    NvU32 version;                             //!< Version number of the mosaic topology
    NvU32 rowCount;                            //!< Horizontal display count
    NvU32 colCount;                            //!< Vertical display count

    struct 
    {
        NvPhysicalGpuHandle hPhysicalGPU;      //!< Physical GPU to be used in the topology
        NvU32               displayOutputId;   //!< Connected display target
        NvS32               overlapX;          //!< Pixels of overlap on the left of target: (+overlap, -gap)
        NvS32               overlapY;          //!< Pixels of overlap on the top of target: (+overlap, -gap)

    } gpuLayout[NVAPI_MAX_MOSAIC_DISPLAY_ROWS][NVAPI_MAX_MOSAIC_DISPLAY_COLUMNS];

} NV_MOSAIC_TOPOLOGY;

//! Used in NV_MOSAIC_TOPOLOGY.
#define NVAPI_MOSAIC_TOPOLOGY_VER         MAKE_NVAPI_VERSION(NV_MOSAIC_TOPOLOGY,1)

//! Used in NvAPI_GetSupportedMosaicTopologies().
typedef struct 
{
    NvU32                   version;                                    
    NvU32                   totalCount;                     //!< Count of valid topologies
    NV_MOSAIC_TOPOLOGY      topos[NVAPI_MAX_MOSAIC_TOPOS];  //!< Maximum number of topologies

} NV_MOSAIC_SUPPORTED_TOPOLOGIES;

//! Used in NV_MOSAIC_SUPPORTED_TOPOLOGIES. 
#define NVAPI_MOSAIC_SUPPORTED_TOPOLOGIES_VER         MAKE_NVAPI_VERSION(NV_MOSAIC_SUPPORTED_TOPOLOGIES,1)

//!@}


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GetSupportedMosaicTopologies
//
//! DESCRIPTION:     This API returns all valid Mosaic topologies.
//!
//! SUPPORTED OS:  Windows XP
//!
//!
//! \since Release: 177
//!
//! \param [out] pMosaicTopos                   An array of valid Mosaic topologies.
//!
//! \retval      NVAPI_OK                       Call succeeded; 1 or more topologies were returned
//! \retval      NVAPI_INVALID_ARGUMENT         One or more arguments are invalid
//! \retval      NVAPI_MIXED_TARGET_TYPES       Mosaic topology is only possible with all targets of the same NV_GPU_OUTPUT_TYPE.
//! \retval      NVAPI_NVIDIA_DEVICE_NOT_FOUND  No NVIDIA GPU driving a display was found
//! \retval      NVAPI_NOT_SUPPORTED            Mosaic is not supported with GPUs on this system.
//! \retval      NVAPI_NO_ACTIVE_SLI_TOPOLOGY   SLI is not enabled, yet needs to be, in order for this function to succeed.
//!
//! \ingroup     mosaicapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetSupportedMosaicTopologies(NV_MOSAIC_SUPPORTED_TOPOLOGIES *pMosaicTopos);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_GetCurrentMosaicTopology
//
//! DESCRIPTION:     This API gets the current Mosaic topology.
//!
//! SUPPORTED OS:  Windows XP
//!
//!
//! \since Release: 177
//!
//! \param [out] pMosaicTopo                    The current Mosaic topology
//! \param [out] pEnabled                       TRUE if returned topology is currently enabled, else FALSE
//!
//! \retval      NVAPI_OK                       Call succeeded
//! \retval      NVAPI_INVALID_ARGUMENT         One or more arguments are invalid
//! \retval      NVAPI_NVIDIA_DEVICE_NOT_FOUND  No NVIDIA GPU driving a display was found
//! \retval      NVAPI_NOT_SUPPORTED            Mosaic is not supported with GPUs on this system.
//! \retval      NVAPI_NO_ACTIVE_SLI_TOPOLOGY   SLI is not enabled, yet needs to be, in order for this function to succeed.
//!
//! \ingroup     mosaicapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GetCurrentMosaicTopology(NV_MOSAIC_TOPOLOGY *pMosaicTopo, NvU32 *pEnabled);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_SetCurrentMosaicTopology
//
//! DESCRIPTION:     This API sets the Mosaic topology, and enables it so that the
//!                  Mosaic display settings are enumerated upon request.
//!
//! SUPPORTED OS:  Windows XP
//!
//!
//! \since Release: 177
//!
//! \param [in]  pMosaicTopo                    A valid Mosaic topology
//!
//! \retval      NVAPI_OK                       Call succeeded
//! \retval      NVAPI_INVALID_ARGUMENT         One or more arguments are invalid
//! \retval      NVAPI_NVIDIA_DEVICE_NOT_FOUND  No NVIDIA GPU driving a display was found
//! \retval      NVAPI_NOT_SUPPORTED            Mosaic is not supported with GPUs on this system.
//! \retval      NVAPI_NO_ACTIVE_SLI_TOPOLOGY   SLI is not enabled, yet needs to be, in order for this function to succeed.
//!
//! \ingroup     mosaicapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_SetCurrentMosaicTopology(NV_MOSAIC_TOPOLOGY *pMosaicTopo);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_EnableCurrentMosaicTopology
//
//! DESCRIPTION:    This API enables or disables the current Mosaic topology. 
//!                 When enabling, the last Mosaic topology will be set.
//!
//!                  - If enabled, enumeration of display settings will include valid Mosaic resolutions.  
//!                  - If disabled, enumeration of display settings will not include Mosaic resolutions.
//!
//! SUPPORTED OS:  Windows XP
//!
//!
//! \since Release: 177
//!
//! \param [in]  enable                         TRUE to enable the Mosaic Topology, FALSE to disable it.
//!
//! \retval      NVAPI_OK                       Call succeeded
//! \retval      NVAPI_INVALID_ARGUMENT         One or more arguments are invalid
//! \retval      NVAPI_NVIDIA_DEVICE_NOT_FOUND  No NVIDIA GPU driving a display was found
//! \retval      NVAPI_NOT_SUPPORTED            Mosaic is not supported with GPUs on this system.
//! \retval      NVAPI_NO_ACTIVE_SLI_TOPOLOGY   SLI is not enabled, yet needs to be, in order for this function to succeed.
//!
//! \ingroup     mosaicapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_EnableCurrentMosaicTopology(NvU32 enable);


#define NVAPI_MAX_GSYNC_DEVICES                       4


// Sync Display APIs

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GSync_EnumSyncDevices
//
//!   DESCRIPTION: This API returns an array of Sync device handles. A Sync device handle represents a
//!                single Sync device on the system.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//! \param [out] nvGSyncHandles-  The caller provides an array of handles, which must contain at least
//!                               NVAPI_MAX_GSYNC_DEVICES elements. The API will zero out the entire array and then fill in one
//!                               or more handles. If an error occurs, the array is invalid.
//! \param [out] *gsyncCount-     The caller provides the storage space. NvAPI_GSync_EnumSyncDevices
//!                               sets *gsyncCount to indicate how many of the elements in the nvGSyncHandles[] array are valid.
//!                               If an error occurs, *gsyncCount will be set to zero.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. 
//!          If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::NVAPI_INVALID_ARGUMENT         nvGSyncHandles or gsyncCount is NULL.
//! \retval ::NVAPI_NVIDIA_DEVICE_NOT_FOUND  The queried Graphics system does not have any Sync Device.
//!
//! \ingroup gsyncapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GSync_EnumSyncDevices(__out NvGSyncDeviceHandle nvGSyncHandles[NVAPI_MAX_GSYNC_DEVICES], __out NvU32 *gsyncCount);


// GSync boardId values
#define NVAPI_GSYNC_BOARD_ID_P358 856		//!< GSync board ID 0x358, see NV_GSYNC_CAPABILITIES
#define NVAPI_GSYNC_BOARD_ID_P2060 8288		//!< GSync board ID 0x2060, see NV_GSYNC_CAPABILITIES 

//! \since Release: 375
#define NVAPI_GSYNC_BOARD_ID_P2061 8289		//!< GSync board ID 0x2061, see NV_GSYNC_CAPABILITIES 


//! Used in NvAPI_GSync_QueryCapabilities().
typedef struct _NV_GSYNC_CAPABILITIES_V1
{
    NvU32   version;                        //!< Version of the structure
    NvU32   boardId;                        //!< Board ID
    NvU32   revision;                       //!< FPGA Revision
    NvU32   capFlags;                       //!< Capabilities of the Sync board. Reserved for future use
} NV_GSYNC_CAPABILITIES_V1;

typedef struct _NV_GSYNC_CAPABILITIES_V2
{
    NvU32   version;                        //!< Version of the structure
    NvU32   boardId;                        //!< Board ID
    NvU32   revision;                       //!< FPGA major revision
    NvU32   capFlags;                       //!< Capabilities of the Sync board. Reserved for future use
    NvU32   extendedRevision;               //!< FPGA minor revision
} NV_GSYNC_CAPABILITIES_V2;

typedef NV_GSYNC_CAPABILITIES_V2 NV_GSYNC_CAPABILITIES;


//! \ingroup gsyncapi
//! Macro for constructing the version field of NV_GSYNC_CAPABILITIES.
#define NV_GSYNC_CAPABILITIES_VER1  MAKE_NVAPI_VERSION(NV_GSYNC_CAPABILITIES_V1,1)
#define NV_GSYNC_CAPABILITIES_VER2  MAKE_NVAPI_VERSION(NV_GSYNC_CAPABILITIES_V2,2)
#define NV_GSYNC_CAPABILITIES_VER NV_GSYNC_CAPABILITIES_VER2


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GSync_QueryCapabilities
//
//!   DESCRIPTION: This API returns the capabilities of the Sync device.
//!
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//! \param [in]    hNvGSyncDevice-        The handle for a Sync device for which the capabilities will be queried.
//! \param [inout] *pNvGSyncCapabilities- The caller provides the storage space. NvAPI_GSync_QueryCapabilities() sets
//!                                       *pNvGSyncCapabilities to the version and capabilities details of the Sync device
//!                                       If an error occurs, *pNvGSyncCapabilities will be set to NULL.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. 
//!          If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::NVAPI_INVALID_ARGUMENT         hNvGSyncDevice is NULL.
//! \retval ::NVAPI_NVIDIA_DEVICE_NOT_FOUND  The queried Graphics system does not have any Sync Device.
//!
//! \ingroup gsyncapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GSync_QueryCapabilities(__in NvGSyncDeviceHandle hNvGSyncDevice, __inout NV_GSYNC_CAPABILITIES *pNvGSyncCapabilities);



//! Connector values for a GPU. Used in NV_GSYNC_GPU.
typedef enum _NVAPI_GSYNC_GPU_TOPOLOGY_CONNECTOR
{
    NVAPI_GSYNC_GPU_TOPOLOGY_CONNECTOR_NONE         = 0,
    NVAPI_GSYNC_GPU_TOPOLOGY_CONNECTOR_PRIMARY      = 1,
    NVAPI_GSYNC_GPU_TOPOLOGY_CONNECTOR_SECONDARY    = 2,
    NVAPI_GSYNC_GPU_TOPOLOGY_CONNECTOR_TERTIARY     = 3,
    NVAPI_GSYNC_GPU_TOPOLOGY_CONNECTOR_QUARTERNARY  = 4,
} NVAPI_GSYNC_GPU_TOPOLOGY_CONNECTOR;

//! Display sync states. Used in NV_GSYNC_DISPLAY.
typedef enum _NVAPI_GSYNC_DISPLAY_SYNC_STATE
{
    NVAPI_GSYNC_DISPLAY_SYNC_STATE_UNSYNCED         = 0,
    NVAPI_GSYNC_DISPLAY_SYNC_STATE_SLAVE            = 1,
    NVAPI_GSYNC_DISPLAY_SYNC_STATE_MASTER           = 2,
} NVAPI_GSYNC_DISPLAY_SYNC_STATE;

typedef struct _NV_GSYNC_GPU
{
    NvU32                               version;            //!< Version of the structure
    NvPhysicalGpuHandle                 hPhysicalGpu;       //!< GPU handle
    NVAPI_GSYNC_GPU_TOPOLOGY_CONNECTOR  connector;          //!< Indicates which connector on the device the GPU is connected to.
    NvPhysicalGpuHandle                 hProxyPhysicalGpu;  //!< GPU through which hPhysicalGpu is connected to the Sync device (if not directly connected)
                                                            //!<  - this is NULL otherwise
    NvU32                               isSynced : 1;       //!< Whether this GPU is sync'd or not.
    NvU32                               reserved : 31;      //!< Should be set to ZERO
} NV_GSYNC_GPU;

typedef struct _NV_GSYNC_DISPLAY
{
    NvU32                               version;            //!< Version of the structure
    NvU32                               displayId;          //!< display identifier for displays.The GPU to which it is connected, can be retireved from NvAPI_SYS_GetPhysicalGpuFromDisplayId
    NvU32                               isMasterable : 1;   //!< Can this display be the master? (Read only)
    NvU32                               reserved : 31;      //!< Should be set to ZERO
    NVAPI_GSYNC_DISPLAY_SYNC_STATE      syncState;          //!< Is this display slave/master
                                                            //!< (Retrieved with topology or set by caller for enable/disable sync)
} NV_GSYNC_DISPLAY;

#define NV_GSYNC_DISPLAY_VER  MAKE_NVAPI_VERSION(NV_GSYNC_DISPLAY,1)
#define NV_GSYNC_GPU_VER      MAKE_NVAPI_VERSION(NV_GSYNC_GPU,1)


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GSync_GetTopology
//
//!   DESCRIPTION: This API returns the topology for the specified Sync device.
//!
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//! \param [in]       hNvGSyncDevice-     The caller provides the handle for a Sync device for which the topology will be queried.
//! \param [in, out]  gsyncGpuCount-      It returns number of GPUs connected to Sync device
//! \param [in, out]  gsyncGPUs-          It returns info about GPUs connected to Sync device
//! \param [in, out]  gsyncDisplayCount-  It returns number of active displays that belongs to Sync device
//! \param [in, out]  gsyncDisplays-      It returns info about all active displays that belongs to Sync device
//!
//! HOW TO USE: 1) make a call to get the number of GPUs connected OR displays synced through Sync device
//!                by passing the gsyncGPUs OR gsyncDisplays as NULL respectively. Both gsyncGpuCount and gsyncDisplayCount can be retrieved in same call by passing
//!                both gsyncGPUs and gsyncDisplays as NULL
//!                On call success:
//!             2) Allocate memory based on gsyncGpuCount(for gsyncGPUs) and/or gsyncDisplayCount(for gsyncDisplays) then make a call to populate gsyncGPUs and/or gsyncDisplays respectively.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. 
//!          If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::NVAPI_INVALID_ARGUMENT               hNvGSyncDevice is NULL.
//! \retval ::NVAPI_NVIDIA_DEVICE_NOT_FOUND        The queried Graphics system does not have any Sync Device.
//! \retval ::NVAPI_INSUFFICIENT_BUFFER            When the actual number of GPUs/displays in the topology exceed the number of elements allocated for SyncGPUs/SyncDisplays respectively.
//!
//! \ingroup gsyncapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GSync_GetTopology(__in NvGSyncDeviceHandle hNvGSyncDevice, __inout_opt NvU32 *gsyncGpuCount,  __inout_ecount_part_opt(*gsyncGpuCount, *gsyncGpuCount) NV_GSYNC_GPU *gsyncGPUs,
                                        __inout_opt NvU32 *gsyncDisplayCount, __inout_ecount_part_opt(*gsyncDisplayCount, *gsyncDisplayCount) NV_GSYNC_DISPLAY *gsyncDisplays);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GSync_SetSyncStateSettings
//
//!   DESCRIPTION: Sets a new sync state for the displays in system.
//!
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//! \param [in]  gsyncDisplayCount-			The number of displays in gsyncDisplays.
//! \param [in]  pGsyncDisplays-			The caller provides the structure containing all displays that need to be synchronized in the system. 
//!											The displays that are not part of pGsyncDisplays, will be un-synchronized.
//! \param [in]  flags-						Reserved for future use.
//!
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. 
//!          If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \retval ::NVAPI_INVALID_ARGUMENT			If the display topology or count not valid.
//! \retval ::NVAPI_NVIDIA_DEVICE_NOT_FOUND		The queried Graphics system does not have any Sync Device.
//! \retval ::NVAPI_INVALID_SYNC_TOPOLOGY       1.If any mosaic grid is partial.
//!                                             2.If timing(HVisible/VVisible/refreshRate) applied of any display is different. 
//!                                             3.If There is a across GPU mosaic grid in system and that is not a part of pGsyncDisplays.
//!
//! \ingroup gsyncapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GSync_SetSyncStateSettings(__in NvU32 gsyncDisplayCount, __in_ecount(gsyncDisplayCount) NV_GSYNC_DISPLAY *pGsyncDisplays, __in NvU32 flags);


//! \ingroup gsyncapi

//! Source signal edge to be used for output pulse. See NV_GSYNC_CONTROL_PARAMS.
typedef enum _NVAPI_GSYNC_POLARITY
{
    NVAPI_GSYNC_POLARITY_RISING_EDGE     = 0,
    NVAPI_GSYNC_POLARITY_FALLING_EDGE    = 1,
    NVAPI_GSYNC_POLARITY_BOTH_EDGES      = 2,
} NVAPI_GSYNC_POLARITY;

//! Used in NV_GSYNC_CONTROL_PARAMS.
typedef enum _NVAPI_GSYNC_VIDEO_MODE
{
    NVAPI_GSYNC_VIDEO_MODE_NONE          = 0,
    NVAPI_GSYNC_VIDEO_MODE_TTL           = 1,
    NVAPI_GSYNC_VIDEO_MODE_NTSCPALSECAM  = 2,
    NVAPI_GSYNC_VIDEO_MODE_HDTV          = 3,
    NVAPI_GSYNC_VIDEO_MODE_COMPOSITE     = 4,
} NVAPI_GSYNC_VIDEO_MODE;

//! Used in NV_GSYNC_CONTROL_PARAMS.  
typedef enum _NVAPI_GSYNC_SYNC_SOURCE
{
    NVAPI_GSYNC_SYNC_SOURCE_VSYNC        = 0,
    NVAPI_GSYNC_SYNC_SOURCE_HOUSESYNC    = 1,
} NVAPI_GSYNC_SYNC_SOURCE;

//! Used in NV_GSYNC_CONTROL_PARAMS. 
typedef struct _NV_GSYNC_DELAY
{
    NvU32        version;          //!< Version of the structure
    NvU32        numLines;         //!< delay to be induced in number of horizontal lines.
    NvU32        numPixels;        //!< delay to be induced in number of pixels.
    NvU32        maxLines;         //!< maximum number of lines supported at current display mode to induce delay. Updated by NvAPI_GSync_GetControlParameters(). Read only.
	NvU32        minPixels;        //!< minimum number of pixels required at current display mode to induce delay. Updated by NvAPI_GSync_GetControlParameters(). Read only.
} NV_GSYNC_DELAY;

#define NV_GSYNC_DELAY_VER  MAKE_NVAPI_VERSION(NV_GSYNC_DELAY,1)

//! Used in NvAPI_GSync_GetControlParameters() and NvAPI_GSync_SetControlParameters().
typedef struct _NV_GSYNC_CONTROL_PARAMS
{
    NvU32                       version;            //!< Version of the structure
    NVAPI_GSYNC_POLARITY        polarity;           //!< Leading edge / Falling edge / both
    NVAPI_GSYNC_VIDEO_MODE      vmode;              //!< None, TTL, NTSCPALSECAM, HDTV
    NvU32                       interval;           //!< Number of pulses to wait between framelock signal generation
    NVAPI_GSYNC_SYNC_SOURCE     source;             //!< VSync/House sync
    NvU32                       interlaceMode:1;    //!< interlace mode for a Sync device
    NvU32                       syncSourceIsOutput:1; //!< Set this to make house sync as an output; valid only when NV_GSYNC_CONTROL_PARAMS::source is NVAPI_GSYNC_SYNC_SOURCE_VSYNC on P2061 boards. 
                                                      //!< syncSourceIsOutput should always be NVAPI_GSYNC_SYNC_SOURCE_HOUSESYNC i.e. 0 on P2060 boards or when NV_GSYNC_CONTROL_PARAMS::source is set to NVAPI_GSYNC_SYNC_SOURCE_HOUSESYNC.
    NvU32                       reserved:30;        //!< should be set zero
	NV_GSYNC_DELAY              syncSkew;           //!< The time delay between the frame sync signal and the GPUs signal. 
    NV_GSYNC_DELAY              startupDelay;       //!< Sync start delay for master. 
} NV_GSYNC_CONTROL_PARAMS;

#define NV_GSYNC_CONTROL_PARAMS_VER  MAKE_NVAPI_VERSION(NV_GSYNC_CONTROL_PARAMS,1)


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GSync_GetControlParameters
//
//!   DESCRIPTION: This API queries for sync control parameters as defined in NV_GSYNC_CONTROL_PARAMS.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//! \param [in]    hNvGSyncDevice-   The caller provides the handle of the Sync device for which to get parameters
//! \param [inout] *pGsyncControls-  The caller provides the storage space. NvAPI_GSync_GetControlParameters() populates *pGsyncControls with values.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. 
//!          If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::NVAPI_INVALID_ARGUMENT          hNvGSyncDevice is NULL.
//! \retval ::NVAPI_NVIDIA_DEVICE_NOT_FOUND   The queried Graphics system does not have any Sync Device.
//!
//! \ingroup gsyncapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GSync_GetControlParameters(__in NvGSyncDeviceHandle hNvGSyncDevice, __inout NV_GSYNC_CONTROL_PARAMS *pGsyncControls);



//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GSync_SetControlParameters
//
//!   DESCRIPTION: This API sets control parameters as defined in NV_SYNC_CONTROL_PARAMS.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//! \param [in]  hNvGSyncDevice-   The caller provides the handle of the Sync device for which to get parameters
//! \param [inout]  *pGsyncControls-  The caller provides NV_GSYNC_CONTROL_PARAMS. skew and startDelay will be updated to the applied values.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. 
//!          If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::NVAPI_INVALID_ARGUMENT          hNvGSyncDevice is NULL.
//! \retval ::NVAPI_NVIDIA_DEVICE_NOT_FOUND   The queried Graphics system does not have any Sync Device.
//! \retval ::NVAPI_SYNC_MASTER_NOT_FOUND     Control Parameters can only be set if there is a Sync Master enabled on the Gsync card.
//!
//! \ingroup gsyncapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GSync_SetControlParameters(__in NvGSyncDeviceHandle hNvGSyncDevice, __inout NV_GSYNC_CONTROL_PARAMS *pGsyncControls);




//! Used in NvAPI_GSync_AdjustSyncDelay()
typedef enum _NVAPI_GSYNC_DELAY_TYPE
{
	NVAPI_GSYNC_DELAY_TYPE_UNKNOWN			= 0,
    NVAPI_GSYNC_DELAY_TYPE_SYNC_SKEW     	= 1,
    NVAPI_GSYNC_DELAY_TYPE_STARTUP     		= 2
} NVAPI_GSYNC_DELAY_TYPE;

//////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GSync_AdjustSyncDelay
//
//!   DESCRIPTION: This API adjusts the skew and startDelay to the closest possible values. Use this API before calling NvAPI_GSync_SetControlParameters for skew or startDelay.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 319
//!
//! \param [in]  hNvGSyncDevice-   	The caller provides the handle of the Sync device for which to get parameters
//! \param [in]  delayType-   		Specifies whether the delay is syncSkew or startupDelay. 
//! \param [inout]  *pGsyncDelay-  	The caller provides NV_GSYNC_DELAY. skew and startDelay will be adjusted and updated to the closest values.
//! \param [out]  *syncSteps-  		This parameter is optional. It returns the sync delay in unit steps. If 0, it means either the NV_GSYNC_DELAY::numPixels is less than NV_GSYNC_DELAY::minPixels or NV_GSYNC_DELAY::numOfLines exceeds the NV_GSYNC_DELAY::maxLines.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. 
//!          If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup gsyncapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GSync_AdjustSyncDelay(__in NvGSyncDeviceHandle hNvGSyncDevice, __in NVAPI_GSYNC_DELAY_TYPE delayType, __inout NV_GSYNC_DELAY *pGsyncDelay, __out_opt NvU32* syncSteps);



//! Used in NvAPI_GSync_GetSyncStatus().
typedef struct _NV_GSYNC_STATUS
{
    NvU32 version;                          //!< Version of the structure
    NvU32 bIsSynced;                        //!< Is timing in sync?
    NvU32 bIsStereoSynced;                  //!< Does the phase of the timing signal from the GPU = the phase of the master sync signal?
    NvU32 bIsSyncSignalAvailable;           //!< Is the sync signal available?
} NV_GSYNC_STATUS;

//! Macro for constructing the version field for NV_GSYNC_STATUS.
#define NV_GSYNC_STATUS_VER  MAKE_NVAPI_VERSION(NV_GSYNC_STATUS,1)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GSync_GetSyncStatus
//
//!   DESCRIPTION: This API queries the sync status of a GPU - timing, stereosync and sync signal availability.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//! \param [in]  hNvGSyncDevice-     Handle of the Sync device
//! \param [in]  hPhysicalGpu-       GPU to be queried for sync status.
//! \param [out] *status-            The caller provides the storage space. NvAPI_GSync_GetSyncStatus() populates *status with
//!                                  values - timing, stereosync and signal availability. On error, *status is set to NULL.
//!
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. 
//!          If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::NVAPI_INVALID_ARGUMENT          hNvGSyncDevice is NULL / SyncTarget is NULL.
//! \retval ::NVAPI_NVIDIA_DEVICE_NOT_FOUND   The queried Graphics system does not have any G-Sync Device.
//!
//! \ingroup gsyncapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GSync_GetSyncStatus(__in NvGSyncDeviceHandle hNvGSyncDevice, __in NvPhysicalGpuHandle hPhysicalGpu, __inout NV_GSYNC_STATUS *status);


//! \ingroup gsyncapi

#define NVAPI_MAX_RJ45_PER_GSYNC    2

//! Used in NV_GSYNC_STATUS_PARAMS.
typedef enum _NVAPI_GSYNC_RJ45_IO
{
    NVAPI_GSYNC_RJ45_OUTPUT     = 0,
    NVAPI_GSYNC_RJ45_INPUT      = 1,
    NVAPI_GSYNC_RJ45_UNUSED     = 2 //!< This field is used to notify that the framelock is not actually present.

} NVAPI_GSYNC_RJ45_IO;

//! \ingroup gsyncapi
//! Used in NvAPI_GSync_GetStatusParameters().
typedef struct _NV_GSYNC_STATUS_PARAMS_V1
{
    NvU32                       version;
    NvU32                       refreshRate;                                //!< The refresh rate
    NVAPI_GSYNC_RJ45_IO         RJ45_IO[NVAPI_MAX_RJ45_PER_GSYNC];          //!< Configured as input / output
    NvU32                       RJ45_Ethernet[NVAPI_MAX_RJ45_PER_GSYNC];    //!< Connected to ethernet hub? [ERRONEOUSLY CONNECTED!]
    NvU32                       houseSyncIncoming;                          //!< Incoming house sync frequency in Hz
    NvU32                       bHouseSync;                                 //!< Is house sync connected?
} NV_GSYNC_STATUS_PARAMS_V1;

typedef struct _NV_GSYNC_STATUS_PARAMS_V2
{
    NvU32                       version;
    NvU32                       refreshRate;                                //!< The refresh rate
    NVAPI_GSYNC_RJ45_IO         RJ45_IO[NVAPI_MAX_RJ45_PER_GSYNC];          //!< Configured as input / output
    NvU32                       RJ45_Ethernet[NVAPI_MAX_RJ45_PER_GSYNC];    //!< Connected to ethernet hub? [ERRONEOUSLY CONNECTED!]
    NvU32                       houseSyncIncoming;                          //!< Incoming house sync frequency in Hz
    NvU32                       bHouseSync;                                 //!< Is house sync connected?
    NvU32                       bInternalSlave : 1;                         //!< Valid only for P2061 board.
                                                                            //!< If set to 1, it means that this P2061 board receives input from another P2061 board.
    NvU32                       reserved : 31;                              //!< Reserved for future use.
} NV_GSYNC_STATUS_PARAMS_V2;


typedef NV_GSYNC_STATUS_PARAMS_V2 NV_GSYNC_STATUS_PARAMS;
 
//! \ingroup gsyncapi
//! Macro for constructing the version field of NV_GSYNC_STATUS_PARAMS 
#define NV_GSYNC_STATUS_PARAMS_VER1  MAKE_NVAPI_VERSION(NV_GSYNC_STATUS_PARAMS_V1,1)
#define NV_GSYNC_STATUS_PARAMS_VER2  MAKE_NVAPI_VERSION(NV_GSYNC_STATUS_PARAMS_V2,2)
#define NV_GSYNC_STATUS_PARAMS_VER   NV_GSYNC_STATUS_PARAMS_VER2

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GSync_GetStatusParameters
//
//!   DESCRIPTION: This API queries for sync status parameters as defined in NV_GSYNC_STATUS_PARAMS.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 313
//!
//! \param [in]  hNvGSyncDevice   The caller provides the handle of the GSync device for which to get parameters
//! \param [out] *pStatusParams   The caller provides the storage space. NvAPI_GSync_GetStatusParameters populates *pStatusParams with
//!                               values.
//! 
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. 
//!          If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::NVAPI_INVALID_ARGUMENT            hNvGSyncDevice is NULL / pStatusParams is NULL.
//! \retval ::NVAPI_NVIDIA_DEVICE_NOT_FOUND     The queried Graphics system does not have any GSync Device.
//!
//! \ingroup gsyncapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_GSync_GetStatusParameters(NvGSyncDeviceHandle hNvGSyncDevice, NV_GSYNC_STATUS_PARAMS *pStatusParams);

//! @}








#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_D3D9_RegisterResource
//
//! DESCRIPTION:    This API binds a resource (surface/texture) so that it can be retrieved
//!                 internally by NVAPI.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//! \param [in]     pResource      surface/texture
//!
//! \return ::NVAPI_OK, ::NVAPI_ERROR 
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_RegisterResource(IDirect3DResource9* pResource);
#endif //defined(_D3D9_H_)

#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_D3D9_UnregisterResource
//
//! DESCRIPTION:     This API unbinds a resource (surface/texture) after use.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//! 
//! \param [in]     pResource    surface/texture
//!
//! \return ::NVAPI_OK, ::NVAPI_ERROR 
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_UnregisterResource(IDirect3DResource9* pResource);

#endif //defined(_D3D9_H_)




#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D9_AliasSurfaceAsTexture
//
//! \fn NvAPI_D3D9_AliasSurfaceAsTexture(IDirect3DDevice9* pDev,
//!                                              IDirect3DSurface9* pSurface,
//!                                              IDirect3DTexture9 **ppTexture,
//!                                              DWORD dwFlag);
//!   DESCRIPTION: Create a texture that is an alias of a surface registered with NvAPI.  The
//!                new texture can be bound with IDirect3DDevice9::SetTexture().  Note that the texture must
//!                be unbound before drawing to the surface again.
//!                Unless the USE_SUPER flag is passed, MSAA surfaces will be resolved before
//!                being used as a texture.  MSAA depth buffers are resolved with a point filter,
//!                and non-depth MSAA surfaces are resolved with a linear filter.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]    pDev         The D3D device that owns the objects
//! \param [in]    pSurface     Pointer to a surface that has been registered with NvAPI 
//!                             to which a texture alias is to be provided
//! \param [out]   ppTexture    Fill with the texture created
//! \param [in]    dwFlag       NVAPI_ALIAS_SURFACE_FLAG to describe how to handle the texture
//!
//! \retval ::NVAPI_OK                  completed request
//! \retval ::NVAPI_INVALID_POINTER     A null pointer was passed as an argument
//! \retval ::NVAPI_INVALID_ARGUMENT    One of the arguments was invalid, probably dwFlag.
//! \retval ::NVAPI_UNREGISTERED_RESOURCE pSurface has not been registered with NvAPI
//! \retval ::NVAPI_ERROR               error occurred
//
///////////////////////////////////////////////////////////////////////////////


//! \ingroup dx
//! See NvAPI_D3D9_AliasSurfaceAsTexture().
typedef enum {
    NVAPI_ALIAS_SURFACE_FLAG_NONE                     = 0x00000000,
    NVAPI_ALIAS_SURFACE_FLAG_USE_SUPER                = 0x00000001,  //!< Use the surface's msaa buffer directly as a texture, rather than resolving. (This is much slower, but potentially has higher quality.)
    NVAPI_ALIAS_SURFACE_FLAG_MASK                     = 0x00000001
} NVAPI_ALIAS_SURFACE_FLAG;


//! \ingroup dx
NVAPI_INTERFACE NvAPI_D3D9_AliasSurfaceAsTexture(IDirect3DDevice9* pDev,
                                                 IDirect3DSurface9* pSurface,
                                                 IDirect3DTexture9 **ppTexture,
                                                 DWORD dwFlag);
#endif //defined(_D3D9_H_)

#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_D3D9_StretchRectEx
//
//! DESCRIPTION:     This API copies the contents of the source resource to the destination
//!                  resource.  This function can convert
//!                  between a wider range of surfaces than
//!                  IDirect3DDevice9::StretchRect.  For example, it can copy
//!                  from a depth/stencil surface to a texture.
//!
//!                  The source and destination resources *must* be registered
//!                  with NvAPI before being used with NvAPI_D3D9_StretchRectEx().
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     pDevice             The D3D device that owns the objects.
//! \param [in]     pSourceResource     Pointer to the source resource.
//! \param [in]     pSrcRect            Defines the rectangle on the source to copy from.  If NULL, copy from the entire resource.
//! \param [in]     pDestResource       Pointer to the destination resource.
//! \param [in]     pDstRect            Defines the rectangle on the destination to copy to.  If NULL, copy to the entire resource.
//! \param [in]     Filter              Choose a filtering method: D3DTEXF_NONE, D3DTEXF_POINT, D3DTEXF_LINEAR.
//!
//! \retval ::NVAPI_OK                       completed request
//! \retval ::NVAPI_INVALID_POINTER          An invalid pointer was passed as an argument (probably NULL)
//! \retval ::NVAPI_INVALID_ARGUMENT         One of the arguments was invalid
//! \retval ::NVAPI_UNREGISTERED_RESOURCE    a resource was passed in without being registered
//! \retval ::NVAPI_ERROR                    error occurred
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_StretchRectEx(IDirect3DDevice9 * pDevice,
                                         IDirect3DResource9 * pSourceResource,
                                         CONST RECT * pSourceRect,
                                         IDirect3DResource9 * pDestResource,
                                         CONST RECT * pDestRect,
                                         D3DTEXTUREFILTERTYPE Filter);

#endif //defined(_D3D9_H_)

#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_D3D9_ClearRT
//
//! DESCRIPTION:     This API Clears the currently bound render target(s) with the 
//!                  given color
//!
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     pDevice             The D3D device that owns the objects.
//! \param [in]     dwNumRects          The no of rectangles to clear. If 0, clear the entire surface (clipped to viewport)
//! \param [in]     pRects              Defines the rectangles to clear.  Should be NULL if dwNumRects == 0
//! \param [in]     r                   red component of the clear color
//! \param [in]     g                   green component of the clear color
//! \param [in]     b                   blue component of the clear color
//! \param [in]     a                   alpha component of the clear color
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_ClearRT(IDirect3DDevice9 * pDevice,
                                   NvU32 dwNumRects,
                                   CONST RECT * pRects,
                                   float r, float g, float b, float a);
#endif //if defined(_D3D9_H_)










#if defined(_D3D9_H_) && defined(__cplusplus)
//! SUPPORTED OS:  Windows 7 and higher
//!

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D9_GetSurfaceHandle
//
//!  This function gets the handle of a given surface. This handle uniquely 
//!  identifies the surface through all NvAPI entries.
//!
//!
//! \since Release: 313
//!
//! \param [in]     pSurface  Surface to be identified
//! \param [out]    pHandle    Will be filled by the return handle
//!
//! \return An int which could be an NvAPI status or DX HRESULT code
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_GetSurfaceHandle(IDirect3DSurface9 *pSurface, 
                                        NVDX_ObjectHandle *pHandle);

#endif //defined(_D3D9_H_) && defined(__cplusplus)

#if defined(_D3D9_H_) && defined(__cplusplus)
//! SUPPORTED OS:  Windows 7 and higher
//!
//! \addtogroup dxvidcontrol
//! @{

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION_NAME: NvAPI_D3D9_VideoSetStereoInfo
//
//! \fn NvAPI_D3D9_VideoSetStereoInfo(IDirect3DDevice9 *pDev,
//!                                            NV_DX_VIDEO_STEREO_INFO *pStereoInfo);
//! \code
//!   DESCRIPTION: This api specifies the stereo format of a surface, so that the
//!                surface could be used for stereo video processing or compositing. 
//!                In particular, this api could be used to link the left and right 
//!                views of a decoded picture.
//!
//! \since Release: 313
//!
//!         INPUT: pDev        - The device on which the stereo surface will be used
//!                pStereoInfo - The stereo format of the surface
//!
//! RETURN STATUS: an int which could be an NvAPI status or DX HRESULT code
//! \endcode
///////////////////////////////////////////////////////////////////////////////

#ifndef NV_STEREO_VIDEO_FORMAT_DEFINE
#define NV_STEREO_VIDEO_FORMAT_DEFINE


typedef enum _NV_STEREO_VIDEO_FORMAT
{
    NV_STEREO_VIDEO_FORMAT_NOT_STEREO         = 0,

    NV_STEREO_VIDEO_FORMAT_SIDE_BY_SIDE_LR    = 1,
    NV_STEREO_VIDEO_FORMAT_SIDE_BY_SIDE_RL    = 2,
    NV_STEREO_VIDEO_FORMAT_TOP_BOTTOM_LR      = 3,
    NV_STEREO_VIDEO_FORMAT_TOP_BOTTOM_RL      = 4,
    NV_STEREO_VIDEO_FORMAT_ROW_INTERLEAVE_LR  = 5,
    NV_STEREO_VIDEO_FORMAT_ROW_INTERLEAVE_RL  = 6,
    NV_STEREO_VIDEO_FORMAT_TWO_FRAMES_LR      = 7,
    NV_STEREO_VIDEO_FORMAT_MONO_PLUS_OFFSET   = 8,

    NV_STEREO_VIDEO_FORMAT_LAST               = 9,
} NV_STEREO_VIDEO_FORMAT;

#endif // NV_STEREO_VIDEO_FORMAT_DEFINE


typedef struct _NV_DX_VIDEO_STEREO_INFO {
    NvU32                     dwVersion;         //!< Must be NV_DX_VIDEO_STEREO_INFO_VER
    NVDX_ObjectHandle         hSurface;          //!< The surface whose stereo format is to be set
    NVDX_ObjectHandle         hLinkedSurface;    //!< The linked surface (must be valid when eFormat==NV_STEREO_VIDEO_FORMAT_TWO_FRAMES_LR)
    NV_STEREO_VIDEO_FORMAT    eFormat;           //!< Stereo format of the surface
    NvS32                     sViewOffset;       //!< Signed offset of each view (positive offset indicating left view is shifted left)
    BOOL                      bStereoEnable;     //!< Whether stereo rendering should be enabled (if FALSE, only left view will be used)
} NV_DX_VIDEO_STEREO_INFO;

//! Macro for constructing the version field of ::NV_DX_VIDEO_STEREO_INFO
#define NV_DX_VIDEO_STEREO_INFO_VER  MAKE_NVAPI_VERSION(NV_DX_VIDEO_STEREO_INFO,1)

NVAPI_INTERFACE NvAPI_D3D9_VideoSetStereoInfo(IDirect3DDevice9 *pDev,
                                              NV_DX_VIDEO_STEREO_INFO *pStereoInfo);

//! @}
#endif //defined(_D3D9_H_) && defined(__cplusplus)


#if defined(__cplusplus) && defined(__d3d10_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D10_SetDepthBoundsTest
//
//!   DESCRIPTION: This function enables/disables the depth bounds test.
//!
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//!   \param [in]  pDev         The device to set the depth bounds test
//!   \param [in]  bEnable      Enable(non-zero)/disable(zero) the depth bounds test
//!   \param [in]  fMinDepth    The minimum depth for the depth bounds test
//!   \param [in]  fMaxDepth    The maximum depth for the depth bounds test \n
//!                             The valid values for fMinDepth and fMaxDepth
//!                             are such that 0 <= fMinDepth <= fMaxDepth <= 1
//!
//!   \return NVAPI_OK if the depth bounds test was correctly enabled or disabled 
//!
//!   \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D10_SetDepthBoundsTest(ID3D10Device *pDev,
                                               NvU32 bEnable,
                                               float fMinDepth,
                                               float fMaxDepth);

#endif //defined(__cplusplus) && defined(__d3d10_h__)





#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_IsNvShaderExtnOpCodeSupported
//
//!   DESCRIPTION: This function checks if a nv HLSL shader extension opcode is 
//!                supported on current hardware. List of opcodes is in nvShaderExtnEnums.h
//!                To use Nvidia HLSL extensions the application must include nvHLSLExtns.h 
//!                in the hlsl shader code. See nvHLSLExtns.h for more details on supported opcodes.
//!
//!                This function can be called from a different thread than the one calling immediate device setstate functions.  
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]        pDev         The device on which to query for support,
//!                                 should be a ID3D11Device+ device
//! \param [in]        opCode       the opcode to check
//! \param [out]       pSupported   true if supported, false otherwise
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::         NVAPI_OK if the call succeeded
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_IsNvShaderExtnOpCodeSupported(__in  IUnknown *pDev,
                                                          __in  NvU32 opCode,
                                                          __out bool *pSupported);

#endif //defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_SetNvShaderExtnSlot
//
//!   DESCRIPTION: This function sets the fake UAV slot that is used by Nvidia HLSL
//!                shader extensions globally. All createShader calls made to the driver after
//!                setting this slot would treat writes/reads to this UAV in a 
//!                different way. Applications are expected to bind null UAV to this slot.
//!                The same slot is used for all shader stages.
//!                To disable shader extensions the app need to set this uav slot to 0xFFFFFFFF.
//!                To use Nvidia HLSL extensions the application must include nvHLSLExtns.h 
//!                in the hlsl shader code. See nvHLSLExtns.h for more details.
//!
//!                This function can be called from a different thread than the one calling immediate device setstate functions.  
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]        pDev         The device for which to set the extension slot
//!                                 should be a ID3D11Device+ device
//! \param [in]        uavSlot      the uav slot to use
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::         NVAPI_OK    : success, the uavSlot was set sucessfully
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_SetNvShaderExtnSlot(__in IUnknown *pDev,
                                                __in NvU32 uavSlot);

#endif //defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

#if defined (__cplusplus) && defined (__d3d12_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_SetNvShaderExtnSlotSpace
//
//!   DESCRIPTION: This function is specifically created for ray tracing since we do not
//!                currently support PSOs with DXR.
//!                This function sets the device's fake UAV slot and space that is used by Nvidia HLSL
//!                shader extensions globally. All state objects created by the driver after
//!                setting this slot would treat writes/reads to this UAV in a 
//!                different way. Applications are expected to bind null UAV to this slot.
//!                The same slot is used for all shader stages.
//!                To disable shader extensions the app need to set this uav slot to 0xFFFFFFFF.
//!                To use Nvidia HLSL extensions the application must include nvHLSLExtns.h 
//!                in the hlsl shader code. See nvHLSLExtns.h for more details.
//!
//!                This function can be called from a different thread than the one calling immediate device setstate functions.  
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]        pDev         The device for which to set the extension slot
//!                                 should be a ID3D12Device+ device
//! \param [in]        uavSlot      The uav slot to use
//! \param [in]        uavSpace     The uav space to use
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::         NVAPI_OK    : success, the uavSlot and uavSpace were set sucessfully
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D12_SetNvShaderExtnSlotSpace(__in IUnknown *pDev,
                                                __in NvU32 uavSlot,
                                                __in NvU32 uavSpace);

                                                ///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_SetNvShaderExtnSlotSpaceLocalThread
//
//!   DESCRIPTION: This function is specifically created for ray tracing shaders since we do not
//!                currently support PSOs with DXR.
//!                This function sets the device's fake UAV slot that is used by Nvidia HLSL
//!                shader extensions on local thread. All state objects created by the driver
//!                on the same thread that call this function after setting this slot would treat writes/reads 
//!                to this UAV in a different way.
//!                Applications are expected to bind null UAV to this slot.
//!                The same slot is used for all shader stages for the device.
//!                To disable shader extensions the app may set this uav slot to 0xFFFFFFFF.
//!                To use Nvidia HLSL extensions the application must include nvHLSLExtns.h 
//!                in the hlsl shader code. See nvHLSLExtns.h for more details.
//!
//!                This function can be called from a different thread than the one calling immediate device setstate functions.  
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//!
//! \since Release: 387
//!
//! \param [in]        pDev         The device for which to set the extension slot
//!                                 should be a ID3D12Device+ device
//! \param [in]        uavSlot      the uav slot to use
//! \param [in]        uavSpace     the uav space to use
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::         NVAPI_OK    : success, the uavSlot and uavSpace were set sucessfully
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D12_SetNvShaderExtnSlotSpaceLocalThread(__in IUnknown *pDev,
                                                                __in NvU32 uavSlot,
                                                                __in NvU32 uavSpace);

#endif //defined (__cplusplus) && defined (__d3d12_h__)

#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_SetNvShaderExtnSlotLocalThread
//
//!   DESCRIPTION: This function sets the fake UAV slot that is used by Nvidia HLSL
//!                shader extensions on local thread. All createShader calls on the same thread 
//!                that calls this function after setting this slot would treat writes/reads 
//!                to this UAV in a different way.
//!                Applications are expected to bind null UAV to this slot.
//!                The same slot is used for all shader stages.
//!                To disable shader extensions the app may set this uav slot to 0xFFFFFFFF.
//!                To use Nvidia HLSL extensions the application must include nvHLSLExtns.h 
//!                in the hlsl shader code. See nvHLSLExtns.h for more details.
//!
//!                This function can be called from a different thread than the one calling immediate device setstate functions.  
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 387
//!
//! \param [in]        pDev         The device for which to set the extension slot
//!                                 should be a ID3D11Device+ device
//! \param [in]        uavSlot      the uav slot to use
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::         NVAPI_OK    : success, the uavSlot was set sucessfully
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_SetNvShaderExtnSlotLocalThread(__in IUnknown *pDev,
                                                           __in NvU32 uavSlot);

#endif //defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_BeginUAVOverlapEx
//
//!   DESCRIPTION: Causes the driver to skip synchronization that is normally needed when accessing UAVs.
//!                Applications must use this with caution otherwise this might cause data hazards when
//!                multiple draw calls/compute shader launches are accessing same memory locations
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]        *pDeviceOrContext     pointer to D3D11 device, or D3D11 device context
//! \param [in]        insertWFIFlags        bit fields to indicate which WFI would be inserted (gfx / compute / both).
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
typedef enum _NVAPI_D3D11_INSERTWFI_FLAG
{
    NVAPI_D3D_BEGIN_UAV_OVERLAP_NO_WFI      = 0x00000000,   //!< no WFI
    NVAPI_D3D_BEGIN_UAV_OVERLAP_GFX_WFI     = 0x00000001,   //!< (bit 0) force graphics WFI
    NVAPI_D3D_BEGIN_UAV_OVERLAP_COMP_WFI    = 0x00000002,   //!< (bit 1) force compute WFI
} NVAPI_D3D11_INSERTWFI_FLAG;

NVAPI_INTERFACE NvAPI_D3D11_BeginUAVOverlapEx(__in  IUnknown *pDeviceOrContext, __in NvU32 insertWFIFlags);

#endif //defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_BeginUAVOverlap
//
//!   DESCRIPTION: Causes the driver to skip synchronization that is normally needed when accessing UAVs.
//!                Applications must use this with caution otherwise this might cause data hazards when
//!                multiple draw calls/compute shader launches are accessing same memory locations
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]        *pDeviceOrContext     pointer to D3D11 device, or D3D11 device context
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_BeginUAVOverlap(__in  IUnknown *pDeviceOrContext);

#endif //defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_EndUAVOverlap
//
//!   DESCRIPTION: Re-enables driver synchronization between calls that access same UAVs
//!                See NvAPI_D3D_BeginUAVOverlap for more details.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]        *pDeviceOrContext     pointer to D3D11 device, or D3D11 device context
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_EndUAVOverlap(__in  IUnknown *pDeviceOrContext);

#endif //defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

#if defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_SetFPSIndicatorState
//
//!   DESCRIPTION: Display an overlay that tracks the number of times the app presents per second, or,   
//!      the number of frames-per-second (FPS)
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in] bool    Whether or not to enable the fps indicator.
//!                
//! \return ::NVAPI_OK, 
//!         ::NVAPI_ERROR
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 
NVAPI_INTERFACE NvAPI_D3D_SetFPSIndicatorState(IUnknown *pDev, NvU8 doEnable);

#endif //if defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__)

#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D9_Present
//
//! DESCRIPTION: This API presents the contents of the next buffer in the sequence of back buffers 
//!              owned by a IDirect3DDevice9 device.
//!              This Present operation supports using a SwapGroup and SwapBarrier on the SwapChain
//!              that owns the back buffer to be presented.
//!
//!          NOTE: NvAPI_D3D9_Present is a wrapper of the method IDirect3DDevice9::Present which
//!                additionally notifies the D3D driver of the SwapChain used by the runtime for
//!                presentation, thus allowing the D3D driver to apply SwapGroup and SwapBarrier
//!                functionality to that SwapChain.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]    pDevice     The IDirect3DDevice9 interface that is used to issue the Present, 
//!                using the following IDirect3DDevice9::Present input parameters
//! \param [in]    pSwapChain   Optional pointer to a IDirect3DSwapChain9 interface. If provided, the presentation is executed
//!                             using this interface (i.e. pSwapChain->Present()) for the given swapchain only.
//!                             If NULL, the presentation is executed on the device for all swapchains as in pDevice->Present()
//! \param [in]    pSourceRect  A pointer to a RECT structure containing the source rectangle. 
//!                             If NULL, the entire source surface is presented.
//! \param [in]    pDestRect      A pointer to a RECT structure containing the destination rectangle, in window client coordinates. 
//!                   If NULL, the entire client area is filled. 
//! \param [in]    hDestWindowOverride  A pointer to a destination window whose client area is taken as the target for this presentation. 
//!                                     If this value is NULL, then the hWndDeviceWindow member of D3DPRESENT_PARAMTERS is taken. 
//! \param [in]    pDirtyRegion (IN)   A pointer to a region to be presented. It must be NULL unless the swap chain was reated with 
//!                                    D3DSWAPEFFECT_COPY. If this value is non-NULL, the contained region is expressed in back buffer coordinates.
//!
//! \retval ::NVAPI_OK                 the Present operation was successfully executed
//! \retval ::NVAPI_D3D_DEVICE_LOST    D3D device status is D3DERR_DEVICELOST or D3DERR_DEVICENOTRESET, the caller has to reset device   
//! \retval ::NVAPI_DEVICE_BUSY        the Present operation failed with an error other than D3DERR_DEVICELOST or D3DERR_DEVICENOTRESET
//! \retval ::NVAPI_ERROR              the communication with the D3D driver failed, SwapGroup/SwapBarrier may not be possible.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI was not yet initialized.
//!
//!\ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_Present(IDirect3DDevice9 *pDevice,
                                   IDirect3DSwapChain9 *pSwapChain,
                                   const RECT *pSourceRect,
                                   const RECT *pDestRect,
                                   HWND hDestWindowOverride,
                                   const RGNDATA *pDirtyRegion);
#endif //if defined(_D3D9_H_)

#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D9_QueryFrameCount
//
//!   DESCRIPTION: This API queries the universal framecounter of the Quadro-Sync master device.
//!
//! \param [in]   pDevice            The caller provides the DX9 device that has access to the Quadro-Sync device
//! \param [out]  pFrameCount        The caller provides the storage space where the framecount is stored.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                  *pFrameCount populated with framecount value.
//! \retval ::NVAPI_ERROR               The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT    One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_QueryFrameCount(IDirect3DDevice9 *pDevice,
                                           NvU32 *pFrameCount);
#endif //if defined(_D3D9_H_)

#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D9_ResetFrameCount
//
//!   DESCRIPTION: This API resets the universal framecounter on the Quadro-Sync master device.
//!
//! \param [in]   pDevice            The caller provides the DX9 device that has access to the Quadro-Sync device
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                   framecounter has been reset
//! \retval ::NVAPI_ERROR                The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT     One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED  NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_ResetFrameCount(IDirect3DDevice9 *pDevice);
#endif //if defined(_D3D9_H_)

#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D9_QueryMaxSwapGroup
//
//!   DESCRIPTION: This API queries the number of supported SwapGroups and SwapBarriers in the graphics system.
//!
//! \param [in]   pDevice              The caller provides the DirectX 9 device that is used as a swapgroup client
//! \param [out]  pMaxGroups           The caller provides the storage space where the number of available SwapGroups is stored.
//! \param [out]  pMaxBarriers         The caller provides the storage space where the number of available SwapBarriers is stored.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                   the number of SwapGroups and SwapBarriers has been stored
//! \retval ::NVAPI_ERROR                The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT     One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED  NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_QueryMaxSwapGroup(IDirect3DDevice9 *pDevice, 
                                             NvU32 *pMaxGroups, 
                                             NvU32 *pMaxBarriers); 
#endif //if defined(_D3D9_H_)

#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D9_QuerySwapGroup
//
//!   DESCRIPTION: This API queries the current SwapGroup and SwapBarrier that a SwapChain of a specific client device is bound to.
//!
//! \param [in]   pDevice            The caller provides the DirectX 9 device that is used as a swapgroup client
//! \param [in]   pSwapChain         The caller provides the IDirect3DSwapChain9 interface as a handle to the SwapChain  
//!                                  that belongs to the swapgroup client device
//! \param [out]  pSwapGroup         The caller provides the storage space where the current SwapGroup is stored.
//! \param [out]  pSwapBarrier       The caller provides the storage space where the current SwapBarrier is stored.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                   the current SwapGroup and SwapBarrier has been stored
//! \retval ::NVAPI_ERROR                The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT     One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED  NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_QuerySwapGroup(IDirect3DDevice9 *pDevice, 
                                          IDirect3DSwapChain9 *pSwapChain, 
                                          NvU32 *pSwapGroup, 
                                          NvU32 *pSwapBarrier); 
#endif //if defined(_D3D9_H_)

#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D9_JoinSwapGroup
//
//!   DESCRIPTION: This API causes the SwapChain of a SwapGroup client to join or leave the specified SwapGroup.
//!
//! \param [in]    pDevice               The caller provides the DirectX 9 device that is used as a swapgroup client
//! \param [in]    pSwapChain            The caller provides the IDirect3DSwapChain9 interface as a handle to the SwapChain  
//!                                      that belongs to the swapgroup client device
//! \param [in]    group                 The caller specifies the SwapGroup which the SwapChain should join.
//!                                      - If the value of group is zero, the SwapChain leaves the SwapGroup.
//!                                      - The SwapChain joins a SwapGroup if the SwapGroup number is a positive integer less than or
//!                                        equal to the maximum number of SwapGroups queried by NvAPI_SwapGroup_QueryMaxSwapGroup.
//! \param [in]    blocking              The caller specifies that a presentation of this SwapChain should return immediately or block
//!                                      until all members of the SwapGroup are ready and the presentation was actually executed.
//!                                      A boolean value of false means the Present operation returns immediately and a value of true
//!                                      means the Present operation is blocking.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                   the SwapChain joined/left the SwapGroup accordingly
//! \retval ::NVAPI_ERROR                The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT     One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED  NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_JoinSwapGroup(IDirect3DDevice9 *pDevice, 
                                         IDirect3DSwapChain9 *pSwapChain, 
                                         NvU32 group,
                                         BOOL blocking); 
#endif //if defined(_D3D9_H_)

#if defined(_D3D9_H_)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D9_BindSwapBarrier
//
//!   DESCRIPTION: This API causes a SwapGroup to be bound to or released from the specified SwapBarrier.
//!
//! \param [in]    pDevice    The caller provides the DirectX 9 device that is used as a swapgroup client
//! \param [in]    group      The caller specifies the SwapGroup to be bound to the SwapBarrier.
//! \param [in]    barrier    The caller specifies the SwapBarrier that the SwapGroup should be bound to.
//!                              - If the value of barrier is zero, the SwapGroup will be released from the SwapBarrier.
//!                              - The SwapGroup will be bound to the SwapBarrier if the value of barrier is a positive 
//!                                integer less than or equal to the maximum number of SwapBarriers queried by NvAPI_SwapGroup_QueryMaxSwapGroup.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                  the SwapGroup is bound to or released from the specified SwapBarrier
//! \retval ::NVAPI_ERROR               The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT    One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_BindSwapBarrier(IDirect3DDevice9 *pDevice, 
                                           NvU32 group, 
                                           NvU32 barrier); 
#endif //if defined(_D3D9_H_)

#if defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D1x_Present
//
//! DESCRIPTION: Presents the contents of the next buffer in the sequence of back buffers 
//!              owned by a D3D device.
//!              This Present operation supports using a SwapGroup and SwapBarrier on the SwapChain
//!              that owns the back buffer to be presented.
//!
//!          NOTE: NvAPI_D3D1x_Present is a wrapper of the method IDXGISwapChain::Present which
//!                additionally notifies the D3D driver of the SwapChain used by the runtime for
//!                presentation, thus allowing the D3D driver to apply SwapGroup and SwapBarrier
//!                functionality to that SwapChain.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     pDevice          The D3D device interface that is used to issue the Present operation, 
//!                                 using the following IDirect3DDevice9::Present input parameters.
//!                                  pDevice can be either ID3D10Device or ID3D10Device1 or ID3D11Device or ID3D12Device.
//! \param [in]     pSwapChain       The IDXGISwapChain interface that is intended to present
//! \param [in]     SyncInterval     An integer that specifies the how to synchronize presentation of a frame with the vertical blank.
//!                                          Values are: 
//!                                          - 0:  The presentation occurs immediately, there is no synchronization.
//!                                          - 1,2,3,4 : Synchronize presentation after the n'th vertical blank.
//! \param [in]     Flags            An integer value that contains swap-chain presentation options as defined in DXGI_PRESENT.
//!
//! \retval ::NVAPI_OK                 the Present operation was successfully executed
//! \retval ::NVAPI_DEVICE_BUSY        the Present operation failed with an error DXGI_ERROR_DEVICE_RESET or DXGI_ERROR_DEVICE_REMOVED, 
//                                     DXGI_STATUS_OCCLUDED, or D3DDDIERR_DEVICEREMOVED. 
//! \retval ::NVAPI_ERROR              the communication with the D3D driver failed, SwapGroup/SwapBarrier may not be possible.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D1x_Present(IUnknown *pDevice,
                                    IDXGISwapChain *pSwapChain,
                                    UINT SyncInterval,
                                    UINT Flags);
#endif // defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)

#if defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D1x_QueryFrameCount
//
//!   DESCRIPTION: This API queries the universal framecounter of the Quadro-Sync master device.
//!
//! \param [in]    pDevice           The caller provides the D3D device that has access to the Quadro-Sync device,
//!                                  pDevice can be either ID3D10Device or ID3D10Device1 or ID3D11Device or ID3D12Device.
//! \param [out]   pFrameCount       The caller provides the storage space where the framecount is stored.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                   *pFrameCount populated with framecount value.
//! \retval ::NVAPI_ERROR                The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT     One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED  NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D1x_QueryFrameCount(IUnknown *pDevice,
                                            NvU32 *pFrameCount);
#endif // defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)

#if defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D1x_ResetFrameCount
//
//!   DESCRIPTION: This API resets the universal framecounter on the Quadro-Sync master device.
//!
//! \param [in]    pDevice         The caller provides the D3D device that has access to the Quadro-Sync device,
//!                                pDevice can be either ID3D10Device or ID3D10Device1 or ID3D11Device or ID3D12Device.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                   framecounter has been reset
//! \retval ::NVAPI_ERROR                The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT     pDevice arg passed in is invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED  NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D1x_ResetFrameCount(IUnknown *pDevice);
#endif // defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)

#if defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D1x_QueryMaxSwapGroup
//
//!   DESCRIPTION: This API queries the number of supported SwapGroups and SwapBarriers in the graphics system.
//!
//! \param [in]    pDevice        The caller provides the D3D device that is intended to use SwapGroup functionality.
//!                               pDevice can be either ID3D10Device or ID3D10Device1 or ID3D11Device or ID3D12Device.
//! \param [out]   pMaxGroups     The caller provides the storage space where the number of available SwapGroups is stored.
//! \param [out]   pMaxBarriers   The caller provides the storage space where the number of available SwapBarriers is stored.
//! 
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                  the number of SwapGroups and SwapBarriers has been stored
//! \retval ::NVAPI_ERROR               The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT    One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D1x_QueryMaxSwapGroup(IUnknown *pDevice, 
                                              NvU32 *pMaxGroups, 
                                              NvU32 *pMaxBarriers); 
#endif // defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)

#if defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D1x_QuerySwapGroup
//
//!   DESCRIPTION: This API queries the current SwapGroup and SwapBarrier that a SwapChain of a specific client device is bound to.
//!
//! \param [in]    pDevice            The caller provides the D3D device that owns the SwapChain used as a SwapGroup client.
//!                                   pDevice can be either ID3D10Device or ID3D10Device1 or ID3D11Device or ID3D12Device.
//! \param [in]    pSwapChain         The IDXGISwapChain interface that is used as the SwapGroup client.
//!
//! \param [out]   pSwapGroup         The caller provides the storage space where the current SwapGroup is stored.
//! \param [out]   pSwapBarrier       The caller provides the storage space where the current SwapBarrier is stored.
//! 
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                   the current SwapGroup and SwapBarrier has been stored
//! \retval ::NVAPI_ERROR                The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT     One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED  NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D1x_QuerySwapGroup(IUnknown *pDevice, 
                                           IDXGISwapChain  *pSwapChain, 
                                           NvU32 *pSwapGroup, 
                                           NvU32 *pSwapBarrier); 
#endif // defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)

#if defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D1x_JoinSwapGroup
//
//!   DESCRIPTION: This API causes the SwapChain of a SwapGroup client to join or leave the specified SwapGroup.
//!
//! \param [in]   pDevice            The caller provides the D3D device that owns the SwapChain used as a SwapGroup client.
//!                                  pDevice can be either ID3D10Device or ID3D10Device1 or ID3D11Device or ID3D12Device.
//! \param [in]   pSwapChain         The IDXGISwapChain interface that is used as the SwapGroup client.
//! \param [in]   group              The caller specifies the SwapGroup which the SwapChain should join.
//!                                      - If the value of group is zero, the SwapChain leaves the SwapGroup.
//!                                      - The SwapChain joins a SwapGroup if the SwapGroup number is a positive integer less than or
//!                                        equal to the maximum number of SwapGroups queried by NvAPI_SwapGroup_QueryMaxSwapGroup.
//! \param [in]   blocking           The caller specifies that a presentation of this SwapChain should return immediately or block
//!                                  until all members of the SwapGroup are ready and the presentation was actually executed.
//!                                  A boolean value of false means the Present operation returns immediately and a value of true
//!                                  means the Present operation is blocking.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                   the SwapChain joined/left the SwapGroup accordingly
//! \retval ::NVAPI_ERROR                The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT     One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED  NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D1x_JoinSwapGroup(IUnknown *pDevice, 
                                          IDXGISwapChain  *pSwapChain, 
                                          NvU32 group,
                                          BOOL blocking);
#endif // defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)

#if defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D1x_BindSwapBarrier
//
//!   DESCRIPTION: This API causes a SwapGroup to be bound to or released from the specified SwapBarrier.
//!
//! \param [in]   pDevice            The caller provides the D3D device that owns the SwapChain used as a SwapGroup client.
//!                                  pDevice can be either ID3D10Device or ID3D10Device1 or ID3D11Device or ID3D12Device.
//! \param [in]   group              The caller specifies the SwapGroup to be bound to the SwapBarrier.
//! \param [in]   barrier            The caller specifies the SwapBarrier that the SwapGroup should be bound to.
//!                                      - If the value of barrier is zero, the SwapGroup releases the SwapBarrier.
//!                                      - The SwapGroup will be bound to the SwapBarrier if the value of barrier is a positive 
//!                                        integer less than or equal to the maximum number of SwapBarriers queried by NvAPI_D3D1x_QueryMaxSwapGroup.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \retval ::NVAPI_OK                  the SwapGroup is bound to the specified SwapBarrier
//! \retval ::NVAPI_ERROR                The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT     One or more args passed in are invalid.
//! \retval ::NVAPI_API_NOT_INITIALIZED  NvAPI was not yet initialized.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D1x_BindSwapBarrier(IUnknown *pDevice, 
                                            NvU32 group, 
                                            NvU32 barrier); 
#endif // defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__) || defined(__d3d12_h__))

enum NVAPI_QUAD_FILLMODE
{
    NVAPI_QUAD_FILLMODE_DISABLED = 0,
    NVAPI_QUAD_FILLMODE_BBOX = 1,
    NVAPI_QUAD_FILLMODE_FULL_VIEWPORT = 2,
};

#endif //defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__) || defined(__d3d12_h__))

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

typedef struct NvAPI_D3D11_RASTERIZER_DESC_EX
{
    // D3D11_RASTERIZER_DESC member variables
    D3D11_FILL_MODE FillMode;
    D3D11_CULL_MODE CullMode;
    BOOL FrontCounterClockwise;
    INT DepthBias;
    FLOAT DepthBiasClamp;
    FLOAT SlopeScaledDepthBias;
    BOOL DepthClipEnable;
    BOOL ScissorEnable;
    BOOL MultisampleEnable;
    BOOL AntialiasedLineEnable;

    // NvAPI_D3D11_RASTERIZER_DESC_EX specific member variables
    NvU32 ForcedSampleCount;                 //<! Added DX 11.1, part of _DESC1 version of this struct.
    bool ProgrammableSamplePositionsEnable;  //<! enable Programmable Samples feature
    bool InterleavedSamplingEnable;          //<! when jitter is enabled, an app need to fill the whole arrays below, otherwise only as much entries as samples
    NvU8 SampleCount;                        //<! number of samples. In TIR N->1 it needs to match N, in non-TIR it needs to match RT sample count. Ignored if ForcePerSampleInterlock is set
    NvU8 SamplePositionsX[16];               //<! x positions in API sample order
    NvU8 SamplePositionsY[16];               //<! y positions in API sample order
    bool ConservativeRasterEnable;           //<! rasterize all pixels a primitive touches in any way instead of just those with the centroid covered. 
    NVAPI_QUAD_FILLMODE QuadFillMode;        //<! Fill a triangle outside its bounds as a screen-aligned quad, matching the tri's bounding-box or filling the full viewport.
    bool PostZCoverageEnable;                //<! Enable pixel-shader input SV_COVERAGE to account for z-test in early-z mode.
    bool CoverageToColorEnable;              //<! Enable output of coverage to a color render-target.
    NvU8 CoverageToColorRTIndex;             //<! Index of RT for coverage-to-color.
    bool TargetIndepentRasterWithDepth;      //<! TargetIndepentRasterWithDepth = TRUE enables rasterezation mode where sample count of both raster and depth-stencil buffer are equal and do not match RT sample count.
    NvU8 reserved[63];                       //<! reserved for expansion, set to zero.
} NvAPI_D3D11_RASTERIZER_DESC_EX;
 
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateRasterizerState
//
//! \code
//!   DESCRIPTION: This function is an extension of ID3D11Device::CreateRasterizerState with additional raster states
//!
//!                If programmable sample positions is used, to decompress the surface using the currently bound
//!                programmable sample positions, use function NvAPI_D3D11_DecompressView.
//!
//!                This function is  free-threaded create compatible i.e. it can be called from a different thread 
//!                than the one calling immediate device setstate functions.   
//!
//!         \param [in]        pDevice             current d3d device
//!         \param [in]        pRasterizerDesc     Rasterizer state description of type NVAPI_D3D11_RASTERIZER_DESC_EX
//!         \param [out]       ppRasterizerState   ID3D11RasterizerState 
//!
//!
//! \return ::NVAPI_OK     if the call succeeds.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_CreateRasterizerState(__in ID3D11Device *pDevice,
                                                  __in const NvAPI_D3D11_RASTERIZER_DESC_EX *pRasterizerDesc,
                                                  __out ID3D11RasterizerState **ppRasterizerState);

#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined (__cplusplus) && (defined(_D3D9_H_) || defined(__d3d11_h__) || defined(__d3d11_1_h__) || defined(__d3d12_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_ConfigureAnsel
//
//! \code
//!   DESCRIPTION: This function configure the setting of AnselShim, including hotkey.
//!
//!         \param [in]        pDevice             current d3d device (should be ID3D11Device, ID3D10Device, or ID3D12Device)
//!         \param [in]        pAnselConfig        configuration of Ansel to be set, including hotkey setting
//!
//!
//! \return ::NVAPI_OK     if the call succeeds.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

typedef enum _NVAPI_ANSEL_FEATURE
{
    NVAPI_ANSEL_FEATURE_UNKNOWN         = 0,
    NVAPI_ANSEL_FEATURE_BLACK_AND_WHITE = 1,
    NVAPI_ANSEL_FEATURE_HUDLESS         = 2
} NVAPI_ANSEL_FEATURE;

typedef enum _NVAPI_ANSEL_FEATURE_STATE
{
    NVAPI_ANSEL_FEATURE_STATE_UNKNOWN   = 0,
    NVAPI_ANSEL_FEATURE_STATE_ENABLE    = 1,    //!< Toggle feature on
    NVAPI_ANSEL_FEATURE_STATE_DISABLE   = 2     //!< Toggle feature off
} NVAPI_ANSEL_FEATURE_STATE;

typedef enum _NVAPI_ANSEL_HOTKEY_MODIFIER
{
    NVAPI_ANSEL_HOTKEY_MODIFIER_UNKNOWN = 0,
    NVAPI_ANSEL_HOTKEY_MODIFIER_CTRL    = 1,    //!< Use control in the hotkey combination
    NVAPI_ANSEL_HOTKEY_MODIFIER_SHIFT   = 2,    //!< Use shift in the hotkey combination
    NVAPI_ANSEL_HOTKEY_MODIFIER_ALT     = 3     //!< Use alternate in the hotkey combination
} NVAPI_ANSEL_HOTKEY_MODIFIER;

typedef struct NVAPI_ANSEL_FEATURE_CONFIGURATION_STRUCT {
    NVAPI_ANSEL_FEATURE featureId;              //!< Id of the feature
    NVAPI_ANSEL_FEATURE_STATE featureState;     //!< Whether the feature is enabled or not
    UINT hotkey;                                //!< An optional virtual key associated with this feature
} NVAPI_ANSEL_FEATURE_CONFIGURATION_STRUCT;

typedef struct NVAPI_ANSEL_CONFIGURATION_STRUCT_V1 {
    NvU32 version;                                                //!< Structure version
    NVAPI_ANSEL_HOTKEY_MODIFIER hotkeyModifier;                   //!< Modifier key to use in hotkey combination
    UINT keyEnable;                                               //!< VKEY to enable/disable Ansel
    UINT numAnselFeatures;                                        //!< Number of features in pAnselFeatures
    NVAPI_ANSEL_FEATURE_CONFIGURATION_STRUCT * pAnselFeatures; //!< Array of features configurations
} NVAPI_ANSEL_CONFIGURATION_STRUCT_V1;

typedef NVAPI_ANSEL_CONFIGURATION_STRUCT_V1 NVAPI_ANSEL_CONFIGURATION_STRUCT;

#define NVAPI_ANSEL_CONFIGURATION_STRUCT_VER1  MAKE_NVAPI_VERSION(NVAPI_ANSEL_CONFIGURATION_STRUCT_V1,1)
#define NVAPI_ANSEL_CONFIGURATION_STRUCT_VER NVAPI_ANSEL_CONFIGURATION_STRUCT_VER1

NVAPI_INTERFACE NvAPI_D3D_ConfigureAnsel(__in IUnknown *pDevice,
                                         __in NVAPI_ANSEL_CONFIGURATION_STRUCT *pNLSConfig);

#endif //defined (__cplusplus) && (defined(_D3D9_H_) || defined(__d3d11_h__) || defined(__d3d11_1_h__) || defined(__d3d12_h__))

//! SUPPORTED OS:  Windows 8 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_2_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateTiledTexture2DArray
//
//! \since Release: 375
//
//! \code
//!   DESCRIPTION: Tiled resource is supported for Texture2D Array, however, but only when mip packing is not triggered.
//!                So any mip level cannot be smaller than a single tile size(64KB).
//!                This set of API is an extension of D3D11 support for tiled resource to allow a tiled texture2D array with mip packing.
//!                If any of API from this set is used, using all of them is highly recommended.
//!                It includes NvAPI_D3D11_CreateTiledTexture2DArray, NvAPI_D3D11_TiledTexture2DArrayGetDesc,
//!                NvAPI_D3D11_UpdateTileMappings, NvAPI_D3D11_CopyTileMappings, NvAPI_D3D11_TiledResourceBarrier.
//!                Reminder: all API in this set other than NvAPI_D3D11_CreateTiledTexture2DArray won't has D3D Debug Layer information.
//!
//!                NvAPI_D3D11_CreateTiledTexture2DArray is an extension of ID3D11Device::CreateTexutre2D.
//!                Use this function to create a tiled Texture2D array with mip packing.
//!                Runtime doesn't know the created resource is actually a tiled resource.
//!                Any other D3D11 API where runtime will check whether resource is tiled or not has a corresponding NVAPI version and they should be used.
//!                Different from DX12 implementation, this API should only be called when creating a tiled texture2D array with mip packing.
//!                Other normal tiled resource following D3D spec must use the standard ID3D11Device::CreateTexutre2D to create.
//!
//!         \param [in]        pDevice             current d3d device
//!         \param [in]        pDesc               The Texture2D Array descriptor, ArraySize > 1 && (pDesc->MiscFlags&D3D11_RESOURCE_MISC_TILED)
//!         \param [in]        pInitialData        A pointer to an array of D3D11_SUBRESOURCE_DATA structures that describe subresources for the 2D texture resource.
//!         \param [out]       ppTexture2D         A pointer to a buffer that receives a pointer to a ID3D11Texture2D interface for the created texture.

//!
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_CreateTiledTexture2DArray(__in       ID3D11Device           *pDevice,
                                                      __in const D3D11_TEXTURE2D_DESC   *pDesc,
                                                      __in const D3D11_SUBRESOURCE_DATA *pInitialData,
                                                      __out      ID3D11Texture2D        **ppTexture2D);

#endif //defined(__cplusplus) && defined(__d3d11_2_h__)

//! SUPPORTED OS:  Windows 10 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_h__)

typedef enum _NV_D3D11_FEATURE
{
    NV_D3D11_FEATURE_RASTERIZER,
} NV_D3D11_FEATURE;

typedef struct _NV_D3D11_FEATURE_DATA_RASTERIZER_SUPPORT
{
    BOOL TargetIndependentRasterWithDepth;
    BOOL ProgrammableSamplePositions;
    BOOL InterleavedSampling;
    BOOL ConservativeRaster;
    BOOL PostZCoverage;
    BOOL CoverageToColor;
} NV_D3D11_FEATURE_DATA_RASTERIZER_SUPPORT;

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CheckFeatureSupport
//
//!   DESCRIPTION: This function gets information about the features that are supported by the current graphics driver.
//!
//!
//!
//! \param [in]        pDevice                 The device on which to query for support.
//! \param [in]        Feature                 A member of the NvAPI_D3D11_FEATURE enumerated type that describes which feature to query for suppor.
//! \param [in]        pFeatureSupportData     Upon completion of the method, the passed structure is filled with data that describes the feature support.
//! \param [out]       FeatureSupportDataSize  The size of the structure passed to the pFeatureSupportData parameter.
//!
//! \since Release: 410
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//! \retval ::         Returns NVAPI_OK if successful; returns NVAPI_INVALID_ARGUMENT if an unsupported data type is passed to the pFeatureSupportData parameter
//!                    or a size mismatch is detected for the FeatureSupportDataSize parameter;
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_CheckFeatureSupport(__in  ID3D11Device        *pDevice,
                                                __in  NV_D3D11_FEATURE    Feature,
                                                __out void                *pFeatureSupportData,
                                                __in  UINT                FeatureSupportDataSize);

#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 10 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateImplicitMSAATexture2D
//
//! \since Release: 410
//
//! \code
//!   DESCRIPTION: NvAPI_D3D11_CreateImplicitMSAATexture2D is a simple wrapper of ID3D11Device::CreateTexture2D 
//!                which allows to create multisampled 2D texture that is exposed to DX runtime as non-multisampled texture.
//!
//!         \param [in]       pDevice      Current d3d device
//!         \param [in]       pDesc        A pointer to a D3D11_TEXTURE2D_DESC structure that describes a 2D texture resource.
//!                                        To create a typeless resource that can be interpreted at runtime into different,
//!                                        compatible formats, specify a typeless format in the texture description.
//!                                        To generatemipmap levels automatically, set the number of mipmap levels to 0.
//!                                        SampleDesc.SampleCount specifies actual resource sample count, while D3D runtime object
//!                                        sees resource as non-multisampled.
//!
//!         \param [out]      ppTexture2D  A pointer to a buffer that receives a pointer to a ID3D11Texture2D interface for the
//!                                        created texture.
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  See MSDN for the API specific error codes.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_CreateImplicitMSAATexture2D(__in  ID3D11Device                 *pDevice,
                                                        __in  const D3D11_TEXTURE2D_DESC   *pDesc,
                                                        __out ID3D11Texture2D              **ppTexture2D);

#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 10 and higher
//!

#if defined (__cplusplus) && defined(__d3d12_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_CreateImplicitMSAATexture2D
//
//! \since Release: 410
//
//! \code
//!   DESCRIPTION: NvAPI_D3D12_CreateCommittedImplicitMSAATexture2D is a simple wrapper of ID3D12Device::CreateCommittedResource 
//!                which allows to create multisampled 2D texture that is exposed to DX runtime as non-multisampled texture.
//!
//!         \param [in]       pDevice      Current d3d device
//!         \param [in]       pDesc        A pointer to a D3D12_RESOURCE_DESC structure that describes a 2D texture resource.
//!                                        To create a typeless resource that can be interpreted at runtime into different,
//!                                        compatible formats, specify a typeless format in the texture description.
//!                                        To generatemipmap levels automatically, set the number of mipmap levels to 0.
//!                                        SampleDesc.SampleCount specifies actual resource sample count, while D3D runtime object
//!                                        sees resource as non-multisampled.
//!         \param [in]       pHeapProperties, HeapFlags, InitialResourceState, pOptimizedClearValue, riidResource  See D3D12 docs
//!
//!         \param [out]      ppResource   Same ID3D12Device::CreateCommittedResource 
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  See MSDN for the API specific error codes.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D12_CreateCommittedImplicitMSAATexture2D(
        __in  ID3D12Device* pDevice,
        __in  const D3D12_HEAP_PROPERTIES *pHeapProperties,
        D3D12_HEAP_FLAGS HeapFlags,
        __in  const D3D12_RESOURCE_DESC *pDesc,
        D3D12_RESOURCE_STATES InitialResourceState,
        __in_opt  const D3D12_CLEAR_VALUE *pOptimizedClearValue,
        REFIID riidResource,
        __out void **ppvResource);

#endif //defined(__cplusplus) && defined(__d3d12_h__)

 
//! SUPPORTED OS:  Windows 10 and higher
//!

//! \ingroup dx
//! Valid modes for NvAPI_D3D11_ResolveSubresourceRegion() and NvAPI_D3D12_ResolveSubresourceRegion
typedef enum _NV_RESOLVE_MODE {
    NV_RESOLVE_MODE_SAMPLE_0,
} NV_RESOLVE_MODE;

#if defined (__cplusplus) && defined(__d3d11_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_ResolveSubresourceRegion
//
//! \since Release: 410
//
//! \code
//!   DESCRIPTION: NvAPI_D3D11_ResolveSubresourceRegion is D3D11 an analog of D3D12 ResolveSubresourceRegion.
//!
//!         \param [in]       pDstResource    Destination resource. Must be a created with the D3D11_USAGE_DEFAULT flag and be single-sampled.
//!         \param [in]       DstSubresource  A zero-based index, that identifies the destination subresource. Use D3D11CalcSubresource to calculate the index.
//!         \param [in]       DstX            The X coordinate of the left-most edge of the destination region.
//!                                           The width of the destination region is the same as the width of the source rect.
//!
//!         \param [in]       DstY            The Y coordinate of the top-most edge of the destination region.
//!                                           The height of the destination region is the same as the height of the source rect.
//!
//!         \param [in]       pSrcResource    Source resource. Must be multisampled.
//!         \param [in]       SrcSubresource  The source subresource of the source resource.
//!         \param [in]       pSrcRect        Specifies the rectangular region of the source resource to be resolved.
//!                                           Passing NULL for pSrcRect specifies that the entire subresource is to be resolved.
//!
//!         \param [in]       Format          A DXGI_FORMAT that indicates how the multisampled resource will be resolved to a single-sampled resource.
//!         \param [in]       ResolveMode     Specifies the operation used to resolve the source samples. NV_RESOLVE_MODE_SAMPLE_0 is the only supported mode.
//!                                           NV_RESOLVE_MODE_SAMPLE_0 outputs sample 0 and discards all other samples.
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  See MSDN for the API specific error codes.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_ResolveSubresourceRegion(
    __in  ID3D11Device     *pDevice,
    __in  ID3D11Texture2D  *pDstResource,
    __in  UINT              DstSubresource,
    __in  UINT              DstX,
    __in  UINT              DstY,
    __in  ID3D11Texture2D  *pSrcResource,
    __in  UINT              SrcSubresource,
    __in_opt const RECT    *pSrcRect,
    __in  DXGI_FORMAT       Format,
    __in  NV_RESOLVE_MODE   ResolveMode);

#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 10 and higher
//!

#if defined (__cplusplus) && defined(__d3d12_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_ResolveSubresourceRegion
//
//! \since Release: 410
//
//! \code
//!   DESCRIPTION: NvAPI_D3D12_ResolveSubresourceRegion is D3D11 an analog of D3D12 ResolveSubresourceRegion.
//!
//!         \param [in]       pDstResource    Destination resource. Must be a created with the D3D11_USAGE_DEFAULT flag and be single-sampled.
//!         \param [in]       DstSubresource  A zero-based index, that identifies the destination subresource. Use D3D11CalcSubresource to calculate the index.
//!         \param [in]       DstX            The X coordinate of the left-most edge of the destination region.
//!                                           The width of the destination region is the same as the width of the source rect.
//!
//!         \param [in]       DstY            The Y coordinate of the top-most edge of the destination region.
//!                                           The height of the destination region is the same as the height of the source rect.
//!
//!         \param [in]       pSrcResource    Source resource. Must be multisampled.
//!         \param [in]       SrcSubresource  The source subresource of the source resource.
//!         \param [in]       pSrcRect        Specifies the rectangular region of the source resource to be resolved.
//!                                           Passing NULL for pSrcRect specifies that the entire subresource is to be resolved.
//!
//!         \param [in]       Format          A DXGI_FORMAT that indicates how the multisampled resource will be resolved to a single-sampled resource.
//!         \param [in]       ResolveMode     Specifies the operation used to resolve the source samples. NV_RESOLVE_MODE_SAMPLE_0 is the only supported mode.
//!                                           NV_RESOLVE_MODE_SAMPLE_0 outputs sample 0 and discards all other samples.
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  See MSDN for the API specific error codes.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D12_ResolveSubresourceRegion(
    __in     ID3D12GraphicsCommandList1*pCommandList,
    __in     ID3D12Resource            *pDstResource,
    __in     UINT                       DstSubresource,
    __in     UINT                       DstX,
    __in     UINT                       DstY,
    __in     ID3D12Resource            *pSrcResource,
    __in     UINT                       SrcSubresource,
    __in_opt RECT                      *pSrcRect,
    __in     DXGI_FORMAT                Format,
    __in     NV_RESOLVE_MODE            ResolveMode);

#endif //defined(__cplusplus) && defined(__d3d12_h__)

//! SUPPORTED OS:  Windows 8 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_2_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_TiledTexture2DArrayGetDesc
//
//! \since Release: 375
//
//! \code
//!   DESCRIPTION: NvAPI_D3D11_TiledTexture2DArrayGetDesc is an simple wrapper of ID3D11Texture2D::GetDesc 
//!                when pTiledTexture2DArray is created with NvAPI_D3D11_CreateTiledTexture2DArray.
//!                Runtime doesn't know the created resource is actually a tiled resource.
//!                So calling ID3D11Texture2D::GetDesc will get a desc without D3D11_RESOURCE_MISC_TILED in MiscFlags.
//!                This wrapper API just adds D3D11_RESOURCE_MISC_TILED back.
//!
//!         \param [in]        pTiledTexture2DArray  Pointer of tiled texture2D array to get resource desc from.
//!         \param [out]       pDesc                 Pointer to a resource description.

//!
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_TiledTexture2DArrayGetDesc(__in  ID3D11Texture2D      *pTiledTexture2DArray,
                                                       __out D3D11_TEXTURE2D_DESC *pDesc);

#endif //defined(__cplusplus) && defined(__d3d11_2_h__)

//! SUPPORTED OS:  Windows 8 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_2_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_UpdateTileMappings
//
//! \since Release: 375
//
//! \code
//!   DESCRIPTION: NvAPI_D3D11_UpdateTileMappings is an extension of ID3D11DeviceContext2::UpdateTileMappings.
//!                It allows pTiledResource to be a resource created with NvAPI_D3D11_CreateTiledTexture2DArray, and should be used only in such case.
//!
//!         \param [in]        pDeviceContext                       Must be Immediate DeviceContext.
//!         \param [in]        pTiledResource                       A pointer to the tiled texture 2D array resource created by NvAPI_D3D11_CreateTiledTexture2DArray.
//!         \param [in]        NumTiledResourceRegions              The number of tiled resource regions.
//!         \param [in]        pTiledResourceRegionStartCoordinates An array of D3D11_TILED_RESOURCE_COORDINATE structures that describe the starting coordinates of the tiled resource regions. Cannot be NULL.
//!         \param [in]        pTiledResourceRegionSizes            An array of D3D11_TILE_REGION_SIZE structures that describe the sizes of the tiled resource regions. Cannot be NULL.
//!         \param [in]        pTilePool                            A pointer to the tile pool. This resource should be created by standard API.
//!         \param [in]        NumRanges                            The number of tile-pool ranges.
//!         \param [in]        pRangeFlags                          An array of D3D11_TILE_RANGE_FLAG values that describe each tile-pool range.
//!         \param [in]        pTilePoolStartOffsets                An array of offsets into the tile pool. These are 0-based tile offsets, counting in tiles (not bytes).
//!         \param [in]        pRangeTileCounts                     An array of values that specify the number of tiles in each tile-pool range.
//!         \param [in]        Flags                                A combination of D3D11_TILE_MAPPING_FLAGS values that are combined by using a bitwise OR operation.

//!
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_UpdateTileMappings(
    __in       ID3D11DeviceContext2            *pDeviceContext,
    __in       ID3D11Resource                  *pTiledResource,
    __in       UINT                             NumTiledResourceRegions,
    __in const D3D11_TILED_RESOURCE_COORDINATE *pTiledResourceRegionStartCoordinates,
    __in const D3D11_TILE_REGION_SIZE          *pTiledResourceRegionSizes,
    __in       ID3D11Buffer                    *pTilePool,
    __in       UINT                             NumRanges,
    __in const UINT                            *pRangeFlags,
    __in const UINT                            *pTilePoolStartOffsets,
    __in const UINT                            *pRangeTileCounts,
    __in       UINT                             Flags);

#endif //defined(__cplusplus) && defined(__d3d11_2_h__)

//! SUPPORTED OS:  Windows 8 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_2_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CopyTileMappings
//
//! \since Release: 375
//
//! \code
//!   DESCRIPTION: NvAPI_D3D11_CopyTileMappings is an extension of ID3D11DeviceContext2::CopyTileMappings 
//!                It allows pDestTiledResource or pSourceTiledResource or both to be created with NvAPI_D3D11_CreateTiledTexture2DArray.
//!                It should be used only in such case.
//!
//!         \param [in]        pDeviceContext                       Must be Immediate DeviceContext.
//!         \param [in]        pDestTiledResource                   Tiled resource created by NvAPI_D3D11_CreateTiledTexture2DArray to copy tile mappings into.
//!         \param [in]        pDestRegionStartCoordinate           A pointer to a D3D11_TILED_RESOURCE_COORDINATE structure that describes the starting coordinates of the destination tiled resource.
//!         \param [in]        pSourceTiledResource                 Tiled resource created by NvAPI_D3D11_CreateTiledTexture2DArray to copy tile mappings from.
//!         \param [in]        pSourceRegionStartCoordinate         A pointer to a D3D11_TILED_RESOURCE_COORDINATE structure that describes the starting coordinates of the source tiled resource.
//!         \param [in]        pTileRegionSize                      A pointer to a D3D11_TILE_REGION_SIZE structure that describes the size of the tiled region.
//!         \param [in]        Flags                                A combination of D3D11_TILE_MAPPING_FLAGS values that are combined by using a bitwise OR operation.

//!
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_CopyTileMappings(
    __in       ID3D11DeviceContext             *pDeviceContext,
    __in       ID3D11Resource                  *pDestTiledResource,
    __in const D3D11_TILED_RESOURCE_COORDINATE *pDestRegionStartCoordinate,
    __in       ID3D11Resource                  *pSourceTiledResource,
    __in const D3D11_TILED_RESOURCE_COORDINATE *pSourceRegionStartCoordinate,
    __in const D3D11_TILE_REGION_SIZE          *pTileRegionSize,
    __in       UINT                             Flags);

#endif //defined(__cplusplus) && defined(__d3d11_2_h__)

//! SUPPORTED OS:  Windows 8 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_2_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_TiledResourceBarrier
//
//! \since Release: 375
//
//! \code
//!   DESCRIPTION: NvAPI_D3D11_TiledResourceBarrier is an extension of ID3D11DeviceContext2::TiledResourceBarrier, but only works on ID3D11Resource(no support for ID3D11View).
//!                If pTiledResourceAccessBeforeBarrier or pTiledResourceAccessAfterBarrier or both are created by NvAPI_D3D11_CreateTiledTexture2DArray, 
//!                NvAPI_D3D11_TiledResourceBarrier must be used instead of ID3D11DeviceContext2::TiledResourceBarrier.
//!
//!         \param [in]        pDeviceContext                          Must be Immediate DeviceContext.
//!         \param [in]        pTiledResourceAccessBeforeBarrier       Access operations on this resource must complete before the access operations on the object that pTiledResourceAccessAfterBarrier specifies.
//!         \param [in]        pTiledResourceAccessAfterBarrier        Access operations on this resource must begin after the access operations on the object that pTiledResourceAccessBeforeBarrier specifies.

//!
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_TiledResourceBarrier(
    __in       ID3D11DeviceContext             *pDeviceContext,
    __in       ID3D11Resource                  *pTiledResourceAccessBeforeBarrier,
    __in       ID3D11Resource                  *pTiledResourceAccessAfterBarrier);

#endif //defined(__cplusplus) && defined(__d3d11_2_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_AliasMSAATexture2DAsNonMSAA
//
//! \code
//!   DESCRIPTION: This function allows creating (aliasing) a non-MSAA Texture2D object using the same memory as the given multi-sampled
//!                texture (pInputTex). The surface created would be bloated in width and height but it will have SampleCount = 1
//!                For 2X MSAA: OutTex.Width = InputTex.Width * 2, outTex.Height = InputTex.Height
//!                For 4X MSAA: OutTex.Width = InputTex.Width * 2, outTex.Height = InputTex.Height * 2
//!                For 8X MSAA: OutTex.Width = InputTex.Width * 4, outTex.Height = InputTex.Height * 2
//!                Only textures SampleQuality = 0 can be aliased as Non MSAA
//!                The app should ensure that original texture is released only after the aliased copy is released.
//!
//!                This function is  free-threaded create compatible i.e. it can be called from a different thread 
//!                than the one calling immediate device setstate functions.   
//!
//!         \param [in]        pDevice             current d3d device
//!         \param [in]        pInputTex           The MultiSampled Texture2D resource that is being aliased
//!         \param [out]       ppOutTex            The aliased non AA copy MultiSampled Texture2D resource
//!
//!
//! \return :: NVAPI_OK                   if the call succeeds.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_AliasMSAATexture2DAsNonMSAA(__in ID3D11Device *pDevice,
                                                        __in ID3D11Texture2D *pInputTex,
                                                        __out ID3D11Texture2D **ppOutTex);

#endif //defined(__cplusplus) && defined(__d3d11_h__)

#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__)) && (!defined(CINTERFACE))
typedef UINT NvAPI_D3D11_SWIZZLE_MODE;

typedef enum _NV_SWIZZLE_MODE
{
    NV_SWIZZLE_POS_X = 0,
    NV_SWIZZLE_NEG_X = 1,
    NV_SWIZZLE_POS_Y = 2,
    NV_SWIZZLE_NEG_Y = 3,
    NV_SWIZZLE_POS_Z = 4,
    NV_SWIZZLE_NEG_Z = 5,
    NV_SWIZZLE_POS_W = 6,
    NV_SWIZZLE_NEG_W = 7
}NV_SWIZZLE_MODE;

typedef enum _NV_SWIZZLE_OFFSET
{
    NV_SWIZZLE_OFFSET_X = 0,
    NV_SWIZZLE_OFFSET_Y = 4,
    NV_SWIZZLE_OFFSET_Z = 8,
    NV_SWIZZLE_OFFSET_W = 12
}NV_SWIZZLE_OFFSET;

#endif //defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__)) && (!defined(CINTERFACE))

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__)) && (!defined(CINTERFACE))
#define NV_CUSTOM_SEMANTIC_MAX_LIMIT 32

typedef enum NV_CUSTOM_SEMANTIC_TYPE
{
    NV_NONE_SEMANTIC                    = 0,
    NV_X_RIGHT_SEMANTIC                 = 1,
    NV_VIEWPORT_MASK_SEMANTIC           = 2,
    NV_XYZW_RIGHT_SEMANTIC              = 3,
    NV_VIEWPORT_MASK_2_SEMANTIC         = 4,

    NV_POSITION_SEMANTIC                = 5,
    NV_CLIP_DISTANCE_0_SEMANTIC         = 6,    // MultiView can accept upto two vec4 values. So the application should not use
    NV_CLIP_DISTANCE_1_SEMANTIC         = 7,    // more than 2 of the below Clip / Cull semantics in a single shader.
    NV_CULL_DISTANCE_0_SEMANTIC         = 8,
    NV_CULL_DISTANCE_1_SEMANTIC         = 9,
    NV_GENERIC_ATTRIBUTE_SEMANTIC       = 10,

    NV_PACKED_EYE_INDEX_SEMANTIC        = 17,
    NV_CUSTOM_SEMANTIC_MAX        = NV_CUSTOM_SEMANTIC_MAX_LIMIT,
} NV_CUSTOM_SEMANTIC_TYPE;

typedef struct _NV_CUSTOM_SEMANTIC
{
    UINT                        version;                                // NV_CUSTOM_SEMANTIC_VERSION

    NV_CUSTOM_SEMANTIC_TYPE     NVCustomSemanticType;                   // type of custom semantic (NV_CUSTOM_SEMANTIC_TYPE)
    NvAPI_LongString            NVCustomSemanticNameString;             // name of custom semantic e.g. "NV_X_RIGHT", "NV_VIEWPORT_MASK"
    BOOL                        RegisterSpecified;                      // (optional) set to TRUE to explicitly provide register number and mask as below
    NvU32                       RegisterNum;                            // (optional) output register which has the custom semantic.
    NvU32                       RegisterMask;                           // (optional) output register component mask which has the custom semantic (X:1, Y:2, Z:4)
    NvU32                       Reserved;                               // reserved
} NV_CUSTOM_SEMANTIC;

#define NV_CUSTOM_SEMANTIC_VERSION      MAKE_NVAPI_VERSION(NV_CUSTOM_SEMANTIC, 1)

#endif //defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__)) && (!defined(CINTERFACE))

#if defined (__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE))

typedef struct NvAPI_D3D11_CREATE_GEOMETRY_SHADER_EX_V5
{
    UINT version;

    BOOL UseViewportMask;
    BOOL OffsetRtIndexByVpIndex;
    BOOL ForceFastGS;
    BOOL DontUseViewportOrder;
    BOOL UseAttributeSkipMask;
    BOOL UseCoordinateSwizzle;
    NvAPI_D3D11_SWIZZLE_MODE *pCoordinateSwizzling;

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // pointer to array of NV_CUSTOM_SEMANTIC
    BOOL ConvertToFastGS;                               // reserved
    BOOL UseSpecificShaderExt;                          // TRUE if creating minimal specific shaders with nvapi shader extensions
} NvAPI_D3D11_CREATE_GEOMETRY_SHADER_EX_V5;

typedef NvAPI_D3D11_CREATE_GEOMETRY_SHADER_EX_V5        NvAPI_D3D11_CREATE_GEOMETRY_SHADER_EX;
#define NVAPI_D3D11_CREATEGEOMETRYSHADEREX_2_VER_5      MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_GEOMETRY_SHADER_EX_V5, 5)
#define NVAPI_D3D11_CREATEGEOMETRYSHADEREX_2_VERSION    NVAPI_D3D11_CREATEGEOMETRYSHADEREX_2_VER_5

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateGeometryShaderEx
//
//! \fn NvAPI_D3D11_CreateGeometryShaderEx
//!                                                
//!   DESCRIPTION: This function allows us to extend the creation of geometry shaders with extra bits
//!                of functionality.
//!                
//!                The first parameters are identical to ID3D11Device::CreateGeometryShader() 
//!                so please refer to its documentation for their usage.
//!                
//!                The new parameter is UseViewportMask which is to tell the driver to create a shader
//!                that outputs a viewport mask instead when a viewport index is indicated.
//!                Outputting a viewport mask allows a single primitive to land on many different viewports
//!                as specified by the bits set in the mask, rather than to rely on a single number that tells it
//!                which unique viewport it would be drawn on.
//!                This can be used for example in conjunction with the setting of coordinates swizzling (see XXX_NVAPI function)
//!                to generates multiple adjacent views of the same primitive in a more efficient fashion 
//!                (outputting the primitive only once).
//!                
//!                This function is  free-threaded create compatible i.e. it can be called from a different 
//!                thread than the one calling immediate device setstate functions.
//!                
//! \since Release: 
//!                
//!   \param [in]  pDevice               The device pointer
//!   \param [in]  pShaderBytecode       A pointer to the compiled shader.
//!   \param [in]  BytecodeLength        Size of the compiled geometry shader.
//!   \param [in]  pClassLinkage         A pointer to a class linkage interface. Can be NULL.
//!   \param [in]  UseViewportMask       Set to FALSE for custom semantic shaders. Tell the driver to create a shader that outputs the viewport mask in lieu of the viewport index. See above description.
//!   \param [in]  OffsetRtIndexByVpIndex Set to FALSE for custom semantic shaders. The Rendertarget index is offset by the viewport index
//!   \param [in]  ForceFastGS           If TRUE, GS must be written with maxvertexcount(1) and must pass-through input vertex 0 to the output without modification
//!   \param [in]  DontUseViewportOrder  Default FALSE for Primitives batched per viewport to improve performance. Set TRUE for API order (slow).
//!   \param [in]  UseAttributeSkipMask  reserved
//!   \param [in]  UseCoordinateSwizzle  reserved
//!   \param [in]  pCoordinateSwizzling  reserved
//!   \param [in]  NumCustomSemantics    Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
//!   \param [in]  pCustomSemantics      pointer to array of NV_CUSTOM_SEMANTIC
//!   \param [in]  ConvertToFastGS       reserved
//!   \param [in]  UseSpecificShaderExt  TRUE if creating minimal specific shaders with nvapi shader extensions
//!   \param [out] ppGeometryShader      Address of a pointer to a ID3D11GeometryShader interface. 
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_CreateGeometryShaderEx_2(__in ID3D11Device *pDevice, __in const void *pShaderBytecode, 
                                                     __in SIZE_T BytecodeLength, __in_opt ID3D11ClassLinkage *pClassLinkage, 
                                                     __in const NvAPI_D3D11_CREATE_GEOMETRY_SHADER_EX *pCreateGeometryShaderExArgs,
                                                    __out ID3D11GeometryShader **ppGeometryShader);

#endif //defined(__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE))

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE) ) 

typedef struct NvAPI_D3D11_CREATE_VERTEX_SHADER_EX_V1
{
    UINT version;

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // pointer to array of NV_CUSTOM_SEMANTIC
} NvAPI_D3D11_CREATE_VERTEX_SHADER_EX_V1;

typedef struct NvAPI_D3D11_CREATE_VERTEX_SHADER_EX_V2
{
    UINT version;

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // pointer to array of NV_CUSTOM_SEMANTIC
    BOOL UseWithFastGS;                                 // reserved
} NvAPI_D3D11_CREATE_VERTEX_SHADER_EX_V2;

typedef struct NvAPI_D3D11_CREATE_VERTEX_SHADER_EX_V3
{
    UINT version;

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // pointer to array of NV_CUSTOM_SEMANTIC
    BOOL UseWithFastGS;                                 // reserved
    BOOL UseSpecificShaderExt;                          // TRUE if creating minimal specific shaders with nvapi shader extensions
} NvAPI_D3D11_CREATE_VERTEX_SHADER_EX_V3;

typedef NvAPI_D3D11_CREATE_VERTEX_SHADER_EX_V3          NvAPI_D3D11_CREATE_VERTEX_SHADER_EX;
#define NVAPI_D3D11_CREATEVERTEXSHADEREX_VER_1          MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_VERTEX_SHADER_EX_V1, 1)
#define NVAPI_D3D11_CREATEVERTEXSHADEREX_VER_2          MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_VERTEX_SHADER_EX_V2, 2)
#define NVAPI_D3D11_CREATEVERTEXSHADEREX_VER_3          MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_VERTEX_SHADER_EX_V2, 3)
#define NVAPI_D3D11_CREATEVERTEXSHADEREX_VERSION        NVAPI_D3D11_CREATEVERTEXSHADEREX_VER_3

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateVertexShaderEx
//
//! \fn NvAPI_D3D11_CreateVertexShaderEx
//!                                                
//!   DESCRIPTION: This function allows us to extend the creation of vertex shaders with extra bits
//!                of functionality.
//!                
//!                The first parameters are identical to ID3D11Device::CreateVertexShader() 
//!                so please refer to its documentation for their usage.
//!                
//!                The new parameter are custom semantics which allow setting of custom semantic variables
//!                in the shader
//!
//!                This function is  free-threaded create compatible i.e. it can be called from a different thread 
//!                than the one calling immediate device setstate functions.  
//!                
//! \since Release: 
//!                
//!   \param [in]  pDevice               The device pointer
//!   \param [in]  pShaderBytecode       A pointer to the compiled shader.
//!   \param [in]  BytecodeLength        Size of the compiled vertex shader.
//!   \param [in]  pClassLinkage         A pointer to a class linkage interface. Can be NULL.
//!   \param [in]  NumCustomSemantics    Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
//!   \param [in]  pCustomSemantics      pointer to array of NV_CUSTOM_SEMANTIC
//!   \param [in]  UseWithFastGS         reserved
//!   \param [in]  UseSpecificShaderExt  TRUE if creating minimal specific shaders with nvapi shader extensions
//!   \param [out] ppVertexShader        Address of a pointer to a ID3D11VertexShader interface. 
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_CreateVertexShaderEx(__in ID3D11Device *pDevice, __in const void *pShaderBytecode, 
                                                 __in SIZE_T BytecodeLength, __in_opt ID3D11ClassLinkage *pClassLinkage, 
                                                 __in const NvAPI_D3D11_CREATE_VERTEX_SHADER_EX *pCreateVertexShaderExArgs,
                                                 __out ID3D11VertexShader **ppVertexShader);

#endif //defined(__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE))

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE) ) 

typedef struct NvAPI_D3D11_CREATE_HULL_SHADER_EX_V1
{
    UINT version;

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // pointer to array of NV_CUSTOM_SEMANTIC
    BOOL UseWithFastGS;                                 // reserved
} NvAPI_D3D11_CREATE_HULL_SHADER_EX_V1;

typedef struct NvAPI_D3D11_CREATE_HULL_SHADER_EX_V2
{
    UINT version;

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // pointer to array of NV_CUSTOM_SEMANTIC
    BOOL UseWithFastGS;                                 // reserved
    BOOL UseSpecificShaderExt;                          // TRUE if creating minimal specific shaders with nvapi shader extensions
} NvAPI_D3D11_CREATE_HULL_SHADER_EX_V2;

typedef NvAPI_D3D11_CREATE_HULL_SHADER_EX_V2          NvAPI_D3D11_CREATE_HULL_SHADER_EX;
#define NVAPI_D3D11_CREATEHULLSHADEREX_VER_1          MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_HULL_SHADER_EX_V1, 1)
#define NVAPI_D3D11_CREATEHULLSHADEREX_VER_2          MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_HULL_SHADER_EX_V1, 2)
#define NVAPI_D3D11_CREATEHULLSHADEREX_VERSION        NVAPI_D3D11_CREATEHULLSHADEREX_VER_2

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateHullShaderEx
//
//! \fn NvAPI_D3D11_CreateHullShaderEx
//!                                                
//!   DESCRIPTION: This function allows us to extend the creation of hull shaders with extra bits
//!                of functionality.
//!                
//!                The first parameters are identical to ID3D11Device::CreateHullShader() 
//!                so please refer to its documentation for their usage.
//!                
//!                The new parameter are custom semantics which allow setting of custom semantic variables
//!                in the shader
//!                
//!                This function is  free-threaded create compatible i.e. it can be called from a different thread 
//!                than the one calling immediate device setstate functions.  
//!                
//! \since Release: 
//!                
//!   \param [in]  pDevice               The device pointer
//!   \param [in]  pShaderBytecode       A pointer to the compiled shader.
//!   \param [in]  BytecodeLength        Size of the compiled hull shader.
//!   \param [in]  pClassLinkage         A pointer to a class linkage interface. Can be NULL.
//!   \param [in]  NumCustomSemantics    Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
//!   \param [in]  pCustomSemantics      pointer to array of NV_CUSTOM_SEMANTIC
//!   \param [in]  UseWithFastGS         reserved
//!   \param [in]  UseSpecificShaderExt  TRUE if creating minimal specific shaders with nvapi shader extensions
//!   \param [out] ppHullShader          Address of a pointer to a ID3D11HullShader interface. 
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_CreateHullShaderEx(__in ID3D11Device *pDevice, __in const void *pShaderBytecode, 
                                               __in SIZE_T BytecodeLength, __in_opt ID3D11ClassLinkage *pClassLinkage, 
                                               __in const NvAPI_D3D11_CREATE_HULL_SHADER_EX *pCreateHullShaderExArgs,
                                               __out ID3D11HullShader **ppHullShader);

#endif //defined(__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE))

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE) ) 

typedef struct NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX_V1
{
    UINT version;

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // pointer to array of NV_CUSTOM_SEMANTIC
} NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX_V1;

typedef struct NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX_V2
{
    UINT version;

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // pointer to array of NV_CUSTOM_SEMANTIC
    BOOL UseWithFastGS;                                 // reserved
} NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX_V2;

typedef struct NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX_V3
{
    UINT version;

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // pointer to array of NV_CUSTOM_SEMANTIC
    BOOL UseWithFastGS;                                 // reserved
    BOOL UseSpecificShaderExt;                          // TRUE if creating minimal specific shaders with nvapi shader extensions
} NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX_V3;

typedef NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX_V3          NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX;
#define NVAPI_D3D11_CREATEDOMAINSHADEREX_VER_1          MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX_V1, 1)
#define NVAPI_D3D11_CREATEDOMAINSHADEREX_VER_2          MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX_V2, 2)
#define NVAPI_D3D11_CREATEDOMAINSHADEREX_VER_3          MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX_V3, 3)
#define NVAPI_D3D11_CREATEDOMAINSHADEREX_VERSION        NVAPI_D3D11_CREATEDOMAINSHADEREX_VER_3

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateDomainShaderEx
//
//! \fn NvAPI_D3D11_CreateDomainShaderEx
//!                                                
//!   DESCRIPTION: This function allows us to extend the creation of domain shaders with extra bits
//!                of functionality.
//!                
//!                The first parameters are identical to ID3D11Device::CreateDomainShader() 
//!                so please refer to its documentation for their usage.
//!                
//!                The new parameter are custom semantics which allow setting of custom semantic variables
//!                in the shader
//!                
//!                This function is  free-threaded create compatible i.e. it can be called from a different thread 
//!                than the one calling immediate device setstate functions.  
//!                
//! \since Release: 
//!                
//!   \param [in]  pDevice               The device pointer
//!   \param [in]  pShaderBytecode       A pointer to the compiled shader.
//!   \param [in]  BytecodeLength        Size of the compiled domain shader.
//!   \param [in]  pClassLinkage         A pointer to a class linkage interface. Can be NULL.
//!   \param [in]  NumCustomSemantics    Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
//!   \param [in]  pCustomSemantics      pointer to array of NV_CUSTOM_SEMANTIC
//!   \param [in]  UseWithFastGS         reserved
//!   \param [in]  UseSpecificShaderExt  TRUE if creating minimal specific shaders with nvapi shader extensions
//!   \param [out] ppDomainShader        Address of a pointer to a ID3D11DomainShader interface. 
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_CreateDomainShaderEx(__in ID3D11Device *pDevice, __in const void *pShaderBytecode, 
                                                 __in SIZE_T BytecodeLength, __in_opt ID3D11ClassLinkage *pClassLinkage, 
                                                 __in const NvAPI_D3D11_CREATE_DOMAIN_SHADER_EX *pCreateDomainShaderExArgs,
                                                 __out ID3D11DomainShader **ppDomainShader);

#endif //defined(__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE))

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE) ) 

typedef struct NvAPI_D3D11_CREATE_PIXEL_SHADER_EX_V1
{
    UINT version;

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // pointer to array of NV_CUSTOM_SEMANTIC
} NvAPI_D3D11_CREATE_PIXEL_SHADER_EX_V1;

#define NVAPI_D3D11_CREATEPIXELSHADEREX_VER_1           MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_PIXEL_SHADER_EX_V1, 1)

typedef struct NvAPI_D3D11_CREATE_PIXEL_SHADER_EX_V2
{
    UINT version;                                                       // Always use NVAPI_D3D11_CREATEPIXELSHADEREX_VERSION

    NvU32 NumCustomSemantics;                                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;                               // pointer to array of NV_CUSTOM_SEMANTIC
    NvU32 bEnableSuperSamplingPredicationForVRS                 : 1;    // This enables sampling within a pixel for SuperSampling mode of Variable Rate Shading for relevant attributes tagged with "sample" modifier
    NvU32 bEnableSuperSamplingPredicationForVRSAllAttributes    : 1;    // This enables sampling within a pixel for SuperSampling mode of Variable Rate Shading for all relevant attributes
    NvU32 reserved : 30;                                                // Reserved for further use
} NvAPI_D3D11_CREATE_PIXEL_SHADER_EX_V2;

typedef NvAPI_D3D11_CREATE_PIXEL_SHADER_EX_V2           NvAPI_D3D11_CREATE_PIXEL_SHADER_EX;
#define NVAPI_D3D11_CREATEPIXELSHADEREX_VER_2           MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_PIXEL_SHADER_EX_V2, 2)
#define NVAPI_D3D11_CREATEPIXELSHADEREX_VERSION         NVAPI_D3D11_CREATEPIXELSHADEREX_VER_2

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreatePixelShaderEx_2
//
//! \fn NvAPI_D3D11_CreatePixelShaderEx_2
//!                                                
//!   DESCRIPTION: This function allows us to extend the creation of pixel shaders with extra bits
//!                of functionality.
//!                
//!                The first parameters are identical to ID3D11Device::CreatePixelShader() 
//!                so please refer to its documentation for their usage.
//!                
//!                The new parameter are custom semantics which allow setting of custom semantic variables
//!                in the shader
//!
//!                This function is  free-threaded create compatible i.e. it can be called from a different thread 
//!                than the one calling immediate device setstate functions.  
//!                
//! \since Release: 410
//!                
//!   \param [in]  pDevice               The device pointer
//!   \param [in]  pShaderBytecode       A pointer to the compiled shader.
//!   \param [in]  BytecodeLength        Size of the compiled domain shader.
//!   \param [in]  pClassLinkage         A pointer to a class linkage interface. Can be NULL.
//!   \param [in]  NumCustomSemantics    Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
//!   \param [in]  pCustomSemantics      pointer to array of NV_CUSTOM_SEMANTIC
//!   \param [out] ppPixelShader         Address of a pointer to a ID3D11PixelShader interface. 
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_CreatePixelShaderEx_2(__in ID3D11Device *pDevice, __in const void *pShaderBytecode, 
                                                 __in SIZE_T BytecodeLength, __in_opt ID3D11ClassLinkage *pClassLinkage, 
                                                 __in const NvAPI_D3D11_CREATE_PIXEL_SHADER_EX *pCreatePixelShaderExArgs,
                                                 __out ID3D11PixelShader **ppPixelShader);

#endif //defined(__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE))

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__)) && (!defined(CINTERFACE))

typedef enum _NV_FASTGS_FLAGS
{
    NV_FASTGS_USE_VIEWPORT_MASK             = 0x01,   // Causes SV_ViewportArrayIndex value to be interpreted as a bitmask of viewports to broadcast to.
    NV_FASTGS_OFFSET_RT_INDEX_BY_VP_INDEX   = 0x02,   // Causes SV_RenderTargetArrayIndex value to be offset by the viewport index when broadcasting.
    NV_FASTGS_STRICT_API_ORDER              = 0x04,   // Causes broadcast primitives to be rendered strictly in API order (slow).
                                                      // By default, primitives may be batched per viewport to improve performance.
} NV_FASTGS_FLAGS;

#endif //defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__)) && (!defined(CINTERFACE))

#if defined (__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE))

struct NvAPI_D3D11_CREATE_FASTGS_EXPLICIT_DESC_V1
{
    NvU32 version;                                   // ALWAYS == NVAPI_D3D11_CREATEFASTGSEXPLICIT_VER
    NvU32 flags;                                     // A combination of flags from NV_FASTGS_FLAGS
    NvAPI_D3D11_SWIZZLE_MODE *pCoordinateSwizzling;  // [optional] Array of 16 coordinate swizzle modes, one per viewport. NULL if not used.
                                                     // The output x, y, z, and w coordinates of all vertices can be set to any of the coordinates or their 
                                                     // negated versions i.e. {x, y, z, w, -x, -y, -z, -w}. Coordinates are swizzled before any viewport 
                                                     // operation occurs i.e. before frustum clipping, scaling, and viewport clipping. And after 
                                                     // last of vertex/tesselation/geometry shader stage, stream-out and viewport broadcast expansion (see NV_FASTGS_USE_VIEWPORT_MASK)
                                                     // pCoordinateSwizzling[i] sets the swizzle-mode of each component for viewport i.
                                                     // See NV_SWIZZLE_MODE for values of allowed swizzle modes.
                                                     // See NV_SWIZZLE_OFFSET for bit offset from where NV_SWIZZLE_MODE to be set for each component.
                                                     // For example : 
                                                     // 1. To set swizzle for viewport 0 such that -  w and z are unchanged and values of x and y are swapped : 
                                                     // pCoordinateSwizzling[0] = (NV_SWIZZLE_POS_W << NV_SWIZZLE_OFFSET_W) | 
                                                     //                           (NV_SWIZZLE_POS_Z << NV_SWIZZLE_OFFSET_Z) | 
                                                     //                           (NV_SWIZZLE_POS_X << NV_SWIZZLE_OFFSET_Y) | 
                                                     //                           (NV_SWIZZLE_POS_Y << NV_SWIZZLE_OFFSET_X);
                                                     // 2. To set swizzle for viewport 0 such that -  w, z and y are unchanged and value of x is negated :
                                                     // pCoordinateSwizzling[0] = (NV_SWIZZLE_POS_W << NV_SWIZZLE_OFFSET_W) | 
                                                     //                           (NV_SWIZZLE_POS_Z << NV_SWIZZLE_OFFSET_Z) | 
                                                     //                           (NV_SWIZZLE_POS_Y << NV_SWIZZLE_OFFSET_Y) | 
                                                     //                           (NV_SWIZZLE_NEG_X << NV_SWIZZLE_OFFSET_X);
                                                     // Need to set some valid combination of swizzle-modes for all viewports, irrespective of whether that viewport is set.
                                                     // Invalid swizzle-mode for any viewport (even if that viewport is not set) may result in removal of device.
};

#define NVAPI_D3D11_CREATEFASTGSEXPLICIT_VER1 MAKE_NVAPI_VERSION(NvAPI_D3D11_CREATE_FASTGS_EXPLICIT_DESC_V1, 1)
#define NVAPI_D3D11_CREATEFASTGSEXPLICIT_VER  NVAPI_D3D11_CREATEFASTGSEXPLICIT_VER1

typedef NvAPI_D3D11_CREATE_FASTGS_EXPLICIT_DESC_V1 NvAPI_D3D11_CREATE_FASTGS_EXPLICIT_DESC;

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateFastGeometryShaderExplicit
//
//! \fn NvAPI_D3D11_CreateFastGeometryShaderExplicit
//!
//!   DESCRIPTION: This function will create a fast geometry shader written using an "explicit"
//!                coding style, rather than converting a standard GS. For the explicit coding
//!                style, the GS must be written with maxvertexcount(1), and must pass-through
//!                input vertex 0 to the output without modification.
//!
//!                Additional per-primitive outputs may also be computed and written to the single
//!                output vertex. If these outputs are read by the pixel shader, they must be
//!                declared with the "nointerpolation" attribute in the PS input signature;
//!                otherwise, visual corruption may occur. Also, unlike D3D API, there is no guarantee 
//!                that pixel shader will get the default value of an attribute if that attribute is not written 
//!                by the earlier shader stage in the pipeline.
//!
//!                The first four parameters are identical to ID3D11Device::CreateGeometryShader(),
//!                so please refer to its documentation for their usage.
//!
//!                This function is  free-threaded create compatible i.e. it can be called from a different thread 
//!                than the one calling immediate device setstate functions.   
//!
//! \since Release:
//!
//!   \param [in]  pDevice               The device pointer
//!   \param [in]  pShaderBytecode       A pointer to the compiled shader.
//!   \param [in]  BytecodeLength        Size of the compiled geometry shader.
//!   \param [in]  pClassLinkage         A pointer to a class linkage interface. Can be NULL.
//!   \param [in]  pCreateFastGSArgs     A pointer to a NvAPI_D3D11_CREATE_FASTGS_EXPLICIT struct.
//!   \param [out] ppGeometryShader      Address of a pointer to a ID3D11GeometryShader interface.
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_CreateFastGeometryShaderExplicit(__in ID3D11Device *pDevice, __in const void *pShaderBytecode,
                                                             __in SIZE_T BytecodeLength, __in_opt ID3D11ClassLinkage *pClassLinkage,
                                                             __in const NvAPI_D3D11_CREATE_FASTGS_EXPLICIT_DESC *pCreateFastGSArgs,
                                                             __out ID3D11GeometryShader **ppGeometryShader);

#endif //defined(__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE))

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE) ) 
////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateFastGeometryShader
//
//! \fn NvAPI_D3D11_CreateFastGeometryShader
//!                                                
//!   DESCRIPTION: This function will convert a regular geometry shader into a fast GS variant if possible.
//!                It will not do any validation regarding the compatibility of the resulting fast GS with any 
//!                Pixel shader. The validation has to be done by the application manually.
//!                
//!                The parameters are identical to ID3D11Device::CreateGeometryShader() 
//!                so please refer to its documentation for their usage.
//!                
//!                If the shader is too complex or is not in adequate form to be converted to fast GS
//!                this function will simply fail. You should then call ID3D11Device::CreateGeometryShader() 
//!                to create the regular geometry shader.
//!                
//!                This function is  free-threaded create compatible i.e. it can be called from a different thread 
//!                than the one calling immediate device setstate functions.  
//!                
//! \since Release: 
//!                
//!   \param [in]  pDevice               The device pointer
//!   \param [in]  pShaderBytecode       A pointer to the compiled shader.
//!   \param [in]  BytecodeLength        Size of the compiled geometry shader.
//!   \param [in]  pClassLinkage         A pointer to a class linkage interface. Can be NULL.
//!   \param [out] ppGeometryShader      Address of a pointer to a ID3D11GeometryShader interface. 
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_CreateFastGeometryShader(__in ID3D11Device *pDevice, __in const void *pShaderBytecode, 
                                                     __in SIZE_T BytecodeLength, __in_opt ID3D11ClassLinkage *pClassLinkage,
                                                     __out ID3D11GeometryShader **ppGeometryShader);

#endif //defined(__cplusplus) && defined(__d3d11_h__) && (!defined(CINTERFACE))

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined (__cplusplus) && defined(__d3d11_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_DecompressView
//
//! \code
//!   DESCRIPTION: This function is used to decompress a surface using the currently bound programmable sample positions.
//!
//!                This is needed:
//!                  - When writing to a surface in a region previously rendered by different sample positions and no clear was done.
//!                  - When reading a surface in a shader that was rendered using non-standard sample positions.
//!                  - When copying from a surface that was rendered using non-standard sample positions.
//!
//!         \param [in]        pDevice             Current d3d11 device
//!         \param [in]        pDeviceContext      Current d3d11 device context
//!         \param [in]        pView               Current view to decompress
//!
//!
//! \return ::NVAPI_OK     if the call succeeds.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_DecompressView(__in ID3D11Device* pDevice, __in ID3D11DeviceContext *pDeviceContext, __in ID3D11View* pView);

#endif //defined(__cplusplus) && defined(__d3d11_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)

//! Enum for CreatePSO extensions.
//! \ingroup dx
typedef enum _NV_PSO_EXTENSION
{
    NV_PSO_RASTER_EXTENSION = 0,
    NV_PSO_REQUEST_FASTGS_EXTENSION = 1,
    NV_PSO_GEOMETRY_SHADER_EXTENSION = 2,
    NV_PSO_ENABLE_DEPTH_BOUND_TEST_EXTENSION = 3,
    NV_PSO_EXPLICIT_FASTGS_EXTENSION = 4,
    NV_PSO_SET_SHADER_EXTNENSION_SLOT_AND_SPACE = 5,
    NV_PSO_VERTEX_SHADER_EXTENSION = 6,
    NV_PSO_DOMAIN_SHADER_EXTENSION = 7,
    NV_PSO_HULL_SHADER_EXTENSION = 9,
}NV_PSO_EXTENSION;

struct NVAPI_D3D12_PSO_EXTENSION_DESC_V1 
{
    NvU32            baseVersion;         //<! Always use NV_PSO_EXTENSION_DESC_VER
    NV_PSO_EXTENSION psoExtension;
};

#define NV_PSO_EXTENSION_DESC_VER_1   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_EXTENSION_DESC_V1, 1)
#define NV_PSO_EXTENSION_DESC_VER     NV_PSO_EXTENSION_DESC_VER_1

typedef NVAPI_D3D12_PSO_EXTENSION_DESC_V1   NVAPI_D3D12_PSO_EXTENSION_DESC;


struct NVAPI_D3D12_PSO_RASTERIZER_STATE_DESC_V1 : public NVAPI_D3D12_PSO_EXTENSION_DESC
{
    NvU32 version;                           //<! Always use NV_RASTERIZER_PSO_EXTENSION_DESC_VER
    // These are additional parameters on the top of D3D12_RASTERIZER_DESC
    bool ProgrammableSamplePositionsEnable;  //<! enable Programmable Samples feature
    bool InterleavedSamplingEnable;          //<! when jitter is enabled, an app need to fill the whole arrays below, otherwise only as much entries as samples
    NvU8 SampleCount;                        //<! number of samples. In TIR N->1 it needs to match N, in non-TIR it needs to match RT sample count. Ignored if ForcePerSampleInterlock is set
    NvU8 SamplePositionsX[16];               //<! x positions in API sample order
    NvU8 SamplePositionsY[16];               //<! y positions in API sample order
    NVAPI_QUAD_FILLMODE QuadFillMode;        //<! Fill a triangle outside its bounds as a screen-aligned quad, matching the tri's bounding-box or filling the full viewport.
    bool PostZCoverageEnable;                //<! Enable pixel-shader input SV_COVERAGE to account for z-test in early-z mode.
    bool CoverageToColorEnable;              //<! Enable output of coverage to a color render-target.
    NvU8 CoverageToColorRTIndex;             //<! Index of RT for coverage-to-color.
    
    //Added with NV_RASTERIZER_PSO_EXTENSION_DESC_VER_2
    bool TargetIndepentRasterWithDepth;      //<! TargetIndepentRasterWithDepth = TRUE enables rasterezation mode where sample count of both raster and depth-stencil buffer are equal and do not match RT sample count.
    NvU8 ForcedSampleCount;                  //<! Must be set when TargetIndepentRasterWithDepth is true - refers to SampleDesc.Count for the DSV
    
    //Reserved
    NvU8 reserved[62];                       //<! reserved for expansion, set to zero.
};

#define NV_RASTERIZER_PSO_EXTENSION_DESC_VER_1   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_RASTERIZER_STATE_DESC_V1, 1)
#define NV_RASTERIZER_PSO_EXTENSION_DESC_VER_2   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_RASTERIZER_STATE_DESC_V1, 2)
#define NV_RASTERIZER_PSO_EXTENSION_DESC_VER     NV_RASTERIZER_PSO_EXTENSION_DESC_VER_2

typedef NVAPI_D3D12_PSO_RASTERIZER_STATE_DESC_V1   NVAPI_D3D12_PSO_RASTERIZER_STATE_DESC;

struct NVAPI_D3D12_PSO_CREATE_FASTGS_EXPLICIT_DESC_V1 : public NVAPI_D3D12_PSO_EXTENSION_DESC
{
    NvU32 version;                                   // ALWAYS == NV_FASTGS_EXPLICIT_PSO_EXTENSION_VER
    NvU32 flags;                                     // A combination of flags from NV_FASTGS_FLAGS
    NvAPI_D3D11_SWIZZLE_MODE *pCoordinateSwizzling;  // [optional] Array of 16 coordinate swizzle modes, one per viewport. NULL if not used.
                                                     // The output x, y, z, and w coordinates of all vertices can be set to any of the coordinates or their 
                                                     // negated versions i.e. {x, y, z, w, -x, -y, -z, -w}. Coordinates are swizzled before any viewport 
                                                     // operation occurs i.e. before frustum clipping, scaling, and viewport clipping. And after 
                                                     // last of vertex/tesselation/geometry shader stage, stream-out and viewport broadcast expansion (see NV_FASTGS_USE_VIEWPORT_MASK)
                                                     // pCoordinateSwizzling[i] sets the swizzle-mode of each component for viewport i.
                                                     // See NV_SWIZZLE_MODE for values of allowed swizzle modes.
                                                     // See NV_SWIZZLE_OFFSET for bit offset from where NV_SWIZZLE_MODE to be set for each component.
                                                     // For example : 
                                                     // 1. To set swizzle for viewport 0 such that -  w and z are unchanged and values of x and y are swapped : 
                                                     // pCoordinateSwizzling[0] = (NV_SWIZZLE_POS_W << NV_SWIZZLE_OFFSET_W) | 
                                                     //                           (NV_SWIZZLE_POS_Z << NV_SWIZZLE_OFFSET_Z) | 
                                                     //                           (NV_SWIZZLE_POS_X << NV_SWIZZLE_OFFSET_Y) | 
                                                     //                           (NV_SWIZZLE_POS_Y << NV_SWIZZLE_OFFSET_X);
                                                     // 2. To set swizzle for viewport 0 such that -  w, z and y are unchanged and value of x is negated :
                                                     // pCoordinateSwizzling[0] = (NV_SWIZZLE_POS_W << NV_SWIZZLE_OFFSET_W) | 
                                                     //                           (NV_SWIZZLE_POS_Z << NV_SWIZZLE_OFFSET_Z) | 
                                                     //                           (NV_SWIZZLE_POS_Y << NV_SWIZZLE_OFFSET_Y) | 
                                                     //                           (NV_SWIZZLE_NEG_X << NV_SWIZZLE_OFFSET_X);
                                                     // Need to set some valid combination of swizzle-modes for all viewports, irrespective of whether that viewport is set.
                                                     // Invalid swizzle-mode for any viewport (even if that viewport is not set) may result in removal of device.
};

#define NV_FASTGS_EXPLICIT_PSO_EXTENSION_VER_1 MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_CREATE_FASTGS_EXPLICIT_DESC_V1, 1)
#define NV_FASTGS_EXPLICIT_PSO_EXTENSION_VER   NV_FASTGS_EXPLICIT_PSO_EXTENSION_VER_1

typedef NVAPI_D3D12_PSO_CREATE_FASTGS_EXPLICIT_DESC_V1   NVAPI_D3D12_PSO_CREATE_FASTGS_EXPLICIT_DESC;


struct NVAPI_D3D12_PSO_REQUEST_FAST_GEOMETRY_SHADER_DESC_V1 : public NVAPI_D3D12_PSO_EXTENSION_DESC
{
    NvU32 version; //<! Always use NV_FAST_GEOMETRY_SHADER_PSO_EXTENSION_VER
};

#define NV_FAST_GEOMETRY_SHADER_PSO_EXTENSION_VER_1   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_REQUEST_FAST_GEOMETRY_SHADER_DESC_V1, 1)
#define NV_FAST_GEOMETRY_SHADER_PSO_EXTENSION_VER     NV_FAST_GEOMETRY_SHADER_PSO_EXTENSION_VER_1

typedef NVAPI_D3D12_PSO_REQUEST_FAST_GEOMETRY_SHADER_DESC_V1   NVAPI_D3D12_PSO_REQUEST_FAST_GEOMETRY_SHADER_DESC;


struct NVAPI_D3D12_PSO_GEOMETRY_SHADER_DESC_V5 : public NVAPI_D3D12_PSO_EXTENSION_DESC
{
    NvU32 version; //<! Always use NV_GEOMETRY_SHADER_PSO_EXTENSION_DESC_VER

    BOOL UseViewportMask;                               // Set to FALSE for custom semantic shaders. Tell the driver to create a shader that outputs the viewport mask in lieu of the viewport index. See above description.
    BOOL OffsetRtIndexByVpIndex;                        // Set to FALSE for custom semantic shaders. The Rendertarget index is offset by the viewport index
    BOOL ForceFastGS;                                   // If TRUE, GS must be written with maxvertexcount(1) and must pass-through input vertex 0 to the output without modification
    BOOL DontUseViewportOrder;                          // Default FALSE for Primitives batched per viewport to improve performance. Set TRUE for API order (slow).
    BOOL UseAttributeSkipMask;                          // Reserved
    BOOL UseCoordinateSwizzle;                          // Reserved
    NvAPI_D3D11_SWIZZLE_MODE *pCoordinateSwizzling;     // Reserved    

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // pointer to array of NV_CUSTOM_SEMANTIC

    BOOL ConvertToFastGS;                               // Tell the driver to attempt to create a fast geometry shader

    BOOL UseSpecificShaderExt;                          // TRUE if creating minimal specific shaders with nvapi shader extensions
};

#define NV_GEOMETRY_SHADER_PSO_EXTENSION_DESC_VER_5   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_GEOMETRY_SHADER_DESC_V5, 5)
#define NV_GEOMETRY_SHADER_PSO_EXTENSION_DESC_VER     NV_GEOMETRY_SHADER_PSO_EXTENSION_DESC_VER_5

typedef NVAPI_D3D12_PSO_GEOMETRY_SHADER_DESC_V5       NVAPI_D3D12_PSO_GEOMETRY_SHADER_DESC;


struct NVAPI_D3D12_PSO_VERTEX_SHADER_DESC_V1 : public NVAPI_D3D12_PSO_EXTENSION_DESC
{
    NvU32 version; //<! Always use NV_VERTEX_SHADER_PSO_EXTENSION_DESC_VER

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // Pointer to array of NV_CUSTOM_SEMANTIC
};

struct NVAPI_D3D12_PSO_VERTEX_SHADER_DESC_V2 : public NVAPI_D3D12_PSO_VERTEX_SHADER_DESC_V1
{
    BOOL UseWithFastGS;                                 // Reserved
};

struct NVAPI_D3D12_PSO_VERTEX_SHADER_DESC_V3 : public NVAPI_D3D12_PSO_VERTEX_SHADER_DESC_V2
{
    BOOL UseSpecificShaderExt;                          // TRUE if creating minimal specific shaders with NvAPI shader extensions
};

#define NV_VERTEX_SHADER_PSO_EXTENSION_DESC_VER_1   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_VERTEX_SHADER_DESC_V1, 1)
#define NV_VERTEX_SHADER_PSO_EXTENSION_DESC_VER_2   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_VERTEX_SHADER_DESC_V2, 2)
#define NV_VERTEX_SHADER_PSO_EXTENSION_DESC_VER_3   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_VERTEX_SHADER_DESC_V3, 3)
#define NV_VERTEX_SHADER_PSO_EXTENSION_DESC_VER     NV_VERTEX_SHADER_PSO_EXTENSION_DESC_VER_3

typedef NVAPI_D3D12_PSO_VERTEX_SHADER_DESC_V3       NVAPI_D3D12_PSO_VERTEX_SHADER_DESC;


struct NVAPI_D3D12_PSO_HULL_SHADER_DESC_V1 : public NVAPI_D3D12_PSO_EXTENSION_DESC
{
    NvU32 version; //<! Always use NV_HULL_SHADER_PSO_EXTENSION_DESC_VER

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // Pointer to array of NV_CUSTOM_SEMANTIC
    BOOL UseWithFastGS;                                 // Reserved
};

struct NVAPI_D3D12_PSO_HULL_SHADER_DESC_V2 : public NVAPI_D3D12_PSO_HULL_SHADER_DESC_V1
{
    BOOL UseSpecificShaderExt;                          // TRUE if creating minimal specific shaders with nvapi shader extensions
};

#define NV_HULL_SHADER_PSO_EXTENSION_DESC_VER_1   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_HULL_SHADER_DESC_V1, 1)
#define NV_HULL_SHADER_PSO_EXTENSION_DESC_VER_2   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_HULL_SHADER_DESC_V2, 2)
#define NV_HULL_SHADER_PSO_EXTENSION_DESC_VER     NV_HULL_SHADER_PSO_EXTENSION_DESC_VER_2

typedef NVAPI_D3D12_PSO_HULL_SHADER_DESC_V2       NVAPI_D3D12_PSO_HULL_SHADER_DESC;


struct NVAPI_D3D12_PSO_DOMAIN_SHADER_DESC_V1 : public NVAPI_D3D12_PSO_EXTENSION_DESC
{
    NvU32 version; //<! Always use NV_DOMAIN_SHADER_PSO_EXTENSION_DESC_VER

    NvU32 NumCustomSemantics;                           // Number of custom semantics elements (upto NV_CUSTOM_SEMANTIC_MAX) provided in array pointer pCustomSemantics
    NV_CUSTOM_SEMANTIC *pCustomSemantics;               // Pointer to array of NV_CUSTOM_SEMANTIC
};

struct NVAPI_D3D12_PSO_DOMAIN_SHADER_DESC_V2 : public NVAPI_D3D12_PSO_DOMAIN_SHADER_DESC_V1
{
    BOOL UseWithFastGS;                                 // Reserved
};

struct NVAPI_D3D12_PSO_DOMAIN_SHADER_DESC_V3 : public NVAPI_D3D12_PSO_DOMAIN_SHADER_DESC_V2
{
    BOOL UseSpecificShaderExt;                          // TRUE if creating minimal specific shaders with NvAPI shader extensions
};

#define NV_DOMAIN_SHADER_PSO_EXTENSION_DESC_VER_1   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_DOMAIN_SHADER_DESC_V1, 1)
#define NV_DOMAIN_SHADER_PSO_EXTENSION_DESC_VER_2   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_DOMAIN_SHADER_DESC_V2, 2)
#define NV_DOMAIN_SHADER_PSO_EXTENSION_DESC_VER_3   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_DOMAIN_SHADER_DESC_V3, 3)
#define NV_DOMAIN_SHADER_PSO_EXTENSION_DESC_VER     NV_DOMAIN_SHADER_PSO_EXTENSION_DESC_VER_3

typedef NVAPI_D3D12_PSO_DOMAIN_SHADER_DESC_V3       NVAPI_D3D12_PSO_DOMAIN_SHADER_DESC;


struct NVAPI_D3D12_PSO_ENABLE_DEPTH_BOUND_TEST_DESC_V1 : public NVAPI_D3D12_PSO_EXTENSION_DESC
{
    NvU32 version; //<! Always use NV_ENABLE_DEPTH_BOUND_TEST_PSO_EXTENSION_DESC_VER
    bool EnableDBT;
};

#define NV_ENABLE_DEPTH_BOUND_TEST_PSO_EXTENSION_DESC_VER_1   MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_ENABLE_DEPTH_BOUND_TEST_DESC_V1, 1)
#define NV_ENABLE_DEPTH_BOUND_TEST_PSO_EXTENSION_DESC_VER     NV_ENABLE_DEPTH_BOUND_TEST_PSO_EXTENSION_DESC_VER_1

typedef NVAPI_D3D12_PSO_ENABLE_DEPTH_BOUND_TEST_DESC_V1       NVAPI_D3D12_PSO_ENABLE_DEPTH_BOUND_TEST_DESC;


struct NVAPI_D3D12_PSO_SET_SHADER_EXTENSION_SLOT_DESC_V1 : public NVAPI_D3D12_PSO_EXTENSION_DESC
{
    NvU32 version; //<! Always use NV_SET_SHADER_EXTENSION_SLOT_DESC_VER
    NvU32 uavSlot;
    NvU32 registerSpace;
};

#define NV_SET_SHADER_EXTENSION_SLOT_DESC_VER_1               MAKE_NVAPI_VERSION(NVAPI_D3D12_PSO_SET_SHADER_EXTENSION_SLOT_DESC_V1, 1)
#define NV_SET_SHADER_EXTENSION_SLOT_DESC_VER                 NV_SET_SHADER_EXTENSION_SLOT_DESC_VER_1

typedef NVAPI_D3D12_PSO_SET_SHADER_EXTENSION_SLOT_DESC_V1     NVAPI_D3D12_PSO_SET_SHADER_EXTENSION_SLOT_DESC;


//! \ingroup dx
//! Enum for compute shader derivative modes
typedef enum _NV_COMPUTE_SHADER_DERIVATIVES
{
    NV_COMPUTE_SHADER_DERIVATIVE_NONE = 0,                  // No support for compute derivatives

    NV_COMPUTE_SHADER_DERIVATIVE_GROUP_LINEAR = 1,          // Compute derivatives supported. Quads are defined as groups of four threads with linear thread IDs of the form 4N..4N + 3

    NV_COMPUTE_SHADER_DERIVATIVE_GROUP_QUADS = 2,           // Compute derivatives supported. Quads are defined as group of 2x2 thread IDs in a 2D (or 3D) CTA. In this mode
                                                            // the app must ensure that width and height of the CTA dimension are both multiples of 2
} NV_COMPUTE_SHADER_DERIVATIVES;

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_CreateGraphicsPipelineState
//
//! \code
//!   DESCRIPTION: This function will create PSO with provided extensions
//!
//! \note   Note that "Cached PSO" functionality is not supported with the Pipeline State Object created using
//!         this NvAPI. GetCachedBlob() should not be called with such a PSO.
//!
//!         \param [in]        pDevice              Current d3d device
//!         \param [in]        pPSODesc             PSO description of type D3D12_GRAPHICS_PIPELINE_STATE_DESC
//!         \param [in]        numExtensions        Number of extensions
//!         \param [in]        ppExtensions         Array of PSO extensions (see NV_PSO_EXTENSION  for possible extensions)
//!         \param [out]       ppPSO                Output PSO object of type ID3D12PipelineState 
//!
//! SUPPORTED OS:  Windows 10
//!
//! \return ::NVAPI_OK     if the call succeeds.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D12_CreateGraphicsPipelineState(__in ID3D12Device *pDevice,
                                                        __in const D3D12_GRAPHICS_PIPELINE_STATE_DESC *pPSODesc,
                                                          NvU32 numExtensions,
                                                        __in const NVAPI_D3D12_PSO_EXTENSION_DESC** ppExtensions,
                                                        __out ID3D12PipelineState **ppPSO);

#endif //defined(__cplusplus) && defined(__d3d12_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_CreateComputePipelineState
//
//! \code
//!   DESCRIPTION: This function will create PSO with provided extensions
//!
//!         \param [in]        pDevice              Current d3d device
//!         \param [in]        pPSODesc             PSO description of type D3D12_COMPUTE_PIPELINE_STATE_DESC
//!         \param [in]        numExtensions        Number of extensions
//!         \param [in]        ppExtensions         Array of PSO extensions (see NV_PSO_EXTENSION  for possible extensions)
//!         \param [out]       ppPSO                Output PSO object of type ID3D12PipelineState 
//!
//! \since Release: 364
//!
//! SUPPORTED OS:  Windows 10
//!
//! \return ::NVAPI_OK     if the call succeeds.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D12_CreateComputePipelineState(__in ID3D12Device *pDevice,
                                                       __in const D3D12_COMPUTE_PIPELINE_STATE_DESC *pPSODesc,
                                                         NvU32 numExtensions,
                                                       __in const NVAPI_D3D12_PSO_EXTENSION_DESC** ppExtensions,
                                                       __out ID3D12PipelineState **ppPSO);

#endif //defined(__cplusplus) && defined(__d3d12_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_SetDepthBoundsTestValues
//
//! \code
//!   DESCRIPTION: This function will set the minDepth and maxDepth values for depth bounds test
//!                To enable/ disable depth bounds test use PSO extension NV_PSO_ENABLE_DEPTH_BOUND_TEST_EXTENSION
//!                in the nvapi NvAPI_D3D12_CreateGraphicsPipelineState
//!
//!         \param [in]        pCommandList         Command List to set depth bounds test
//!         \param [in]        minDepth             min value for depth bound test
//!         \param [in]        maxDepth             max value for depth bound test
//!
//! The valid values for minDepth and maxDepth are such that 0 <= minDepth <= maxDepth <= 1
//!
//! SUPPORTED OS:  Windows 10
//!
//! \return ::NVAPI_OK     if the call succeeds.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D12_SetDepthBoundsTestValues(__in ID3D12GraphicsCommandList *pCommandList,
                                                     __in const float minDepth,
                                                     __in const float maxDepth);

#endif //defined(__cplusplus) && defined(__d3d12_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_CreateReservedResource
//
//! \since Release: 375
//
//! \code
//!   DESCRIPTION: Tiled resource is supported for Texture2D Array, however, but only when mip packing is not triggered.
//!                So any mip level cannot be smaller than a single tile size(64KB).
//!                This set of API is an extension of D3D12 support for tiled resource to allow a tiled texture2D array with mip packing.
//!                If any of API from this set is used, using all of them is highly recommended.
//!                It includes NvAPI_D3D12_CreateReservedResource, NvAPI_D3D12_CreateHeap, NvAPI_D3D12_ReservedResourceGetDesc,
//!                NvAPI_D3D12_UpdateTileMappings, NvAPI_D3D12_CopyTileMappings, NvAPI_D3D12_ResourceAliasingBarrier.
//!                Reminder: all API in this set other than NvAPI_D3D12_CreateReservedResource won't has D3D Debug Layer information.
//!
//!                NvAPI_D3D12_CreateReservedResource is an extension of ID3D12Device::CreateReservedResource.
//!                Use this function to create a tiled Texture2D array with mip packing.
//!                Runtime doesn't know the created resource is actually a tiled resource.
//!                Any other D3D12 API where runtime will check whether resource is tiled or not, has a corresponding NVAPI version and they should be used.
//!                Different from DX11 implementation, we highly recommend replace all ID3D12Device::CreateReservedResource with NvAPI_D3D12_CreateReservedResource, 
//!                and use bTexture2DArrayMipPack to control which creation to use.
//!                Otherwise, NvAPI_D3D12_ResourceAliasingBarrier will fail if any resource is not created by NvAPI_D3D12_CreateReservedResource.
//!                DX11 implementation doesn't have this restriction and resource created by NVAPI and D3D API can be used together.
//!                pHeap is necessary when bTexture2DArrayMipPack is true. pHeap can be any heap and this API doens't change anything to it.
//!
//!         \param [in]        pDevice                A pointer to D3D12 device.
//!         \param [in]        pDesc                  A pointer to a D3D12_RESOURCE_DESC structure that describes the resource.
//!         \param [in]        InitialState           The initial state of the resource, as a bitwise-OR'd combination of D3D12_RESOURCE_STATES enumeration constants.
//!         \param [in]        pOptimizedClearValue   Specifies a D3D12_CLEAR_VALUE that describes the default value for a clear color.
//!         \param [in]        riid                   The globally unique identifier (GUID) for the resource interface.
//!         \param [out]       ppvResource            A pointer to a memory block that receives a pointer to the resource. Cannot be NULL.
//!         \param [in]        bTexture2DArrayMipPack Whether pDesc indicates it's a texture2D array resource with mip packing.
//!                                                   TRUE:  Use NVAPI to create. Will check pHeap to be not NULL.
//!                                                   FALSE: Standard D3D12 API will be used, use DebugDevice to check any runtime ERROR. Won't check pHeap
//!         \param [in]        pHeap                  A pointer to ID3D12Heap. Cannot be NULL when bTexture2DArrayMipPack is true.
//!
//! SUPPORTED OS:  Windows 10
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D12_CreateReservedResource(__in       ID3D12Device           *pDevice,
                                                   __in const D3D12_RESOURCE_DESC    *pDesc,
                                                   __in       D3D12_RESOURCE_STATES   InitialState,
                                                   __in const D3D12_CLEAR_VALUE      *pOptimizedClearValue,
                                                   __in       REFIID                  riid,
                                                   __out      void                  **ppvResource,
                                                   __in       bool                    bTexture2DArrayMipPack,
                                                   __in       ID3D12Heap             *pHeap);

#endif //defined(__cplusplus) && defined(__d3d12_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_CreateHeap
//
//! \since Release: 375
//
//! \code
//!   DESCRIPTION: NvAPI_D3D12_CreateHeap is a safe replacement of ID3D12Device::CreateHeap with no functionality change and overhead.
//!                In NvAPI_D3D12_UpdateTileMappings, pTilePool must be created with NvAPI_D3D12_CreateHeap and otherwise NvAPI_D3D12_UpdateTileMappings will fail.
//!                In other word, any tile pool used in NvAPI_D3D12_UpdateTileMappings must be created by NvAPI_D3D12_CreateHeap.
//!
//!         \param [in]        pDevice                A pointer to D3D12 device.
//!         \param [in]        pDesc                  A pointer to a D3D12_HEAP_DESC structure that describes the heap.
//!         \param [in]        riid                   The globally unique identifier (GUID) for the resource interface.
//!         \param [out]       ppvHeap                A pointer to a memory block that receives a pointer to the heap. Cannot be NULL.
//!
//! SUPPORTED OS:  Windows 10
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D12_CreateHeap(__in       ID3D12Device     *pDevice,
                                       __in const D3D12_HEAP_DESC  *pDesc,
                                       __in       REFIID            riid,
                                       __out      void            **ppvHeap);

#endif //defined(__cplusplus) && defined(__d3d12_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_ReservedResourceGetDesc
//
//! \since Release: 375
//
//! \code
//!   DESCRIPTION: NvAPI_D3D12_ReservedResourceGetDesc is an simple wrapper of ID3D12Resource::GetDesc when pReservedResource is created by NvAPI_D3D12_CreateReservedResource.
//!                Runtime doesn't know the created resource is actually a tiled resource if bTexture2DArrayMipPack = true in NvAPI_D3D12_CreateReservedResource.
//!                So calling ID3D12Resource::GetDesc on such resource will get a desc with D3D12_TEXTURE_LAYOUT_UNKNOWN in Layout instead of D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE.
//!                This wrapper API just set Layout to D3D12_TEXTURE_LAYOUT_64KB_UNDEFINED_SWIZZLE if Layout is D3D12_TEXTURE_LAYOUT_UNKNOWN.
//!
//!         \param [in]        pReservedResource  Pointer of reserved resource to get resource desc from.
//!         \param [out]       pDesc              Pointer to a resource description.
//! SUPPORTED OS:  Windows 10
//!
//!
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D12_ReservedResourceGetDesc(__in  ID3D12Resource      *pReservedResource,
                                                    __out D3D12_RESOURCE_DESC *pDesc);

#endif //defined(__cplusplus) && defined(__d3d12_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_UpdateTileMappings
//
//! \since Release: 375
//
//! \code
//!   DESCRIPTION: NvAPI_D3D12_UpdateTileMappings is an extension of ID3D12CommandQueue::UpdateTileMappings.
//!                pTiledResource must be created by NvAPI_D3D12_CreateReservedResource.
//!                pTilePool must be created by NvAPI_D3D12_CreateHeap.
//!
//!         \param [in]        pCommandQueue                        A pointer to ID3D12CommandQueue.
//!         \param [in]        pTiledResource                       A pointer to the tiled resource created by NvAPI_D3D12_CreateReservedResource.
//!         \param [in]        NumTiledResourceRegions              The number of tiled resource regions.
//!         \param [in]        pTiledResourceRegionStartCoordinates An array of D3D12_TILED_RESOURCE_COORDINATE structures that describe the starting coordinates of the tiled resource regions. Cannot be NULL.
//!         \param [in]        pTiledResourceRegionSizes            An array of D3D12_TILE_REGION_SIZE structures that describe the sizes of the tiled resource regions. Cannot be NULL.
//!         \param [in]        pTilePool                            A pointer to the resource heap created by NvAPI_D3D12_CreateHeap.
//!         \param [in]        NumRanges                            The number of tile-pool ranges.
//!         \param [in]        pRangeFlags                          A pointer to an array of D3D12_TILE_RANGE_FLAGS values that describes each tile range. 
//!         \param [in]        pTilePoolStartOffsets                An array of offsets into the tile pool. These are 0-based tile offsets, counting in tiles (not bytes).
//!         \param [in]        pRangeTileCounts                     An array of values that specify the number of tiles in each tile-pool range.
//!         \param [in]        Flags                                A combination of D3D12_TILE_MAPPING_FLAGS values that are combined by using a bitwise OR operation.
//! SUPPORTED OS:  Windows 10
//!
//!
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D12_UpdateTileMappings(
    __in       ID3D12CommandQueue              *pCommandQueue,
    __in       ID3D12Resource                  *pResource,
    __in       UINT                             NumResourceRegions,
    __in const D3D12_TILED_RESOURCE_COORDINATE *pResourceRegionStartCoordinates,
    __in const D3D12_TILE_REGION_SIZE          *pResourceRegionSizes,
    __in       ID3D12Heap                      *pHeap,
    __in       UINT                             NumRanges,
    __in const D3D12_TILE_RANGE_FLAGS          *pRangeFlags,
    __in const UINT                            *pHeapRangeStartOffsets,
    __in const UINT                            *pRangeTileCounts,
    __in       D3D12_TILE_MAPPING_FLAGS         Flags);

#endif //defined(__cplusplus) && defined(__d3d12_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_CopyTileMappings
//
//! \since Release: 375
//
//! \code
//!   DESCRIPTION: NvAPI_D3D12_CopyTileMappings is an extension of ID3D12CommandQueue::CopyTileMappings
//!                pDstResource and pSrcResource must be created by NvAPI_D3D12_CreateReservedResource.
//!
//!         \param [in]        pCommandQueue                        A pointer to ID3D12CommandQueue.
//!         \param [in]        pDstResource                         Tiled resource created by NvAPI_D3D12_CreateReservedResource to copy tile mappings into.
//!         \param [in]        pDstRegionStartCoordinate            A pointer to a D3D12_TILED_RESOURCE_COORDINATE structure that describes the starting coordinates of the destination reserved resource.
//!         \param [in]        pSrcResource                         Tiled resource created by NvAPI_D3D12_CreateReservedResource to copy tile mappings from.
//!         \param [in]        pSourceRegionStartCoordinate         A pointer to a D3D12_TILED_RESOURCE_COORDINATE structure that describes the starting coordinates of the source reserved resource.
//!         \param [in]        pTileRegionSize                      A pointer to a D3D12_TILE_REGION_SIZE structure that describes the size of the reserved region.
//!         \param [in]        Flags                                One member of D3D12_TILE_MAPPING_FLAGS.
//! SUPPORTED OS:  Windows 10
//!
//!
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D12_CopyTileMappings(
    __in       ID3D12CommandQueue              *pCommandQueue,
    __in       ID3D12Resource                  *pDstResource,
    __in const D3D12_TILED_RESOURCE_COORDINATE *pDstRegionStartCoordinate,
    __in       ID3D12Resource                  *pSrcResource,
    __in const D3D12_TILED_RESOURCE_COORDINATE *pSrcRegionStartCoordinate,
    __in const D3D12_TILE_REGION_SIZE          *pRegionSize,
    __in       D3D12_TILE_MAPPING_FLAGS         Flags);

#endif //defined(__cplusplus) && defined(__d3d12_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_ResourceAliasingBarrier
//
//! \since Release: 375
//
//! \code
//!   DESCRIPTION: NvAPI_D3D12_ResourceAliasingBarrier is an extension of ID3D12GraphicsCommandList::ResourceBarrier, but only for D3D12_RESOURCE_ALIASING_BARRIER.
//!                Both resource in pBarriers must be created by NvAPI_D3D12_CreateReservedTexture2DArray.
//!
//!         \param [in]        pCommandList                            A pointer to ID3D12GraphicsCommandList.
//!         \param [in]        NumBarriers                             The number of submitted barrier descriptions.
//!         \param [in]        pBarriers                               Pointer to an array of barrier descriptions with D3D12_RESOURCE_ALIASING_BARRIER type only.
//! SUPPORTED OS:  Windows 10
//!
//!
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D12_ResourceAliasingBarrier(
    __in       ID3D12GraphicsCommandList *pCommandList,
    __in       UINT                       NumBarriers,
    __in const D3D12_RESOURCE_BARRIER    *pBarriers);

#endif //defined(__cplusplus) && defined(__d3d12_h__)



#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))
///////////////////////////////////////////////////////////////////////////////
//
// MetaCommands common defines
//
//! \since Release: 400
//
///////////////////////////////////////////////////////////////////////////////
#pragma pack(push, 4)

enum NV_D3D_GRAPHICS_STATES
{
    NV_D3D_GRAPHICS_STATE_NONE                    =        0 ,
    NV_D3D_GRAPHICS_STATE_IA_VERTEX_BUFFERS       = (1 <<  0),
    NV_D3D_GRAPHICS_STATE_IA_INDEX_BUFFER         = (1 <<  1),
    NV_D3D_GRAPHICS_STATE_IA_PRIMITIVE_TOPOLOGY   = (1 <<  2),
    NV_D3D_GRAPHICS_STATE_DESCRIPTOR_HEAP         = (1 <<  3),
    NV_D3D_GRAPHICS_STATE_GRAPHICS_ROOT_SIGNATURE = (1 <<  4),
    NV_D3D_GRAPHICS_STATE_COMPUTE_ROOT_SIGNATURE  = (1 <<  5),
    NV_D3D_GRAPHICS_STATE_RS_VIEWPORTS            = (1 <<  6),
    NV_D3D_GRAPHICS_STATE_RS_SCISSOR_RECTS        = (1 <<  7),
    NV_D3D_GRAPHICS_STATE_PREDICATION             = (1 <<  8),
    NV_D3D_GRAPHICS_STATE_OM_RENDER_TARGETS       = (1 <<  9),
    NV_D3D_GRAPHICS_STATE_OM_STENCIL_REF          = (1 << 10),
    NV_D3D_GRAPHICS_STATE_OM_BLEND_FACTOR         = (1 << 11),
    NV_D3D_GRAPHICS_STATE_PIPELINE_STATE          = (1 << 12),
    NV_D3D_GRAPHICS_STATE_SO_TARGETS              = (1 << 13),
    NV_D3D_GRAPHICS_STATE_OM_DEPTH_BOUNDS         = (1 << 14),
    NV_D3D_GRAPHICS_STATE_SAMPLE_POSITIONS        = (1 << 15),
    NV_D3D_GRAPHICS_STATE_VIEW_INSTANCE_MASK      = (1 << 16),
};

struct NVAPI_META_COMMAND_DESC
{
    GUID Id;
    LPCWSTR Name;

    // states that the initialization and execution of the metacommand will dirty
    NV_D3D_GRAPHICS_STATES InitializationDirtyState;
    NV_D3D_GRAPHICS_STATES ExecutionDirtyState;
};

typedef NvU64 NV_META_COMMAND_BOOL;

#define NV_META_COMMAND_MAX_TENSOR_DIM 5
// dimensions (Size and Stride) are always indexed this way, irrespective of NV_META_COMMAND_LAYOUT
// For DimensionCount = 5: N, C, D, H, W
// For DimensionCount = 4: N, C, H, W


#define NV_META_COMMAND_ACTIVATION_MAX_PARAMS 2

#if __cplusplus >= 201103L

#define compile_time_assert(b)  static_assert((b), "Compile time assertion failed: "#b)

enum NV_META_COMMAND_TENSOR_DATA_TYPE : NvU64
{
    NV_META_COMMAND_TENSOR_DATA_TYPE_FLOAT32,
    NV_META_COMMAND_TENSOR_DATA_TYPE_FLOAT16,
    NV_META_COMMAND_TENSOR_DATA_TYPE_UINT32,

    NV_META_COMMAND_TENSOR_DATA_TYPE_COUNT,
};


enum NV_META_COMMAND_TENSOR_LAYOUT : NvU64
{
    NV_META_COMMAND_TENSOR_LAYOUT_UNKNOWN,             // opaque HW-native layout
    NV_META_COMMAND_TENSOR_LAYOUT_STANDARD,            // NCDHW - planar / row major layout (width is inner-most dimension, batch-size N is the outermost)
    NV_META_COMMAND_TENSOR_LAYOUT_COUNT,
};

enum NV_META_COMMAND_TENSOR_FLAGS : NvU64
{
    NV_META_COMMAND_TENSOR_FLAG_NONE = 0,
    NV_META_COMMAND_TENSOR_FLAG_DATA_STATIC = 0x1,    // data pointed by the tensor is static (i.e, won't be modified after command list recording)
};

enum NV_META_COMMAND_PRECISION : NvU64
{
    NV_META_COMMAND_PRECISION_FLOAT32,
    NV_META_COMMAND_PRECISION_FLOAT16,
    NV_META_COMMAND_PRECISION_MUL_FLOAT16_ADD_FLOAT32,

    NV_META_COMMAND_PRECISION_COUNT,
};

struct NV_META_COMMAND_TENSOR_DESC
{
    NV_META_COMMAND_TENSOR_DATA_TYPE DataType;
    NV_META_COMMAND_TENSOR_LAYOUT Layout;
    NV_META_COMMAND_TENSOR_FLAGS Flags;
    NvU64 DimensionCount;       // 4 or 5
    NvU64 Size[NV_META_COMMAND_MAX_TENSOR_DIM];
    NvU64 Stride[NV_META_COMMAND_MAX_TENSOR_DIM];    // only used with NV_META_COMMAND_TENSOR_LAYOUT_STANDARD 
};

enum NV_META_COMMAND_ACTIVATION_FUNCTION : NvU64
{
    NV_META_COMMAND_ACTIVATION_FUNCTION_ELU,
    NV_META_COMMAND_ACTIVATION_FUNCTION_HARDMAX,
    NV_META_COMMAND_ACTIVATION_FUNCTION_HARD_SIGMOID,
    NV_META_COMMAND_ACTIVATION_FUNCTION_IDENTITY,
    NV_META_COMMAND_ACTIVATION_FUNCTION_LEAKY_RELU,
    NV_META_COMMAND_ACTIVATION_FUNCTION_LINEAR,
    NV_META_COMMAND_ACTIVATION_FUNCTION_LOG_SOFTMAX,
    NV_META_COMMAND_ACTIVATION_FUNCTION_PARAMETERIZED_RELU,
    NV_META_COMMAND_ACTIVATION_FUNCTION_PARAMETRIC_SOFTPLUS,
    NV_META_COMMAND_ACTIVATION_FUNCTION_RELU,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SCALED_ELU,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SCALED_TANH,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SIGMOID,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SOFTMAX,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SOFTPLUS,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SOFTSIGN,
    NV_META_COMMAND_ACTIVATION_FUNCTION_TANH,
    NV_META_COMMAND_ACTIVATION_FUNCTION_THRESHOLDED_RELU,

    NV_META_COMMAND_ACTIVATION_FUNCTION_COUNT,
};

struct NV_META_COMMAND_ACTIVATION_DESC
{
    NV_META_COMMAND_ACTIVATION_FUNCTION Function;
    float Params[NV_META_COMMAND_ACTIVATION_MAX_PARAMS];
};

#else

#define compile_time_assert(b)  typedef char compile_time_assertion_failed_in_line_##__LINE__[(b)?1:-1]

enum NV_META_COMMAND_TENSOR_DATA_TYPE
{
    NV_META_COMMAND_TENSOR_DATA_TYPE_FLOAT32,
    NV_META_COMMAND_TENSOR_DATA_TYPE_FLOAT16,
    NV_META_COMMAND_TENSOR_DATA_TYPE_UINT32,

    NV_META_COMMAND_TENSOR_DATA_TYPE_COUNT,
};


enum NV_META_COMMAND_TENSOR_LAYOUT
{
    NV_META_COMMAND_TENSOR_LAYOUT_UNKNOWN,             // opaque HW-native layout
    NV_META_COMMAND_TENSOR_LAYOUT_STANDARD,            // NCDHW - planar / row major layout (width is inner-most dimension, batch-size N is the outermost)
    NV_META_COMMAND_TENSOR_LAYOUT_COUNT,
};

enum NV_META_COMMAND_TENSOR_FLAGS
{
    NV_META_COMMAND_TENSOR_FLAG_NONE = 0,
    NV_META_COMMAND_TENSOR_FLAG_DATA_STATIC = 0x1,    // data pointed by the tensor is static (i.e, won't be modified after command list recording)
};

enum NV_META_COMMAND_PRECISION
{
    NV_META_COMMAND_PRECISION_FLOAT32,
    NV_META_COMMAND_PRECISION_FLOAT16,
    NV_META_COMMAND_PRECISION_MUL_FLOAT16_ADD_FLOAT32,

    NV_META_COMMAND_PRECISION_COUNT,
};

struct NV_META_COMMAND_TENSOR_DESC
{
    NvU64 DataType;             // NV_META_COMMAND_TENSOR_DATA_TYPE
    NvU64 Layout;               // NV_META_COMMAND_TENSOR_LAYOUT
    NvU64 Flags;                // NV_META_COMMAND_TENSOR_FLAGS
    NvU64 DimensionCount;       // 4 or 5
    NvU64 Size[NV_META_COMMAND_MAX_TENSOR_DIM];
    NvU64 Stride[NV_META_COMMAND_MAX_TENSOR_DIM];    // only used with NV_META_COMMAND_TENSOR_LAYOUT_STANDARD 
};

enum NV_META_COMMAND_ACTIVATION_FUNCTION
{
    NV_META_COMMAND_ACTIVATION_FUNCTION_ELU,
    NV_META_COMMAND_ACTIVATION_FUNCTION_HARDMAX,
    NV_META_COMMAND_ACTIVATION_FUNCTION_HARD_SIGMOID,
    NV_META_COMMAND_ACTIVATION_FUNCTION_IDENTITY,
    NV_META_COMMAND_ACTIVATION_FUNCTION_LEAKY_RELU,
    NV_META_COMMAND_ACTIVATION_FUNCTION_LINEAR,
    NV_META_COMMAND_ACTIVATION_FUNCTION_LOG_SOFTMAX,
    NV_META_COMMAND_ACTIVATION_FUNCTION_PARAMETERIZED_RELU,
    NV_META_COMMAND_ACTIVATION_FUNCTION_PARAMETRIC_SOFTPLUS,
    NV_META_COMMAND_ACTIVATION_FUNCTION_RELU,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SCALED_ELU,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SCALED_TANH,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SIGMOID,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SOFTMAX,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SOFTPLUS,
    NV_META_COMMAND_ACTIVATION_FUNCTION_SOFTSIGN,
    NV_META_COMMAND_ACTIVATION_FUNCTION_TANH,
    NV_META_COMMAND_ACTIVATION_FUNCTION_THRESHOLDED_RELU,

    NV_META_COMMAND_ACTIVATION_FUNCTION_COUNT,
};

struct NV_META_COMMAND_ACTIVATION_DESC
{
    NvU64 Function;     // NV_META_COMMAND_ACTIVATION_FUNCTION
    float Params[NV_META_COMMAND_ACTIVATION_MAX_PARAMS];
};

#endif

struct NV_META_COMMAND_OPTIONAL_TENSOR_DESC : NV_META_COMMAND_TENSOR_DESC
{
    // true when the tensor isn't needed (e.g, bias is optional)
    NV_META_COMMAND_BOOL IsNull;
};


struct NV_META_COMMAND_OPTIONAL_ACTIVATION_DESC : NV_META_COMMAND_ACTIVATION_DESC
{
    // true when activation isn't needed
    NV_META_COMMAND_BOOL IsNull;
};


enum NV_META_COMMAND_PADDING_MODE
{
    NV_META_COMMAND_PADDING_ZEROS,
    NV_META_COMMAND_PADDING_MIRROR,
    NV_META_COMMAND_PADDING_CLAMP,
    NV_META_COMMAND_PADDING_CONSTANT,

    NV_META_COMMAND_PADDING_COUNT,
};

struct NV_META_COMMAND_PADDING_DESC
{
    NV_META_COMMAND_PADDING_MODE Mode;

    // used with NV_META_COMMAND_PADDING_CONSTANT
    float ConstantPadVal;
};

// use this enum to query resource sizes using GetRequiredParameterResourceSize() call
enum NV_META_COMMAND_RESOURCE_TYPE
{
    NV_META_COMMAND_RESOURCE_TYPE_INPUT = 0,
    NV_META_COMMAND_RESOURCE_TYPE_OUTPUT = 1,
    
    NV_META_COMMAND_RESOURCE_TYPE_FILTER = 2,
    NV_META_COMMAND_RESOURCE_TYPE_WEIGHT = 2,
    NV_META_COMMAND_RESOURCE_TYPE_BIAS = 3,
    
    NV_META_COMMAND_RESOURCE_TYPE_MATRIX_A = 0,
    NV_META_COMMAND_RESOURCE_TYPE_MATRIX_B = 2,
    NV_META_COMMAND_RESOURCE_TYPE_MATRIX_C = 3,
    
    NV_META_COMMAND_RESOURCE_TYPE_PERSISTENT = 4,
    NV_META_COMMAND_RESOURCE_TYPE_TEMPORARY = 5,
};


// Extended version of convolution operation that performs:
//
// y = act ( alpha1 * conv(x) + alpha2 * z + bias )
//
// alpha1 and alpha2 are either scalars or if PerChannelScaling is TRUE, they are vectors of
// same dimension as the bias tensor (vector of size equal to number of output channels)
//
// z (SkipConnectionResource) has same dimension as output tensor y (OutputResource).

static const GUID MetaCommand_ConvolutionEx = 
{ 0xa7666f1e, 0x9c55, 0x47ee, { 0x9e, 0xb3, 0xe1, 0x62, 0x0, 0x92, 0xd1, 0xe9 } };

#define NV_META_COMMAND_NUM_SPATIAL_DIM 3
// D, H, W when DimensionCount is 3
// H, W when DimensionCount is 2


#if __cplusplus >= 201103L
enum NV_META_COMMAND_CONVOLUTION_DIRECTION : NvU64
{
    NV_META_COMMAND_CONVOLUTION_DIRECTION_FORWARD,  // Corresponds to regular Convolution
    NV_META_COMMAND_CONVOLUTION_DIRECTION_BACKWARD, // Corresponds to ConvolutionTranspose
    
    NV_META_COMMAND_CONVOLUTION_DIRECTION_COUNT,
};

enum NV_META_COMMAND_CONVOLUTION_MODE : NvU64
{
    NV_META_COMMAND_CONVOLUTION_MODE_CONVOLUTION,
    NV_META_COMMAND_CONVOLUTION_MODE_CROSS_CORRELATION,
    
    NV_META_COMMAND_CONVOLUTION_MODE_COUNT,
};

struct NV_META_COMMAND_CREATE_CONVOLUTION_EX_DESC
{
    // Descriptor of the input tensor
    NV_META_COMMAND_TENSOR_DESC DescIn;
    
    // Descriptor of the tensor acting as the filter kernel
    NV_META_COMMAND_TENSOR_DESC DescFilter;
    
    // Descriptor of the optional bias tensor
    NV_META_COMMAND_OPTIONAL_TENSOR_DESC DescBias;
    
    // Descriptor of the output tensor
    NV_META_COMMAND_TENSOR_DESC DescOut;
    
    // Convolution mode (CROSS_CORRELATION or CONVOLUTION)
    NV_META_COMMAND_CONVOLUTION_MODE Mode;
    
    // Convolution direction (FORWARD or BACKWARD)
    NV_META_COMMAND_CONVOLUTION_DIRECTION Direction;
    
    // Precision at which convolution is done
    NV_META_COMMAND_PRECISION Precision;
    
    // Optional activation function
    NV_META_COMMAND_OPTIONAL_ACTIVATION_DESC Activation;
    
    // Padding mode (only used when output tensor dimensions are different from input tensor dimensions)
    NV_META_COMMAND_PADDING_DESC Padding;

    // enables per channel scaling i.e, use Alpha1Resource and Alpha2Resource 
    // instead of Alpha1 and Alpha2 below
    NV_META_COMMAND_BOOL PerChannelScaling;

    // scaling factors used when PerChannelScaling is FALSE
    // set Alpha1 = 1.0f, Alpha2 = 0.0f for simple convolutions 
    // that don't need scaling or skip connection
    float Alpha1;
    float Alpha2;

    // Strides for the filter kernel position
    NvU64 Stride[NV_META_COMMAND_NUM_SPATIAL_DIM];
    
    // The distance per dimension between elements that are multiplied
    NvU64 Dilation[NV_META_COMMAND_NUM_SPATIAL_DIM];
    
    // Padding at the start of each dimension
    NvU64 StartPadding[NV_META_COMMAND_NUM_SPATIAL_DIM];
    
    // Padding at the end of each dimension
    NvU64 EndPadding[NV_META_COMMAND_NUM_SPATIAL_DIM];
    
    // Number of dimensions to which convolution occurs (2 or 3)
    NvU64 DimensionCount;

    // Number of channel groups convolved independently
    NvU64 GroupCount;
};

#else

enum NV_META_COMMAND_CONVOLUTION_DIRECTION
{
    NV_META_COMMAND_CONVOLUTION_DIRECTION_FORWARD,  // Corresponds to regular Convolution
    NV_META_COMMAND_CONVOLUTION_DIRECTION_BACKWARD, // Corresponds to ConvolutionTranspose
    
    NV_META_COMMAND_CONVOLUTION_DIRECTION_COUNT,
};

enum NV_META_COMMAND_CONVOLUTION_MODE
{
    NV_META_COMMAND_CONVOLUTION_MODE_CONVOLUTION,
    NV_META_COMMAND_CONVOLUTION_MODE_CROSS_CORRELATION,
    
    NV_META_COMMAND_CONVOLUTION_MODE_COUNT,
};

struct NV_META_COMMAND_CREATE_CONVOLUTION_EX_DESC
{
    // Descriptor of the input tensor
    NV_META_COMMAND_TENSOR_DESC DescIn;
    
    // Descriptor of the tensor acting as the filter kernel
    NV_META_COMMAND_TENSOR_DESC DescFilter;
    
    // Descriptor of the optional bias tensor
    NV_META_COMMAND_OPTIONAL_TENSOR_DESC DescBias;
    
    // Descriptor of the output tensor
    NV_META_COMMAND_TENSOR_DESC DescOut;
    
    // Convolution mode (CROSS_CORRELATION or CONVOLUTION)
    NvU64 Mode;     // NV_META_COMMAND_CONVOLUTION_MODE
    
    // Convolution direction (FORWARD or BACKWARD)
    NvU64 Direction;    // NV_META_COMMAND_CONVOLUTION_DIRECTION
    
    // Precision at which convolution is done
    NvU64 Precision;    // NV_META_COMMAND_PRECISION
    
    // Optional activation function
    NV_META_COMMAND_OPTIONAL_ACTIVATION_DESC Activation;
    
    // Padding mode (only used when output tensor dimensions are different from input tensor dimensions)
    NV_META_COMMAND_PADDING_DESC Padding;

    // enables per channel scaling i.e, use Alpha1Resource and Alpha2Resource 
    // instead of Alpha1 and Alpha2 below
    NV_META_COMMAND_BOOL PerChannelScaling;

    // scaling factors used when PerChannelScaling is FALSE
    // set Alpha1 = 1.0f, Alpha2 = 0.0f for simple convolutions 
    // that don't need scaling or skip connection
    float Alpha1;
    float Alpha2;

    // Strides for the filter kernel position
    NvU64 Stride[NV_META_COMMAND_NUM_SPATIAL_DIM];
    
    // The distance per dimension between elements that are multiplied
    NvU64 Dilation[NV_META_COMMAND_NUM_SPATIAL_DIM];
    
    // Padding at the start of each dimension
    NvU64 StartPadding[NV_META_COMMAND_NUM_SPATIAL_DIM];
    
    // Padding at the end of each dimension
    NvU64 EndPadding[NV_META_COMMAND_NUM_SPATIAL_DIM];
    
    // Number of dimensions to which convolution occurs (2 or 3)
    NvU64 DimensionCount;

    // Number of channel groups convolved independently
    NvU64 GroupCount;
};
#endif


// Fused Convolution variants

// supported combinations right now are: 
//  - Convolution + Max Pooling (also optionally outputs pre-pool data)
//  - 2x2 upsample + (optional) residual add + Convolution
//
// other combinations may be exposed in future

static const GUID MetaCommand_ConvolutionExFused = 
{ 0xe1b112eb, 0xdecd, 0x4ff6,{ 0x85, 0xbb, 0x1f, 0xe, 0x3a, 0xb0, 0x4, 0x14 } };


enum NV_META_COMMAND_CONVOLUTION_POOL_MODE
{
    NV_META_COMMAND_CONVOLUTION_POOL_MODE_NONE,
    NV_META_COMMAND_CONVOLUTION_POOL_MODE_REDUCTION_MAX,
    NV_META_COMMAND_CONVOLUTION_POOL_MODE_REDUCTION_AVG,
    NV_META_COMMAND_CONVOLUTION_POOL_MODE_REDUCTION_MIN,

    NV_META_COMMAND_CONVOLUTION_POOL_MODE_COUNT,
};

enum NV_META_COMMAND_CONVOLUTION_UPSAMPLE_MODE
{
    NV_META_COMMAND_CONVOLUTION_UPSAMPLE_MODE_NONE,
    NV_META_COMMAND_CONVOLUTION_UPSAMPLE_MODE_REPLICATE,
    NV_META_COMMAND_CONVOLUTION_UPSAMPLE_MODE_BILINEAR,

    NV_META_COMMAND_CONVOLUTION_UPSAMPLE_MODE_COUNT,
};

enum NV_META_COMMAND_CONVOLUTION_SKIP_MODE
{
    NV_META_COMMAND_CONVOLUTION_SKIP_MODE_NONE,
    NV_META_COMMAND_CONVOLUTION_SKIP_MODE_ADD,
    NV_META_COMMAND_CONVOLUTION_SKIP_MODE_CONCAT,

    NV_META_COMMAND_CONVOLUTION_SKIP_MODE_COUNT,
};

struct NV_META_COMMAND_CONVOLUTION_FUSE_DESC
{
    NV_META_COMMAND_CONVOLUTION_POOL_MODE PoolMode;
    NV_META_COMMAND_CONVOLUTION_UPSAMPLE_MODE UpsampleMode;
    NV_META_COMMAND_CONVOLUTION_SKIP_MODE SkipMode;

    NV_META_COMMAND_BOOL OutputPrepool;     // used with NV_META_COMMAND_CONVOLUTION_POOL_MODE
};

// uses same structures for init and execute descriptors
// SkipConnectionResource is used to specify the resource for pre-pool data or residual add
struct NV_META_COMMAND_CREATE_CONVOLUTION_EX_FUSED_DESC : NV_META_COMMAND_CREATE_CONVOLUTION_EX_DESC
{
    NV_META_COMMAND_CONVOLUTION_FUSE_DESC FuseDesc;
};

// make sure structure sizes match what the driver assumes
compile_time_assert(sizeof(NV_META_COMMAND_TENSOR_DESC) == 112);
compile_time_assert(sizeof(NV_META_COMMAND_CREATE_CONVOLUTION_EX_DESC) == 640);
compile_time_assert(sizeof(NV_META_COMMAND_CONVOLUTION_FUSE_DESC) == 20);
compile_time_assert(sizeof(NV_META_COMMAND_CREATE_CONVOLUTION_EX_FUSED_DESC) == 660);


// GEMM (General matrix multiply)
//
// Y = alpha * t(A) * t(B) + beta * C,
//
// where t is a matrix transform option
// 
// If C is null, and beta is non-zero, the output 
// matrix is used as C matrix. i.e, the operation performed is:
// Y = alpha * t(A) * t(B) + beta * Y
// 
static const GUID MetaCommand_Gemm = 
    { 0x8f9ff059, 0xfe72, 0x488e, { 0xa0, 0x66, 0xb1, 0x4e, 0x79, 0x48, 0xec, 0x8 } };

#if __cplusplus >= 201103L

enum NV_META_COMMAND_MATRIX_TRANSFORM : NvU64
{
    NV_META_COMMAND_MATRIX_TRANSFORM_NONE,
    NV_META_COMMAND_MATRIX_TRANSFORM_TRANSPOSE,

    NV_META_COMMAND_MATRIX_TRANSFORM_COUNT,
};

struct NV_META_COMMAND_CREATE_GEMM_DESC
{
    NV_META_COMMAND_TENSOR_DESC DescA;
    NV_META_COMMAND_TENSOR_DESC DescB;
    NV_META_COMMAND_OPTIONAL_TENSOR_DESC DescC;
    NV_META_COMMAND_TENSOR_DESC DescOut;
    NV_META_COMMAND_PRECISION Precision;

    NV_META_COMMAND_MATRIX_TRANSFORM TransA;
    NV_META_COMMAND_MATRIX_TRANSFORM TransB;
    float Alpha;
    float Beta;

    NV_META_COMMAND_OPTIONAL_ACTIVATION_DESC Activation;
};

#else

enum NV_META_COMMAND_MATRIX_TRANSFORM
{
    NV_META_COMMAND_MATRIX_TRANSFORM_NONE,
    NV_META_COMMAND_MATRIX_TRANSFORM_TRANSPOSE,

    NV_META_COMMAND_MATRIX_TRANSFORM_COUNT,
};

struct NV_META_COMMAND_CREATE_GEMM_DESC
{
    NV_META_COMMAND_TENSOR_DESC DescA;
    NV_META_COMMAND_TENSOR_DESC DescB;
    NV_META_COMMAND_OPTIONAL_TENSOR_DESC DescC;
    NV_META_COMMAND_TENSOR_DESC DescOut;
    NvU64 Precision;    // NV_META_COMMAND_PRECISION

    NvU64 TransA;       // NV_META_COMMAND_MATRIX_TRANSFORM
    NvU64 TransB;       // NV_META_COMMAND_MATRIX_TRANSFORM
    float Alpha;
    float Beta;

    NV_META_COMMAND_OPTIONAL_ACTIVATION_DESC Activation;
};

#endif


#pragma pack(pop)

#endif // #if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))


#if defined (__cplusplus) && defined(__d3d11_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_EnumerateMetaCommands
//
//! \since Release: 400
//
//! \code
//!   DESCRIPTION: Enumerates MetaCommands supported on the system
//!
//!         \param [in]        pDevice                A pointer to D3D11 device.
//!         \param [in/out]    pNumMetaCommands       Should be non-null. When the value pointed by pNumMetaCommands is 0 (or when pDescs is NULL), the function returns number of metacommands supported.
//!                                                   When the value pointed is non-zero, the value indicates number of Metacommand descriptions to be populated in pDescs array.
//!         \param [out]       pDescs                 Pointer to array where Metacommand descriptions will be returned. Can be null to indicate that the app is querying the number of supported metacommands. 
//!                                                   Otherwise should have enough space to hold *pNumMetaCommands descriptors
//! SUPPORTED OS:  Windows 10
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_EnumerateMetaCommands(__in                                  ID3D11Device                    *pDevice,
                                                  __inout                               NvU32                           *pNumMetaCommands,
                                                  __out_ecount_opt(*pNumMetaCommands)   NVAPI_META_COMMAND_DESC         *pDescs);

#endif //defined(__cplusplus) && defined(__d3d11_h__)



#if defined (__cplusplus) && defined(__d3d11_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateMetaCommand
//
//! \since Release: 400
//
//! \code
//!   DESCRIPTION: Creates a MetaCommand object which can be used to execute optimized operations exposed by driver like convolutions.
//!
//!         \param [in]        pDevice                      A pointer to D3D11 device.
//!         \param [in]        CommandId                    GUID of the operations to perform
//!         \param [in]        pCreationParametersData      structure containing all creation parameters for the requested Metacommand
//!         \param [in]        CreationParametersDataSize   size of parameter data structure
//!         \param [out]       ppMetaCommand                A pointer to memory that receives the pointer to the created MetaCommand object. 
//! SUPPORTED OS:  Windows 10
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!         
//!          DXGI_ERROR_NOT_SUPPORTED  - The requested Metacommand is not supported.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 4)
struct NV_D3D11_META_COMMAND_RESOURCE
{
    union
    {
         NVDX_ObjectHandle ResourceHandle;    // NVAPI handle of a buffer resource (use NvAPI_D3D11_GetResourceHandle to get this handle)
         NvU64             unused;            // to get correct sturcutre size on 32 bit builds
    };
    NvU64 Offset;                             // offset within the resource in bytes
};

struct NV_D3D11_META_COMMAND_INITIALIZE_CONVOLUTION_EX_DESC
{
    // Persistent resource used as scratch space by driver
    // it's written at time of init, and read at time of execute
    // use GetRequiredParameterResourceSize to query its size
    NV_D3D11_META_COMMAND_RESOURCE PersistentResource;
};

struct NV_D3D11_META_COMMAND_EXECUTE_CONVOLUTION_EX_DESC
{
    NV_D3D11_META_COMMAND_RESOURCE InputResource;
    NV_D3D11_META_COMMAND_RESOURCE FilterResource;
    NV_D3D11_META_COMMAND_RESOURCE BiasResource;            // optional
    NV_D3D11_META_COMMAND_RESOURCE OutputResource;

    // Alpha1Resource and Alpha2Resource are used only when 
    // PerChannelScaling is set. Otherwise the scalars Alpha1/Alpha2 are used
    // should have same dimension as bias
    NV_D3D11_META_COMMAND_RESOURCE Alpha1Resource;
    NV_D3D11_META_COMMAND_RESOURCE Alpha2Resource;

    // optional, same dimension/descriptor as output
    NV_D3D11_META_COMMAND_RESOURCE SkipConnectionResource;


    // should point to same memory that was specified at time of init
    NV_D3D11_META_COMMAND_RESOURCE PersistentResource;
    
    // temporary resource used as scratch space by driver
    // used for both read and write at the time of execute
    // use GetRequiredParameterResourceSize to query its size
    NV_D3D11_META_COMMAND_RESOURCE TemporaryResource;
};

// make sure structure sizes match what the driver assumes
compile_time_assert(sizeof(NV_D3D11_META_COMMAND_INITIALIZE_CONVOLUTION_EX_DESC) == 16);
compile_time_assert(sizeof(NV_D3D11_META_COMMAND_EXECUTE_CONVOLUTION_EX_DESC) == 144);

struct NV_D3D11_META_COMMAND_INITIALIZE_GEMM_DESC
{
    NV_D3D11_META_COMMAND_RESOURCE PersistentResource;
};

struct NV_D3D11_META_COMMAND_EXECUTE_GEMM_DESC
{
    NV_D3D11_META_COMMAND_RESOURCE AResource;
    NV_D3D11_META_COMMAND_RESOURCE BResource;
    NV_D3D11_META_COMMAND_RESOURCE CResource;
    NV_D3D11_META_COMMAND_RESOURCE OutputResource;

    NV_D3D11_META_COMMAND_RESOURCE PersistentResource;
    NV_D3D11_META_COMMAND_RESOURCE TemporaryResource;
};

// make sure structure sizes match what the driver assumes
compile_time_assert(sizeof(NV_D3D11_META_COMMAND_INITIALIZE_GEMM_DESC) == 16);
compile_time_assert(sizeof(NV_D3D11_META_COMMAND_EXECUTE_GEMM_DESC) == 96);


#pragma pack(pop)

DECLARE_INTERFACE_(__declspec(uuid("00BF193A-117B-42BC-BBCD-E964A0EA4F2B"))ID3D11NvMetaCommand_V1, IUnknown)
{
    BEGIN_INTERFACE

    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // ** ID3D11NvMetaCommand methods ***
    // Return size of parameter
    STDMETHOD(GetRequiredParameterResourceSize)(THIS_ NV_META_COMMAND_RESOURCE_TYPE ResourceType, NvU64 *SizeInBytes) const PURE;

    END_INTERFACE
};

typedef ID3D11NvMetaCommand_V1    ID3D11NvMetaCommand;
#define ID3D11NvMetaCommand_VER1  MAKE_NVAPI_VERSION(IID3D11NvMetaCommand_V1, 1)
#define ID3D11NvMetaCommand_VER   ID3D11NvMetaCommand_VER1

NVAPI_INTERFACE NvAPI_D3D11_CreateMetaCommand(__in                                     ID3D11Device            *pDevice,
                                              __in                                     REFGUID                  CommandId,
                                              __in_bcount(CreationParametersDataSize)  const void              *pCreationParametersData,
                                              __in                                     NvU32                    CreationParametersDataSize,
                                              __out                                    ID3D11NvMetaCommand    **ppMetaCommand);

#endif //defined(__cplusplus) && defined(__d3d11_h__)


#if defined (__cplusplus) && defined(__d3d11_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_InitializeMetaCommand
//
//! \since Release: 400
//
//! \code
//!   DESCRIPTION: Initializes the given MetaCommand with the parameters passed in
//!
//!         \param [in]        pDeviceContext                   A pointer to the d3d11 device context
//!         \param [in]        pMetaCommand                     the MetaCommand to initialize
//!         \param [in]        pInitializationParametersData    Structure containing parameters
//!         \param [in]        InitializationParametersDataSize Size of the parameter structure in bytes 
//! SUPPORTED OS:  Windows 10
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_InitializeMetaCommand(__in                                          ID3D11DeviceContext       *pDeviceContext,
                                                  __in                                          ID3D11NvMetaCommand       *pMetaCommand,
                                                  __in_bcount(InitializationParametersDataSize) const void                *pInitializationParametersData,
                                                  __in                                          NvU32                      InitializationParametersDataSize);

#endif //defined(__cplusplus) && defined(__d3d11_h__)


#if defined (__cplusplus) && defined(__d3d11_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_ExecuteMetaCommand
//
//! \since Release: 400
//
//! \code
//!   DESCRIPTION: Executes the given MetaCommand with the parameters passed in
//!
//!         \param [in]        pDeviceContext                   A pointer to the d3d11 device context
//!         \param [in]        pMetaCommand                     the MetaCommand to execute
//!         \param [in]        pExecutionParametersData         Structure containing parameters
//!         \param [in]        ExecutionParametersDataSize      Size of the parameter structure in bytes 
//! SUPPORTED OS:  Windows 10
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_ExecuteMetaCommand(__in                                          ID3D11DeviceContext       *pDeviceContext,
                                               __in                                          ID3D11NvMetaCommand       *pMetaCommand,
                                               __in_bcount(ExecutionParametersDataSize)      const void                *pExecutionParametersData,
                                               __in                                          NvU32                      ExecutionParametersDataSize);

#endif //defined(__cplusplus) && defined(__d3d11_h__)




#if defined (__cplusplus) && defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_EnumerateMetaCommands
//
//! \since Release: 400
//
//! \code
//!   DESCRIPTION: Enumerates MetaCommands supported on the system
//!
//!         \param [in]        pDevice                A pointer to D3D12 device.
//!         \param [in/out]    pNumMetaCommands       Should be non-null. When the value pointed by pNumMetaCommands is 0 or when pDescs is NULL, the function returns number of metacommands supported.
//!                                                   When the value pointed is non-zero, the value indicates number of Metacommand descriptions to be populated in pDescs array.
//!         \param [out]       pDescs                 Pointer to array where Metacommand descriptions will be returned. Can be null to indicate that the app is querying the number of supported metacommands. 
//!                                                   Otherwise should have enough space to hold *pNumMetaCommands descriptors
//! SUPPORTED OS:  Windows 10
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D12_EnumerateMetaCommands(__in                                  ID3D12Device                    *pDevice,
                                                  __inout                               NvU32                           *pNumMetaCommands,
                                                  __out_ecount_opt(*pNumMetaCommands)   NVAPI_META_COMMAND_DESC         *pDescs);

#endif //defined(__cplusplus) && defined(__d3d12_h__)



#if defined (__cplusplus) && defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_CreateMetaCommand
//
//! \since Release: 400
//
//! \code
//!   DESCRIPTION: Creates a MetaCommand object which can be used to execute optimized operations exposed by driver like convolutions.
//!
//!         \param [in]        pDevice                      A pointer to D3D12 device.
//!         \param [in]        CommandId                    GUID of the operations to perform
//!         \param [in]        NodeMask                     GPU mask for which metacommand is to be created. Set it to 0 for single GPU systems
//!         \param [in]        pCreationParametersData      structure containing all creation parameters for the requested Metacommand
//!         \param [in]        CreationParametersDataSize   size of parameter data structure
//!         \param [out]       ppMetaCommand                A pointer to memory that receives the pointer to the created MetaCommand object. 
//! SUPPORTED OS:  Windows 10
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!         
//!          DXGI_ERROR_NOT_SUPPORTED  - The requested Metacommand is not supported.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 4)
struct NV_D3D12_META_COMMAND_INITIALIZE_CONVOLUTION_EX_DESC
{
    // Persistent resource used as scratch space by driver
    // it's written at time of init, and read at time of execute
    // use GetRequiredParameterResourceSize to query its size
    D3D12_GPU_VIRTUAL_ADDRESS PersistentResource;
};

struct NV_D3D12_META_COMMAND_EXECUTE_CONVOLUTION_EX_DESC
{
    D3D12_GPU_VIRTUAL_ADDRESS InputResource;
    D3D12_GPU_VIRTUAL_ADDRESS FilterResource;
    D3D12_GPU_VIRTUAL_ADDRESS BiasResource;             // optional
    D3D12_GPU_VIRTUAL_ADDRESS OutputResource;

    // Alpha1Resource and Alpha2Resource are used only when 
    // PerChannelScaling is set. Otherwise the scalars Alpha1/Alpha2 are used
    // should have same dimension as bias
    D3D12_GPU_VIRTUAL_ADDRESS Alpha1Resource;
    D3D12_GPU_VIRTUAL_ADDRESS Alpha2Resource;

   // optional, same dimension/descriptor as output
    D3D12_GPU_VIRTUAL_ADDRESS SkipConnectionResource;

    // should point to same memory that was specified at time of init
    D3D12_GPU_VIRTUAL_ADDRESS PersistentResource;

    // temporary resource used as scratch space by driver
    // both written and read at time of execute
    // use GetRequiredParameterResourceSize to query its size
    D3D12_GPU_VIRTUAL_ADDRESS TemporaryResource;
};

// make sure structure sizes match what the driver assumes
compile_time_assert(sizeof(NV_D3D12_META_COMMAND_INITIALIZE_CONVOLUTION_EX_DESC) == 8);
compile_time_assert(sizeof(NV_D3D12_META_COMMAND_EXECUTE_CONVOLUTION_EX_DESC) == 72);

struct NV_D3D12_META_COMMAND_INITIALIZE_GEMM_DESC
{
    NvU64 PersistentResource;
};

struct NV_D3D12_META_COMMAND_EXECUTE_GEMM_DESC
{
    NvU64 AResource;
    NvU64 BResource;
    NvU64 CResource;
    NvU64 OutputResource;

    NvU64 PersistentResource;
    NvU64 TemporaryResource;
};

// make sure structure sizes match what the driver assumes
compile_time_assert(sizeof(NV_D3D12_META_COMMAND_INITIALIZE_GEMM_DESC) == 8);
compile_time_assert(sizeof(NV_D3D12_META_COMMAND_EXECUTE_GEMM_DESC) == 48);


#pragma pack(pop)

DECLARE_INTERFACE_(__declspec(uuid("00BF193A-117B-42BC-BBCD-E964A0EA4F2B"))ID3D12NvMetaCommand_V1, IUnknown)
{
    BEGIN_INTERFACE

    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // ** ID3D12NvMetaCommand methods ***
    // Return size of parameter
    STDMETHOD(GetRequiredParameterResourceSize)(THIS_ NV_META_COMMAND_RESOURCE_TYPE ResourceType, NvU64 *SizeInBytes) const PURE;

    END_INTERFACE
};

typedef ID3D12NvMetaCommand_V1    ID3D12NvMetaCommand;
#define ID3D12NvMetaCommand_VER1  MAKE_NVAPI_VERSION(IID3D12NvMetaCommand_V1, 1)
#define ID3D12NvMetaCommand_VER   ID3D12NvMetaCommand_VER1

NVAPI_INTERFACE NvAPI_D3D12_CreateMetaCommand(__in                                     ID3D12Device            *pDevice,
                                              __in                                     REFGUID                  CommandId,
                                              __in                                     NvU32                    NodeMask,
                                              __in_bcount(CreationParametersDataSize)  const void              *pCreationParametersData,
                                              __in                                     NvU32                    CreationParametersDataSize,
                                              __out                                    ID3D12NvMetaCommand    **ppMetaCommand);

#endif //defined(__cplusplus) && defined(__d3d12_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_InitializeMetaCommand
//
//! \since Release: 400
//
//! \code
//!   DESCRIPTION: Initializes the given MetaCommand with the parameters passed in
//!
//!         \param [in]        pCommandList                     A pointer to D3D12 command list.
//!         \param [in]        pMetaCommand                     the MetaCommand to initialize
//!         \param [in]        pInitializationParametersData    Structure containing parameters
//!         \param [in]        InitializationParametersDataSize Size of the parameter structure in bytes 
//! SUPPORTED OS:  Windows 10
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D12_InitializeMetaCommand(__in                                          ID3D12GraphicsCommandList *pCommandlist,
                                                  __in                                          ID3D12NvMetaCommand       *pMetaCommand,
                                                  __in_bcount(InitializationParametersDataSize) const void                *pInitializationParametersData,
                                                  __in                                          NvU32                      InitializationParametersDataSize);

#endif //defined(__cplusplus) && defined(__d3d12_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_ExecuteMetaCommand
//
//! \since Release: 400
//
//! \code
//!   DESCRIPTION: Executes the given MetaCommand with the parameters passed in
//!
//!         \param [in]        pCommandList                     A pointer to D3D12 command list.
//!         \param [in]        pMetaCommand                     the MetaCommand to execute
//!         \param [in]        pExecutionParametersData         Structure containing parameters
//!         \param [in]        ExecutionParametersDataSize      Size of the parameter structure in bytes 
//! SUPPORTED OS:  Windows 10
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D12_ExecuteMetaCommand(__in                                          ID3D12GraphicsCommandList *pCommandlist,
                                               __in                                          ID3D12NvMetaCommand       *pMetaCommand,
                                               __in_bcount(ExecutionParametersDataSize)      const void                *pExecutionParametersData,
                                               __in                                          NvU32                      ExecutionParametersDataSize);

#endif //defined(__cplusplus) && defined(__d3d12_h__)


#if defined (__cplusplus) && defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_IsNvShaderExtnOpCodeSupported
//
//!   DESCRIPTION: This function checks if a nv HLSL shader extension opcode is 
//!                supported on current hardware. List of opcodes is in nvShaderExtnEnums.h
//!                To use Nvidia HLSL extensions the application must include nvHLSLExtns.h 
//!                in the hlsl shader code. See nvHLSLExtns.h for more details on supported opcodes.
//!
//! \since Release: 364
//!
//! SUPPORTED OS:  Windows 10
//!
//!
//! \param [in]        pDev         The device on which to query for support
//! \param [in]        opCode       the opcode to check
//! \param [out]       pSupported   true if supported, false otherwise
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//! \retval ::         NVAPI_OK if the call succeeded
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D12_IsNvShaderExtnOpCodeSupported(__in  ID3D12Device *pDevice,
                                                          __in  NvU32 opCode,
                                                          __out bool *pSupported);

#endif //defined (__cplusplus) && defined(__d3d12_h__)


#if defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d11_h__)

///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_D3D_IsGSyncCapable
//!   DESCRIPTION: This API gets G-Sync capability for the given device context.
//!                This is only reliable after the first present call has completed.
//! \param [in]    pDeviceOrContext    The D3D9, D3D10, D3D11 device, or D3D11 device context
//! \param [in]    NVDX_ObjectHandle   The handle of primary surface
//! \param [out]   pIsGsyncCapable     if G-Sync can be enabled, *pIsGsyncCapable is true.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! RETURN STATUS: This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D_IsGSyncCapable(__in IUnknown *pDeviceOrContext, __in NVDX_ObjectHandle primarySurface, __out BOOL *pIsGsyncCapable);
#endif //if defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d11_h__)



#if defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d11_h__)

///////////////////////////////////////////////////////////////////////////////
//!
//! FUNCTION NAME: NvAPI_D3D_IsGSyncActive
//!   DESCRIPTION: This API get the G-Sync state for the given device context.
//!                This is only reliable after the first present call has completed.
//!                As it is a bit time consuming, It should not be called per frame. 
//! \param [in]    pDeviceOrContext    The D3D9, D3D10, D3D11 device, or D3D11 device context
//! \param [in]    NVDX_ObjectHandle   The handle of primary surface
//! \param [out]   pIsGsyncActive      if G-Sync is active, *pisGsyncActive is true.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! RETURN STATUS: This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D_IsGSyncActive(__in IUnknown *pDeviceOrContext, __in NVDX_ObjectHandle primarySurface, __out BOOL *pIsGsyncActive);
#endif //if defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d11_h__)



//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && ( defined(__d3d10_h__) || defined(__d3d10_1_h__) ||defined(__d3d11_h__) ) 
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D1x_DisableShaderDiskCache
//
//! DESCRIPTION: Disables driver managed caching of shader compilations to disk
//!
//! \param [in]    pDevice               Device to disabled the shader disk cache on
//!
//!
//! \retval ::NVAPI_OK                   Shader disk cache was disabled
//! \retval ::NVAPI_ERROR                The operation failed.
//! \retval ::NVAPI_INVALID_ARGUMENT     Argument passed in is invalid.
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D1x_DisableShaderDiskCache(IUnknown *pDevice);

#endif //defined(__cplusplus) && ( defined(__d3d10_h__) || defined(__d3d10_1_h__) ||defined(__d3d11_h__) )

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && defined(__d3d11_h__) 
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_MultiGPU_GetCaps
//
//! DESCRIPTION: Request to get multi GPU extension caps.
//!
//! \param [in/out]    pMultiGPUCaps     Pointer to a structure returning multi GPU caps
//! \retval ::NVAPI_OK                   Call succeeded.
//! \retval ::NVAPI_ERROR                Call failed.
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
//! \ingroup dx
typedef struct _NV_MULTIGPU_CAPS_V1
{
    NvU32 multiGPUVersion;
    NvU32 reserved;
    NvU32 nTotalGPUs;
    NvU32 nSLIGPUs;
    NvU32 videoBridgePresent;
} NV_MULTIGPU_CAPS_V1, *PNV_MULTIGPU_CAPS_V1;

#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && defined(__d3d11_h__) 

typedef struct _NV_MULTIGPU_CAPS_V2
{
    NvU32 multiGPUVersion;
    union 
    {
        NvU32 reserved;
        NvU32 version;          //!< The version of the structure
    };
    NvU32 nTotalGPUs;
    NvU32 nSLIGPUs;
    NvU32 videoBridgePresent;
    NvU32 NvLinkPresent;
    NvU32 fastNvLinkReads;
} NV_MULTIGPU_CAPS_V2, *PNV_MULTIGPU_CAPS_V2;

#define NV_MULTIGPU_CAPS_VER1   MAKE_NVAPI_VERSION(NV_MULTIGPU_CAPS_V1, 1)
#define NV_MULTIGPU_CAPS_VER2   MAKE_NVAPI_VERSION(NV_MULTIGPU_CAPS_V2, 2)

#define NV_MULTIGPU_CAPS_VER    NV_MULTIGPU_CAPS_VER2
typedef NV_MULTIGPU_CAPS_V2     NV_MULTIGPU_CAPS;
typedef PNV_MULTIGPU_CAPS_V2    PNV_MULTIGPU_CAPS;

#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && defined(__d3d11_h__) 
//! \ingroup dx

#ifndef NV_MULTIGPU_CAPS_VER
typedef NV_MULTIGPU_CAPS_V1     NV_MULTIGPU_CAPS;
typedef PNV_MULTIGPU_CAPS_V1    PNV_MULTIGPU_CAPS;
#endif

NVAPI_INTERFACE NvAPI_D3D11_MultiGPU_GetCaps(__inout PNV_MULTIGPU_CAPS pMultiGPUCaps);

#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && defined(__d3d11_h__) 
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_MultiGPU_Init
//
//! DESCRIPTION: Request to enable/disable multi GPU extension. Also if enabled automatically disables auto stereo.
//!
//! \param [in]    bEnable               if true enables the extension for all subsequently created devices. Otherwise disables it
//! \retval ::NVAPI_OK                   Call succeeded.
//! \retval ::NVAPI_ERROR                Call failed.
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D11_MultiGPU_Init(__in bool bEnable);

#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && defined(__d3d11_h__) 
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateMultiGPUDevice
//! \code
//!   DESCRIPTION: This function returns ID3D11MultiGPUDevice used for multi GPU VR support
//!
//!         \param [in]        pDevice                  current d3d device
//!         \param [in]        version                  version of requested ID3D11MultiGPUDevice. 
//!         \param [out]       currentVersion           pointer to returned current version of ID3D11MultiGPUDevice. 
//!         \param [out]       ppD3D11MultiGPUDevice    pointer to returned ID3D11MultiGPUDevice. 
//!         \param [in]        maxGpus                  max number of gpus this ID3D11MultiGPUDevice is allowed to use
//!
//!
//! \return ::NVAPI_OK                          if the call succeeds.
//!           NVAPI_INVALID_ARGUMENT            if NvAPI_D3D11_MultiGPU_Init() was not enabled prior to the creation of the D3D11 device, 
//!                                             or if maxGpus is greater than the number of GPUs in the SLI group.
//!           NVAPI_NO_ACTIVE_SLI_TOPOLOGY      on single-GPU systems, or 
//!                                             if SLI has not been enabled in the NVIDIA control panel.
//!           NVAPI_INVALID_CALL                if there is already an ID3D11MultiGPUDevice created for the specified ID3D11Device.
//!           NVAPI_INCOMPATIBLE_STRUCT_VERSION if requested interface version is greater than the one that is supported by installed.
//!                                             driver. In this case currentVersion will contain version supported by installed driver. 
//! \endcode
///////////////////////////////////////////////////////////////////////////////


//! \ingroup dx

#define NVAPI_COPY_ASYNCHRONOUSLY                    1
#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && defined(__d3d11_h__) 
#define NVAPI_COPY_P2P_READ                          2
#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && defined(__d3d11_h__) 

#define NVAPI_CPU_RESOURCE                  0xffffffff 

DECLARE_INTERFACE(ID3D11MultiGPUDevice_V1)
{
//////////////////////////////   VER1 methods //////////////////////////////////////////
    STDMETHOD_(void,Destroy)(THIS) PURE;
    STDMETHOD_(UINT,SetGPUMask)(THIS_ __in UINT GPUMask) PURE;
    STDMETHOD_(NvAPI_Status,CopySubresourceRegion)(THIS_ __in ID3D11DeviceContext *pContext, __in ID3D11Resource *pDstResource, __in UINT DstSubresource, 
                                                   __in UINT DstGPUIndex, __in UINT DstX, __in UINT DstY, __in UINT DstZ, 
                                                   __in ID3D11Resource *pSrcResource, __in UINT SrcSubresource, __in UINT SrcGPUIndex, 
                                                   __in const D3D11_BOX *pSrcBox, __in UINT ExtendedFlags = 0) PURE;
#if defined(__d3d11_1_h__)
    STDMETHOD_(NvAPI_Status,CopySubresourceRegion1)(THIS_ __in ID3D11DeviceContext1 *pContext1, __in ID3D11Resource *pDstResource, __in UINT DstSubresource, 
                                                   __in UINT DstGPUIndex, __in UINT DstX, __in UINT DstY, __in UINT DstZ, 
                                                   __in ID3D11Resource *pSrcResource, __in UINT SrcSubresource, __in UINT SrcGPUIndex, 
                                                   __in const D3D11_BOX *pSrcBox, __in UINT CopyFlags, __in UINT ExtendedFlags = 0 ) PURE;
#else
    STDMETHOD_(NvAPI_Status,CopySubresourceRegion1)(THIS_ __in void *pContext1, __in ID3D11Resource *pDstResource, __in UINT DstSubresource, 
                                                   __in UINT DstGPUIndex, __in UINT DstX, __in UINT DstY, __in UINT DstZ, 
                                                   __in ID3D11Resource *pSrcResource, __in UINT SrcSubresource, __in UINT SrcGPUIndex, 
                                                   __in const D3D11_BOX *pSrcBox, __in UINT CopyFlags, __in UINT ExtendedFlags = 0 ) PURE;
#endif
    STDMETHOD_(NvAPI_Status,UpdateSubresource)(THIS_ __in ID3D11DeviceContext *pContext,__in ID3D11Resource *pDstResource, __in UINT DstSubresource, __in UINT DstGPUIndex, 
                                               __in const D3D11_BOX *pDstBox, __in const void *pSrcData, __in UINT SrcRowPitch, __in UINT SrcDepthPitch) PURE;
    STDMETHOD_(NvAPI_Status,VSSetConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT GPUMask, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers,
                                                 __in UINT *const pFirstConstant = NULL, __in UINT *const pNumConstants = NULL) PURE;
    STDMETHOD_(NvAPI_Status,PSSetConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT GPUMask, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers,
                                                 __in UINT *const pFirstConstant = NULL, __in UINT *const pNumConstants = NULL) PURE;
    STDMETHOD_(NvAPI_Status,GSSetConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT GPUMask, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers,
                                                 __in UINT *const pFirstConstant = NULL, __in UINT *const pNumConstants = NULL) PURE;
    STDMETHOD_(NvAPI_Status,DSSetConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT GPUMask, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers,
                                                 __in UINT *const pFirstConstant = NULL, __in UINT *const pNumConstants = NULL) PURE;
    STDMETHOD_(NvAPI_Status,HSSetConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT GPUMask, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers,
                                                 __in UINT *const pFirstConstant = NULL, __in UINT *const pNumConstants = NULL) PURE;
    STDMETHOD_(NvAPI_Status,CSSetConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT GPUMask, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers,
                                                 __in UINT *const pFirstConstant = NULL, __in UINT *const pNumConstants = NULL) PURE;
    STDMETHOD_(NvAPI_Status,SetViewports)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT GPUMask, __in UINT NumViewports, 
                                          __in const D3D11_VIEWPORT *pViewports) PURE;
    STDMETHOD_(NvAPI_Status,SetScissorRects)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT GPUMask, __in UINT NumRects, 
                                            __in const D3D11_RECT *pRects) PURE;
    STDMETHOD_(HRESULT,GetData)(THIS_ __in ID3D11DeviceContext *pContext, __in ID3D11Asynchronous *pAsync, __in UINT GPUIndex, 
                                    __out void *pData, __in UINT DataSize, __in UINT GetDataFlags) PURE;
#if defined(__d3d11_2_h__)
    STDMETHOD_(NvAPI_Status,UpdateTiles)(THIS_ __in ID3D11DeviceContext2 *pContext2, __in ID3D11Resource *pDestTiledResource, 
                                        __in UINT GPUMask, __in const D3D11_TILED_RESOURCE_COORDINATE *pDestTileRegionStartCoordinate, 
                                        __in const D3D11_TILE_REGION_SIZE *pDestTileRegionSize, __in const void *pSourceTileData, 
                                        __in UINT Flags) PURE;
#else
    STDMETHOD_(NvAPI_Status,UpdateTiles)(THIS_ __in void *pContext2, __in ID3D11Resource *pDestTiledResource, 
                                        __in UINT GPUMask, __in const void *pDestTileRegionStartCoordinate, 
                                        __in const void *pDestTileRegionSize, __in const void *pSourceTileData, 
                                        __in UINT Flags) PURE;
#endif

    STDMETHOD_(NvAPI_Status,CreateFences)(THIS_ __in UINT count, __out void **ppFences) PURE;
    STDMETHOD_(NvAPI_Status,SetFence)(THIS_ __in UINT GPUIndex, __in void *hFence, __in UINT64 value) PURE;
    STDMETHOD_(NvAPI_Status,WaitForFence)(THIS_ __in UINT GPUIMask, __in void *hFence, __in UINT64 value) PURE;
    STDMETHOD_(NvAPI_Status,FreeFences)(THIS_ __in UINT count, __in void **ppFences) PURE;
    STDMETHOD_(NvAPI_Status,PresentCompositingConfig )(THIS_ __in IUnknown *pSwapChain, __in UINT GPUMask, 
                                         __in const D3D11_RECT *pRects, __in UINT flags) PURE;
//////////////////////////////   end of VER1 methods   //////////////////////////////////////////

//////////////////////////////   Methods added in VER2 //////////////////////////////////////////
    STDMETHOD_(NvAPI_Status,SetContextGPUMask)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT GPUMask) PURE;
    STDMETHOD_(NvAPI_Status,GetVideoBridgeStatus)(THIS_ __in IUnknown *pSwapChain, __in UINT* pVideoBridgeStatus) PURE;
//////////////////////////////   end of VER2 methods   //////////////////////////////////////////

//////////////////////////////   Methods added in VER3 //////////////////////////////////////////
    STDMETHOD_(NvAPI_Status,CreateMultiGPUConstantBuffer)(THIS_ __in const D3D11_BUFFER_DESC *pDesc, __in_opt const D3D11_SUBRESOURCE_DATA **ppInitialData, __out ID3D11Buffer **ppBuffer) PURE;
    STDMETHOD_(NvAPI_Status,ReleaseMultiGPUConstantBuffer)(THIS_ __in ID3D11Buffer *pBuffer) PURE;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Following XXSetMGPUConstantBuffers work the same way as DX XXSetConstantBuffers
    // The difference is that 
    // 1. They are setting constant buffers on GPUs that are defined by 
    //    current GPU mask that is set via SetGPUMask.
    // 2. For constant buffer created via CreateMultiGPUConstantBuffer these calls set GPU specific constant buffer.
    // 3. For regular constant buffers these calls set the same constant buffer on all GPUs defined by 1.
    // 4. If these functions are called in deferred context then GPUs are defined as GlobalGPUmask&LocalGPUMask
    //      where GlobalGPUmask is GPUMask set before CL execution
    //      and  LocalGPUMask is current GPUMask set in  CL.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    STDMETHOD_(NvAPI_Status,VSSetMGPUConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers) PURE;
    STDMETHOD_(NvAPI_Status,PSSetMGPUConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers) PURE;
    STDMETHOD_(NvAPI_Status,GSSetMGPUConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers) PURE;
    STDMETHOD_(NvAPI_Status,DSSetMGPUConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers) PURE;
    STDMETHOD_(NvAPI_Status,HSSetMGPUConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers) PURE;
    STDMETHOD_(NvAPI_Status,CSSetMGPUConstantBuffers)(THIS_ __in ID3D11DeviceContext *pContext, __in UINT StartSlot, 
                                                 __in UINT NumBuffers, __in ID3D11Buffer *const *ppConstantBuffers) PURE;
    
    STDMETHOD_(NvAPI_Status,UpdateConstantBuffer)(__in ID3D11DeviceContext *pContext, __in ID3D11Buffer *pBuffer, __in const void *pSrcData, __in_opt UINT GPUMask = 0) PURE;
 //////////////////////////////   end of VER3 methods   //////////////////////////////////////////
};

//! Synchronization macros based on fences.
#define FENCE_SYNCHRONIZATION_START(pMultiGPUDevice, hFence, Value, srcGpu, dstGpu) \
    pMultiGPUDevice->SetFence(dstGpu, hFence, Value); \
    pMultiGPUDevice->WaitForFence(1 << (srcGpu), hFence, Value); \
    Value++;

#define FENCE_SYNCHRONIZATION_END(pMultiGPUDevice, hFence, Value, srcGpu, dstGpu) \
    pMultiGPUDevice->SetFence(srcGpu, hFence, Value); \
    pMultiGPUDevice->WaitForFence(1 << (dstGpu), hFence, Value); \
    Value++;
    
//! PresentCompositingConfig method flags.
#define NVAPI_PRESENT_COMPOSITING_CONFIG_FLAG_USE_VIDEO_BRIDGE          0x01
#define NVAPI_PRESENT_COMPOSITING_CONFIG_FLAG_CLEAR_OUTBANDS            0x02
#define NVAPI_PRESENT_COMPOSITING_CONFIG_FLAG_GET_VIDEO_BRIDGE_STATUS   0x80000000

#define NVAPI_VIDEO_BRIDGE_STATUS_AVAILABLE       0
#define NVAPI_VIDEO_BRIDGE_STATUS_NOT_AVAILABLE   1
#define NVAPI_VIDEO_BRIDGE_STATUS_FAILED_ACCESS   2
#define NVAPI_VIDEO_BRIDGE_STATUS_UNKNOWN         3

#define NVAPI_ALL_GPUS              0
typedef ID3D11MultiGPUDevice_V1     ID3D11MultiGPUDevice;

#define ID3D11MultiGPUDevice_VER1   MAKE_NVAPI_VERSION(ID3D11MultiGPUDevice_V1, 1)
#define ID3D11MultiGPUDevice_VER2   MAKE_NVAPI_VERSION(ID3D11MultiGPUDevice_V1, 2)
#define ID3D11MultiGPUDevice_VER3   MAKE_NVAPI_VERSION(ID3D11MultiGPUDevice_V1, 3)
#define ID3D11MultiGPUDevice_VER    ID3D11MultiGPUDevice_VER3

#define ALL_GPUS 0

//! \ingroup dx
NVAPI_INTERFACE NvAPI_D3D11_CreateMultiGPUDevice(__in ID3D11Device *pDevice, __in ULONG version, __out ULONG *currentVersion, __out ID3D11MultiGPUDevice **ppD3D11MultiGPUDevice, __in UINT maxGpus=ALL_GPUS);

#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!
//! Used to query the support of Single Pass Stereo HW feature
//! \ingroup dx 
typedef struct _NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_V1
{
    NvU32 version;                                                          // parameter struct version
    NvU32 bSinglePassStereoSupported;                                       // Single Pass Stereo supported
} NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_V1;

typedef struct _NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_V2
{
    NvU32 version;                                                          // _IN_     parameter struct version
    NvU32 bSinglePassStereoSupported : 1;                                   // _OUT_    Single Pass Stereo supported
    NvU32 bSinglePassStereoXYZWSupported : 1;                               // _OUT_    Single Pass Stereo XYZW supported
    NvU32 reserved : 30;                                                    // _INOUT_  bits reserved for future use
} NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_V2;

typedef NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_V2                 NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS;
#define NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_VER1               MAKE_NVAPI_VERSION(NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_V1, 1)
#define NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_VER2               MAKE_NVAPI_VERSION(NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_V2, 2)
#define NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_VER                NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_VER2

#ifndef NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_VER
typedef NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_V1                 NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS;
#define NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_VER1               MAKE_NVAPI_VERSION(NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_V1, 1)
#define NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_VER                NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS_VER1
#endif

#if defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_QuerySinglePassStereoSupport
//
//!   DESCRIPTION: Queries the support of Single Pass Stereo feature on current setup and returns appropriate boolean value.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     pDevice                                 The ID3D11Device to use.
//! \param [inout]  pSinglePassStereoSupportedParams        Stores value of whether Single Pass Stereo is supported on current setup or not.
//!
//! \retval  NVAPI_OK                           Call succeeded.
//! \retval  NVAPI_ERROR                        Call failed.
//! \retval  NVAPI_INVALID_ARGUMENT             One or more arguments are invalid.
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 
NVAPI_INTERFACE NvAPI_D3D_QuerySinglePassStereoSupport(__in IUnknown *pDevice,
                                                __inout NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS *pQuerySinglePassStereoSupportedParams);

#endif //defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__))

#if defined(__cplusplus) && defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_SetSinglePassStereoMode
//
//!   DESCRIPTION: Set the Single Pass Stereo state
//!
//! \note   Note that this is an asynchronous function and returns NVAPI_OK if all arguments are valid.
//!         Returned value NVAPI_OK does not reflect that Single Pass Stereo is supported or is set in hardware.
//!         One must call NvAPI_D3D_QuerySinglePassStereoSupport() to confirm that the current setup
//!         supports Single Pass Stereo before calling this set-function.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  pDevOrContext                  The ID3D11Device or ID3D11DeviceContext to use.
//! \param [in]  numViews                       Number of views to render.
//! \param [in]  renderTargetIndexOffset        Offset between render targets of the different views.
//! \param [in]  independentViewportMaskEnable  Is the independent viewport mask enabled.
//!                
//! \retval  NVAPI_OK                           Call succeeded.
//! \retval  NVAPI_ERROR                        Call failed.
//! \retval  NVAPI_INVALID_ARGUMENT             One or more arguments are invalid.
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 
NVAPI_INTERFACE NvAPI_D3D_SetSinglePassStereoMode(__in IUnknown *pDevOrContext, __in NvU32 numViews, __in NvU32 renderTargetIndexOffset, __in NvU8 independentViewportMaskEnable);

#endif //defined(__cplusplus) && defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__)

#if defined(__cplusplus) && ( defined(__d3d12_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_QuerySinglePassStereoSupport
//
//!   DESCRIPTION: Queries the support of Single Pass Stereo feature on current setup and returns appropriate boolean value.
//!
//! SUPPORTED OS:  Windows 10
//!
//!
//! \param [in]     pDevice                                 The IDirect3DDevice12 to use.
//! \param [inout]  pQuerySinglePassStereoSupportedParams   Stores value of whether Single Pass Stereo is supported on current setup or not.
//!
//! \retval  NVAPI_OK                                       Call succeeded.
//! \retval  NVAPI_ERROR                                    Call failed.
//! \retval  NVAPI_INVALID_ARGUMENT                         One or more arguments are invalid.
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 

NVAPI_INTERFACE NvAPI_D3D12_QuerySinglePassStereoSupport(__in ID3D12Device *pDevice,
                                                         __inout NV_QUERY_SINGLE_PASS_STEREO_SUPPORT_PARAMS *pQuerySinglePassStereoSupportedParams);

#endif // defined(__cplusplus) && ( defined(__d3d12_h__))

#if defined(__cplusplus) && ( defined(__d3d12_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_SetSinglePassStereoMode
//
//!   DESCRIPTION: Set the Single Pass Stereo state.
//!
//! \note   Note that Single Pass Stereo state persists on a particular CommandList till it is closed.
//!         The state is reset to default (disabled) for every newly created CommandList.
//!         One must call NvAPI_D3D12_QuerySinglePassStereoSupport() to confirm that the current setup
//!         supports Single Pass Stereo before calling this set-function.
//!
//! SUPPORTED OS:  Windows 10
//!
//!
//! \param [in]  pCommandList                   The command list in which we will add push buffer commmands for enabling Single Pass Stereo feature
//!                                             Note: Command list of type D3D12_COMMAND_LIST_TYPE_BUNDLE is not allowed for setting the state of this feature.
//! \param [in]  numViews                       Number of views to render.
//! \param [in]  RenderTargetIndexOffset        Offset between render targets of the different views.
//! \param [in]  IndependentViewportMaskEnable  Is the independent viewport mask enabled.
//!
//! \retval  NVAPI_OK                           Call succeeded.
//! \retval  NVAPI_ERROR                        Call failed.
//! \retval  NVAPI_INVALID_ARGUMENT             One or more arguments are invalid.
//!
//! \ingroup dx 
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D12_SetSinglePassStereoMode(__in ID3D12GraphicsCommandList* pCommandList,
                                                    __in NvU32 numViews,
                                                    __in NvU32 renderTargetIndexOffset,
                                                    __in NvU8 independentViewportMaskEnable);

#endif // defined(__cplusplus) && ( defined(__d3d12_h__))

//! SUPPORTED OS:  Windows 7 and higher
//!
//! Used to query the support of MultiView HW feature
//! \ingroup dx 

typedef struct _NV_QUERY_MULTIVIEW_SUPPORT_PARAMS_V1
{
    NvU32 version;                                              //  _IN_     parameter struct version
    NvU32 bMultiViewSupported : 1;                              //  _OUT_    MultiView supported (Render 4 views in a single pass)
    NvU32 bSinglePassStereoSupported : 1;                       //  _OUT_    StereoX supported (Render 2 views in a single pass)
    NvU32 bSinglePassStereoXYZWSupported : 1;                   //  _OUT_    StereoXYZW supported (Render 2 views in a single pass)
    NvU32 reserved : 29;                                        //  _INOUT_  bits reserved for future use
} NV_QUERY_MULTIVIEW_SUPPORT_PARAMS_V1;

typedef NV_QUERY_MULTIVIEW_SUPPORT_PARAMS_V1                    NV_QUERY_MULTIVIEW_SUPPORT_PARAMS;
#define NV_QUERY_MULTIVIEW_SUPPORT_PARAMS_VER1                  MAKE_NVAPI_VERSION(NV_QUERY_MULTIVIEW_SUPPORT_PARAMS_V1, 1)
#define NV_QUERY_MULTIVIEW_SUPPORT_PARAMS_VER                   NV_QUERY_MULTIVIEW_SUPPORT_PARAMS_VER1
#define NV_MULTIVIEW_MAX_SUPPORTED_VIEWS                        4

#if defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_QueryMultiViewSupport
//
//!   DESCRIPTION: Queries the support of MultiView feature on current setup and returns appropriate boolean value.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since          Release: 410
//!
//! \param [in]     pDevice                                 The ID3D11Device to use.
//! \param [inout]  pMultiViewSupportedParams               Stores value of whether MultiView is supported on current setup or not.
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!         (none)
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 
NVAPI_INTERFACE NvAPI_D3D_QueryMultiViewSupport(__in IUnknown *pDevice,
                                                __inout NV_QUERY_MULTIVIEW_SUPPORT_PARAMS *pQueryMultiViewSupportedParams);

#endif //defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__))

//! SUPPORTED OS:  Windows 7 and higher
//!
//! Used for setting the Mode for MultiView HW Feature.
//! \ingroup dx 
typedef struct _NV_MULTIVIEW_PARAMS_V1
{
    NvU32 version;                                                                 // _IN_  parameter struct version
    NvU32 numViews;                                                                // _IN_  Number of views to render.
    NvU32 renderTargetIndexOffset[NV_MULTIVIEW_MAX_SUPPORTED_VIEWS];               // _IN_  Offset between render targets for each of the per views.
    NvU8  independentViewportMaskEnable;                                           // _IN_  Is the independent viewport mask enabled.
} NV_MULTIVIEW_PARAMS_V1;

typedef NV_MULTIVIEW_PARAMS_V1                                       NV_MULTIVIEW_PARAMS;
#define NV_MULTIVIEW_PARAMS_VER1                                     MAKE_NVAPI_VERSION(NV_MULTIVIEW_PARAMS_V1, 1)
#define NV_MULTIVIEW_PARAMS_VER                                      NV_MULTIVIEW_PARAMS_VER1

#if defined(__cplusplus) && defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_SetMultiViewMode
//
//!   DESCRIPTION: Set the MultiView state
//!
//! \note   Note that this is an asynchronous function and returns NVAPI_OK if all arguments are valid.
//!         Returned value NVAPI_OK does not reflect that MultiView is supported or is set in hardware.
//!         One must call NvAPI_D3D_QueryMultiViewSupport() to confirm that the current setup
//!         supports MultiView before calling this set-function.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since          Release: 410
//!
//! \param [in]  pDevOrContext                      The ID3D11Device or ID3D11DeviceContext to use.
//! \param [in]  pMultiViewParams                   MultiView Params
//!                
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!         (none)
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 
NVAPI_INTERFACE NvAPI_D3D_SetMultiViewMode(__in IUnknown *pDevOrContext, __in NV_MULTIVIEW_PARAMS *pMultiViewParams);

#endif //defined(__cplusplus) && defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!
//! Used to query the support of Lens Matched Shading HW feature
//! \ingroup dx 
typedef struct _NV_QUERY_MODIFIED_W_SUPPORT_PARAMS
{
    NvU32 version;                                                 // parameter struct version
    NvU32 bModifiedWSupported;                                     // Modified W supported
} NV_QUERY_MODIFIED_W_SUPPORT_PARAMS_V1;

typedef NV_QUERY_MODIFIED_W_SUPPORT_PARAMS_V1               NV_QUERY_MODIFIED_W_SUPPORT_PARAMS;
#define NV_QUERY_MODIFIED_W_SUPPORT_PARAMS_VER1             MAKE_NVAPI_VERSION(NV_QUERY_MODIFIED_W_SUPPORT_PARAMS_V1, 1)
#define NV_QUERY_MODIFIED_W_SUPPORT_PARAMS_VER              NV_QUERY_MODIFIED_W_SUPPORT_PARAMS_VER1

#if defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_QueryModifiedWSupport
//
//!   DESCRIPTION: Queries the support of Modified W feature on current setup and returns appropriate boolean value.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     pDevice                             The ID3D11Device to use.
//! \param [inout]  pQueryModifiedWSupportedParams      Stores value of whether Modified W is supported on current setup or not.
//!
//! \retval  NVAPI_OK                           Call succeeded.
//! \retval  NVAPI_ERROR                        Call failed.
//! \retval  NVAPI_INVALID_ARGUMENT             One or more arguments are invalid.
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 
NVAPI_INTERFACE NvAPI_D3D_QueryModifiedWSupport(__in IUnknown *pDev,
                                            __inout NV_QUERY_MODIFIED_W_SUPPORT_PARAMS *pQueryModifiedWSupportedParams);
#endif //defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__))

//! SUPPORTED OS:  Windows 7 and higher
//!
#define NV_MODIFIED_W_MAX_VIEWPORTS    16

typedef struct _NV_MODIFIED_W_COEFFICIENTS
{
    float fA;                               // A coefficient in w' = w + Ax + By
    float fB;                               // B coefficient in w' = w + Ax + By
    float fAReserved;                       // reserved
    float fBReserved;                       // reserved

    float fReserved[2];                     // reserved
} NV_MODIFIED_W_COEFFICIENTS;

typedef struct _NV_MODIFIED_W_PARAMS
{
    NvU32 version;                                                                 // parameter struct version
    NvU32 numEntries;                                                              // number of valid NV_MODIFIED_W_COEFFICIENTS structs in array
    NV_MODIFIED_W_COEFFICIENTS modifiedWCoefficients[NV_MODIFIED_W_MAX_VIEWPORTS]; // coefficients

    NvU32 id;                                                     // reserved
    NvU32 reserved[NV_MODIFIED_W_MAX_VIEWPORTS];                  // reserved
} NV_MODIFIED_W_PARAMS_V1;

typedef NV_MODIFIED_W_PARAMS_V1      NV_MODIFIED_W_PARAMS;
#define NV_MODIFIED_W_PARAMS_VER1    MAKE_NVAPI_VERSION(NV_MODIFIED_W_PARAMS_V1, 1)
#define NV_MODIFIED_W_PARAMS_VER     NV_MODIFIED_W_PARAMS_VER1

#if defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_SetModifiedWMode
//
//!   DESCRIPTION: Set the Modified W state and A,B coefficients for HW support
//!
//! \note   Note that this is an asynchronous function and returns NVAPI_OK if all arguments are valid.
//!         Returned value NVAPI_OK does not reflect that Modified-W is supported or is set in hardware.
//!         One must call NvAPI_D3D_QueryModifiedWSupport() to confirm that the current setup
//!         supports Modified-W before calling this set-function.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  pDevOrContext                  The ID3D11Device or ID3D11DeviceContext to use.
//! \param [in]  psModifiedWParams              Modified W parameters.
//!
//! \retval  NVAPI_OK                           Call succeeded.
//! \retval  NVAPI_ERROR                        Call failed.
//! \retval  NVAPI_INVALID_ARGUMENT             One or more arguments are invalid.
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 
NVAPI_INTERFACE NvAPI_D3D_SetModifiedWMode(__in IUnknown *pDevOrContext, __in NV_MODIFIED_W_PARAMS *psModifiedWParams);

#endif //defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__))

#if defined(__cplusplus) && ( defined(__d3d12_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_QueryModifiedWSupport
//
//!   DESCRIPTION: Queries the support of Modified-W feature on current setup and returns appropriate boolean value.
//!
//! SUPPORTED OS:  Windows 10
//!
//!
//! \param [in]     pDevice                         The ID3D12Device Device created by application
//! \param [inout]  pQueryModifiedWSupportedParams  Stores value of whether Modified-W is supported on current setup or not.
//!
//! \retval  NVAPI_OK                               Call succeeded.
//! \retval  NVAPI_ERROR                            Call failed.
//! \retval  NVAPI_INVALID_ARGUMENT                 One or more arguments are invalid.
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 

NVAPI_INTERFACE NvAPI_D3D12_QueryModifiedWSupport(__in ID3D12Device *pDevice,
                                                  __inout NV_QUERY_MODIFIED_W_SUPPORT_PARAMS *pQueryModifiedWSupportedParams);

#endif // defined(__cplusplus) && ( defined(__d3d12_h__))

#if defined(__cplusplus) && ( defined(__d3d12_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_SetModifiedWMode
//
//!   DESCRIPTION: Set the Modified-W state and A, B coefficients for HW support
//!
//! \note   Note that Modified-W state persists on a particular CommandList till it is closed.
//!         The state is reset to default (disabled) for every newly created CommandList.
//!         One must call NvAPI_D3D12_QueryModifiedWSupport() to confirm that the current setup
//!         supports Modified-W before calling this set-function.
//!
//! SUPPORTED OS:  Windows 10
//!
//!
//! \param [in]  pCommandList                   The command list in which we will add push buffer commmands for enabling Modified-W feature
//!                                             Note: Command list of type D3D12_COMMAND_LIST_TYPE_BUNDLE is not allowed for setting the state of this feature.
//! \param [in]  pModifiedWParams               Modified-W parameters.
//!
//! \retval  NVAPI_OK                           Call succeeded.
//! \retval  NVAPI_ERROR                        Call failed.
//! \retval  NVAPI_INVALID_ARGUMENT             One or more arguments are invalid.
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 

NVAPI_INTERFACE NvAPI_D3D12_SetModifiedWMode(__in ID3D12GraphicsCommandList* pCommandList,
                                             __in NV_MODIFIED_W_PARAMS *pModifiedWParams);

#endif // defined(__cplusplus) && ( defined(__d3d12_h__))


#if defined(__cplusplus) && (defined(__d3d11_h__))

//! \ingroup dx
//! See NvAPI_D3D_CreateLateLatchObject
DECLARE_INTERFACE(ID3DLateLatchObject_V1)
{
    STDMETHOD_(UINT,Release)                    (THIS) PURE;                                    //! Release the created LateLatch object and associated buffers.
    STDMETHOD_(NvAPI_Status,Latch)              (THIS_ __in IUnknown* pContext = NULL) PURE;    //! Request to queue the latch operation to the GPU.
    STDMETHOD_(ID3D11Buffer*,GetD3D11Buffer)    (THIS_ __in UINT index = 0) PURE;               //! Get ID3D11Buffer* available at the given 'index'
    STDMETHOD_(UINT,GetBufferCount)             (THIS) PURE;                                    //! Returns the number of late latch buffers created for this LateLatchObject.
    STDMETHOD_(NvAPI_Status,UpdateData)         (THIS_ __in void **ppData) PURE;                //! Fully update all LateLatch buffers with new data.
    STDMETHOD_(NvAPI_Status,UpdateData)         (THIS_ __in void *pData, __in size_t offset, 
                                                  __in size_t size, __in UINT index = 0) PURE;  //! Partially update one of the LateLatch buffers with new data.
};

//! \ingroup dx
//! See NvAPI_D3D_CreateLateLatchObject
typedef ID3DLateLatchObject_V1    ID3DLateLatchObject;
#define ID3DLateLatchObject_VER1  MAKE_NVAPI_VERSION(ID3DLateLatchObject_V1, 1)
#define ID3DLateLatchObject_VER   ID3DLateLatchObject_VER1

typedef struct _NV_D3D_LATELATCH_OBJECT_DESC_V1
{
    NvU32                       version;
    NvU32                       numBuffers;                 // _IN_  Number of LateLatch buffers that the app wants to create. 
    D3D11_BUFFER_DESC           **ppBufferDesc;             // _IN_  Description of buffers
    ID3DLateLatchObject         **ppD3DLateLatchObject;     // _Out_ Pointer to created interface
} NV_D3D_LATELATCH_OBJECT_DESC_V1;

typedef NV_D3D_LATELATCH_OBJECT_DESC_V1    NV_D3D_LATELATCH_OBJECT_DESC;
#define NV_D3D_LATELATCH_OBJECT_DESC_VER1  MAKE_NVAPI_VERSION(NV_D3D_LATELATCH_OBJECT_DESC_V1, 1)
#define NV_D3D_LATELATCH_OBJECT_DESC_VER   NV_D3D_LATELATCH_OBJECT_DESC_VER1

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_CreateLateLatchObject
//
//! DESCRIPTION: Creates a Late Latch Object interface
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 384
//!
//! \param [in]      pDevice                   Current ID3D11Device.
//! \param [inout]   pLateLatchObjectDesc      Pointer to in/out structure for late latch object creation
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D_CreateLateLatchObject(__in IUnknown *pDevice, __inout NV_D3D_LATELATCH_OBJECT_DESC* pLateLatchObjectDesc);

#endif // defined(__cplusplus) && (defined(__d3d11_h__))



#if defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))
//! \ingroup dx
//! See NvAPI_D3D_QueryLateLatchSupport
typedef struct _NV_QUERY_LATELATCH_SUPPORT_PARAMS
{
    NvU32 version;                      //!< (IN)  Parameter structure version
    NvU32 bLateLatchSupported;          //!< (OUT) LateLatch supported
} NV_QUERY_LATELATCH_SUPPORT_PARAMS_V1;

typedef NV_QUERY_LATELATCH_SUPPORT_PARAMS_V1               NV_QUERY_LATELATCH_SUPPORT_PARAMS;
#define NV_QUERY_LATELATCH_SUPPORT_PARAMS_VER1             MAKE_NVAPI_VERSION(NV_QUERY_LATELATCH_SUPPORT_PARAMS_V1, 1)
#define NV_QUERY_LATELATCH_SUPPORT_PARAMS_VER              NV_QUERY_LATELATCH_SUPPORT_PARAMS_VER1

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_QueryLateLatchSupport
//
//!   DESCRIPTION: Queries the support of DX11 Late Latch feature on current setup.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 384
//!
//! \param [in]     pDevice                             Current ID3D11Device.
//! \param [inout]  pQueryLateLatchSupportParams        Stores value of whether Late Latch is supported on current setup or not.
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 
NVAPI_INTERFACE NvAPI_D3D_QueryLateLatchSupport(__in IUnknown *pDevice,
                                            __inout NV_QUERY_LATELATCH_SUPPORT_PARAMS *pQueryLateLatchSupportParams);
#endif // defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))



#if defined (__cplusplus) && (defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_RegisterDevice
//
//!   DESCRIPTION: Tells NvAPI about a D3D device. This must be called prior to using any DX1x
//!                deferred-context calls.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  pDev                           The ID3D10Device or ID3D11Device to use.
//!                
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 
NVAPI_INTERFACE NvAPI_D3D_RegisterDevice(__in IUnknown *pDev);

#endif //if defined(__cplusplus) && (defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__))



#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_MultiDrawInstancedIndirect
//
//!   DESCRIPTION: Extension of DrawInstancedIndirect that takes a draw count in. The effect of this function is to loop over 
//!                that draw count and perform the DrawInstancedIndirect operation each time, incrementing the buffer offset
//!                by the supplied stride each time.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]        *pDevContext11                  Pointer to D3D11 device context (IC or DC)
//! \param [in]         drawCount                      Do DrawInstancedIndirect operation this many times
//! \param [in]        *pBuffer                        ID3D11Buffer that contains the command parameters
//! \param [in]         alignedByteOffsetForArgs       Start in pBuffer of the command parameters
//! \param [in]         alignedByteStrideForArgs       Stride of the command parameters - must be >= 4 * sizeof(NvU32)
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \retval  NVAPI_D3D_DEVICE_NOT_REGISTERED     When MultiDraw is called on a deferred context, and the device has not yet
//!                                              been registered (NvAPI_D3D_RegisterDevice), this error is returned.
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_MultiDrawInstancedIndirect(__in ID3D11DeviceContext *pDevContext11,
                                                       __in NvU32                drawCount,
                                                       __in ID3D11Buffer        *pBuffer,
                                                       __in NvU32                alignedByteOffsetForArgs,
                                                       __in NvU32                alignedByteStrideForArgs);

#endif //defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))


#if defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_MultiDrawIndexedInstancedIndirect
//
//!   DESCRIPTION: Extension of DrawIndexedInstancedIndirect that takes a draw count in. The effect of this function is to loop over 
//!                that draw count and perform the DrawIndexedInstancedIndirect operation each time, incrementing the buffer offset
//!                by the supplied stride each time.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]        *pDevContext11                  Pointer to D3D11 device context (IC or DC)
//! \param [in]         drawCount                      Do DrawIndexedInstancedIndirect operation this many times
//! \param [in]        *pBuffer                        ID3D11Buffer that contains the command parameters
//! \param [in]         alignedByteOffsetForArgs       Start in pBuffer of the command parameters
//! \param [in]         alignedByteStrideForArgs       Stride of the command parameters - must be >= 5 * sizeof(NvU32)
//!
//! RETURN STATUS:     This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                    If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \retval  NVAPI_D3D_DEVICE_NOT_REGISTERED     When MultiDraw is called on a deferred context, and the device has not yet
//!                                              been registered (NvAPI_D3D_RegisterDevice), this error is returned.
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_MultiDrawIndexedInstancedIndirect(__in ID3D11DeviceContext *pDevContext11,
                                                              __in NvU32                drawCount,
                                                              __in ID3D11Buffer        *pBuffer,
                                                              __in NvU32                alignedByteOffsetForArgs,
                                                              __in NvU32                alignedByteStrideForArgs);

#endif //defined (__cplusplus) && (defined(__d3d11_h__) || defined(__d3d11_1_h__))

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && ( defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) ||defined(__d3d11_h__) )
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME:   NvAPI_D3D_ImplicitSLIControl
//
//! This function enables/disables the SLI rendering mode. It has to be called prior to D3D device creation. Once this function is called with DISABLE_IMPLICIT_SLI
//! parameter all subsequently created devices will be forced to run in a single gpu mode until the same function is called with ENABLE_IMPLICIT_SLI parameter. The enable 
//! call will force all subsequently created devices to run in default implicit SLI mode being determined by an application profile or a global control panel SLI setting.
//! This NvAPI call is supported in all DX10+ versions of the driver. It is supported on all Windows versions.
//!
//! \retval    NVAPI_OK                             Completed request
//! \retval    NVAPI_ERROR                          Error occurred
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

//! \ingroup dx
typedef enum _IMPLICIT_SLI_CONTROL
{
    DISABLE_IMPLICIT_SLI    = 0,
    ENABLE_IMPLICIT_SLI     = 1,
} IMPLICIT_SLI_CONTROL;

//! \ingroup dx
NVAPI_INTERFACE NvAPI_D3D_ImplicitSLIControl(__in IMPLICIT_SLI_CONTROL implicitSLIControl);

#endif //defined (__cplusplus) && ( defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) ||defined(__d3d11_h__) )

//! SUPPORTED OS:  Windows 10
//!

#if defined (__cplusplus) && defined(__d3d12_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_UseDriverHeapPriorities
//
//! \code
//!   DESCRIPTION: Sets the driver to override Microsoft's heap allocation priority values with Nvidia driver priority values. Use this once per process before allocating resources.
//!
//! \param [in] pDevice The IDirect3DDevice12 to use.
//!
//! \return  This API can return any of the error codes enumerated in
//!          #NvAPI_Status.  If there are return error codes with specific
//!          meaning for this API, they are listed below.
//!
//! \since Release: 381
//!
//! \endcode
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D12_UseDriverHeapPriorities(__in ID3D12Device *pDevice);

#endif // defined (__cplusplus) && defined(__d3d12_h__)

//! SUPPORTED OS:  Windows 10 and higher
//!
#if defined(__cplusplus) && ( defined(__d3d12_h__))


typedef struct _NV_D3D12_MOSAIC_GETCOMPANIONALLOCATIONS
{
    __in    NvU32            version;               //!< Structure version
    __in    ID3D12Device    *pDevice;               //!< The ID3D12Device created by application.
    __in    ID3D12Resource  *pSwapChainBuffer;      //!< The ID3D12Resource part of the application swap chain that has companion allocations.
    __in    NvU32            companionBufferCount;  //!< The number of ID3D12Resource pointers requested to be returned in the ppComanionResources array, which should match ID3D12Device::GetNodeCount for the complete set of companion allocations.
    __inout ID3D12Resource **ppCompanionResources;  //!< An array of ID3D12Resource pointers sized to match companionBufferCount, which will receive the companion allocations.
} NV_D3D12_MOSAIC_GETCOMPANIONALLOCATIONS_V1;

typedef NV_D3D12_MOSAIC_GETCOMPANIONALLOCATIONS_V1              NV_D3D12_MOSAIC_GETCOMPANIONALLOCATIONS;
#define NV_D3D12_MOSAIC_GETCOMPANIONALLOCATIONS_VER1            MAKE_NVAPI_VERSION(NV_D3D12_MOSAIC_GETCOMPANIONALLOCATIONS_V1, 1)
#define NV_D3D12_MOSAIC_GETCOMPANIONALLOCATIONS_VER             NV_D3D12_MOSAIC_GETCOMPANIONALLOCATIONS_VER1

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_Mosaic_GetCompanionAllocations
//
//!   DESCRIPTION: Queries the driver for internally created allocations that accompany a swap chain buffer for present-related operations.
//!                Surfaces returned by this interface must be destroied at the same time that the original swap chain buffer is destroyed.
//!                In general this occurs prior to a ResizeBuffers call, or when the swap chain is released.
//!                Note that this function only works in Landscape orientation due to Windows behavior, and attempts to utilize it with any type of display
//!                rotation will result in failure.
//!
//! \param [inout]  companionBufferCount            The parameters for this function.
//!
//! \retval  NVAPI_OK                               Call succeeded.
//! \retval  NVAPI_ERROR                            Call failed.
//! \retval  NVAPI_INVALID_ARGUMENT                 One or more arguments are invalid.
//! \retval  NVAPI_INVALID_CALL                     System configuration does not support this interface (eg, display is rotated, mosaic not enabled, etc)
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 

NVAPI_INTERFACE NvAPI_D3D12_Mosaic_GetCompanionAllocations(__inout NV_D3D12_MOSAIC_GETCOMPANIONALLOCATIONS *params);

#endif // defined(__cplusplus) && ( defined(__d3d12_h__))

//! SUPPORTED OS:  Windows 10 and higher
//!
#if defined(__cplusplus) && ( defined(__d3d12_h__))

typedef struct _NV_D3D12_MOSAIC_GETVIEWPORTANDGPUPARTITIONS
{
    __in    NvU32                                     version;              //!< Structure version
    __in    ID3D12Device                             *pDevice;              //!< The ID3D12Device created by application.
    __in    ID3D12Resource                           *pSwapChainBuffer;     //!< The ID3D12Resource part of the application swap chain.
    __inout NvU32                                    *pPartitionCount;      //!< A variable to receive the number of NV_MGPU_MOSAIC_DISPLAY_SURFACE_PARTITION elements returned or that holds the size of pPartitions when it is non-NULL.
    __inout RECT                                     *pViewport;            //!< An optional array to hold the viewport information per partition. When this is valid pNodeMask must also be valid.
    __inout NvU32                                    *pNodeMask;            //!< An optional array to hold the GPU mask where this viewport must be valid per partition. When this is valid pViewport must also be valid.
} NV_D3D12_MOSAIC_GETVIEWPORTANDGPUPARTITIONS_V1;

typedef NV_D3D12_MOSAIC_GETVIEWPORTANDGPUPARTITIONS_V1          NV_D3D12_MOSAIC_GETVIEWPORTANDGPUPARTITIONS;
#define NV_D3D12_MOSAIC_GETVIEWPORTANDGPUPARTITIONS_VER1        MAKE_NVAPI_VERSION(NV_D3D12_MOSAIC_GETVIEWPORTANDGPUPARTITIONS_V1, 1)
#define NV_D3D12_MOSAIC_GETVIEWPORTANDGPUPARTITIONS_VER         NV_D3D12_MOSAIC_GETVIEWPORTANDGPUPARTITIONS_VER1

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D12_Mosaic_GetViewportAndGpuPartitions
//
//!   DESCRIPTION: Queries the driver for how a swap chain display surface is subdivided across devices in relation to display connectivity.
//!                Call this interface with NULL pPartitions in order to know how many subdivisions exist and allocate the proper size to hold all data.
//!                Call it a second time with a properly sized partitions array to receive all subdivisions along with GPU node masks of each rectangle.
//!                Note that this function only works in Landscape orientation due to Windows behavior, and attempts to utilize it with any type of display
//!                rotation will result in failure.
//!
//! \param [inout]  params                          The parameters for this function.
//!
//! \retval  NVAPI_OK                               Call succeeded.
//! \retval  NVAPI_ERROR                            Call failed.
//! \retval  NVAPI_INVALID_ARGUMENT                 One or more arguments are invalid.
//! \retval  NVAPI_INVALID_CALL                     System configuration does not support this interface (eg, display is rotated, mosaic not enabled, etc)
//!
//! \ingroup dx 
/////////////////////////////////////////////////////////////////////////////// 

NVAPI_INTERFACE NvAPI_D3D12_Mosaic_GetViewportAndGpuPartitions(__inout NV_D3D12_MOSAIC_GETVIEWPORTANDGPUPARTITIONS *params);

#endif // defined(__cplusplus) && ( defined(__d3d12_h__))


#if defined(__cplusplus) && (defined(__d3d11_h__))
//! \ingroup dx
//! See NvAPI_D3D1x_GetGraphicsCapabilities

typedef struct _NV_D3D1x_GRAPHICS_CAPS_V1
{
    NvU32   bExclusiveScissorRectsSupported         :  1;     //!< (OUT) Outputs whether Exclusive Scissor Rects are supported or not
    NvU32   bVariablePixelRateShadingSupported      :  1;     //!< (OUT) Outputs whether Variable Pixel Shading Rates are supported or not
    NvU32   reservedBits                            : 30;     // Reserved bits for future expansion
    NvU32   reserved[7];                                      // Reserved for future expansion
} NV_D3D1x_GRAPHICS_CAPS_V1;

#define NV_D3D1x_GRAPHICS_CAPS_VER1  MAKE_NVAPI_VERSION(NV_D3D1x_GRAPHICS_CAPS_V1, 1)

typedef struct _NV_D3D1x_GRAPHICS_CAPS_V2
{
    NvU32   bExclusiveScissorRectsSupported         :  1;     //!< (OUT) Outputs whether Exclusive Scissor Rects are supported or not
    NvU32   bVariablePixelRateShadingSupported      :  1;     //!< (OUT) Outputs whether Variable Pixel Shading Rates are supported or not
    NvU32   bFastUAVClearSupported                  :  1;     //!< (OUT) Outputs whether UAVClear is implemented using ZBC rather than compute shader
    NvU32   reservedBits                            : 29;     // Reserved bits for future expansion
    NvU16   majorSMVersion;                                   //!< (OUT) Major SM version of the device
    NvU16   minorSMVersion;                                   //!< (OUT) Minor SM version of the device
    NvU32   reserved[14];                                     // Reserved for future expansion
} NV_D3D1x_GRAPHICS_CAPS_V2;

typedef NV_D3D1x_GRAPHICS_CAPS_V2    NV_D3D1x_GRAPHICS_CAPS;
#define NV_D3D1x_GRAPHICS_CAPS_VER2  MAKE_NVAPI_VERSION(NV_D3D1x_GRAPHICS_CAPS_V2, 2)
#define NV_D3D1x_GRAPHICS_CAPS_VER   NV_D3D1x_GRAPHICS_CAPS_VER2

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D1x_GetGraphicsCapabilities
//
//! DESCRIPTION: Get the graphics capabilities for current hardware/software setup
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 410
//!
//! \param [in]        pDevice              The ID3D11Device device to be used for getting the graphics capabilities.
//! \param [in]        structVersion        Version of the caps struct. Should be set to NV_D3D1x_GRAPHICS_CAPS_VER.
//! \param [inout]     pGraphicsCaps        Pointer to a NV_D3D1x_GRAPHICS_CAPS_CAPS struct created by app.
//!                                         Graphics capabilities will be filled in this struct by the driver.
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!         (none)
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D1x_GetGraphicsCapabilities(__in IUnknown *pDevice,
                                                    __in NvU32 structVersion,
                                                    __inout NV_D3D1x_GRAPHICS_CAPS *pGraphicsCaps);

#endif // defined(__cplusplus) && (defined(__d3d11_h__))


#if defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))
#define NV_MAX_NUM_EXCLUSIVE_SCISSOR_RECTS    16
#endif // defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))

#if defined(__cplusplus) && (defined(__d3d11_h__))
//! \ingroup dx
//! See NvAPI_D3D11_RSSetExclusiveScissorRects

typedef struct _NV_D3D11_EXCLUSIVE_SCISSOR_RECT_DESC_V1
{
    bool        enableExclusiveScissorRect; //!< (IN) Control of enabling Exclusive ScissorRect per rect
    D3D11_RECT  scissorRect;                //!< (IN) Single rect dimensions
} NV_D3D11_EXCLUSIVE_SCISSOR_RECT_DESC_V1;

typedef struct _NV_D3D11_EXCLUSIVE_SCISSOR_RECTS_DESC_V1
{
    NvU32                                       version;    //!< (IN) Parameter struct version
    NvU32                                       numRects;   //!< (IN) Number of Exclusive Scissor Rects to be set.
                                                            //        \note Passing zero will globally disable Exclusive Scissor Rects
                                                            //        \note Max value can be equal to NV_MAX_NUM_EXCLUSIVE_SCISSOR_RECTS
    NV_D3D11_EXCLUSIVE_SCISSOR_RECT_DESC_V1    *pRects;     //!< (IN) Array of NV_D3D11_EXCLUSIVE_SCISSOR_RECT_DESC with number of elements equal to Exclusive Scissor Rects
} NV_D3D11_EXCLUSIVE_SCISSOR_RECTS_DESC_V1;

typedef NV_D3D11_EXCLUSIVE_SCISSOR_RECTS_DESC_V1     NV_D3D11_EXCLUSIVE_SCISSOR_RECTS_DESC;
typedef NV_D3D11_EXCLUSIVE_SCISSOR_RECT_DESC_V1      NV_D3D11_EXCLUSIVE_SCISSOR_RECT_DESC;
#define NV_D3D11_EXCLUSIVE_SCISSOR_RECTS_DESC_VER1   MAKE_NVAPI_VERSION(NV_D3D11_EXCLUSIVE_SCISSOR_RECTS_DESC_V1, 1)
#define NV_D3D11_EXCLUSIVE_SCISSOR_RECTS_DESC_VER    NV_D3D11_EXCLUSIVE_SCISSOR_RECTS_DESC_VER1

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_RSSetExclusiveScissorRects
//
//! DESCRIPTION: Sets Exclusive Scissor Rects. The content bounded within the Scissor Rects
//!              will be excluded from rendering unlike regular Scissor Rects. These are
//!              orthogonal with Regular Scissor Rects.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 410
//!
//! \param [in]        pContext                     The device context (ID3D11DeviceContext) to be used for setting the Exclusive Scissor Rects.
//! \param [in]        pExclusiveScissorRectsDesc   Description of the Exclusive Scissor Rects duly filled with their dimensions
//!                                                 and control over enablement of individual ScissorRect
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!         (none)
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_RSSetExclusiveScissorRects(__in IUnknown *pContext,
                                                       __in NV_D3D11_EXCLUSIVE_SCISSOR_RECTS_DESC *pExclusiveScissorRectsDesc);

#endif // defined(__cplusplus) && (defined(__d3d11_h__))

#if defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))
//! \ingroup dx
//! See NvAPI_D3D11_RSSetViewportsPixelShadingRates

#define NV_MAX_PIXEL_SHADING_RATES  16 // Currently only 12 Shading Rates are available
#define NV_MAX_NUM_VIEWPORTS        16

// Every element in Shading Rate Resource represents the shading rate for all pixels in the corresponding tile
// The Shading Rate Resource dimensions must be the bound render target size divided by the tile dimensions (width/height)

#define NV_VARIABLE_PIXEL_SHADING_TILE_WIDTH    16 // Width of the tile, in pixels
#define NV_VARIABLE_PIXEL_SHADING_TILE_HEIGHT   16 // Height of the tile, in pixels

typedef enum
{
    NV_PIXEL_X0_CULL_RASTER_PIXELS,         // No shading, tiles are culled
    NV_PIXEL_X16_PER_RASTER_PIXEL,          // 16 shading passes per 1 raster pixel
    NV_PIXEL_X8_PER_RASTER_PIXEL,           //  8 shading passes per 1 raster pixel
    NV_PIXEL_X4_PER_RASTER_PIXEL,           //  4 shading passes per 1 raster pixel
    NV_PIXEL_X2_PER_RASTER_PIXEL,           //  2 shading passes per 1 raster pixel
    NV_PIXEL_X1_PER_RASTER_PIXEL,           //  Per-pixel shading
    NV_PIXEL_X1_PER_2X1_RASTER_PIXELS,      //  1 shading pass per  2 raster pixels
    NV_PIXEL_X1_PER_1X2_RASTER_PIXELS,      //  1 shading pass per  2 raster pixels
    NV_PIXEL_X1_PER_2X2_RASTER_PIXELS,      //  1 shading pass per  4 raster pixels
    NV_PIXEL_X1_PER_4X2_RASTER_PIXELS,      //  1 shading pass per  8 raster pixels
    NV_PIXEL_X1_PER_2X4_RASTER_PIXELS,      //  1 shading pass per  8 raster pixels
    NV_PIXEL_X1_PER_4X4_RASTER_PIXELS       //  1 shading pass per 16 raster pixels
} NV_PIXEL_SHADING_RATE;
#endif // defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))

#if defined(__cplusplus) && (defined(__d3d11_h__))
typedef struct _NV_D3D11_VIEWPORT_SHADING_RATE_DESC_V1
{
    bool                    enableVariablePixelShadingRate;                         //!< (IN) Control of enabling Variable Pixel Shading Rate per viewport
    NV_PIXEL_SHADING_RATE   shadingRateTable[NV_MAX_PIXEL_SHADING_RATES];           //!< (IN) Lookup table of converting Shading Rate Index to NV_PIXEL_SHADING_RATE
                                                                                    //        \note Shading Rate Resource View would be populated by application with indices of this table
} NV_D3D11_VIEWPORT_SHADING_RATE_DESC_V1;

typedef struct _NV_D3D11_VIEWPORTS_SHADING_RATE_DESC_V1
{
    NvU32                                        version;       //!< (IN) Struct version
    NvU32                                        numViewports;  //!< (IN) Number of viewports with shading rate set.
                                                                //        \note Passing zero will globally disable Variable Pixel Rate Shading for all viewports immaterial of values in pViewports
                                                                //        \note Max value can be equal to NV_MAX_NUM_VIEWPORTS
    NV_D3D11_VIEWPORT_SHADING_RATE_DESC_V1      *pViewports;    //!< (IN) Array of NV_D3D11_VIEWPORT_SHADING_RATE_DESC with number of elements equal to NumViewports
} NV_D3D11_VIEWPORTS_SHADING_RATE_DESC_V1;

typedef NV_D3D11_VIEWPORTS_SHADING_RATE_DESC_V1     NV_D3D11_VIEWPORTS_SHADING_RATE_DESC;
typedef NV_D3D11_VIEWPORT_SHADING_RATE_DESC_V1      NV_D3D11_VIEWPORT_SHADING_RATE_DESC;
#define NV_D3D11_VIEWPORTS_SHADING_RATE_DESC_VER1   MAKE_NVAPI_VERSION(NV_D3D11_VIEWPORTS_SHADING_RATE_DESC_V1, 1)
#define NV_D3D11_VIEWPORTS_SHADING_RATE_DESC_VER    NV_D3D11_VIEWPORTS_SHADING_RATE_DESC_VER1

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_RSSetViewportsPixelShadingRates
//
//! DESCRIPTION: Sets Pixel Shading Rates and Enables/Disables per-viewport Variable Pixel Shading Rate feature
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 410
//!
//! \param [in]        pContext             The device context (ID3D11DeviceContext) to be used for setting the Viewports Shading Rates
//! \param [in]        pShadingRateDesc     Shading rate descriptor
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!         (none)
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_RSSetViewportsPixelShadingRates(__in IUnknown *pContext,
                                                            __in NV_D3D11_VIEWPORTS_SHADING_RATE_DESC *pShadingRateDesc);

#endif // defined(__cplusplus) && (defined(__d3d11_h__))

#if defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))

typedef enum _NV_SRRV_DIMENSION
{ 
    NV_SRRV_DIMENSION_TEXTURE2D           = 4,
    NV_SRRV_DIMENSION_TEXTURE2DARRAY      = 5,
} NV_SRRV_DIMENSION;

typedef struct _NV_TEX2D_SRRV
{
    UINT MipSlice;
} NV_TEX2D_SRRV;

typedef struct _NV_TEX2D_ARRAY_SRRV
{
    UINT MipSlice;
    UINT FirstArraySlice;
    UINT ArraySize;
} NV_TEX2D_ARRAY_SRRV;
#endif // defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))

#if defined(__cplusplus) && (defined(__d3d11_h__))
typedef struct _NV_D3D11_SHADING_RATE_RESOURCE_VIEW_DESC_V1
{
    NvU32               version;                //!< (IN) Parameter struct version
    DXGI_FORMAT         Format;                 //!< (IN) Format of the resource used as Shading Rate Surface. Should be either DXGI_FORMAT_R8_UINT or DXGI_FORMAT_R8_TYPELESS
    NV_SRRV_DIMENSION   ViewDimension;          //!< (IN) This declares whether the Shading Rate Surface is a simple 2D Texture or Array of 2D Textures
    union
    {
        NV_TEX2D_SRRV       Texture2D;
        NV_TEX2D_ARRAY_SRRV Texture2DArray;
    };
} NV_D3D11_SHADING_RATE_RESOURCE_VIEW_DESC_V1;

typedef NV_D3D11_SHADING_RATE_RESOURCE_VIEW_DESC_V1     NV_D3D11_SHADING_RATE_RESOURCE_VIEW_DESC;
#define NV_D3D11_SHADING_RATE_RESOURCE_VIEW_DESC_VER1   MAKE_NVAPI_VERSION(NV_D3D11_SHADING_RATE_RESOURCE_VIEW_DESC_V1, 1)
#define NV_D3D11_SHADING_RATE_RESOURCE_VIEW_DESC_VER    NV_D3D11_SHADING_RATE_RESOURCE_VIEW_DESC_VER1

DECLARE_INTERFACE_(__declspec(uuid("E14BE7F6-8FF5-4F5E-B63A-AD016EB8FBE5"))ID3D11NvShadingRateResourceView_V1, ID3D11View)
{
    BEGIN_INTERFACE

    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // **** ID3D11View method **/
    // Get Shading Rate Resource used while creating the Shading Rate Resource View
    STDMETHOD_(void,GetResource)(THIS_ _Outptr_ ID3D11Resource **ppResource) PURE;

    // ** ID3D11NvShadingRateResourceView methods ***
    // The descriptor used while creating the Shading Rate Resource View
    STDMETHOD(GetDesc)(THIS_ NV_D3D11_SHADING_RATE_RESOURCE_VIEW_DESC* pDesc) PURE;

    END_INTERFACE
};

typedef ID3D11NvShadingRateResourceView_V1    ID3D11NvShadingRateResourceView;
#define ID3D11NvShadingRateResourceView_VER1  MAKE_NVAPI_VERSION(ID3D11NvShadingRateResourceView_V1, 1)
#define ID3D11NvShadingRateResourceView_VER   ID3D11NvShadingRateResourceView_VER1

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_CreateShadingRateResourceView
//
//! DESCRIPTION: Creates Shading Rate Resource View by taking ID3D11Resource as an input Shading Rate Surface.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 410
//!
//! \param [in]        pDevice                      The device to be used for creating the Shading Rate Resource View
//! \param [in]        pShadingRateResource         Shading Rate Resource on which the view is to be created.
//!                                                 \note This should be of format DXGI_FORMAT_R8_UINT or DXGI_FORMAT_R8_TYPELESS
//!                                                 \note This should be confined to size calculated using render target dimensions,
//!                                                       NV_VARIABLE_PIXEL_SHADING_TILE_WIDTH and NV_VARIABLE_PIXEL_SHADING_TILE_HEIGHT
//! \param [in]        pShadingRateDesc             Shading Rate Resource View descriptor
//! \param [out]       ppShadingRateResourceView    Address of a pointer to ID3D11NvShadingRateResourceView for returning the newly created Shading Rate Resource View
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!         (none)
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_CreateShadingRateResourceView(__in  ID3D11Device *pDevice,
                                                          __in  ID3D11Resource *pShadingRateResource,
                                                          __in  NV_D3D11_SHADING_RATE_RESOURCE_VIEW_DESC *pShadingRateResourceViewDesc,
                                                          __out ID3D11NvShadingRateResourceView **ppShadingRateResourceView);

#endif // defined(__cplusplus) && (defined(__d3d11_h__))

#if defined(__cplusplus) && (defined(__d3d11_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_RSSetShadingRateResourceView
//
//! DESCRIPTION: Sets Shading Rate Resource View
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 410
//!
//! \param [in]        pContext                     The device context (ID3D11DeviceContext) used for setting the Shading Rate Resource View
//! \param [out]       pShadingRateResourceView     Shading Rate Resource View to be set
//!                                                 \note See NvAPI_D3D11_CreateShadingRateResourceView
//!                                                 \note Passing this as null will reset Shading Rate Resource View to defaults
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!         (none)
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_RSSetShadingRateResourceView(__in IUnknown *pContext,
                                                         __in ID3D11NvShadingRateResourceView *pShadingRateResourceView);

#endif // defined(__cplusplus) && (defined(__d3d11_h__))

#if defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))
//! \ingroup dx
//! See NvAPI_D3D11_RSGetPixelShadingRateSampleOrder
//! See NvAPI_D3D11_RSSetPixelShadingRateSampleOrder

// X, Y = sample position. S = sample number.
// The inner-most dimension is the sample number, followed by X and Y.

typedef struct _NV_PIXEL_SRSO_1x2
{
    struct NV_PIXEL_SRSO_1x2_X1 { NvU8 Y[2];         } X1;
    struct NV_PIXEL_SRSO_1x2_X2 { NvU8 YS[2][2];     } X2;
    struct NV_PIXEL_SRSO_1x2_X4 { NvU8 YS[2][4];     } X4;
    struct NV_PIXEL_SRSO_1x2_X8 { NvU8 YS[2][8];     } X8;
} NV_PIXEL_SRSO_1x2;

typedef struct _NV_PIXEL_SRSO_2x1
{
    struct NV_PIXEL_SRSO_2x1_X1 { NvU8 X[2];         } X1;
    struct NV_PIXEL_SRSO_2x1_X2 { NvU8 XS[2][2];     } X2;
    struct NV_PIXEL_SRSO_2x1_X4 { NvU8 XS[2][4];     } X4;
} NV_PIXEL_SRSO_2x1;

typedef struct _NV_PIXEL_SRSO_2x2
{
    struct NV_PIXEL_SRSO_2x2_X1 { NvU8 YX[2][2];     } X1;
    struct NV_PIXEL_SRSO_2x2_X2 { NvU8 YXS[2][2][2]; } X2;
    struct NV_PIXEL_SRSO_2x2_X4 { NvU8 YXS[2][2][4]; } X4;
} NV_PIXEL_SRSO_2x2;

typedef struct _NV_PIXEL_SRSO_2x4
{
    struct NV_PIXEL_SRSO_2x4_X1 { NvU8 YX[4][2];     } X1;
    struct NV_PIXEL_SRSO_2x4_X2 { NvU8 YXS[4][2][2]; } X2;
} NV_PIXEL_SRSO_2x4;

typedef struct _NV_PIXEL_SRSO_4x2
{
    struct NV_PIXEL_SRSO_4x2_X1 { NvU8 YX[2][4];     } X1;
} NV_PIXEL_SRSO_4x2;

typedef struct _NV_PIXEL_SRSO_4x4
{
    struct NV_PIXEL_SRSO_4x4_X1 { NvU8 YX[4][4];     } X1;
} NV_PIXEL_SRSO_4x4;

typedef struct _NV_PIXEL_SHADING_RATE_SAMPLE_ORDER_TABLE_V1
{
    NvU32 version;
    NV_PIXEL_SRSO_1x2 Pixel_1x2;
    NV_PIXEL_SRSO_2x1 Pixel_2x1;
    NV_PIXEL_SRSO_2x2 Pixel_2x2;
    NV_PIXEL_SRSO_2x4 Pixel_2x4;
    NV_PIXEL_SRSO_4x2 Pixel_4x2;
    NV_PIXEL_SRSO_4x4 Pixel_4x4;
} NV_PIXEL_SHADING_RATE_SAMPLE_ORDER_TABLE_V1;

typedef NV_PIXEL_SHADING_RATE_SAMPLE_ORDER_TABLE_V1     NV_PIXEL_SHADING_RATE_SAMPLE_ORDER_TABLE;
#define NV_PIXEL_SHADING_RATE_SAMPLE_ORDER_TABLE_VER1   MAKE_NVAPI_VERSION(NV_PIXEL_SHADING_RATE_SAMPLE_ORDER_TABLE_V1, 1)
#define NV_PIXEL_SHADING_RATE_SAMPLE_ORDER_TABLE_VER    NV_PIXEL_SHADING_RATE_SAMPLE_ORDER_TABLE_VER1

#endif // defined(__cplusplus) && (defined(__d3d11_h__) || defined(__d3d12_h__))

#if defined(__cplusplus) && (defined(__d3d11_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_RSGetPixelShadingRateSampleOrder
//
//! DESCRIPTION: Get the Sample Order for Variable Shading Rate
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 410
//!
//! \param [in]        pContext                     The device context (ID3D11DeviceContext) used for getting the Shading Rate Sample Order
//! \param [out]       pSampleOrderTable            A pointer to NV_PIXEL_SHADING_RATE_SAMPLE_ORDER_TABLE where the current Sample Order for Variable Pixel Rate Shading that is returned
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!         (none)
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_RSGetPixelShadingRateSampleOrder(__in IUnknown *pContext,
                                                             __out NV_PIXEL_SHADING_RATE_SAMPLE_ORDER_TABLE* pSampleOrderTable);

#endif // defined(__cplusplus) && (defined(__d3d11_h__))

#if defined(__cplusplus) && (defined(__d3d11_h__))

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D11_RSSetPixelShadingRateSampleOrder
//
//! DESCRIPTION: Set the Sample Order for Variable Shading Rate
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 410
//!
//! \param [in]        pContext                     The device context (ID3D11DeviceContext) used for setting the Shading Rate Sample Order
//! \param [out]       pSampleOrderTable            Sample Order for Variable Shading Rate to be set
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!         (none)
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D11_RSSetPixelShadingRateSampleOrder(__in IUnknown *pContext,
                                                             __in NV_PIXEL_SHADING_RATE_SAMPLE_ORDER_TABLE* pSampleOrderTable);

#endif // defined(__cplusplus) && (defined(__d3d11_h__))

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined(__cplusplus) && (defined(__d3d11_h__))
typedef struct _NV_VRS_HELPER_LATCH_GAZE_PARAMS_V1
{
    NvU32               version;    //!< (IN) Struct version
    NvU32               flags;      //!< (IN) Reserved for future use
} NV_VRS_HELPER_LATCH_GAZE_PARAMS_V1;

typedef NV_VRS_HELPER_LATCH_GAZE_PARAMS_V1     NV_VRS_HELPER_LATCH_GAZE_PARAMS;
#define NV_VRS_HELPER_LATCH_GAZE_PARAMS_VER1   MAKE_NVAPI_VERSION(NV_VRS_HELPER_LATCH_GAZE_PARAMS_V1, 1)
#define NV_VRS_HELPER_LATCH_GAZE_PARAMS_VER    NV_VRS_HELPER_LATCH_GAZE_PARAMS_VER1

typedef enum _NV_VRS_CONTENT_TYPE
{
    NV_VRS_CONTENT_TYPE_INVALID = 0x0,
    NV_VRS_CONTENT_TYPE_FOVEATED_RENDERING = 0x1,
    NV_VRS_CONTENT_TYPE_MAX = NV_VRS_CONTENT_TYPE_FOVEATED_RENDERING
} NV_VRS_CONTENT_TYPE;

typedef enum _NV_FOVEATED_RENDERING_SHADING_RATE_PRESET
{
    NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_INVALID = 0,
    NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGHEST_PERFORMANCE = 1,
    NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGH_PERFORMANCE = 2,
    NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_BALANCED = 3,
    NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGH_QUALITY = 4,
    NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_HIGHEST_QUALITY = 5,
    NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_CUSTOM = 6,
    NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_MAX = NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_CUSTOM
} NV_FOVEATED_RENDERING_SHADING_RATE_PRESET;

typedef struct _NV_FOVEATED_RENDERING_CUSTOM_SHADING_RATE_PRESET_DESC_V1
{
    NvU32 version;

    NV_PIXEL_SHADING_RATE InnerMostRegionShadingRate;       //!< (IN) Shading Rate for the inner-most region of the foveated rendering pattern
    NV_PIXEL_SHADING_RATE MiddleRegionShadingRate;          //!< (IN) Shading Rate for the middle region of the foveated rendering pattern
    NV_PIXEL_SHADING_RATE PeripheralRegionShadingRate;      //!< (IN) Shading Rate for the peripheral region of the foveated rendering pattern
} NV_FOVEATED_RENDERING_CUSTOM_SHADING_RATE_PRESET_DESC_V1;

typedef NV_FOVEATED_RENDERING_CUSTOM_SHADING_RATE_PRESET_DESC_V1     NV_FOVEATED_RENDERING_CUSTOM_SHADING_RATE_PRESET_DESC;
#define NV_FOVEATED_RENDERING_CUSTOM_SHADING_RATE_PRESET_DESC_VER1   MAKE_NVAPI_VERSION(NV_FOVEATED_RENDERING_CUSTOM_SHADING_RATE_PRESET_DESC_V1, 1)
#define NV_FOVEATED_RENDERING_CUSTOM_SHADING_RATE_PRESET_DESC_VER    NV_FOVEATED_RENDERING_CUSTOM_SHADING_RATE_PRESET_DESC_VER1

typedef enum _NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET
{
    NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_INVALID = 0,
    NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_WIDE = 1,
    NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_BALANCED = 2,
    NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_NARROW = 3,
    NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_CUSTOM = 4,
    NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_MAX = NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_CUSTOM
} NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET;

typedef struct _NV_FOVEATED_RENDERING_CUSTOM_FOVEATION_PATTERN_PRESET_DESC_V1
{
    NvU32 version;

    float fInnermostRadii[2];       //!< (IN) Horizontal and vertical radius for the inner-most region of the foveated rendering pattern
    float fMiddleRadii[2];          //!< (IN) Horizontal and vertical radius for the middle region of the foveated rendering pattern
    float fPeripheralRadii[2];      //!< (IN) Horizontal and vertical radius for the peripheral region of the foveated rendering pattern
} NV_FOVEATED_RENDERING_CUSTOM_FOVEATION_PATTERN_PRESET_DESC_V1;

typedef NV_FOVEATED_RENDERING_CUSTOM_FOVEATION_PATTERN_PRESET_DESC_V1     NV_FOVEATED_RENDERING_CUSTOM_FOVEATION_PATTERN_PRESET_DESC;
#define NV_FOVEATED_RENDERING_CUSTOM_FOVEATION_PATTERN_PRESET_DESC_VER1   MAKE_NVAPI_VERSION(NV_FOVEATED_RENDERING_CUSTOM_FOVEATION_PATTERN_PRESET_DESC_V1, 1)
#define NV_FOVEATED_RENDERING_CUSTOM_FOVEATION_PATTERN_PRESET_DESC_VER    NV_FOVEATED_RENDERING_CUSTOM_FOVEATION_PATTERN_PRESET_DESC_VER1

typedef struct _NV_FOVEATED_RENDERING_DESC_V1
{
    NvU32                                           version;  //!< (IN) Struct version
    NvU32                                           flags;    //!< (IN) Reserved for future use

    NV_FOVEATED_RENDERING_SHADING_RATE_PRESET                       ShadingRatePreset;                  //!< (IN) Preset of the shading rate
    NV_FOVEATED_RENDERING_CUSTOM_SHADING_RATE_PRESET_DESC_V1        ShadingRateCustomPresetDesc;        //!< (IN) To be provided only if ShadingRatePreset is NV_FOVEATED_RENDERING_SHADING_RATE_PRESET_CUSTOM

    NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET                  FoveationPatternPreset;             //!< (IN) Preset of the foveation pattern
    NV_FOVEATED_RENDERING_CUSTOM_FOVEATION_PATTERN_PRESET_DESC_V1   FoveationPatternCustomPresetDesc;   //!< (IN) To be provided only if FoveationPatternPreset is NV_FOVEATED_RENDERING_FOVEATION_PATTERN_PRESET_CUSTOM

    NvU32 GazeDataDeviceId;     //!< (IN) ID of the gaze data provider. Needed only for supporting more than one device with eye tracking.
                                //        Should be 0 if gaze data is provided only from a single device. Should be less than (MAX_NUMBER_OF_GAZE_DATA_PROVIDERS - 1)

} NV_FOVEATED_RENDERING_DESC_V1;

typedef NV_FOVEATED_RENDERING_DESC_V1     NV_FOVEATED_RENDERING_DESC;
#define NV_FOVEATED_RENDERING_DESC_VER1   MAKE_NVAPI_VERSION(NV_FOVEATED_RENDERING_DESC_V1, 1)
#define NV_FOVEATED_RENDERING_DESC_VER    NV_FOVEATED_RENDERING_DESC_VER1

typedef enum _NV_VRS_RENDER_MODE
{
    NV_VRS_RENDER_MODE_INVALID = 0,
    NV_VRS_RENDER_MODE_MONO = 1,            // States mono rendering on the entire render target
    NV_VRS_RENDER_MODE_LEFT_EYE = 2,        // States Left eye rendering of a stereo pair on the entire render target
    NV_VRS_RENDER_MODE_RIGHT_EYE = 3,       // States Right eye rendering of a stereo pair on the entire render target
    NV_VRS_RENDER_MODE_STEREO = 4,          // States side-by-side stereo rendering on the render target
    NV_VRS_RENDER_MODE_MAX = NV_VRS_RENDER_MODE_STEREO
} NV_VRS_RENDER_MODE;

#define MAX_NUMBER_OF_GAZE_DATA_PROVIDERS   8   // Maximum number of gaze data providers / devices.

typedef struct _NV_VRS_HELPER_ENABLE_PARAMS_V1
{
    NvU32                   version;        //!< (IN) Struct version
    NvU32                   flags;          //!< (IN) Reserved for future use

    NV_VRS_RENDER_MODE      RenderMode;     //!< (IN) This defines whether subsequent render calls are for mono/stereo
    NV_VRS_CONTENT_TYPE     ContentType;    //!< (IN) This defines the type of content with which the VRS pattern will be generated

    NV_FOVEATED_RENDERING_DESC_V1   sFoveatedRenderingDesc;     //!< (IN) Provide this if ContentType has NV_VRS_CONTENT_TYPE_FOVEATED_RENDERING flag
} NV_VRS_HELPER_ENABLE_PARAMS_V1;

typedef NV_VRS_HELPER_ENABLE_PARAMS_V1     NV_VRS_HELPER_ENABLE_PARAMS;
#define NV_VRS_HELPER_ENABLE_PARAMS_VER1   MAKE_NVAPI_VERSION(NV_VRS_HELPER_ENABLE_PARAMS_V1, 1)
#define NV_VRS_HELPER_ENABLE_PARAMS_VER    NV_VRS_HELPER_ENABLE_PARAMS_VER1

typedef struct _NV_VRS_HELPER_DISABLE_PARAMS_V1
{
    NvU32           version;    //!< (IN) Struct version
    NvU32           reserved;   //!< (IN) Reserved for future use
} NV_VRS_HELPER_DISABLE_PARAMS_V1;

typedef NV_VRS_HELPER_DISABLE_PARAMS_V1     NV_VRS_HELPER_DISABLE_PARAMS;
#define NV_VRS_HELPER_DISABLE_PARAMS_VER1   MAKE_NVAPI_VERSION(NV_VRS_HELPER_DISABLE_PARAMS_V1, 1)
#define NV_VRS_HELPER_DISABLE_PARAMS_VER    NV_VRS_HELPER_DISABLE_PARAMS_VER1

typedef struct _NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS_V1
{
    NvU32                   version;                                        //!< (IN)  Struct version
    IUnknown              **ppShadingRateResource;                          //!< (OUT) Pointer to 2D Texture resource with currently applied shading rate pattern
    NV_PIXEL_SHADING_RATE   shadingRateTable[NV_MAX_PIXEL_SHADING_RATES];   //!< (OUT) Shading Rate Table filled by the driver
} NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS_V1;

typedef NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS_V1     NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS;
#define NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS_VER1   MAKE_NVAPI_VERSION(NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS_V1, 1)
#define NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS_VER    NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS_VER1

typedef struct _NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS_V1
{
    NvU32           version;    //!< (IN) Struct version
    NvU32           reserved;   //!< (IN) Reserved for future use
} NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS_V1;

typedef NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS_V1     NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS;
#define NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS_VER1   MAKE_NVAPI_VERSION(NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS_V1, 1)
#define NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS_VER    NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS_VER1

DECLARE_INTERFACE(ID3DNvVRSHelper_V1)
{
    BEGIN_INTERFACE

    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // Latches the latest gaze which will be used for subsequent foveated rendering. Recommended to be called once per frame before scene drawing begins.
    STDMETHOD_(NvAPI_Status,LatchGaze)(THIS_ IUnknown* pContext, NV_VRS_HELPER_LATCH_GAZE_PARAMS* pLatchGazeParams) PURE;

    // Enables VRS with sepcified content type and preset. This can be called per draw call.
    STDMETHOD_(NvAPI_Status,Enable)(THIS_ IUnknown* pContext, NV_VRS_HELPER_ENABLE_PARAMS* pEnableParams) PURE;

    // Disables VRS till re-enabled.
    STDMETHOD_(NvAPI_Status,Disable)(THIS_ IUnknown* pContext, NV_VRS_HELPER_DISABLE_PARAMS* pDisableParams) PURE;

    // Creates a 2D texture, copies the current shading rate pattern on it and returns the pointer to this texture.
    // It also returns an array that conveys which value in the shading rate resource corresponds to which exact pixel shading rate.
    STDMETHOD_(NvAPI_Status,GetShadingRateResource)(THIS_ IUnknown* pContext, NV_VRS_HELPER_GET_SHADING_RATE_RESOURCE_PARAMS *pGetShadingRateResourceParams) PURE;

    // Destroys all internally created shading rate resources and views.
    STDMETHOD_(NvAPI_Status,PurgeInternalShadingRateResources)(THIS_ IUnknown* pContext, NV_VRS_HELPER_PURGE_INTERNAL_RESOURCES_PARAMS* pPurgeParams) PURE;

    END_INTERFACE
};

typedef ID3DNvVRSHelper_V1    ID3DNvVRSHelper;
#define ID3DNvVRSHelper_VER1  MAKE_NVAPI_VERSION(ID3DNvVRSHelper_V1, 1)
#define ID3DNvVRSHelper_VER   ID3DNvVRSHelper_VER1

typedef struct _NV_VRS_HELPER_INIT_PARAMS_V1
{
    NvU32           version;    //!< (IN) Struct version
    NvU32           flags;      //!< (IN) Reserved for future use

    ID3DNvVRSHelper_V1    **ppVRSHelper;  //!< (OUT) Interface for Shading Rate Pattern Tracker
} NV_VRS_HELPER_INIT_PARAMS_V1;

typedef NV_VRS_HELPER_INIT_PARAMS_V1     NV_VRS_HELPER_INIT_PARAMS;
#define NV_VRS_HELPER_INIT_PARAMS_VER1   MAKE_NVAPI_VERSION(NV_VRS_HELPER_INIT_PARAMS_V1, 1)
#define NV_VRS_HELPER_INIT_PARAMS_VER    NV_VRS_HELPER_INIT_PARAMS_VER1

//! SUPPORTED OS:  Windows 7 and higher
//!
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_InitializeVRSHelper
//
//! DESCRIPTION: Creates an interface for updating, enabling and disabling internally tracked shading rate pattern for Variable Rate Shading
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 430
//!
//! \param [in]        pDevice                              The device to be used for creating the VRS Handler interface
//!                                                         \note This should be same the device used for Gaze Handler. See also: NvAPI_D3D_InitializeNvGazeHandler.
//! \param [in]        pInitializeVRSHelperParams           Descriptor for VRS Helper initialization
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!         (none)
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D_InitializeVRSHelper(__in    IUnknown *pDevice,
                                              __inout NV_VRS_HELPER_INIT_PARAMS *pInitializeVRSHelperParams);

typedef enum _NV_GAZE_DATA_VALIDITY_FLAGS
{
    NV_GAZE_ORIGIN_VALID            = 0x1,
    NV_GAZE_DIRECTION_VALID         = 0x2,
    NV_GAZE_LOCATION_VALID          = 0x4,
    NV_GAZE_VELOCITY_VALID          = 0x8,
    NV_GAZE_PUPIL_DIAMETER_VALID    = 0x10,
    NV_GAZE_EYE_OPENNESS_VALID      = 0x20,
    NV_GAZE_EYE_SACCADE_DATA_VALID  = 0x40
} NV_GAZE_DATA_VALIDITY_FLAGS;

typedef struct _NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE
{
    NvU32 version;                          //!< (IN) Version of the structure

    NvU32 GazeDataValidityFlags;            //!< (IN) To be populated with OR'ing flags from NV_GAZE_DATA_VALIDITY_FLAGS

    float fGazeOrigin_mm[3];                //!< (IN) Use flag NV_GAZE_ORIGIN_VALID. Origin of the eye in millimeters. Used mainly to detect whether Left Eye or Right Eye.
    float fGazeDirection[3];                //!< (IN) Use flag NV_GAZE_DIRECTION_VALID. Normalized direction of the gaze of the eye. Used for calculating the gaze location using the FOV.
    float fGazeNormalizedLocation[2];       //!< (IN) Use flag NV_GAZE_LOCATION_VALID. Precalculated normalized gaze location in limits (-1 to +1) for X and Y. Center of the screen denotes (0, 0). If this is valid, this will be given higher priority than direction.
    float fGazeVelocity[2];                 //!< (IN) Use flag NV_GAZE_VELOCITY_VALID. Optional: Velocity of the eye on the normalized space in each direction. Central foveated region would be skewed in the direction of the velocity.
    float fPupilDiameter_mm;                //!< (IN) Use flag NV_GAZE_PUPIL_DIAMETER_VALID. Unused at the moment.
    float fEyeOpenness;                     //!< (IN) Use flag NV_GAZE_EYE_OPENNESS_VALID. Unused at the moment.
    BOOL  bInSaccade;                       //!< (IN) Use flag NV_GAZE_EYE_SACCADE_DATA_VALID. Denotes whether eye is currently in saccade movement or not.
} NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_V1;

typedef NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_V1     NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE;
#define NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_VER1   MAKE_NVAPI_VERSION(NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_V1, 1)
#define NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_VER    NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_VER1

typedef struct _NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS
{
    NvU32 version;              //!< (IN) Struct version
    NvU32 flags;                //!< (IN) Reserved for future use

    NvU64 Timestamp;            //!< (IN) Timestamp at which the gaze data has been captured. Should be larger than timestamp provided at previous update.
    union
    {
        NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_V1 sMonoData;           //!< (IN) Gaze data for Mono rendering mode
        struct
        {
            NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_V1 sLeftEye;        //!< (IN) Gaze data for Left Eye of Stereo rendering mode
            NV_FOVEATED_RENDERING_GAZE_DATA_PER_EYE_V1 sRightEye;       //!< (IN) Gaze data for Right Eye of Stereo rendering mode
        } sStereoData;
    };
} NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS_V1;

typedef NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS_V1     NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS;
#define NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS_VER1   MAKE_NVAPI_VERSION(NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS_V1, 1)
#define NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS_VER    NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS_VER1

DECLARE_INTERFACE(ID3DNvGazeHandler_V1)
{
    BEGIN_INTERFACE

    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // Updates the gaze data for foveated rendering
    STDMETHOD_(NvAPI_Status,UpdateGazeData)(THIS_ IUnknown* pContext, NV_FOVEATED_RENDERING_UPDATE_GAZE_DATA_PARAMS* pUpdateGazeDataParams) PURE;

    END_INTERFACE
};

typedef ID3DNvGazeHandler_V1    ID3DNvGazeHandler;
#define ID3DNvGazeHandler_VER1  MAKE_NVAPI_VERSION(ID3DNvGazeHandler_V1, 1)
#define ID3DNvGazeHandler_VER   ID3DNvGazeHandler_VER1

typedef enum _NV_GAZE_DATA_TYPE
{
    NV_GAZE_DATA_INVALID = 0,
    NV_GAZE_DATA_MONO = 1,
    NV_GAZE_DATA_STEREO = 2,
    NV_GAZE_DATA_MAX = NV_GAZE_DATA_STEREO
} NV_GAZE_DATA_TYPE;

typedef struct _NV_GAZE_HANDLER_INIT_PARAMS_V1
{
    NvU32               version;            //!< (IN) Struct version
    
    NvU32               GazeDataDeviceId;   //!< (IN) ID of the gaze data provider. Needed only for supporting more than one device with eye tracking.
                                            //        Should be 0 if gaze data is provided only from a single device. Should be less than (MAX_NUMBER_OF_GAZE_DATA_PROVIDERS - 1)

    NV_GAZE_DATA_TYPE   GazeDataType;       //!< (IN) Describes whether gaze is Mono or Stereo
    NvU32               flags;              //!< (IN) Reserved for future use
    float               fHorizontalFOV;     //!< (IN) Horizontal Field of View
    float               fVericalFOV;        //!< (IN) Vertical Field of View

    ID3DNvGazeHandler_V1    **ppNvGazeHandler;    //!< (OUT) Interface for Gaze Data Handler
} NV_GAZE_HANDLER_INIT_PARAMS_V1;

typedef NV_GAZE_HANDLER_INIT_PARAMS_V1     NV_GAZE_HANDLER_INIT_PARAMS;
#define NV_GAZE_HANDLER_INIT_PARAMS_VER1   MAKE_NVAPI_VERSION(NV_GAZE_HANDLER_INIT_PARAMS_V1, 1)
#define NV_GAZE_HANDLER_INIT_PARAMS_VER    NV_GAZE_HANDLER_INIT_PARAMS_VER1

#endif // defined(__cplusplus) && (defined(__d3d11_h__))

//! SUPPORTED OS:  Windows 7 and higher
//!

#if defined(__cplusplus) && (defined(__d3d11_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_InitializeNvGazeHandler
//
//! DESCRIPTION: Creates an interface for updating and managing gaze data
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 430
//!
//! \param [in]        pDevice                          The device to be used for creating the Gaze Handler interface
//!                                                     \note This should be same as the device used with VRS Handler. See also: NvAPI_D3D_InitializeVRSHelper.
//! \param [in]        pInitializeNvGazeHandlerParams   Descriptor for Gaze Data Handler initialization
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!         (none)
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////

NVAPI_INTERFACE NvAPI_D3D_InitializeNvGazeHandler(__in    IUnknown *pDevice,
                                                  __inout NV_GAZE_HANDLER_INIT_PARAMS *pInitializeNvGazeHandlerParams);

#endif // defined(__cplusplus) && (defined(__d3d11_h__))

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && defined(__d3d11_h__) 

//! \ingroup dx

typedef enum NV_SMP_ASSIST_TYPE
{
    NV_SMP_ASSIST_NONE = 0,
    NV_SMP_ASSIST_MRS  = 1,       // SMP Assist required for Multi-Res Shading
    NV_SMP_ASSIST_LMS  = 2,       // SMP Assist required for Lens Matched Shading
    NV_SMP_ASSIST_NUM_TYPES
} NV_SMP_ASSIST_TYPE;

typedef enum NV_SMP_ASSIST_LEVEL
{
    NV_SMP_ASSIST_LEVEL_FULL    = 0,      // Full assistance. App selects a pre-baked MRS/LMS config, driver handles correct setting of viewport, scissors and FastGS
    NV_SMP_ASSIST_LEVEL_PARTIAL = 1,      // Partial assistance. App provides a custom MRS/LMS config, driver handles correct setting of viewport, scissors and FastGS
    NV_SMP_ASSIST_LEVEL_MINIMAL = 2,      // Minimal assistance. App provides viewports and scissors. App sets FastGS as required. App sets LMS params as required (NvAPI_D3D_SetModifiedWMode). App provides SMPType as NONE. Driver handles correct setting of viewports and scissors.
    NV_SMP_ASSIST_NUM_LEVELS
} NV_SMP_ASSIST_LEVEL;

typedef enum NV_MRS_CONFIG
{
    NV_MRS_CONFIG_BALANCED                     = 0,
    NV_MRS_CONFIG_AGGRESSIVE                   = 1,
    NV_MRS_CONFIG_OCULUSRIFT_CV1_CONSERVATIVE  = 2,
    NV_MRS_CONFIG_OCULUSRIFT_CV1_BALANCED      = 3,
    NV_MRS_CONFIG_OCULUSRIFT_CV1_AGGRESSIVE    = 4,
    NV_MRS_CONFIG_HTC_VIVE_CONSERVATIVE        = 5,
    NV_MRS_CONFIG_HTC_VIVE_BALANCED            = 6,
    NV_MRS_CONFIG_HTC_VIVE_AGGRESSIVE          = 7,
    NV_MRS_NUM_CONFIGS
} NV_MRS_CONFIG;

typedef enum NV_LMS_CONFIG
{
    NV_LMS_CONFIG_OCULUSRIFT_CV1_CONSERVATIVE = 0,
    NV_LMS_CONFIG_OCULUSRIFT_CV1_BALANCED     = 1,
    NV_LMS_CONFIG_OCULUSRIFT_CV1_AGGRESSIVE   = 2,
    NV_LMS_CONFIG_HTC_VIVE_CONSERVATIVE       = 3,
    NV_LMS_CONFIG_HTC_VIVE_BALANCED           = 4,
    NV_LMS_CONFIG_HTC_VIVE_AGGRESSIVE         = 5,
    NV_LMS_NUM_CONFIGS
} NV_LMS_CONFIG;

#define NV_SMP_ASSIST_FLAGS_DEFAULT    0x00000000

#define NV_SMP_ASSIST_MAX_VIEWPORTS    16

typedef struct _NV_MRS_CUSTOM_CONFIG_V1
{
    float           centerWidth;        //!< (IN) Size of the central viewport, ranging (0,1], where 1 is full original viewport size
    float           centerHeight;
    float           centerX;            //!< (IN) Location of the central viewport, ranging 0..1, where 0.5 is the center of the screen
    float           centerY;
    float           densityScaleX[3];   //!< (IN) Pixel density scale factors: how much the linear pixel density is scaled within each row and column (1.0 = full density)
    float           densityScaleY[3];
} NV_MRS_CUSTOM_CONFIG_V1;

typedef NV_MRS_CUSTOM_CONFIG_V1 NV_MRS_CUSTOM_CONFIG;

typedef struct _NV_MRS_INSTANCED_STEREO_CONFIG_V1
{                                          //!< (OUT) MRS Instanced stereo config returned by the SMP Assist GetConstants API
    float           centerWidth[2];        //!< (OUT) Size of the central viewport, ranging (0,1], where 1 is full original viewport size
    float           centerHeight;
    float           centerX[2];            //!< (OUT) Location of the central viewport, ranging 0..1, where 0.5 is the center of the screen
    float           centerY;
    float           densityScaleX[5];      //!< (OUT) Pixel density scale factors: how much the linear pixel density is scaled within each row and column (1.0 = full density)
    float           densityScaleY[3];
} NV_MRS_INSTANCED_STEREO_CONFIG_V1;

typedef NV_MRS_INSTANCED_STEREO_CONFIG_V1 NV_MRS_INSTANCED_STEREO_CONFIG;

typedef struct _NV_LMS_CUSTOM_CONFIG_V1
{
    float           warpLeft;           //!< (IN) LMS params to control warping of the 2 left quadrants
    float           warpRight;          //!< (IN) LMS params to control warping of the 2 right quadrants
    float           warpUp;             //!< (IN) LMS params to control warping of the 2 upper quadrants
    float           warpDown;           //!< (IN) LMS params to control warping of the 2 lower quadrants

    float           relativeSizeLeft;   //!< (IN) LMS params to control the width of the 2 left quandrants relative to the bounding box width
    float           relativeSizeRight;  //!< (IN) LMS params to control the width of the 2 right quandrants relative to the bounding box width
    float           relativeSizeUp;     //!< (IN) LMS params to control the height of the 2 upper quandrants relative to the bounding box height
    float           relativeSizeDown;   //!< (IN) LMS params to control the height of the 2 lower quandrants relative to the bounding box height
} NV_LMS_CUSTOM_CONFIG_V1;

typedef NV_LMS_CUSTOM_CONFIG_V1 NV_LMS_CUSTOM_CONFIG;

typedef struct _NV_LMS_INSTANCED_STEREO_CONFIG_V1
{                                             //!< (OUT) LMS Instanced stereo config returned by the SMP Assist GetConstants API
    NV_LMS_CUSTOM_CONFIG_V1 sLeftConfig;      //!< (OUT) LMS config for the Left eye view
    NV_LMS_CUSTOM_CONFIG_V1 sRightConfig;     //!< (OUT) LMS config for the Right eye view
} NV_LMS_INSTANCED_STEREO_CONFIG_V1;

typedef NV_LMS_INSTANCED_STEREO_CONFIG_V1 NV_LMS_INSTANCED_STEREO_CONFIG;

typedef enum _NV_SMP_ASSIST_EYE_INDEX
{
    NV_SMP_ASSIST_EYE_INDEX_MONO                  = 0,            // Non-stereo rendering
    NV_SMP_ASSIST_EYE_INDEX_LEFT_EYE              = 1,            // Stereo - Rendering left eye
    NV_SMP_ASSIST_EYE_INDEX_RIGHT_EYE             = 2,            // Stereo - Rendering right eye
    NV_SMP_ASSIST_EYE_INDEX_INSTANCED_STEREO      = 3,            // Stereo - Rendering both eyes
} NV_SMP_ASSIST_EYE_INDEX;

#define NV_SMP_ASSIST_MINIMAL_LEVEL_NUM_EYE_INDICES 4 

typedef struct _NV_CUSTOM_RECTS_V1
{
    NvU32                     numViewports[NV_SMP_ASSIST_MINIMAL_LEVEL_NUM_EYE_INDICES];
    D3D11_VIEWPORT           *pViewports[NV_SMP_ASSIST_MINIMAL_LEVEL_NUM_EYE_INDICES];        //!< (IN) Viewports, for each eye index, that should be set when app calls Enable(eyeIndex) 
    D3D11_RECT               *pScissors[NV_SMP_ASSIST_MINIMAL_LEVEL_NUM_EYE_INDICES];         //!< (IN) Scissors, for each eye index, that should be set when app calls Enable(eyeIndex)
} NV_CUSTOM_RECTS_V1;

typedef NV_CUSTOM_RECTS_V1 NV_CUSTOM_RECTS;

typedef struct _NV_SMP_ASSIST_ENABLE_PARAMS_V1
{
    NvU32                    version;          //!< (IN) Structure version
    NV_SMP_ASSIST_EYE_INDEX  eEyeIndex;        //!< (IN) Rendering mode for upcoming draw calls (Mono/Stereo-Left/Stereo-Right/Instanced Stereo)
} NV_SMP_ASSIST_ENABLE_PARAMS_V1;

typedef NV_SMP_ASSIST_ENABLE_PARAMS_V1          NV_SMP_ASSIST_ENABLE_PARAMS;
#define NV_SMP_ASSIST_ENABLE_PARAMS_VER1        MAKE_NVAPI_VERSION(NV_SMP_ASSIST_ENABLE_PARAMS_V1, 1)
#define NV_SMP_ASSIST_ENABLE_PARAMS_VER         NV_SMP_ASSIST_ENABLE_PARAMS_VER1

typedef struct _NV_SMP_ASSIST_DISABLE_PARAMS_V1
{
    NvU32                version;               //!< (IN) Structure version
    NvU32                Reserved;              //!< (IN) Unused.
} NV_SMP_ASSIST_DISABLE_PARAMS_V1;

typedef NV_SMP_ASSIST_DISABLE_PARAMS_V1          NV_SMP_ASSIST_DISABLE_PARAMS;
#define NV_SMP_ASSIST_DISABLE_PARAMS_VER1        MAKE_NVAPI_VERSION(NV_SMP_ASSIST_DISABLE_PARAMS_V1, 1)
#define NV_SMP_ASSIST_DISABLE_PARAMS_VER         NV_SMP_ASSIST_DISABLE_PARAMS_VER1

// FastGS constant buffer data returned by the GetConstants API.
// Refer VRWorks SDK's multiprojection_dx app (struct FastGSCBData)
typedef struct _NV_SMP_ASSIST_FASTGSCBDATA_V1
{
    float NDCSplitsX[2];                        //!< (OUT) FastGS constant buffer data for Multi-Res Shading/ Lens Matched Shading
    float NDCSplitsY[2];
} NV_SMP_ASSIST_FASTGSCBDATA_V1;

typedef NV_SMP_ASSIST_FASTGSCBDATA_V1 NV_SMP_ASSIST_FASTGSCBDATA;

typedef struct _NV_SMP_ASSIST_FASTGSCBDATA_MRS_INSTANCED_STEREO_V1
{
    float NDCSplitsX[4];                        //!< (OUT) FastGS constant buffer data for Multi-Res Shading (Instanced stereo). 2 splits for left eye followed by 2 splits for right eye
    float NDCSplitsY[2];
} NV_SMP_ASSIST_FASTGSCBDATA_MRS_INSTANCED_STEREO_V1;

typedef NV_SMP_ASSIST_FASTGSCBDATA_MRS_INSTANCED_STEREO_V1 NV_SMP_ASSIST_FASTGSCBDATA_MRS_INSTANCED_STEREO;

// Constant buffer data to supply the UV-remapping helper functions
// Refer VRWorks SDK's multiprojection_dx app (struct RemapCBData)
typedef struct _NV_SMP_ASSIST_REMAPCBDATA_V1
{
                                                 //!< (OUT) Constant buffer data to supply the UV-remapping helper functions
    float        ClipToWindowSplitsX[2];
    float        ClipToWindowSplitsY[2];
    float        ClipToWindowX[3][2];            // ClipToWindowX[i][0] is Scale and ClipToWindowX[i][1] is Bias
    float        ClipToWindowY[3][2];            // ClipToWindowY[i][0] is Scale and ClipToWindowY[i][1] is Bias
    float        ClipToWindowZ[2];               // ClipToWindowZ[0] is Scale and ClipToWindowZ[1] is Bias

    float        WindowToClipSplitsX[2];
    float        WindowToClipSplitsY[2];
    float        WindowToClipX[3][2];            // WindowToClipX[i][0] is Scale and WindowToClipX[i][1] is Bias
    float        WindowToClipY[3][2];            // WindowToClipY[i][0] is Scale and WindowToClipY[i][1] is Bias
    float        WindowToClipZ[2];               // WindowToClipZ[0] is Scale and WindowToClipZ[1] is Bias

    float       BoundingRectOriginX;
    float       BoundingRectOriginY;
    float       BoundingRectSizeWidth;
    float       BoundingRectSizeHeight;
    float       BoundingRectSizeInvWidth;
    float       BoundingRectSizeInvHeight;

    float        Padding[2];
}NV_SMP_ASSIST_REMAPCBDATA_V1;

typedef NV_SMP_ASSIST_REMAPCBDATA_V1 NV_SMP_ASSIST_REMAPCBDATA;

//! SUPPORTED OS:  Windows 7 and higher
//!

typedef struct _NV_SMP_ASSIST_GET_CONSTANTS_V3
{
    NvU32                    version;                                        //!< (IN) Structure version
    NV_SMP_ASSIST_EYE_INDEX  eEyeIndex;                                      //!< (IN) Viewports/scissors/constant buffer data corresponding to the input eEyeIndex will be returned
    NvU32                    numViewports;                                   //!< (OUT) Number of valid viewport entries
    D3D11_VIEWPORT           *pViewports;                                    //!< (OUT) If not NULL, this will contain the viewports computed by driver based on init params
    D3D11_RECT               *pScissors;                                     //!< (OUT) If not NULL, this will contain the scissors computed by the driver based on init params
                                                                             
    NV_SMP_ASSIST_TYPE       eSMPAssistType;                                 //!< (OUT) SMP type provided in Init call
    NV_SMP_ASSIST_LEVEL      eSMPAssistLevel;                                //!< (OUT) SMP Assist level provided in Init call
                                                                             
    union                                                                    
    {                                                                        
        NV_MRS_CUSTOM_CONFIG_V1 sMRSConfig;                                  //!< (OUT) If eSMPAssistType is MRS, then MRS config will be populated
        NV_LMS_CUSTOM_CONFIG_V1 sLMSConfig;                                  //!< (OUT) If eSMPAssistType is LMS, then LMS config will be populated
    };                                                                       
                                                                             
    float projectionSizeWidth;                                               //!< (OUT) MRS/LMS projection size
    float projectionSizeHeight;                                              //!< (OUT) 
                                                                             
    NV_SMP_ASSIST_FASTGSCBDATA_V1 *pFastGSCBData;                            //!< (OUT) If not NULL, this will contain constant buffer data to supply the FastGS for culling primitives per-viewport
    NV_SMP_ASSIST_REMAPCBDATA_V1  *pRemapCBData;                             //!< (OUT) If not NULL, this will contain constant buffer data to supply the UV-remapping helper functions
                                                                             
    D3D11_VIEWPORT          boundingViewport;                                //!< (OUT) If eSMPType is MRS or LMS then this will be a union of the individual viewports populated in pViewports
    D3D11_RECT              boundingScissor;                                 //!< (OUT) If eSMPType is MRS or LMS then this will be a union of the individual scissor rects populated in pScissors

    union
    {                                                                             
        NV_MRS_INSTANCED_STEREO_CONFIG_V1 sMRS_ISConfig;                     //!< (OUT) If eSMPAssistType is MRS and eEyeIndex is NV_SMP_ASSIST_EYE_INDEX_INSTANCED_STEREO then MRS Instanced stereo config will be populated
        NV_LMS_INSTANCED_STEREO_CONFIG_V1 sLMS_ISConfig;                     //!< (OUT) If eSMPAssistType is LMS and eEyeIndex is NV_SMP_ASSIST_EYE_INDEX_INSTANCED_STEREO then LMS Instanced stereo config will be populated
    };

    NV_SMP_ASSIST_FASTGSCBDATA_MRS_INSTANCED_STEREO_V1 *pFastGSCBDataMRS_IS; //!< (OUT) If non-NULL and eSMPAssistType is MRS and eEyeIndex is NV_SMP_ASSIST_EYE_INDEX_INSTANCED_STEREO then MRS Instanced stereo FastGS constant buffer data will be populated
} NV_SMP_ASSIST_GET_CONSTANTS_V3;

#define NV_SMP_ASSIST_GET_CONSTANTS_VER3        MAKE_NVAPI_VERSION(NV_SMP_ASSIST_GET_CONSTANTS_V3, 3)
typedef NV_SMP_ASSIST_GET_CONSTANTS_V3          NV_SMP_ASSIST_GET_CONSTANTS;
#define NV_SMP_ASSIST_GET_CONSTANTS_VER         NV_SMP_ASSIST_GET_CONSTANTS_VER3

//! SUPPORTED OS:  Windows 7 and higher
//!

typedef struct _NV_SMP_ASSIST_SETUP_PARAMS_V1
{
    NvU32                       version;             //!< (IN) Structure version
    union
    {
        NV_MRS_CONFIG           eMRSConfig;          //!< (IN) If eSMPAssistType is MRS and SMP Assist Level is Full then provide MRS config enum
        NV_LMS_CONFIG           eLMSConfig;          //!< (IN) If eSMPAssistType is LMS and SMP Assist Level is Full then provide LMS config enum
        NV_MRS_CUSTOM_CONFIG_V1 sMRSCustomConfig;    //!< (IN) If eSMPAssistType is MRS and SMP Assist Level is Partial, then provide MRS config
        NV_LMS_CUSTOM_CONFIG_V1 sLMSCustomConfig;    //!< (IN) If eSMPAssistType is LMS and SMP Assist Level is Partial, then provide LMS config
        NV_CUSTOM_RECTS_V1      sCustomRects;        //!< (IN) If SMP Assist Level is Minimal, provide custom viewports and scissor rects for each eye index.
    };

    float           resolutionScale;                 //!< (IN) A resolution multiplier in the range [0.1, 3.0] if app wants to render at higher resolution
    D3D11_VIEWPORT  boundingBox;                     //!< (IN) Rect on the rendertarget, to place the projection
    float           vpOffsets[2];                    //!< (IN) Default set to 0. If non-zero, MRS/LMS viewports' TopLeftX and TopLeftY will be 
                                                     //!<      offset by vpOffsets[0] and vpOffsets[1] respectively.
} NV_SMP_ASSIST_SETUP_PARAMS_V1;

typedef NV_SMP_ASSIST_SETUP_PARAMS_V1          NV_SMP_ASSIST_SETUP_PARAMS;
#define NV_SMP_ASSIST_SETUP_PARAMS_VER1        MAKE_NVAPI_VERSION(NV_SMP_ASSIST_SETUP_PARAMS_V1, 1)
#define NV_SMP_ASSIST_SETUP_PARAMS_VER         NV_SMP_ASSIST_SETUP_PARAMS_VER1

typedef struct _NV_SMP_ASSIST_UPDATE_INSTANCEDSTEREO_DATA_PARAMS_V1
{
    NvU32                version;               //!< (IN) Structure version
    NV_SMP_ASSIST_TYPE   eSMPAssistType;        // Patch instanced stereo shaders (created with packed eye index) with this SMPAssistType (NV_SMP_ASSIST_LMS only)
    float                leftCoeffs[4];         // Left eye: outpos.x = dotproduct(outputpos, leftCoeffs) + leftConst
    float                leftConst;
    float                rightCoeffs[4];        // Right eye: outpos.x = dotproduct(outputpos, rightCoeffs) + rightConst
    float                rightConst;
} NV_SMP_ASSIST_UPDATE_INSTANCEDSTEREO_DATA_PARAMS_V1;

typedef NV_SMP_ASSIST_UPDATE_INSTANCEDSTEREO_DATA_PARAMS_V1          NV_SMP_ASSIST_UPDATE_INSTANCEDSTEREO_DATA_PARAMS;
#define NV_SMP_ASSIST_UPDATE_INSTANCEDSTEREO_DATA_PARAMS_VER1        MAKE_NVAPI_VERSION(NV_SMP_ASSIST_UPDATE_INSTANCEDSTEREO_DATA_PARAMS_V1, 1)
#define NV_SMP_ASSIST_UPDATE_INSTANCEDSTEREO_DATA_PARAMS_VER         NV_SMP_ASSIST_UPDATE_INSTANCEDSTEREO_DATA_PARAMS_VER1

//! SUPPORTED OS:  Windows 7 and higher
//!

DECLARE_INTERFACE(ID3DNvSMPAssist_V1)
{
//////////////////////////////   VER1 methods //////////////////////////////////////////
    // Disable SMP Assist for further Draw calls
    STDMETHOD_(NvAPI_Status,Disable)(THIS_ __in IUnknown *pDevContext, __in const NV_SMP_ASSIST_DISABLE_PARAMS *psSMPAssistDisableParams) PURE;

    // Enable SMP Assist for further Draw calls. App has to provide the type of rendering done in upcoming Draw calls - Mono/Left eye/Right eye/Instanced Stereo
    STDMETHOD_(NvAPI_Status,Enable)(THIS_ __in IUnknown *pDevContext, __in const NV_SMP_ASSIST_ENABLE_PARAMS *psSMPAssistEnableParams) PURE; 

    // Get the constants used by the drivers
    STDMETHOD_(NvAPI_Status,GetConstants)(THIS_ __inout NV_SMP_ASSIST_GET_CONSTANTS *psSMPAssistGetConstants) PURE; 

    // Setup the projections (rects, constant buffer data etc.)
    STDMETHOD_(NvAPI_Status,SetupProjections)(THIS_ __in IUnknown *pDevice, __in const NV_SMP_ASSIST_SETUP_PARAMS *psSMPAssistSetupParams) PURE;

    // Update instanced stereo specific data
    STDMETHOD_(NvAPI_Status,UpdateInstancedStereoData)(THIS_ __in IUnknown *pDevice, __in const NV_SMP_ASSIST_UPDATE_INSTANCEDSTEREO_DATA_PARAMS *psSMPAssistInstancedStereoParams) PURE;
//////////////////////////////   end of VER1 methods   //////////////////////////////////////////
};

typedef ID3DNvSMPAssist_V1     ID3DNvSMPAssist;
#define ID3DNVSMPASSIST_VER1   MAKE_NVAPI_VERSION(ID3DNvSMPAssist_V1, 1)
#define ID3DNVSMPASSIST_VER    ID3DNVSMPASSIST_VER1

//! SUPPORTED OS:  Windows 7 and higher
//!

typedef struct _NV_SMP_ASSIST_INITIALIZE_PARAMS_V1
{
    NvU32                   version;                 //!< (IN) Structure version
    NV_SMP_ASSIST_TYPE      eSMPAssistType;          //!< (IN) None/MRS/LMS
    NV_SMP_ASSIST_LEVEL     eSMPAssistLevel;         //!< (IN) Full/Partial/Minimal 
    NvU32                   flags;                   //!< (IN) Flags, if any

    ID3DNvSMPAssist     **ppD3DNvSMPAssist;          //!< (OUT) Interface pointer returned by the Init call. Use for future Enable/Disable etc. calls

} NV_SMP_ASSIST_INITIALIZE_PARAMS_V1;

#define NV_SMP_ASSIST_INITIALIZE_PARAMS_VER1        MAKE_NVAPI_VERSION(NV_SMP_ASSIST_INITIALIZE_PARAMS_V1, 1)
#ifndef NV_SMP_ASSIST_INITIALIZE_PARAMS_VER
typedef NV_SMP_ASSIST_INITIALIZE_PARAMS_V1          NV_SMP_ASSIST_INITIALIZE_PARAMS;
#define NV_SMP_ASSIST_INITIALIZE_PARAMS_VER         NV_SMP_ASSIST_INITIALIZE_PARAMS_VER1
#endif

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_InitializeSMPAssist
//
//! DESCRIPTION: Initialize SMP Assist extension. Take the SMPAssist(MRS/LMS) params from the application. 
//!              Provide the application with a interface pointer for future use.
//!
//! \since Release: 396
//! \param [in]       pDevice                   Pointer to IUnknown (Currently supports ID3D11Device)
//! \param [inout]    pSMPAssistInitParams      SMP Assist initialization params
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::NVAPI_OK                          Call succeeded.
//! \retval ::NVAPI_ERROR                       Call failed.
//! \retval ::NVAPI_INVALID_ARGUMENT            One of the required input arguments was NULL
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D_InitializeSMPAssist(__in IUnknown *pDevice, __inout NV_SMP_ASSIST_INITIALIZE_PARAMS *pSMPAssistInitParams);

#endif //defined(__cplusplus) && defined(__d3d11_h__)

//! SUPPORTED OS:  Windows 7 and higher
//!
#if defined (__cplusplus) && defined(__d3d11_h__) 
//! \ingroup dx
typedef struct _NV_QUERY_SMP_ASSIST_SUPPORT_PARAMS_V1
{
    NvU32               version;                       //!< (IN) Structure version
    NV_SMP_ASSIST_TYPE  eSMPAssistType;                //!< (IN) None/MRS/LMS
    NV_SMP_ASSIST_LEVEL eSMPAssistLevel;               //!< (IN) Full/Partial/Minimal
    NvBool              bSMPAssistSupported;           //!< (OUT) SMP Assist supported or not
} NV_QUERY_SMP_ASSIST_SUPPORT_PARAMS_V1;

typedef NV_QUERY_SMP_ASSIST_SUPPORT_PARAMS_V1          NV_QUERY_SMP_ASSIST_SUPPORT_PARAMS;
#define NV_QUERY_SMP_ASSIST_SUPPORT_PARAMS_VER1        MAKE_NVAPI_VERSION(NV_QUERY_SMP_ASSIST_SUPPORT_PARAMS_V1, 1)
#define NV_QUERY_SMP_ASSIST_SUPPORT_PARAMS_VER         NV_QUERY_SMP_ASSIST_SUPPORT_PARAMS_VER1

//! SUPPORTED OS:  Windows 7 and higher
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_QuerySMPAssistSupport
//
//! DESCRIPTION: Query SMP assist extension support.
//!
//! \since Release: 396
//! \param [in]     pDev                                Pointer to IUnknown (Currently supports ID3D11Device)
//! \param [out]    pQuerySMPAssistSupportParams        Pointer to a structure returning requested SMP assist support
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, they are listed below.
//! \retval ::NVAPI_NOT_SUPPORTED                       Requested SMP assist is not supported.
//! \retval ::NVAPI_OK                                  Call succeeded. Check value of pQuerySMPAssistSupportParams->bSMPAssistSupported 
//! \retval ::NVAPI_INVALID_POINTER                     pDev or pQuerySMPAssistSupportParams was a NULL pointer
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D_QuerySMPAssistSupport(__in IUnknown *pDev, __inout NV_QUERY_SMP_ASSIST_SUPPORT_PARAMS *pQuerySMPAssistSupportParams);

#endif //defined(__cplusplus) && defined(__d3d11_h__)



//! SUPPORTED OS:  Windows 7 and higher
//!
//! Used to get sleep status
//! \ingroup dx
typedef struct _NV_GET_SLEEP_STATUS_PARAMS
{
    NvU32  version;                                       //!< (IN) Structure version
    NvBool bLowLatencyMode;                               //!< (OUT) Is low latency mode enabled?
    NvU8   rsvd[128];                                     //!< (IN) Reserved. Must be set to 0s.
} NV_GET_SLEEP_STATUS_PARAMS_V1;

typedef NV_GET_SLEEP_STATUS_PARAMS_V1            NV_GET_SLEEP_STATUS_PARAMS;
#define NV_GET_SLEEP_STATUS_PARAMS_VER1          MAKE_NVAPI_VERSION(NV_GET_SLEEP_STATUS_PARAMS_V1, 1)
#define NV_GET_SLEEP_STATUS_PARAMS_VER           NV_GET_SLEEP_STATUS_PARAMS_VER1

#if defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_GetSleepStatus
//
//!   DESCRIPTION: This function can be used to get the latest sleep status.
//!   bLowLatencyMode indicates whether low latency mode is currently
//!   enabled in the driver.
//!   Note that it may not always reflect the previously requested sleep mode,
//!   as the feature may not be available on the platform, or the setting has
//!   been overridden by the control panel, for example.
//!
//! \since Release: 455
//! \param [in] pDev                          The target device.
//! \param [in] pGetSleepStatusParams         Sleep status params.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status.
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D_GetSleepStatus(__in IUnknown *pDev, __in NV_GET_SLEEP_STATUS_PARAMS *pGetSleepStatusParams);
#endif //defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__))

//! SUPPORTED OS:  Windows 7 and higher
//!
//! Used to set sleep mode
//! \ingroup dx
typedef struct _NV_SET_SLEEP_MODE_PARAMS
{
    NvU32  version;                                       //!< (IN) Structure version
    NvBool bLowLatencyMode;                               //!< (IN) Low latency mode enable/disable.
    NvBool bLowLatencyBoost;                              //!< (IN) Request maximum GPU clock frequency regardless of workload.
    NvU32  minimumIntervalUs;                             //!< (IN) Minimum frame interval in microseconds. 0 = no frame rate limit. 
    NvU8   rsvd[32];                                      //!< (IN) Reserved. Must be set to 0s.
} NV_SET_SLEEP_MODE_PARAMS_V1;

typedef NV_SET_SLEEP_MODE_PARAMS_V1            NV_SET_SLEEP_MODE_PARAMS;
#define NV_SET_SLEEP_MODE_PARAMS_VER1          MAKE_NVAPI_VERSION(NV_SET_SLEEP_MODE_PARAMS_V1, 1)
#define NV_SET_SLEEP_MODE_PARAMS_VER           NV_SET_SLEEP_MODE_PARAMS_VER1

#if defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_SetSleepMode
//
//!   DESCRIPTION: This function can be used to update sleep mode dynamically.
//!   The settings are not dependent to each other, meaning low latency mode
//!   can be enabled/disabled regardless of whether minimum interval is set or
//!   not. The former is to intelligently lower latency without impacting frame
//!   rate. The later is to limit frame rate (e.g. minimumIntervalUs = 10000
//!   limits frame rate to 100 FPS). They work well separately and/or together.
//!   Note that minimumIntervalUs usage is not limited to lowering latency, so
//!   feel free to use it to limit frame rate for menu, cut scenes, etc.
//!   Note that low latency mode can be enabled, and/or minimum interval can
//!   be set, even without using NvAPI_D3D_Sleep(). However, without it, the
//!   sleep to achieve these features would happen at a less optimal point,
//!   resulting in higher overall latency.
//!   The bLowLatencyBoost will request the GPU run at max clocks even in 
//!   scenarios where it is idle most of the frame and would normally try
//!   to save power.  This can decrease latency in CPU-limited scenarios.
//!   While this function can be called as often as needed, it is not
//!   necessary nor recommended to call this too frequently (e.g. every frame),
//!   as the settings persist for the target device.
//!
//! \since Release: 455
//! \param [in] pDev                          The target device.
//! \param [in] pSetSleepModeParams           Sleep mode params.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status.
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D_SetSleepMode(__in IUnknown *pDev, __in NV_SET_SLEEP_MODE_PARAMS *pSetSleepModeParams);
#endif //defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__))

#if defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_Sleep
//
//!   DESCRIPTION: It is recommended to call this function at the very start of
//!   each frame (e.g. before input sampling). If there is a need to sleep,
//!   due to frame rate limit and/or low latency features, for example,
//!   this call provides an entry point for the driver to sleep at the most
//!   optimal spot to achieve the lowest latency.
//!   It is recommended to call this function even when low latency mode is
//!   disabled and minimum interval is 0. Other features, such as Maximum Frame
//!   Rate setting, could be enabled in the control panel to benefit from this.
//!   It is OK to start (or stop) using this function at any time. However,
//!   when using this function, it must be called exactly once on each frame.
//!   If this function is not called, after several frames, the driver would
//!   fallback to sleep at its less optimal spot.
//!
//! \since Release: 455
//! \param [in] pDev                          The target device.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status.
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D_Sleep(__in IUnknown *pDev);
#endif //defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__))

//! SUPPORTED OS:  Windows 7 and higher
//!
//! Used to get latency report.
//! \ingroup dx

typedef struct _NV_LATENCY_RESULT_PARAMS
{
    NvU32  version;                                       //!< (IN) Structure version
    struct FrameReport {
        NvU64 frameID;
        NvU64 inputSampleTime;
        NvU64 simStartTime;
        NvU64 simEndTime;
        NvU64 renderSubmitStartTime;
        NvU64 renderSubmitEndTime;
        NvU64 presentStartTime;
        NvU64 presentEndTime;
        NvU64 driverStartTime;
        NvU64 driverEndTime;
        NvU64 osRenderQueueStartTime;
        NvU64 osRenderQueueEndTime;
        NvU64 gpuRenderStartTime;
        NvU64 gpuRenderEndTime;
        NvU8  rsvd[128];
    } frameReport[64];
    NvU8  rsvd[32];
} NV_LATENCY_RESULT_PARAMS_V1;

typedef NV_LATENCY_RESULT_PARAMS_V1            NV_LATENCY_RESULT_PARAMS;
#define NV_LATENCY_RESULT_PARAMS_VER1          MAKE_NVAPI_VERSION(NV_LATENCY_RESULT_PARAMS_V1, 1)
#define NV_LATENCY_RESULT_PARAMS_VER           NV_LATENCY_RESULT_PARAMS_VER1

#if defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_GetLatency
//
//!   DESCRIPTION: Get a latency report including the timestamps of the 
//!   application latency markers set with NvAPI_D3D_SetLatencyMarker as well
//!   as driver, OS queue and graphics hardware times.  Requires calling
//!   NvAPI_D3D_SetLatencyMarker with incrementing frameID for valid results.
//!   Rendering for at least 90 frames is recommended to properly fill out the
//!   structure.  The newest completed frame is at the end (element 63) and
//!   is preceeded by older frames.  If not enough frames are valid then all
//!   frames are returned with all zeroes.
//!
//! \since Release: 455
//! \param [in] pDev                          The target device
//! \param [in] pGetLatencyParams             The latency result structure.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status.
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D_GetLatency(__in IUnknown *pDev, __out NV_LATENCY_RESULT_PARAMS *pGetLatencyParams);
#endif //defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__))

//! SUPPORTED OS:  Windows 7 and higher
//!
//! Used define latency marker type
//! \ingroup dx
typedef enum
{
    SIMULATION_START        = 0,
    SIMULATION_END          = 1,
    RENDERSUBMIT_START      = 2,
    RENDERSUBMIT_END        = 3,
    PRESENT_START           = 4,
    PRESENT_END             = 5,
    INPUT_SAMPLE            = 6,
    TRIGGER_FLASH           = 7,
} NV_LATENCY_MARKER_TYPE;

//! SUPPORTED OS:  Windows 7 and higher
//!
//! Used set latency markers
//! \ingroup dx
typedef struct _NV_LATENCY_MARKER_PARAMS
{
    NvU32  version;                                       //!< (IN) Structure version
    NvU64  frameID;
    NV_LATENCY_MARKER_TYPE markerType;
    NvU8   rsvd[64];
} NV_LATENCY_MARKER_PARAMS_V1;

typedef NV_LATENCY_MARKER_PARAMS_V1            NV_LATENCY_MARKER_PARAMS;
#define NV_LATENCY_MARKER_PARAMS_VER1          MAKE_NVAPI_VERSION(NV_LATENCY_MARKER_PARAMS_V1, 1)
#define NV_LATENCY_MARKER_PARAMS_VER           NV_LATENCY_MARKER_PARAMS_VER1

#if defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__))
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D_SetLatencyMarker
//
//!   DESCRIPTION: Set a latency marker to be tracked by the
//!   NvAPI_D3D_GetLatency function. SIMULATION_START must be the first marker
//!   sent in a frame, after the previous frame's Sleep call (if used).
//!   INPUT_SAMPLE may be sent to record the moment user input was sampled and
//!   should come between SIMULATION_START and SIMULATION_END.
//!   RENDERSUBMIT_START should come before any Direct3D calls are made for
//!   the given frame and RENDERSUBMIT_END should come before calling Present.
//!   PRESENT_START and END should wrap the Present call to inform the driver
//!   of a present block done by the OS before the driver receives the Present.
//!   TRIGGER_FLASH tells the driver to render its flash indicator for latency
//!   testing, typically driven by a mouse click.
//!   The frameID can start at an abitrary moment in the application lifetime
//!   but must strictly increment from that point forward for consistent results.
//!
//! \since Release: 455
//! \param [in] pDev                          The target device
//! \param [in] pSetLatencyMarkerParams       The latency marker structure
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status.
//!         If there are return error codes with specific meaning for this API, they are listed below.
//!
//! \ingroup dx
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D_SetLatencyMarker(__in IUnknown *pDev, __in NV_LATENCY_MARKER_PARAMS* pSetLatencyMarkerParams);
#endif //defined(__cplusplus) && (defined(_D3D9_H_) || defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__) || defined(__d3d12_h__))




//! \ingroup vidio
//! Unique identifier for VIO owner (process identifier or NVVIOOWNERID_NONE)
typedef NvU32   NVVIOOWNERID;                               


//! \addtogroup vidio
//! @{


#define NVVIOOWNERID_NONE                   0      //!< Unregistered ownerId        


//! Owner type for device
typedef enum _NVVIOOWNERTYPE                               
{
    NVVIOOWNERTYPE_NONE                             ,       //!<  No owner for the device
    NVVIOOWNERTYPE_APPLICATION                      ,       //!<  Application owns the device
    NVVIOOWNERTYPE_DESKTOP                          ,       //!<  Desktop transparent mode owns the device (not applicable for video input)
}NVVIOOWNERTYPE;

// Access rights for NvAPI_VIO_Open()

//! Read access             (not applicable for video output)
#define NVVIO_O_READ                        0x00000000      

//! Write exclusive access  (not applicable for video input)
#define NVVIO_O_WRITE_EXCLUSIVE             0x00010001      

//! 
#define NVVIO_VALID_ACCESSRIGHTS            (NVVIO_O_READ              | \
                                             NVVIO_O_WRITE_EXCLUSIVE   )

              
//! VIO_DATA.ulOwnerID high-bit is set only if device has been initialized by VIOAPI
//! examined at NvAPI_GetCapabilities|NvAPI_VIO_Open to determine if settings need to be applied from registry or POR state read
#define NVVIO_OWNERID_INITIALIZED  0x80000000

//! VIO_DATA.ulOwnerID next-bit is set only if device is currently in exclusive write access mode from NvAPI_VIO_Open()
#define NVVIO_OWNERID_EXCLUSIVE    0x40000000
                                              
//! VIO_DATA.ulOwnerID lower bits are:
//!  NVGVOOWNERTYPE_xxx enumerations indicating use context
#define NVVIO_OWNERID_TYPEMASK     0x0FFFFFFF //!< mask for NVVIOOWNERTYPE_xxx


//! @}
              
//---------------------------------------------------------------------
// Enumerations
//---------------------------------------------------------------------


//! \addtogroup vidio
//! @{

//! Video signal format and resolution
typedef enum _NVVIOSIGNALFORMAT
{
    NVVIOSIGNALFORMAT_NONE,                //!< Invalid signal format 
    NVVIOSIGNALFORMAT_487I_59_94_SMPTE259_NTSC,     //!< 01  487i    59.94Hz  (SMPTE259) NTSC
    NVVIOSIGNALFORMAT_576I_50_00_SMPTE259_PAL,      //!< 02  576i    50.00Hz  (SMPTE259) PAL
    NVVIOSIGNALFORMAT_1035I_60_00_SMPTE260,         //!< 03  1035i   60.00Hz  (SMPTE260)
    NVVIOSIGNALFORMAT_1035I_59_94_SMPTE260,         //!< 04  1035i   59.94Hz  (SMPTE260)
    NVVIOSIGNALFORMAT_1080I_50_00_SMPTE295,         //!< 05  1080i   50.00Hz  (SMPTE295)
    NVVIOSIGNALFORMAT_1080I_60_00_SMPTE274,         //!< 06  1080i   60.00Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080I_59_94_SMPTE274,         //!< 07  1080i   59.94Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080I_50_00_SMPTE274,         //!< 08  1080i   50.00Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080P_30_00_SMPTE274,         //!< 09  1080p   30.00Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080P_29_97_SMPTE274,         //!< 10  1080p   29.97Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080P_25_00_SMPTE274,         //!< 11  1080p   25.00Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080P_24_00_SMPTE274,         //!< 12  1080p   24.00Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080P_23_976_SMPTE274,        //!< 13  1080p   23.976Hz (SMPTE274)
    NVVIOSIGNALFORMAT_720P_60_00_SMPTE296,          //!< 14  720p    60.00Hz  (SMPTE296)
    NVVIOSIGNALFORMAT_720P_59_94_SMPTE296,          //!< 15  720p    59.94Hz  (SMPTE296)
    NVVIOSIGNALFORMAT_720P_50_00_SMPTE296,          //!< 16  720p    50.00Hz  (SMPTE296)
    NVVIOSIGNALFORMAT_1080I_48_00_SMPTE274,         //!< 17  1080I   48.00Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080I_47_96_SMPTE274,         //!< 18  1080I   47.96Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_720P_30_00_SMPTE296,          //!< 19  720p    30.00Hz  (SMPTE296)
    NVVIOSIGNALFORMAT_720P_29_97_SMPTE296,          //!< 20  720p    29.97Hz  (SMPTE296)
    NVVIOSIGNALFORMAT_720P_25_00_SMPTE296,          //!< 21  720p    25.00Hz  (SMPTE296)
    NVVIOSIGNALFORMAT_720P_24_00_SMPTE296,          //!< 22  720p    24.00Hz  (SMPTE296)
    NVVIOSIGNALFORMAT_720P_23_98_SMPTE296,          //!< 23  720p    23.98Hz  (SMPTE296)
    NVVIOSIGNALFORMAT_2048P_30_00_SMPTE372,         //!< 24  2048p   30.00Hz  (SMPTE372)
    NVVIOSIGNALFORMAT_2048P_29_97_SMPTE372,         //!< 25  2048p   29.97Hz  (SMPTE372)
    NVVIOSIGNALFORMAT_2048I_60_00_SMPTE372,         //!< 26  2048i   60.00Hz  (SMPTE372)
    NVVIOSIGNALFORMAT_2048I_59_94_SMPTE372,         //!< 27  2048i   59.94Hz  (SMPTE372)
    NVVIOSIGNALFORMAT_2048P_25_00_SMPTE372,         //!< 28  2048p   25.00Hz  (SMPTE372)
    NVVIOSIGNALFORMAT_2048I_50_00_SMPTE372,         //!< 29  2048i   50.00Hz  (SMPTE372)
    NVVIOSIGNALFORMAT_2048P_24_00_SMPTE372,         //!< 30  2048p   24.00Hz  (SMPTE372)
    NVVIOSIGNALFORMAT_2048P_23_98_SMPTE372,         //!< 31  2048p   23.98Hz  (SMPTE372)
    NVVIOSIGNALFORMAT_2048I_48_00_SMPTE372,         //!< 32  2048i   48.00Hz  (SMPTE372)
    NVVIOSIGNALFORMAT_2048I_47_96_SMPTE372,         //!< 33  2048i   47.96Hz  (SMPTE372)
    
    NVVIOSIGNALFORMAT_1080PSF_25_00_SMPTE274,       //!< 34  1080PsF 25.00Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080PSF_29_97_SMPTE274,       //!< 35  1080PsF 29.97Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080PSF_30_00_SMPTE274,       //!< 36  1080PsF 30.00Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080PSF_24_00_SMPTE274,       //!< 37  1080PsF 24.00Hz  (SMPTE274)
    NVVIOSIGNALFORMAT_1080PSF_23_98_SMPTE274,       //!< 38  1080PsF 23.98Hz  (SMPTE274)

    NVVIOSIGNALFORMAT_1080P_50_00_SMPTE274_3G_LEVEL_A, //!< 39  1080P   50.00Hz  (SMPTE274) 3G Level A
    NVVIOSIGNALFORMAT_1080P_59_94_SMPTE274_3G_LEVEL_A, //!< 40  1080P   59.94Hz  (SMPTE274) 3G Level A
    NVVIOSIGNALFORMAT_1080P_60_00_SMPTE274_3G_LEVEL_A, //!< 41  1080P   60.00Hz  (SMPTE274) 3G Level A
    
    NVVIOSIGNALFORMAT_1080P_60_00_SMPTE274_3G_LEVEL_B, //!< 42  1080p   60.00Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_1080I_60_00_SMPTE274_3G_LEVEL_B, //!< 43  1080i   60.00Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_2048I_60_00_SMPTE372_3G_LEVEL_B, //!< 44  2048i   60.00Hz  (SMPTE372) 3G Level B
    NVVIOSIGNALFORMAT_1080P_50_00_SMPTE274_3G_LEVEL_B, //!< 45  1080p   50.00Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_1080I_50_00_SMPTE274_3G_LEVEL_B, //!< 46  1080i   50.00Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_2048I_50_00_SMPTE372_3G_LEVEL_B, //!< 47  2048i   50.00Hz  (SMPTE372) 3G Level B
    NVVIOSIGNALFORMAT_1080P_30_00_SMPTE274_3G_LEVEL_B, //!< 48  1080p   30.00Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_2048P_30_00_SMPTE372_3G_LEVEL_B, //!< 49  2048p   30.00Hz  (SMPTE372) 3G Level B
    NVVIOSIGNALFORMAT_1080P_25_00_SMPTE274_3G_LEVEL_B, //!< 50  1080p   25.00Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_2048P_25_00_SMPTE372_3G_LEVEL_B, //!< 51  2048p   25.00Hz  (SMPTE372) 3G Level B
    NVVIOSIGNALFORMAT_1080P_24_00_SMPTE274_3G_LEVEL_B, //!< 52  1080p   24.00Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_2048P_24_00_SMPTE372_3G_LEVEL_B, //!< 53  2048p   24.00Hz  (SMPTE372) 3G Level B
    NVVIOSIGNALFORMAT_1080I_48_00_SMPTE274_3G_LEVEL_B, //!< 54  1080i   48.00Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_2048I_48_00_SMPTE372_3G_LEVEL_B, //!< 55  2048i   48.00Hz  (SMPTE372) 3G Level B
    NVVIOSIGNALFORMAT_1080P_59_94_SMPTE274_3G_LEVEL_B, //!< 56  1080p   59.94Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_1080I_59_94_SMPTE274_3G_LEVEL_B, //!< 57  1080i   59.94Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_2048I_59_94_SMPTE372_3G_LEVEL_B, //!< 58  2048i   59.94Hz  (SMPTE372) 3G Level B
    NVVIOSIGNALFORMAT_1080P_29_97_SMPTE274_3G_LEVEL_B, //!< 59  1080p   29.97Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_2048P_29_97_SMPTE372_3G_LEVEL_B, //!< 60  2048p   29.97Hz  (SMPTE372) 3G Level B
    NVVIOSIGNALFORMAT_1080P_23_98_SMPTE274_3G_LEVEL_B, //!< 61  1080p   29.98Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_2048P_23_98_SMPTE372_3G_LEVEL_B, //!< 62  2048p   29.98Hz  (SMPTE372) 3G Level B
    NVVIOSIGNALFORMAT_1080I_47_96_SMPTE274_3G_LEVEL_B, //!< 63  1080i   47.96Hz  (SMPTE274) 3G Level B
    NVVIOSIGNALFORMAT_2048I_47_96_SMPTE372_3G_LEVEL_B, //!< 64  2048i   47.96Hz  (SMPTE372) 3G Level B
    
    NVVIOSIGNALFORMAT_END                              //!< 65  To indicate end of signal format list

}NVVIOSIGNALFORMAT;

//! SMPTE standards format
typedef enum _NVVIOVIDEOSTANDARD
{
    NVVIOVIDEOSTANDARD_SMPTE259                        ,       //!< SMPTE259
    NVVIOVIDEOSTANDARD_SMPTE260                        ,       //!< SMPTE260
    NVVIOVIDEOSTANDARD_SMPTE274                        ,       //!< SMPTE274
    NVVIOVIDEOSTANDARD_SMPTE295                        ,       //!< SMPTE295
    NVVIOVIDEOSTANDARD_SMPTE296                        ,       //!< SMPTE296
    NVVIOVIDEOSTANDARD_SMPTE372                        ,       //!< SMPTE372
}NVVIOVIDEOSTANDARD;

//! HD or SD video type
typedef enum _NVVIOVIDEOTYPE
{
    NVVIOVIDEOTYPE_SD                                  ,       //!< Standard-definition (SD)
    NVVIOVIDEOTYPE_HD                                  ,       //!< High-definition     (HD)
}NVVIOVIDEOTYPE;

//! Interlace mode
typedef enum _NVVIOINTERLACEMODE 
{
    NVVIOINTERLACEMODE_PROGRESSIVE                     ,       //!< Progressive               (p)
    NVVIOINTERLACEMODE_INTERLACE                       ,       //!< Interlace                 (i)
    NVVIOINTERLACEMODE_PSF                             ,       //!< Progressive Segment Frame (psf)
}NVVIOINTERLACEMODE;

//! Video data format
typedef enum _NVVIODATAFORMAT
{
    NVVIODATAFORMAT_UNKNOWN   = -1                     ,       //!< Invalid DataFormat
    NVVIODATAFORMAT_R8G8B8_TO_YCRCB444                 ,       //!< R8:G8:B8                => YCrCb  (4:4:4)
    NVVIODATAFORMAT_R8G8B8A8_TO_YCRCBA4444             ,       //!< R8:G8:B8:A8             => YCrCbA (4:4:4:4)
    NVVIODATAFORMAT_R8G8B8Z10_TO_YCRCBZ4444            ,       //!< R8:G8:B8:Z10            => YCrCbZ (4:4:4:4)
    NVVIODATAFORMAT_R8G8B8_TO_YCRCB422                 ,       //!< R8:G8:B8                => YCrCb  (4:2:2)
    NVVIODATAFORMAT_R8G8B8A8_TO_YCRCBA4224             ,       //!< R8:G8:B8:A8             => YCrCbA (4:2:2:4)
    NVVIODATAFORMAT_R8G8B8Z10_TO_YCRCBZ4224            ,       //!< R8:G8:B8:Z10            => YCrCbZ (4:2:2:4)
    NVVIODATAFORMAT_X8X8X8_444_PASSTHRU                ,       //!< R8:G8:B8                => RGB    (4:4:4)
    NVVIODATAFORMAT_X8X8X8A8_4444_PASSTHRU             ,       //!< R8:G8:B8:A8             => RGBA   (4:4:4:4)
    NVVIODATAFORMAT_X8X8X8Z10_4444_PASSTHRU            ,       //!< R8:G8:B8:Z10            => RGBZ   (4:4:4:4)
    NVVIODATAFORMAT_X10X10X10_444_PASSTHRU             ,       //!< Y10:CR10:CB10           => YCrCb  (4:4:4)
    NVVIODATAFORMAT_X10X8X8_444_PASSTHRU               ,       //!< Y10:CR8:CB8             => YCrCb  (4:4:4)
    NVVIODATAFORMAT_X10X8X8A10_4444_PASSTHRU           ,       //!< Y10:CR8:CB8:A10         => YCrCbA (4:4:4:4)
    NVVIODATAFORMAT_X10X8X8Z10_4444_PASSTHRU           ,       //!< Y10:CR8:CB8:Z10         => YCrCbZ (4:4:4:4)
    NVVIODATAFORMAT_DUAL_R8G8B8_TO_DUAL_YCRCB422       ,       //!< R8:G8:B8 + R8:G8:B8     => YCrCb  (4:2:2 + 4:2:2)
    NVVIODATAFORMAT_DUAL_X8X8X8_TO_DUAL_422_PASSTHRU   ,       //!< Y8:CR8:CB8 + Y8:CR8:CB8 => YCrCb  (4:2:2 + 4:2:2)
    NVVIODATAFORMAT_R10G10B10_TO_YCRCB422              ,       //!< R10:G10:B10             => YCrCb  (4:2:2)
    NVVIODATAFORMAT_R10G10B10_TO_YCRCB444              ,       //!< R10:G10:B10             => YCrCb  (4:4:4)
    NVVIODATAFORMAT_X12X12X12_444_PASSTHRU             ,       //!< X12:X12:X12             => XXX    (4:4:4)
    NVVIODATAFORMAT_X12X12X12_422_PASSTHRU             ,       //!< X12:X12:X12             => XXX    (4:2:2)
    NVVIODATAFORMAT_Y10CR10CB10_TO_YCRCB422            ,       //!< Y10:CR10:CB10           => YCrCb  (4:2:2)
    NVVIODATAFORMAT_Y8CR8CB8_TO_YCRCB422               ,       //!< Y8:CR8:CB8              => YCrCb  (4:2:2)
    NVVIODATAFORMAT_Y10CR8CB8A10_TO_YCRCBA4224         ,       //!< Y10:CR8:CB8:A10         => YCrCbA (4:2:2:4)
    NVVIODATAFORMAT_R10G10B10_TO_RGB444                ,       //!< R10:G10:B10             => RGB    (4:4:4)
    NVVIODATAFORMAT_R12G12B12_TO_YCRCB444              ,       //!< R12:G12:B12             => YCrCb  (4:4:4)
    NVVIODATAFORMAT_R12G12B12_TO_YCRCB422              ,       //!< R12:G12:B12             => YCrCb  (4:2:2)
}NVVIODATAFORMAT;

//! Video output area
typedef enum _NVVIOOUTPUTAREA
{
    NVVIOOUTPUTAREA_FULLSIZE                           ,       //!< Output to entire video resolution (full size)
    NVVIOOUTPUTAREA_SAFEACTION                         ,       //!< Output to centered 90% of video resolution (safe action)
    NVVIOOUTPUTAREA_SAFETITLE                          ,       //!< Output to centered 80% of video resolution (safe title)
}NVVIOOUTPUTAREA;

//! Synchronization source
typedef enum _NVVIOSYNCSOURCE
{
    NVVIOSYNCSOURCE_SDISYNC                            ,       //!< SDI Sync  (Digital input)
    NVVIOSYNCSOURCE_COMPSYNC                           ,       //!< COMP Sync (Composite input)
}NVVIOSYNCSOURCE;

//! Composite synchronization type
typedef enum _NVVIOCOMPSYNCTYPE
{
    NVVIOCOMPSYNCTYPE_AUTO                             ,       //!< Auto-detect
    NVVIOCOMPSYNCTYPE_BILEVEL                          ,       //!< Bi-level signal
    NVVIOCOMPSYNCTYPE_TRILEVEL                         ,       //!< Tri-level signal
}NVVIOCOMPSYNCTYPE;

//! Video input output status
typedef enum _NVVIOINPUTOUTPUTSTATUS
{
    NVINPUTOUTPUTSTATUS_OFF                            ,       //!< Not in use
    NVINPUTOUTPUTSTATUS_ERROR                          ,       //!< Error detected
    NVINPUTOUTPUTSTATUS_SDI_SD                         ,       //!< SDI (standard-definition)
    NVINPUTOUTPUTSTATUS_SDI_HD                         ,       //!< SDI (high-definition)
}NVVIOINPUTOUTPUTSTATUS;

//! Synchronization input status
typedef enum _NVVIOSYNCSTATUS
{
    NVVIOSYNCSTATUS_OFF                                ,       //!< Sync not detected
    NVVIOSYNCSTATUS_ERROR                              ,       //!< Error detected
    NVVIOSYNCSTATUS_SYNCLOSS                           ,       //!< Genlock in use, format mismatch with output
    NVVIOSYNCSTATUS_COMPOSITE                          ,       //!< Composite sync
    NVVIOSYNCSTATUS_SDI_SD                             ,       //!< SDI sync (standard-definition)
    NVVIOSYNCSTATUS_SDI_HD                             ,       //!< SDI sync (high-definition)
}NVVIOSYNCSTATUS;

//! Video Capture Status
typedef enum _NVVIOCAPTURESTATUS
{
    NVVIOSTATUS_STOPPED                                ,       //!< Sync not detected
    NVVIOSTATUS_RUNNING                                ,       //!< Error detected
    NVVIOSTATUS_ERROR                                  ,       //!< Genlock in use, format mismatch with output
}NVVIOCAPTURESTATUS;

//! Video Capture Status
typedef enum _NVVIOSTATUSTYPE
{
    NVVIOSTATUSTYPE_IN                                 ,       //!< Input Status
    NVVIOSTATUSTYPE_OUT                                ,       //!< Output Status
}NVVIOSTATUSTYPE;


//! Assumption, maximum 4 SDI input and 4 SDI output cards supported on a system
#define NVAPI_MAX_VIO_DEVICES                 8   

//! 4 physical jacks supported on each SDI input card.
#define NVAPI_MAX_VIO_JACKS                   4   


//! Each physical jack an on SDI input card can have
//! two "channels" in the case of "3G" VideoFormats, as specified
//! by SMPTE 425; for non-3G VideoFormats, only the first channel within
//! a physical jack is valid.
#define NVAPI_MAX_VIO_CHANNELS_PER_JACK       2   

//! 4 Streams, 1 per physical jack
#define NVAPI_MAX_VIO_STREAMS                 4   

#define NVAPI_MIN_VIO_STREAMS                 1   

//! SDI input supports a max of 2 links per stream
#define NVAPI_MAX_VIO_LINKS_PER_STREAM        2   


#define NVAPI_MAX_FRAMELOCK_MAPPING_MODES     20

//! Min number of capture images 
#define NVAPI_GVI_MIN_RAW_CAPTURE_IMAGES      1   

//! Max number of capture images        
#define NVAPI_GVI_MAX_RAW_CAPTURE_IMAGES      32  

//! Default number of capture images
#define NVAPI_GVI_DEFAULT_RAW_CAPTURE_IMAGES  5   



// Data Signal notification events. These need a event handler in RM.
// Register/Unregister and PopEvent NVAPI's are already available.

//! Device configuration
typedef enum _NVVIOCONFIGTYPE
{
    NVVIOCONFIGTYPE_IN                                 ,       //!< Input Status
    NVVIOCONFIGTYPE_OUT                                ,       //!< Output Status
}NVVIOCONFIGTYPE;

typedef enum _NVVIOCOLORSPACE
{
    NVVIOCOLORSPACE_UNKNOWN,
    NVVIOCOLORSPACE_YCBCR,
    NVVIOCOLORSPACE_YCBCRA,
    NVVIOCOLORSPACE_YCBCRD,
    NVVIOCOLORSPACE_GBR,
    NVVIOCOLORSPACE_GBRA,
    NVVIOCOLORSPACE_GBRD,
} NVVIOCOLORSPACE;

//! Component sampling
typedef enum _NVVIOCOMPONENTSAMPLING
{
    NVVIOCOMPONENTSAMPLING_UNKNOWN,
    NVVIOCOMPONENTSAMPLING_4444,
    NVVIOCOMPONENTSAMPLING_4224,
    NVVIOCOMPONENTSAMPLING_444,
    NVVIOCOMPONENTSAMPLING_422
} NVVIOCOMPONENTSAMPLING;

typedef enum _NVVIOBITSPERCOMPONENT
{
    NVVIOBITSPERCOMPONENT_UNKNOWN,
    NVVIOBITSPERCOMPONENT_8,
    NVVIOBITSPERCOMPONENT_10,
    NVVIOBITSPERCOMPONENT_12,
} NVVIOBITSPERCOMPONENT;

typedef enum _NVVIOLINKID 
{
    NVVIOLINKID_UNKNOWN,
    NVVIOLINKID_A,
    NVVIOLINKID_B,
    NVVIOLINKID_C,
    NVVIOLINKID_D
} NVVIOLINKID;


typedef enum _NVVIOANCPARITYCOMPUTATION
{
    NVVIOANCPARITYCOMPUTATION_AUTO,
    NVVIOANCPARITYCOMPUTATION_ON,
    NVVIOANCPARITYCOMPUTATION_OFF
} NVVIOANCPARITYCOMPUTATION;



//! @}


//---------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------

//! \addtogroup vidio
//! @{


//! Supports Serial Digital Interface (SDI) output
#define NVVIOCAPS_VIDOUT_SDI                0x00000001      

//! Supports Internal timing source
#define NVVIOCAPS_SYNC_INTERNAL             0x00000100      

//! Supports Genlock timing source
#define NVVIOCAPS_SYNC_GENLOCK              0x00000200      

//! Supports Serial Digital Interface (SDI) synchronization input
#define NVVIOCAPS_SYNCSRC_SDI               0x00001000      

//! Supports Composite synchronization input
#define NVVIOCAPS_SYNCSRC_COMP              0x00002000      

//! Supports Desktop transparent mode
#define NVVIOCAPS_OUTPUTMODE_DESKTOP        0x00010000      

//! Supports OpenGL application mode
#define NVVIOCAPS_OUTPUTMODE_OPENGL         0x00020000      

//! Supports Serial Digital Interface (SDI) input
#define NVVIOCAPS_VIDIN_SDI                 0x00100000  

//! Supports Packed ANC
#define NVVIOCAPS_PACKED_ANC_SUPPORTED      0x00200000     

//! Supports ANC audio blanking
#define NVVIOCAPS_AUDIO_BLANKING_SUPPORTED  0x00400000  

//! SDI-class interface: SDI output with two genlock inputs
#define NVVIOCLASS_SDI                      0x00000001      

//! Device capabilities
typedef struct _NVVIOCAPS
{
    NvU32             version;                              //!< Structure version
    NvAPI_String      adapterName;                          //!< Graphics adapter name
    NvU32             adapterClass;                         //!< Graphics adapter classes (NVVIOCLASS_SDI mask)
    NvU32             adapterCaps;                          //!< Graphics adapter capabilities (NVVIOCAPS_* mask)
    NvU32             dipSwitch;                            //!< On-board DIP switch settings bits
    NvU32             dipSwitchReserved;                    //!< On-board DIP switch settings reserved bits
    NvU32             boardID;                              //!< Board ID
    //! Driver version
    struct                                                  //
    {                                                      
        NvU32          majorVersion;                        //!< Major version. For GVI, majorVersion contains MajorVersion(HIWORD) And MinorVersion(LOWORD)
        NvU32          minorVersion;                        //!< Minor version. For GVI, minorVersion contains Revison(HIWORD) And Build(LOWORD)
    } driver;                                               //
    //! Firmware version 
    struct                                                  
    {                                                       
        NvU32          majorVersion;                        //!< Major version. In version 2, for both GVI and GVO, majorVersion contains MajorVersion(HIWORD) And MinorVersion(LOWORD)
        NvU32          minorVersion;                        //!< Minor version. In version 2, for both GVI and GVO, minorVersion contains Revison(HIWORD) And Build(LOWORD)
    } firmWare;                                             //
    NVVIOOWNERID      ownerId;                              //!< Unique identifier for owner of video output (NVVIOOWNERID_INVALID if free running)
    NVVIOOWNERTYPE    ownerType;                            //!< Owner type (OpenGL application or Desktop mode)
} NVVIOCAPS;

//! Macro for constructing the version field of NVVIOCAPS
#define NVVIOCAPS_VER1  MAKE_NVAPI_VERSION(NVVIOCAPS,1)
#define NVVIOCAPS_VER2  MAKE_NVAPI_VERSION(NVVIOCAPS,2)
#define NVVIOCAPS_VER   NVVIOCAPS_VER2

//! Input channel status
typedef struct _NVVIOCHANNELSTATUS
{
    NvU32                  smpte352;                         //!< 4-byte SMPTE 352 video payload identifier
    NVVIOSIGNALFORMAT      signalFormat;                     //!< Signal format
    NVVIOBITSPERCOMPONENT  bitsPerComponent;                 //!< Bits per component
    NVVIOCOMPONENTSAMPLING samplingFormat;                   //!< Sampling format
    NVVIOCOLORSPACE        colorSpace;                       //!< Color space
    NVVIOLINKID            linkID;                           //!< Link ID
} NVVIOCHANNELSTATUS;

//! Input device status
typedef struct _NVVIOINPUTSTATUS
{
    NVVIOCHANNELSTATUS     vidIn[NVAPI_MAX_VIO_JACKS][NVAPI_MAX_VIO_CHANNELS_PER_JACK];     //!< Video input status per channel within a jack
    NVVIOCAPTURESTATUS     captureStatus;                  //!< status of video capture
} NVVIOINPUTSTATUS;

//! Output device status
typedef struct _NVVIOOUTPUTSTATUS
{
    NVVIOINPUTOUTPUTSTATUS    vid1Out;                        //!< Video 1 output status
    NVVIOINPUTOUTPUTSTATUS    vid2Out;                        //!< Video 2 output status
    NVVIOSYNCSTATUS        sdiSyncIn;                      //!< SDI sync input status
    NVVIOSYNCSTATUS        compSyncIn;                     //!< Composite sync input status
    NvU32            syncEnable;                     //!< Sync enable (TRUE if using syncSource)
    NVVIOSYNCSOURCE        syncSource;                     //!< Sync source
    NVVIOSIGNALFORMAT        syncFormat;                     //!< Sync format
    NvU32            frameLockEnable;                //!< Framelock enable flag
    NvU32            outputVideoLocked;              //!< Output locked status
    NvU32            dataIntegrityCheckErrorCount;   //!< Data integrity check error count
    NvU32            dataIntegrityCheckEnabled;      //!< Data integrity check status enabled 
    NvU32            dataIntegrityCheckFailed;       //!< Data integrity check status failed 
    NvU32                       uSyncSourceLocked;              //!< genlocked to framelocked to ref signal
    NvU32                       uPowerOn;                       //!< TRUE: indicates there is sufficient power
} NVVIOOUTPUTSTATUS;

//! Video device status.
typedef struct _NVVIOSTATUS
{
    NvU32                 version;                        //!< Structure version
    NVVIOSTATUSTYPE       nvvioStatusType;                //!< Input or Output status
    union                                                   
    {
        NVVIOINPUTSTATUS  inStatus;                       //!<  Input device status
        NVVIOOUTPUTSTATUS outStatus;                      //!<  Output device status
    }vioStatus;      
} NVVIOSTATUS;

//! Macro for constructingthe version field of NVVIOSTATUS
#define NVVIOSTATUS_VER   MAKE_NVAPI_VERSION(NVVIOSTATUS,1)

//! Output region
typedef struct _NVVIOOUTPUTREGION
{
    NvU32              x;                                    //!< Horizontal origin in pixels
    NvU32              y;                                    //!< Vertical origin in pixels
    NvU32              width;                                //!< Width of region in pixels
    NvU32              height;                               //!< Height of region in pixels
} NVVIOOUTPUTREGION;

//! Gamma ramp (8-bit index)
typedef struct _NVVIOGAMMARAMP8
{
    NvU16              uRed[256];                            //!< Red channel gamma ramp (8-bit index, 16-bit values)
    NvU16              uGreen[256];                          //!< Green channel gamma ramp (8-bit index, 16-bit values)
    NvU16              uBlue[256];                           //!< Blue channel gamma ramp (8-bit index, 16-bit values)
} NVVIOGAMMARAMP8;

//! Gamma ramp (10-bit index)
typedef struct _NVVIOGAMMARAMP10
{
    NvU16              uRed[1024];                           //!< Red channel gamma ramp (10-bit index, 16-bit values)
    NvU16              uGreen[1024];                         //!< Green channel gamma ramp (10-bit index, 16-bit values)
    NvU16              uBlue[1024];                          //!< Blue channel gamma ramp (10-bit index, 16-bit values)
} NVVIOGAMMARAMP10;


//! Sync delay
typedef struct _NVVIOSYNCDELAY
{
    NvU32              version;                              //!< Structure version
    NvU32              horizontalDelay;                      //!< Horizontal delay in pixels
    NvU32              verticalDelay;                        //!< Vertical delay in lines
} NVVIOSYNCDELAY;

//! Macro for constructing the version field of NVVIOSYNCDELAY
#define NVVIOSYNCDELAY_VER   MAKE_NVAPI_VERSION(NVVIOSYNCDELAY,1)


//! Video mode information
typedef struct _NVVIOVIDEOMODE
{
    NvU32                horizontalPixels;                   //!< Horizontal resolution (in pixels)
    NvU32                verticalLines;                      //!< Vertical resolution for frame (in lines)
    float                fFrameRate;                         //!< Frame rate
    NVVIOINTERLACEMODE   interlaceMode;                      //!< Interlace mode 
    NVVIOVIDEOSTANDARD   videoStandard;                      //!< SMPTE standards format
    NVVIOVIDEOTYPE       videoType;                          //!< HD or SD signal classification
} NVVIOVIDEOMODE;

//! Signal format details
typedef struct _NVVIOSIGNALFORMATDETAIL
{   
    NVVIOSIGNALFORMAT    signalFormat;                       //!< Signal format enumerated value
    NVVIOVIDEOMODE       videoMode;                          //!< Video mode for signal format
}NVVIOSIGNALFORMATDETAIL;


//! R8:G8:B8
#define NVVIOBUFFERFORMAT_R8G8B8                  0x00000001
 
//! R8:G8:B8:Z24  
#define NVVIOBUFFERFORMAT_R8G8B8Z24               0x00000002
   
//! R8:G8:B8:A8
#define NVVIOBUFFERFORMAT_R8G8B8A8                0x00000004   

//! R8:G8:B8:A8:Z24       
#define NVVIOBUFFERFORMAT_R8G8B8A8Z24             0x00000008   

//! R16FP:G16FP:B16FP
#define NVVIOBUFFERFORMAT_R16FPG16FPB16FP         0x00000010   

//! R16FP:G16FP:B16FP:Z24
#define NVVIOBUFFERFORMAT_R16FPG16FPB16FPZ24      0x00000020   

//! R16FP:G16FP:B16FP:A16FP
#define NVVIOBUFFERFORMAT_R16FPG16FPB16FPA16FP    0x00000040   

//! R16FP:G16FP:B16FP:A16FP:Z24
#define NVVIOBUFFERFORMAT_R16FPG16FPB16FPA16FPZ24 0x00000080   



//! Data format details
typedef struct _NVVIODATAFORMATDETAIL
{
    NVVIODATAFORMAT   dataFormat;                              //!< Data format enumerated value
    NvU32             vioCaps;                                 //!< Data format capabilities (NVVIOCAPS_* mask)
}NVVIODATAFORMATDETAIL;

//! Colorspace conversion
typedef struct _NVVIOCOLORCONVERSION
{
    NvU32       version;                                    //!<  Structure version
    float       colorMatrix[3][3];                          //!<  Output[n] =
    float       colorOffset[3];                             //!<  Input[0] * colorMatrix[n][0] +
    float       colorScale[3];                              //!<  Input[1] * colorMatrix[n][1] +
                                                            //!<  Input[2] * colorMatrix[n][2] +
                                                            //!<  OutputRange * colorOffset[n]
                                                            //!<  where OutputRange is the standard magnitude of
                                                            //!<  Output[n][n] and colorMatrix and colorOffset 
                                                            //!<  values are within the range -1.0 to +1.0
    NvU32      compositeSafe;                               //!<  compositeSafe constrains luminance range when using composite output
} NVVIOCOLORCONVERSION;

//! macro for constructing the version field of _NVVIOCOLORCONVERSION.
#define NVVIOCOLORCONVERSION_VER   MAKE_NVAPI_VERSION(NVVIOCOLORCONVERSION,1)

//! Gamma correction
typedef struct _NVVIOGAMMACORRECTION
{
    NvU32            version;                               //!< Structure version
    NvU32            vioGammaCorrectionType;                //!< Gamma correction type (8-bit or 10-bit)
    //! Gamma correction:
    union                                                   
    {                                                       
        NVVIOGAMMARAMP8  gammaRamp8;                        //!< Gamma ramp (8-bit index, 16-bit values)
        NVVIOGAMMARAMP10 gammaRamp10;                       //!< Gamma ramp (10-bit index, 16-bit values)
    }gammaRamp;                                      
    float            fGammaValueR;            //!< Red Gamma value within gamma ranges. 0.5 - 6.0
    float            fGammaValueG;            //!< Green Gamma value within gamma ranges. 0.5 - 6.0
    float            fGammaValueB;            //!< Blue Gamma value within gamma ranges. 0.5 - 6.0
} NVVIOGAMMACORRECTION;

//! Macro for constructing thevesion field of _NVVIOGAMMACORRECTION
#define NVVIOGAMMACORRECTION_VER   MAKE_NVAPI_VERSION(NVVIOGAMMACORRECTION,1)

//! Maximum number of ranges per channel
#define MAX_NUM_COMPOSITE_RANGE      2                      


typedef struct _NVVIOCOMPOSITERANGE
{
    NvU32   uRange;
    NvU32   uEnabled;
    NvU32   uMin;
    NvU32   uMax;
} NVVIOCOMPOSITERANGE;



// Device configuration (fields masks indicating NVVIOCONFIG fields to use for NvAPI_VIO_GetConfig/NvAPI_VIO_SetConfig() )
//
#define NVVIOCONFIG_SIGNALFORMAT            0x00000001      //!< fields: signalFormat
#define NVVIOCONFIG_DATAFORMAT              0x00000002      //!< fields: dataFormat
#define NVVIOCONFIG_OUTPUTREGION            0x00000004      //!< fields: outputRegion
#define NVVIOCONFIG_OUTPUTAREA              0x00000008      //!< fields: outputArea
#define NVVIOCONFIG_COLORCONVERSION         0x00000010      //!< fields: colorConversion
#define NVVIOCONFIG_GAMMACORRECTION         0x00000020      //!< fields: gammaCorrection
#define NVVIOCONFIG_SYNCSOURCEENABLE        0x00000040      //!< fields: syncSource and syncEnable
#define NVVIOCONFIG_SYNCDELAY               0x00000080      //!< fields: syncDelay
#define NVVIOCONFIG_COMPOSITESYNCTYPE       0x00000100      //!< fields: compositeSyncType
#define NVVIOCONFIG_FRAMELOCKENABLE         0x00000200      //!< fields: EnableFramelock
#define NVVIOCONFIG_422FILTER               0x00000400      //!< fields: bEnable422Filter
#define NVVIOCONFIG_COMPOSITETERMINATE      0x00000800      //!< fields: bCompositeTerminate (Not supported on Quadro FX 4000 SDI)         
#define NVVIOCONFIG_DATAINTEGRITYCHECK      0x00001000      //!< fields: bEnableDataIntegrityCheck (Not supported on Quadro FX 4000 SDI)
#define NVVIOCONFIG_CSCOVERRIDE             0x00002000      //!< fields: colorConversion override
#define NVVIOCONFIG_FLIPQUEUELENGTH         0x00004000      //!< fields: flipqueuelength control
#define NVVIOCONFIG_ANCTIMECODEGENERATION   0x00008000      //!< fields: bEnableANCTimeCodeGeneration
#define NVVIOCONFIG_COMPOSITE               0x00010000      //!< fields: bEnableComposite
#define NVVIOCONFIG_ALPHAKEYCOMPOSITE       0x00020000      //!< fields: bEnableAlphaKeyComposite
#define NVVIOCONFIG_COMPOSITE_Y             0x00040000      //!< fields: compRange
#define NVVIOCONFIG_COMPOSITE_CR            0x00080000      //!< fields: compRange
#define NVVIOCONFIG_COMPOSITE_CB            0x00100000      //!< fields: compRange
#define NVVIOCONFIG_FULL_COLOR_RANGE        0x00200000      //!< fields: bEnableFullColorRange
#define NVVIOCONFIG_RGB_DATA                0x00400000      //!< fields: bEnableRGBData
#define NVVIOCONFIG_RESERVED_SDIOUTPUTENABLE         0x00800000      //!< fields: bEnableSDIOutput
#define NVVIOCONFIG_STREAMS                 0x01000000      //!< fields: streams
#define NVVIOCONFIG_ANC_PARITY_COMPUTATION  0x02000000      //!< fields: ancParityComputation
#define NVVIOCONFIG_ANC_AUDIO_REPEAT		0x04000000      //!< fields: enableAudioBlanking
 

// Don't forget to update NVVIOCONFIG_VALIDFIELDS in nvapi.spec when NVVIOCONFIG_ALLFIELDS changes.
#define NVVIOCONFIG_ALLFIELDS   ( NVVIOCONFIG_SIGNALFORMAT          | \
                                  NVVIOCONFIG_DATAFORMAT            | \
                                  NVVIOCONFIG_OUTPUTREGION          | \
                                  NVVIOCONFIG_OUTPUTAREA            | \
                                  NVVIOCONFIG_COLORCONVERSION       | \
                                  NVVIOCONFIG_GAMMACORRECTION       | \
                                  NVVIOCONFIG_SYNCSOURCEENABLE      | \
                                  NVVIOCONFIG_SYNCDELAY             | \
                                  NVVIOCONFIG_COMPOSITESYNCTYPE     | \
                                  NVVIOCONFIG_FRAMELOCKENABLE       | \
                                  NVVIOCONFIG_422FILTER             | \
                                  NVVIOCONFIG_COMPOSITETERMINATE    | \
                                  NVVIOCONFIG_DATAINTEGRITYCHECK    | \
                                  NVVIOCONFIG_CSCOVERRIDE           | \
                                  NVVIOCONFIG_FLIPQUEUELENGTH       | \
                                  NVVIOCONFIG_ANCTIMECODEGENERATION | \
                                  NVVIOCONFIG_COMPOSITE             | \
                                  NVVIOCONFIG_ALPHAKEYCOMPOSITE     | \
                                  NVVIOCONFIG_COMPOSITE_Y           | \
                                  NVVIOCONFIG_COMPOSITE_CR          | \
                                  NVVIOCONFIG_COMPOSITE_CB          | \
                                  NVVIOCONFIG_FULL_COLOR_RANGE      | \
                                  NVVIOCONFIG_RGB_DATA              | \
                                  NVVIOCONFIG_RESERVED_SDIOUTPUTENABLE | \
                                  NVVIOCONFIG_STREAMS               | \
                                  NVVIOCONFIG_ANC_PARITY_COMPUTATION | \
								  NVVIOCONFIG_ANC_AUDIO_REPEAT )

#define NVVIOCONFIG_VALIDFIELDS  ( NVVIOCONFIG_SIGNALFORMAT          | \
                                   NVVIOCONFIG_DATAFORMAT            | \
                                   NVVIOCONFIG_OUTPUTREGION          | \
                                   NVVIOCONFIG_OUTPUTAREA            | \
                                   NVVIOCONFIG_COLORCONVERSION       | \
                                   NVVIOCONFIG_GAMMACORRECTION       | \
                                   NVVIOCONFIG_SYNCSOURCEENABLE      | \
                                   NVVIOCONFIG_SYNCDELAY             | \
                                   NVVIOCONFIG_COMPOSITESYNCTYPE     | \
                                   NVVIOCONFIG_FRAMELOCKENABLE       | \
                                   NVVIOCONFIG_RESERVED_SDIOUTPUTENABLE | \
                                   NVVIOCONFIG_422FILTER             | \
                                   NVVIOCONFIG_COMPOSITETERMINATE    | \
                                   NVVIOCONFIG_DATAINTEGRITYCHECK    | \
                                   NVVIOCONFIG_CSCOVERRIDE           | \
                                   NVVIOCONFIG_FLIPQUEUELENGTH       | \
                                   NVVIOCONFIG_ANCTIMECODEGENERATION | \
                                   NVVIOCONFIG_COMPOSITE             | \
                                   NVVIOCONFIG_ALPHAKEYCOMPOSITE     | \
                                   NVVIOCONFIG_COMPOSITE_Y           | \
                                   NVVIOCONFIG_COMPOSITE_CR          | \
                                   NVVIOCONFIG_COMPOSITE_CB          | \
                                   NVVIOCONFIG_FULL_COLOR_RANGE      | \
                                   NVVIOCONFIG_RGB_DATA              | \
                                   NVVIOCONFIG_RESERVED_SDIOUTPUTENABLE | \
                                   NVVIOCONFIG_STREAMS               | \
                                   NVVIOCONFIG_ANC_PARITY_COMPUTATION | \
								   NVVIOCONFIG_ANC_AUDIO_REPEAT)

#define NVVIOCONFIG_DRIVERFIELDS ( NVVIOCONFIG_OUTPUTREGION          | \
                                   NVVIOCONFIG_OUTPUTAREA            | \
                                   NVVIOCONFIG_COLORCONVERSION       | \
                                   NVVIOCONFIG_FLIPQUEUELENGTH)

#define NVVIOCONFIG_GAMMAFIELDS  ( NVVIOCONFIG_GAMMACORRECTION       )

#define NVVIOCONFIG_RMCTRLFIELDS ( NVVIOCONFIG_SIGNALFORMAT          | \
                                   NVVIOCONFIG_DATAFORMAT            | \
                                   NVVIOCONFIG_SYNCSOURCEENABLE      | \
                                   NVVIOCONFIG_COMPOSITESYNCTYPE     | \
                                   NVVIOCONFIG_FRAMELOCKENABLE       | \
                                   NVVIOCONFIG_422FILTER             | \
                                   NVVIOCONFIG_COMPOSITETERMINATE    | \
                                   NVVIOCONFIG_DATAINTEGRITYCHECK    | \
                                   NVVIOCONFIG_COMPOSITE             | \
                                   NVVIOCONFIG_ALPHAKEYCOMPOSITE     | \
                                   NVVIOCONFIG_COMPOSITE_Y           | \
                                   NVVIOCONFIG_COMPOSITE_CR          | \
                                   NVVIOCONFIG_COMPOSITE_CB)

#define NVVIOCONFIG_RMSKEWFIELDS ( NVVIOCONFIG_SYNCDELAY             )

#define NVVIOCONFIG_ALLOWSDIRUNNING_FIELDS ( NVVIOCONFIG_DATAINTEGRITYCHECK     | \
                                             NVVIOCONFIG_SYNCDELAY              | \
                                             NVVIOCONFIG_CSCOVERRIDE            | \
                                             NVVIOCONFIG_ANCTIMECODEGENERATION  | \
                                             NVVIOCONFIG_COMPOSITE              | \
                                             NVVIOCONFIG_ALPHAKEYCOMPOSITE      | \
                                             NVVIOCONFIG_COMPOSITE_Y            | \
                                             NVVIOCONFIG_COMPOSITE_CR           | \
                                             NVVIOCONFIG_COMPOSITE_CB           | \
                                             NVVIOCONFIG_ANC_PARITY_COMPUTATION)

                                             
 #define NVVIOCONFIG_RMMODESET_FIELDS ( NVVIOCONFIG_SIGNALFORMAT         | \
                                        NVVIOCONFIG_DATAFORMAT           | \
                                        NVVIOCONFIG_SYNCSOURCEENABLE     | \
                                        NVVIOCONFIG_FRAMELOCKENABLE      | \
                                        NVVIOCONFIG_COMPOSITESYNCTYPE	 | \
										NVVIOCONFIG_ANC_AUDIO_REPEAT)                                            
                                             

//! Output device configuration 
// No members can be deleted from below structure. Only add new members at the 
// end of the structure.
typedef struct _NVVIOOUTPUTCONFIG_V1
{
    NVVIOSIGNALFORMAT    signalFormat;                         //!< Signal format for video output
    NVVIODATAFORMAT      dataFormat;                           //!< Data format for video output
    NVVIOOUTPUTREGION    outputRegion;                         //!< Region for video output (Desktop mode)
    NVVIOOUTPUTAREA      outputArea;                           //!< Usable resolution for video output (safe area)
    NVVIOCOLORCONVERSION colorConversion;                      //!< Color conversion.
    NVVIOGAMMACORRECTION gammaCorrection;
    NvU32                syncEnable;                           //!< Sync enable (TRUE to use syncSource)
    NVVIOSYNCSOURCE      syncSource;                           //!< Sync source
    NVVIOSYNCDELAY       syncDelay;                            //!< Sync delay
    NVVIOCOMPSYNCTYPE    compositeSyncType;                    //!< Composite sync type
    NvU32                frameLockEnable;                      //!< Flag indicating whether framelock was on/off
    NvU32                psfSignalFormat;                      //!< Indicates whether contained format is PSF Signal format
    NvU32                enable422Filter;                      //!< Enables/Disables 4:2:2 filter
    NvU32                compositeTerminate;                   //!< Composite termination
    NvU32                enableDataIntegrityCheck;             //!< Enable data integrity check: true - enable, false - disable
    NvU32                cscOverride;                          //!< Use provided CSC color matrix to overwrite 
    NvU32                flipQueueLength;                      //!< Number of buffers used for the internal flipqueue
    NvU32                enableANCTimeCodeGeneration;          //!< Enable SDI ANC time code generation
    NvU32                enableComposite;                      //!< Enable composite
    NvU32                enableAlphaKeyComposite;              //!< Enable Alpha key composite
    NVVIOCOMPOSITERANGE  compRange;                            //!< Composite ranges
    NvU8                 reservedData[256];                    //!< Inicates last stored SDI output state TRUE-ON / FALSE-OFF
    NvU32                enableFullColorRange;                 //!< Flag indicating Full Color Range
    NvU32                enableRGBData;                        //!< Indicates data is in RGB format
} NVVIOOUTPUTCONFIG_V1;

typedef struct _NVVIOOUTPUTCONFIG_V2
{
    NVVIOSIGNALFORMAT    signalFormat;                         //!< Signal format for video output
    NVVIODATAFORMAT      dataFormat;                           //!< Data format for video output
    NVVIOOUTPUTREGION    outputRegion;                         //!< Region for video output (Desktop mode)
    NVVIOOUTPUTAREA      outputArea;                           //!< Usable resolution for video output (safe area)
    NVVIOCOLORCONVERSION colorConversion;                      //!< Color conversion.
    NVVIOGAMMACORRECTION gammaCorrection;
    NvU32                syncEnable;                           //!< Sync enable (TRUE to use syncSource)
    NVVIOSYNCSOURCE      syncSource;                           //!< Sync source
    NVVIOSYNCDELAY       syncDelay;                            //!< Sync delay
    NVVIOCOMPSYNCTYPE    compositeSyncType;                    //!< Composite sync type
    NvU32                frameLockEnable;                      //!< Flag indicating whether framelock was on/off
    NvU32                psfSignalFormat;                      //!< Indicates whether contained format is PSF Signal format
    NvU32                enable422Filter;                      //!< Enables/Disables 4:2:2 filter
    NvU32                compositeTerminate;                   //!< Composite termination
    NvU32                enableDataIntegrityCheck;             //!< Enable data integrity check: true - enable, false - disable
    NvU32                cscOverride;                          //!< Use provided CSC color matrix to overwrite
    NvU32                flipQueueLength;                      //!< Number of buffers used for the internal flip queue
    NvU32                enableANCTimeCodeGeneration;          //!< Enable SDI ANC time code generation
    NvU32                enableComposite;                      //!< Enable composite
    NvU32                enableAlphaKeyComposite;              //!< Enable Alpha key composite
    NVVIOCOMPOSITERANGE  compRange;                            //!< Composite ranges
    NvU8                 reservedData[256];                    //!< Indicates last stored SDI output state TRUE-ON / FALSE-OFF
    NvU32                enableFullColorRange;                 //!< Flag indicating Full Color Range
    NvU32                enableRGBData;                        //!< Indicates data is in RGB format
    NVVIOANCPARITYCOMPUTATION ancParityComputation;            //!< Enable HW ANC parity bit computation (auto/on/off)
} NVVIOOUTPUTCONFIG_V2;

typedef struct _NVVIOOUTPUTCONFIG_V3
{
    NVVIOSIGNALFORMAT    signalFormat;                         //!< Signal format for video output
    NVVIODATAFORMAT      dataFormat;                           //!< Data format for video output
    NVVIOOUTPUTREGION    outputRegion;                         //!< Region for video output (Desktop mode)
    NVVIOOUTPUTAREA      outputArea;                           //!< Usable resolution for video output (safe area)
    NVVIOCOLORCONVERSION colorConversion;                      //!< Color conversion.
    NVVIOGAMMACORRECTION gammaCorrection;
    NvU32                syncEnable;                           //!< Sync enable (TRUE to use syncSource)
    NVVIOSYNCSOURCE      syncSource;                           //!< Sync source
    NVVIOSYNCDELAY       syncDelay;                            //!< Sync delay
    NVVIOCOMPSYNCTYPE    compositeSyncType;                    //!< Composite sync type
    NvU32                frameLockEnable;                      //!< Flag indicating whether framelock was on/off
    NvU32                psfSignalFormat;                      //!< Indicates whether contained format is PSF Signal format
    NvU32                enable422Filter;                      //!< Enables/Disables 4:2:2 filter
    NvU32                compositeTerminate;                   //!< Composite termination
    NvU32                enableDataIntegrityCheck;             //!< Enable data integrity check: true - enable, false - disable
    NvU32                cscOverride;                          //!< Use provided CSC color matrix to overwrite
    NvU32                flipQueueLength;                      //!< Number of buffers used for the internal flip queue
    NvU32                enableANCTimeCodeGeneration;          //!< Enable SDI ANC time code generation
    NvU32                enableComposite;                      //!< Enable composite
    NvU32                enableAlphaKeyComposite;              //!< Enable Alpha key composite
    NVVIOCOMPOSITERANGE  compRange;                            //!< Composite ranges
    NvU8                 reservedData[256];                    //!< Indicates last stored SDI output state TRUE-ON / FALSE-OFF
    NvU32                enableFullColorRange;                 //!< Flag indicating Full Color Range
    NvU32                enableRGBData;                        //!< Indicates data is in RGB format
    NVVIOANCPARITYCOMPUTATION ancParityComputation;            //!< Enable HW ANC parity bit computation (auto/on/off)
	NvU32				 enableAudioBlanking;				   //!< Enable HANC audio blanking on repeat frames
} NVVIOOUTPUTCONFIG_V3;

//! Stream configuration
typedef struct _NVVIOSTREAM
{
    NvU32                   bitsPerComponent;                     //!< Bits per component
    NVVIOCOMPONENTSAMPLING  sampling;                             //!< Sampling   
    NvU32                   expansionEnable;                      //!< Enable/disable 4:2:2->4:4:4 expansion
    NvU32                   numLinks;                             //!< Number of active links
    struct
    {
        NvU32               jack;                                 //!< This stream's link[i] will use the specified (0-based) channel within the
        NvU32               channel;                              //!< specified (0-based) jack
    } links[NVAPI_MAX_VIO_LINKS_PER_STREAM];
} NVVIOSTREAM;

//! Input device configuration
typedef struct _NVVIOINPUTCONFIG
{
    NvU32                numRawCaptureImages;                  //!< numRawCaptureImages is the number of frames to keep in the capture queue. 
                                                               //!< must be between NVAPI_GVI_MIN_RAW_CAPTURE_IMAGES and NVAPI_GVI_MAX_RAW_CAPTURE_IMAGES, 
    NVVIOSIGNALFORMAT    signalFormat;                         //!< Signal format.
                                                               //!< Please note that both numRawCaptureImages and signalFormat should be set together.
    NvU32                numStreams;                           //!< Number of active streams.
    NVVIOSTREAM          streams[NVAPI_MAX_VIO_STREAMS];       //!< Stream configurations
    NvU32                bTestMode;                            //!< This attribute controls the GVI test mode.
                                                               //!< Possible values 0/1. When testmode enabled, the
                                                               //!< GVI device will generate fake data as quickly as possible.
} NVVIOINPUTCONFIG;

typedef struct _NVVIOCONFIG_V1
{
    NvU32                version;                              //!< Structure version
    NvU32                fields;                               //!< Caller sets to NVVIOCONFIG_* mask for fields to use
    NVVIOCONFIGTYPE      nvvioConfigType;                      //!< Input or Output configuration
    union                                                   
    {
        NVVIOINPUTCONFIG  inConfig;                            //!<  Input device configuration
        NVVIOOUTPUTCONFIG_V1 outConfig;                           //!<  Output device configuration
    }vioConfig; 
} NVVIOCONFIG_V1;


typedef struct _NVVIOCONFIG_V2
{
    NvU32                version;                              //!< Structure version
    NvU32                fields;                               //!< Caller sets to NVVIOCONFIG_* mask for fields to use
    NVVIOCONFIGTYPE      nvvioConfigType;                      //!< Input or Output configuration
    union
    {
        NVVIOINPUTCONFIG     inConfig;                         //!< Input device configuration
        NVVIOOUTPUTCONFIG_V2 outConfig;                        //!< Output device configuration
    }vioConfig;
} NVVIOCONFIG_V2;

typedef struct _NVVIOCONFIG_V3
{
    NvU32                version;                              //!< Structure version
    NvU32                fields;                               //!< Caller sets to NVVIOCONFIG_* mask for fields to use
    NVVIOCONFIGTYPE      nvvioConfigType;                      //!< Input or Output configuration
    union
    {
        NVVIOINPUTCONFIG     inConfig;                         //!< Input device configuration
        NVVIOOUTPUTCONFIG_V3 outConfig;                        //!< Output device configuration
    }vioConfig;
} NVVIOCONFIG_V3;
typedef NVVIOOUTPUTCONFIG_V3 NVVIOOUTPUTCONFIG;
typedef NVVIOCONFIG_V3 NVVIOCONFIG;

#define NVVIOCONFIG_VER1  MAKE_NVAPI_VERSION(NVVIOCONFIG_V1,1)
#define NVVIOCONFIG_VER2  MAKE_NVAPI_VERSION(NVVIOCONFIG_V2,2)
#define NVVIOCONFIG_VER3  MAKE_NVAPI_VERSION(NVVIOCONFIG_V3,3)
#define NVVIOCONFIG_VER   NVVIOCONFIG_VER3


typedef struct
{
    NvPhysicalGpuHandle                    hPhysicalGpu;                    //!< Handle to Physical GPU (This could be NULL for GVI device if its not binded)
    NvVioHandle                         hVioHandle;                     //!<handle to SDI Input/Output device
    NvU32                               vioId;                          //!<device Id of SDI Input/Output device
    NvU32                               outputId;            //!<deviceMask of the SDI display connected to GVO device. 
                                                                        //!<outputId will be 0 for GVI device.
} NVVIOTOPOLOGYTARGET;                                                    

typedef struct _NV_VIO_TOPOLOGY
{
    NvU32                       version;
    NvU32                       vioTotalDeviceCount;                    //!<How many video I/O targets are valid
    NVVIOTOPOLOGYTARGET         vioTarget[NVAPI_MAX_VIO_DEVICES];       //!<Array of video I/O targets
}NV_VIO_TOPOLOGY, NVVIOTOPOLOGY;


//! Macro for constructing the version field of NV_VIO_TOPOLOGY
#define NV_VIO_TOPOLOGY_VER  MAKE_NVAPI_VERSION(NV_VIO_TOPOLOGY,1)

//! Macro for constructing the version field of NVVIOTOPOLOGY
#define NVVIOTOPOLOGY_VER    MAKE_NVAPI_VERSION(NVVIOTOPOLOGY,1)



//! @} 



//! \addtogroup vidio
//! @{
///////////////////////////////////////////////////////////////////////////////
//!   
//!   Function:    NvAPI_VIO_GetCapabilities
//!  
//!   Description: This API determine the graphics adapter video I/O capabilities.
//!  
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!  
//! \param [in]  NvVioHandle   The caller provides the SDI device handle as input.
//! \param [out] pAdapterCaps  Pointer to receive capabilities
//!  
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_GetCapabilities(NvVioHandle     hVioHandle,
                                          NVVIOCAPS       *pAdapterCaps);


////////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_Open
//!  
//!   Description: This API opens the graphics adapter for video I/O operations
//!                using the OpenGL application interface.  Read operations
//!                are permitted in this mode by multiple clients, but Write 
//!                operations are application exclusive.
//!  
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!  
//! \param [in]  NvVioHandle      The caller provides the SDI output device handle as input.
//! \param [in]  vioClass         Class interface (NVVIOCLASS_* value)
//! \param [in]  ownerType        Specify NVVIOOWNERTYPE_APPLICATION or NVVIOOWNERTYPE_DESKTOP.
//!  
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_Open(NvVioHandle       hVioHandle,
                               NvU32             vioClass,
                               NVVIOOWNERTYPE    ownerType);

///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_Close
//!  
//!   Description: This API closes the graphics adapter for graphics-to-video operations
//!                using the OpenGL application interface.  Closing an 
//!                OpenGL handle releases the device.
//!  
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!  
//! \param [in]  NvVioHandle   The caller provides the SDI output device handle as input.
//! \param [in]  bRelease      boolean value to either keep or release ownership
//!  
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_Close(NvVioHandle       hVioHandle,
                                NvU32             bRelease);
///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_Status
//!  
//!   Description: This API gets the Video I/O LED status.
//!  
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!  
//! \param [in]  NvVioHandle   The caller provides the SDI device handle as input.
//! \param [out] pStatus       Return pointer to NVVIOSTATUS
//!  
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_Status(NvVioHandle     hVioHandle, 
                                 NVVIOSTATUS     *pStatus);

////////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_SyncFormatDetect
//!  
//!   Description: This API detects the Video I/O incoming sync video format.
//!  
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!  
//! \param [in]  NvVioHandle  The caller provides the SDI device handle as input.
//! \param [out] pWait        Pointer to receive how many milliseconds will lapse 
//!                           before VIOStatus returns the detected syncFormat.
//!  
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_SyncFormatDetect(NvVioHandle hVioHandle,
                                           NvU32       *pWait);
///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_GetConfig
//!  
//!   Description: This API gets the graphics-to-video configuration.
//!  
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!  
//! \param [in]  NvVioHandle   The caller provides the SDI device handle as input.
//! \param [out] pConfig       Pointer to the graphics-to-video configuration
//!  
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_GetConfig(NvVioHandle        hVioHandle,
                                    NVVIOCONFIG        *pConfig); 

///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_SetConfig
//!  
//!   Description: This API sets the graphics-to-video configuration.
//!  
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!  
//! \param [in]  NvVioHandle      The caller provides the SDI device handle as input.
//! \param [in]  pConfig          Pointer to Graphics-to-Video configuration
//!  
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_SetConfig(NvVioHandle            hVioHandle,
                                    const NVVIOCONFIG      *pConfig);

///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_SetCSC
//!  
//!   Description: This API sets the colorspace conversion parameters.
//!  
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_SetConfig.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!
//! \param [in]  NvVioHandle      The caller provides the SDI device handle as input.
//! \param [in]  pCSC             Pointer to CSC parameters
//!  
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
////////////////////////////////////////////////////////////////////////////////----
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_SetConfig.")
NVAPI_INTERFACE NvAPI_VIO_SetCSC(NvVioHandle           hVioHandle,
                                 NVVIOCOLORCONVERSION  *pCSC);
////////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_GetCSC
//! 
//!   Description: This API gets the colorspace conversion parameters.
//!
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_GetConfig.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!
//! \param [in]  NvVioHandle      The caller provides the SDI device handle as input.
//! \param [out] pCSC             Pointer to CSC parameters
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
////////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_GetConfig.")
NVAPI_INTERFACE NvAPI_VIO_GetCSC(NvVioHandle           hVioHandle,
                                 NVVIOCOLORCONVERSION  *pCSC);
///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_SetGamma
//! 
//!   Description: This API sets the gamma conversion parameters.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_SetConfig.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!
//! \param [in]  NvVioHandle       The caller provides the SDI device handle as input.
//! \param [in]  pGamma            Pointer to gamma parameters
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_SetConfig.")
NVAPI_INTERFACE NvAPI_VIO_SetGamma(NvVioHandle           hVioHandle,
                                   NVVIOGAMMACORRECTION  *pGamma);

///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_GetGamma
//! 
//!   Description: This API gets the gamma conversion parameters.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_GetConfig.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!
//! \param [in]  NvVioHandle      The caller provides the SDI device handle as input.
//! \param [out] pGamma           Pointer to gamma parameters
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_GetConfig.")
NVAPI_INTERFACE NvAPI_VIO_GetGamma(NvVioHandle           hVioHandle,
                                   NVVIOGAMMACORRECTION* pGamma);
////////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_SetSyncDelay
//! 
//!   Description: This API sets the sync delay parameters.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_SetConfig.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//! 
//! \param [in]  NvVioHandle   The caller provides the SDI device handle as input.
//! \param [in]  pSyncDelay    Pointer to sync delay parameters
//!  
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_SetConfig.")
NVAPI_INTERFACE NvAPI_VIO_SetSyncDelay(NvVioHandle            hVioHandle,
                                       const NVVIOSYNCDELAY   *pSyncDelay);

////////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_GetSyncDelay
//! 
//!   Description: This API gets the sync delay parameters.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_GetConfig.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//! 
//! \param [in]  NvVioHandle      The caller provides the SDI device handle as input.
//! \param [out] pSyncDelay       Pointer to sync delay parameters
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 290. Instead, use NvAPI_VIO_GetConfig.")
NVAPI_INTERFACE NvAPI_VIO_GetSyncDelay(NvVioHandle      hVioHandle,
                                       NVVIOSYNCDELAY   *pSyncDelay);

typedef enum _NVVIOPCILINKRATE
{
    NVVIOPCILINKRATE_UNKNOWN          = 0,
    NVVIOPCILINKRATE_GEN1             = 1,                    //<! 2.5 Gbps.
    NVVIOPCILINKRATE_GEN2             = 2,                    //<! 5 Gbps.
    NVVIOPCILINKRATE_GEN3             = 3,                    //<! 8 Gbps.
}NVVIOPCILINKRATE;

typedef enum _NVVIOPCILINKWIDTH
{
    NVVIOPCILINKWIDTH_UNKNOWN         = 0,
    NVVIOPCILINKWIDTH_x1              = 1,
    NVVIOPCILINKWIDTH_x2              = 2,
    NVVIOPCILINKWIDTH_x4              = 4,
    NVVIOPCILINKWIDTH_x8              = 8,
    NVVIOPCILINKWIDTH_x16            = 16,
}NVVIOPCILINKWIDTH;

typedef struct _NVVIOPCIINFO
{
    NvU32                     version;                            //!< Structure version
    
    NvU32                   pciDeviceId;                        //!< specifies the internal PCI device identifier for the GVI.
    NvU32                   pciSubSystemId;                        //!< specifies the internal PCI subsystem identifier for the GVI.
    NvU32                   pciRevisionId;                        //!< specifies the internal PCI device-specific revision identifier for the GVI.
    NvU32                   pciDomain;                            //!< specifies the PCI domain of the GVI device.
    NvU32                   pciBus;                                //!< specifies the PCI bus number of the GVI device.
    NvU32                   pciSlot;                            //!< specifies the PCI slot number of the GVI device.
    NVVIOPCILINKWIDTH       pciLinkWidth;                        //!< specifies the the negotiated PCIE link width.
    NVVIOPCILINKRATE           pciLinkRate;                        //!< specifies the the negotiated PCIE link rate.
} NVVIOPCIINFO_V1;

typedef NVVIOPCIINFO_V1                                         NVVIOPCIINFO;
#define NVVIOPCIINFO_VER1                                          MAKE_NVAPI_VERSION(NVVIOPCIINFO_V1,1)
#define NVVIOPCIINFO_VER                                        NVVIOPCIINFO_VER1

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_VIO_GetPCIInfo()
//
// DESCRIPTION:   This API gets PCI information of the attached SDI(input) capture card.
//
// PARAMETERS:      hVioHandle    (IN)    - Handle to SDI capture card.
//                  pVioPCIInfo    (OUT)    - PCI information of the attached SDI capture card.
//
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_GetPCIInfo(__in NvVioHandle hVioHandle, 
                                            __inout NVVIOPCIINFO* pVioPCIInfo);

////////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_IsRunning
//! 
//!   Description: This API determines if Video I/O is running.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//! 
//! \param [in]  NvVioHandle[IN]          The caller provides the SDI device handle as input.
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_IsRunning(NvVioHandle   hVioHandle);

///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_Start
//! 
//!   Description: This API starts Video I/O.
//!              This API should be called for NVVIOOWNERTYPE_DESKTOP only and will not work for OGL applications.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!
//! \param [in]  NvVioHandle[IN]     The caller provides the SDI device handle as input.
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_Start(NvVioHandle     hVioHandle);

///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_Stop
//! 
//!   Description: This API stops Video I/O.
//!              This API should be called for NVVIOOWNERTYPE_DESKTOP only and will not work for OGL applications.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//! 
//! \param [in]  NvVioHandle[IN]     The caller provides the SDI device handle as input.
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_Stop(NvVioHandle     hVioHandle);


///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_IsFrameLockModeCompatible
//! 
//!   Description: This API checks whether modes are compatible in frame lock mode.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//! 
//! \param [in]   NvVioHandle          The caller provides the SDI device handle as input.
//! \param [in]   srcEnumIndex         Source Enumeration index
//! \param [in]   destEnumIndex        Destination Enumeration index
//! \param [out]  pbCompatible         Pointer to receive compatibility
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_IsFrameLockModeCompatible(NvVioHandle              hVioHandle,
                                                    NvU32                    srcEnumIndex,
                                                    NvU32                    destEnumIndex,
                                                    NvU32*                   pbCompatible);


///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_EnumDevices
//! 
//!   Description: This API enumerate all VIO devices connected to the system.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!
//! \param [out]  NvVioHandle                  User passes the pointer of NvVioHandle[] array to get handles to
//!                                            all the connected video I/O devices.
//! \param [out]  vioDeviceCount               User gets total number of VIO devices connected to the system.
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_EnumDevices(NvVioHandle       hVioHandle[NVAPI_MAX_VIO_DEVICES],
                                      NvU32             *vioDeviceCount);
                                                                                          

///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_QueryTopology
//! 
//!   Description: This API queries the valid SDI topologies.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!
//! \param [out] pNvVIOTopology     User passes the pointer to NVVIOTOPOLOGY to fetch all valid SDI topologies.
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_QueryTopology(NV_VIO_TOPOLOGY   *pNvVIOTopology);


///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_EnumSignalFormats
//! 
//!   Description: This API enumerates signal formats supported by Video I/O.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//!
//! \param [in]   NvVioHandle          The caller provides the SDI device handle as input.
//! \param [in]   enumIndex            Enumeration index
//! \param [out]  pSignalFormatDetail  Pointer to receive detail or NULL
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_EnumSignalFormats(NvVioHandle              hVioHandle,
                                            NvU32                    enumIndex,
                                            NVVIOSIGNALFORMATDETAIL  *pSignalFormatDetail);

///////////////////////////////////////////////////////////////////////////////
//!   Function:    NvAPI_VIO_EnumDataFormats
//! 
//!   Description: This API enumerates data formats supported by Video I/O.
//! 
//! \deprecated  Do not use this function - it is deprecated in release 440.
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 190
//! 
//! \param [in]  NvVioHandle         The caller provides the SDI device handle as input.
//! \param [in]  enumIndex           Enumeration index
//! \param [out] pDataFormatDetail   Pointer to receive detail or NULL
//! 
//! \retval :: NVAPI_NOT_SUPPORTED   API is not supported 
//!
///////////////////////////////////////////////////////////////////////////////
__nvapi_deprecated_function("Do not use this function - it is deprecated in release 440.")
NVAPI_INTERFACE NvAPI_VIO_EnumDataFormats(NvVioHandle            hVioHandle,
                                          NvU32                  enumIndex,
                                          NVVIODATAFORMATDETAIL  *pDataFormatDetail);
                                                                                      

//! @}




///////////////////////////////////////////////////////////////////////////////////
//  CAMERA TEST API
//  These APIs allows test apps to perform low level camera tests

//! \addtogroup vidio
//! @{
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_CreateConfigurationProfileRegistryKey
//
//! \fn NvAPI_Stereo_CreateConfigurationProfileRegistryKey(NV_STEREO_REGISTRY_PROFILE_TYPE registryProfileType)
//!
//! DESCRIPTION:   Creates new configuration registry key for current application.
//!
//!                If there is no configuration profile prior to the function call,
//!                this API tries to create a new configuration profile registry key
//!                for a given application and fill it with the default values.
//!                If an application already has a configuration profile registry key, the API does nothing.
//!                The name of the key is automatically set to the name of the executable that calls this function.
//!                Because of this, the executable should have a distinct and unique name.
//!                If the application is using only one version of DirectX, then the default profile type will be appropriate.
//!                If the application is using more than one version of DirectX from the same executable,
//!                it should use the appropriate profile type for each configuration profile.
//!
//! HOW TO USE:    When there is a need for an application to have default stereo parameter values,
//!                use this function to create a key to store the values.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]    registryProfileType  Type of profile the application wants to create. It should be one of the symbolic constants defined in
//!                                     ::NV_STEREO_REGISTRY_PROFILE_TYPE. Any other value will cause function to do nothing and return
//!                                     ::NV_STEREO_REGISTRY_PROFILE_TYPE_NOT_SUPPORTED.
//!
//! \retval ::NVAPI_OK                                           Key exists in the registry.
//! \retval ::NVAPI_STEREO_REGISTRY_PROFILE_TYPE_NOT_SUPPORTED   This profile type is not supported.
//! \retval ::NVAPI_STEREO_REGISTRY_ACCESS_FAILED                Access to registry failed.
//! \retval ::NVAPI_API_NOT_INTIALIZED           
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED                       Stereo part of NVAPI not initialized.
//! \retval ::NVAPI_ERROR 
//!
///////////////////////////////////////////////////////////////////////////////


//! \ingroup stereoapi
//! Used in NvAPI_Stereo_CreateConfigurationProfileRegistryKey() 
typedef enum _NV_StereoRegistryProfileType
{
    NVAPI_STEREO_DEFAULT_REGISTRY_PROFILE, //!< Default registry configuration profile.
    NVAPI_STEREO_DX9_REGISTRY_PROFILE,     //!< Separate registry configuration profile for a DirectX 9 executable.
    NVAPI_STEREO_DX10_REGISTRY_PROFILE     //!< Separate registry configuration profile for a DirectX 10 executable.
} NV_STEREO_REGISTRY_PROFILE_TYPE;


//! \ingroup stereoapi
NVAPI_INTERFACE NvAPI_Stereo_CreateConfigurationProfileRegistryKey(NV_STEREO_REGISTRY_PROFILE_TYPE registryProfileType);




///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_DeleteConfigurationProfileRegistryKey
//
//! DESCRIPTION:   Removes configuration registry key for current application.
//!
//!                If an application already has a configuration profile prior to this function call,
//!                the function attempts to remove the application's configuration profile registry key from the registry.
//!                If there is no configuration profile registry key prior to the function call,
//!                the function does nothing and does not report an error.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]   registryProfileType   Type of profile that the application wants to delete. This should be one of the symbolic 
//!                                     constants defined in ::NV_STEREO_REGISTRY_PROFILE_TYPE. Any other value will cause the function 
//!                                     to do nothing and return ::NV_STEREO_REGISTRY_PROFILE_TYPE_NOT_SUPPORTED.
//!
//! \retval ::NVAPI_OK                                           Key does not exist in the registry any more.
//! \retval ::NVAPI_STEREO_REGISTRY_PROFILE_TYPE_NOT_SUPPORTED   This profile type is not supported.
//! \retval ::NVAPI_STEREO_REGISTRY_ACCESS_FAILED                Access to registry failed.
//! \retval ::NVAPI_API_NOT_INTIALIZED                           NVAPI is not initialized.
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED                       Stereo part of NVAPI is not initialized.
//! \retval ::NVAPI_ERROR 
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_DeleteConfigurationProfileRegistryKey(NV_STEREO_REGISTRY_PROFILE_TYPE registryProfileType);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_SetConfigurationProfileValue
//
//! \fn NvAPI_Stereo_SetConfigurationProfileValue(NV_STEREO_REGISTRY_PROFILE_TYPE registryProfileType, NV_STEREO_REGISTRY_ID valueRegistryID, void *pValue)
//!
//! DESCRIPTION:   This API sets the given parameter value under the application's registry key.
//!
//!                If the value does not exist under the application's registry key,
//!                the value will be created under the key.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]     registryProfileType  The type of profile the application wants to access. It should be one of the 
//!                                      symbolic constants defined in ::NV_STEREO_REGISTRY_PROFILE_TYPE. Any other value 
//!                                      will cause function to do nothing and return ::NV_STEREO_REGISTRY_PROFILE_TYPE_NOT_SUPPORTED.
//! \param [in]     valueRegistryID      ID of the value that is being set. It should be one of the symbolic constants defined in
//!                                      ::NV_STEREO_REGISTRY_PROFILE_TYPE. Any other value will cause function to do nothing
//!                                      and return ::NVAPI_STEREO_REGISTRY_VALUE_NOT_SUPPORTED.
//! \param [in]     pValue               Address of the value that is being set. It should be either address of a DWORD or of a float,
//!                                      dependent on the type of the stereo parameter whose value is being set. The API will then cast that
//!                                      address to DWORD* and write whatever is in those 4 bytes as a DWORD to the registry.
//!
//! \retval ::NVAPI_OK                                           Value is written to registry.
//! \retval ::NVAPI_STEREO_REGISTRY_PROFILE_TYPE_NOT_SUPPORTED   This profile type is not supported.
//! \retval ::NVAPI_STEREO_REGISTRY_VALUE_NOT_SUPPORTED          This value is not supported.
//! \retval ::NVAPI_STEREO_REGISTRY_ACCESS_FAILED                Access to registry failed.
//! \retval ::NVAPI_API_NOT_INTIALIZED                           NVAPI is not initialized.
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED                       Stereo part of NVAPI is not initialized.
//! \retval ::NVAPI_ERROR                                        Something is wrong (generic error).
//
///////////////////////////////////////////////////////////////////////////////


//! \ingroup stereoapi
//! Used in NvAPI_Stereo_SetConfigurationProfileValue()
typedef enum _NV_StereoRegistryID
{
    NVAPI_CONVERGENCE_ID,         //!< Symbolic constant for convergence registry ID.
    NVAPI_FRUSTUM_ADJUST_MODE_ID, //!< Symbolic constant for frustum adjust mode registry ID.
} NV_STEREO_REGISTRY_ID;


//! \ingroup stereoapi
NVAPI_INTERFACE NvAPI_Stereo_SetConfigurationProfileValue(NV_STEREO_REGISTRY_PROFILE_TYPE registryProfileType, NV_STEREO_REGISTRY_ID valueRegistryID, void *pValue);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_DeleteConfigurationProfileValue
//
//! DESCRIPTION:   This API removes the given value from the application's configuration profile registry key.
//!                If there is no such value, the function does nothing and does not report an error.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]     registryProfileType   The type of profile the application wants to access. It should be one of the 
//!                                       symbolic constants defined in ::NV_STEREO_REGISTRY_PROFILE_TYPE. Any other value will 
//!                                       cause function to do nothing and return ::NV_STEREO_REGISTRY_PROFILE_TYPE_NOT_SUPPORTED.
//! \param [in]     valueRegistryID       ID of the value that is being deleted. It should be one of the symbolic constants defined in
//!                                       ::NV_STEREO_REGISTRY_PROFILE_TYPE. Any other value will cause function to do nothing and return
//!                                       ::NVAPI_STEREO_REGISTRY_VALUE_NOT_SUPPORTED.
//!
//! \retval ::NVAPI_OK                                           Value does not exist in registry any more.
//! \retval ::NVAPI_STEREO_REGISTRY_PROFILE_TYPE_NOT_SUPPORTED   This profile type is not supported.
//! \retval ::NVAPI_STEREO_REGISTRY_VALUE_NOT_SUPPORTED          This value is not supported.
//! \retval ::NVAPI_STEREO_REGISTRY_ACCESS_FAILED                Access to registry failed.
//! \retval ::NVAPI_API_NOT_INTIALIZED 
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED                       Stereo part of NVAPI not initialized.
//! \retval ::NVAPI_ERROR 
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_DeleteConfigurationProfileValue(NV_STEREO_REGISTRY_PROFILE_TYPE registryProfileType, NV_STEREO_REGISTRY_ID valueRegistryID);






//! \addtogroup stereoapi
//! @{

typedef struct _NVAPI_STEREO_CAPS
{
    NvU32 version;
    NvU32 supportsWindowedModeOff        : 1;
    NvU32 supportsWindowedModeAutomatic  : 1;
    NvU32 supportsWindowedModePersistent : 1;
    NvU32 reserved                       : 29;  // must be 0
    NvU32 reserved2[3];                         // must be 0
} NVAPI_STEREO_CAPS_V1;

#define NVAPI_STEREO_CAPS_VER1  MAKE_NVAPI_VERSION(NVAPI_STEREO_CAPS,1)
#define NVAPI_STEREO_CAPS_VER   NVAPI_STEREO_CAPS_VER1

typedef NVAPI_STEREO_CAPS_V1    NVAPI_STEREO_CAPS;

//! @}

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_GetStereoSupport
//
//! DESCRIPTION:  This API checks what kind of stereo support is currently supported on a particular display.
//!               If the the display is prohibited from showing stereo (e.g. secondary in a multi-mon setup), we will 
//!               return 0 for all stereo modes (full screen exclusive, automatic windowed, persistent windowed).
//!               Otherwise, we will check which stereo mode is supported. On 120Hz display, this will be what
//!               the user chooses in control panel. On HDMI 1.4 display, persistent windowed mode is always assumed to be
//!               supported. Note that this function does not check if the CURRENT RESOLUTION/REFRESH RATE can support
//!               stereo. For HDMI 1.4, it is the app's responsibility to change the resolution/refresh rate to one that is
//!               3D compatible. For 120Hz, the driver will ALWAYS force 120Hz anyway.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 304
//!
//! \param [in]     hMonitor handle to monitor that app is going to run on
//! \param [out]    pCaps    Address where the result of the inquiry will be placed.
//!                          *pCaps is defined in NVAPI_STEREO_CAPS.
//! \return       This API can return any of the following error codes enumerated in #NvAPI_Status
//! \retval ::NVAPI_OK
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_GetStereoSupport(__in NvMonitorHandle hMonitor, __out NVAPI_STEREO_CAPS *pCaps);









///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_DecreaseSeparation
//
//! DESCRIPTION:   This API decreases separation for the given device interface (just like the Ctrl+F3 hotkey). 
//!
//! WHEN TO USE:   After the stereo handle for device interface is created via successfull call to the appropriate NvAPI_Stereo_CreateHandleFrom() function.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]   stereoHandle  Stereo handle that corresponds to the device interface.
//!
//! \retval ::NVAPI_OK - Decrease of separation percentage was successfull.
//! \retval ::NVAPI_STEREO_INVALID_DEVICE_INTERFACE - Device interface is not valid. Create again, then attach again.
//! \retval ::NVAPI_API_NOT_INTIALIZED - NVAPI not initialized.
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED - Stereo part of NVAPI not initialized.
//! \retval ::NVAPI_ERROR - Something is wrong (generic error).
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_DecreaseSeparation(StereoHandle stereoHandle);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_IncreaseSeparation
//
//! DESCRIPTION:   This API increases separation for the given device interface (just like the Ctrl+F4 hotkey).
//!
//! WHEN TO USE:   After the stereo handle for the device interface is created via successfull call to the appropriate NvAPI_Stereo_CreateHandleFrom() function.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]    stereoHandle  Stereo handle that corresponds to the device interface.
//!
//! \retval ::NVAPI_OK                               Increase of separation percentage was successfull.
//! \retval ::NVAPI_STEREO_INVALID_DEVICE_INTERFACE  Device interface is not valid. Create again, then attach again.
//! \retval ::NVAPI_API_NOT_INTIALIZED               NVAPI not initialized.
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED           Stereo part of NVAPI not initialized.
//! \retval ::NVAPI_ERROR                            Something is wrong (generic error).
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_IncreaseSeparation(StereoHandle stereoHandle);




///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_DecreaseConvergence
//
//! DESCRIPTION:   This API decreases convergence for the given device interface (just like the Ctrl+F5 hotkey). 
//!
//! WHEN TO USE:   After the stereo handle for the device interface is created via successfull call to the appropriate NvAPI_Stereo_CreateHandleFrom() function.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]     stereoHandle  Stereo handle that corresponds to the device interface.
//!
//! \retval ::NVAPI_OK - Decrease of convergence was successfull.
//! \retval ::NVAPI_STEREO_INVALID_DEVICE_INTERFACE - Device interface is not valid. Create again, then attach again.
//! \retval ::NVAPI_API_NOT_INTIALIZED - NVAPI not initialized.
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED - Stereo part of NVAPI not initialized.
//! \retval ::NVAPI_ERROR - Something is wrong (generic error).
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_DecreaseConvergence(StereoHandle stereoHandle);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_IncreaseConvergence
//
//! DESCRIPTION:   This API increases convergence for given the device interface (just like the Ctrl+F5 hotkey). 
//!
//! WHEN TO USE:   After the stereo handle for the device interface is created via successfull call to the appropriate NvAPI_Stereo_CreateHandleFrom() function.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]    stereoHandle  Stereo handle that corresponds to the device interface.
//!
//! \retval ::NVAPI_OK                               Increase of convergence was successfull.
//! \retval ::NVAPI_STEREO_INVALID_DEVICE_INTERFACE  Device interface is not valid. Create again, then attach again.
//! \retval ::NVAPI_API_NOT_INTIALIZED 
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED           Stereo part of NVAPI not initialized.
//! \retval ::NVAPI_ERROR 
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_IncreaseConvergence(StereoHandle stereoHandle);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_GetFrustumAdjustMode
//
//! \fn NvAPI_Stereo_GetFrustumAdjustMode(StereoHandle stereoHandle, NV_FRUSTUM_ADJUST_MODE *pFrustumAdjustMode)
//! DESCRIPTION:   This API gets the current frustum adjust mode value. 
//!
//! WHEN TO USE:   After the stereo handle for the device interface is created via successfull call to the appropriate NvAPI_Stereo_CreateHandleFrom() function.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]     stereoHandle         Stereo handle that corresponds to the device interface.
//! \param [out]    pFrustumAdjustMode   Address of the NV_FRUSTUM_ADJUST_MODE type variable to store current frustum value in.
//!
//! \retval ::NVAPI_OK - Retrieval of frustum adjust mode was successfull.
//! \retval ::NVAPI_STEREO_INVALID_DEVICE_INTERFACE - Device interface is not valid. Create again, then attach again.
//! \retval ::NVAPI_API_NOT_INTIALIZED - NVAPI not initialized.
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED - Stereo part of NVAPI not initialized.
//! \retval ::NVAPI_ERROR - Something is wrong (generic error).
//!
///////////////////////////////////////////////////////////////////////////////

//! \ingroup stereoapi
//! Used in NvAPI_Stereo_GetFrustumAdjustMode().
typedef enum _NV_FrustumAdjustMode
{
    NVAPI_NO_FRUSTUM_ADJUST,    //!< Do not adjust frustum.
    NVAPI_FRUSTUM_STRETCH,      //!< Stretch images in X.
    NVAPI_FRUSTUM_CLEAR_EDGES   //!< Clear corresponding edges for each eye.
} NV_FRUSTUM_ADJUST_MODE;

//! \ingroup stereoapi
NVAPI_INTERFACE NvAPI_Stereo_GetFrustumAdjustMode(StereoHandle stereoHandle, NV_FRUSTUM_ADJUST_MODE *pFrustumAdjustMode);





///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_SetFrustumAdjustMode
//
//! DESCRIPTION:   This API sets the current frustum adjust mode value. 
//!
//! WHEN TO USE:   After the stereo handle for the device interface is created via successfull call to the appropriate NvAPI_Stereo_CreateHandleFrom() function.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]     stereoHandle                Stereo handle that corresponds to the device interface.
//! \param [in]     newFrustumAdjustModeValue   New value for frustum adjust mode. It should be one of the symbolic constants defined in
//!                                             ::NV_FRUSTUM_ADJUST_MODE. Any other value will cause function to do nothing and return
//!                                             ::NVAPI_STEREO_FRUSTUM_ADJUST_MODE_NOT_SUPPORTED.
//!
//! \retval ::NVAPI_OK                                         Retrieval of frustum adjust mode was successfull.
//! \retval ::NVAPI_STEREO_INVALID_DEVICE_INTERFACE            Device interface is not valid. Create again, then attach again.
//! \retval ::NVAPI_API_NOT_INTIALIZED 
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED                     Stereo part of NVAPI not initialized.
//! \retval ::NVAPI_STEREO_FRUSTUM_ADJUST_MODE_NOT_SUPPORTED   Given frustum adjust mode is not supported.
//! \retval ::NVAPI_ERROR 
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_SetFrustumAdjustMode(StereoHandle stereoHandle, NV_FRUSTUM_ADJUST_MODE newFrustumAdjustModeValue);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_CaptureJpegImage
//
//! DESCRIPTION:   This API captures the current stereo image in JPEG stereo format with the given quality.
//!                Only the last capture call per flip will be effective. 
//!
//! WHEN TO USE:   After the stereo handle for the device interface is created via successfull call to the appropriate NvAPI_Stereo_CreateHandleFrom() function.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]     stereoHandle  Stereo handle that corresponds to the device interface.
//! \param [in]     quality        Quality of the JPEG image to be captured. Integer value betweeen 0 and 100.
//! 
//! \retval ::NVAPI_OK                              Image captured.
//! \retval ::NVAPI_STEREO_INVALID_DEVICE_INTERFACE Device interface is not valid. Create again, then attach again.
//! \retval ::NVAPI_API_NOT_INTIALIZED 
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED 
//! \retval ::NVAPI_STEREO_PARAMETER_OUT_OF_RANGE   Given quality is out of [0..100] range.
//! \retval ::NVAPI_ERROR 
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_CaptureJpegImage(StereoHandle stereoHandle, NvU32 quality);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_InitActivation
//
//! DESCRIPTION:   This API allows an application to enable stereo viewing, without the need of a GUID/Key pair
//!				   This API cannot be used to enable stereo viewing on 3DTV.
//!
//! HOW TO USE:    Call this function immediately after device creation, then follow with a reset. \n 
//!                Very generically:
//!                Create Device->Create Stereo Handle->InitActivation->Reset Device
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//! \since Release: 302
//!
//! \param [in]   stereoHandle            Stereo handle corresponding to the device interface.
//! \param [in]   bDelayed				  Use delayed activation
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, 
//!         they are listed below.
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED - Stereo part of NVAPI not initialized.
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////

//! \addtogroup stereoapi
//! @{

//! InitActivation Flags
typedef enum _NVAPI_STEREO_INIT_ACTIVATION_FLAGS
{
	NVAPI_STEREO_INIT_ACTIVATION_IMMEDIATE = 0X00,
    NVAPI_STEREO_INIT_ACTIVATION_DELAYED = 0x01,
} NVAPI_STEREO_INIT_ACTIVATION_FLAGS;

NVAPI_INTERFACE NvAPI_Stereo_InitActivation(__in StereoHandle hStereoHandle, __in NVAPI_STEREO_INIT_ACTIVATION_FLAGS flags);

//! @}

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_Trigger_Activation
//
//! DESCRIPTION:   This API allows an application to trigger creation of a stereo desktop, 
//!				   in case the creation was stopped on application launch. 
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//! \since Release: 302
//!
//! \param [in]   stereoHandle   Stereo handle that corresponds to the device interface.
//!
//! \return This API can return any of the error codes enumerated in #NvAPI_Status. 
//!         If there are return error codes with specific meaning for this API, 
//!         they are listed below.
//! \retval ::NVAPI_STEREO_INIT_ACTIVATION_NOT_DONE - Stereo InitActivation not called.
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED - Stereo part of NVAPI not initialized.
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_Trigger_Activation(__in StereoHandle hStereoHandle);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_CapturePngImage
//
//! DESCRIPTION:   This API captures the current stereo image in PNG stereo format.
//!                Only the last capture call per flip will be effective. 
//!
//! WHEN TO USE:   After the stereo handle for the device interface is created via successfull call to the appropriate NvAPI_Stereo_CreateHandleFrom() function.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//! \param [in]     stereoHandle  Stereo handle corresponding to the device interface.
//!
//! \retval ::NVAPI_OK                               Image captured.
//! \retval ::NVAPI_STEREO_INVALID_DEVICE_INTERFACE  Device interface is not valid. Create again, then attach again.
//! \retval ::NVAPI_API_NOT_INTIALIZED 
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED           Stereo part of NVAPI not initialized.
//! \retval ::NVAPI_ERROR 
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_CapturePngImage(StereoHandle stereoHandle);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_ReverseStereoBlitControl
//
//! DESCRIPTION:   This API turns on/off reverse stereo blit.
//!
//! HOW TO USE:    Use after the stereo handle for the device interface is created via successfull call to the appropriate 
//!                NvAPI_Stereo_CreateHandleFrom() function.
//!                After reversed stereo blit control is turned on, blits from the stereo surface will
//!                produce the right-eye image in the left side of the destination surface and the left-eye
//!                image in the right side of the destination surface.
//!
//!                In DirectX 9, the destination surface must be created as the render target, and StretchRect must be used.
//!                Conditions:
//!                - DstWidth == 2*SrcWidth
//!                - DstHeight == SrcHeight
//!                - Src surface is the stereo surface.
//!                - SrcRect must be {0,0,SrcWidth,SrcHeight}
//!                - DstRect must be {0,0,DstWidth,DstHeight}
//!
//!                In DirectX 10, ResourceCopyRegion must be used. 
//!                Conditions:
//!                - DstWidth == 2*SrcWidth
//!                - DstHeight == SrcHeight
//!                - dstX == 0,
//!                - dstY == 0,
//!                - dstZ == 0,
//!                - SrcBox: left=top=front==0; right==SrcWidth; bottom==SrcHeight; back==1;
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 185
//!
//! \param [in]    stereoHandle  Stereo handle corresponding to the device interface.
//! \param [in]    TurnOn         != 0 : Turns on \n 
//!                               == 0 : Turns off 
//!
//!
//! \retval ::NVAPI_OK                                Retrieval of frustum adjust mode was successfull.
//! \retval ::NVAPI_STEREO_INVALID_DEVICE_INTERFACE   Device interface is not valid. Create again, then attach again.
//! \retval ::NVAPI_API_NOT_INTIALIZED 
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED            Stereo part of NVAPI not initialized.
//! \retval ::NVAPI_ERROR 
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_ReverseStereoBlitControl(StereoHandle hStereoHandle, NvU8 TurnOn);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_Stereo_SetNotificationMessage
//
//! DESCRIPTION:   This API is a Setup notification message that the stereo driver uses to notify the application
//!                when the user changes the stereo driver state. 
//!
//!                When the user changes the stereo state (Activated or Deactivated, separation or conversion)
//!                the stereo driver posts a defined message with the following parameters:
//!
//!                lParam  is the current conversion. (Actual conversion is *(float*)&lParam )
//!          
//!                wParam == MAKEWPARAM(l, h) where
//!                - l == 0 if stereo is deactivated
//!                - l == 1 if stereo is deactivated
//!                - h is the current separation. (Actual separation is float(h*100.f/0xFFFF)  
//!                
//!                Call this API with NULL hWnd to prohibit notification.
//!
//! WHEN TO USE:   Use after the stereo handle for device interface is created via successful call to appropriate
//!                NvAPI_Stereo_CreateHandleFrom() function.
//!                
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 180
//!
//!
//! \param [in]     stereoHandle  Stereo handle corresponding to the device interface.
//! \param [in]     hWnd          Window HWND that will be notified when the user changes the stereo driver state.
//!                               Actual HWND must be cast to an NvU64.
//! \param [in]     messageID     MessageID of the message that will be posted to hWnd
//!
//! \retval ::NVAPI_OK                                Notification set.
//! \retval ::NVAPI_STEREO_INVALID_DEVICE_INTERFACE   Device interface is not valid. Create again, then attach again.
//! \retval ::NVAPI_API_NOT_INTIALIZED 
//! \retval ::NVAPI_STEREO_NOT_INITIALIZED            Stereo part of NVAPI not initialized.
//! \retval ::NVAPI_ERROR 
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_Stereo_SetNotificationMessage(StereoHandle hStereoHandle, NvU64 hWnd,NvU64 messageID);















//! \ingroup stereoapi
#define NVAPI_STEREO_QUADBUFFERED_API_VERSION   0x2

//! \ingroup stereoapi
 typedef enum _NV_StereoSwapChainMode
 {
   NVAPI_STEREO_SWAPCHAIN_DEFAULT = 0,
   NVAPI_STEREO_SWAPCHAIN_STEREO = 1,
   NVAPI_STEREO_SWAPCHAIN_MONO = 2,
 } NV_STEREO_SWAPCHAIN_MODE;

#if defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D1x_CreateSwapChain
//
//! DESCRIPTION: This API allows the user to create a mono or a stereo swap chain.
//!
//!          NOTE: NvAPI_D3D1x_CreateSwapChain is a wrapper of the method IDXGIFactory::CreateSwapChain which
//!                additionally notifies the D3D driver of the mode in which stereo mode the swap chain is to be
//!                created.
//!
//! \since Release: 285
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     hStereoHandle      Stereo handle that corresponds to the device interface.
//!                                   A pointer to the device that will write 2D images to the swap chain.
//! \param [in]     pDesc             A pointer to the swap-chain description (DXGI_SWAP_CHAIN_DESC). This parameter cannot be NULL.
//! \param [out]    ppSwapChain       A pointer to the swap chain created.
//! \param [in]     mode              The stereo mode fot the swap chain.
//!                                   NVAPI_STEREO_SWAPCHAIN_DEFAULT 
//!                                   NVAPI_STEREO_SWAPCHAIN_STEREO 
//!                                   NVAPI_STEREO_SWAPCHAIN_MONO 
//!
//! \retval ::NVAPI_OK                 The swap chain was created successfully.
//! \retval ::NVAPI_ERROR              The operation failed.
//!
//! \ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D1x_CreateSwapChain(StereoHandle hStereoHandle,
                                            DXGI_SWAP_CHAIN_DESC* pDesc,
                                            IDXGISwapChain** ppSwapChain,
                                            NV_STEREO_SWAPCHAIN_MODE mode);

#endif //if defined(__d3d10_h__) || defined(__d3d10_1_h__) || defined(__d3d11_h__)


#if defined(_D3D9_H_) //NvAPI_D3D9_CreateSwapChain
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_D3D9_CreateSwapChain
//
//! DESCRIPTION: This API allows the user to create a mono or a stereo swap chain.
//!
//!          NOTE: NvAPI_D3D9_CreateSwapChain is a wrapper of the method IDirect3DDevice9::CreateAdditionalSwapChain which
//!                additionally notifies the D3D driver if the swap chain creation mode must be stereo or mono.
//!
//!
//! \since Release: 285
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]      hStereoHandle            Stereo handle that corresponds to the device interface.
//! \param [in, out] pPresentationParameters  A pointer to the swap-chain description (DXGI). This parameter cannot be NULL.
//! \param [out]     ppSwapChain              A pointer to the swap chain created.
//! \param [in]      mode                     The stereo mode for the swap chain.
//!                                           NVAPI_STEREO_SWAPCHAIN_DEFAULT 
//!                                           NVAPI_STEREO_SWAPCHAIN_STEREO 
//!                                           NVAPI_STEREO_SWAPCHAIN_MONO 
//!
//! \retval ::NVAPI_OK                        The swap chain creation was successful 
//! \retval ::NVAPI_ERROR                     The operation failed.
//!
//!\ingroup stereoapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_D3D9_CreateSwapChain(StereoHandle hStereoHandle,
                                           D3DPRESENT_PARAMETERS *pPresentationParameters,
                                           IDirect3DSwapChain9 **ppSwapChain,
                                           NV_STEREO_SWAPCHAIN_MODE mode);
#endif //if defined(_D3D9_H_) //NvAPI_D3D9_CreateSwapChain





//! \addtogroup drsapi
//! @{


// GPU Profile APIs

NV_DECLARE_HANDLE(NvDRSSessionHandle);
NV_DECLARE_HANDLE(NvDRSProfileHandle);

#define NVAPI_DRS_GLOBAL_PROFILE                             ((NvDRSProfileHandle) -1)

#define NVAPI_SETTING_MAX_VALUES                             100

typedef enum _NVDRS_SETTING_TYPE
{
     NVDRS_DWORD_TYPE,
     NVDRS_BINARY_TYPE,
     NVDRS_STRING_TYPE,
     NVDRS_WSTRING_TYPE
} NVDRS_SETTING_TYPE;

typedef enum _NVDRS_SETTING_LOCATION
{
     NVDRS_CURRENT_PROFILE_LOCATION,
     NVDRS_GLOBAL_PROFILE_LOCATION,
     NVDRS_BASE_PROFILE_LOCATION,
     NVDRS_DEFAULT_PROFILE_LOCATION
} NVDRS_SETTING_LOCATION;


typedef struct _NVDRS_GPU_SUPPORT
{
    NvU32 geforce    :  1;
    NvU32 quadro     :  1;
    NvU32 nvs        :  1;
    NvU32 reserved4  :  1;
    NvU32 reserved5  :  1;
    NvU32 reserved6  :  1;
    NvU32 reserved7  :  1;
    NvU32 reserved8  :  1;
    NvU32 reserved9  :  1;
    NvU32 reserved10 :  1;
    NvU32 reserved11 :  1;
    NvU32 reserved12 :  1;
    NvU32 reserved13 :  1;
    NvU32 reserved14 :  1;
    NvU32 reserved15 :  1;
    NvU32 reserved16 :  1;
    NvU32 reserved17 :  1;
    NvU32 reserved18 :  1;
    NvU32 reserved19 :  1;
    NvU32 reserved20 :  1;
    NvU32 reserved21 :  1;
    NvU32 reserved22 :  1;
    NvU32 reserved23 :  1;
    NvU32 reserved24 :  1;
    NvU32 reserved25 :  1;
    NvU32 reserved26 :  1;
    NvU32 reserved27 :  1;
    NvU32 reserved28 :  1;
    NvU32 reserved29 :  1;
    NvU32 reserved30 :  1;
    NvU32 reserved31 :  1;
    NvU32 reserved32 :  1;
} NVDRS_GPU_SUPPORT;

//! Enum to decide on the datatype of setting value.
typedef struct _NVDRS_BINARY_SETTING 
{
     NvU32                valueLength;               //!< valueLength should always be in number of bytes.
     NvU8                 valueData[NVAPI_BINARY_DATA_MAX];
} NVDRS_BINARY_SETTING;

typedef struct _NVDRS_SETTING_VALUES
{
     NvU32                      version;                //!< Structure Version
     NvU32                      numSettingValues;       //!< Total number of values available in a setting.
     NVDRS_SETTING_TYPE         settingType;            //!< Type of setting value.  
     union                                              //!< Setting can hold either DWORD or Binary value or string. Not mixed types.
     {
         NvU32                      u32DefaultValue;    //!< Accessing default DWORD value of this setting.
         NVDRS_BINARY_SETTING       binaryDefaultValue; //!< Accessing default Binary value of this setting.
                                                        //!< Must be allocated by caller with valueLength specifying buffer size, or only valueLength will be filled in.
         NvAPI_UnicodeString        wszDefaultValue;    //!< Accessing default unicode string value of this setting.
     };
     union                                                //!< Setting values can be of either DWORD, Binary values or String type,
     {                                                    //!< NOT mixed types.
         NvU32                      u32Value;           //!< All possible DWORD values for a setting
         NVDRS_BINARY_SETTING       binaryValue;        //!< All possible Binary values for a setting
         NvAPI_UnicodeString        wszValue;           //!< Accessing current unicode string value of this setting.
     }settingValues[NVAPI_SETTING_MAX_VALUES];
} NVDRS_SETTING_VALUES;

//! Macro for constructing the version field of ::_NVDRS_SETTING_VALUES
#define NVDRS_SETTING_VALUES_VER    MAKE_NVAPI_VERSION(NVDRS_SETTING_VALUES,1)
     
typedef struct _NVDRS_SETTING_V1
{
     NvU32                      version;                //!< Structure Version
     NvAPI_UnicodeString        settingName;            //!< String name of setting
     NvU32                      settingId;              //!< 32 bit setting Id
     NVDRS_SETTING_TYPE         settingType;            //!< Type of setting value.  
     NVDRS_SETTING_LOCATION     settingLocation;        //!< Describes where the value in CurrentValue comes from. 
     NvU32                      isCurrentPredefined;    //!< It is different than 0 if the currentValue is a predefined Value, 
                                                        //!< 0 if the currentValue is a user value. 
     NvU32                      isPredefinedValid;      //!< It is different than 0 if the PredefinedValue union contains a valid value. 
     union                                              //!< Setting can hold either DWORD or Binary value or string. Not mixed types.
     {
         NvU32                      u32PredefinedValue;    //!< Accessing default DWORD value of this setting.
         NVDRS_BINARY_SETTING       binaryPredefinedValue; //!< Accessing default Binary value of this setting.
                                                           //!< Must be allocated by caller with valueLength specifying buffer size, 
                                                           //!< or only valueLength will be filled in.
         NvAPI_UnicodeString        wszPredefinedValue;    //!< Accessing default unicode string value of this setting.
     };
     union                                              //!< Setting can hold either DWORD or Binary value or string. Not mixed types.
     {
         NvU32                      u32CurrentValue;    //!< Accessing current DWORD value of this setting.
         NVDRS_BINARY_SETTING       binaryCurrentValue; //!< Accessing current Binary value of this setting.
                                                        //!< Must be allocated by caller with valueLength specifying buffer size, 
                                                        //!< or only valueLength will be filled in.
         NvAPI_UnicodeString        wszCurrentValue;    //!< Accessing current unicode string value of this setting.
     };                                                 
} NVDRS_SETTING_V1;

//! Macro for constructing the version field of ::_NVDRS_SETTING
#define NVDRS_SETTING_VER1        MAKE_NVAPI_VERSION(NVDRS_SETTING_V1, 1)

typedef NVDRS_SETTING_V1          NVDRS_SETTING;
#define NVDRS_SETTING_VER         NVDRS_SETTING_VER1

typedef struct _NVDRS_APPLICATION_V1
{
     NvU32                      version;            //!< Structure Version
     NvU32                      isPredefined;       //!< Is the application userdefined/predefined
     NvAPI_UnicodeString        appName;            //!< String name of the Application
     NvAPI_UnicodeString        userFriendlyName;   //!< UserFriendly name of the Application
     NvAPI_UnicodeString        launcher;           //!< Indicates the name (if any) of the launcher that starts the application  
} NVDRS_APPLICATION_V1;

typedef struct _NVDRS_APPLICATION_V2
{
     NvU32                      version;            //!< Structure Version
     NvU32                      isPredefined;       //!< Is the application userdefined/predefined
     NvAPI_UnicodeString        appName;            //!< String name of the Application
     NvAPI_UnicodeString        userFriendlyName;   //!< UserFriendly name of the Application
     NvAPI_UnicodeString        launcher;           //!< Indicates the name (if any) of the launcher that starts the Application
     NvAPI_UnicodeString        fileInFolder;       //!< Select this application only if this file is found.
                                                    //!< When specifying multiple files, separate them using the ':' character.
} NVDRS_APPLICATION_V2;

typedef struct _NVDRS_APPLICATION_V3
{
     NvU32                      version;            //!< Structure Version
     NvU32                      isPredefined;       //!< Is the application userdefined/predefined
     NvAPI_UnicodeString        appName;            //!< String name of the Application
     NvAPI_UnicodeString        userFriendlyName;   //!< UserFriendly name of the Application
     NvAPI_UnicodeString        launcher;           //!< Indicates the name (if any) of the launcher that starts the Application
     NvAPI_UnicodeString        fileInFolder;       //!< Select this application only if this file is found.
                                                    //!< When specifying multiple files, separate them using the ':' character.
     NvU32                      isMetro:1;          //!< Windows 8 style app
     NvU32                      isCommandLine:1;    //!< Command line parsing for the application name
     NvU32                      reserved:30;        //!< Reserved. Should be 0.
} NVDRS_APPLICATION_V3;

typedef struct _NVDRS_APPLICATION_V4
{
     NvU32                      version;            //!< Structure Version
     NvU32                      isPredefined;       //!< Is the application userdefined/predefined
     NvAPI_UnicodeString        appName;            //!< String name of the Application
     NvAPI_UnicodeString        userFriendlyName;   //!< UserFriendly name of the Application
     NvAPI_UnicodeString        launcher;           //!< Indicates the name (if any) of the launcher that starts the Application
     NvAPI_UnicodeString        fileInFolder;       //!< Select this application only if this file is found.
                                                    //!< When specifying multiple files, separate them using the ':' character.
     NvU32                      isMetro:1;          //!< Windows 8 style app
     NvU32                      isCommandLine:1;    //!< Command line parsing for the application name
     NvU32                      reserved:30;        //!< Reserved. Should be 0.
     NvAPI_UnicodeString        commandLine;        //!< If isCommandLine is set to 0 this must be an empty. If isCommandLine is set to 1 
                                                    //!< this contains application's command line as if it was returned by GetCommandLineW.
} NVDRS_APPLICATION_V4;

#define NVDRS_APPLICATION_VER_V1        MAKE_NVAPI_VERSION(NVDRS_APPLICATION_V1,1)
#define NVDRS_APPLICATION_VER_V2        MAKE_NVAPI_VERSION(NVDRS_APPLICATION_V2,2)
#define NVDRS_APPLICATION_VER_V3        MAKE_NVAPI_VERSION(NVDRS_APPLICATION_V3,3)
#define NVDRS_APPLICATION_VER_V4        MAKE_NVAPI_VERSION(NVDRS_APPLICATION_V4,4)

typedef NVDRS_APPLICATION_V4 NVDRS_APPLICATION;
#define NVDRS_APPLICATION_VER NVDRS_APPLICATION_VER_V4

typedef struct _NVDRS_PROFILE_V1
{
     NvU32                      version;            //!< Structure Version
     NvAPI_UnicodeString        profileName;        //!< String name of the Profile
     NVDRS_GPU_SUPPORT          gpuSupport;         //!< This read-only flag indicates the profile support on either
                                                    //!< Quadro, or Geforce, or both.
     NvU32                      isPredefined;       //!< Is the Profile user-defined, or predefined
     NvU32                      numOfApps;          //!< Total number of applications that belong to this profile. Read-only
     NvU32                      numOfSettings;      //!< Total number of settings applied for this Profile. Read-only
} NVDRS_PROFILE_V1;

typedef NVDRS_PROFILE_V1         NVDRS_PROFILE;

//! Macro for constructing the version field of ::NVDRS_PROFILE
#define NVDRS_PROFILE_VER1       MAKE_NVAPI_VERSION(NVDRS_PROFILE_V1,1)
#define NVDRS_PROFILE_VER        NVDRS_PROFILE_VER1


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_CreateSession
//
//!   DESCRIPTION: This API allocates memory and initializes the session.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [out]  *phSession Return pointer to the session handle.
//!                
//! \retval ::NVAPI_OK SUCCESS
//! \retval ::NVAPI_ERROR: For miscellaneous errors.
//
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_CreateSession(NvDRSSessionHandle *phSession);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_DestroySession
//
//!   DESCRIPTION: This API frees the allocation: cleanup of NvDrsSession.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in] hSession Input to the session handle.
//!                
//! \retval ::NVAPI_OK SUCCESS
//! \retval ::NVAPI_ERROR For miscellaneous errors.
//
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_DestroySession(NvDRSSessionHandle hSession);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_LoadSettings
//
//!   DESCRIPTION: This API loads and parses the settings data.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in] hSession  Input to the session handle.
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_LoadSettings(NvDRSSessionHandle hSession);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_SaveSettings
//
//!   DESCRIPTION: This API saves the settings data to the system.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in] hSession  Input to the session handle.
//!                
//! \retval ::NVAPI_OK    SUCCESS
//! \retval ::NVAPI_ERROR For miscellaneous errors.
//
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_SaveSettings(NvDRSSessionHandle hSession);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_LoadSettingsFromFile
//
//!   DESCRIPTION: This API loads settings from the given file path.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession Input to the session handle
//! \param [in]  fileName Binary File Name/Path
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_LoadSettingsFromFile(NvDRSSessionHandle hSession, NvAPI_UnicodeString fileName);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_SaveSettingsToFile
//
//!   DESCRIPTION: This API saves settings to the given file path.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession  Input to the session handle.
//! \param [in]  fileName  Binary File Name/Path
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_SaveSettingsToFile(NvDRSSessionHandle hSession, NvAPI_UnicodeString fileName);

//! @}



///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_CreateProfile
//
//!   DESCRIPTION: This API creates an empty profile.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession        Input to the session handle.
//! \param [in]  *pProfileInfo   Input pointer to NVDRS_PROFILE.
//! \param [in]  *phProfile      Returns pointer to profile handle.
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_CreateProfile(NvDRSSessionHandle hSession, NVDRS_PROFILE *pProfileInfo, NvDRSProfileHandle *phProfile);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_DeleteProfile
//
//!   DESCRIPTION: This API deletes a profile or sets it back to a predefined value.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in] hSession  Input to the session handle.
//! \param [in] hProfile  Input profile handle.
//!                
//! \retval ::NVAPI_OK     SUCCESS if the profile is found
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_DeleteProfile(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_SetCurrentGlobalProfile
//
//!   DESCRIPTION: This API sets the current global profile in the driver.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in] hSession                Input to the session handle.
//! \param [in] wszGlobalProfileName    Input current Global profile name.
//!               
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_SetCurrentGlobalProfile(NvDRSSessionHandle hSession, NvAPI_UnicodeString wszGlobalProfileName);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_GetCurrentGlobalProfile
//
//!   DESCRIPTION: This API returns the handle to the current global profile.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]   hSession     Input to the session handle.
//! \param [out]  *phProfile   Returns current Global profile handle.
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_GetCurrentGlobalProfile(NvDRSSessionHandle hSession, NvDRSProfileHandle *phProfile);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_GetProfileInfo
//
//!   DESCRIPTION: This API gets information about the given profile. User needs to specify the name of the Profile.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession       Input to the session handle.
//! \param [in]  hProfile       Input profile handle.
//! \param [out] *pProfileInfo  Return the profile info.
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_GetProfileInfo(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile, NVDRS_PROFILE *pProfileInfo);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_SetProfileInfo
//
//!   DESCRIPTION: Specifies flags for a given profile. Currently only the NVDRS_GPU_SUPPORT is
//!                used to update the profile. Neither the name, number of settings or applications
//!                or other profile information can be changed with this function. 
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession       Input to the session handle.
//! \param [in]  hProfile       Input profile handle.
//! \param [in]  *pProfileInfo  Input the new profile info.
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_SetProfileInfo(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile, NVDRS_PROFILE *pProfileInfo);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_FindProfileByName
//
//!   DESCRIPTION: This API finds a profile in the current session.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]   hSession      Input to the session handle.
//! \param [in]   profileName   Input profileName.
//! \param [out]  phProfile     Input profile handle.
//!                
//! \retval ::NVAPI_OK                SUCCESS if the profile is found
//! \retval ::NVAPI_PROFILE_NOT_FOUND if profile is not found
//! \retval ::NVAPI_ERROR             For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_FindProfileByName(NvDRSSessionHandle hSession, NvAPI_UnicodeString profileName, NvDRSProfileHandle* phProfile);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_EnumProfiles
//
//!   DESCRIPTION: This API enumerates through all the profiles in the session.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]   hSession        Input to the session handle.
//! \param [in]   index           Input the index for enumeration.
//! \param [out]  *phProfile      Returns profile handle.
//!                
//!   RETURN STATUS: NVAPI_OK: SUCCESS if the profile is found
//!                  NVAPI_ERROR: For miscellaneous errors.
//!                  NVAPI_END_ENUMERATION: index exceeds the total number of available Profiles in DB.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_EnumProfiles(NvDRSSessionHandle hSession, NvU32 index, NvDRSProfileHandle *phProfile);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_GetNumProfiles
//
//!   DESCRIPTION: This API obtains the number of profiles in the current session object.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession       Input to the session handle.
//! \param out]  *numProfiles   Returns count of profiles in the current hSession.
//!                
//! \retval ::NVAPI_OK                  SUCCESS
//! \retval ::NVAPI_API_NOT_INTIALIZED  Failed to initialize.
//! \retval ::NVAPI_INVALID_ARGUMENT    Invalid Arguments.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_GetNumProfiles(NvDRSSessionHandle hSession, NvU32 *numProfiles);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_CreateApplication
//
//!   DESCRIPTION: This API adds an executable name to a profile.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession       Input to the session handle.
//! \param [in]  hProfile       Input profile handle.
//! \param [in]  *pApplication  Input NVDRS_APPLICATION struct with the executable name to be added.
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_CreateApplication(NvDRSSessionHandle hSession, NvDRSProfileHandle  hProfile, NVDRS_APPLICATION *pApplication);
 

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_DeleteApplicationEx
//
//!   DESCRIPTION: This API removes an executable from a profile.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]   hSession  - Input to the session handle.
//! \param [in]   hProfile  - Input profile handle.
//! \param [in]   *pApp     - Input all the information about the application to be removed.
//!
//! \retval ::NVAPI_OK  SUCCESS
//! \retval ::NVAPI_ERROR For miscellaneous errors.
//! \retval ::NVAPI_EXECUTABLE_PATH_IS_AMBIGUOUS If the path provided could refer to two different executables,
//!                                              this error will be returned.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_DeleteApplicationEx(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile, NVDRS_APPLICATION *pApp);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_DeleteApplication
//
//!   DESCRIPTION: This API removes an executable name from a profile.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSessionPARAMETERS   Input to the session handle.
//! \param [in]  hProfile             Input profile handle.
//! \param [in]  appName              Input the executable name to be removed.
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//! \retval ::NVAPI_EXECUTABLE_PATH_IS_AMBIGUOUS If the path provided could refer to two different executables,
//!                                              this error will be returned
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_DeleteApplication(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile, NvAPI_UnicodeString appName);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_GetApplicationInfo
//
//!   DESCRIPTION: This API gets information about the given application.  The input application name
//!                must match exactly what the Profile has stored for the application. 
//!                This function is better used to retrieve application information from a previous
//!                enumeration.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]   hSession       Input to the session handle.
//! \param [in]   hProfile       Input profile handle.
//! \param [in]   appName        Input application name.
//! \param [out]  *pApplication  Returns NVDRS_APPLICATION struct with all the attributes.
//!                
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. 
//!          If there are return error codes with specific meaning for this API, 
//!          they are listed below.
//! \retval ::NVAPI_EXECUTABLE_PATH_IS_AMBIGUOUS   The application name could not 
//                                                single out only one executable.
//! \retval ::NVAPI_EXECUTABLE_NOT_FOUND           No application with that name is found on the profile.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_GetApplicationInfo(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile, NvAPI_UnicodeString appName, NVDRS_APPLICATION *pApplication);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_EnumApplications
//
//!   DESCRIPTION: This API enumerates all the applications in a given profile from the starting index to the maximum length.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]      hSession         Input to the session handle.
//! \param [in]      hProfile         Input profile handle.
//! \param [in]      startIndex       Indicates starting index for enumeration.
//! \param [in,out]  *appCount        Input maximum length of the passed in arrays. Returns the actual length.
//! \param [out]     *pApplication    Returns NVDRS_APPLICATION struct with all the attributes.
//!                
//! \retval ::NVAPI_OK               SUCCESS
//! \retval ::NVAPI_ERROR            For miscellaneous errors.
//! \retval ::NVAPI_END_ENUMERATION  startIndex exceeds the total appCount.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_EnumApplications(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile, NvU32 startIndex, NvU32 *appCount, NVDRS_APPLICATION *pApplication);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_FindApplicationByName
//
//!   DESCRIPTION: This API searches the application and the associated profile for the given application name.
//!                If a fully qualified path is provided, this function will always return the profile
//!                the driver will apply upon running the application (on the path provided).
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]      hSession       Input to the hSession handle
//! \param [in]      appName        Input appName. For best results, provide a fully qualified path of the type
//!                                 c:/Folder1/Folder2/App.exe
//! \param [out]     *phProfile     Returns profile handle.
//! \param [in,out]  *pApplication  Returns NVDRS_APPLICATION struct pointer.
//!                
//! \return  This API can return any of the error codes enumerated in #NvAPI_Status. 
//!                  If there are return error codes with specific meaning for this API, 
//!                  they are listed below:
//! \retval ::NVAPI_APPLICATION_NOT_FOUND          If App not found
//! \retval ::NVAPI_EXECUTABLE_PATH_IS_AMBIGUOUS   If the input appName was not fully qualified, this error might return in the case of multiple matches
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_FindApplicationByName(__in NvDRSSessionHandle hSession, __in NvAPI_UnicodeString appName, __out NvDRSProfileHandle *phProfile, __inout NVDRS_APPLICATION *pApplication);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_SetSetting
//
//!   DESCRIPTION: This API adds/modifies a setting to a profile.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession     Input to the session handle.
//! \param [in]  hProfile     Input profile handle.
//! \param [in]   *pSetting   Input NVDRS_SETTING struct pointer.
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_SetSetting(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile, NVDRS_SETTING *pSetting);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_GetSetting
//
//!   DESCRIPTION: This API gets information about the given setting.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]   hSession   Input to the session handle.
//! \param [in]   hProfile   Input profile handle.
//! \param [in]   settingId  Input settingId.
//! \param [out]  *pSetting  Returns all the setting info
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_GetSetting(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile, NvU32 settingId, NVDRS_SETTING *pSetting);


///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_EnumSettings
//
//!   DESCRIPTION: This API enumerates all the settings of a given profile from startIndex to the maximum length.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]      hSession        Input to the session handle.
//! \param [in]      hProfile        Input profile handle.
//! \param [in]      startIndex      Indicates starting index for enumeration.
//! \param [in,out]  *settingsCount  Input max length of the passed in arrays, Returns the actual length.
//! \param [out]     *pSetting       Returns all the settings info.
//!                
//! \retval ::NVAPI_OK              SUCCESS
//! \retval ::NVAPI_ERROR           For miscellaneous errors.
//! \retval ::NVAPI_END_ENUMERATION startIndex exceeds the total appCount.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_EnumSettings(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile, NvU32 startIndex, NvU32 *settingsCount, NVDRS_SETTING *pSetting);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_EnumAvailableSettingIds
//
//!   DESCRIPTION: This API enumerates all the Ids of all the settings recognized by NVAPI.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [out]    pSettingIds     User-provided array of length *pMaxCount that NVAPI will fill with IDs.
//! \param [in,out] pMaxCount       Input max length of the passed in array, Returns the actual length.
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!                  NVAPI_END_ENUMERATION: the provided pMaxCount is not enough to hold all settingIds.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_EnumAvailableSettingIds(NvU32 *pSettingIds, NvU32 *pMaxCount);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_EnumAvailableSettingValues
//
//!   DESCRIPTION: This API enumerates all available setting values for a given setting.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]      settingId          Input settingId.
//! \param [in,out]  pMaxNumValues      Input max length of the passed in arrays, Returns the actual length.
//! \param [out]     *pSettingValues    Returns all available setting values and its count.
//!                
//! \retval ::NVAPI_OK     SUCCESS
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_EnumAvailableSettingValues(NvU32 settingId, NvU32 *pMaxNumValues, NVDRS_SETTING_VALUES *pSettingValues);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_GetSettingIdFromName
//
//!   DESCRIPTION: This API gets the binary ID of a setting given the setting name.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]   settingName  Input Unicode settingName.
//! \param [out]  *pSettingId  Returns corresponding settingId.
//!                
//! \retval ::NVAPI_OK                 SUCCESS if the profile is found
//! \retval ::NVAPI_PROFILE_NOT_FOUND  if profile is not found
//! \retval ::NVAPI_SETTING_NOT_FOUND  if setting is not found
//! \retval ::NVAPI_ERROR              For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_GetSettingIdFromName(NvAPI_UnicodeString settingName, NvU32 *pSettingId);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_GetSettingNameFromId
//
//!   DESCRIPTION: This API gets the setting name given the binary ID.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  settingId        Input settingId.
//! \param [in]  *pSettingName    Returns corresponding Unicode settingName.
//!                
//! \retval ::NVAPI_OK                 SUCCESS if the profile is found
//! \retval ::NVAPI_PROFILE_NOT_FOUND  if profile is not found
//! \retval ::NVAPI_SETTING_NOT_FOUND  if setting is not found
//! \retval ::NVAPI_ERROR              For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_GetSettingNameFromId(NvU32 settingId, NvAPI_UnicodeString *pSettingName);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_DeleteProfileSetting
//
//!   DESCRIPTION: This API deletes a setting or sets it back to predefined value.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession            Input to the session handle.
//! \param [in]  hProfile            Input profile handle.
//! \param [in]  settingId           Input settingId to be deleted.
//!                
//! \retval ::NVAPI_OK     SUCCESS if the profile is found
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
/////////////////////////////////////////////////////////////////////////////// 
NVAPI_INTERFACE NvAPI_DRS_DeleteProfileSetting(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile, NvU32 settingId);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_RestoreAllDefaults
//
//!   DESCRIPTION: This API restores the whole system to predefined(default) values.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession  Input to the session handle.
//!                
//! \retval ::NVAPI_OK     SUCCESS if the profile is found
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
/////////////////////////////////////////////////////////////////////////////// 
NVAPI_INTERFACE NvAPI_DRS_RestoreAllDefaults(NvDRSSessionHandle hSession);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_RestoreProfileDefault
//
//!   DESCRIPTION: This API restores the given profile to predefined(default) values.
//!                Any and all user specified modifications will be removed. 
//!                If the whole profile was set by the user, the profile will be removed.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession  Input to the session handle.
//! \param [in]  hProfile  Input profile handle.
//!                
//! \retval ::NVAPI_OK              SUCCESS if the profile is found
//! \retval ::NVAPI_ERROR           For miscellaneous errors.
//! \retval ::NVAPI_PROFILE_REMOVED SUCCESS, and the hProfile is no longer valid.
//! \retval ::NVAPI_ERROR           For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_RestoreProfileDefault(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_RestoreProfileDefaultSetting
//
//!   DESCRIPTION: This API restores the given profile setting to predefined(default) values.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession  Input to the session handle.
//! \param [in]  hProfile  Input profile handle.
//! \param [in]  settingId Input settingId.
//!                
//! \retval ::NVAPI_OK     SUCCESS if the profile is found
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_RestoreProfileDefaultSetting(NvDRSSessionHandle hSession, NvDRSProfileHandle hProfile, NvU32 settingId);

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_DRS_GetBaseProfile
//
//!   DESCRIPTION: Returns the handle to the current global profile.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]  hSession    Input to the session handle.
//! \param [in]  *phProfile   Returns Base profile handle.
//!                
//! \retval ::NVAPI_OK     SUCCESS if the profile is found
//! \retval ::NVAPI_ERROR  For miscellaneous errors.
//!
//! \ingroup drsapi
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_DRS_GetBaseProfile(NvDRSSessionHandle hSession, NvDRSProfileHandle *phProfile);




//! \addtogroup sysgeneral
//! @{

typedef struct
{
    NvU32               version;            //!< structure version
    NvU32               vendorId;           //!< Chipset vendor identification
    NvU32               deviceId;           //!< Chipset device identification
    NvAPI_ShortString   szVendorName;       //!< Chipset vendor Name
    NvAPI_ShortString   szChipsetName;      //!< Chipset device Name
    NvU32               flags;              //!< Chipset info flags - obsolete
    NvU32               subSysVendorId;     //!< Chipset subsystem vendor identification
    NvU32               subSysDeviceId;     //!< Chipset subsystem device identification 
    NvAPI_ShortString   szSubSysVendorName; //!< subsystem vendor Name
    NvU32               HBvendorId;         //!< Host bridge vendor identification
    NvU32               HBdeviceId;         //!< Host bridge device identification
    NvU32               HBsubSysVendorId;   //!< Host bridge subsystem vendor identification
    NvU32               HBsubSysDeviceId;   //!< Host bridge subsystem device identification

} NV_CHIPSET_INFO_v4;

typedef struct
{
    NvU32               version;            //!< structure version
    NvU32               vendorId;           //!< vendor ID
    NvU32               deviceId;           //!< device ID
    NvAPI_ShortString   szVendorName;       //!< vendor Name
    NvAPI_ShortString   szChipsetName;      //!< device Name
    NvU32               flags;              //!< Chipset info flags - obsolete
    NvU32               subSysVendorId;     //!< subsystem vendor ID
    NvU32               subSysDeviceId;     //!< subsystem device ID
    NvAPI_ShortString   szSubSysVendorName; //!< subsystem vendor Name
} NV_CHIPSET_INFO_v3;

typedef enum
{
    NV_CHIPSET_INFO_HYBRID          = 0x00000001,
} NV_CHIPSET_INFO_FLAGS;

typedef struct
{
    NvU32               version;        //!< structure version
    NvU32               vendorId;       //!< vendor ID
    NvU32               deviceId;       //!< device ID
    NvAPI_ShortString   szVendorName;   //!< vendor Name
    NvAPI_ShortString   szChipsetName;  //!< device Name
    NvU32               flags;          //!< Chipset info flags
} NV_CHIPSET_INFO_v2;

typedef struct
{
    NvU32               version;        //structure version
    NvU32               vendorId;       //vendor ID
    NvU32               deviceId;       //device ID
    NvAPI_ShortString   szVendorName;   //vendor Name
    NvAPI_ShortString   szChipsetName;  //device Name
} NV_CHIPSET_INFO_v1;

#define NV_CHIPSET_INFO_VER_1  MAKE_NVAPI_VERSION(NV_CHIPSET_INFO_v1,1)
#define NV_CHIPSET_INFO_VER_2   MAKE_NVAPI_VERSION(NV_CHIPSET_INFO_v2,2)
#define NV_CHIPSET_INFO_VER_3   MAKE_NVAPI_VERSION(NV_CHIPSET_INFO_v3,3)
#define NV_CHIPSET_INFO_VER_4   MAKE_NVAPI_VERSION(NV_CHIPSET_INFO_v4,4)

#define NV_CHIPSET_INFO         NV_CHIPSET_INFO_v4
#define NV_CHIPSET_INFO_VER     NV_CHIPSET_INFO_VER_4

//! @}

///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_SYS_GetChipSetInfo
//
//!  This function returns information about the system's chipset.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 95
//!
//! \retval  NVAPI_INVALID_ARGUMENT              pChipSetInfo is NULL.
//! \retval  NVAPI_OK                           *pChipSetInfo is now set.
//! \retval  NVAPI_INCOMPATIBLE_STRUCT_VERSION   NV_CHIPSET_INFO version not compatible with driver.
//! \ingroup sysgeneral
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_SYS_GetChipSetInfo(NV_CHIPSET_INFO *pChipSetInfo);


//! \ingroup sysgeneral
//! Lid and dock information - used in NvAPI_GetLidDockInfo()
typedef struct 
{
    NvU32 version;    //! Structure version, constructed from the macro #NV_LID_DOCK_PARAMS_VER
    NvU32 currentLidState;
    NvU32 currentDockState;
    NvU32 currentLidPolicy;
    NvU32 currentDockPolicy;
    NvU32 forcedLidMechanismPresent;
    NvU32 forcedDockMechanismPresent;
}NV_LID_DOCK_PARAMS;


//! ingroup sysgeneral
#define NV_LID_DOCK_PARAMS_VER  MAKE_NVAPI_VERSION(NV_LID_DOCK_PARAMS,1)
///////////////////////////////////////////////////////////////////////////////
//
// FUNCTION NAME: NvAPI_GetLidDockInfo
//
//! DESCRIPTION: This function returns the current lid and dock information.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \since Release: 177
//!
//! \retval ::NVAPI_OK  
//! \retval ::NVAPI_ERROR
//! \retval ::NVAPI_NOT_SUPPORTED
//! \retval ::NVAPI_HANDLE_INVALIDATED
//! \retval ::NVAPI_API_NOT_INTIALIZED 
//!
//! \ingroup sysgeneral
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_SYS_GetLidAndDockInfo(NV_LID_DOCK_PARAMS *pLidAndDock);




///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_SYS_GetDisplayIdFromGpuAndOutputId
//
//! DESCRIPTION:     This API converts a Physical GPU handle and output ID to a
//!                  display ID.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     hPhysicalGpu   Handle to the physical GPU
//! \param [in]     outputId       Connected display output ID on the 
//!                                target GPU - must only have one bit set
//! \param [out]    displayId      Pointer to an NvU32 which contains
//!                                 the display ID
//!
//! \retval  ::NVAPI_OK - completed request
//! \retval  ::NVAPI_API_NOT_INTIALIZED - NVAPI not initialized
//! \retval  ::NVAPI_ERROR - miscellaneous error occurred
//! \retval  ::NVAPI_INVALID_ARGUMENT - Invalid input parameter.
//!
//! \ingroup sysgeneral
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_SYS_GetDisplayIdFromGpuAndOutputId(NvPhysicalGpuHandle hPhysicalGpu, NvU32 outputId, NvU32* displayId);


///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_SYS_GetGpuAndOutputIdFromDisplayId
//
//! DESCRIPTION:     This API converts a display ID to a Physical GPU handle and output ID.
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! \param [in]     displayId       Display ID of display to retrieve 
//!                                 GPU and outputId for
//! \param [out]    hPhysicalGpu    Handle to the physical GPU
//! \param [out]    outputId )      Connected display output ID on the 
//!                                 target GPU will only have one bit set.
//!
//! \retval ::NVAPI_OK 
//! \retval ::NVAPI_API_NOT_INTIALIZED 
//! \retval ::NVAPI_ID_OUT_OF_RANGE    The DisplayId corresponds to a 
//!                                    display which is not within the
//!                                    normal outputId range.
//! \retval ::NVAPI_ERROR   
//! \retval ::NVAPI_INVALID_ARGUMENT 
//!
//! \ingroup sysgeneral
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_SYS_GetGpuAndOutputIdFromDisplayId(NvU32 displayId, NvPhysicalGpuHandle *hPhysicalGpu, NvU32 *outputId);


///////////////////////////////////////////////////////////////////////////////
// FUNCTION NAME:   NvAPI_SYS_GetPhysicalGpuFromDisplayId
//
//! \code
//! DESCRIPTION:     This API retrieves the Physical GPU handle of the connected display
//!
//! \since Release: 313
//!
//! SUPPORTED OS:  Windows 7 and higher
//!
//!
//! PARAMETERS:      displayId(IN)     - Display ID of display to retrieve 
//!                                      GPU handle
//!                  hPhysicalGpu(OUT) - Handle to the physical GPU
//!
//! RETURN STATUS:
//!                  NVAPI_OK - completed request
//!                  NVAPI_API_NOT_INTIALIZED - NVAPI not initialized
//!                  NVAPI_ERROR - miscellaneous error occurred
//!                  NVAPI_INVALID_ARGUMENT - Invalid input parameter.
//! \endcode
//! \ingroup sysgeneral
///////////////////////////////////////////////////////////////////////////////
NVAPI_INTERFACE NvAPI_SYS_GetPhysicalGpuFromDisplayId(NvU32 displayId, NvPhysicalGpuHandle *hPhysicalGpu);




#ifdef __cplusplus
}; //extern "C" {

#endif

#pragma pack(pop)

#endif // _NVAPI_H

#include"nvapi_lite_salend.h"
