#include "Radar2.h"

#include <stdlib.h>
#include <fedtime.hh>

#ifndef _MSC_VER
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#else
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::string;
#endif

#include <stdio.h>
#include <string>

#include <RTI.hh>
#include "engine.h"  

extern RTI::RTIambassador	rtiAmb;
extern Radar2*		myRadar;
extern RTIfedTime Lookahead;
extern RTIfedTime CurrentTime;
extern RTI::Boolean	Regulating;

RTI::RTIambassador*	Radar2::ms_rtiAmb = NULL;
RTI::Boolean		Radar2::ms_doRegistry = RTI::RTI_FALSE;
RTI::ObjectClassHandle	Radar2::m_RadarHandle = 0;
RTI::AttributeHandle	Radar2::m_StationPositionHandle = 0;
RTI::AttributeHandle	Radar2::m_BeamDirectionHandle = 0;
RTI::AttributeHandle	Radar2::m_WorkStatusHandle = 0;
RTI::AttributeHandle	Radar2::m_DetectionHandle = 0;
RTI::AttributeHandle	Radar2::m_PrivilegeToDeleteHandle = 0;

char* const		Radar2::ms_RadarTypeStr = "Radar";
char* const		Radar2::ms_StationPositionTypeStr = "StationPosition";
char* const		Radar2::ms_BeamDirectionTypeStr = "BeamDirection";
char* const		Radar2::ms_WorkStatusTypeStr = "WorkStatus";
char* const		Radar2::ms_DetectionTypeStr = "Detection";
char* const		Radar2::ms_PrivilegeToDeleteTypeStr = "PrivilegeToDelete";

Radar2*	Radar2::ms_RadarExtent[4] = { NULL, NULL, NULL, NULL };
int		Radar2::ms_extentCardinality = 0;

//////////////////////////////////////////////////////////////////////
//                     Radar类初始化
//////////////////////////////////////////////////////////////////////

const double radarpos[4][3] = { 0, 0, 0,-1000,0,0,0,360,0,0,2000,0 };

Radar2::Radar2()
	:ms_name(NULL),
	m_StationPosition(),
	m_BeamDirection(),
	m_WorkStatus(),
	m_Detection(),

	ms_OwnBeamDirection(RTI::RTI_TRUE),
	ms_OwnWorkStatus(RTI::RTI_TRUE),
	ms_OwnDetection(RTI::RTI_TRUE),
	ms_OwnPrivilegeToDelete(RTI::RTI_TRUE),
	ms_TransferBeamDirection(RTI::RTI_FALSE),
	ms_TransferWorkStatus(RTI::RTI_FALSE),
	ms_TransferDetection(RTI::RTI_FALSE),
	ms_TransferPrivilegeToDelete(RTI::RTI_FALSE),
	ms_BeamDirectionUpdate(RTI::RTI_FALSE),
	ms_WorkStatusUpdate(RTI::RTI_FALSE),
	ms_DetectionUpdate(RTI::RTI_FALSE),
	ms_InstanceDeleted(RTI::RTI_FALSE)
{
	cout << "***Radar2() OK" << endl << endl;
	Radar2::ms_extentCardinality = 4;

	for (int i = 0; i < 4; i++)
	{
		Radar2::ms_RadarExtent[i] = this;

		string thisname = "radar_" + std::to_string(i);
		this->SetName(thisname.c_str());
		double pos[3];
		for (int k = 0; k < 3; k++)
		{
			pos[k] = radarpos[i][k];
		}
		this->SetStationPosition(pos);
		const char* Ready = "ready";
		double initdetec[3] = { -1,-1,-1 };
		double initbeam[2] = { -1,-1 };
		this->SetWorkStatus(Ready);
		this->SetDetection(initdetec);
		this->SetBeamDirection(initbeam);
	}


}

Radar2::Radar2(RTI::ObjectHandle id)
	:ms_name(NULL),
	m_InstanceId(id),
	m_StationPosition(),
	m_BeamDirection(),
	m_WorkStatus(),
	m_Detection(),

	ms_OwnBeamDirection(RTI::RTI_FALSE),
	ms_OwnWorkStatus(RTI::RTI_FALSE),
	ms_OwnDetection(RTI::RTI_FALSE),
	ms_OwnPrivilegeToDelete(RTI::RTI_FALSE),
	ms_TransferBeamDirection(RTI::RTI_FALSE),
	ms_TransferWorkStatus(RTI::RTI_FALSE),
	ms_TransferDetection(RTI::RTI_FALSE),
	ms_TransferPrivilegeToDelete(RTI::RTI_FALSE),
	ms_BeamDirectionUpdate(RTI::RTI_FALSE),
	ms_WorkStatusUpdate(RTI::RTI_FALSE),
	ms_DetectionUpdate(RTI::RTI_FALSE),
	ms_InstanceDeleted(RTI::RTI_FALSE)
{
	cout << "***Radar(id) OK" << endl << endl;

	for (int i = 0; i < 4; i++)
	{
		Radar2::ms_RadarExtent[i] = this;

		string thisname = "radar_" + std::to_string(i);
		this->SetName(thisname.c_str());
		double pos[3];
		for (int k = 0; k < 3; k++)
		{
			pos[k] = radarpos[i][k];
		}
		this->SetStationPosition(pos);
		const char* Ready = "ready";
		double initdetec[3] = { -1,-1,-1 };
		double initbeam[2] = { -1,-1 };
		this->SetWorkStatus(Ready);
		this->SetDetection(initdetec);
		this->SetBeamDirection(initbeam);
	}

}

