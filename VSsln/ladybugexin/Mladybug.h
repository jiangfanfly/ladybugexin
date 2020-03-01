#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ladybug/ladybug.h>
#include <ladybug/ladybuggeom.h>
#include <ladybug/ladybugstream.h>
#include <ladybug/ladybugrenderer.h>

using namespace std;

struct InPART
{
	double x0rectified, y0rectified, frectified;
	double x0distorted, y0distorted;
	int CamID;
};
struct DistRectP
{
	double Rectifiedx, Rectifiedy;
	double Distortedx, Distortedy;
};

class Mladybug
{
public:
    Mladybug(/* args */);
    ~Mladybug();

    // 初始化后调用
    bool AtctivateLadybug(string FilePath);

    bool RectifyImg(int CamID, double xd, double yd, double &xr, double &yr);
	bool UnRectifyImg(int CamID, double xr, double yr, double &xd, double &yd);

	bool GetRectifiedImgCenter(int CamID, double &x0, double &y0);
	bool GetRectifiedImgFocalLength(int CamID, double &f);
	bool GetUnitCameraExtrinsics(int CamID, double Ex[6]);
	bool isHighBitDepth(LadybugDataFormat format);

	bool GenerateDistRectPoints();
	bool GenerateExPara(string exoutpath);

	bool ExportImges(string outPath);
	string num2str(int i);


    LadybugStreamContext m_streamContextLadybug;
	LadybugContext m_contextLadybug;
	LadybugStreamHeadInfo m_streamHeaderInfo;
	
	LadybugImage m_currentLadybugImage;
	int m_nAllImgNum;
	string m_csPOSInfoFile, m_csWorkDir;
	double *m_pRoadHei;
	int *m_nDeleteFlag;

	double m_Tx[18];
	
	int m_nCurrentImgID;//从0开始

	vector<vector<double>> vIn;

	vector<vector<double>> vDistRectP[6];  // 1 相机   2 点数   

	LadybugError error;

	// config
	int iOutputImageWidth = 2048;
	int iOutputImageHeight = 1024;
	LadybugOutputImage outputImageType = LADYBUG_ALL_RAW_IMAGES;
	LadybugSaveFileFormat outputImageFormat = LADYBUG_FILEFORMAT_JPG;
	LadybugColorProcessingMethod colorProcessingMethod = LADYBUG_HQLINEAR;
	unsigned int iTextureWidth, iTextureHeight;
	unsigned char* arpTextureBuffers[LADYBUG_NUM_CAMERAS] = { NULL, NULL, NULL, NULL, NULL, NULL };

};

