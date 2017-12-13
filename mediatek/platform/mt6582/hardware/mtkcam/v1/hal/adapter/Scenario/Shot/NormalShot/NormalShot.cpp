/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#define LOG_TAG "MtkCam/Shot"
//
#include <fcntl.h>
#include <sys/stat.h>
#include <mtkcam/Log.h>
#include <mtkcam/common.h>
#include <mtkcam/common/hw/hwstddef.h>
//
#include <mtkcam/common/camutils/CamFormat.h>
#include <mtkcam/v1/camutils/CamInfo.h>
#include <mtkcam/hwutils/CameraProfile.h>
using namespace CPTool;
//
#include <mtkcam/hal/sensor_hal.h>
//
#include <mtkcam/camshot/ICamShot.h>
#include <mtkcam/camshot/ISingleShot.h>
//
#include <Shot/IShot.h>
//
#include "ImpShot.h"
#include "NormalShot.h"
#include <dlfcn.h>

#include <cutils/properties.h>
//
using namespace android;
using namespace NSShot;

#define SHUTTER_TIMING (NSCamShot::ECamShot_NOTIFY_MSG_EOF)
/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)(%s)[%s] "fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)(%s)[%s] "fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)(%s)[%s] "fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)(%s)[%s] "fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)(%s)[%s] "fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("(%d)(%s)[%s] "fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("(%d)(%s)[%s] "fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

#define ACCESS_BEAUTIFIER_USE

#ifdef ACCESS_BEAUTIFIER_USE
typedef int (*Pfn_Access_Beautifier_Initialize)(int maxFaceNum, int width, int height, int mode);
typedef void (*Pfn_Access_Beautifier_Setting_Smartskin)(int ssRate);
typedef void (*Pfn_Access_Beautifier_Setting_Whitening)(int whRate);
typedef int (*Pfn_Access_Beautifier_Function_422)(unsigned char *in_y);
typedef void (*Pfn_Access_Beautifier_Finalize)(void);

Pfn_Access_Beautifier_Initialize pfn_access_beautifier_initialize =NULL;
Pfn_Access_Beautifier_Setting_Smartskin pfn_access_beautifier_setting_smartskin =NULL;
Pfn_Access_Beautifier_Setting_Whitening pfn_access_beautifier_setting_whitening =NULL;
Pfn_Access_Beautifier_Function_422 pfn_access_beautifier_function_422 =NULL;
Pfn_Access_Beautifier_Finalize pfn_access_beautifier_finalize =NULL;
void *handle = NULL;

static int enable_beauty;
#endif

/******************************************************************************
 *
 ******************************************************************************/
extern "C"
sp<IShot>
createInstance_NormalShot(
    char const*const    pszShotName, 
    uint32_t const      u4ShotMode, 
    int32_t const       i4OpenId
)
{
    sp<IShot>       pShot = NULL;
    sp<NormalShot>  pImpShot = NULL;
    //
    //  (1.1) new Implementator.
    pImpShot = new NormalShot(pszShotName, u4ShotMode, i4OpenId);
    if  ( pImpShot == 0 ) {
        CAM_LOGE("[%s] new NormalShot", __FUNCTION__);
        goto lbExit;
    }
    //
    //  (1.2) initialize Implementator if needed.
    if  ( ! pImpShot->onCreate() ) {
        CAM_LOGE("[%s] onCreate()", __FUNCTION__);
        goto lbExit;
    }
    //
    //  (2)   new Interface.
    pShot = new IShot(pImpShot);
    if  ( pShot == 0 ) {
        CAM_LOGE("[%s] new IShot", __FUNCTION__);
        goto lbExit;
    }
    //
lbExit:
    //
    //  Free all resources if this function fails.
    if  ( pShot == 0 && pImpShot != 0 ) {
        pImpShot->onDestroy();
        pImpShot = NULL;
    }
    //
    return  pShot;
}


