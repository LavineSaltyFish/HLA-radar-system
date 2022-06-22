#include "Target.h"
#include "Radar.h"

#include "RadarSysFederateAmbassador.h"
#include <RTI.hh>

#  include <iostream>
# include <windows.h>
using std::cout;
using std::cerr;
using std::endl;
#if !defined(WIN32) && !defined(VXWORKS) && !defined(__KCC) && (__SUNPRO_CC_COMPAT != 5)
//#  include <stream.h>
#endif

#include <string.h>
#include <stdlib.h>

#if !defined(WIN32) && !defined(VXWORKS)
//#  include <unistd.h>
#endif

#if defined(WIN32)
#  include <windows.h>   // for "Sleep"
#  include <sys/timeb.h> // for "struct _timeb"

#  include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#elif defined(VXWORKS)
#  include <ace/OS.h>
#  include <iostream>
#  include <timers.h>
using std::cout;
using std::flush;
using std::endl;
using std::cerr;
#else
//#  include <sys/time.h>
#endif

#include <fedtime.hh>
char federateName[12]= "<MODULE_01>";
RTIfedTime Lookahead(0.0002);
RTIfedTime CurrentTime(0);

RTI::Boolean Regulating = RTI::RTI_FALSE;
RTI::Boolean Constrained = RTI::RTI_FALSE;
RTI::Boolean TimeRegulated = RTI::RTI_FALSE;
RTI::Boolean TimeConstrained = RTI::RTI_FALSE;

//Time Advance Grant flag
RTI::Boolean timeAdvGrant = RTI::RTI_FALSE;
RTI::Boolean TimeAdvanceOutstanding = RTI::RTI_FALSE;



// import necessary lib
#pragma comment( lib, "libeng.lib")
#pragma comment( lib, "libmx.lib")
#pragma comment( lib, "libmat.lib")

RTI::RTIambassador rtiAmb;
RadarSysFederateAmbassador fedAmb;

using namespace std;


