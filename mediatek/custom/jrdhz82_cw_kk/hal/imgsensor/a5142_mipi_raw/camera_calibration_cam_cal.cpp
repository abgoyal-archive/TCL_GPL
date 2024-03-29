#define LOG_TAG "CamCalCamCal"

#include <cutils/xlog.h> //#include <utils/Log.h>
#include <cutils/properties.h>
#include <fcntl.h>
#include <math.h>

//seanlin 120921 for 658x #include "MediaHal.h"
//#include "src/lib/inc/MediaLog.h" //#include "src/lib/inc/MediaLog.h"
//#include "camera_custom_nvram.h"
#include "camera_custom_nvram.h"
//cam_cal
#include "cam_cal.h"
#include "cam_cal_define.h"
extern "C"{
//#include "cam_cal_layout.h"
#include "camera_custom_cam_cal.h"
}
#include "camera_calibration_cam_cal.h"

#include <stdio.h> //for rand?
#include <stdlib.h>  //sl121106 for atoi()//for rand?
//#include "src/core/scenario/camera/mhal_cam.h" //for timer

//COMMON


#define DEBUG_CALIBRATION_LOAD

#define CUSTOM_CAM_CAL_ROTATION_00 CUSTOM_CAM_CAL_ROTATION_0_DEGREE    
#define CUSTOM_CAM_CAL_ROTATION_01 CUSTOM_CAM_CAL_ROTATION_0_DEGREE    
#define CUSTOM_CAM_CAL_COLOR_ORDER_00 CUSTOM_CAM_CAL_COLOR_SHIFT_00     //SeanLin@20110629: 
#define CUSTOM_CAM_CAL_COLOR_ORDER_01 CUSTOM_CAM_CAL_COLOR_SHIFT_00 

//#define CUSTOM_CAM_CAL_PART_NUMBERS_START_ADD 5
//#define CUSTOM_CAM_CAL_NEW_MODULE_NUMBER_CHECK 1 //

#define CAM_CAL_SHOW_LOG 1
#define CAM_CAL_VER "ver8900~"   //83 : 6583, 00 : draft version 120920

#ifdef CAM_CAL_SHOW_LOG
//#define CAM_CAL_LOG(fmt, arg...)    LOGD(fmt, ##arg)
#define CAM_CAL_LOG(fmt, arg...)    XLOGD(CAM_CAL_VER " "fmt, ##arg)
#define CAM_CAL_ERR(fmt, arg...)    XLOGE(CAM_CAL_VER "Err: %5d: "fmt, __LINE__, ##arg)
#else
#define CAM_CAL_LOG(fmt, arg...)    void(0)
#define CAM_CAL_ERR(fmt, arg...)    void(0)
#endif
#define CAM_CAL_LOG_IF(cond, ...)      do { if ( (cond) ) { CAM_CAL_LOG(__VA_ARGS__); } }while(0)

////<
#if 0 ////seanlin 121016 for 658x 
UINT32 DoCamCalDefectLoad(INT32 CamcamFID, UINT32 start_addr, UINT32* pGetSensorCalData);
UINT32 DoCamCalPregainLoad(INT32 CamcamFID, UINT32 start_addr, UINT32* pGetSensorCalData);
UINT32 DoCamcalISPSlimShadingLoad(INT32 CamcamFID, UINT32 start_addr, UINT32* pGetSensorCalData);
UINT32 DoCamCalISPDynamicShadingLoad(INT32 CamcamFID, UINT32 start_addr, UINT32* pGetSensorCalData);
UINT32 DoCamCalISPFixShadingLoad(INT32 CamcamFID, UINT32 start_addr, UINT32* pGetSensorCalData);
UINT32 DoCamCalISPSensorShadingLoad(INT32 CamcamFID, UINT32 start_addr, UINT32* pGetSensorCalData);
#else			
UINT32 DoCamCalModuleVersion(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize,UINT32* pGetSensorCalData);
UINT32 DoCamCalPartNumber(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData);
//UINT32 DoCamCalShadingTable(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData);
UINT32 DoCamCalSingleLsc(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData);
UINT32 DoCamCalN3dLsc(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData);
UINT32 DoCamCalAWBGain(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData);
UINT32 DoCamCal2AGain(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData);
UINT32 DoCamCal3AGain(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData);
UINT32 DoCamCal3DGeo(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData);
#endif ////seanlin 121016 for 658x 
#if 0 ////seanlin 121016 for 658x 
enum
{
	CALIBRATION_LAYOUT_SLIM_LSC1 = 0,
	CALIBRATION_LAYOUT_SLIM_LSC2,
	CALIBRATION_LAYOUT_DYANMIC_LSC1,
	CALIBRATION_LAYOUT_DYANMIC_LSC2,
	CALIBRATION_LAYOUT_FIX_LSC1,
	CALIBRATION_LAYOUT_FIX_LSC2,
	CALIBRATION_LAYOUT_SENSOR_LSC1,
	CALIBRATION_LAYOUT_SENSOR_LSC2,
	CALIBRATION_LAYOUT_SUNNY_Q8N03D_LSC1,  //SL 110317
	MAX_CALIBRATION_LAYOUT_NUM
};
#define CALIBRATION_DATA_SIZE_SLIM_LSC1 	656
#define CALIBRATION_DATA_SIZE_SLIM_LSC2		3716
#define CALIBRATION_DATA_SIZE_DYANMIC_LSC1	2048
#define CALIBRATION_DATA_SIZE_DYANMIC_LSC2	5108
#define CALIBRATION_DATA_SIZE_FIX_LSC1		4944
#define CALIBRATION_DATA_SIZE_FIX_LSC2		8004
#define CALIBRATION_DATA_SIZE_SENSOR_LSC1	20
#define CALIBRATION_DATA_SIZE_SENSOR_LSC2	3088
#define CALIBRATION_DATA_SIZE_SUNNY_Q8N03D_LSC1 656 //SL 110317
#define MAX_CALIBRATION_DATA_SIZE			CALIBRATION_DATA_SIZE_FIX_LSC2

#endif ////seanlin 121016 for 658x  




#if 0 //from camera_custom_cam_cal.h
const MUINT32 CamCalReturnErr[CAMERA_CAM_CAL_DATA_LIST]= {
                                                    CAM_CAL_ERR_NO_VERSION,
                                                    CAM_CAL_ERR_NO_PARTNO,
                                                    CAM_CAL_ERR_NO_SHADING,
                                                    CAM_CAL_ERR_NO_3A_GAIN,
                                                    CAM_CAL_ERR_NO_3D_GEO};
typedef enum
{
    CAMERA_CAM_CAL_DATA_MODULE_VERSION=0,            //seanlin 121016 it's for user to get info. of single module or N3D module    
    CAMERA_CAM_CAL_DATA_PART_NUMBER,                      //seanlin 121016 return 5x4 byes gulPartNumberRegCamCal[5]
    CAMERA_CAM_CAL_DATA_SHADING_TABLE,                  //seanlin 121016 return SingleLsc or N3DLsc
    CAMERA_CAM_CAL_DATA_3A_GAIN,                              //seanlin 121016 return Single2A or N3D3A
    CAMERA_CAM_CAL_DATA_3D_GEO,                               //seanlin 121016 return none or N3D3D 
    CAMERA_CAM_CAL_DATA_LIST
} CAMERA_CAM_CAL_TYPE_ENUM;


#endif //from camera_custom_cam_cal.h


#if 0 //use the same CAMERA_CAM_CAL_TYPE_ENUM in camera_custom_cam_cal.h
enum
{
	CALIBRATION_ITEM_DEFECT = 0,
	CALIBRATION_ITEM_PREGAIN,
	CALIBRATION_ITEM_SHADING,
	MAX_CALIBRATION_ITEM_NUM	
};
#endif  //use the same CAMERA_CAM_CAL_TYPE_ENUM in camera_custom_cam_cal.h

#if 0 //use the same error code in camera_custom_cam_cal.h
static UINT32 GetCalErr[MAX_CALIBRATION_ITEM_NUM] =
{
	CAM_CAL_ERR_NO_DEFECT,
	CAM_CAL_ERR_NO_PREGAIN,
	CAM_CAL_ERR_NO_SHADING,
};
#endif //use the same error code in camera_custom_cam_cal.h

#if 1
//typedef enum
enum
{
	CALIBRATION_LAYOUT_SLIM_LSC1 = 0, //Legnacy module for 657x
	CALIBRATION_LAYOUT_N3D_DATA1, //N3D module for 658x
	CALIBRATION_LAYOUT_SUNNY_Q8N03D_LSC1,  //SL 110317
    CALIBRATION_LAYOUT_SENSOR_OTP,
	MAX_CALIBRATION_LAYOUT_NUM
};
//}CAM_CAL_MODULE_TYPE;

#else
#define 	CALIBRATION_LAYOUT_SLIM_LSC1  0 //Legnacy module for 657x
#define 	CALIBRATION_LAYOUT_N3D_DATA1 1 //N3D module for 658x
#define 	CALIBRATION_LAYOUT_SUNNY_Q8N03D_LSC1 2  //SL 110317
#define 	MAX_CALIBRATION_LAYOUT_NUM 3
#endif
#if 1
typedef enum // : MUINT32
{
    CAM_CAL_LAYOUT_RTN_PASS = 0x0,
    CAM_CAL_LAYOUT_RTN_FAILED = 0x1,
    CAM_CAL_LAYOUT_RTN_QUEUE = 0x2
} CAM_CAL_LAYOUT_T;
#else
#define CAM_CAL_LAYOUT_RTN_PASS  0x0
#define CAM_CAL_LAYOUT_RTN_FAILED  0x1
#define CAM_CAL_LAYOUT_RTN_QUEUE  0x2