Radar2::~Radar2()
{
	cout << "***~Radar() OK" << endl << endl;

	Radar2::ms_extentCardinality = 4;
	Radar2 *pRadar = NULL;
	int idx = 0;

	for (idx = 0; idx<Radar2::ms_extentCardinality; idx++)
	{
		pRadar = Radar2::ms_RadarExtent[idx];

		if (pRadar && pRadar->GetInstanceId() == this->GetInstanceId())
		{
			break;
		}
	}
	if (pRadar)
	{
		for (int i = idx; (i<Radar2::ms_extentCardinality) && (Radar2::ms_RadarExtent[i] != NULL); i++)
		{
			Radar2::ms_RadarExtent[i] = Radar2::ms_RadarExtent[i + 1];
		}
		Radar2::ms_extentCardinality = Radar2::ms_extentCardinality - 1;
		if (ms_rtiAmb && idx == 0)
		{
			ms_rtiAmb->queryLookahead(Lookahead);
			(void)ms_rtiAmb->deleteObjectInstance(this->GetInstanceId(), CurrentTime + Lookahead, "Radar Quits");
		}
		else
		{
		}
	}
	delete[] ms_name;
}


void	Radar2::Init(RTI::RTIambassador* rtiAmb)
{
	cout << "***Init() OK" << endl << endl;

	ms_rtiAmb = rtiAmb;
	if (ms_rtiAmb)
	{
		cout << "***acquired  class attribute handles" << endl;
		try
		{
			m_RadarHandle = ms_rtiAmb->getObjectClassHandle(ms_RadarTypeStr);
			cout << "RadarHandle:" << m_RadarHandle << endl;
			m_StationPositionHandle = ms_rtiAmb->getAttributeHandle(ms_StationPositionTypeStr, m_RadarHandle);
			cout << "StationPositionHandle:" << m_StationPositionHandle << endl;
			m_BeamDirectionHandle = ms_rtiAmb->getAttributeHandle(ms_BeamDirectionTypeStr, m_RadarHandle);
			cout << "BeamDirectionHandle:" << m_BeamDirectionHandle << endl;
			m_WorkStatusHandle = ms_rtiAmb->getAttributeHandle(ms_WorkStatusTypeStr, m_RadarHandle);
			cout << "WorkStatusHandle:" << m_WorkStatusHandle << endl;
			m_DetectionHandle = ms_rtiAmb->getAttributeHandle(ms_DetectionTypeStr, m_RadarHandle);
			cout << "DetectionHandle:" << m_DetectionHandle << endl;
			m_PrivilegeToDeleteHandle = ms_rtiAmb->getAttributeHandle(ms_PrivilegeToDeleteTypeStr, m_RadarHandle);
			cout << "PrivilegeToDeleteHandle:" << m_PrivilegeToDeleteHandle << endl << endl;
		}
		catch (RTI::Exception& e)
		{
			cout << e << endl;
			exit(1);
		}
	}
}
//////////////////////////////////////////////////////////////////////
//                     Declaration
//////////////////////////////////////////////////////////////////////

void	Radar2::Publishing()
{
	cout << "***Radar2::Publishing() OK" << endl << endl;

	if (ms_rtiAmb)
	{
		RTI::AttributeHandleSet* RadarAttributes;

		try
		{
			RadarAttributes = RTI::AttributeHandleSetFactory::create(5);
			RadarAttributes->add(m_StationPositionHandle);
			RadarAttributes->add(m_BeamDirectionHandle);
			RadarAttributes->add(m_WorkStatusHandle);
			RadarAttributes->add(m_DetectionHandle);
			RadarAttributes->add(m_PrivilegeToDeleteHandle);
		}
		catch (RTI::Exception& e)
		{
			cout << e << endl;
			exit(1);
		}
		try
		{

			ms_rtiAmb->publishObjectClass(m_RadarHandle, *RadarAttributes);
			cout << "*** published objectclass Radar" << endl << endl;
		}
		catch (RTI::Exception& e)
		{
			cout << e << endl;
			exit(1);
		}
		if (RadarAttributes)
		{
			RadarAttributes->empty();
			delete RadarAttributes;
		}
	}
}

void	Radar2::Subscribing()
{
	cout << "***SubscribingtoRadar() OK" << endl << endl;

	if (ms_rtiAmb)
	{
		RTI::AttributeHandleSet* RadarAttributes;

		try
		{
			RadarAttributes = RTI::AttributeHandleSetFactory::create(5);
			RadarAttributes->add(m_StationPositionHandle);
			RadarAttributes->add(m_BeamDirectionHandle);
			RadarAttributes->add(m_WorkStatusHandle);
			RadarAttributes->add(m_DetectionHandle);
			RadarAttributes->add(m_PrivilegeToDeleteHandle);

		}
		catch (RTI::Exception& e)
		{
			cout << e << endl;
			exit(1);
		}
		try
		{

			ms_rtiAmb->subscribeObjectClassAttributes(m_RadarHandle, *RadarAttributes);
			cout << "*** Subscribed to objectclass Radar" << endl << endl;
		}
		catch (RTI::Exception& e)
		{
			cout << e << endl;
			exit(1);
		}
		if (RadarAttributes)
		{
			RadarAttributes->empty();
			delete RadarAttributes;
		}
	}
}

//////////////////////////////////////////////////////////////////////
//                     Object Management
//////////////////////////////////////////////////////////////////////

