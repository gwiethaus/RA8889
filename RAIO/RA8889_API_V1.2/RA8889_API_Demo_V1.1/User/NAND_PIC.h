/*********************************************************************/
//
//	brief: This file provides an easy way to obtain image data information.
//
//
//	BINARY_INFO[PICTURE_NAME].number      => To obtain sequence number information of the "PICTURE_NAME"(.bmp or .jpg) which is generated in the excel file
//	BINARY_INFO[PICTURE_NAME].img_width       => To obtain image width information of the "PICTURE_NAME" (.bmp or .jpg)
//	BINARY_INFO[PICTURE_NAME].img_height      => To obtain image height information of the "PICTURE_NAME" (.bmp or .jpg)
//	BINARY_INFO[PICTURE_NAME].img_size        => To obtain an image size which is converted and stored in the binary file, please notice, the binary file is combined and converted from the several files (.bmp or .jpg)
//	BINARY_INFO[PICTURE_NAME].start_addr  => To obtain start address of binary file which is converted from the file "PICTURE_NAME" (.bmp or .jpg)
//
//	For example, the struct and enum type as:
//
//	const INFO (code) BINARY_INFO[3]=
//	{
//  	/*  No. , Width , Height , Size (bytes) , Start Address  */
//  	{1,1280,800,2048000,0},          /*     pic_01_1280x800 , element 0     */
//  	{2,320,240,153600,2048000},      /*     RAiO , element 1     */
//  	{3,128,128,32768,2201600},       /*     S1_16 , element 2     */
//	}
//
//
//  typedef enum
//  {
//    pic_01_1280x800=0,  /*     0     */
//    RAiO,               /*     1     */
//    S1_16,              /*     2     */
//  }PICTURE_NAME;
//
//	(1).  To obtain size informations of the file "pic_01_1280x800" (.bmp or jpg),
//        the C code is:
//
//									long param1 = 0;
//
//									param1 = BINARY_INFO[pic_01_1280x800].img_size;
//									/*  or  */
//									param1 = BINARY_INFO[0].img_size;
//
//									/* the param1 is 2048000 (bytes) */
//
//
//	(2).  To obtain start address informations of the file "S1_16" (.bmp or jpg),
//        the C code is:
//
//									long param2 = 0;
//
//									param2 = BINARY_INFO[S1_16].start_addr;
//									/*  or  */
//									param2 = BINARY_INFO[2].start_addr;
//
//									/* the param2 is 2201600 (bytes) */
//
/*********************************************************************/
enum
{
  USELESS=0,
};

typedef struct _info
{
  unsigned short number;
  unsigned short img_width;
  unsigned short img_height;
  unsigned long img_size;
  unsigned long start_addr;
}INFO;

  /* The 'code' is KEIL C 8051 instruction, please refer to http://www.keil.com/support/man/docs/c51/c51_le_code.htm */
  /* If you do not use the 8051 microcontroller system, please remove the 'code' instruction. */

const INFO BINARY_INFO[8]=
{
  /*  No. , Width , Height , Size , Start Address  */ 
  {1,800,480,1152000,0},          /*     A1_1000 , element 0     */
  {2,800,480,42237,563},          /*     output_305 , element 1     */
  {3,800,480,30131,584},          /*     output_440 , element 2     */
  {4,800,480,28364,599},          /*     output_755 , element 3     */
  {5,800,480,43942,613},          /*     output_2318 , element 4     */
  {6,USELESS,USELESS,44414546,635},          /*     mjpeg , element 5     */
  {7,80,80,25600,22322},          /*     «ö¶s_play2 , element 6     */
  {8,80,80,25600,22335},          /*     «ö¶s_stop2_d , element 7     */
};

typedef enum
{
  A1_1000=0,          /*     0     */
  output_305,          /*     1     */
  output_440,          /*     2     */
  output_755,          /*     3     */
  output_2318,          /*     4     */
  mjpeg,          /*     5     */
  play2,          /*     6     */
  stop2_d,          /*     7     */
}PICTURE_NAME;