#endif

typedef struct
{
    UINT16 Include; //calibration layout include this item?
    UINT32 StartAddr; // item Start Address
    UINT32 BlockSize;   //BlockSize
    UINT32 (*GetCalDataProcess)(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData);//(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData);
} CALIBRATION_ITEM_STRUCT;

typedef struct
{
	UINT32 HeaderAddr; //Header Address
	UINT32 HeaderId;   //Header ID
	UINT32 DataVer;   ////new for 658x CAM_CAL_SINGLE_EEPROM_DATA, CAM_CAL_SINGLE_OTP_DATA,CAM_CAL_N3D_DATA
//seanlin 121016 for 658x 	UINT32 CheckShading; // Do check shading ID?
//seanlin 121016 for 658x 	UINT32 ShadingID;    // Shading ID
	CALIBRATION_ITEM_STRUCT CalItemTbl[CAMERA_CAM_CAL_DATA_LIST];
} CALIBRATION_LAYOUT_STRUCT;


//static UINT8 gIsInitedCamCal = 0;//seanlin 121017 why static? Because cam_cal_drv will get data one block by one block instead of overall in one time.
const MUINT8 CamCalPartNumber[24]={0x57,0x61,0x6E,0x70,0x65,0x69,0x20,0x4C,0x69,0x61,0x6E,0x67,
	                                                       0x20,0x53,0x6F,0x70,0x68,0x69,0x65,0x52,0x79,0x61,0x6E,0x00};

const CALIBRATION_LAYOUT_STRUCT CalLayoutTbl[MAX_CALIBRATION_LAYOUT_NUM]=
{
	{//CALIBRATION_LAYOUT_SLIM_LSC1 without Defect //data sheet of excel : "Slim"
		0x00000000, 0x010200FF, CAM_CAL_SINGLE_EEPROM_DATA,
		{
			{0x00000001, 0x00000000, 0x00000000, DoCamCalModuleVersion}, //CAMERA_CAM_CAL_DATA_MODULE_VERSION
			{0x00000001, 0x00000000, 0x00000000, DoCamCalPartNumber}, //CAMERA_CAM_CAL_DATA_PART_NUMBER
			{0x00000001, 0x0000000C, 0x00000284, DoCamCalSingleLsc}, //CAMERA_CAM_CAL_DATA_SHADING_TABLE
			{0x00000001, 0x00000004, 0x00000008, DoCamCalAWBGain}, //CAMERA_CAM_CAL_DATA_3A_GAIN
			{0x00000000, 0x00000000, 0x00000000, DoCamCal3DGeo}  //CAMERA_CAM_CAL_DATA_3D_GEO
		}
	},
	{//CALIBRATION_LAYOUT_N3D //data sheet of excel : "3D_EEPROM 8M+2M _0A_2"
		0x00000000, 0x020A00FF,CAM_CAL_N3D_DATA,
		{
			{0x00000001, 0x00000002, 0x00000000, DoCamCalModuleVersion}, //CAMERA_CAM_CAL_DATA_MODULE_VERSION
			{0x00000001, 0x00000000, 0x00000018, DoCamCalPartNumber}, //CAMERA_CAM_CAL_DATA_PART_NUMBER
			{0x00000001, 0x1480009C, 0x00000840, DoCamCalN3dLsc}, //CAMERA_CAM_CAL_DATA_SHADING_TABLE
			{0x00000001, 0x1400001C, 0x00000080, DoCamCal3AGain}, //CAMERA_CAM_CAL_DATA_3A_GAIN
			{0x00000001, 0x00000A00, 0x00000898, DoCamCal3DGeo}  //CAMERA_CAM_CAL_DATA_3D_GEO
		}
	},	
	{//CALIBRATION_LAYOUT_SUNY
		0x00000000, 0x796e7573, CAM_CAL_SINGLE_EEPROM_DATA,
		{		
			{0x00000001, 0x00000000, 0x00000000, DoCamCalModuleVersion}, //CAMERA_CAM_CAL_DATA_MODULE_VERSION
			{0x00000001, 0x00000000, 0x00000000, DoCamCalPartNumber}, //CAMERA_CAM_CAL_DATA_PART_NUMBER
			{0x00000001, 0x0000000C, 0x00000284, DoCamCalSingleLsc}, //CAMERA_CAM_CAL_DATA_SHADING_TABLE
			{0x00000001, 0x00000004, 0x00000008, DoCamCalAWBGain}, //CAMERA_CAM_CAL_DATA_3A_GAIN
			{0x00000000, 0x00000000, 0x00000000, DoCamCal3DGeo}  //CAMERA_CAM_CAL_DATA_3D_GEO
		}
	},	
	{//CALIBRATION_LAYOUT_SENSOR_OTP
		0x00003806, 0x00000000, CAM_CAL_SINGLE_OTP_DATA,
		{		
			{0x00000001, 0x00000000, 0x00000000, DoCamCalModuleVersion}, //CAMERA_CAM_CAL_DATA_MODULE_VERSION
			{0x00000001, 0x00000005, 0x00000002, DoCamCalPartNumber}, //CAMERA_CAM_CAL_DATA_PART_NUMBER
			{0x00000000, 0x00000017, 0x00000001, DoCamCalSingleLsc}, //CAMERA_CAM_CAL_DATA_SHADING_TABLE
			{0x00000001, 0x00003810, 0x0000000E, DoCamCal2AGain}, //CAMERA_CAM_CAL_DATA_3A_GAIN
			{0x00000000, 0x00000000, 0x00000000, DoCamCal3DGeo}  //CAMERA_CAM_CAL_DATA_3D_GEO
		}
	}	
};

//static CAM_CAL_LAYOUT_T gIsInitedCamCal = CAM_CAL_LAYOUT_RTN_QUEUE;//seanlin 121017 why static? Because cam_cal_drv will get data one block by one block instead of overall in one time.
static UINT16 LayoutType = (MAX_CALIBRATION_LAYOUT_NUM+1); //seanlin 121017 why static? Because cam_cal_drv will get data one block by one block instead of overall in one time.
static bool bFirstLoad = TRUE;    
static MINT32 dumpEnable=1; //longxuewei

static CAM_CAL_LAYOUT_T  gIsInitedCamCal = CAM_CAL_LAYOUT_RTN_QUEUE;//(CAM_CAL_LAYOUT_T)CAM_CAL_LAYOUT_RTN_QUEUE;//seanlin 121017 why static? Because cam_cal_drv will get data one block by one block instead of overall in one time.
//MUINT32 gIsInitedCamCal = CAM_CAL_LAYOUT_RTN_QUEUE;//(CAM_CAL_LAYOUT_T)CAM_CAL_LAYOUT_RTN_QUEUE;//seanlin 121017 why static? Because cam_cal_drv will get data one block by one block instead of overall in one time.

UINT32 ShowCmdErrorLog(CAMERA_CAM_CAL_TYPE_ENUM cmd)
{
       CAM_CAL_ERR("Return ERROR %s\n",CamCalErrString[cmd]);
       return 0;
}
#if 0 //for test, no use currently
UINT32 DoReadDataByCmd(CAMERA_CAM_CAL_TYPE_ENUM Command, NT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize,UINT8* pGetSensorCalData)
{
    stCAM_CAL_INFO_STRUCT  cam_calCfg;
    UINT32 ioctlerr, err;
    cam_calCfg.u4Offset = start_addr;
    cam_calCfg.u4Length = BlockSize; //sizeof(ucModuleNumber)
    cam_calCfg.pu1Params= pGetSensorCalData;
    ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
    if(!ioctlerr)
    {
        err = CAM_CAL_ERR_NO_ERR;
    }
    else
    {
        err = CAM_CAL_ERR_NO_DEVICE;
        CAM_CAL_ERR("ioctl err\n");
        ShowCmdErrorLog(Command);
    }
    return err;
}
#endif //for test, no use currently
UINT32 DoCamCalModuleVersion(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize,UINT32* pGetSensorCalData)
{
    PCAM_CAL_DATA_STRUCT pCamCalData = (PCAM_CAL_DATA_STRUCT)pGetSensorCalData;
    UINT32 err=  0;
    /*
    if(start_addr<CAM_CAL_TYPE_NUM)
    {
        pCamCalData->DataVer = start_addr;    
    }
    else
    {
       err =  CamCalReturnErr[pCamCalData->Command];       
       ShowCmdErrorLog(pCamCalData->Command);

    }*/
    #ifdef DEBUG_CALIBRATION_LOAD
    CAM_CAL_LOG_IF(dumpEnable,"======================Module version==================\n");
    CAM_CAL_LOG_IF(dumpEnable,"[DataVer] = 0x%x\n", pCamCalData->DataVer);
    CAM_CAL_LOG_IF(dumpEnable,"RETURN = 0x%x \n", err);            
    CAM_CAL_LOG_IF(dumpEnable,"======================Module version==================\n");
    #endif    
    return err;
}

