#include "Radar.h"
#include "Target.h"
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
extern Radar*		myRadar;
extern RTIfedTime Lookahead;
extern RTIfedTime CurrentTime;
extern RTI::Boolean	Regulating;

RTI::RTIambassador*	Radar::ms_rtiAmb = NULL;
RTI::Boolean		Radar::ms_doRegistry = RTI::RTI_FALSE;
RTI::ObjectClassHandle	Radar::m_RadarHandle = 0;
RTI::AttributeHandle	Radar::m_StationPositionHandle = 0;
RTI::AttributeHandle	Radar::m_BeamDirectionHandle = 0;
RTI::AttributeHandle	Radar::m_WorkStatusHandle = 0;
RTI::AttributeHandle	Radar::m_DetectionHandle = 0;
RTI::AttributeHandle	Radar::m_PrivilegeToDeleteHandle = 0;

char* const		Radar::ms_RadarTypeStr = "Radar";
char* const		Radar::ms_StationPositionTypeStr = "StationPosition";
char* const		Radar::ms_BeamDirectionTypeStr = "BeamDirection";
char* const		Radar::ms_WorkStatusTypeStr = "WorkStatus";
char* const		Radar::ms_DetectionTypeStr = "Detection";
char* const		Radar::ms_PrivilegeToDeleteTypeStr = "PrivilegeToDelete";

Radar*	Radar::ms_RadarExtent[4] ={NULL, NULL,NULL,NULL};
int		Radar::ms_extentCardinality = 0;

//////////////////////////////////////////////////////////////////////
//                     Radar¿‡≥ı ºªØ
//////////////////////////////////////////////////////////////////////

const double radarpos[4][3] = { 0,400, 0,0,2000,0,0,0,0,-1000,0,0};

Radar::Radar()
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
	cout << endl << "---------------------------------------" << endl;
	cout << "***Radar() OK" << endl << endl;

	Radar::ms_RadarExtent[Radar::ms_extentCardinality++] = this;

	string thisname = "radar_" + std::to_string(ms_extentCardinality+2);
	this->SetName(thisname.c_str());
	double pos[3];
	for (int k = 0; k < 3; k++)
	{
		pos[k] = radarpos[ms_extentCardinality-1][k];
	}
	this->SetStationPosition(pos);
	const char* Ready = "ready  ";
	double initdetec[3] = { -1,-1,-1};
	double initbeam[2] = { -1,-1 };
	this->SetWorkStatus(Ready);
	this->SetDetection(initdetec);
	this->SetBeamDirection(initbeam);
	cout << "finish construct " << this->GetName() << endl;
	}

Radar::Radar(RTI::ObjectHandle id)
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
	Radar::ms_RadarExtent[Radar::ms_extentCardinality++] = this;

	double pos[3];
	for (int k = 0; k < 3; k++)
	{
		pos[k] = radarpos[ms_extentCardinality -1][k];
	}
	this->SetStationPosition(pos);
	const char* Ready = "ready";
	double initdetec[3] = { -1,-1,-1 };
	double initbeam[2] = { -1,-1 };
	this->SetWorkStatus(Ready);
	this->SetDetection(initdetec);
	this->SetBeamDirection(initbeam);


	cout << "***Radar(id) OK" << endl << endl;
}

