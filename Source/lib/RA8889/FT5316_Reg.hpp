//FT5x06 definitions I2C
#define FT_I2C_ADDRESS                         0x38       //CTP IIC Address
#define FT_I2C_CMDWRITE                        0x70       //Write Command for I2C
#define FT_I2C_DATAWRITE                       0x00       //Write Data for I2C
#define FT_I2C_DATAREAD                        0x00       //Read Data from I2C

//Multitoque
#define FT5316_MAX_TOUCHES                     0x05      //Max multitouch allowed (Touch Register Sets)

//Register address

#define FT_DEVIDE_MODE                         0x00      //Device mode [6:4] 
#define FT_GEST_ID                             0x01      //Gesture ID[7:0]
#define FT_TD_STATUS                           0x02      //Frame remaining or number of events (Win7 protocol)[7:4], Number of touch points[3:0]
#define FT_TOUCH1_XH                           0x03      //1st Event Flag [7:6], 1st Touch X High byte Position[3:0]
#define FT_TOUCH1_XL                           0x04      //1st Touch X Low byte Position[7:0]
#define FT_TOUCH1_YH                           0x05      //1st Touch ID[7:4], 1st Touch Y High byte Position[3:0] 
#define FT_TOUCH1_YL                           0x06      //1st Touch Y Low byte Position[7:0]
#define FT_TOUCH1_WEIGHT                       0x07      //1st Touch Weight[7:0]
#define FT_TOUCH1_MISC                         0x08      //1st Touch Area[7:4], 1st Touch Direction [3:2], 1st Touch Speed [1:0]
#define FT_TOUCH2_XH                           0x09      //2nd Event Flag [7:6], 2nd Touch X High byte Position[3:0]
#define FT_TOUCH2_XL                           0x0a      //2nd Touch X Low byte Position[7:0]
#define FT_TOUCH2_YH                           0x0b      //2nd Touch ID[7:4], 2nd Touch Y High byte Position[3:0]
#define FT_TOUCH2_YL                           0x0c      //2nd Touch Y Low byte Position[7:0]
#define FT_TOUCH2_WEIGHT                       0x0d      //2nd Touch Weight[7:0]
#define FT_TOUCH2_MISC                         0x0e      //2nd Touch Area[7:4], 2nd Touch Direction [3:2], 2nd Touch Speed [1:0]
#define FT_TOUCH3_XH                           0x0f      //3th Event Flag [7:6], 3th Touch X High byte Position[3:0]
#define FT_TOUCH3_XL                           0x10      //3th Touch X Low byte Position[7:0]
#define FT_TOUCH3_YH                           0x11      //3th Touch ID[7:4], 3th Touch Y High byte Position[3:0] 
#define FT_TOUCH3_YL                           0x12      //3th Touch Y Low byte Position[7:0]
#define FT_TOUCH3_WEIGHT                       0x13      //3th Touch Weight[7:0]
#define FT_TOUCH3_MISC                         0x14      //3th Touch Area[7:4], 3th Touch Direction [3:2], 3th Touch Speed [1:0]
#define FT_TOUCH4_XH                           0x15      //4th Event Flag [7:6], 4th Touch X High byte Position[3:0]
#define FT_TOUCH4_XL                           0x16      //4th Touch X Low byte Position[7:0]
#define FT_TOUCH4_YH                           0x17      //4th Touch ID[7:4], 4th Touch Y High byte Position[3:0]
#define FT_TOUCH4_YL                           0x18      //4th Touch Y Low byte Position[7:0]
#define FT_TOUCH4_WEIGHT                       0x19      //4th Touch Weight[7:0]
#define FT_TOUCH4_MISC                         0x1a      //4th Touch Area[7:4], 4th Touch Direction [3:2], 4th Touch Speed [1:0]
#define FT_TOUCH5_XH                           0x1b      //5th Event Flag [7:6], 5th Touch X High byte Position[3:0]
#define FT_TOUCH5_XL                           0x1c      //5th Touch X Low byte Position[7:0]
#define FT_TOUCH5_YH                           0x1d      //5th Touch ID[7:4], 5th Touch Y High byte Position[3:0]
#define FT_TOUCH5_YL                           0x1e      //5th Touch Y Low byte Position[7:0]
#define FT_TOUCH5_WEIGHT                       0x1f      //5th Touch Weight[7:0]
#define FT_TOUCH5_MISC                         0x20      //5th Touch Area[7:4], 5th Touch Direction [3:2], 5th Touch Speed [1:0]

    //Touch Status Event FT_TOUCHn_XH
    #define PUT_DOWN                           0x00      //Leave touch
    #define PUT_UP                             0x40      //Pressed touch
    #define PUT_CONTACT                        0x80      //Contact touch

