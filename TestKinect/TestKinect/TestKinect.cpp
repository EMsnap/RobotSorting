// CoodinateMapper.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
// This source code is licensed under the MIT license. Please see the License in License.txt.
// “This is preliminary software and/or hardware and APIs are preliminary and subject to change.”
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <Kinect.h>
#include <opencv2/opencv.hpp>
//Socket 头文件
#include<cstring>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

template<class Interface>

inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL) {
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

struct  xyz{
	float x;
	float y;
	float z;
};

xyz convertxyz(float x, float y, float z) {

	


};

void saveImg(cv::Mat mat) {



}

int server()
{
	//初始化WSA  
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//创建套接字  
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//绑定IP和端口  
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(5050);
	sin.sin_addr.S_un.S_addr = "";
	/*if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}*/
	bind(slisten, (LPSOCKADDR)&sin, sizeof(sin));
	cout << "yes";
	//开始监听  
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	//循环接收数据  
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[255];
	while (true)
	{
		printf("等待连接...\n");
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;
		}
		//printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
		InetPton(AF_INET, _T("192.168.1.1"),&remoteAddr.sin_addr.s_addr);

		//接收数据  
		int ret = recv(sClient, revData, 255, 0);
		if (ret > 0)
		{
			revData[ret] = 0x00;
			printf(revData);
		}

		//发送数据  
		const char * sendData = "你好，TCP客户端！\n";
		send(sClient, sendData, strlen(sendData), 0);
		closesocket(sClient);
	}

	closesocket(slisten);
	WSACleanup();
	return 0;
}


int client()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}
	
	while (true) {
		SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sclient == INVALID_SOCKET)
		{
			printf("invalid socket!");
			return 0;
		}
		sockaddr_in serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(8888);
		InetPton(AF_INET, _T("192.168.1.1"), &serAddr.sin_addr.s_addr);
		if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{  //连接失败 
			printf("connect error !");
			closesocket(sclient);
			return 0;
		}
		string data;
		cin >> data;
		const char * sendData;
		sendData = data.c_str();   //string转const char* 
								   //char * sendData = "你好，TCP服务端，我是客户端\n";
		send(sclient, sendData, strlen(sendData), 0);
		//send()用来将数据由指定的socket传给对方主机
		//int send(int s, const void * msg, int len, unsigned int flags)
		//s为已建立好连接的socket，msg指向数据内容，len则为数据长度，参数flags一般设0
		//成功则返回实际传送出去的字符数，失败返回-1，错误原因存于error 

		char recData[255];
		int ret = recv(sclient, recData, 255, 0);
		if (ret>0) {
			recData[ret] = 0x00;
			printf(recData);
		}
		closesocket(sclient);
	}


	WSACleanup();
	return 0;

}