/******************************************************************************
 *  This function is invoked when this object is firstly created.
 *  All resources can be allocated here.
 ******************************************************************************/
bool
NormalShot::
onCreate()
{
#warning "[TODO] NormalShot::onCreate()"
    bool ret = true;

#ifdef ACCESS_BEAUTIFIER_USE
     String8 const s8LibPath = String8::format("/system/lib/libaccess_beautifier.so");
    if(handle == NULL)
  {
     handle = ::dlopen(s8LibPath.string(), RTLD_NOW);
     if ( !handle )
    {
        char const *err_str = ::dlerror();
        CAM_LOGE("dlopen library=%s %s", s8LibPath.string(), err_str?err_str:"unknown");
 	   return ret;
    }
      pfn_access_beautifier_initialize = (Pfn_Access_Beautifier_Initialize)::dlsym(handle, "Access_Beautifier_initialize");
      pfn_access_beautifier_setting_smartskin = (Pfn_Access_Beautifier_Setting_Smartskin)::dlsym(handle, "Access_Beautifier_setting_smartskin");
      pfn_access_beautifier_setting_whitening = (Pfn_Access_Beautifier_Setting_Whitening)::dlsym(handle, "Access_Beautifier_setting_whitening");
      pfn_access_beautifier_function_422 = (Pfn_Access_Beautifier_Function_422)::dlsym(handle, "Access_Beautifier_function_422");
      pfn_access_beautifier_finalize = (Pfn_Access_Beautifier_Finalize)::dlsym(handle, "Access_Beautifier_finalize");
    }
	CAM_LOGD("dlopen library pfn_access_beautifier_initialize:%d \n", pfn_access_beautifier_initialize);
#endif

    return ret;
}


/******************************************************************************
 *  This function is invoked when this object is ready to destryoed in the
 *  destructor. All resources must be released before this returns.
 ******************************************************************************/
void
NormalShot::
onDestroy()
{
#warning "[TODO] NormalShot::onDestroy()"
}


/******************************************************************************
 *
 ******************************************************************************/
NormalShot::
NormalShot(
    char const*const pszShotName, 
    uint32_t const u4ShotMode, 
    int32_t const i4OpenId
)
    : ImpShot(pszShotName, u4ShotMode, i4OpenId)
{
}


/******************************************************************************
 *
 ******************************************************************************/
NormalShot::
~NormalShot()
{
}


/******************************************************************************
 *
 ******************************************************************************/
bool
NormalShot::
sendCommand(
    uint32_t const  cmd, 
    uint32_t const  arg1, 
    uint32_t const  arg2
)
{
    AutoCPTLog cptlog(Event_Shot_sendCmd, cmd, arg1);
    bool ret = true;
    //
    switch  (cmd)
    {
    //  This command is to reset this class. After captures and then reset, 
    //  performing a new capture should work well, no matter whether previous 
    //  captures failed or not.
    //
    //  Arguments:
    //          N/A
    case eCmd_reset:
        ret = onCmd_reset();
        break;

    //  This command is to perform capture.
    //
    //  Arguments:
    //          N/A
    case eCmd_capture:
        ret = onCmd_capture();
        break;

    //  This command is to perform cancel capture.
    //
    //  Arguments:
    //          N/A
    case eCmd_cancel:
        onCmd_cancel();
        break;
    //
    default:
        ret = ImpShot::sendCommand(cmd, arg1, arg2);
    }
    //
    return ret;
}


/******************************************************************************
 *
 ******************************************************************************/
bool
NormalShot::
onCmd_reset()
{
#warning "[TODO] NormalShot::onCmd_reset()"
    bool ret = true;
    return ret;
}


/******************************************************************************
 *
 ******************************************************************************/
