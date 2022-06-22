#include "RadarSysFederateAmbassador.h"
#include "Target.h"
#include "Radar.h"
#include <fedtime.hh>
#include <RTI.hh>
#ifndef _MSC_VER
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#else
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#endif

extern RTIfedTime CurrentTime;
extern RTI::Boolean	TimeRegulated;
extern RTI::Boolean	TimeConstrained;
extern RTI::Boolean	timeAdvGrant;
extern RTI::Boolean	TimeAdvanceOutstanding;

RadarSysFederateAmbassador::RadarSysFederateAmbassador()
{
}

RadarSysFederateAmbassador::~RadarSysFederateAmbassador()
throw(RTI::FederateInternalError)
{
	cerr << "FED :call in RadarSysFederateAmbassador::~RadarSysFederateAmbassador()" << endl;
}
/////////////////////////////////////////////////////////////////////////
//                                时间管理部分
/////////////////////////////////////////////////////////////////////////


void RadarSysFederateAmbassador::timeRegulationEnabled(
	const  RTI::FedTime& theFederateTime) // supplied C4
	throw (
		RTI::InvalidFederationTime,
		RTI::EnableTimeRegulationWasNotPending,
		RTI::FederateInternalError)
{
	CurrentTime = theFederateTime;
	cout << "###timeRegulationEnabled callback" << endl;
	cout << "### Federate currenttime is " << CurrentTime.getTime() << endl;
	TimeRegulated = RTI::RTI_TRUE;
}

void RadarSysFederateAmbassador::timeConstrainedEnabled(
	const RTI::FedTime& theFederateTime) // supplied C4
	throw (
		RTI::InvalidFederationTime,
		RTI::EnableTimeConstrainedWasNotPending,
		RTI::FederateInternalError)
{
	CurrentTime = theFederateTime;
	if (CurrentTime.isPositiveInfinity()) CurrentTime = 0.0;
	cout << "###timeConstrainedEnabled callback" << endl;
	cout << "### Federate currenttime is " << CurrentTime.getTime() << endl;
	TimeConstrained = RTI::RTI_TRUE;
}

void RadarSysFederateAmbassador::timeAdvanceGrant(
	const RTI::FedTime& theTime) // supplied C4
	throw (
		RTI::InvalidFederationTime,
		RTI::TimeAdvanceWasNotInProgress,
		RTI::FederateInternalError)
{
	cout << endl << "###---------------------------------------" << endl;
	cout << "###timeAdvanceGrant callback" << endl;
	timeAdvGrant = RTI::RTI_TRUE;
	TimeAdvanceOutstanding = RTI::RTI_FALSE;
	CurrentTime = theTime;
	cout << "###currenttime:" << CurrentTime.getTime() << endl;
}

///////////////////////////////////////////////////////////////////////////
//                           Decleration Management
///////////////////////////////////////////////////////////////////////////

void RadarSysFederateAmbassador::startRegistrationForObjectClass(
	RTI::ObjectClassHandle   theClass)      // supplied C1
	throw (
		RTI::ObjectClassNotPublished,
		RTI::FederateInternalError)
{
	cout << endl << "###---------------------------------------" << endl;
	cout << "### startRegistrationForObjectClass callback	" << endl;
	if (theClass == Target::GetTargetRtiId())
	{
		cout << endl;
		cout << "###startRegistrationForObjectClass "<< Target::GetTargetRtiId()<< " callback" << endl;
		cout << "###发现异地对 Target 的订购，开启对 Target 的注册！" << endl;
		cout << "### turned registration on for Target class" << endl;
		Target::SetRegistration(RTI::RTI_TRUE);
	}
	else if (theClass == Radar::GetRadarRtiId())
	{
		cout << endl;
		cout << "###startRegistrationForObjectClass "<< Radar::GetRadarRtiId()<< " callback" << endl;
		cout << "###发现异地对 Radar 的订购，开启对 Radar 的注册！" << endl;
		cout << "### turned registration on for Radar class" << endl;
		Radar::SetRegistration(RTI::RTI_TRUE);
	}

	else
	{
		cerr << "### startRegistrationForObjectClass unkown class:" << theClass << endl;
	}
	cout << endl << "###---------------------------------------" << endl;
}