int RSmain1()
{

	/*初始化成员数据*/
	char* const 	federationName = "RadarSystem";

	Target* myTarget = NULL;
	Target* pTarget = NULL;

	Radar* myRadar = NULL;

	 Regulating = RTI::RTI_TRUE;
	 Constrained = RTI::RTI_TRUE;

	 Engine*ep=engOpen(NULL);
	 engEvalString(ep, "addpath('F:\my_HLA_RadarSystem');");
	 engEvalString(ep, "fc = 1e10;");
	 engEvalString(ep, "fs = 6e6;");

	try
	{

		RTI::FederateHandle federateId;

		////////////////////////////////////////////////////////////////////////////////////
		//                                联邦管理
		//                          创建联邦执行+加入联邦执行
		///////////////////////////////////////////////////////////////////////////////////
		RTI::Boolean Joined = RTI::RTI_FALSE;
		int          numTries = 0;

		while (!Joined && (numTries++ < 20))
		{
			/*创建联邦执行*/
			try
			{
				cout << "***Creating Federation Execution" << endl;
				rtiAmb.createFederationExecution(federationName, "RadarSystem.fed");
				cout << "***Successful Create Federation Execution" << endl;
			}
			catch (RTI::FederationExecutionAlreadyExists& e)
			{
				cerr << "***Note: Federation execution already exists." <<
					e << endl;
			}
			catch (RTI::Exception& e)
			{
				cerr << "***ERROR:" << e << endl;
				return -1;
			}

			/*加入联邦执行*/
			try
			{
				cout << "***JOINING FEDERATION EXECUTION: " << endl;
				cout<< federateName << endl;
				federateId = rtiAmb.joinFederationExecution(federateName,
					federationName,
					&fedAmb);
				Joined = RTI::RTI_TRUE;
				
			}
			catch (RTI::FederateAlreadyExecutionMember& e)
			{
				cerr << "***ERROR: " << federateName
					<< " already exists in the Federation Execution "
					<< federationName << "." << endl;
				cerr << e << endl;
				return -1;
			}
			catch (RTI::FederationExecutionDoesNotExist&)
			{
				cerr << "***ERROR: " << federationName << " Federation Execution "
					<< "does not exists." << endl;
			}
			catch (RTI::Exception& e)
			{
				cerr << "***ERROR:" << e << endl;
				return -1;
			}
		}
		cout << "***JOINED SUCCESSFULLY: " << federationName
			<< ": Federate Handle = " << federateId << endl;
		///////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////

		/*打开恰当性建议开关*/
		try
		{
			rtiAmb.enableClassRelevanceAdvisorySwitch();
		}
		catch (RTI::Exception& e)
		{
			cout << "Error: " << " 开启联邦成员Class是否存在异地定购恰当性建议失败：" << &e << endl;
		}
		try
		{
			rtiAmb.enableInteractionRelevanceAdvisorySwitch();
		}
		catch (RTI::Exception& e)
		{
			cout << "Error: " << " 开启联邦成员Interaction是否存在异地定购恰当性建议失败：" << &e << endl;
		}
		///////////////////////////////////////////////////////////////////////////////////
		//                       创建实例
		///////////////////////////////////////////////////////////////////////////////////

		/*创建交互类实例*/

		///*创建对象类实例*/
		myTarget = new Target();
		myTarget->SetTargetTakeOff(myTarget, ep);


		myRadar = new Radar();
		myRadar = new Radar();


		for (int i = 1; i < 3; i++)
		{
			myRadar->setMATLABRadar(myRadar, ep, i);

		}

		/*实例初始化*/		
		myRadar->Init(&rtiAmb);
		myTarget->Init(&rtiAmb);


		/*公布订购对象类/交互类*/
//		myTarget->Publishing();
//		myTarget->Subscribing();

		myRadar->Publishing();
		myRadar->Subscribing();

		myRadar->generateBeamPosition(ep);
		mxArray *beamPosition = engGetVariable(ep, "beamPosition");
		int Bnum = mxGetN(beamPosition);
		cout << endl << "---------------------------------------" << endl;
		cout <<endl<<"BeamNum: "<< Bnum << endl;

		engEvalString(ep, " DetectionResult1=[];");
		engEvalString(ep, " DetectionResult2=[];");
		engEvalString(ep, "currentjob1 = jobq1.SearchQueue(53);");
		engEvalString(ep, "jobq1.SearchIndex = 53;");
		engEvalString(ep, "currentjob2 = jobq2.SearchQueue(1);");
		cout << "MATLAB jobqueue ok" << endl;
		cout << endl << "---------------------------------------" << endl;

		///////////////////////////////////////////////////////////////////////////////////
		//                        时间推进策略
		///////////////////////////////////////////////////////////////////////////////////

		/*确定时间推进策略*/
		if (!Regulating && !Constrained)
		{
			cout << "***time Regulating turned off" << endl;
			cout << "***time Constrained turned off" << endl;
			cout << "*** currenttime:" << CurrentTime.getTime() << endl;
		}
		else if (Regulating && !Constrained)
		{
			try
			{
				TimeRegulated = RTI::RTI_FALSE;
				cout << "*** time regulating turned on" << endl;
				rtiAmb.enableTimeRegulation(CurrentTime, Lookahead);//将CurrentTime设成FederateTime
				while (!TimeRegulated)
				{
					rtiAmb.tick(0.01, 1.0);
				}
				cout << "***time constraint turned off" << endl;
			}
			catch (RTI::Exception& e)
			{
				cout << e << endl;
				exit(1);
			}
		}
		else if (!Regulating && Constrained)
		{
			try
			{
				cout << "***time regulating turned off" << endl;
				TimeConstrained = RTI::RTI_FALSE;
				cout << "***time Constrained turned on" << endl;
				rtiAmb.enableTimeConstrained();//CurrentTime需等于LBTS，设成0
				while (!TimeConstrained)
				{
					rtiAmb.tick(0.01, 1.0);
				}
			}
			catch (RTI::Exception& e)
			{
				cout << e << endl;
				exit(1);
			}
		}
		else
		{
			try
			{
				TimeRegulated = RTI::RTI_FALSE;
				cout <<endl<<"time regulating turned on" << endl<<endl;
				rtiAmb.enableTimeRegulation(CurrentTime, Lookahead);
				while (!TimeRegulated)
				{
					rtiAmb.tick(0.01, 1.0);
				}
				TimeConstrained = RTI::RTI_FALSE;
				cout <<endl<< "time Constrained turned on" << endl<<endl;
				rtiAmb.enableTimeConstrained();
				while (!TimeConstrained)
				{
					rtiAmb.tick(0.01, 1.0);
				}
			}
			catch (RTI::Exception& e)
			{
				cout << e << endl;
				exit(1);
			}
		}
		////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////
		
		myRadar->Register();

		/*等待联邦成员加入*/
		cout << "等联邦成员都已加入" << endl;
		while (Radar::ms_extentCardinality < 4)
		{
			rtiAmb.tick(0.1, 1.0);
		}
		cout << "所有联邦成员都已加入" << endl << endl;
		for (int k = 0; k< Radar::ms_extentCardinality; k++)
		{
			cout << "ID: " << (myRadar->ms_RadarExtent[k])->GetInstanceId() << "   "
				<< "Name: " << (myRadar->ms_RadarExtent[k])->GetName() << endl;
		}


		cout << "----------simulation loop----------" << endl;

	/*仿真循环开始*/

		for (int i = 0; i<2*Bnum; i++)
		{

			cout <<endl<< "   -------loop:" << i << "-------"<<endl;

			/*更新对象实例*/
			myTarget->MATLABTargetMove(myTarget, ep);
			cout << endl << "......................................." << endl;
			for (int n = 0; n<4; n++)
			{
				Radar* pRadar = NULL;
				pRadar = myRadar->ms_RadarExtent[n];

				pRadar->getMATLABRadarStatus(pRadar, ep, n+1);
				pRadar->getMATLABEcho(pRadar, ep, n + 1);
				pRadar->getMATLABdetection(pRadar, ep, n + 1);

				if (pRadar->GetInstanceDeleted())continue;
				
				if ((pRadar->GetBeamDirectionUpdate() && pRadar->GetOwnBeamDirection()) ||
					(pRadar->GetWorkStatusUpdate() && pRadar->GetOwnWorkStatus()) ||
					(pRadar->GetDetectionUpdate() && pRadar->GetOwnDetection()))
				{
					
					cout << endl << "... Radar Updating ..." << endl;
					pRadar->RadarUpdate(pRadar);
					cout << endl << "......................................." << endl;
				}

			}

			cout << endl << "---------------State Print of Loop: " << i << "-----------------" << endl;		
			cout << "This Federate's time: " << CurrentTime.getTime() << endl << endl;

			for (int n = 0; n < myRadar->ms_extentCardinality; n++)
			{
				Radar* pRadar = NULL;
				pRadar = myRadar->ms_RadarExtent[n];
				cout << "--------[name]: " << pRadar->GetName() << " --------" << endl;
				cout << "[StationPosition]: " << *(pRadar->GetStationPosition()) << ", " << *(pRadar->GetStationPosition() + 1) << ", "<<*(pRadar->GetStationPosition() + 2) << endl;
				cout << "[WorkStatus]: " << pRadar->GetWorkStatus() << endl;
				cout << "[Beam]: " << *(pRadar->GetBeamDirection()) << ", "<< *(pRadar->GetBeamDirection()+1)<<endl;
				cout << "[Detection]: " << *(pRadar->GetDetection()) << ", " << *(pRadar->GetDetection() + 1)<< ", " << *(pRadar->GetDetection() + 2)<<endl<<endl;
			}

			cout << endl << "---------------------------------------" << endl;
			Sleep(4000);

			/*请求时间推进*/
			if (!Regulating && !Constrained)
			{
				cout << "***this federate is neither regulating nor constrained!!!" << endl;
			}
			else
			{
				if (!TimeAdvanceOutstanding)
				{
					try
					{
						cout << "***advancing time from " << CurrentTime.getTime() << " to "
							<< CurrentTime.getTime() + 0.0003 << endl;
						TimeAdvanceOutstanding = RTI::RTI_TRUE;
						rtiAmb.timeAdvanceRequest(CurrentTime + 0.0003);
						cout << "***ticking***" << endl;
						while (TimeAdvanceOutstanding)
						{
							rtiAmb.tick(0.01, 1.0);
						}
					}
					catch (RTI::Exception& e)
					{
						cout << "time advance error:" << e << endl;
						exit(1);
					}
				}
			}		
		}



		/*联邦成员退出联邦执行*/
		try
		{
			cout << "***RESIGN FEDERATION EXECUTION CALLED" << endl;

			rtiAmb.resignFederationExecution(
				RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
			cout << "*** SUCCESSFUL RESIGN FEDERATION EXECUTION CALLED" << endl;
		}
		catch (RTI::Exception& e)
		{
			cerr << "***ERROR:" << e << endl;
			return -1;
		}

		/*撤销联邦执行*/
		try
		{
			cout << "***DESTROY FEDERATION EXECUTION CALLED" << endl;
			rtiAmb.destroyFederationExecution(federationName);
			cout << "***SUCCESSFUL DESTROY FEDERATION EXECUTION CALLED" << endl;
		}
		catch (RTI::FederatesCurrentlyJoined& /* e */)
		{
			cerr << "***FederatesCurrentlyJoined" << endl;
			return 0;
		}
		catch (RTI::FederationExecutionDoesNotExist& /* e */)
		{
			cerr << "***FederationExecutionDoesNotExist" << endl;
			return 0;
		}
		catch (RTI::Exception& e)
		{
			cerr << "***ERROR:" << e << endl;
			return -1;
		}
	}
	catch (RTI::Exception& e)
	{
		cerr << "***Unhandled exception from the RTI:" << e << endl;
		return -1;
	}
	return 0;
}

int main()
{
	return RSmain1();
}

