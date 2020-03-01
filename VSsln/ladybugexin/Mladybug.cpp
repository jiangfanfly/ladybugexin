#include "Mladybug.h"

Mladybug::Mladybug(/* args */)
{
	m_streamContextLadybug = NULL;
	m_contextLadybug = NULL;
	m_pRoadHei = NULL;
	m_nDeleteFlag = NULL;
}

Mladybug::~Mladybug()
{
	if ( m_streamContextLadybug != NULL)
	{
		ladybugDestroyStreamContext( &m_streamContextLadybug );
		m_streamContextLadybug = NULL;
	}
	if ( m_contextLadybug != NULL)
	{
		ladybugDestroyContext( &m_contextLadybug );
		m_contextLadybug = NULL;
	}
	if(m_pRoadHei)
	{
		delete m_pRoadHei;
		m_pRoadHei = NULL;
	}
	if(m_nDeleteFlag)
	{
		delete m_nDeleteFlag;
		m_nDeleteFlag = NULL;
	}
}

bool Mladybug::AtctivateLadybug(string FilePath)
{

	//Open the stream file
	if ( m_streamContextLadybug != NULL)
	{
       ladybugDestroyStreamContext( &m_streamContextLadybug );
       m_streamContextLadybug = NULL;
	}
	error = ladybugCreateStreamContext( &m_streamContextLadybug );
	if(error != LADYBUG_OK)
	{
		string outerr =  ladybugErrorToString( error );
		cout << outerr << endl;
		return false;
	}
	error = ladybugInitializeStreamForReading( m_streamContextLadybug, FilePath.c_str());
	if(error != LADYBUG_OK)
	{
		string outerr = ladybugErrorToString( error );
		cout << outerr << endl;
		return false;
	}

	//Get the total number of images in this stream
	unsigned int nTotalImageNum;
    error = ladybugGetStreamNumOfImages( m_streamContextLadybug, &nTotalImageNum );
    if(error != LADYBUG_OK)
	{
		string outerr = ladybugErrorToString( error );
		cout << outerr << endl;
		return false;
	}
	if(nTotalImageNum <= 0)
	{
		cout << "No image in stream" << endl;
		return false;
	}
	m_nAllImgNum = nTotalImageNum;
	cout << "Load " << m_nAllImgNum << " images!" <<endl;
	//Read Header
	error = ladybugGetStreamHeader( m_streamContextLadybug, &m_streamHeaderInfo );
	if(error != LADYBUG_OK)
	{
		string outerr = ladybugErrorToString( error );
		cout << outerr << endl;
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//Initial Content 1
	if ( m_contextLadybug != NULL)
	{
       ladybugDestroyContext( &m_contextLadybug );
       m_contextLadybug = NULL;
	}
	error = ladybugCreateContext( &m_contextLadybug );
    if ( error != LADYBUG_OK) 
	{
	   string outerr = ladybugErrorToString( error );
	   cout << outerr << endl;
	   return false;
    }

	// Load config file.
	const char* pszTempFileName   = NULL;
	string a = ".\\temp.cal";
	pszTempFileName = a.c_str();

	error = ladybugGetStreamConfigFile ( m_streamContextLadybug, pszTempFileName );  
	 if(error != LADYBUG_OK)
	 {
	 	string outerr = ladybugErrorToString( error );
	 	return false;
	 }
	error = ladybugLoadConfig(m_contextLadybug, pszTempFileName);
	 if(error != LADYBUG_OK)
	 {
	 	string outerr =ladybugErrorToString( error );
	 	return false;
	 }
	
	// DeleteFile (pszTempFileName);
	// free( pszTempFileName );
    // }

	// Set color tile format
	ladybugSetColorTileFormat( m_contextLadybug, m_streamHeaderInfo.stippledFormat );
	error = ladybugSetColorProcessingMethod(m_contextLadybug, colorProcessingMethod);
	
	LadybugImage image;
	error = ladybugReadImageFromStream(m_streamContextLadybug, &image);

	//
	// Allocate the texture buffers that hold the color-processed images for all cameras
	//
	if (colorProcessingMethod == LADYBUG_DOWNSAMPLE4 || colorProcessingMethod == LADYBUG_MONO)
	{
		iTextureWidth = image.uiCols / 2;
		iTextureHeight = image.uiRows / 2;
	}
	else
	{
		iTextureWidth = image.uiCols;
		iTextureHeight = image.uiRows;
	}

	const unsigned int outputBytesPerPixel = isHighBitDepth(m_streamHeaderInfo.dataFormat) ? 2 : 1;
	for (int i = 0; i < LADYBUG_NUM_CAMERAS; i++)
	{
		arpTextureBuffers[i] = new unsigned char[iTextureWidth * iTextureHeight * 4 * outputBytesPerPixel];
	}

	
	
	// Initialize LadybugImage structure
    m_currentLadybugImage.dataFormat = m_streamHeaderInfo.dataFormat;
    m_currentLadybugImage.resolution = m_streamHeaderInfo.resolution;
	  switch( m_currentLadybugImage.resolution )
	  {
	  case LADYBUG_RESOLUTION_1616x1232:
		 m_currentLadybugImage.uiCols = 1616;
		 m_currentLadybugImage.uiRows = 1232;
		 break;
 	  /*case LADYBUG_RESOLUTION_2448x2048:
 		 m_currentLadybugImage.uiCols = 2448;
 		 m_currentLadybugImage.uiRows = 2048;
 		 break;*/
	// 	  case LADYBUG_RESOLUTION_1216x1216:
	// 		 m_currentLadybugImage.uiCols = 1216;
	// 		 m_currentLadybugImage.uiRows = 1216;
	// 		 break;
	  case LADYBUG_RESOLUTION_1024x768:
		 m_currentLadybugImage.uiCols = 1024;
		 m_currentLadybugImage.uiRows = 768;
		 break;
	  default:
		 m_currentLadybugImage.uiCols = 1024;
		 m_currentLadybugImage.uiRows = 768;
		 break;
	  }
    //	m_currentLadybugImage2 = m_currentLadybugImage;

	ladybugSetFalloffCorrectionFlag( m_contextLadybug, false );

	// For ladybug5
    // ladybugSetCorrectionAttenuation( m_contextLadybug, 1.0 );
	ladybugSetFalloffCorrectionAttenuation( m_contextLadybug, 1.0 );

	error = ladybugConfigureOutputImages( m_contextLadybug,  LADYBUG_PANORAMIC|LADYBUG_SPHERICAL );
	if(error != LADYBUG_OK)
	{
		string outerr = ladybugErrorToString( error );
		return false;
	}

	cout << "read pgr file sucessful!" << endl;

	return true;
}

bool Mladybug::GenerateDistRectPoints()
{

	for(int CamID = 0; CamID < 6; CamID++)
	{
		int Pidx = 0;
		for (int i = 57; i < 1616; i = i + 150)
		{
			for (int j = 75; j < 1232; j = j + 120)
			{
				double Rectifiedx, Rectifiedy;
				ladybugRectifyPixel(m_contextLadybug, CamID, j, i, &Rectifiedy, &Rectifiedx);
				
				//DistRectP drp;
				//drp.Distortedx =  i;
				//drp.Distortedy =  j;
				//drp.Rectifiedx =  Rectifiedx;
				//drp.Rectifiedy =  Rectifiedy;
				vector<double> drp;
				drp.resize(4);
				drp[0] = i;
				drp[1] = j;
				drp[2] = Rectifiedx;
				drp[3] = Rectifiedy;
				
				vDistRectP[CamID].emplace_back(drp);
				Pidx++;
			}
		}
	}

	return true;
}

bool Mladybug::GenerateExPara(string exoutpath)
{
	ofstream outf;
	outf.open(exoutpath.c_str());
	for (int i = 0; i < 6; i++)
	{
		double *Ex = new double[6];
		ladybugGetCameraUnitExtrinsics(m_contextLadybug, i, Ex);
		outf  << Ex[0] << "   " << Ex[1] << "   " << Ex[2] << "   " << Ex[3] << "   " << Ex[4] << "   " << Ex[5] << std::endl;
		cout << Ex[0] << "   " << Ex[1] << "   " << Ex[2] << "   " << Ex[3] << "   " << Ex[4] << "   " << Ex[5] << std::endl;
		delete[] Ex;

		double x0rectified, y0rectified, frectified;
		double x0distorted, y0distorted;
		ladybugGetCameraUnitImageCenter(m_contextLadybug, i, &x0rectified, &y0rectified);
		ladybugGetCameraUnitFocalLength(m_contextLadybug, i, &frectified);
		ladybugUnrectifyPixel(m_contextLadybug, i, y0rectified, x0rectified, &y0distorted, &x0distorted);
		//InPART in;
		//in.CamID = i;
		//in.frectified = frectified;
		//in.x0distorted = x0distorted;
		//in.y0distorted = y0distorted;
		//in.x0rectified = x0rectified;
		//in.y0rectified = y0rectified;
		vector<double> in;
		in.resize(6);
		in[0] = x0distorted;
		in[1] = y0distorted;
		in[2] = x0rectified;
		in[3] = y0rectified;
		in[4] = frectified;

		vIn.emplace_back(in);
	}
	
	outf.close();

	return true;
}


bool Mladybug::isHighBitDepth(LadybugDataFormat format)
{
	return (format == LADYBUG_DATAFORMAT_RAW12 ||
		format == LADYBUG_DATAFORMAT_HALF_HEIGHT_RAW12 ||
		format == LADYBUG_DATAFORMAT_COLOR_SEP_JPEG12 ||
		format == LADYBUG_DATAFORMAT_COLOR_SEP_HALF_HEIGHT_JPEG12 ||
		format == LADYBUG_DATAFORMAT_RAW16 ||
		format == LADYBUG_DATAFORMAT_HALF_HEIGHT_RAW16);
}

bool Mladybug::ExportImges(string outPath)
{
	error = ladybugConfigureOutputImages(m_contextLadybug, outputImageType);
	
	LadybugImage image;
	/*for (int n = 0; n < 6; n++)
	{*/
	error = ladybugGoToImage(m_streamContextLadybug, 0);
		
		for (int i = 0; i < m_nAllImgNum; i++)
		{
			printf("Processing frame %u of %u\n", i, m_nAllImgNum-1);

			//
			// Read one frame from stream
			//
			error = ladybugReadImageFromStream(m_streamContextLadybug, &image);

			//
			// Convert the image to BGRU format texture buffers
			//
			error = ladybugConvertImage(m_contextLadybug, &image, arpTextureBuffers, isHighBitDepth(m_streamHeaderInfo.dataFormat) ? LADYBUG_BGRU16 : LADYBUG_BGRU);

			//
			// Update the textures on graphics card
			//
			error = ladybugUpdateTextures(m_contextLadybug, LADYBUG_NUM_CAMERAS, (const unsigned char**)arpTextureBuffers, isHighBitDepth(m_streamHeaderInfo.dataFormat) ? LADYBUG_BGRU16 : LADYBUG_BGRU);

			////ladybugSetOffScreenImageSize(m_contextLadybug, outputImageType, 1616, 1232);
			//LadybugOutputImage outputImageType1 = LADYBUG_RECTIFIED_CAM0;
			LadybugProcessedImage processedImage;
			//error = ladybugRenderOffScreenImage(
			//	m_contextLadybug, outputImageType1, LADYBUG_BGR, &processedImage);

			for (int iCamera = 0; iCamera < LADYBUG_NUM_CAMERAS-1; iCamera++)
			{
				/*_snprintf(pszFilename, _MAX_PATH, "%sladybug_Color_%s_Cam%d.bmp",
					pszDestDir, pszFileNameString, iCamera);*/
				string outimgPath;
				if(outPath.empty() == true)
					outimgPath = ".\\Img" + to_string(iCamera);
				else 
					outimgPath = outPath + "\\Img" + to_string(iCamera);
				processedImage.uiRows = 1232;
				processedImage.uiCols = 1616;
				processedImage.pData = arpTextureBuffers[iCamera];
				processedImage.pixelFormat = LADYBUG_BGRU;
				string imgPath = outimgPath + "\\" + num2str(i) + ".jpg";

				error = ladybugSaveImage(m_contextLadybug, &processedImage, imgPath.c_str(), outputImageFormat, true);
			}
			
		}

	//}
	return true;
}

string Mladybug::num2str(int i)

{

	char ss[10];

	sprintf(ss, "%05d", i);

	return ss;

}