void RadarSysFederateAmbassador::stopRegistrationForObjectClass(
	RTI::ObjectClassHandle   theClass)      // supplied C1
	throw (
		RTI::ObjectClassNotPublished,
		RTI::FederateInternalError)
{
	cout << "### stopRegistrationForObjectClass callback	" << endl;
	if (theClass == Target::GetTargetRtiId())
	{
		cout << endl;
		cout << "### stopRegistrationForObjectClass callback for Target" << endl;
		Target::SetRegistration(RTI::RTI_FALSE);
	}
	else if (theClass == Radar::GetRadarRtiId())
	{
		cout << endl;
		cout << "### stopRegistrationForObjectClass callback for Radar" << endl;
		Radar::SetRegistration(RTI::RTI_FALSE);
	}
}


void RadarSysFederateAmbassador::turnInteractionsOn(
	RTI::InteractionClassHandle theHandle) // supplied C1
	throw (
		RTI::InteractionClassNotPublished,
		RTI::FederateInternalError)
{
}

void RadarSysFederateAmbassador::turnInteractionsOff(
	RTI::InteractionClassHandle theHandle) // supplied C1
	throw (
		RTI::InteractionClassNotPublished,
		RTI::FederateInternalError)
{
}

///////////////////////////////////////////////////////////////////////////
//                           Object Management
///////////////////////////////////////////////////////////////////////////

void RadarSysFederateAmbassador::discoverObjectInstance(
	RTI::ObjectHandle          theObject,      // supplied C1
	RTI::ObjectClassHandle     theObjectClass, // supplied C1
	const char *               theObjectName)  // supplied C4
	throw (
		RTI::CouldNotDiscover,
		RTI::ObjectClassNotKnown,
		RTI::FederateInternalError)
{
	cout << endl << "###---------------------------------------" << endl;
	cout << "### discoverObjectInstance callback "
		<< theObjectName << " with handle: " << theObject << endl;

	if (theObjectClass == Target::GetTargetRtiId())
	{
		Target* tmpPtr = new Target(theObject);
		tmpPtr->SetName(theObjectName);
	}
	else if (theObjectClass == Radar::GetRadarRtiId())
	{
		Radar* myRadar = new Radar(theObject);
		myRadar->SetName(theObjectName);
	}
	else
	{
		cerr << "### discovered object class unkown to me." << endl;
	}
	cout << endl << "###---------------------------------------" << endl;
}

void RadarSysFederateAmbassador::turnUpdatesOnForObjectInstance(
	RTI::ObjectHandle        theObject,     // supplied C1
	const RTI::AttributeHandleSet& theAttributes) // supplied C4
	throw (
		RTI::ObjectNotKnown,
		RTI::AttributeNotOwned,
		RTI::FederateInternalError)
{
	cout << endl;
	cout << "### turnUpdatesOnForObjectInstance callback" << endl;

	Target* pTarget = Target::Find(theObject);
	Radar* pRadar = Radar::Find(theObject);

	if (pTarget)
	{
		pTarget->SetUpdateControl(RTI::RTI_TRUE, theAttributes);
	}
	else if (pRadar)
	{
		pRadar->SetUpdateControl(RTI::RTI_TRUE, theAttributes);
	}
	else
	{
		cout << "### Object" << theObject << "not found" << endl;
	}
}