void	Radar2::Register()
{
	cout << "***Register() OK" << endl << endl;

	if (ms_rtiAmb)
	{
		if (!ms_doRegistry)
		{
			cout << "*** because no one subscribes to Radar class,so tick time!" << endl;
		}
		while (!ms_doRegistry)
		{
			ms_rtiAmb->tick(0.01, 2.0);
		}
		for (int n = 0; n<4; n++)
		{
			try
			{
				printf("registering: %d", n);
				(this->ms_RadarExtent[n])->m_InstanceId =
					ms_rtiAmb->registerObjectInstance(
						(this->ms_RadarExtent[n])->GetRadarRtiId(),
						(this->ms_RadarExtent[n])->GetName());
				printf("registering: %d", n);
				cout << "*** registered Radar Instance ID:"
					<< (this->ms_RadarExtent[n])->GetInstanceId() << "    "
					<< "Name:" << (this->ms_RadarExtent[n])->GetName() << endl;
				ms_rtiAmb->tick(0.01, 2.0);
			}
			catch (RTI::Exception& e)
			{
				cout << e << endl;
				exit(1);
			}
		}
	}
}

void	Radar2::DeleteInstance(Radar2* myRadar)
{
	cout << "***DeleteInstance() OK" << endl << endl;


	Radar2* pRadar = NULL;
	for (int idx = 0; idx<myRadar->ms_extentCardinality; idx++)
	{
		pRadar = myRadar->ms_RadarExtent[idx];
		if (pRadar->GetInstanceDeleted() && pRadar->GetOwnPrivilegeToDelete())
		{
			try
			{
				cout << endl;
				cout << "*****************************" << endl;
				cout << "*** delete object:" << pRadar->GetName() << endl;
				cout << "*****************************" << endl;
				if (Regulating)
				{
					ms_rtiAmb->queryLookahead(Lookahead);
					ms_rtiAmb->deleteObjectInstance(pRadar->GetInstanceId(), CurrentTime + Lookahead, "Radar Quits");
					ms_rtiAmb->tick(0.01, 2.0);
				}
				else
				{
					ms_rtiAmb->deleteObjectInstance(pRadar->GetInstanceId(), "Radar Quits");
					ms_rtiAmb->tick(0.01, 2.0);
				}
			}
			catch (RTI::Exception& e)
			{
				cout << e << endl;
				exit(1);
			}
			pRadar->SetOwnPrivilegeToDelete(RTI::RTI_FALSE);
		}
	}
}

void	Radar2::RadarUpdate(Radar2* pRadar)
{
	cout << "****RadarUpdate() OK" << endl;

	RTI::Double* BeamDirection;
	RTI::Double* Detection;
	const char*  WorkStatus = "NULL";


	RTI::AttributeHandleValuePairSet* attrs = NULL;

	try
	{
		attrs = RTI::AttributeSetFactory::create(3);
		if (pRadar->GetBeamDirectionUpdate())
		{
			BeamDirection = pRadar->GetBeamDirection();
			RTI::Double BeamDirTemp[2] = { *BeamDirection,*(BeamDirection + 1) };
			attrs->add(pRadar->GetBeamDirectionRtiId(), (char*)BeamDirTemp, sizeof(BeamDirTemp));
			pRadar->SetBeamDirectionUpdate(RTI::RTI_FALSE);
		}
		if (pRadar->GetDetectionUpdate())
		{
			Detection = pRadar->GetDetection();
			RTI::Double DetecTemp[3] = { *Detection,*(Detection + 1),*(Detection + 2) };
			attrs->add(pRadar->GetDetectionRtiId(), (char*)DetecTemp, sizeof(DetecTemp));
			pRadar->SetDetectionUpdate(RTI::RTI_FALSE);
		}
		if (pRadar->GetWorkStatusUpdate())
		{
			WorkStatus = pRadar->GetWorkStatus();
			attrs->add(pRadar->GetWorkStatusRtiId(), (char*)WorkStatus, sizeof(WorkStatus));//workstatus要改
			pRadar->SetWorkStatusUpdate(RTI::RTI_FALSE);
		}
		if (attrs->size())
		{
			cout << "***update" << pRadar->GetName() << endl;
			if (Regulating)
			{
				rtiAmb.queryLookahead(Lookahead);
				rtiAmb.updateAttributeValues(pRadar->GetInstanceId(),
					*attrs,
					CurrentTime + Lookahead,
					"RadarUpdate");
				rtiAmb.tick(0.1, 2.0);
			}
			else
			{
				rtiAmb.updateAttributeValues(pRadar->GetInstanceId(),
					*attrs,
					"RadarUpdate");
				ms_rtiAmb->tick(0.1, 2.0);
			}
		}
		if (attrs)
		{
			attrs->empty();
			delete attrs;
		}
	}
	catch (RTI::Exception& e)
	{
		cout << e << endl;
		exit(1);
	}
}