UINT32 DoCamCalPartNumber(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData)
{
    stCAM_CAL_INFO_STRUCT  cam_calCfg;
    PCAM_CAL_DATA_STRUCT pCamCalData = (PCAM_CAL_DATA_STRUCT)pGetSensorCalData;
    MUINT32 idx;    
    UINT32 ioctlerr;
    UINT32 err =  CamCalReturnErr[pCamCalData->Command];     
    UINT8 ucModuleNumber[CAM_CAL_PART_NUMBERS_COUNT_BYTE]={0,0,0,0,0,0,0,0,0,0,0,0,
    	                                                                                                     0,0,0,0,0,0,0,0,0,0,0,0};
    if(BlockSize==(CAM_CAL_PART_NUMBERS_COUNT_BYTE))
    {
        cam_calCfg.u4Offset = start_addr;
        cam_calCfg.u4Length = CAM_CAL_PART_NUMBERS_COUNT_BYTE; //sizeof(ucModuleNumber)
        cam_calCfg.pu1Params= (u8 *)&ucModuleNumber[0];
        ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
        if(!ioctlerr)
        {
            err = CAM_CAL_ERR_NO_ERR;
        }
        else
        {
            CAM_CAL_ERR("ioctl err\n");
            ShowCmdErrorLog(pCamCalData->Command);
        }
    }
    else
    {
        CAM_CAL_LOG_IF(dumpEnable,"use default part number\n");        
        srand(time(NULL));
        for(idx=0;idx<(CAM_CAL_PART_NUMBERS_COUNT*LSC_DATA_BYTES);idx++)
        {   
            ucModuleNumber[idx]=CamCalPartNumber[idx];
            if(ucModuleNumber[idx] ==0x20)
            {            
                //disable random> TBD
                //ucModuleNumber[idx] = (UINT32)rand(); //random
                //disable random< TBD
            }            
        }
        err = CAM_CAL_ERR_NO_ERR;
    }
    CAM_CAL_LOG_IF(dumpEnable,"%s\n",ucModuleNumber);       
    memcpy((char*)&pCamCalData->PartNumber[0],ucModuleNumber,sizeof(CAM_CAL_PART_NUMBERS_COUNT_BYTE));
    #ifdef DEBUG_CALIBRATION_LOAD
    CAM_CAL_LOG_IF(dumpEnable,"======================Part Number==================\n");
    CAM_CAL_LOG_IF(dumpEnable,"[Part Number] = %s\n", pCamCalData->PartNumber);
    CAM_CAL_LOG_IF(dumpEnable,"RETURN = 0x%x \n", err);            
    CAM_CAL_LOG_IF(dumpEnable,"======================Part Number==================\n");
    #endif        
    return err;
}

UINT32 DoCamCalAWBGain(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData)
{
    stCAM_CAL_INFO_STRUCT  cam_calCfg;
    PCAM_CAL_DATA_STRUCT pCamCalData = (PCAM_CAL_DATA_STRUCT)pGetSensorCalData;
    MUINT32 idx;    
    UINT32 ioctlerr;
    UINT32 err =  CamCalReturnErr[pCamCalData->Command];     
    UINT32 PregainFactor, PregainOffset;
    UINT32 PregainFactorH, PregainOffsetH;
    UINT32 GainValue;
    if(pCamCalData->DataVer >= CAM_CAL_N3D_DATA)
    {
        err = CAM_CAL_ERR_NO_DEVICE;
        CAM_CAL_ERR("ioctl err\n");
        ShowCmdErrorLog(pCamCalData->Command);
    }
    else if(pCamCalData->DataVer < CAM_CAL_N3D_DATA)
    { 
        if(BlockSize!=CAM_CAL_SINGLE_AWB_COUNT_BYTE)
        {
            CAM_CAL_ERR("BlockSize(%d) is not correct (%d)\n",BlockSize,CAM_CAL_SINGLE_AWB_COUNT_BYTE);
            ShowCmdErrorLog(pCamCalData->Command);            
        }
        else
        {
            ////Only AWB Gain without AF>////
            pCamCalData->Single2A.S2aVer = 0x01;
            pCamCalData->Single2A.S2aBitEn = CAM_CAL_AWB_BITEN;
            pCamCalData->Single2A.S2aAfBitflagEn = 0x0;// //Bit: step 0(inf.), 1(marco), 2, 3, 4,5,6,7
            memset(pCamCalData->Single2A.S2aAf,0x0,sizeof(pCamCalData->Single2A.S2aAf));
            ////Only AWB Gain without AF<////
            ////Only AWB Gain Gathering >////
            cam_calCfg.u4Offset = start_addr|0xFFFF;
            cam_calCfg.u4Length = 4;
            cam_calCfg.pu1Params = (u8 *)&PregainFactor;            
            ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
            if(!ioctlerr)
            {
                err = CAM_CAL_ERR_NO_ERR;
            }
            else        
            {
                pCamCalData->Single2A.S2aBitEn = CAM_CAL_NONE_BITEN;
                CAM_CAL_ERR("ioctl err\n");
                ShowCmdErrorLog(pCamCalData->Command);
            }            
            cam_calCfg.u4Offset = start_addr+4;
            cam_calCfg.u4Length = 4;
            cam_calCfg.pu1Params = (u8 *)&PregainOffset; 
            ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
            if(!ioctlerr)
            {
                err = CAM_CAL_ERR_NO_ERR;
            }
            else        
            {
                pCamCalData->Single2A.S2aBitEn = CAM_CAL_NONE_BITEN;
                CAM_CAL_ERR("ioctl err\n");
                ShowCmdErrorLog(pCamCalData->Command);
            }            

            PregainFactorH = ((PregainFactor>>16)&0xFFFF);
            PregainOffsetH = ((PregainOffset>>16)&0xFFFF);
            if((PregainOffset==0)||(PregainOffsetH==0))
            {
                //pre gain	
                pCamCalData->Single2A.S2aAwb.rUnitGainu4R = 512;
                pCamCalData->Single2A.S2aAwb.rUnitGainu4G = 512;
                pCamCalData->Single2A.S2aAwb.rUnitGainu4B  = 512;
                CAM_CAL_LOG_IF(dumpEnable,"Pegain has no Calinration Data!!!\n");                
            }
            else
            {
                //pre gain	
                pCamCalData->Single2A.S2aAwb.rUnitGainu4R = 
                                                                           (((PregainFactor&0xFF)<<8)|
                                                                       ((PregainFactor&0xFF00)>>8))*512 /
                                                                           (((PregainOffset&0xFF)<<8)|
                                                                       ((PregainOffset&0xFF00)>>8));
                pCamCalData->Single2A.S2aAwb.rUnitGainu4G = 512;
                pCamCalData->Single2A.S2aAwb.rUnitGainu4B  =
                                                                           (((PregainFactorH&0xFF)<<8)|
                	                                                 ((PregainFactorH&0xFF00)>>8))*512/
                	                                                     (((PregainOffsetH&0xFF)<<8)|
                	                                                 ((PregainOffsetH&0xFF00)>>8));
            	err=0;
            }

            if((pCamCalData->Single2A.S2aAwb.rUnitGainu4R==0)||(pCamCalData->Single2A.S2aAwb.rUnitGainu4B==0))
            {
                //pre gain	
                pCamCalData->Single2A.S2aAwb.rUnitGainu4R = 512;
                pCamCalData->Single2A.S2aAwb.rUnitGainu4G = 512;
                pCamCalData->Single2A.S2aAwb.rUnitGainu4B  = 512;
                CAM_CAL_ERR("RGB Gain is not reasonable!!!\n");       
                pCamCalData->Single2A.S2aBitEn = CAM_CAL_NONE_BITEN;
                ShowCmdErrorLog(pCamCalData->Command);
            }    
            ////Only AWB Gain Gathering <////
            #ifdef DEBUG_CALIBRATION_LOAD
            CAM_CAL_LOG_IF(dumpEnable,"======================AWB CAM_CAL==================\n");
            CAM_CAL_LOG_IF(dumpEnable,"[CAM_CAL PREGAIN VALUE] = 0x%x\n", PregainFactor);
            CAM_CAL_LOG_IF(dumpEnable,"[CAM_CAL PREGAIN OFFSET] = 0x%x\n", PregainOffset);
            CAM_CAL_LOG_IF(dumpEnable,"[rCalGain.u4R] = %d\n", pCamCalData->Single2A.S2aAwb.rUnitGainu4R);
            CAM_CAL_LOG_IF(dumpEnable,"[rCalGain.u4G] = %d\n",  pCamCalData->Single2A.S2aAwb.rUnitGainu4G);
            CAM_CAL_LOG_IF(dumpEnable,"[rCalGain.u4B] = %d\n", pCamCalData->Single2A.S2aAwb.rUnitGainu4B);	
            CAM_CAL_LOG_IF(dumpEnable,"======================AWB CAM_CAL==================\n");
            #endif
////////////////////////////////////////////////////////////////////////////////        
        }        
    }    
    return err;
}