void RadarSysFederateAmbassador::turnUpdatesOffForObjectInstance(
	RTI::ObjectHandle        theObject,      // supplied C1
	const RTI::AttributeHandleSet& theAttributes) // supplied C4
	throw (
		RTI::ObjectNotKnown,
		RTI::AttributeNotOwned,
		RTI::FederateInternalError)
{
	cout << endl;
	cout << "### turnUpdatesOffForObjectInstance callback" << endl;
	Target* pTarget = Target::Find(theObject);
	Radar* pRadar = Radar::Find(theObject);
	if (pTarget)
	{
		pTarget->SetUpdateControl(RTI::RTI_FALSE, theAttributes);
	}
	else if (pRadar)
	{
		pRadar->SetUpdateControl(RTI::RTI_FALSE, theAttributes);
	}
	else
	{
		cout << "### Object" << theObject << "not found" << endl;
	}
}

void RadarSysFederateAmbassador::removeObjectInstance(
	RTI::ObjectHandle          theObject, // supplied C1
	const RTI::FedTime&              theTime,   // supplied C4
	const char                      *theTag,    // supplied C4
	RTI::EventRetractionHandle theHandle) // supplied C1
	throw (
		RTI::ObjectNotKnown,
		RTI::InvalidFederationTime,
		RTI::FederateInternalError)
{
	removeObjectInstance(theObject, theTag);
}

void RadarSysFederateAmbassador::removeObjectInstance(
	RTI::ObjectHandle          theObject, // supplied C1
	const char                      *theTag)    // supplied C4
	throw (
		RTI::ObjectNotKnown,
		RTI::FederateInternalError)
{
	cout << endl;
	cout << "###removeObjectInstance callback" << endl;
	Target* pTarget = Target::Find(theObject);
	Radar* pRadar = Radar::Find(theObject);
	if (pTarget)
	{
		cout << "#################################" << endl;
		cout << "### Removing Target " << pTarget->GetName() << endl;
		cout << "##################################" << endl;
		pTarget->SetInstanceDeleted(RTI::RTI_TRUE);
	}
	else if (pRadar)
	{
		cout << "#################################" << endl;
		cout << "### Removing Radar " << pRadar->GetName() << endl;
		cout << "##################################" << endl;
		pRadar->SetInstanceDeleted(RTI::RTI_TRUE);
	}
	else
	{
		cout << "###  Object" << theObject << "not found" << endl;
	}
}

void RadarSysFederateAmbassador::provideAttributeValueUpdate(
	RTI::ObjectHandle        theObject,     // supplied C1
	const RTI::AttributeHandleSet& theAttributes) // supplied C4
	throw (
		RTI::ObjectNotKnown,
		RTI::AttributeNotKnown,
		RTI::AttributeNotOwned,
		RTI::FederateInternalError)
{
	cout << "### provideAttributeValueUpdate" << endl;
	//-----------------------------------------------------------------
	// Find the instance this request is for.
	//-----------------------------------------------------------------
	Target *pTarget = Target::Find(theObject);
	Radar *pRadar = Radar::Find(theObject);
	if (pTarget)
	{
		RTI::AttributeHandle attrHandle;

		for (unsigned int i = 0; i < theAttributes.size(); i++)
		{
			attrHandle = theAttributes.getHandle(i);
			if (attrHandle == Target::GetPositionRtiId())
			{
				pTarget->SetPosition(pTarget->GetPosition());
			}
			else if (attrHandle == Target::GetVelocityRtiId())
			{
				pTarget->SetVelocity(pTarget->GetVelocity());
			}
			else if (attrHandle == Target::GetAccelerationRtiId())
			{
				pTarget->SetAcceleration(pTarget->GetAcceleration());
			}
		}
	}
	else if (pRadar)
	{
		RTI::AttributeHandle attrHandle;

		for (unsigned int i = 0; i < theAttributes.size(); i++)
		{
			attrHandle = theAttributes.getHandle(i);
			if (attrHandle == Radar::GetBeamDirectionRtiId())
			{
				pRadar->SetBeamDirection(pRadar->GetBeamDirection());
			}
			else if (attrHandle == Radar::GetDetectionRtiId())
			{
				pRadar->SetDetection(pRadar->GetDetection());
			}
			else if (attrHandle == Radar::GetWorkStatusRtiId())
			{
				pRadar->SetWorkStatus(pRadar->GetWorkStatus());
			}
		}
	}
	
}