int _tmain(int argc, _TCHAR* argv[])
{
	server();

	cv::setUseOptimized(true);

	// Sensor
	IKinectSensor* pSensor;
	HRESULT hResult = S_OK;
	hResult = GetDefaultKinectSensor(&pSensor);
	if (FAILED(hResult)) {
		std::cerr << "Error : GetDefaultKinectSensor" << std::endl;
		return -1;
	}

	hResult = pSensor->Open();
	if (FAILED(hResult)) {
		std::cerr << "Error : IKinectSensor::Open()" << std::endl;
		return -1;
	}

	// Source
	IColorFrameSource* pColorSource;
	hResult = pSensor->get_ColorFrameSource(&pColorSource);
	if (FAILED(hResult)) {
		std::cerr << "Error : IKinectSensor::get_ColorFrameSource()" << std::endl;
		return -1;
	}

	IDepthFrameSource* pDepthSource;
	hResult = pSensor->get_DepthFrameSource(&pDepthSource);
	if (FAILED(hResult)) {
		std::cerr << "Error : IKinectSensor::get_DepthFrameSource()" << std::endl;
		return -1;
	}

	// Reader
	IColorFrameReader* pColorReader;
	hResult = pColorSource->OpenReader(&pColorReader);
	if (FAILED(hResult)) {
		std::cerr << "Error : IColorFrameSource::OpenReader()" << std::endl;
		return -1;
	}

	IDepthFrameReader* pDepthReader;
	hResult = pDepthSource->OpenReader(&pDepthReader);
	if (FAILED(hResult)) {
		std::cerr << "Error : IDepthFrameSource::OpenReader()" << std::endl;
		return -1;
	}

	// Description
	IFrameDescription* pColorDescription;
	hResult = pColorSource->get_FrameDescription(&pColorDescription);
	if (FAILED(hResult)) {
		std::cerr << "Error : IColorFrameSource::get_FrameDescription()" << std::endl;
		return -1;
	}

	int colorWidth = 0;
	int colorHeight = 0;
	pColorDescription->get_Width(&colorWidth); // 1920
	pColorDescription->get_Height(&colorHeight); // 1080
	unsigned int colorBufferSize = colorWidth * colorHeight * 4 * sizeof(unsigned char);

	cv::Mat colorBufferMat(colorHeight, colorWidth, CV_8UC4);
	cv::Mat colorMat(colorHeight / 2, colorWidth / 2, CV_8UC4);
	cv::namedWindow("Color");

	IFrameDescription* pDepthDescription;
	hResult = pDepthSource->get_FrameDescription(&pDepthDescription);
	if (FAILED(hResult)) {
		std::cerr << "Error : IDepthFrameSource::get_FrameDescription()" << std::endl;
		return -1;
	}

	int depthWidth = 0;
	int depthHeight = 0;
	pDepthDescription->get_Width(&depthWidth); // 512
	pDepthDescription->get_Height(&depthHeight); // 424
	unsigned int depthBufferSize = depthWidth * depthHeight * sizeof(unsigned short);
	cv::Mat depthBufferMat(depthHeight, depthWidth, CV_16UC1);
	cv::Mat temp(depthHeight, depthWidth, CV_16UC1);
	cv::Mat tempFlip(depthHeight, depthWidth, CV_16UC1);
	cv::Mat img(depthHeight, depthWidth, CV_8UC1);
	cv::Mat depthMat(depthHeight, depthWidth, CV_8UC1);
	cv::namedWindow("Depth");

	// Coordinate Mapper
	ICoordinateMapper* pCoordinateMapper;
	hResult = pSensor->get_CoordinateMapper(&pCoordinateMapper);
	if (FAILED(hResult)) {
		std::cerr << "Error : IKinectSensor::get_CoordinateMapper()" << std::endl;
		return -1;
	}

	cv::Mat coordinateMapperMat(depthHeight, depthWidth, CV_8UC4);
	cv::Mat coordinateMapperMatFlip(depthHeight, depthWidth, CV_8UC4);
	cv::namedWindow("CoordinateMapper");

	unsigned short minDepth, maxDepth;
	pDepthSource->get_DepthMinReliableDistance(&minDepth);
	pDepthSource->get_DepthMaxReliableDistance(&maxDepth);

	int k = 1;
	while (1) {
		// Color Frame
		IColorFrame* pColorFrame = nullptr;
		hResult = pColorReader->AcquireLatestFrame(&pColorFrame);
		if (SUCCEEDED(hResult)) {
			hResult = pColorFrame->CopyConvertedFrameDataToArray(colorBufferSize, reinterpret_cast<BYTE*>(colorBufferMat.data), ColorImageFormat::ColorImageFormat_Bgra);
			if (SUCCEEDED(hResult)) {
			//	cv::resize(colorBufferMat, colorMat, cv::Size(), 0.5, 0.5);
			}
		}
		//SafeRelease( pColorFrame );

		// Depth Frame
		IDepthFrame* pDepthFrame = nullptr;
		hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame);
		if (SUCCEEDED(hResult)) {
			hResult = pDepthFrame->AccessUnderlyingBuffer(&depthBufferSize, reinterpret_cast<UINT16**>(&depthBufferMat.data));
			pDepthFrame->CopyFrameDataToArray(depthHeight * depthWidth, (UINT16 *)temp.data); //先把数据存入16位的图像矩阵中
			if (SUCCEEDED(hResult)) {
				depthBufferMat.convertTo(depthMat, CV_8U, -255.0f / 8000.0f, 255.0f);
			}
		}
		//SafeRelease( pDepthFrame );

		// Mapping (Depth to Color)
		if (SUCCEEDED(hResult)) {
			std::vector<ColorSpacePoint> colorSpacePoints(depthWidth * depthHeight);
			hResult = pCoordinateMapper->MapDepthFrameToColorSpace(depthWidth * depthHeight, reinterpret_cast<UINT16*>(depthBufferMat.data), depthWidth * depthHeight, &colorSpacePoints[0]);
			if (SUCCEEDED(hResult)) {
				coordinateMapperMat = cv::Scalar(0, 0, 0, 0);
				for (int y = 0; y < depthHeight; y++) {
					for (int x = 0; x < depthWidth; x++) {
						unsigned int index = y * depthWidth + x;
						ColorSpacePoint point = colorSpacePoints[index];
						int colorX = static_cast<int>(std::floor(point.X + 0.5));
						int colorY = static_cast<int>(std::floor(point.Y + 0.5));
						unsigned short depth = depthBufferMat.at<unsigned short>(y, x);
						if ((colorX >= 0) && (colorX < colorWidth) && (colorY >= 0) && (colorY < colorHeight)/* && ( depth >= minDepth ) && ( depth <= maxDepth )*/) {
							coordinateMapperMat.at<cv::Vec4b>(y, x) = colorBufferMat.at<cv::Vec4b>(colorY, colorX);
						}

					}
				}
			}
		}
		//反转图像

		cv::flip(coordinateMapperMat, coordinateMapperMatFlip, 1);
		cv::flip(temp, tempFlip, 1);

		string Img_Name = "D:\\git_project\\yolo\\darknetpro\\darknet\\build\\darknet\\x64\\testpic\\test_" + to_string(k) + ".jpg";
		string txt_addr = "D:\\git_project\\yolo\\darknetpro\\darknet\\build\\darknet\\x64\\picpath\\test_" + to_string(k) + ".txt";
		string buff = "testpic/test_" + to_string(k) + ".jpg";

		//保存图片到指定文件夹
		//cv::imwrite(Img_Name, coordinateMapperMat);
		//cv::imwrite(Img_Name, coordinateMapperMatFlip);
		//保存图片路径到TXT文件中，供YOLO读取。
		/*FILE *fp;
		const char*  txt_address = txt_addr.c_str();
		const char*  buff_char = buff.c_str();
		fopen_s(&fp, txt_address, "w");

		cout << "Saving image...\n";
		if (fp == NULL)
		{
		printf("open file error!");
		}
		else
		{
		fwrite(buff_char, 1, buff.size(), fp);
		cout << "Done!" << "  File " << k << "\n";
		fclose(fp);
		}*/




		std::cout << "1:  " << tempFlip.at<UINT16>(162, 226) << "\t";
		std::cout << "2:  " << tempFlip.at<UINT16>(171, 328) << "\t";
		std::cout << "3:  " << tempFlip.at<UINT16>(258, 329) << "\t";
		std::cout << "4:  " << tempFlip.at<UINT16>(245, 210) << "\t";
		std::cout << "5:  " << temp.at<UINT16>(200, 219) << "\n";
		//std::cout << "3:   " << depth3 << "\n";
		//std::cout << "4:   " << depth4 << "\n"; 
		temp.convertTo(img, CV_8UC1, 255.0 / 4500);   //再把16位转换为8位
		//cv::imwrite("D:\\Projects-2018\\Robotic_Sorting_7_13\\Kinect\\TestKinect\\DepthImage\\test_1.jpg", coordinateMapperMat);
		Sleep(2000);
		SafeRelease(pColorFrame);
		SafeRelease(pDepthFrame);

		//cv::imshow("Color", colorMat);
		//cv::imshow("Depth", depthMat);
		//cv::imshow("CoordinateMapper", coordinateMapperMat);
		//cv::imshow("Depth", img);
		//cv::imshow("Depth", colorBufferMat);
		//cv::imshow("Depth", temp);
		//cv::imshow("Depthflip", tempFlip);
		//cv::imshow("CoordinateMapper", coordinateMapperMatFlip);
		/*if (cv::waitKey(30) == VK_ESCAPE) {
			break;
		}*/
		k++;
	}

	SafeRelease(pColorSource);
	SafeRelease(pDepthSource);
	SafeRelease(pColorReader);
	SafeRelease(pDepthReader);
	SafeRelease(pColorDescription);
	SafeRelease(pDepthDescription);
	SafeRelease(pCoordinateMapper);
	if (pSensor) {
		pSensor->Close();
	}
	SafeRelease(pSensor);
	cv::destroyAllWindows();

	return 0;
}