/********************************************************/
//Please put your AWB+AF data funtion, here.
/********************************************************/
UINT32 DoCamCal2AGain(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData)
{
	stCAM_CAL_INFO_STRUCT  cam_calCfg;
    PCAM_CAL_DATA_STRUCT pCamCalData = (PCAM_CAL_DATA_STRUCT)pGetSensorCalData;
    MUINT32 idx;    
    UINT32 ioctlerr;
    UINT32 err =  CamCalReturnErr[pCamCalData->Command];     
    UINT32 GainValue;
	
	UINT32 CalGain, FacGain;
	UINT8 AWBAFConfig;
	UINT32 AFInf, AFMacro;
	UINT8 tempMin = 255;
	UINT8 tempGain, CalR, CalGr, CalGb, CalB, FacR, FacGr, FacGb, FacB;
	
	UINT16 Current_Ratio_RG, Current_Ratio_BG,Current_Ratio_GbGr;	
	UINT16 Golden_Ratio_RG, Golden_Ratio_BG;

	UINT16 Current_Ratio_GR, Current_Ratio_GB,Current_Ratio_GrR, Current_Ratio_GrB,Current_Ratio_GbR, Current_Ratio_GbB;	
	UINT16 Golden_Ratio_GR, Golden_Ratio_GB;

	
	UINT16 OTP_Flag,Gold_R,Gold_B,Gold_GR,Gold_GB,Cur_R,Cur_B,Cur_GR,Cur_GB;
	double Cal_R_G_Value,Cal_B_G_Value,Gold_R_G_Value,Gold_B_G_Value;//float
u8 data[2];
		CAM_CAL_LOG_IF(dumpEnable,"DoCamCal2AGain start \r\n");
	//Structure
	/*
	Byte[0]:Version
	Byte[1]: [x,x,x,x,enbAFMacro, enbAFInf, enbAF, enbWB]
	Byte[9:2]: {GoldenB, GoldenGb, GoldenGr, GoldenR, UnitB, UnitGb, UnitGr, UnitR}
	Byte[11:10]: AF inf.
	Byte[13:12]: AF Macro
	*/
    if(pCamCalData->DataVer >= CAM_CAL_N3D_DATA)
    {
        err = CAM_CAL_ERR_NO_DEVICE;
        CAM_CAL_ERR("ioctl err\n");
        ShowCmdErrorLog(pCamCalData->Command);
    }
    else if(pCamCalData->DataVer < CAM_CAL_N3D_DATA)
    { 
        if(BlockSize!=14)
        {
            CAM_CAL_ERR("BlockSize(%d) is not correct (%d)\n",BlockSize,14);
            ShowCmdErrorLog(pCamCalData->Command);            
        }
        else
          #if 0
        {
			// Check the config. for AWB & AF
			cam_calCfg.u4Offset = (start_addr+1);
            cam_calCfg.u4Length = 1;
            cam_calCfg.pu1Params = (u8 *)&AWBAFConfig;            
            ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
            if(!ioctlerr)
            {
                err = CAM_CAL_ERR_NO_ERR;
            }
            else        
            {
                pCamCalData->Single2A.S2aBitEn = CAM_CAL_NONE_BITEN;
                CAM_CAL_ERR("ioctl err\n");
                ShowCmdErrorLog(pCamCalData->Command);
            } 

			pCamCalData->Single2A.S2aVer = 0x01;
            pCamCalData->Single2A.S2aBitEn = (0x03 & AWBAFConfig);
			CAM_CAL_LOG_IF(dumpEnable,"S2aBitEn=0x%x", pCamCalData->Single2A.S2aBitEn);
            pCamCalData->Single2A.S2aAfBitflagEn = (0x0C & AWBAFConfig);// //Bit: step 0(inf.), 1(marco), 2, 3, 4,5,6,7
            //memset(pCamCalData->Single2A.S2aAf,0x0,sizeof(pCamCalData->Single2A.S2aAf));

            if(0x1&AWBAFConfig){
                ////AWB////
                cam_calCfg.u4Offset = (start_addr+2);
                cam_calCfg.u4Length = 4;
                cam_calCfg.pu1Params = (u8 *)&CalGain;            
                ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
				CAM_CAL_LOG_IF(dumpEnable,"Read CalGain OK\n");
                if(!ioctlerr)
                {                
    				// Get min gain
    				CalR  = CalGain&0xFF;
					CalGr = (CalGain>>8)&0xFF;
					CalGb = (CalGain>>16)&0xFF;
					CalB  = (CalGain>>24)&0xFF;
					CAM_CAL_LOG_IF(dumpEnable,"Extract CalGain OK\n");
    				for (int i=0;i<4;i++){
						tempGain = (CalGain>>(8*i)) & 0xFF;
						CAM_CAL_LOG_IF(dumpEnable,"item=%d", tempGain);
                        if( tempGain < tempMin){
    					    tempMin = tempGain;
    						CAM_CAL_LOG_IF(dumpEnable,"New tempMin=%d", tempMin);
                        }
                    }
    				err = CAM_CAL_ERR_NO_ERR;
                }
                else        
                {
                    pCamCalData->Single2A.S2aBitEn = CAM_CAL_NONE_BITEN;
                    CAM_CAL_ERR("ioctl err\n");
                    ShowCmdErrorLog(pCamCalData->Command);
                }            
    
    
    			
                cam_calCfg.u4Offset = (start_addr+6);
                cam_calCfg.u4Length = 4;
                cam_calCfg.pu1Params = (u8 *)&FacGain; 
                ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
				CAM_CAL_LOG_IF(dumpEnable,"Read FacGain OK\n");
                if(!ioctlerr)
                {                
    				// Get min gain
    				FacR  = FacGain&0xFF;
					FacGr = (FacGain>>8)&0xFF;
					FacGb = (FacGain>>16)&0xFF;
					FacB  = (FacGain>>24)&0xFF;
					CAM_CAL_LOG_IF(dumpEnable,"Extract CalGain OK\n");
    				for (int i=0;i<4;i++){
    					tempGain = (FacGain>>(8*i)) & 0xFF;
						CAM_CAL_LOG_IF(dumpEnable,"item=%d", tempGain);
                        if( tempGain < tempMin){
    					    tempMin = tempGain;
    						CAM_CAL_LOG_IF(dumpEnable,"New tempMin=%d", tempMin);
                        }
                    }
    				err = CAM_CAL_ERR_NO_ERR;
                }
                else        
                {
                    pCamCalData->Single2A.S2aBitEn = CAM_CAL_NONE_BITEN;
                    CAM_CAL_ERR("ioctl err\n");
                    ShowCmdErrorLog(pCamCalData->Command);
                }            
    	        CAM_CAL_LOG_IF(dumpEnable,"Start assign value\n");
                pCamCalData->Single2A.S2aAwb.rUnitGainu4R = (u32)(CalR*512/tempMin);
                pCamCalData->Single2A.S2aAwb.rUnitGainu4G = (u32)( (CalGr+CalGb)*512/2/tempMin );
                pCamCalData->Single2A.S2aAwb.rUnitGainu4B  = (u32)(CalB*512/tempMin);
    
    	        pCamCalData->Single2A.S2aAwb.rGoldGainu4R = (u32)(FacR*512/tempMin);
                pCamCalData->Single2A.S2aAwb.rGoldGainu4G = (u32)( (FacGr+FacGb)*512/2/tempMin );
                pCamCalData->Single2A.S2aAwb.rGoldGainu4B  = (u32)(FacB*512/tempMin);
    			
                ////Only AWB Gain Gathering <////
                #ifdef DEBUG_CALIBRATION_LOAD
                CAM_CAL_LOG_IF(dumpEnable,"======================AWB CAM_CAL==================\n");
                CAM_CAL_LOG_IF(dumpEnable,"[CalGain] = 0x%x\n", CalGain);
                CAM_CAL_LOG_IF(dumpEnable,"[FacGain] = 0x%x\n", FacGain);
                CAM_CAL_LOG_IF(dumpEnable,"[rCalGain.u4R] = %d\n", pCamCalData->Single2A.S2aAwb.rUnitGainu4R);
                CAM_CAL_LOG_IF(dumpEnable,"[rCalGain.u4G] = %d\n", pCamCalData->Single2A.S2aAwb.rUnitGainu4G);
                CAM_CAL_LOG_IF(dumpEnable,"[rCalGain.u4B] = %d\n", pCamCalData->Single2A.S2aAwb.rUnitGainu4B);	
    			CAM_CAL_LOG_IF(dumpEnable,"[rFacGain.u4R] = %d\n", pCamCalData->Single2A.S2aAwb.rGoldGainu4R);
                CAM_CAL_LOG_IF(dumpEnable,"[rFacGain.u4G] = %d\n", pCamCalData->Single2A.S2aAwb.rGoldGainu4G);
                CAM_CAL_LOG_IF(dumpEnable,"[rFacGain.u4B] = %d\n", pCamCalData->Single2A.S2aAwb.rGoldGainu4B);	
                CAM_CAL_LOG_IF(dumpEnable,"======================AWB CAM_CAL==================\n");
                #endif
            }
			if(0x2&AWBAFConfig){
                ////AF////
                cam_calCfg.u4Offset = (start_addr+10);
                cam_calCfg.u4Length = 2;
                cam_calCfg.pu1Params = (u8 *)&AFInf;            
                ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
                if(!ioctlerr)
                {                
    				err = CAM_CAL_ERR_NO_ERR;
                }
                else        
                {
                    pCamCalData->Single2A.S2aBitEn = CAM_CAL_NONE_BITEN;
                    CAM_CAL_ERR("ioctl err\n");
                    ShowCmdErrorLog(pCamCalData->Command);
                }            
    
                cam_calCfg.u4Offset = (start_addr+12);
                cam_calCfg.u4Length = 2;
                cam_calCfg.pu1Params = (u8 *)&AFMacro;            
                ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
                if(!ioctlerr)
                {                
    				err = CAM_CAL_ERR_NO_ERR;
                }
                else        
                {
                    pCamCalData->Single2A.S2aBitEn = CAM_CAL_NONE_BITEN;
                    CAM_CAL_ERR("ioctl err\n");
                    ShowCmdErrorLog(pCamCalData->Command);
                }
    	
                pCamCalData->Single2A.S2aAf[0] = AFInf;
                pCamCalData->Single2A.S2aAf[1] = AFMacro;
     			
                ////Only AWB Gain Gathering <////
                #ifdef DEBUG_CALIBRATION_LOAD
                CAM_CAL_LOG_IF(dumpEnable,"======================AF CAM_CAL==================\n");
                CAM_CAL_LOG_IF(dumpEnable,"[AFInf] = %d\n", AFInf);
                CAM_CAL_LOG_IF(dumpEnable,"[AFMacro] = %d\n", AFMacro);
                CAM_CAL_LOG_IF(dumpEnable,"======================AF CAM_CAL==================\n");
                #endif
            }
			
////////////////////////////////////////////////////////////////////////////////        
        }  
#else 
{
                ////AWB////

				start_addr=0x00003810;
				//cam_calCfg.pu1Params = (u8 *)&data;
				
				cam_calCfg.u4Offset = 0xE2;
				cam_calCfg.u4Length = 1;
				cam_calCfg.pu1Params = (u8 *)&OTP_Flag;            
				ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
				CAM_CAL_LOG_IF(dumpEnable,"[ioctlerr] = 0x%x\n", ioctlerr);
				//OTP_Flag=(UINT16 )(cam_calCfg.pu1Params);
				CAM_CAL_LOG_IF(dumpEnable,"[OTP_Flag] = 0x%x\n", OTP_Flag);
				
				       if((OTP_Flag & 0x0F) == 0x0F) 
				        {
						CAM_CAL_LOG_IF(dumpEnable,"Read OTP_Flag OK\n");
						cam_calCfg.u4Offset = 0;
						cam_calCfg.u4Length = 1;
						cam_calCfg.pu1Params = (u8 *)&Current_Ratio_RG;            
						ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);
						CAM_CAL_LOG_IF(dumpEnable,"[Current_R/Gr_Address] = 0x%x\n", start_addr+0);
						CAM_CAL_LOG_IF(dumpEnable,"Current_R/Gr = 0x%x\n", Current_Ratio_RG);

						cam_calCfg.u4Offset = 2;
						cam_calCfg.u4Length = 1;
						cam_calCfg.pu1Params = (u8 *)&Current_Ratio_BG;            
						ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);
						CAM_CAL_LOG_IF(dumpEnable,"[Current_B/Gr_Address] = 0x%x\n", start_addr+2);		
						CAM_CAL_LOG_IF(dumpEnable,"Current_B/Gr = 0x%x\n", Current_Ratio_BG);	

						cam_calCfg.u4Offset = 4;
						cam_calCfg.u4Length = 1;
						cam_calCfg.pu1Params = (u8 *)&Current_Ratio_GbGr;			  
						ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);
						CAM_CAL_LOG_IF(dumpEnable,"[Current_Gb/Gr_Address] = 0x%x\n", start_addr+4);
						CAM_CAL_LOG_IF(dumpEnable,"Current_Gb/Gr = 0x%x\n", Current_Ratio_GbGr);

						cam_calCfg.u4Offset = 6;
						cam_calCfg.u4Length = 1;
						cam_calCfg.pu1Params = (u8 *)&Golden_Ratio_RG;			  
						ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);
						CAM_CAL_LOG_IF(dumpEnable,"[Gold_R/G_Address] = 0x%x\n", start_addr+6);
						CAM_CAL_LOG_IF(dumpEnable,"Gold_R/G = 0x%x\n", Golden_Ratio_RG);

						cam_calCfg.u4Offset = 8;
						cam_calCfg.u4Length = 1;
						cam_calCfg.pu1Params = (u8 *)&Golden_Ratio_BG;			  
						ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);
						CAM_CAL_LOG_IF(dumpEnable,"[Gold_B/G_Address] = 0x%x\n", start_addr+8);
						CAM_CAL_LOG_IF(dumpEnable,"Gold_B/G = 0x%x\n", Golden_Ratio_BG);

				#if 0
						cam_calCfg.u4Offset = 10;
						cam_calCfg.u4Length = 1;
						cam_calCfg.pu1Params = (u8 *)&Cur_B;			  
						ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);
						CAM_CAL_LOG_IF(dumpEnable,"[Cur_B_Address] = 0x%x\n", start_addr+10);
						CAM_CAL_LOG_IF(dumpEnable,"Cur_B = 0x%x\n", Cur_B);

						cam_calCfg.u4Offset = 12;
						cam_calCfg.u4Length = 1;
						cam_calCfg.pu1Params = (u8 *)&Cur_GR;			  
						ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);
						CAM_CAL_LOG_IF(dumpEnable,"[Cur_GR_Address] = 0x%x\n", start_addr+12);
						CAM_CAL_LOG_IF(dumpEnable,"Cur_GR = 0x%x\n", Cur_GR);

						cam_calCfg.u4Offset = 14;
						cam_calCfg.u4Length = 1;
						cam_calCfg.pu1Params = (u8 *)&Cur_GB;			  
						ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);
						CAM_CAL_LOG_IF(dumpEnable,"[Cur_GB_Address] = 0x%x\n", start_addr+14);
						CAM_CAL_LOG_IF(dumpEnable,"Cur_GB = 0x%x\n", Cur_GB);
				#endif

						
				          }
				      else
					  {
						        CAM_CAL_ERR("ioctl err\n");
						     } 
                //Gold_R=Gold_R-42;
                //Gold_B=Gold_B-42;
                //Gold_GR=Gold_GR-42;
                //Gold_GB=Gold_GB-42;
                //Cur_R=Cur_R-42;
                //Cur_B=Cur_B-42;
                //Cur_GR=Cur_GR-42;
                //Cur_GB=Cur_GB-42;

    	        CAM_CAL_LOG_IF(dumpEnable,"Start assign value\n");

				if(Current_Ratio_RG==0 || Current_Ratio_BG==0 || Golden_Ratio_RG==0 ||Golden_Ratio_BG==0)
				{
					Current_Ratio_RG	= 1024;
					Current_Ratio_BG	= 1024;	
					Golden_Ratio_RG		= 1024;
					Golden_Ratio_BG		= 1024;
					Current_Ratio_GbGr	= 1024;
				}

				Current_Ratio_GrR	= 512*1024/ Current_Ratio_RG;
				Current_Ratio_GrB	= 512*1024/ Current_Ratio_BG;
				Current_Ratio_GbR	= Current_Ratio_GrR*Current_Ratio_GbGr/1024;
				Current_Ratio_GbB	= Current_Ratio_GrB*Current_Ratio_GbGr/1024;

				Current_Ratio_GR	= (Current_Ratio_GrR+Current_Ratio_GbR)/2;
				Current_Ratio_GB	= (Current_Ratio_GrB+Current_Ratio_GbB)/2;
				
				Golden_Ratio_GR		= 512*1024/Golden_Ratio_RG;
				Golden_Ratio_GB		= 512*1024/Golden_Ratio_BG;	


    	          //Cal_R_G_Value = ((double)(Cur_GR+Cur_GB)/2)/Cur_R;
    	          //Cal_B_G_Value = ((double)(Cur_GR+Cur_GB)/2)/Cur_B;
    	          //Gold_R_G_Value = ((double)(Gold_GR+Gold_GB)/2)/Gold_R;
    	          //Gold_B_G_Value = ((double)(Gold_GR+Gold_GB)/2)/Gold_B;

          /*Cal_R_G_Value = (double)Cur_GR/Cur_R;
                  Cal_B_G_Value =(double)Cur_GR/Cur_B;
                  Gold_R_G_Value = (double)Gold_GR/Gold_R;
                  Gold_B_G_Value =(double)Gold_GR/Gold_B;*/

		  //CAM_CAL_LOG_IF(dumpEnable,"[Cal_R_G_Value] = %f\n", Cal_R_G_Value);
		  //CAM_CAL_LOG_IF(dumpEnable,"[Cal_B_G_Value] = %f\n", Cal_B_G_Value);
		  //CAM_CAL_LOG_IF(dumpEnable,"[Gold_R_G] = %f\n", Gold_R_G_Value);
		  //CAM_CAL_LOG_IF(dumpEnable,"[Gold_B_G] = %f\n", Gold_B_G_Value);

                pCamCalData->Single2A.S2aAwb.rUnitGainu4R = (u32)Current_Ratio_GR;//(Cal_R_G_Value*512);
                pCamCalData->Single2A.S2aAwb.rUnitGainu4G = (u32)( 512 );
                pCamCalData->Single2A.S2aAwb.rUnitGainu4B = (u32)Current_Ratio_GB;//(Cal_B_G_Value*512);
    
    	        pCamCalData->Single2A.S2aAwb.rGoldGainu4R = (u32)Golden_Ratio_GR;//(Gold_R_G_Value*512);
                pCamCalData->Single2A.S2aAwb.rGoldGainu4G = (u32)( 512 );
                pCamCalData->Single2A.S2aAwb.rGoldGainu4B = (u32)Golden_Ratio_GB;//(Gold_B_G_Value*512);
							 			
                ////Only AWB Gain Gathering <////
                #ifdef DEBUG_CALIBRATION_LOAD
                CAM_CAL_LOG_IF(dumpEnable,"======================AWB READ_OTP==================\n");
                CAM_CAL_LOG_IF(dumpEnable,"Current_Ratio_RG = 0x%x\n", Current_Ratio_RG);				
				CAM_CAL_LOG_IF(dumpEnable,"Current_Ratio_BG = 0x%x\n", Current_Ratio_BG);			
				CAM_CAL_LOG_IF(dumpEnable,"Current_Ratio_GbGr = 0x%x\n", Current_Ratio_GbGr);				
				CAM_CAL_LOG_IF(dumpEnable,"Golden_Ratio_RG = 0x%x\n", Golden_Ratio_RG);
				CAM_CAL_LOG_IF(dumpEnable,"Golden_Ratio_BG = 0x%x\n", Golden_Ratio_BG);
				
                CAM_CAL_LOG_IF(dumpEnable,"======================AWB READ_VAL_CHARGE==================\n");
                CAM_CAL_LOG_IF(dumpEnable,"Current_Ratio_GrR = 0x%x\n", Current_Ratio_GrR);
                CAM_CAL_LOG_IF(dumpEnable,"Current_Ratio_GrB = 0x%x\n", Current_Ratio_GrB);
                CAM_CAL_LOG_IF(dumpEnable,"Current_Ratio_GbR = 0x%x\n", Current_Ratio_GbR);
                CAM_CAL_LOG_IF(dumpEnable,"Current_Ratio_GbB = 0x%x\n", Current_Ratio_GbB);
                CAM_CAL_LOG_IF(dumpEnable,"Current_Ratio_GR = 0x%x\n", Current_Ratio_GR);
                CAM_CAL_LOG_IF(dumpEnable,"Current_Ratio_GB = 0x%x\n", Current_Ratio_GB);
                CAM_CAL_LOG_IF(dumpEnable,"Golden_Ratio_GR = 0x%x\n", Golden_Ratio_GR);
                CAM_CAL_LOG_IF(dumpEnable,"Golden_Ratio_GB = 0x%x\n", Golden_Ratio_GB);

                CAM_CAL_LOG_IF(dumpEnable,"======================AWB CAM_CAL==================\n");
                //CAM_CAL_LOG_IF(dumpEnable,"[CalGain] = 0x%x\n", CalGain);
                //CAM_CAL_LOG_IF(dumpEnable,"[FacGain] = 0x%x\n", FacGain);
                CAM_CAL_LOG_IF(dumpEnable,"[rCalGain.u4R] = %d\n", pCamCalData->Single2A.S2aAwb.rUnitGainu4R);
                CAM_CAL_LOG_IF(dumpEnable,"[rCalGain.u4G] = %d\n", pCamCalData->Single2A.S2aAwb.rUnitGainu4G);
                CAM_CAL_LOG_IF(dumpEnable,"[rCalGain.u4B] = %d\n", pCamCalData->Single2A.S2aAwb.rUnitGainu4B);	
    			CAM_CAL_LOG_IF(dumpEnable,"[rFacGain.u4R] = %d\n", pCamCalData->Single2A.S2aAwb.rGoldGainu4R);
                CAM_CAL_LOG_IF(dumpEnable,"[rFacGain.u4G] = %d\n", pCamCalData->Single2A.S2aAwb.rGoldGainu4G);
                CAM_CAL_LOG_IF(dumpEnable,"[rFacGain.u4B] = %d\n", pCamCalData->Single2A.S2aAwb.rGoldGainu4B);	
                CAM_CAL_LOG_IF(dumpEnable,"======================AWB CAM_CAL==================\n");
                #endif

				err = CAM_CAL_ERR_NO_ERR;
}  
#endif   
    }    
    return err;
}
UINT32 DoCamCalSingleLsc(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData)
{
    stCAM_CAL_INFO_STRUCT  cam_calCfg;
    PCAM_CAL_DATA_STRUCT pCamCalData = (PCAM_CAL_DATA_STRUCT)pGetSensorCalData;
    MUINT32 idx;    
    UINT32 ioctlerr;
    UINT32 err =  CamCalReturnErr[pCamCalData->Command];     
    UINT32 PregainFactor, PregainOffset;
    UINT32 PregainFactorH, PregainOffsetH;
    UINT32 GainValue;
    if(pCamCalData->DataVer >= CAM_CAL_N3D_DATA)
    {
        err = CAM_CAL_ERR_NO_DEVICE;
        CAM_CAL_ERR("ioctl err\n");
        ShowCmdErrorLog(pCamCalData->Command);
    }
    else
    {
        if(BlockSize!=CAM_CAL_SINGLE_LSC_SIZE)
        {
            CAM_CAL_ERR("BlockSize(%d) is not correct (%d)\n",BlockSize,CAM_CAL_SINGLE_LSC_SIZE);
            ShowCmdErrorLog(pCamCalData->Command);            
        }
        else
        {
            pCamCalData->SingleLsc.TableRotation=CUSTOM_CAM_CAL_ROTATION_00;         
            cam_calCfg.u4Offset = (start_addr|0xFFFF);
            cam_calCfg.u4Length = BlockSize; //sizeof(ucModuleNumber)
            cam_calCfg.pu1Params= (u8 *)&pCamCalData->SingleLsc.LscTable.Data[0];
            ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
            if(!ioctlerr)
            {
                err = CAM_CAL_ERR_NO_ERR;
            }
            else
            {
                CAM_CAL_ERR("ioctl err\n");
                err =  CamCalReturnErr[pCamCalData->Command];  
                ShowCmdErrorLog(pCamCalData->Command);
            }
        }
    }    
    #ifdef DEBUG_CALIBRATION_LOAD
    CAM_CAL_LOG_IF(dumpEnable,"======================SingleLsc Data==================\n");
    CAM_CAL_LOG_IF(dumpEnable,"[1st] = %x, %x, %x, %x \n", pCamCalData->SingleLsc.LscTable.Data[0],
    	                                                                         pCamCalData->SingleLsc.LscTable.Data[1],
    	                                                                         pCamCalData->SingleLsc.LscTable.Data[2],
    	                                                                         pCamCalData->SingleLsc.LscTable.Data[3]);
    CAM_CAL_LOG_IF(dumpEnable,"[1st] = SensorLSC(1)?MTKLSC(2)?  %x \n", pCamCalData->SingleLsc.LscTable.MtkLcsData.MtkLscType);
    CAM_CAL_LOG_IF(dumpEnable,"RETURN = 0x%x \n", err);            
    CAM_CAL_LOG_IF(dumpEnable,"======================SingleLsc Data==================\n");
    #endif    
    return err;	
}
UINT32 DoCamCalN3dLsc(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData)
{
    stCAM_CAL_INFO_STRUCT  cam_calCfg;
    PCAM_CAL_DATA_STRUCT pCamCalData = (PCAM_CAL_DATA_STRUCT)pGetSensorCalData;
    MUINT32 idx;    
    UINT32 ioctlerr;
    UINT32 err =  CamCalReturnErr[pCamCalData->Command];     
    UINT32 PregainFactor, PregainOffset;
    UINT32 PregainFactorH, PregainOffsetH;
    UINT32 GainValue;
    if(pCamCalData->DataVer != CAM_CAL_N3D_DATA)
    {
        err = CAM_CAL_ERR_NO_DEVICE;
        CAM_CAL_ERR("ioctl err\n");
        ShowCmdErrorLog(pCamCalData->Command);
    }
    else
    {
        if(BlockSize!=CAM_CAL_N3D_LSC_SIZE)
        {
            CAM_CAL_ERR("BlockSize(%d) is not correct (%d)\n",BlockSize,CAM_CAL_N3D_LSC_SIZE);
            ShowCmdErrorLog(pCamCalData->Command);            
        }
        else
        {
            pCamCalData->N3DLsc.Data[0].TableRotation=CUSTOM_CAM_CAL_ROTATION_00;
            pCamCalData->N3DLsc.Data[1].TableRotation=CUSTOM_CAM_CAL_ROTATION_01;            
            cam_calCfg.u4Offset = (start_addr|0xFFFF);
            cam_calCfg.u4Length = BlockSize; //sizeof(ucModuleNumber)
            cam_calCfg.pu1Params= (u8 *)&pCamCalData->N3DLsc.Data[0].LscTable.Data[0];
            ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
            if(!ioctlerr)
            {
                err = CAM_CAL_ERR_NO_ERR;
            }
            else
            {
                CAM_CAL_ERR("ioctl err\n");
                err =  CamCalReturnErr[pCamCalData->Command];  
                ShowCmdErrorLog(pCamCalData->Command);
            }
            cam_calCfg.u4Offset = ((start_addr>>16)|0xFFFF);
            cam_calCfg.u4Length = BlockSize; //sizeof(ucModuleNumber)
            cam_calCfg.pu1Params= (u8 *)&pCamCalData->N3DLsc.Data[1].LscTable.Data[0];
            ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
            if(!ioctlerr)
            {
                err = CAM_CAL_ERR_NO_ERR;
            }
            else
            {
                CAM_CAL_ERR("ioctl err\n");
                err =  CamCalReturnErr[pCamCalData->Command];  
                ShowCmdErrorLog(pCamCalData->Command);
            }
        }
    }    
    #ifdef DEBUG_CALIBRATION_LOAD
    CAM_CAL_LOG_IF(dumpEnable,"======================3DLsc Data==================\n");
    CAM_CAL_LOG_IF(dumpEnable,"[1st] = %x, %x, %x, %x \n", pCamCalData->N3DLsc.Data[0].LscTable.Data[0],
    	                                                                         pCamCalData->N3DLsc.Data[0].LscTable.Data[1],
    	                                                                         pCamCalData->N3DLsc.Data[0].LscTable.Data[2],
    	                                                                         pCamCalData->N3DLsc.Data[0].LscTable.Data[3]);
    CAM_CAL_LOG_IF(dumpEnable,"[1st] = SensorLSC(1)?MTKLSC(2)?  %x \n", 
    	                                                  pCamCalData->N3DLsc.Data[0].LscTable.MtkLcsData.MtkLscType);    
    CAM_CAL_LOG_IF(dumpEnable,"[2nd] = %x, %x, %x, %x \n", pCamCalData->N3DLsc.Data[1].LscTable.Data[0],
    	                                                                         pCamCalData->N3DLsc.Data[1].LscTable.Data[1],
    	                                                                         pCamCalData->N3DLsc.Data[1].LscTable.Data[2],
    	                                                                         pCamCalData->N3DLsc.Data[1].LscTable.Data[3]);
    CAM_CAL_LOG_IF(dumpEnable,"[[2nd]] = SensorLSC(1)?MTKLSC(2)?  %x \n", 
    	                                                  pCamCalData->N3DLsc.Data[1].LscTable.MtkLcsData.MtkLscType);    
    CAM_CAL_LOG_IF(dumpEnable,"RETURN = 0x%x \n", err);            
    CAM_CAL_LOG_IF(dumpEnable,"======================3DLsc Data==================\n");
    #endif    
    return err;	
}

