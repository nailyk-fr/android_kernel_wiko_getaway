#ifndef _ACCESS_BEAUTIFIER_H_
#define _ACCESS_BEAUTIFIER_H_

#define ACCESS_FACE_DETECTION_MODE_CAPTURE 0
#define ACCESS_FACE_DETECTION_MODE_PREVIEW 1


#ifdef __cplusplus
extern "C"
{
#endif
int Access_Beautifier_initialize(int maxFaceNum, int width, int height, int mode);
void Access_Beautifier_setting_smartskin(int ssRate);
void Access_Beautifier_setting_whitening(int whRate);
// int Access_Beautifier_function_420(BYTE *in_y, BYTE *in_uv, BYTE *out_y, BYTE *out_uv);
int Access_Beautifier_function_422(unsigned char *in_y);
void Access_Beautifier_finalize(void);

#ifdef __cplusplus
}
#endif
#endif
