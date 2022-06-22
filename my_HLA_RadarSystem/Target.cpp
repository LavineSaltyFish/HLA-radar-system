#include "Target.h"
#include "stdafx.h"
//#include <stdlib.h>
//#include <fedtime.hh>

#ifndef _MSC_VER
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#else
#include <winsock2.h>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;
#endif


#include <stdio.h>
#include <string>

#include <fedtime.hh>
#include <RTI.hh>
#include "engine.h"  

extern RTI::RTIambassador	rtiAmb;
extern Target*	myTarget;
extern RTIfedTime Lookahead;
extern RTIfedTime CurrentTime;
extern RTI::Boolean	Regulating;

RTI::RTIambassador*	Target::ms_rtiAmb = NULL;
RTI::Boolean		Target::ms_doRegistry = RTI::RTI_FALSE;
RTI::ObjectClassHandle	Target::m_TargetHandle = 0;
RTI::AttributeHandle	Target::m_PositionHandle = 0;
RTI::AttributeHandle	Target::m_VelocityHandle = 0;
RTI::AttributeHandle	Target::m_AccelerationHandle = 0;
RTI::AttributeHandle	Target::m_PrivilegeToDeleteHandle = 0;

char* const		Target::ms_TargetTypeStr = "Target";
char* const		Target::ms_PositionTypeStr = "Position";
char* const		Target::ms_VelocityTypeStr = "Velocity";
char* const		Target::ms_AccelerationTypeStr = "Acceleartion";
char* const		Target::ms_PrivilegeToDeleteTypeStr = "PrivilegeToDelete";
Target*		Target::ms_TargetExtent= NULL ;
//int		Target::ms_extentCardinality = 0;

//////////////////////////////////////////////////////////////////////
//                    Target类初始化
//////////////////////////////////////////////////////////////////////

Target::Target()
	:ms_name("Aviat"),
	m_Position(),
	m_Velocity(),
	m_Acceleration(),
	ms_OwnPosition(RTI::RTI_TRUE),
	ms_OwnVelocity(RTI::RTI_TRUE),
	ms_OwnAcceleration(RTI::RTI_TRUE),
	ms_OwnPrivilegeToDelete(RTI::RTI_TRUE),
	ms_TransferPosition(RTI::RTI_FALSE),
	ms_TransferVelocity(RTI::RTI_FALSE),
	ms_TransferAcceleration(RTI::RTI_FALSE),
	ms_TransferPrivilegeToDelete(RTI::RTI_FALSE),
	ms_PositionUpdate(RTI::RTI_FALSE),
	ms_VelocityUpdate(RTI::RTI_FALSE),
	ms_AccelerationUpdate(RTI::RTI_FALSE),
	ms_InstanceDeleted(RTI::RTI_FALSE)
{

	cout << endl << "---------------------------------------" << endl;
	cout<< "***Target() OK" << endl << endl;
	Target::ms_TargetExtent = this;
	
}

Target::Target(RTI::ObjectHandle id)
	:ms_name("Aviat"),
	m_InstanceId(id),
	m_Position(),
	m_Velocity(),
	m_Acceleration(),
	ms_OwnPosition(RTI::RTI_FALSE),
	ms_OwnVelocity(RTI::RTI_FALSE),
	ms_OwnAcceleration(RTI::RTI_FALSE),
	ms_OwnPrivilegeToDelete(RTI::RTI_FALSE),
	ms_TransferPosition(RTI::RTI_FALSE),
	ms_TransferVelocity(RTI::RTI_FALSE),
	ms_TransferAcceleration(RTI::RTI_FALSE),
	ms_TransferPrivilegeToDelete(RTI::RTI_FALSE),
	ms_PositionUpdate(RTI::RTI_FALSE),
	ms_VelocityUpdate(RTI::RTI_FALSE),
	ms_AccelerationUpdate(RTI::RTI_FALSE),
	ms_InstanceDeleted(RTI::RTI_FALSE)
{
	cout << "***Target(id) OK" << endl << endl;

	Target::ms_TargetExtent = this;

}