UINT32 DoCamCal3AGain(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData)
{
    stCAM_CAL_INFO_STRUCT  cam_calCfg;
    PCAM_CAL_DATA_STRUCT pCamCalData = (PCAM_CAL_DATA_STRUCT)pGetSensorCalData;
    MUINT32 idx;    
    UINT32 ioctlerr;
    UINT32 err =  CamCalReturnErr[pCamCalData->Command];     
    UINT32 PregainFactor, PregainOffset;
    UINT32 PregainFactorH, PregainOffsetH;
    UINT32 GainValue;
    if(pCamCalData->DataVer != CAM_CAL_N3D_DATA)
    {
        err = CAM_CAL_ERR_NO_DEVICE;
        CAM_CAL_ERR("ioctl err\n");
        ShowCmdErrorLog(pCamCalData->Command);
    }
    else
    {
        if(BlockSize!=CAM_CAL_N3D_3A_SIZE)
        {
            CAM_CAL_ERR("BlockSize(%d) is not correct (%d)\n",BlockSize,CAM_CAL_N3D_3A_SIZE);
            ShowCmdErrorLog(pCamCalData->Command);            
        }
        else
        {
            cam_calCfg.u4Offset = (start_addr|0xFFFF);
            cam_calCfg.u4Length = BlockSize; //sizeof(ucModuleNumber)
            cam_calCfg.pu1Params= (u8 *)&pCamCalData->N3D3A.Data[0][0];
            ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
            if(!ioctlerr)
            {
                err = CAM_CAL_ERR_NO_ERR;
            }
            else
            {
                CAM_CAL_ERR("ioctl err\n");
                err =  CamCalReturnErr[pCamCalData->Command];  
                ShowCmdErrorLog(pCamCalData->Command);
            }
            cam_calCfg.u4Offset = ((start_addr>>16)|0xFFFF);
            cam_calCfg.u4Length = BlockSize; //sizeof(ucModuleNumber)
            cam_calCfg.pu1Params= (u8 *)&pCamCalData->N3D3A.Data[1][0];
            ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
            if(!ioctlerr)
            {
                err = CAM_CAL_ERR_NO_ERR;
            }
            else
            {
                CAM_CAL_ERR("ioctl err\n");
                err =  CamCalReturnErr[pCamCalData->Command];  
                ShowCmdErrorLog(pCamCalData->Command);
            }
        }
    }    
    #ifdef DEBUG_CALIBRATION_LOAD
    CAM_CAL_LOG_IF(dumpEnable,"======================3A Data==================\n");
    CAM_CAL_LOG_IF(dumpEnable,"[1st] = %x, %x, %x, %x \n", pCamCalData->N3D3A.Data[0][0],pCamCalData->N3D3A.Data[0][1],
    	                                                                         pCamCalData->N3D3A.Data[0][2],pCamCalData->N3D3A.Data[0][3]);
    CAM_CAL_LOG_IF(dumpEnable,"[2nd] = %x, %x, %x, %x \n", pCamCalData->N3D3A.Data[1][0],pCamCalData->N3D3A.Data[1][1],
    	                                                                         pCamCalData->N3D3A.Data[1][2],pCamCalData->N3D3A.Data[1][3]);
    CAM_CAL_LOG_IF(dumpEnable,"RETURN = 0x%x \n", err);            
    CAM_CAL_LOG_IF(dumpEnable,"======================3A Data==================\n");
    #endif    
    return err;
}