void RadarSysFederateAmbassador::reflectAttributeValues(
	RTI::ObjectHandle                 theObject,     // supplied C1
	const RTI::AttributeHandleValuePairSet& theAttributes, // supplied C4
	const RTI::FedTime&                     theTime,       // supplied C1
	const char                             *theTag,        // supplied C4
	RTI::EventRetractionHandle        theHandle)     // supplied C1
	throw (
		RTI::ObjectNotKnown,
		RTI::AttributeNotKnown,
		RTI::FederateOwnsAttributes,
		RTI::InvalidFederationTime,
		RTI::FederateInternalError)
{
	reflectAttributeValues(theObject, theAttributes, theTag);
}

void RadarSysFederateAmbassador::reflectAttributeValues(
	RTI::ObjectHandle                 theObject,     // supplied C1
	const RTI::AttributeHandleValuePairSet& theAttributes, // supplied C4
	const char                             *theTag)        // supplied C4
	throw (
		RTI::ObjectNotKnown,
		RTI::AttributeNotKnown,
		RTI::FederateOwnsAttributes,
		RTI::FederateInternalError)
{
	Target* pTarget = NULL;
	Radar* pRadar = NULL;
	RTI::Double			Position[3] = { 0,0,0 };
	RTI::Double			Velocity[3] = { 0,0,0 };	
	RTI::Double			Acceleration[3] = { 0,0,0 };

	RTI::Double	   BeamDirection[2] = {0,0};
	//const char*		   WorkStatus = NULL;
	char                 Workstatus[6];
	RTI::Double	   Detection[3] = {0,0,0};

	cout << endl << "###---------------------------------------" << endl;
	cout << "###reflectAttributeValues callback,which is coming from: "
		<< theTag << endl;
	pTarget = Target::Find(theObject);
	pRadar = Radar::Find(theObject);

	if (pTarget)
	{
		cout << "###Found: " << pTarget->GetName() << endl;
		try
		{
			RTI::ULong length;
			for (unsigned int a = 0; a < theAttributes.size(); a++)
			{
				if (theAttributes.getHandle(a) == pTarget->GetPositionRtiId())
				{
					cout << "###reflect Position" << endl;
					theAttributes.getValue(a, (char*)Position, length);
					pTarget->SetPosition(Position);
				}
				else if (theAttributes.getHandle(a) == pTarget->GetVelocityRtiId())
				{
					cout << "###reflect Velocity" << endl;
					theAttributes.getValue(a, (char*)Velocity, length);
					pTarget->SetVelocity(Velocity);
				}
				else if (theAttributes.getHandle(a) == pTarget->GetAccelerationRtiId())
				{
					cout << "###reflect Acceleration" << endl;
					theAttributes.getValue(a, (char*)Acceleration, length);
					pTarget->SetAcceleration(Acceleration);
					//	if (pTarget->GetCleanliness() <= 0.0)
					//		pTarget->SetInstanceDeleted(RTI::RTI_TRUE);
				}

				else
				{
					cout << "### unkown attribute handle" << endl;
				}
			}
			
		}
		catch (RTI::Exception& e)
		{
			cout << e << endl;
			exit(1);
		}
	}
	
	if (pRadar)
	{
		cout << "###Found: " << pRadar->GetName() << endl;
		try
		{
			RTI::ULong length;
			for (unsigned int a = 0; a < theAttributes.size(); a++)
			{
				if (theAttributes.getHandle(a) == pRadar->GetBeamDirectionRtiId())
				{
					cout << "###reflect BeamDirection" << endl;
					theAttributes.getValue(a, (char*)BeamDirection, length);
					pRadar->SetBeamDirection(BeamDirection);
				}
				else if (theAttributes.getHandle(a) == pRadar->GetWorkStatusRtiId())
				{
					cout << "###reflect WorkStatus" << endl;
					theAttributes.getValue(a, (char*)Workstatus, length);
					const char* WorkStatus = Workstatus;
					pRadar->SetWorkStatus(WorkStatus);
					//theAttributes.getValue(a, (char*)WorkStatus, length);
					//pRadar->SetWorkStatus(WorkStatus);
				}
				else if (theAttributes.getHandle(a) == pRadar->GetDetectionRtiId())
				{
					cout << "###reflect Detection" << endl;
					theAttributes.getValue(a, (char*)Detection, length);
					pRadar->SetDetection(Detection);
				}

				else
				{
					cout << "### unkown attribute handle" << endl;
				}
			}

		}
		catch (RTI::Exception& e)
		{
			cout << e << endl;
			exit(1);
		}
	}
}

