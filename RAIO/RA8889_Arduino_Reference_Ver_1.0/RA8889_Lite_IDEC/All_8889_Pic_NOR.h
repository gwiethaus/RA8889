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

const INFO  BINARY_INFO[10]=
{
  /*  No. , Width , Height , Size , Start Address  */ 
  {1,640,480,921600,0},          /*     p1 , element 0     */
  {2,640,480,921600,921600},          /*     p2 , element 1     */
  {3,640,480,921600,1843200},          /*     p3 , element 2     */
  {4,640,480,1228800,2764800},          /*     Aircraft_PNG- , element 3     */
  {5,320,240,307200,3993600},          /*     Car_PNG , element 4     */
  {6,640,480,1228800,4300800},          /*     Lion_PNG , element 5     */
  {7,320,240,23744,5529600},          /*     bird , element 6     */
  {8,320,240,21817,5553344},          /*     cat , element 7     */
  {9,320,240,25907,5575161},          /*     fish , element 8     */
  {10,USELESS,USELESS,5509186,5601068},          /*     demo_video320240 , element 9     */
};

typedef enum
{
  p1=0,          /*     0     */
  p2,          /*     1     */
  p3,          /*     2     */
  Aircraft_PNG,          /*     3     */
  Car_PNG,          /*     4     */
  Lion_PNG,          /*     5     */
  bird,          /*     6     */
  cat,          /*     7     */
  fish,          /*     8     */
  demo_video320240,          /*     9     */
}PICTURE_NAME;
