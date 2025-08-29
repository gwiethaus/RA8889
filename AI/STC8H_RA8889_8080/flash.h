/*	Packaging Tool x64 (Ver: 2.0.1.0)		===Data for NOR FLASH===

No.,	Full Path,		Size(byte),	Address,	Width,	Height,	Output Format;

1,"D:\图片库\800x480\sky.jpg",1152000,0,800,480,24bit (RGB:888);
2,"D:\图片库\800x480\sunshine1.jpg",1152000,1152000,800,480,24bit (RGB:888);
3,"D:\图片库\800x480\tree-surrounded-by-water-800x480.jpg",1152000,2304000,800,480,24bit (RGB:888);
4,"D:\图片库\800x480\chaodao_800x480.jpg",107997,3456000,800,480,JPG (Baseline);
5,"D:\图片库\800x480\colorful-drink-shots-hd-800x480.jpg",119676,3563997,800,480,JPG (Baseline);
6,"D:\图片库\800x480\coloured-pencils-great-painting-800x480.jpg",126507,3683673,800,480,JPG (Baseline);
7,"D:\图片库\800x480\girl-in-yellow-reading-a-book-on-the-grass-in-garden-800x480.jpg",116595,3810180,800,480,JPG (Baseline);
8,"D:\图片库\800x480\yellow-butterfly-on-the-beautiful-spring-flowers-800x480.jpg",127363,3926775,800,480,JPG (Baseline);
9,"D:\图片库\png\logo_bbs.png",160000,4054138,400,100,PNG (ARGB:8888);
10,"D:\图片库\png\logo_bbs.png",80000,4214138,400,100,PNG (ARGB:4444);
11,"D:\图片库\avi\mjpeg_600x250_20f_best.avi",7386870,4294138,600,250,AVI (Motion JPEG);

*/

enum
{
	USELESS = 0,
};

typedef struct _info
{
	unsigned short number;
	unsigned short img_width;
	unsigned short img_height;
	unsigned long img_size;
	unsigned long start_addr;
}INFO;

const INFO BINARY_INFO[11] =
{
	/*  No. , Width , Height , Size , Start Address  */
	{1,800,480,1152000,0},	/*	sky.jpg	*/
	{2,800,480,1152000,1152000},	/*	sunshine1.jpg	*/
	{3,800,480,1152000,2304000},	/*	tree-surrounded-by-water-800x480.jpg	*/
	{4,800,480,107997,3456000},	/*	chaodao_800x480.jpg	*/
	{5,800,480,119676,3563997},	/*	colorful-drink-shots-hd-800x480.jpg	*/
	{6,800,480,126507,3683673},	/*	coloured-pencils-great-painting-800x480.jpg	*/
	{7,800,480,116595,3810180},	/*	girl-in-yellow-reading-a-book-on-the-grass-in-garden-800x480.jpg	*/
	{8,800,480,127363,3926775},	/*	yellow-butterfly-on-the-beautiful-spring-flowers-800x480.jpg	*/
	{9,400,100,160000,4054138},	/*	logo_bbs.png	*/
	{10,400,100,80000,4214138},	/*	logo_bbs.png	*/
	{11,600,250,7386870,4294138},	/*	mjpeg_600x250_20f_best.avi	*/
};

typedef enum
{
	sky=0,		/*	0	*/
	sunshine1,		/*	1	*/
	tree_surrounded_by_water_800x480,		/*	2	*/
	chaodao_800x480,		/*	3	*/
	colorful_drink_shots_hd_800x480,		/*	4	*/
	coloured_pencils_great_painting_800x480,		/*	5	*/
	girl_in_yellow_reading_a_book_on_the_grass_in_garden_800x480,		/*	6	*/
	yellow_butterfly_on_the_beautiful_spring_flowers_800x480,		/*	7	*/
	logo_bbs,		/*	8	*/
	logo_bbs1,		/*	9	*/
	mjpeg_600x250_20f_best,		/*	10	*/
}FILE_NAME;

