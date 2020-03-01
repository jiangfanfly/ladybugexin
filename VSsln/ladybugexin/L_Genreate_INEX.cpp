#include <iostream>
#include <io.h>
#include <direct.h>

#include "Mladybug.h"
//#include "calibGenerateIn.h"
#include "calculateInnparms.h"
//#include "calculateInnparms.h"

using namespace std;

int main(int argc, char **argv)
{
    string prgFilePath;
    string outPath;
    string Exoutpath;
    string Innoutpath;
    string InvInnoutpath;
	int generate;

    if(argc == 4)
    {
        prgFilePath = argv[1];
        outPath = argv[2];
		generate = atoi(argv[3]);

		if (0 != _access(outPath.c_str(), 0))
		{
			// if this folder not exist, create a new one.
			_mkdir(outPath.c_str());   // 返回 0 表示创建成功，-1 表示失败
			cout << "no path, create file folder :" << outPath << endl;
		}

        string folderPath = outPath + "\\CalibrationResult";
		if (0 != _access(folderPath.c_str(), 0))
		{
			// if this folder not exist, create a new one.
			_mkdir(folderPath.c_str());   // 返回 0 表示创建成功，-1 表示失败
			cout << "create file folder :" << folderPath << endl;
		}

        Exoutpath = folderPath + "\\ExPara.txt";
        Innoutpath = folderPath + "\\InnPara.txt";
        InvInnoutpath = folderPath + "\\InvInnPara.txt";
    }
    else if(argc == 3)
    {
        //prgFilePath = "F:\\ladybug\\Ladybug-000000.pgr";
		prgFilePath = argv[1];
		generate = atoi(argv[2]);
        string folderPath = ".\\CalibrationResult";
		if (0 != _access(folderPath.c_str(), 0))
		{
			// if this folder not exist, create a new one.
			_mkdir(folderPath.c_str());   // 返回 0 表示创建成功，-1 表示失败
			cout << "create file folder :" << folderPath << endl;
		}
        Exoutpath = ".\\CalibrationResult\\ExPara.txt";
        Innoutpath = ".\\CalibrationResult\\InnPara.txt";
        InvInnoutpath = ".\\CalibrationResult\\InvInnPara.txt";

    }
	else
	{
		std::cout << "Usage: .\\ladybugexin   PATH_TO_PGR 1" << std::endl;
		std::cout << "or" << std::endl;
		std::cout << ".\\ladybugexin   PATH_TO_PGR  PATH_TO_OUTPUT 1" << std::endl;
		std::cout << "!! note: 1:generate images ;   0: don't generate images!" << endl;

		return false;
	}
    
    
    Mladybug LB;
    LadybugContext mm;
    LadybugError err;
    err = ladybugCreateContext( &mm );
    LB.AtctivateLadybug(prgFilePath);

    LB.GenerateExPara(Exoutpath);
    LB.GenerateDistRectPoints();
    //ladybugDestroyStreamContext( prgFilePath );

	// output timestamp
	ofstream oftime;
	string outimgPath;
	if (outPath.empty() == true)
		outimgPath = ".\\timestamp_FishEye.txt";
	else
		outimgPath = outPath + ".\\timestamp_FishEye.txt";

	oftime.open(outimgPath.c_str());
	for (int i = 0; i < LB.m_nAllImgNum; i++)
	{
		long long int t = 1403636580763550000 + i * 1e9;
		oftime << to_string(t) << endl;
	}
	oftime.close();


	// optimizer
	cout << endl << "===========start optimization of Inn-InvInn Parameters =====================" << endl;
	google::InitGoogleLogging(argv[0]);

	std::vector<std::vector<double>> InnInitial = LB.vIn;
	std::vector<double> Inn[6];
	std::vector<double> InnInv[6];
	int nWidth, nHight;
	nWidth = 1616;
	nHight = 1232;
	std::ofstream out1(Innoutpath);
	std::ofstream out2(InvInnoutpath);
	for (int i = 0; i < 6; i++)
	{
		cout << "optmize " << i + 1 << " camera!" << endl;
		std::vector<std::vector<double>> points = LB.vDistRectP[i];
		cout << "Inn ... ..." << endl;
		std::vector<std::vector<double>> pointsForInitial =
			CalculateDistortPara(points, InnInitial[i], nWidth, nHight, Inn[i]);
		cout << "InvInn ... ..." << endl;
		CalculateDistortParaInv(points, InnInitial[i], nWidth, nHight, InnInv[i], pointsForInitial);

		out1 << i << " ";
		for (size_t n = 0; n < Inn[i].size(); n++) {

			if (i != Inn[n].size() - 1) {
				out1 << Inn[i][n] << " ";
			}
			else {
				out1 << Inn[i][n] << std::endl;
			}
		}
		out1 << endl;

		out2 << i << " ";
		for (size_t n = 0; n < InnInv[i].size(); n++) {

			if (i != InnInv[i].size() - 1) {
				out2 << InnInv[i][n] << " ";
			}
			else {
				out2 << InnInv[i][n] << std::endl;
			}
		}
		out2 << endl;

	}

	out1.close();
	out2.close();
	cout <<endl << "============================================="<< endl 
		<< "Inn , InvInn and Ex parameters output!" << endl << endl;

	
	if (generate == 1)
	{
		cout << endl << "=============================================" << endl
			<< "start Export images!" << endl << endl;
		// import images (camera:0-4) to Img0...Img4
		LadybugProcessedImage processedImage;
		for (int i = 0; i < 5; i++)
		{
			string outimgPath;
			if (outPath.empty() == true)
				outimgPath = ".\\Img" + to_string(i);
			else
				outimgPath = outPath + "\\Img" + to_string(i);

			if (0 != _access(outimgPath.c_str(), 0))
			{
				// if this folder not exist, create a new one.
				_mkdir(outimgPath.c_str());   // 返回 0 表示创建成功，-1 表示失败
				cout << "create file folder :" << outimgPath << endl;
			}
		}

		LB.ExportImges(outPath);
		cout << endl << "============== over ===================" << endl;

	}


    return 0;
}