//void	Radar::ProcessOwnership(Radar* myRadar, RTI::Boolean defector, int loop)
//{
//	cout << "****ProcessOwnership loop:" << loop << endl;
//
//	int i;
//	Radar* pRadar = NULL;
//
//	/*己方学生发生叛变，实例属性所有权“推”模式转移*/
//	if (defector && loop == 10)
//	{
//		int defectorIdx = -1;
//
//		/*查找已方第一个未删除的学生实例
//		1：该对象实例在联邦执行中仍然存在
//		2：联邦成员拥有对象实例的所有属性所有权
//		3：对象实例属性所有权处于未转移状态*/
//		for (i = 0; i<myRadar->ms_extentCardinality; i++)
//		{
//			pRadar = myRadar->ms_RadarExtent[i];
//			if (!pRadar->GetInstanceDeleted() &&
//				pRadar->GetOwnLunchMoney() &&
//				!pRadar->GetTransferLunchMoney() &&
//				pRadar->GetOwnAmmoAmount() &&
//				!pRadar->GetTransferAmmoAmount() &&
//				pRadar->GetOwnCleanliness() &&
//				!pRadar->GetTransferCleanliness() &&
//				pRadar->GetOwnPrivilegeToDelete() &&
//				!pRadar->GetTransferPrivilegeToDelete()
//				)
//			{
//				defectorIdx = i;
//				break;
//			}
//		}
//
//		/*请求转让该学生所有实例属性的所有权*/
//		if (defectorIdx>-1)
//		{
//			try
//			{
//				cout << "****************************************" << endl;
//				cout << "****initiating divestiture of" << pRadar->GetName() << "RADAR NUM:" << defectorIdx << endl;
//				cout << "****************************************" << endl;
//				RTI::AttributeHandleSet* attrs = NULL;
//				try
//				{
//					attrs = RTI::AttributeHandleSetFactory::create(4);
//					attrs->add(pRadar->GetLunchMoneyRtiId());
//					attrs->add(pRadar->GetAmmoAmountRtiId());
//					attrs->add(pRadar->GetCleanlinessRtiId());
//					attrs->add(pRadar->GetPrivilegeToDeleteRtiId());
//				}
//				catch (RTI::Exception& e)
//				{
//					cout << e << endl;
//					exit(1);
//				}
//				rtiAmb.negotiatedAttributeOwnershipDivestiture(
//					pRadar->GetInstanceId(),
//					*attrs,
//					"Divest");
//				rtiAmb.tick(0.01, 2.0);
//				attrs->empty();
//				delete attrs;
//			}
//			catch (RTI::Exception& e)
//			{
//				cout << e << endl;
//			}
//		}
//		else
//		{
//			cout << "****************************************" << endl;
//			cout << "****could not find a Radar to divest!***" << endl;
//			cout << "****************************************" << endl;
//		}
//	}
//
//	/*敌方学生发生叛变，实例属性所有权“拉”模式转移*/
//	else if (defector && loop == 30)
//	{
//		int defectorIdx = -1;
//
//		/*查找对方第一个未删除的学生实例
//		1：该对象实例在联邦执行中仍然存在
//		2：联邦成员不拥有对象实例的所有属性所有权
//		3：对象实例属性所有权处于未转移状态*/
//		for (i = 0; i<myRadar->ms_extentCardinality; i++)
//		{
//			pRadar = myRadar->ms_RadarExtent[i];
//			if (!pRadar->GetInstanceDeleted() &&
//				!pRadar->GetOwnLunchMoney() &&
//				!pRadar->GetTransferLunchMoney() &&
//				!pRadar->GetOwnAmmoAmount() &&
//				!pRadar->GetTransferAmmoAmount() &&
//				!pRadar->GetOwnCleanliness() &&
//				!pRadar->GetTransferCleanliness() &&
//				!pRadar->GetOwnPrivilegeToDelete() &&
//				!pRadar->GetTransferPrivilegeToDelete())
//			{
//				defectorIdx = i;
//				break;
//			}
//		}
//
//		/*请求获取该学生所有实例属性的所有权*/
//		if (defectorIdx>-1)
//		{
//			try
//			{
//				cout << "****************************************" << endl;
//				cout << "****initiating acquisition of" << pRadar->GetName() << "RADAR NUM:" << defectorIdx << endl;
//				cout << "****************************************" << endl;
//				RTI::AttributeHandleSet* attrs = NULL;
//				try
//				{
//					attrs = RTI::AttributeHandleSetFactory::create(4);
//					attrs->add(pRadar->GetLunchMoneyRtiId());
//					attrs->add(pRadar->GetAmmoAmountRtiId());
//					attrs->add(pRadar->GetCleanlinessRtiId());
//					attrs->add(pRadar->GetPrivilegeToDeleteRtiId());
//				}
//				catch (RTI::Exception& e)
//				{
//					cout << e << endl;
//					exit(1);
//				}
//				rtiAmb.attributeOwnershipAcquisition(
//					pRadar->GetInstanceId(),
//					*attrs,
//					"Acquire");
//				rtiAmb.tick(0.01, 2.0);
//				attrs->empty();
//				delete attrs;
//			}
//			catch (RTI::Exception& e)
//			{
//				cout << e << endl;
//			}
//		}
//		else
//		{
//			cout << "****************************************" << endl;
//			cout << "****could not find a Radar to Acquire!***" << endl;
//			cout << "****************************************" << endl;
//		}
//	}
//
//	/*属性所有权转移响应处理*/
//
//	/*“拉”模式属性所有权释放响应，在收到其他联邦成员获取对象属性
//	所有权请求后，向请求方发送所有权释放响应*/
//	for (i = 0; i<myRadar->ms_extentCardinality; i++)
//	{
//		pRadar = myRadar->ms_RadarExtent[i];
//
//		/*查找所有需要进行释放响应处理的对象实例：
//		1：该对象实例在联邦执行中仍然存在
//		2：联邦成员拥有对象实例的所有属性所有权
//		3：对象实例属性所有权处于转移状态*/
//		if (!pRadar->GetInstanceDeleted() && (
//			(pRadar->GetOwnLunchMoney() &&
//				pRadar->GetTransferLunchMoney()) ||
//			(pRadar->GetOwnAmmoAmount() &&
//				pRadar->GetTransferAmmoAmount()) ||
//			(pRadar->GetOwnCleanliness() &&
//				pRadar->GetTransferCleanliness()) ||
//			(pRadar->GetOwnPrivilegeToDelete() &&
//				pRadar->GetTransferPrivilegeToDelete())))
//		{
//			try
//			{
//				cout << "****************************************" << endl;
//				cout << "****initiating release of" << pRadar->GetName() << endl;
//				cout << "****************************************" << endl;
//				RTI::AttributeHandleSet* attrs = NULL;
//				try
//				{
//					attrs = RTI::AttributeHandleSetFactory::create(4);
//					if (pRadar->GetTransferLunchMoney())
//					{
//						attrs->add(pRadar->GetLunchMoneyRtiId());
//						pRadar->SetTransferLunchMoney(RTI::RTI_FALSE);
//						cout << "*** TransferLunchMoney=FALSE" << endl;
//					}
//					if (pRadar->GetTransferAmmoAmount())
//					{
//						attrs->add(pRadar->GetAmmoAmountRtiId());
//						pRadar->SetTransferAmmoAmount(RTI::RTI_FALSE);
//						cout << "*** TransferAmmoAmount=FALSE" << endl;
//					}
//					if (pRadar->GetTransferCleanliness())
//					{
//						attrs->add(pRadar->GetCleanlinessRtiId());
//						pRadar->SetTransferCleanliness(RTI::RTI_FALSE);
//						cout << "*** TransferCleanliness=FALSE" << endl;
//					}
//					if (pRadar->GetTransferPrivilegeToDelete())
//					{
//						attrs->add(pRadar->GetPrivilegeToDeleteRtiId());
//						pRadar->SetTransferPrivilegeToDelete(RTI::RTI_FALSE);
//						cout << "*** TransferPrivilegeToDelete=FALSE" << endl;
//					}
//
//				}
//				catch (RTI::Exception& e)
//				{
//					cout << e << endl;
//					exit(1);
//				}
//				rtiAmb.negotiatedAttributeOwnershipDivestiture(
//					pRadar->GetInstanceId(),
//					*attrs,
//					"Release");
//				rtiAmb.tick(0.01, 2.0);
//				attrs->empty();
//				delete attrs;
//			}
//			catch (RTI::Exception& e)
//			{
//				cout << e << endl;
//			}
//		}
//	}
//
//	/*“推”模式属性所有权请求响应，实例属性所有权为其他联邦成员拥有，
//	其他联邦成员发出释放请求，本联邦发送希望获取属性所有权请求*/
//	for (i = 0; i<myRadar->ms_extentCardinality; i++)
//	{
//		pRadar = myRadar->ms_RadarExtent[i];
//
//		/*查找所有需要进行获取响应处理的对象实例：
//		1：该对象实例在联邦执行中仍然存在
//		2：联邦成员未拥有对象实例的所有属性所有权
//		3：对象实例属性所有权处于转移状态*/
//		if (!pRadar->GetInstanceDeleted() &&
//			((!pRadar->GetOwnLunchMoney() &&
//				pRadar->GetTransferLunchMoney()) ||
//				(!pRadar->GetOwnAmmoAmount() &&
//					pRadar->GetTransferAmmoAmount()) ||
//				(!pRadar->GetOwnCleanliness() &&
//					pRadar->GetTransferCleanliness()) ||
//				(!pRadar->GetOwnPrivilegeToDelete() &&
//					pRadar->GetTransferPrivilegeToDelete())))
//		{
//			try
//			{
//				cout << "****************************************" << endl;
//				cout << "****attempting acquire of" << pRadar->GetName() << endl;
//				cout << "****************************************" << endl;
//				RTI::AttributeHandleSet* attrs = NULL;
//				try
//				{
//					attrs = RTI::AttributeHandleSetFactory::create(4);
//					if (pRadar->GetTransferLunchMoney())
//					{
//						attrs->add(pRadar->GetLunchMoneyRtiId());
//						pRadar->SetTransferLunchMoney(RTI::RTI_FALSE);
//						cout << "*** TransferLunchMoney=FALSE" << endl;
//					}
//					if (pRadar->GetTransferAmmoAmount())
//					{
//						attrs->add(pRadar->GetAmmoAmountRtiId());
//						pRadar->SetTransferAmmoAmount(RTI::RTI_FALSE);
//						cout << "*** TransferAmmoAmount=FALSE" << endl;
//					}
//					if (pRadar->GetTransferCleanliness())
//					{
//						attrs->add(pRadar->GetCleanlinessRtiId());
//						pRadar->SetTransferCleanliness(RTI::RTI_FALSE);
//						cout << "*** TransferCleanliness=FALSE" << endl;
//					}
//					if (pRadar->GetTransferPrivilegeToDelete())
//					{
//						attrs->add(pRadar->GetPrivilegeToDeleteRtiId());
//						pRadar->SetTransferPrivilegeToDelete(RTI::RTI_FALSE);
//						cout << "*** TransferPrivilegeToDelete=FALSE" << endl;
//					}
//
//				}
//				catch (RTI::Exception& e)
//				{
//					cout << e << endl;
//					exit(1);
//				}
//				rtiAmb.attributeOwnershipAcquisition(
//					pRadar->GetInstanceId(),
//					*attrs,
//					"defector");
//				rtiAmb.tick(0.01, 2.0);
//				attrs->empty();
//				delete attrs;
//			}
//			catch (RTI::Exception& e)
//			{
//				cout << e << endl;
//			}
//		}
//	}
//}

