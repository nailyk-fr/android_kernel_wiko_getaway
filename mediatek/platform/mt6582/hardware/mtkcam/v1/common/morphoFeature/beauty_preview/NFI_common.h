#ifndef _ACCESS_NFI_COMMON_H_
#define _ACCESS_NFI_COMMON_H_

enum NFI_DETECTION_MODE
{
	NFI_DETECTION_MODE_CAPTURE=0,
	NFI_DETECTION_MODE_PREVIEW
};

typedef struct tag_Point {
	int   x;
	int   y;
} NFI_Point;

typedef struct {
	int           nID;            /* ID Number */
	NFI_Point           ptLeftTop;      /* Left-Top     Face Coordinates    */
	NFI_Point           ptRightTop;     /* Right-Top    Face Coordinates    */
	NFI_Point           ptLeftBottom;   /* Left-Bottom  Face Coordinates    */
	NFI_Point           ptRightBottom;  /* Right-Bottom Face Coordinates    */
} NFI_DetectionInfo;

#endif