UINT32 DoCamCal3DGeo(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData)
{
    stCAM_CAL_INFO_STRUCT  cam_calCfg;
    PCAM_CAL_DATA_STRUCT pCamCalData = (PCAM_CAL_DATA_STRUCT)pGetSensorCalData;
    MUINT32 idx;    
    UINT32 ioctlerr;
    UINT32 err =  CamCalReturnErr[pCamCalData->Command];     
    UINT32 PregainFactor, PregainOffset;
    UINT32 PregainFactorH, PregainOffsetH;
    UINT32 GainValue;
    if(pCamCalData->DataVer != CAM_CAL_N3D_DATA)
    {
        err = CAM_CAL_ERR_NO_DEVICE;
        CAM_CAL_ERR("ioctl err\n");
        ShowCmdErrorLog(pCamCalData->Command);
    }
    else
    {
        if(BlockSize!=CAM_CAL_N3D_3D_SIZE)
        {
            CAM_CAL_ERR("BlockSize(%d) is not correct (%d)\n",BlockSize,CAM_CAL_N3D_3D_SIZE);
            ShowCmdErrorLog(pCamCalData->Command);            
        }
        else
        {
            cam_calCfg.u4Offset = (start_addr|0xFFFF);
            cam_calCfg.u4Length = BlockSize; //sizeof(ucModuleNumber)
            cam_calCfg.pu1Params= (u8 *)&pCamCalData->N3D3D.Data[0][0];
            ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
            if(!ioctlerr)
            {
                err = CAM_CAL_ERR_NO_ERR;
            }
            else
            {
                CAM_CAL_ERR("ioctl err\n");
                err =  CamCalReturnErr[pCamCalData->Command];  
                ShowCmdErrorLog(pCamCalData->Command);
            }
            cam_calCfg.u4Offset = ((start_addr>>16)|0xFFFF);
            cam_calCfg.u4Length = BlockSize; //sizeof(ucModuleNumber)
            cam_calCfg.pu1Params= (u8 *)&pCamCalData->N3D3D.Data[1][0];
            ioctlerr= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);	
            if(!ioctlerr)
            {
                err = CAM_CAL_ERR_NO_ERR;
            }
            else
            {
                CAM_CAL_ERR("ioctl err\n");
                err =  CamCalReturnErr[pCamCalData->Command];  
                ShowCmdErrorLog(pCamCalData->Command);
            }
        }
    }    
    #ifdef DEBUG_CALIBRATION_LOAD
    CAM_CAL_LOG_IF(dumpEnable,"======================3D Data==================\n");
    CAM_CAL_LOG_IF(dumpEnable,"[1st] = %x, %x, %x, %x \n", pCamCalData->N3D3D.Data[0][0],pCamCalData->N3D3D.Data[0][1],
    	                                                                         pCamCalData->N3D3D.Data[0][2],pCamCalData->N3D3D.Data[0][3]);
    CAM_CAL_LOG_IF(dumpEnable,"[2nd] = %x, %x, %x, %x \n", pCamCalData->N3D3D.Data[1][0],pCamCalData->N3D3D.Data[1][1],
    	                                                                         pCamCalData->N3D3D.Data[1][2],pCamCalData->N3D3D.Data[1][3]);
    CAM_CAL_LOG_IF(dumpEnable,"RETURN = 0x%x \n", err);            
    CAM_CAL_LOG_IF(dumpEnable,"======================3D Data==================\n");
    #endif    
    return err;
}

