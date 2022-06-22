#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "engine.h"     // add header file
#include <iostream>
#include "stdafx.h"


using namespace std;

int generateBeamPosition(const double& AZ,const double& EL,const double& BEAMWIDTH,Engine *ep)
{

	if (!(ep = engOpen(NULL))) //测试是否启动Matlab引擎成功。
	{
		cout << "generateBeamPosition::Can't start Matlab engine!" << endl;
		return 1;
	}
	
	double Az[1],El[1],beamWidth[1];
	Az[0] =  AZ;
	El[0] = EL;
	beamWidth[0] = BEAMWIDTH;


	mxArray *AzP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *ElP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *beamWidthP = mxCreateDoubleMatrix(1, 1, mxREAL);
	//memcpy，从源src所指的内存地址的起始位置开始拷贝n个字节到目标dest所指的内存地址的起始位置中。数组赋值方法。
	//mxGetPr返回数组指针：第一个元素指针
	memcpy(mxGetPr(AzP), Az, sizeof(double)); //将数组Az复制到mxarray数组AzP中。
	memcpy(mxGetPr(ElP), El, sizeof(double)); 
	memcpy(mxGetPr(beamWidthP), beamWidth, sizeof(double)); 

	engPutVariable(ep, "Az", AzP); //将mxArray数组AzP写入到Matlab工作空间，命名为Az。
	engPutVariable(ep, "El", ElP); 
	engPutVariable(ep, "beamWidth", beamWidthP); 

	engEvalString(ep, "addpath(F:\my_HLA_RadarSystem);");
	engEvalString(ep, "[beamPosition] = funcTest(Az,El,beamWidth) ;");


	mxDestroyArray(AzP); //销毁mxArray数组xx和yy。
	mxDestroyArray(ElP);
	mxDestroyArray(beamWidthP); 

	engClose(ep); //关闭Matlab引擎。

	return 0;

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