Radar2* Radar2::Find(RTI::ObjectHandle objectId)
{
	cout << "****Find()" << endl;

	Radar2 *pRadar = NULL;

	for (int i = 0; i < Radar2::ms_extentCardinality; i++)
	{
		pRadar = Radar2::ms_RadarExtent[i];

		if (pRadar && pRadar->GetInstanceId() == objectId)
		{
			break;
		}
		else
		{
			pRadar = NULL;
		}
	}

	return pRadar;
}

void Radar2::SetUpdateControl(RTI::Boolean status, const RTI::AttributeHandleSet& theAttrHandles)
{
	cout << "****SetUpdateControl()" << endl;

	RTI::AttributeHandle attrHandle;

	for (unsigned int i = 0; i < theAttrHandles.size(); i++)
	{
		attrHandle = theAttrHandles.getHandle(i);
		if (attrHandle == Radar2::GetBeamDirectionRtiId())
		{
			ms_BeamDirectionUpdate = status;

			const char *pStr = ms_BeamDirectionUpdate ? "ON" : "OFF";
			cout << "FED_HW: Turning Radar. BeamDirection Updates "
				<< pStr << " for object " << this->GetInstanceId()
				<< ". " << endl;
		}
		else if (attrHandle == Radar2::GetDetectionRtiId())
		{
			ms_DetectionUpdate = status;

			const char *pStr = ms_DetectionUpdate ? "ON" : "OFF";
			cout << "FED_HW: Turning Radar. Detection Updates "
				<< pStr << " for object " << this->GetInstanceId()
				<< ". " << endl;

		}
		else if (attrHandle == Radar2::GetWorkStatusRtiId())
		{
			ms_WorkStatusUpdate = status;

			const char *pStr = ms_WorkStatusUpdate ? "ON" : "OFF";
			cout << "FED_HW: Turning Radar. WorkStatus Updates "
				<< pStr << " for object " << this->GetInstanceId()
				<< ". " << endl;

		}
	}
}
//////////////////////////////////////////////////////////////////////
//                     Radar Behavior Implementation
////////////////////////////////////////////////////////////////
void Radar2::SetName(const char* name)
{
	cout << "****SetName()" << endl;

	if (ms_name != name)
	{
		// Delete the existing memory
		delete[] ms_name;

		// Allocate appropriate size string and copy data
		if (name && strlen(name) > 0)
		{
			ms_name = new char[strlen(name) + 1];
			strcpy(ms_name, name);
		}
		else
		{
			ms_name = NULL;
		}
	}

	// Set flag so that when Update( FederateTime ) is called
	// we send this new value to the RTI.
	//hasNameChanged = RTI::RTI_TRUE;
}