UINT32 DoCamCalLayoutCheck(void)
{
    MINT32 lCamcamFID = -1;  //seanlin 121017 01 local for layout check

    UCHAR cBuf[128] = "/dev/";	
    UCHAR HeadID[5] = "NONE";	
    UINT32 result = CAM_CAL_ERR_NO_DEVICE;
    //cam_cal
    stCAM_CAL_INFO_STRUCT  cam_calCfg;
    UINT32 CheckID,i ;
    INT32 err;
	u8 data[2];
//		u8 * pOneByteBuff = (u8 *)&data;
//	pOneByteBuff = ;
	
    switch(gIsInitedCamCal)
    {
        case CAM_CAL_LAYOUT_RTN_PASS:
        result =  CAM_CAL_ERR_NO_ERR;
        break;	        
        case CAM_CAL_LAYOUT_RTN_QUEUE:        	
        case CAM_CAL_LAYOUT_RTN_FAILED:
        default:
        result =  CAM_CAL_ERR_NO_DEVICE;        	
        break;	
    }
    if ((gIsInitedCamCal==CAM_CAL_LAYOUT_RTN_QUEUE) && (CAM_CALInit() != CAM_CAL_NONE) && (CAM_CALDeviceName(&cBuf[0]) == 0))
    {
        lCamcamFID = open(cBuf, O_RDWR);
		CAM_CAL_LOG_IF(dumpEnable,"lCamcamFID= 0x%x", lCamcamFID);
        if(lCamcamFID == -1)
        {            
            CAM_CAL_ERR("----error: can't open CAM_CAL %s----\n",cBuf);
            gIsInitedCamCal=CAM_CAL_LAYOUT_RTN_FAILED;
            result =  CAM_CAL_ERR_NO_DEVICE;      
            return result;//0;
        }	
        //read ID
        cam_calCfg.u4Offset = 0xFFFFFFFF;
        for (i = 0; i< MAX_CALIBRATION_LAYOUT_NUM; i++)
        {
            if (cam_calCfg.u4Offset != CalLayoutTbl[i].HeaderAddr)
            {
                CheckID = 0x00000000;
                cam_calCfg.u4Offset = CalLayoutTbl[i].HeaderAddr;
                cam_calCfg.u4Length = 1;//4
                CAM_CAL_LOG_IF(dumpEnable,"offset:%0x,length:%d\n",cam_calCfg.u4Offset, cam_calCfg.u4Length);
                cam_calCfg.pu1Params = (u8 *)&CheckID;
                err= ioctl(lCamcamFID, CAM_CALIOC_G_READ , &cam_calCfg);                
			CAM_CAL_LOG_IF(dumpEnable,"CheckID:%0x,Params:%x,%x\n",CheckID, cam_calCfg.pu1Params[0],cam_calCfg.pu1Params[1]);	
                if(err< 0)
                {
                    CAM_CAL_ERR("ioctl err\n");
                    CAM_CAL_ERR("Read header ID fail err = 0x%x \n",err);
                    gIsInitedCamCal=CAM_CAL_LAYOUT_RTN_FAILED;
                    result =  CAM_CAL_ERR_NO_DEVICE; 
                    break;
                }
            }
			CAM_CAL_LOG_IF(dumpEnable,"Table[%d] ID= 0x%x",i, CheckID);
            if(CheckID == CalLayoutTbl[i].HeaderId)
            {
                LayoutType = i;
                gIsInitedCamCal=CAM_CAL_LAYOUT_RTN_PASS;   	
                CAM_CAL_LOG_IF(dumpEnable,"gIsInitedCamCal= 0x%x,LayoutType=%x",gIsInitedCamCal,LayoutType);  	
                result =  CAM_CAL_ERR_NO_ERR;	
                break;
            }			
        }        
        CAM_CAL_LOG_IF(dumpEnable,"LayoutType= 0x%x",LayoutType);	
        CAM_CAL_LOG_IF(dumpEnable,"result= 0x%x",result);	 
        ////
        close(lCamcamFID);		
    }	
    else //test
    {
        CAM_CAL_LOG_IF(dumpEnable,"----gIsInitedCamCal_0x%x!----\n",gIsInitedCamCal);
        CAM_CAL_LOG_IF(dumpEnable,"----NO CAM_CAL_%s!----\n",cBuf);
		CAM_CAL_LOG_IF(dumpEnable,"----NO CCAM_CALInit_%d!----\n",CAM_CALInit());
    }

	CAM_CAL_LOG_IF(dumpEnable,"LayoutType= 0x%x",LayoutType);	
	CAM_CAL_LOG_IF(dumpEnable,"result= 0x%x",result);	 
	CAM_CAL_LOG_IF(dumpEnable,"gIsInitedCamCal= 0x%x",gIsInitedCamCal);
    return  result;
}