bool
NormalShot::
onCmd_capture()
{ 
    AutoCPTLog cptlog(Event_Shot_capture);
    MBOOL ret = MTRUE; 
	int reset_cp = 0;
    NSCamShot::ISingleShot *pSingleShot = NSCamShot::ISingleShot::createInstance(static_cast<EShotMode>(mu4ShotMode), "NormalShot"); 
    // 
    pSingleShot->init(); 

	
#ifdef ACCESS_BEAUTIFIER_USE
	MY_LOGD("before pfn_access_beautifier_initialize %d * %d\n", mShotParam.mi4PictureWidth, mShotParam.mi4PictureHeight);
	if(0 == mShotParam.mi4Rotation  || 180 == mShotParam.mi4Rotation){
		if(pfn_access_beautifier_initialize != NULL)
			reset_cp = pfn_access_beautifier_initialize(10, mShotParam.mi4PictureWidth, mShotParam.mi4PictureHeight, 0);
	}
	else{
		if(pfn_access_beautifier_initialize != NULL)
			reset_cp = pfn_access_beautifier_initialize(10, mShotParam.mi4PictureHeight, mShotParam.mi4PictureWidth, 0);
	}

	if(pfn_access_beautifier_setting_smartskin !=NULL)
		pfn_access_beautifier_setting_smartskin(mShotParam.mi4SmartSkin);
	if(pfn_access_beautifier_setting_whitening != NULL)
		pfn_access_beautifier_setting_whitening(mShotParam.mi4WhiteNing);
	MY_LOGD("after smartskin:%d  whitening %d\n",mShotParam.mi4SmartSkin, mShotParam.mi4WhiteNing);
	MY_LOGD("after pfn_access_beautifier_initialize:%d  mShotParam.mi4Rotation %d\n",reset_cp, mShotParam.mi4Rotation);

	enable_beauty = mShotParam.mi4EnableAccess;
#endif 

    // 
    pSingleShot->enableNotifyMsg( SHUTTER_TIMING ); 
    //
    EImageFormat ePostViewFmt = static_cast<EImageFormat>(android::MtkCamUtils::FmtUtils::queryImageioFormat(mShotParam.ms8PostviewDisplayFormat)); 


#ifdef ACCESS_BEAUTIFIER_USE
	pSingleShot->enableDataMsg(NSCamShot::ECamShot_DATA_MSG_JPEG | NSCamShot::ECamShot_DATA_MSG_YUV
                               | ((ePostViewFmt != eImgFmt_UNKNOWN) ? NSCamShot::ECamShot_DATA_MSG_POSTVIEW : NSCamShot::ECamShot_DATA_MSG_NONE)
                               ); 
#else
	pSingleShot->enableDataMsg(NSCamShot::ECamShot_DATA_MSG_JPEG
                               | ((ePostViewFmt != eImgFmt_UNKNOWN) ? NSCamShot::ECamShot_DATA_MSG_POSTVIEW : NSCamShot::ECamShot_DATA_MSG_NONE)
                               ); 
#endif

    // shot param 
    NSCamShot::ShotParam rShotParam(eImgFmt_YUY2,         //yuv format 
                         mShotParam.mi4PictureWidth,      //picutre width 
                         mShotParam.mi4PictureHeight,     //picture height
                         mShotParam.mi4Rotation,          //picture rotation 

//			    (getOpenId()==1? 1:0),
                         (getOpenId()==1? mShotParam.mi4Flip:0), //picture flip 		//add jpg flip 
//                      0,                               //picture flip 

	
                         ePostViewFmt,                    // postview format 
                         mShotParam.mi4PostviewWidth,      //postview width 
                         mShotParam.mi4PostviewHeight,     //postview height 
                         0,                               //postview rotation 
                         0,                               //postview flip 
                         mShotParam.mu4ZoomRatio           //zoom   
                        );                                  
 
    // jpeg param 
    NSCamShot::JpegParam rJpegParam(NSCamShot::ThumbnailParam(mJpegParam.mi4JpegThumbWidth, mJpegParam.mi4JpegThumbHeight, 
                                                                mJpegParam.mu4JpegThumbQuality, MTRUE),
                                                        mJpegParam.mu4JpegQuality,       //Quality 
                                                        MFALSE                            //isSOI 
                         ); 
 
                                                                     
    // sensor param 
    NSCamShot::SensorParam rSensorParam(static_cast<MUINT32>(MtkCamUtils::DevMetaInfo::queryHalSensorDev(getOpenId())),                             //Device ID 
                             ACDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG,         //Scenaio 
                             10,                                       //bit depth 
                             MFALSE,                                   //bypass delay 
                             MFALSE                                   //bypass scenario 
                            );  
    //
    pSingleShot->setCallbacks(fgCamShotNotifyCb, fgCamShotDataCb, this); 
    //     
    ret = pSingleShot->setShotParam(rShotParam); 
    
    //
    ret = pSingleShot->setJpegParam(rJpegParam); 

    // 
    ret = pSingleShot->startOne(rSensorParam); 
   
    //
    ret = pSingleShot->uninit(); 

#ifdef ACCESS_BEAUTIFIER_USE
	MY_LOGD("before pfn_access_beautifier_finalize \n");
	if(pfn_access_beautifier_finalize != NULL)
		pfn_access_beautifier_finalize();
	MY_LOGD("after pfn_access_beautifier_finalize \n");
#endif
    //
    pSingleShot->destroyInstance(); 


    return ret;
}