void Radar2::SetStationPosition(double *StationPosition)
{
	cout << "****SetStationPosition()" << endl;

	m_StationPosition[0] = *StationPosition;
	m_StationPosition[1] = *(StationPosition + 1);
	m_StationPosition[2] = *(StationPosition + 2);
}
void Radar2::SetBeamDirection(double *BeamDirection)
{
	cout << "****SetBeamDirection()" << endl;

	ms_BeamDirectionUpdate = RTI::RTI_TRUE;

	m_BeamDirection[0] = *BeamDirection;
	m_BeamDirection[1] = *(BeamDirection + 1);
}
void Radar2::SetWorkStatus(const char* WorkStatus)
{
	cout << "****SetWorkStatus()" << endl;


	if (m_WorkStatus != WorkStatus)
	{
		ms_WorkStatusUpdate = RTI::RTI_TRUE;
		// Delete the existing memory
		delete[] m_WorkStatus;

		// Allocate appropriate size string and copy data
		if (WorkStatus && strlen(WorkStatus) > 0)
		{
			m_WorkStatus = new char[strlen(WorkStatus) + 1];
			strcpy(m_WorkStatus, WorkStatus);
		}
		else
		{
			m_WorkStatus = NULL;
		}
	}


}
void Radar2::SetDetection(double *Detection)
{
	cout << "****SetDetection()" << endl;

	ms_DetectionUpdate = RTI::RTI_TRUE;

	m_Detection[0] = *Detection;
	m_Detection[1] = *(Detection + 1);
	m_Detection[2] = *(Detection + 2);
}


//-----------------------------------------------------------------
//
// METHOD:
//     ostream &operator << ( ostream &s, Radar &v )
//
// PURPOSE:
//     Overloaded stream operator for outputing objects contents in
//     a readable format.
//
// RETURN VALUES:
//     Returns the stream. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
ostream& operator << (ostream &s, Radar2 &v)
{
	cout << "****ostream& operator" << endl;

	const char* name = v.GetName();
	if (name == 0)
		name = "(unknown)";

	RTI::Double* StationPosition = v.GetStationPosition();
	RTI::Double* BeamDirection = v.GetBeamDirection();
	RTI::Double* Detection = v.GetDetection();
	const char* WorkStatus = v.GetWorkStatus();

	s << "Name: " << name
		<< " StationPosition: " << StationPosition[0] << StationPosition[1] << StationPosition[2]
		<< " WorkStatus: " << WorkStatus
		<< " BeamDirection: " << BeamDirection[0] << BeamDirection[1]
		<< " Detection: " << Detection[0] << Detection[1] << Detection[2];
	return s;


}