void RadarSysFederateAmbassador::receiveInteraction(
	RTI::InteractionClassHandle       theInteraction, // supplied C1
	const RTI::ParameterHandleValuePairSet& theParameters,  // supplied C4
	const RTI::FedTime&                     theTime,        // supplied C4
	const char                             *theTag,         // supplied C4
	RTI::EventRetractionHandle        theHandle)      // supplied C1
	throw (
		RTI::InteractionClassNotKnown,
		RTI::InteractionParameterNotKnown,
		RTI::InvalidFederationTime,
		RTI::FederateInternalError)
{
	receiveInteraction(theInteraction, theParameters, theTag);
}

void RadarSysFederateAmbassador::receiveInteraction(
	RTI::InteractionClassHandle       theInteraction, // supplied C1
	const RTI::ParameterHandleValuePairSet& theParams,  // supplied C4
	const char                             *theTag)         // supplied C4
	throw (
		RTI::InteractionClassNotKnown,
		RTI::InteractionParameterNotKnown,
		RTI::FederateInternalError)
{
}

///////////////////////////////////////////////////////////////////////////
//                           Ownership Management
///////////////////////////////////////////////////////////////////////////

void RadarSysFederateAmbassador::requestAttributeOwnershipAssumption(
	RTI::ObjectHandle        theObject,         // supplied C1
	const RTI::AttributeHandleSet& offeredAttributes, // supplied C4
	const char                    *theTag)            // supplied C4
	throw (
		RTI::ObjectNotKnown,
		RTI::AttributeNotKnown,
		RTI::AttributeAlreadyOwned,
		RTI::AttributeNotPublished,
		RTI::FederateInternalError)
{
}

void RadarSysFederateAmbassador::requestAttributeOwnershipRelease(
	RTI::ObjectHandle        theObject,           // supplied C1
	const RTI::AttributeHandleSet& candidateAttributes, // supplied C4
	const char                    *theTag)              // supplied C4
	throw (
		RTI::ObjectNotKnown,
		RTI::AttributeNotKnown,
		RTI::AttributeNotOwned,
		RTI::FederateInternalError)
{
}

void RadarSysFederateAmbassador::attributeOwnershipDivestitureNotification(
	RTI::ObjectHandle        theObject,          // supplied C1
	const RTI::AttributeHandleSet& releasedAttributes) // supplied C4
	throw (
		RTI::ObjectNotKnown,
		RTI::AttributeNotKnown,
		RTI::AttributeNotOwned,
		RTI::AttributeDivestitureWasNotRequested,
		RTI::FederateInternalError)
{
}

void RadarSysFederateAmbassador::attributeOwnershipAcquisitionNotification(
	RTI::ObjectHandle        theObject,         // supplied C1
	const RTI::AttributeHandleSet& securedAttributes) // supplied C4
	throw (
		RTI::ObjectNotKnown,
		RTI::AttributeNotKnown,
		RTI::AttributeAcquisitionWasNotRequested,
		RTI::AttributeAlreadyOwned,
		RTI::AttributeNotPublished,
		RTI::FederateInternalError)
{
}