/******************************************************************************
 *
 ******************************************************************************/
void
NormalShot::
onCmd_cancel()
{
    AutoCPTLog cptlog(Event_Shot_cancel);
#warning "[TODO] NormalShot::onCmd_cancel()"
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL 
NormalShot::
fgCamShotNotifyCb(MVOID* user, NSCamShot::CamShotNotifyInfo const msg)
{
    AutoCPTLog cptlog(Event_Shot_handleNotifyCb);
    NormalShot *pNormalShot = reinterpret_cast <NormalShot *>(user); 
    if (NULL != pNormalShot) 
    {
        if ( SHUTTER_TIMING == msg.msgType) 
        {
            pNormalShot->mpShotCallback->onCB_Shutter(true, 
                                                      0
                                                     ); 
        }
    }

    return MTRUE; 
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
NormalShot::
fgCamShotDataCb(MVOID* user, NSCamShot::CamShotDataInfo const msg)
{
    NormalShot *pNormalShot = reinterpret_cast<NormalShot *>(user); 
    if (NULL != pNormalShot) 
    {
        if (NSCamShot::ECamShot_DATA_MSG_POSTVIEW == msg.msgType) 
        {
        	if(!enable_beauty){
		pNormalShot->handlePostViewData( msg.puData, msg.u4Size);  
		}  
        }
        else if (NSCamShot::ECamShot_DATA_MSG_JPEG == msg.msgType)
        {
            pNormalShot->handleJpegData(msg.puData, msg.u4Size, reinterpret_cast<MUINT8*>(msg.ext1), msg.ext2);
        }
	else if(NSCamShot::ECamShot_DATA_MSG_YUV == msg.msgType)
	{
		pNormalShot->handleYuvDataCallback(msg.puData, msg.u4Size);
	}
    }

    return MTRUE; 
}


/******************************************************************************
*
*******************************************************************************/
MBOOL
NormalShot::
handlePostViewData(MUINT8* const puBuf, MUINT32 const u4Size)
{
    AutoCPTLog cptlog(Event_Shot_handlePVData);
    MY_LOGD("+ (puBuf, size) = (%p, %d)", puBuf, u4Size); 
    mpShotCallback->onCB_PostviewDisplay(0, 
                                         u4Size, 
                                         reinterpret_cast<uint8_t const*>(puBuf)
                                        ); 

    MY_LOGD("-"); 
    return  MTRUE;
}

/******************************************************************************
*
*******************************************************************************/
MBOOL
NormalShot::
handleJpegData(MUINT8* const puJpegBuf, MUINT32 const u4JpegSize, MUINT8* const puThumbBuf, MUINT32 const u4ThumbSize)
{
    AutoCPTLog cptlog(Event_Shot_handleJpegData);
    MY_LOGD("+ (puJpgBuf, jpgSize, puThumbBuf, thumbSize) = (%p, %d, %p, %d)", puJpegBuf, u4JpegSize, puThumbBuf, u4ThumbSize); 

    MUINT8 *puExifHeaderBuf = new MUINT8[128 * 1024]; 
    MUINT32 u4ExifHeaderSize = 0; 

    CPTLogStr(Event_Shot_handleJpegData, CPTFlagSeparator, "makeExifHeader");
    makeExifHeader(eAppMode_PhotoMode, puThumbBuf, u4ThumbSize, puExifHeaderBuf, u4ExifHeaderSize); 
    MY_LOGD("(thumbbuf, size, exifHeaderBuf, size) = (%p, %d, %p, %d)", 
                      puThumbBuf, u4ThumbSize, puExifHeaderBuf, u4ExifHeaderSize); 

    // dummy raw callback 
    mpShotCallback->onCB_RawImage(0, 0, NULL);   

    // Jpeg callback 
    CPTLogStr(Event_Shot_handleJpegData, CPTFlagSeparator, "onCB_CompressedImage");
    mpShotCallback->onCB_CompressedImage(0,
                                         u4JpegSize, 
                                         reinterpret_cast<uint8_t const*>(puJpegBuf),
                                         u4ExifHeaderSize,                       //header size 
                                         puExifHeaderBuf,                    //header buf
                                         0,                       //callback index 
                                         true                     //final image 
                                         ); 
    MY_LOGD("-"); 

    delete [] puExifHeaderBuf; 

    return MTRUE; 

}

static bool
saveBufToFile(char const*const fname, MUINT8 *const buf, MUINT32 const size)
{
    int nw, cnt = 0;
    uint32_t written = 0;

    int fd = ::open(fname, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR|S_IRUSR);
    if (fd < 0) {
        return false;
    }

    while (written < size) {
        nw = ::write(fd,
                     buf + written,
                     size - written);
        if (nw < 0) {
            break;
        }
        written += nw;
        cnt++;
    }
    ::close(fd);
    return true;
}

MBOOL
NormalShot::
handleYuvDataCallback(MUINT8* const puBuf, MUINT32 const u4Size)
{

    MY_LOGD("[handleYuvDataCallback in %d", u4Size);
    MBOOL   ret = MFALSE;
    int result_ps = 0, enable;

   /* char value[32] = {'\0'};
    property_get("persist.camera.beautify.enable", value, "0");
    enable = atoi(value);    */
	
	/*MUINT8 *p = (MUINT8*)puBuf;			
	MUINT8* end_p = (MUINT8*)puBuf+ u4Size/5;
		while((p<(end_p)) )
		{
			*p = 0;	
			 p++;
		}
	MY_LOGD("pjvendor process  EXIT WHILE \n");		
	saveBufToFile("/sdcard/yuv.yuv", puBuf, u4Size);*/

#ifdef ACCESS_BEAUTIFIER_USE
	if(mShotParam.mi4EnableAccess && (mi4OpenId == 1)){
		MY_LOGD("before pfn_access_beautifier_function_422 %d * %d mi4OpenId: %d\n", mShotParam.mi4PictureWidth, mShotParam.mi4PictureHeight, mi4OpenId);
		if(pfn_access_beautifier_function_422 != NULL)
	    		result_ps = pfn_access_beautifier_function_422(puBuf);
		MY_LOGD("after pfn_access_beautifier_function_422 %d\n", result_ps);
	}
#endif
	return MTRUE;
}