//-----------------------------------------------------------------
//
// METHOD:
//     ostream &operator << ( ostream &s, Radar *v )
//
// PURPOSE:
//     Overloaded stream operator for outputing objects contents in
//     a readable format.
//
// RETURN VALUES:
//     Returns the stream. 
//
// HISTORY:
//     1) Created 11/6/96
//     2) Updated to RTI 1.3 3/26/98
//
//-----------------------------------------------------------------
ostream &operator << (ostream &s, Radar2 *v)
{
	if (!v)
		return s;
	else
	{
		s << *v;
	}

	return s;
}

///////////////////////////////////////////////////////////////////////
//                                
//                         MATLAB Bahavior
//
///////////////////////////////////////////////////////////////////////

void Radar2::setMATLABRadar(Radar2* myRadar, Engine* ep, int posIdx)
{
	double posx[1] = { radarpos[posIdx][0] };
	double posy[1] = { radarpos[posIdx][1] };
	double posz[1] = { radarpos[posIdx][2] };

	mxArray *posxP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *posyP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *poszP = mxCreateDoubleMatrix(1, 1, mxREAL);

	memcpy(mxGetPr(posxP), posx, sizeof(double));
	memcpy(mxGetPr(posyP), posy, sizeof(double));
	memcpy(mxGetPr(poszP), posz, sizeof(double));

	engPutVariable(ep, "posx", posxP);
	cout << "posx put in MATLAB" << endl;
	engPutVariable(ep, "posy", posyP);
	cout << "posy put in MATLAB" << endl;
	engPutVariable(ep, "posz", poszP);
	cout << "posz put in MATLAB" << endl;

	switch (posIdx)
	{
	case 1:
	{
		engEvalString(ep, " [mfradar1,BeamWidth] = setRadar(fc,fs,posx,posy,posz);");
		break;
	}

	case 2:
	{
		engEvalString(ep, " [mfradar2,BeamWidth] = setRadar(fc,fs,posx,posy,posz);");
		break;
	}


	case 3:
	{
		engEvalString(ep, " [mfradar3,BeamWidth] = setRadar(fc,fs,posx,posy,posz);");
		break;
	}

	case 4:
	{
		engEvalString(ep, " [mfradar4,BeamWidth] = setRadar(fc,fs,posx,posy,posz);");
		break;
	}
	default: break;
	}

	mxDestroyArray(posxP);
	mxDestroyArray(posyP);
	mxDestroyArray(poszP);

	cout << "Finished Set MATLAB Radar_" << posIdx + 1 << endl;

}
void Radar2::generateBeamPosition(Engine* ep)
{
	engEvalString(ep, " [beamPosition]=getScanGrid(45,60,BeamWidth);");
	engEvalString(ep, " jobq1 = generateJobQueue(beamPosition);");
	engEvalString(ep, " jobq2 = generateJobQueue(beamPosition);");
	engEvalString(ep, " jobq3 = generateJobQueue(beamPosition);");
	engEvalString(ep, " jobq4 = generateJobQueue(beamPosition);");
	cout << "Radar2::generateBeamPosition OK" << endl;
}

void Radar2::getMATLABRadarStatus(Radar2* myRadar, Engine* ep, int Idx)
{
	switch (Idx)
	{
	case 1:
	{
		engEvalString(ep, " [currentjob1, jobq1] = jobUpdate(jobq1, detection1);");
		engEvalString(ep, " [jobtype1char,size1] = [currentjob1.JobType,size(currentjob1.JobType,2)];");
		engEvalString(ep, " BeamDirection1 = currentjob1.BeamDirection;");
		mxArray *jobtype1 = engGetVariable(ep, "jobtype1char");
		mxArray *getBeamDir1 = engGetVariable(ep, "BeamDirection1");

		char* job1 = mxArrayToString(jobtype1);
		double* BeamDirection1 = mxGetPr(getBeamDir1);

		myRadar->SetWorkStatus(job1);
		myRadar->SetBeamDirection(BeamDirection1);

		myRadar->ms_WorkStatusUpdate = RTI::RTI_TRUE;
		myRadar->ms_BeamDirectionUpdate = RTI::RTI_TRUE;

		mxDestroyArray(jobtype1);
		mxDestroyArray(getBeamDir1);
		break;
	}
	case 2:
	{
		engEvalString(ep, " [currentjob2, jobq2] = jobUpdate(jobq2, detection2);");
		engEvalString(ep, " [jobtype2char,size2] = [currentjob2.JobType,size(currentjob2.JobType,2)];");
		engEvalString(ep, " BeamDirection2 = currentjob2.BeamDirection;");
		mxArray *jobtype2 = engGetVariable(ep, "jobtype2char");
		mxArray *getBeamDir2 = engGetVariable(ep, "BeamDirection2");

		char* job2 = mxArrayToString(jobtype2);
		double* BeamDirection2 = mxGetPr(getBeamDir2);
		myRadar->SetWorkStatus(job2);
		myRadar->SetBeamDirection(BeamDirection2);

		myRadar->ms_WorkStatusUpdate = RTI::RTI_TRUE;
		myRadar->ms_BeamDirectionUpdate = RTI::RTI_TRUE;

		mxDestroyArray(jobtype2);
		mxDestroyArray(getBeamDir2);
		break;
	}
	case 3:
	{
		engEvalString(ep, " [currentjob3, jobq3] = jobUpdate(jobq3, detection3);");
		engEvalString(ep, " [jobtype3char,size3] = [currentjob3.JobType,size(currentjob3.JobType,2)];");
		engEvalString(ep, " BeamDirection3 = currentjob3.BeamDirection;");
		mxArray *jobtype3 = engGetVariable(ep, "jobtype3char");
		mxArray *getBeamDir3 = engGetVariable(ep, "BeamDirection3");

		char* job3 = mxArrayToString(jobtype3);
		double* BeamDirection3 = mxGetPr(getBeamDir3);
		myRadar->SetWorkStatus(job3);
		myRadar->SetBeamDirection(BeamDirection3);

		myRadar->ms_WorkStatusUpdate = RTI::RTI_TRUE;
		myRadar->ms_BeamDirectionUpdate = RTI::RTI_TRUE;

		mxDestroyArray(jobtype3);
		mxDestroyArray(getBeamDir3);
		break;
	}
	case 4:
	{
		engEvalString(ep, " [currentjob4, jobq4] = jobUpdate(jobq4, detection4);");
		engEvalString(ep, " [jobtype4char,size4] = [currentjob4.JobType,size(currentjob4.JobType,2)];");
		engEvalString(ep, " BeamDirection4 = currentjob4.BeamDirection;");

		mxArray *jobtype4 = engGetVariable(ep, "jobtype4char");
		mxArray *getBeamDir4 = engGetVariable(ep, "BeamDirection4");

		char* job4 = mxArrayToString(jobtype4);
		double* BeamDirection4 = mxGetPr(getBeamDir4);

		myRadar->SetWorkStatus(job4);
		myRadar->SetBeamDirection(BeamDirection4);

		myRadar->ms_WorkStatusUpdate = RTI::RTI_TRUE;
		myRadar->ms_BeamDirectionUpdate = RTI::RTI_TRUE;
		mxDestroyArray(jobtype4);
		mxDestroyArray(getBeamDir4);
		break;
	}

	}
}