#define FT_ID_G_THGROUP                        0x80      //Valid touching detect threshold. 
#define FT_ID_G_THPEAK                         0x81
#define FT_ID_G_THCAL                          0x82
#define FT_ID_G_COMPENSATE_STATUS              0x83
#define FT_D_G_COMPENSATE_FLAG                 0x84
#define FT_ID_G_THDIFF                         0x85
#define FT_ID_G_CTRL                           0x86
#define FT_ID_G_TIME_ENTER_MONITOR             0x87
#define FT_ID_G_PERIODACTIVE                   0x88
#define FT_ID_G_PERIODMONITOR                  0x89
#define FT_ID_G_SCAN_RATE                      0x8a
#define FT_ID_G_CHARGER_STATE                  0x8b
#define FT_ID_G_SCAN_REGB                      0x8c
#define FT_ID_G_SCAN_CAP                       0x8d
#define FT_ID_G_SCAN_FILTERMODE                0x8e
#define FT_ID_G_SCAN_REFRESH                   0x8f
#define FT_ID_G_MOVSTH_I                       0x90
#define FT_ID_G_MOVSTH_N                       0x91
#define FT_ID_G_LEFT_RIGHT_OFFSET              0x92
#define FT_ID_G_UP_DOWN_OFFSET                 0x93
#define FT_ID_G_DISTANCE_LEFT_RIGHT            0x94
#define FT_ID_G_DISTANCE_UP_DOWN               0x95
#define FT_ID_G_ZOOM_DIS_SQR                   0x97
#define FT_ID_G_MAX_X_HIGH                     0x98
#define FT_ID_G_MAX_X_LOW                      0x99
#define FT_ID_G_MAX_Y_HIGH                     0x9a
#define FT_ID_G_MAX_Y_LOW                      0x9b
#define FT_ID_G_K_X_HIGH                       0x9c
#define FT_ID_G_K_X_LOW                        0x9d
#define FT_ID_G_K_Y_HIGH                       0x9e
#define FT_ID_G_K_Y_LOW                        0x9f
#define FT_ID_G_AUTO_CLB_MODE                  0xa0
#define FT_ID_G_LIB_VERSION_H                  0xa1
#define FT_ID_G_LIB_VERSION_L                  0xa2
#define FT_ID_G_CIPHER                         0xa3      //Chip vendor ID
    #define FT5X06_ID                          0x55      //ID FT5x06
    #define FT5X16_ID                          0x0a      //ID FT5x16
    #define FT5X36_ID                          0x14      //ID FT5x36
    #define FT6X06_ID                          0x06      //ID FT6x06
    #define FT6X36_ID                          0x36      //ID FT5x36
#define FT_ID_G_MODE                           0xa4
#define FT_ID_G_PMODE                          0xa5      //Power Consume Mode
#define FT_ID_G_FIRMID                         0xa6      //Firmware ID
#define FT_ID_G_STATE                          0xa7
#define FT_ID_G_VENODRID                       0xa8      //CTPM Vendor ID
#define FT_ID_G_ERR                            0xa9
#define FT_ID_G_CLB                            0xaa
#define FT_ID_G_STATIC_TH                      0xab
#define FT_ID_G_MID_SPEED_TH                   0xac
#define FT_ID_G_HIGH_SPEED_TH                  0xad
#define FT_ID_G_DRAW_LINE_TH                   0xae
#define FT_ID_G_RELEASE_CODE_ID                0xaf
#define FT_ID_G_FACE_DEC_MODE                  0xb0 
#define FT_ID_G_PRESIZE_EN                     0xb2
#define FT_ID_G_BIGAREA_PEAK_TH                0xb3
#define FT_D_G_BIGAREA_PEAK_NUM                0xb4
#define FT_ECC                                 0xcc
#define FT_LOG_MSG_CNT                         0xfe 
#define FT_LOG_CUR_CHA                         0xff


