Radar::~Radar()
{
	cout << "***~Radar() OK" << endl << endl;


	Radar *pRadar = NULL;
	int idx = 0;

	for (idx = 0; idx<Radar::ms_extentCardinality; idx++)
	{
		pRadar = Radar::ms_RadarExtent[idx];

		if (pRadar && pRadar->GetInstanceId() == this->GetInstanceId())
		{
			break;
		}
	}
	if (pRadar)
	{
		for (int i = idx; (i<Radar::ms_extentCardinality) && (Radar::ms_RadarExtent[i] != NULL); i++)
		{
			Radar::ms_RadarExtent[i] = Radar::ms_RadarExtent[i + 1];
		}
		Radar::ms_extentCardinality = Radar::ms_extentCardinality - 1;
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

void	Radar::Init(RTI::RTIambassador* rtiAmb)
{
	cout << endl << "---------------------------------------" << endl;
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

void	Radar::Publishing()
{
	cout << endl << "---------------------------------------" << endl;
	cout << "***Radar::Publishing() OK" << endl;

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

void	Radar::Subscribing()
{
	cout << endl << "---------------------------------------" << endl;
	cout << "***SubscribingtoRadar() OK" << endl;

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

void	Radar::Register()
{
	cout << endl << "---------------------------------------" << endl;
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
		for (int n = 0; n<2; n++)
		{
			try
			{
				(this->ms_RadarExtent[n])->m_InstanceId =
					ms_rtiAmb->registerObjectInstance(
						(this->ms_RadarExtent[n])->GetRadarRtiId(),
						(this->ms_RadarExtent[n])->GetName());

				cout <<endl<< "*** registered Radar Instance ID:"
					<< (this->ms_RadarExtent[n])->GetInstanceId()<<"    "
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

void	Radar::DeleteInstance(Radar* myRadar)
{
	cout << "***DeleteInstance() OK" << endl << endl;


	Radar* pRadar = NULL;
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

void	Radar::RadarUpdate(Radar* pRadar)
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
			attrs->add(pRadar->GetWorkStatusRtiId(), (char*)WorkStatus, strlen(WorkStatus));
			pRadar->SetWorkStatusUpdate(RTI::RTI_FALSE);
		}
		if (attrs->size())
		{

			if (Regulating)
			{
				rtiAmb.queryLookahead(Lookahead);
				rtiAmb.updateAttributeValues(pRadar->GetInstanceId(),
					*attrs,
					CurrentTime + Lookahead,
					"RadarUpdate");
				rtiAmb.tick(0.1, 20.0);
				cout << "Update: " << pRadar->GetName() << endl;
			}
			else
			{
				rtiAmb.updateAttributeValues(pRadar->GetInstanceId(),
					*attrs,
					"RadarUpdate");
				ms_rtiAmb->tick(0.1, 20.0);
				cout << "Update: " << pRadar->GetName() << endl;
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

Radar* Radar::Find(RTI::ObjectHandle objectId)
{
	cout << "****Find()" << endl;

	Radar *pRadar = NULL;

	for (int i = 0; i < Radar::ms_extentCardinality; i++)
	{
		pRadar = Radar::ms_RadarExtent[i];

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

void Radar::SetUpdateControl(RTI::Boolean status, const RTI::AttributeHandleSet& theAttrHandles)
{
	cout << "****SetUpdateControl()" << endl;

	RTI::AttributeHandle attrHandle;

	for (unsigned int i = 0; i < theAttrHandles.size(); i++)
	{
		attrHandle = theAttrHandles.getHandle(i);
		if (attrHandle == Radar::GetBeamDirectionRtiId())
		{
			ms_BeamDirectionUpdate = status;

			const char *pStr = ms_BeamDirectionUpdate ? "ON" : "OFF";
			cout << "FED_HW: Turning Radar. BeamDirection Updates "
				<< pStr << " for object " << this->GetInstanceId()
				<< ". " << endl;
		}
		else if (attrHandle == Radar::GetDetectionRtiId())
		{
			ms_DetectionUpdate = status;

			const char *pStr = ms_DetectionUpdate ? "ON" : "OFF";
			cout << "FED_HW: Turning Radar. Detection Updates "
				<< pStr << " for object " << this->GetInstanceId()
				<< ". " << endl;

		}
		else if (attrHandle == Radar::GetWorkStatusRtiId())
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
void Radar::SetName(const char* name)
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

void Radar::SetStationPosition(double *StationPosition)
{
	cout << "****SetStationPosition()" << endl;

	m_StationPosition[0] = *StationPosition;
	m_StationPosition[1] = *(StationPosition + 1);
	m_StationPosition[2] = *(StationPosition + 2);
}
void Radar::SetBeamDirection(double *BeamDirection)
{
	cout << "****SetBeamDirection()" << endl;

	ms_BeamDirectionUpdate = RTI::RTI_TRUE;

	m_BeamDirection[0] = *BeamDirection;
	m_BeamDirection[1] = *(BeamDirection + 1);
}
void Radar::SetWorkStatus(const char* WorkStatus)
{
	cout << "****SetWorkStatus()" << endl;


	if (m_WorkStatus != WorkStatus)
	{
		this->ms_WorkStatusUpdate = RTI::RTI_TRUE;
		// Delete the existing memory
		delete[] m_WorkStatus;

		// Allocate appropriate size string and copy data
		if (WorkStatus && strlen(WorkStatus) > 0)
		{
			m_WorkStatus = new char[strlen(WorkStatus)+1];
			strcpy(m_WorkStatus, WorkStatus);
		}
		else
		{
			m_WorkStatus = NULL;
		}
	}


}
void Radar::SetDetection(double *Detection)
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
ostream& operator << (ostream &s, Radar &v)
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
ostream &operator << (ostream &s, Radar *v)
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

void Radar::setMATLABRadar(Radar* myRadar, Engine* ep, int posIdx)
{
	double posx[1] = { radarpos[posIdx-1][0] };
	double posy[1] = { radarpos[posIdx-1][1] };
	double posz[1] = { radarpos[posIdx-1][2] };

	mxArray *posxP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *posyP = mxCreateDoubleMatrix(1, 1, mxREAL);
	mxArray *poszP = mxCreateDoubleMatrix(1, 1, mxREAL);

	memcpy(mxGetPr(posxP), posx, sizeof(double));
	memcpy(mxGetPr(posyP), posy, sizeof(double));
	memcpy(mxGetPr(poszP), posz, sizeof(double));

	engPutVariable(ep, "posx", posxP);
	cout << endl<<"posx put in MATLAB: " << *posx<<endl;
	engPutVariable(ep, "posy", posyP);
	cout << "posy put in MATLAB: " << *posy<<endl;
	engPutVariable(ep, "posz", poszP);
	cout << "posz put in MATLAB: " << *posz<<endl;

	switch (posIdx)
	{
	case 1: 
	{
		engEvalString(ep, " [mfradar3,BeamWidth] = setRadar3(fc,fs,posx,posy,posz);");
		break;
	}

	case 2: 
	{
		engEvalString(ep, " [mfradar4,BeamWidth] = setRadar4(fc,fs,posx,posy,posz);");
		break;
	}

	default: break;
	}
	
	mxDestroyArray(posxP);
	mxDestroyArray(posyP);
	mxDestroyArray(poszP);

	cout << "Finished Set MATLAB radar_"<< posIdx+2 << endl;

}
void Radar::generateBeamPosition(Engine* ep)
{
	engEvalString(ep, " [beamPosition]=generateBeamPosition(45,60,BeamWidth);");
	engEvalString(ep, " jobq3 = generateJobQueue(beamPosition);");
	engEvalString(ep, " jobq4 = generateJobQueue(beamPosition);");
}

void Radar::getMATLABRadarStatus(Radar* pRadar, Engine* ep, int Idx)
{
	switch (Idx)
	{
	case 1:
	{
		engEvalString(ep, " [currentjob3, jobq3] = jobUpdate(jobq3, DetectionResult3);");
		engEvalString(ep, "  jobtype3char = currentjob3.JobType;");
		engEvalString(ep, " BeamDirection3 = currentjob3.BeamDirection;");
		mxArray *jobtype3 = engGetVariable(ep, "jobtype3char");
		mxArray *getBeamDir3 = engGetVariable(ep, "BeamDirection3");

		char* job3 = mxArrayToString(jobtype3);
		double* BeamDirection3 = mxGetPr(getBeamDir3);
		cout << endl << "radar_" << Idx+2 << " " << "current job:" << endl;		printf("%s", job3);
		cout << endl << *BeamDirection3 << "    " << *(BeamDirection3 + 1) << endl;
		pRadar->SetWorkStatus(job3);
		pRadar->SetBeamDirection(BeamDirection3);

		pRadar->ms_WorkStatusUpdate = RTI::RTI_TRUE;
		pRadar->ms_BeamDirectionUpdate = RTI::RTI_TRUE;

		mxDestroyArray(jobtype3);
		mxDestroyArray(getBeamDir3);
		break;
	}
	case 2:
	{
		engEvalString(ep, " [currentjob4, jobq4] = jobUpdate(jobq4, DetectionResult4);");
		engEvalString(ep, " jobtype4char = currentjob4.JobType;");
		engEvalString(ep, " BeamDirection4 = currentjob4.BeamDirection;");

		mxArray *jobtype4 = engGetVariable(ep, "jobtype4char");
		mxArray *getBeamDir4 = engGetVariable(ep, "BeamDirection4");

		char* job4 = mxArrayToString(jobtype4);
		double* BeamDirection4 = mxGetPr(getBeamDir4);
		cout << endl << "radar_"<< Idx+2<<" "<< "current job:" << endl;
		printf("%s", job4);
		cout << endl << *BeamDirection4 << "    " << *(BeamDirection4 + 1) << endl;

		pRadar->SetWorkStatus(job4);
		pRadar->SetBeamDirection(BeamDirection4);

		pRadar->ms_WorkStatusUpdate = RTI::RTI_TRUE;
		pRadar->ms_BeamDirectionUpdate = RTI::RTI_TRUE;
		mxDestroyArray(jobtype4);
		mxDestroyArray(getBeamDir4);
		break;
	}
	default:break;
	}
}

void Radar::getMATLABEcho(Radar* myRadar, Engine* ep, int Idx)
{
	switch (Idx)
	{
	
	case 1:
	{
		engEvalString(ep, " [xr3] = generateEcho(mfradar3,env,currentjob3);");
		cout << endl  << "echo3 ok" << endl;
		break;
	}
	case 2:
	{
		engEvalString(ep, " [xr4] = generateEcho(mfradar4,env,currentjob4);");
		cout << endl  << "echo4 ok" << endl;
		break;
	}
	default:break;
	}
}

void Radar::getMATLABdetection(Radar* pRadar, Engine* ep, int Idx)
{
	
	switch (Idx)
	{
	case 1:
	{
		engEvalString(ep, " [detection3,flag3] = generateDetection(xr3,mfradar3,currentjob3);");
		engEvalString(ep, " [DetectionResult3,jobq3]  = buildDetectionResult( flag3,detection3,jobq3);");
		mxArray *flag3 = engGetVariable(ep, "flag3");
		double *currentflag3 = mxGetPr(flag3);
		cout << "findflag3: " << *currentflag3 << endl;
		if (*currentflag3 != 0)
		{
			cout << endl << "Detection!" << endl;
			mxArray *detection3 = engGetVariable(ep, "DetectionResult3");
			double *getdetection3 = mxGetPr(detection3);
			cout << "Target Angle: " << *getdetection3 << ",   " << *(getdetection3 + 1) << endl;
			cout << "Target Range: " << *(getdetection3 + 2) << endl;
			pRadar->SetDetection(getdetection3);
			pRadar->ms_DetectionUpdate = RTI::RTI_TRUE;
			mxDestroyArray(detection3);
		}
		mxDestroyArray(flag3);
		break;
	}
	case 2:
	{

		engEvalString(ep, " [detection4,flag4] = generateDetection(xr4,mfradar4,currentjob4);");
		engEvalString(ep, " [DetectionResult4,jobq4]  = buildDetectionResult( flag4,detection4,jobq4);");
		mxArray *flag4 = engGetVariable(ep, "flag4");
		double *currentflag4 = mxGetPr(flag4);
		cout << "findflag4: " << *currentflag4 << endl;
		if (*currentflag4 != 0)
		{
			cout << endl << "Detection!" << endl;
			mxArray *detection4 = engGetVariable(ep, "DetectionResult4");
			double *getdetection4 = mxGetPr(detection4);
			cout << "Target Angle: " << *getdetection4 << ",    " << *(getdetection4 + 1) << endl;
			cout<< "Target Range: " << *(getdetection4 + 2)<<endl;
			pRadar->SetDetection(getdetection4);
			pRadar->ms_DetectionUpdate = RTI::RTI_TRUE;
			mxDestroyArray(detection4);
		}
		mxDestroyArray(flag4);
		break;
	}
	default: break;
	}

}