void Radar2::getMATLABEcho(Radar2* myRadar, Engine* ep, int Idx)
{
	switch (Idx)
	{
	case 1:
	{
		engEvalString(ep, " [xr1] = generateEcho(mfradar1,env,currentjob1);");
		break;
	}
	case 2:
	{
		engEvalString(ep, " [xr2] = generateEcho(mfradar2,env,currentjob2);");
		break;
	}
	case 3:
	{
		engEvalString(ep, " [xr3] = generateEcho(mfradar3,env,currentjob3);");
		break;
	}
	case 4:
	{
		engEvalString(ep, " [xr4] = generateEcho(mfradar4,env,currentjob4);");
		break;
	}
	}
}

void Radar2::getMATLABdetection(Radar2* myRadar, Engine* ep, int Idx)
{
	switch (Idx)
	{
	case 1:
	{
		engEvalString(ep, " [detection1,flag1] = generateDetection(xr1,mfradar1,currentjob1);");
		mxArray *flag1 = engGetVariable(ep, "flag1");
		double *currentflag1 = mxGetPr(flag1);
		if (*currentflag1 == 1.0)
		{
			mxArray *detection1 = engGetVariable(ep, "detection1");
			double *getdetection1 = mxGetPr(detection1);
			myRadar->SetDetection(getdetection1);
			myRadar->ms_DetectionUpdate = RTI::RTI_TRUE;
			mxDestroyArray(detection1);
		}
		mxDestroyArray(flag1);
		break;
	}


	case 2:
	{
		engEvalString(ep, " [detection2,flag2] = generateDetection(xr2,mfradar2,currentjob2);");
		mxArray *flag2 = engGetVariable(ep, "flag2");
		double *currentflag2 = mxGetPr(flag2);
		if (*currentflag2 == 1.0)
		{
			mxArray *detection2 = engGetVariable(ep, "detection2");
			double *getdetection2 = mxGetPr(detection2);
			myRadar->SetDetection(getdetection2);
			myRadar->ms_DetectionUpdate = RTI::RTI_TRUE;
			mxDestroyArray(detection2);
		}
		mxDestroyArray(flag2);
		break;
	}

	case 3:
	{
		engEvalString(ep, " [detection3,flag3] = generateDetection(xr3,mfradar3,currentjob3);");
		mxArray *flag3 = engGetVariable(ep, "flag3");
		double *currentflag3 = mxGetPr(flag3);
		if (*currentflag3 == 1.0)
		{
			mxArray *detection3 = engGetVariable(ep, "detection3");
			double *getdetection3 = mxGetPr(detection3);
			myRadar->SetDetection(getdetection3);
			myRadar->ms_DetectionUpdate = RTI::RTI_TRUE;
			mxDestroyArray(detection3);
		}
		mxDestroyArray(flag3);
		break;
	}
	case 4:
	{
		engEvalString(ep, " [detection4,flag4] = generateDetection(xr4,mfradar4,currentjob4);");
		mxArray *flag4 = engGetVariable(ep, "flag4");
		double *currentflag4 = mxGetPr(flag4);
		if (*currentflag4 == 1.0)
		{
			mxArray *detection4 = engGetVariable(ep, "detection4");
			double *getdetection4 = mxGetPr(detection4);
			myRadar->SetDetection(getdetection4);
			myRadar->ms_DetectionUpdate = RTI::RTI_TRUE;
			mxDestroyArray(detection4);
		}
		mxDestroyArray(flag4);
		break;
	}
	default: break;
	}

}