UINT32 CAM_CALGetCalData(UINT32* pGetSensorCalData)
{	
    UCHAR cBuf[128] = "/dev/";	
    UCHAR HeadID[5] = "NONE";	
    UINT32 result = CAM_CAL_ERR_NO_DEVICE;
    //cam_cal
    stCAM_CAL_INFO_STRUCT  cam_calCfg;
    UINT32 CheckID,i ;
    INT32 err = CAM_CAL_ERR_NO_DEVICE;
//    static UINT16 LayoutType = (MAX_CALIBRATION_LAYOUT_NUM+1); //seanlin 121017 why static? Because cam_cal_drv will get data one block by one block instead of overall in one time.
    INT32 CamcamFID = 0;  //seanlin 121017 why static? Because cam_cal_drv will get data one block by one block instead of overall in one time.
    UINT16 u2IDMatch = 0;
    UINT32 ulPartNumberCount = 0;
    CAMERA_CAM_CAL_TYPE_ENUM lsCommand;
    
    PCAM_CAL_DATA_STRUCT pCamCalData = (PCAM_CAL_DATA_STRUCT)pGetSensorCalData;    
//    CAM_CAL_LOG_IF(dumpEnable,"CAM_CALGetCalData(0x%8x)----\n",(unsigned int)pGetSensorCalData);
    //====== Get Property ======
    char value[PROPERTY_VALUE_MAX] = {'\0'};    
    property_get("camcalcamcal.log", value, "0");
    dumpEnable = atoi(value);
    //====== Get Property ======



    lsCommand = pCamCalData->Command;
    CAM_CAL_LOG_IF(dumpEnable,"pCamCalData->Command = 0x%x \n",pCamCalData->Command);
    CAM_CAL_LOG_IF(dumpEnable,"lsCommand = 0x%x \n",lsCommand);    
    //Make sure Layout is confirmed, first
    if(DoCamCalLayoutCheck()==CAM_CAL_ERR_NO_ERR)
    {  
        pCamCalData->DataVer = (CAM_CAL_DATA_VER_ENUM)CalLayoutTbl[LayoutType].DataVer;   
        if ((CAM_CALInit() != CAM_CAL_NONE) && (CAM_CALDeviceName(&cBuf[0]) == 0))
        {
        CAM_CAL_LOG_IF(dumpEnable,"longxuewei test---1\n");    
            CamcamFID = open(cBuf, O_RDWR);	
            if(CamcamFID == -1)
            {
                CAM_CAL_LOG_IF(dumpEnable,"----error: can't open CAM_CAL %s----\n",cBuf);
                result =  CamCalReturnErr[lsCommand];       
                ShowCmdErrorLog(lsCommand);
                return result;
            } 
            /*********************************************/
            if ((CalLayoutTbl[LayoutType].CalItemTbl[lsCommand].Include != 0) 
            	&&(CalLayoutTbl[LayoutType].CalItemTbl[lsCommand].GetCalDataProcess != NULL))
            {		
              CAM_CAL_LOG_IF(dumpEnable,"longxuewei test---22\n");
                result =  CalLayoutTbl[LayoutType].CalItemTbl[lsCommand].GetCalDataProcess(
                	                    CamcamFID, 
                	                    CalLayoutTbl[LayoutType].CalItemTbl[lsCommand].StartAddr, 
                	                    CalLayoutTbl[LayoutType].CalItemTbl[lsCommand].BlockSize, 
                	                    pGetSensorCalData);	

				
            }
            else
            {
                result =  CamCalReturnErr[lsCommand];       
                ShowCmdErrorLog(lsCommand);       						
            }
            /*********************************************/   
			close(CamcamFID);
        }
    }
    else
    {

       result =  CamCalReturnErr[lsCommand];       
       ShowCmdErrorLog(lsCommand);
        return result;
    }         
    CAM_CAL_LOG_IF(dumpEnable,"result = 0x%x\n",result);	
    return  result;
}


UINT32 DoCamCalDataReset(INT32 CamcamFID, UINT32 start_addr, UINT32 BlockSize, UINT32* pGetSensorCalData)
{
    gIsInitedCamCal = CAM_CAL_LAYOUT_RTN_QUEUE;//seanlin 121017 why static? Because cam_cal_drv will get data one block by one block instead of overall in one time.
    LayoutType = (MAX_CALIBRATION_LAYOUT_NUM+1); //seanlin 121017 why static? Because cam_cal_drv will get data one block by one block instead of overall in one time.
    bFirstLoad = TRUE;
    return 0;
}

#if 0
#if 1
    unsigned int size[MAX_CALIBRATION_LAYOUT_NUM]={ CALIBRATION_DATA_SIZE_SLIM_LSC1,
    												 CALIBRATION_DATA_SIZE_N3D_DATA1,
    												 CALIBRATION_DATA_SIZE_SUNNY_Q8N03D_LSC1};
#endif 
        if(bFirstLoad)
        {
            cam_calCfg.u4Length = CAM_CAL_PART_NUMBERS_COUNT*LSC_DATA_BYTES; //sizeof(ulModuleNumber)
            cam_calCfg.u4Offset = size[LayoutType]+CUSTOM_CAM_CAL_PART_NUMBERS_START_ADD*LSC_DATA_BYTES;
            cam_calCfg.pu1Params= (u8 *)ulModuleNumber;
            err= ioctl(CamcamFID, CAM_CALIOC_S_WRITE, &cam_calCfg);		
            bFirstLoad = FALSE;
            #ifdef CUSTOM_CAM_CAL_NEW_MODULE_NUMBER_CHECK
            cam_calCfg.u4Length = CAM_CAL_PART_NUMBERS_COUNT*LSC_DATA_BYTES; //sizeof(ulModuleNumber)
            cam_calCfg.u4Offset = size[LayoutType]+CUSTOM_CAM_CAL_PART_NUMBERS_START_ADD*LSC_DATA_BYTES;
            cam_calCfg.pu1Params= (u8 *)&ulPartNumbertmp[0];
            err= ioctl(CamcamFID, CAM_CALIOC_G_READ, &cam_calCfg);		
            for(i=0;i<CAM_CAL_PART_NUMBERS_COUNT;i++)
            {    
                CAM_CAL_LOG_IF(dumpEnable,"ulPartNumbertmp[%d]=0x%8x\n",i,ulPartNumbertmp[i]);							
            }    
            #endif			
        }
#endif
