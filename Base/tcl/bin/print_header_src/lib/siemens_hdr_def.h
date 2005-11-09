
/* ---------------------------------------*/
/* Define the offsets in the siemens image header.*/
/* ---------------------------------------*/

/* offsets into siemens header */
#define SIEMENS_ENTRY_TABLE_OFFSET      0
#define SIEMENS_SYS_PARAM_OFFSET      256
#define SIEMENS_PAT_INFO_OFFSET       512
#define SIEMENS_MEAS_PARAM_OFFSET     768
#define SIEMENS_REVIEW_PARAM_OFFSET  1024
#define SIEMENS_SCAN_PARAM_OFFSET    1280
#define SIEMENS_IMAGE_INFO_OFFSET    1408
#define SIEMENS_PARAM_EVAL_OFFSET    1664
#define SIEMENS_DOC_PARAMS_OFFSET    1792
#define SIEMENS_SPEC_EVAL_OFFSET     1920
#define SIEMENS_IMAGE_TEXT_OFFSET    2432
#define SIEMENS_RESERVED_OFFSET      3456
/*
define some useful header offsets
*/
#define SIEMENS_WIDTH_OFFSET  SIEMENS_SCAN_PARAM_OFFSET + 48
#define SIEMENS_PIXEL_SIZE_OFFSET SIEMENS_PAT_INFO_OFFSET + 112
#define SIEMENS_SLICE_THICKNESS_OFFSET SIEMENS_REVIEW_PARAM_OFFSET + 64
#define SIEMENS_LOCATION_OFFSET SIEMENS_IMAGE_INFO_OFFSET + 136
#define SIEMENS_PID_OFFSET 3072+121
#define SIEMENS_SCAN_NO_OFFSET SIEMENS_IMAGE_INFO_OFFSET+152



#define SIEMENS_HDRSIZE_LEN   sizeof(short)
#define SIEMENS_WIDTH_LEN     sizeof(short)
#define SIEMENS_HEIGHT_LEN    sizeof(short)
#define SIEMENS_X_PIXSIZE_LEN sizeof(float)
#define SIEMENS_Y_PIXSIZE_LEN sizeof(float)
#define SIEMENS_THICK_LEN     sizeof(float)
#define SIEMENS_FOV_LEN       sizeof(float)
#define SIEMENS_LOCATION_LEN  sizeof(float)
#define SIEMENS_FILENAME_LEN  50
#define SIEMENS_SLICE_LEN     1
 

#define SIEMENS_HEIGHT_OFFSET     SIEMENS_WIDTH_OFFSET+SIEMENS_WIDTH_LEN
#define SIEMENS_X_PIXSIZE_OFFSET  SIEMENS_HEIGHT_OFFSET+SIEMENS_HEIGHT_LEN
#define SIEMENS_Y_PIXSIZE_OFFSET  SIEMENS_X_PIXSIZE_OFFSET+SIEMENS_X_PIXSIZE_LEN
#define SIEMENS_THICK_OFFSET      SIEMENS_Y_PIXSIZE_OFFSET+SIEMENS_Y_PIXSIZE_LEN
#define SIEMENS_FOV_OFFSET        SIEMENS_THICK_OFFSET+SIEMENS_THICK_LEN
#define SIEMENS_FILENAME_OFFSET   SIEMENS_LOCATION_OFFSET+SIEMENS_LOCATION_LEN
#define SIEMENS_SLICE_OFFSET      SIEMENS_FILENAME_OFFSET+SIEMENS_FILENAME_LEN