Target::~Target()
{
	cout << "***~Target() OK" << endl << endl;


	Target *pTarget = Target::ms_TargetExtent;
	
	if (pTarget)
	{
		
		if (ms_rtiAmb)
		{
			ms_rtiAmb->queryLookahead(Lookahead);
			(void)ms_rtiAmb->deleteObjectInstance(this->GetInstanceId(), CurrentTime + Lookahead, "Target Quits");
		}
		else
		{
		}
	}
	delete[] ms_name;
}


void Target::Init(RTI::RTIambassador* rtiAmb)
{
	cout << endl << "---------------------------------------" << endl;
	cout << "***Init() OK" << endl << endl;
	ms_rtiAmb = rtiAmb;

	if (ms_rtiAmb)
	{
		try
		{
			m_TargetHandle = ms_rtiAmb->getObjectClassHandle(ms_TargetTypeStr);
			cout << "class attribute handles acquired" << endl;
			cout << "TargetHandle:   " << m_TargetHandle << endl;
			m_PositionHandle = ms_rtiAmb->getAttributeHandle(ms_PositionTypeStr, m_TargetHandle);
			cout << "PositionHandle:   " << m_PositionHandle << endl;
			m_VelocityHandle = ms_rtiAmb->getAttributeHandle(ms_VelocityTypeStr, m_TargetHandle);
			cout << "VelocityHandle:   " << m_VelocityHandle << endl;
			/*m_AccelerationHandle = ms_rtiAmb->getAttributeHandle(ms_AccelerationTypeStr, m_TargetHandle);
			cout << "***AccelerationHandle:" << m_AccelerationHandle << endl;*/
			m_PrivilegeToDeleteHandle = ms_rtiAmb->getAttributeHandle(ms_PrivilegeToDeleteTypeStr, m_TargetHandle);
			cout << "PrivilegeToDeleteHandle:   " << m_PrivilegeToDeleteHandle << endl << endl;

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


void Target::Publishing()
	{
		cout << "***Target::Publishing() OK" << endl << endl;

		if (ms_rtiAmb)
		{
			RTI::AttributeHandleSet* TargetAttributes;

			try
			{
				TargetAttributes = RTI::AttributeHandleSetFactory::create(3);
				TargetAttributes->add(m_PositionHandle);
				TargetAttributes->add(m_VelocityHandle);
				TargetAttributes->add(m_PrivilegeToDeleteHandle);
			}
			catch (RTI::Exception& e)
			{
				cout << e << endl;
				exit(1);
			}
			try
			{
				ms_rtiAmb->publishObjectClass(m_TargetHandle, *TargetAttributes);				
				cout << "objectclass Target published " << endl << endl;
			}
			catch (RTI::Exception& e)
			{
				cout << e << endl;
				exit(1);
			}
			if (TargetAttributes)
			{
				TargetAttributes->empty();
				delete TargetAttributes;
			}
		}
	}

void Target::Subscribing()
	{
		cout << "***Target::Subscribing() OK" << endl << endl;

		if (ms_rtiAmb)
		{
			RTI::AttributeHandleSet* TargetAttributes;

			try
			{
				TargetAttributes = RTI::AttributeHandleSetFactory::create(3);
				TargetAttributes->add(m_PositionHandle);
				TargetAttributes->add(m_VelocityHandle);
				TargetAttributes->add(m_PrivilegeToDeleteHandle);
			}
			catch (RTI::Exception& e)
			{
				cout << e << endl;
				exit(1);
			}
			try
			{
				ms_rtiAmb->subscribeObjectClassAttributes(m_TargetHandle, *TargetAttributes);
				cout << "*** Subscribe to objectclass target" << endl << endl;
			}
			catch (RTI::Exception& e)
			{
				cout << e << endl;
				exit(1);
			}
			if (TargetAttributes)
			{
				TargetAttributes->empty();
				delete TargetAttributes;
			}
		}
	}

//////////////////////////////////////////////////////////////////////
//                     Object Management
//////////////////////////////////////////////////////////////////////

void Target::Register()
{
	cout << "***Register() OK" << endl << endl;
	if (ms_rtiAmb)
	{
		if (!ms_doRegistry)
		{
			cout << "because no one subscribe to target class,so tick time!" << endl;
		}
		while (!ms_doRegistry)
		{
			ms_rtiAmb->tick(0.01, 2.0);
		}
			

		try
		{
			(this->ms_TargetExtent)->m_InstanceId =
						ms_rtiAmb->registerObjectInstance(
							(this->ms_TargetExtent)->GetTargetRtiId(),
							(this->ms_TargetExtent)->GetName());

					cout << "registered Target ID:"
						<< (this->ms_TargetExtent)->GetInstanceId()
						<< "      Name:" << (this->ms_TargetExtent)->GetName() << endl<<endl;
					ms_rtiAmb->tick(0.01, 2.0);
				}
				catch (RTI::Exception& e)
				{
					cout << e << endl;
					exit(1);
				}
			
		}
	}

void Target::DeleteInstance(Target* myTarget)
{
	cout << "***DeleteInstance() OK" << endl << endl;

	Target* pTarget = myTarget->ms_TargetExtent;

	if (pTarget->GetInstanceDeleted() && pTarget->GetOwnPrivilegeToDelete())
		{
			try
			{
				cout << endl;
				cout << "*****************************" << endl;
				cout << "*** delete object:" << pTarget->GetName() << endl;
				cout << "*****************************" << endl;
				if (Regulating)
				{
					ms_rtiAmb->queryLookahead(Lookahead);
					ms_rtiAmb->deleteObjectInstance(pTarget->GetInstanceId(), CurrentTime + Lookahead, "Target Quits");
					ms_rtiAmb->tick(0.01, 2.0);
				}
				else
				{
					ms_rtiAmb->deleteObjectInstance(pTarget->GetInstanceId(), "Target Quits");
					ms_rtiAmb->tick(0.01, 2.0);
				}
			}
			catch (RTI::Exception& e)
			{
				cout << e << endl;
				exit(1);
			}
			pTarget->SetOwnPrivilegeToDelete(RTI::RTI_FALSE);
		}
}

void Target::TargetUpdate(Target* pTarget)
{
	cout << "****TargetUpdate() OK" << endl;
	
	RTI::Double* Position;
	RTI::Double* Velocity;
	RTI::Double* Acceleration;

	RTI::AttributeHandleValuePairSet* attrs = NULL;

	try
	{
		attrs = RTI::AttributeSetFactory::create(3);
		if (pTarget->GetPositionUpdate())
		{
			Position = pTarget->GetPosition();
			RTI::Double PosTemp[3]= { *Position,*(Position + 1),*(Position + 2) };
			attrs->add(pTarget->GetPositionRtiId(), (char*)PosTemp, sizeof(PosTemp));
			
			pTarget->SetPositionUpdate(RTI::RTI_FALSE);
		}
		if (pTarget->GetPositionUpdate())
		{
			Velocity = pTarget->GetVelocity();
			RTI::Double VelTemp[3] = { *Velocity,*(Velocity + 1),*(Velocity + 2) };
			attrs->add(pTarget->GetPositionRtiId(), (char*)VelTemp, sizeof(VelTemp));

			pTarget->SetVelocityUpdate(RTI::RTI_FALSE);
		}
		if (pTarget->GetAccelerationUpdate())
		{
			//	Acceleration = pTarget->GetAcceleration();
			//	attrs->add(pTarget->GetAccelerationRtiId(), (char*)&Cleanliness, sizeof(Cleanliness));
				pTarget->SetAccelerationUpdate(RTI::RTI_FALSE);
		}
			if (attrs->size())
			{

				if (Regulating)
				{
					rtiAmb.queryLookahead(Lookahead);
					rtiAmb.updateAttributeValues(pTarget->GetInstanceId(),
						*attrs,
						CurrentTime + Lookahead,
						"TargetUpdate");
					cout << "Update " << pTarget->GetName() << endl;
					rtiAmb.tick(0.1, 2.0);
				}
				else
				{
					rtiAmb.updateAttributeValues(pTarget->GetInstanceId(),
						*attrs,
						"TargetUpdate");
					cout << "Update " << pTarget->GetName() << endl;
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

Target* Target::Find(RTI::ObjectHandle objectId)
{
	cout << "****Find()" << endl;
	Target* pTarget = Target::ms_TargetExtent;

	if (pTarget && pTarget->GetInstanceId() == objectId)
	{
	}
	else
	{
		pTarget = NULL;
	}
	return pTarget;
}

void Target::SetUpdateControl(RTI::Boolean status, const RTI::AttributeHandleSet& theAttrHandles)
{
	cout << "****SetUpdateControl()" << endl;

	RTI::AttributeHandle attrHandle;

	for (unsigned int i = 0; i < theAttrHandles.size(); i++)
	{
		attrHandle = theAttrHandles.getHandle(i);
		if (attrHandle == Target::GetPositionRtiId())
		{
			ms_PositionUpdate = status;

			const char *pStr = ms_PositionUpdate ? "ON" : "OFF";
			cout << " FED: Turning Target. Position Updates "
				<< pStr << " for object " << this->GetInstanceId()
				<< ". " << endl;
		}
		else if (attrHandle == Target::GetVelocityRtiId())
		{
			ms_VelocityUpdate = status;

			const char *pStr = ms_VelocityUpdate ? "ON" : "OFF";
			cout << "FED: Turning Target. Velocity Updates "
				<< pStr << " for object " << this->GetInstanceId()
				<< ". " << endl;

		}
		/*else if (attrHandle == Target::GetAccelerationRtiId())
		{
			ms_AccelerationUpdate = status;

			const char *pStr = ms_AccelerationUpdate ? "ON" : "OFF";
			cout << "FED_HW: Turning Student. Cleanliness Updates "
				<< pStr << " for object " << this->GetInstanceId()
				<< ". " << endl;

		}*/
	}
}
//////////////////////////////////////////////////////////////////////
//                     Target Behavior Implementation
//////////////////////////////////////////////////////////////////////
void Target::SetName(const char* name)
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

void Target::SetPosition(double *Position)
{
	cout << "****SetPosition() OK" << endl;
	ms_PositionUpdate = RTI::RTI_TRUE;

	m_Position[0] = Position[0];
	m_Position[1] = Position[1];
	m_Position[2] = Position[2];

}
void Target::SetVelocity(double *Velocity)
{
	cout << "****SetVelocity() OK" << endl;
	ms_VelocityUpdate = RTI::RTI_TRUE;

	m_Velocity[0] = *Velocity;
	m_Velocity[1] = *(Velocity + 1);
	m_Velocity[2] = *(Velocity + 2);
}
void Target::SetAcceleration(double *Acceleration)
{
	cout << "****SetAcceleration() OK" << endl;
	ms_AccelerationUpdate = RTI::RTI_TRUE;

	m_Acceleration[0] = *Acceleration;
	m_Acceleration[1] = *(Acceleration + 1);
	m_Acceleration[2] = *(Acceleration + 2);
}


//////////////////////////////////////////////////////////////////////
//                     ostream
//////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------
//
// METHOD:
//     ostream &operator << ( ostream &s, Student &v )
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
ostream& operator << (ostream &s, Target &v)
{
	cout << "****ostream& operator" << endl;

	const char* name = v.GetName();
	if (name == 0)
		name = "(unknown)";
	RTI::Double* position = v.GetPosition();
	RTI::Double* velocity = v.GetVelocity();
	RTI::Double* acceleration = v.GetAcceleration();
	s << "Name: " << name
		<< " Position: " << position[0] << position[1] << position[2]
		<< " Velocity: " << velocity[0] << velocity[1] << velocity[2]
		<< " Acceleartion: " << acceleration[0] << acceleration[1] << acceleration[2];
	return s;
}
		
//-----------------------------------------------------------------
//
// METHOD:
//     ostream &operator << ( ostream &s, Student *v )
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
ostream &operator << (ostream &s, Target *v)
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
void Target::SetTargetTakeOff(Target* myTarget,Engine* ep)
{
	cout << "****SetTarget()" << endl;
	
	if (!(ep = engOpen(NULL))) //测试是否启动Matlab引擎成功。
	{
		cout << "SetTargetTakeOff::Can't start Matlab engine!" << endl;
		return;
	}

	double tgtPosition[3];
	double tgtv[3] = {0,0,0};
	double tgta[3] = {0,0,0};
	int mode;
	double RCS;
	if ((myTarget)&&(ep))
	{
		cout << "****Please type in the motion mode of Target:" << myTarget->GetName() << endl;
		cout << "        1 for static" << endl << "        2 for uniform linear motion" << endl << "        3 for uniformly accelerated linear motion"<<endl;
		/*bool flag = false;
		while (!flag)
		{*/
			cin >> mode;
		/*	if (mode >= 4 || mode <= 0)
			{
				cout << "****Please type in an integer from {1,2,3} " << endl;
			}
			else
			flag = true;
		}
		flag = false;*/
		cout << "****Please type in the RCS(in square meter) of Target:" << myTarget->GetName() << endl;
		cin >> RCS;


		cout << "****Please type in the initial position(AZ° EL° R) of Target:" << myTarget->GetName() << endl;
		for (int i = 0; i < 3; i++)
		{
			cin >> tgtPosition[i];
		}

		if (mode == 2)

		{
			cout << "****Please type in the initial velocity(m/s) of Target:" << myTarget->GetName() << endl;
			for (int i = 0; i < 3; i++)
			{
				cin >> tgtv[i];
			}
		}

		else if (mode == 3)
			{
				cout << "****Please type in the initial velocity(m/s) of Target:" << myTarget->GetName() << endl;
				for (int i = 0; i < 3; i++)
				{
					cin >> tgtv[i];
				}
				cout << "****Please type in the acceleration of Target:" << myTarget->GetName() << endl;
				for (int i = 0; i < 3; i++)
				{
					cin >> tgta[i];
				}
			}

		
	}

	setMATLABTarget(mode, RCS, tgtPosition, tgtv, tgta, ep);


	myTarget->SetPosition(tgtPosition);
	myTarget->SetVelocity(tgtv);
	myTarget->SetAcceleration(tgta);
	cout << endl<<endl;
}

void Target::MATLABTargetMove(Target* myTarget, Engine* ep)
{
	Target* pTarget = myTarget;

	engEvalString(ep, "[tgtpos, tgtvel] = step(env.TargetMotion,3.33e-4);");
	mxArray *tgtpos = engGetVariable(ep, "tgtpos");
	mxArray *tgtvel = engGetVariable(ep, "tgtvel");
	double *currentpos = mxGetPr(tgtpos);

	cout <<endl<< "Target has moved" << endl;
	cout << "current position:  "<<*currentpos <<"    "<< *(currentpos+1) << "    " << *(currentpos+2)<< endl;

	double *currentvel = mxGetPr(tgtvel);
	cout <<"current velocity:  "<< *currentvel << "    " << *(currentvel + 1) << "    " << *(currentvel + 2) << endl;

		pTarget->SetPosition(currentpos);
		pTarget->SetVelocity(currentvel);

		pTarget->ms_PositionUpdate = RTI::RTI_TRUE;
		pTarget->ms_VelocityUpdate = RTI::RTI_TRUE;


}

int	Target::setMATLABTarget(const int&  mode, const double& rcs, const double* tgt_Position, const double* tgt_v, const double* tgt_a, Engine *ep)

{
	if (!(ep = engOpen(NULL))) //测试是否启动Matlab引擎成功。
	{
		cout << "setTarget::Can't start Matlab engine!" << endl;
		return 1;
	}

	double Mode[1], RCS[1];
	Mode[0] = mode;
	RCS[0] = rcs;

	double tgt_Az[1], tgt_el[1], tgt_r[1];
	tgt_Az[0] = tgt_Position[0];
	tgt_el[0] = tgt_Position[1];
	tgt_r[0] = tgt_Position[2];

	double tgt_vx[1], tgt_vy[1], tgt_vz[1];
	tgt_vx[0] = tgt_v[0];
	tgt_vy[0] = tgt_v[1];
	tgt_vz[0] = tgt_v[2];

	double tgt_ax[1], tgt_ay[1], tgt_az[1];
	tgt_ax[0] = tgt_a[0];
	tgt_ay[0] = tgt_a[1];
	tgt_az[0] = tgt_a[2];

	mxArray *modeP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *rcsP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *tgt_AzP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *tgt_elP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *tgt_rP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *tgt_vxP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *tgt_vyP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *tgt_vzP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *tgt_axP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *tgt_ayP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *tgt_azP = mxCreateDoubleMatrix(1, 1, mxREAL);

	memcpy(mxGetPr(modeP), Mode, sizeof(double));
	memcpy(mxGetPr(rcsP), RCS, sizeof(double));
	memcpy(mxGetPr(tgt_AzP), tgt_Az, sizeof(double));
	memcpy(mxGetPr(tgt_elP), tgt_el, sizeof(double));
	memcpy(mxGetPr(tgt_rP), tgt_r, sizeof(double));
	memcpy(mxGetPr(tgt_vxP), tgt_vx, sizeof(double));
	memcpy(mxGetPr(tgt_vyP), tgt_vy, sizeof(double));
	memcpy(mxGetPr(tgt_vzP), tgt_vz, sizeof(double));
	memcpy(mxGetPr(tgt_axP), tgt_ax, sizeof(double));
	memcpy(mxGetPr(tgt_ayP), tgt_ay, sizeof(double));
	memcpy(mxGetPr(tgt_azP), tgt_az, sizeof(double));

	engPutVariable(ep, "mode", modeP);
	engPutVariable(ep, "rcs", rcsP);
	engPutVariable(ep, "tgt_Az", tgt_AzP);
	engPutVariable(ep, "tgt_el", tgt_elP);
	engPutVariable(ep, "tgt_r", tgt_rP);
	engPutVariable(ep, "vx", tgt_vxP);
	engPutVariable(ep, "vy", tgt_vyP);
	engPutVariable(ep, "vz", tgt_vzP);
	engPutVariable(ep, "tgt_ax", tgt_axP);
	engPutVariable(ep, "tgt_ay", tgt_ayP);
	engPutVariable(ep, "tgt_az", tgt_azP);

	engEvalString(ep, " env  = setTarget( fs,fc,mode,rcs,tgt_Az,tgt_el,tgt_r,vx,vy,vz,tgt_ax,tgt_ay,tgt_az );");

	mxDestroyArray(modeP);
	mxDestroyArray(rcsP);
	mxDestroyArray(tgt_AzP);
	mxDestroyArray(tgt_elP);
	mxDestroyArray(tgt_rP);
	mxDestroyArray(tgt_vxP);
	mxDestroyArray(tgt_vyP);
	mxDestroyArray(tgt_vzP);
	mxDestroyArray(tgt_axP);
	mxDestroyArray(tgt_ayP);
	mxDestroyArray(tgt_azP);
	
	cout << "MATLAB Target set OK" << endl;


	return 0;

}
	