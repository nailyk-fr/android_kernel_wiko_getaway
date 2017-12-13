#ifndef _ACCESS_BEAUTIFIER_H_
#define _ACCESS_BEAUTIFIER_H_

#include <NFI_common.h>

#ifdef __cplusplus
extern "C"
{
#endif
/*
*/
int NFI_SkinBeauty_initialize_padding(int width, int height, int mode, int maxFaceNum, int strideY, int strideUV);
/*
Name: NFI_SkinBeauty_finalize
Description: Finalize library
Input: 
	[IN]nHandle: The handle of the instance
Output: 0 success, other failed
*/
int NFI_SkinBeauty_finalize(int nHandle);
/*
Name: NFI_SkinBeauty_setSmooth
Description: set skin smooth rate
Input: 
	[IN]nHandle: The handle of the instance
	[IN]ssRate: The skin smooth rate
Output: 0 success, other failed
*/
int NFI_SkinBeauty_setSmooth(int nHandle, int ssRate);
/*
Name: NFI_SkinBeauty_setwhitening
Description: set skin whitening rate
Input: 
	[IN]nHandle: The handle of the instance
	[IN]whRate: The skin whitening rate
Output: 0 success, other failed
*/
int NFI_SkinBeauty_setWhitening(int nHandle, int whRate);
/*
Name: NFI_SkinBeauty_function_420sp
Description: do skin beautifier
Input: 
	[IN]nHandle: The handle of the instance
	[IN]y: The Y data of the image
	[IN]uv: The UV data of the image
Output: 0 success, other failed
*/


int NFI_SkinBeauty_function_420p_padding(int nHandle, BYTE *y, BYTE *u, BYTE *v);

#ifdef __cplusplus
}
#endif
#endif
