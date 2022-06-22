#ifndef Radar_h
#define Radar_h
#include "target.h"
#if defined(_MSC_VER)
#define RTI_USES_STD_FSTREAM
#endif // defined(_MSC_VER)
#include <RTI.hh>
#if defined(RTI_USES_STD_FSTREAM)
using std::ostream;
#endif // defined(_MSC_VER)
#include <fedtime.hh>
#include "engine.h"

class Radar;
class Target;

class Radar
{
public:
	Radar();
	Radar(RTI::ObjectHandle id);
	virtual ~Radar();
	//-------------------------
	// Methods acting on the RTI
	//--------------------------
	static Radar*   Find(RTI::ObjectHandle objectId);
	static void		Init(RTI::RTIambassador* rtiAmb);
	void			Publishing();
	//void			Subscribing();
	void            Subscribing();
	void			Register();
	void			RadarUpdate(Radar* pRadar);
	//void			ProcessOwnership(Radar* myRadar, RTI::Boolean defector, int loop);
	void			DeleteInstance(Radar* pRadar);
	//--------------------------
	// Matlab methods
	//--------------------------

	void setMATLABRadar(Radar* myRadar, Engine* ep, int posIdx);
	void getMATLABEcho(Radar* myRadar, Engine* ep, int Idx);
	void getMATLABRadarStatus(Radar* myRadar, Engine* ep, int Idx);
	void getMATLABdetection(Radar* myRadar, Engine* ep, int Idx);
	void Radar::generateBeamPosition(Engine* ep);

	//-----------------------------------------------------------------
	// operator <<
	//-----------------------------------------------------------------
	friend ostream& operator << (ostream &s, Radar &v);
	friend ostream& operator << (ostream &s, Radar *v);

	//-----------------------------
	// Accessor Methods
	//-----------------------------
	const char*			GetName() { return ms_name; };
	RTI::ObjectHandle&	GetInstanceId() { return m_InstanceId; };
	RTI::Double*		GetStationPosition() { return m_StationPosition; };
	RTI::Double*		GetBeamDirection() { return m_BeamDirection; };
	const char*	        GetWorkStatus() { return m_WorkStatus; };
	RTI::Double*		GetDetection() { return m_Detection; };

	RTI::Boolean		GetOwnBeamDirection() { return ms_OwnBeamDirection; };
	RTI::Boolean		GetOwnWorkStatus() { return ms_OwnWorkStatus; };
	RTI::Boolean		GetOwnDetection() { return ms_OwnDetection; };
	RTI::Boolean		GetOwnPrivilegeToDelete() { return ms_OwnPrivilegeToDelete; };

	RTI::Boolean		GetTransferBeamDirection() { return ms_TransferBeamDirection; };
	RTI::Boolean		GetTransferWorkStatus() { return ms_TransferWorkStatus; };
	RTI::Boolean		GetTransferDetection() { return ms_TransferDetection; };
	RTI::Boolean		GetTransferPrivilegeToDelete() { return ms_TransferPrivilegeToDelete; };

	RTI::Boolean		GetBeamDirectionUpdate() { return ms_BeamDirectionUpdate; };
	RTI::Boolean		GetWorkStatusUpdate() { return ms_WorkStatusUpdate; };
	RTI::Boolean		GetDetectionUpdate() { return ms_DetectionUpdate; };
	RTI::Boolean		GetInstanceDeleted() { return ms_InstanceDeleted; };
	//-----------------------------
	// static Accessor Methods
	//-----------------------------
	static RTI::Boolean				GetRegistration() { return ms_doRegistry; };
	static RTI::ObjectClassHandle	GetRadarRtiId() { return m_RadarHandle; };
	static RTI::AttributeHandle		GetStationPositionRtiId() { return m_StationPositionHandle; };
	static RTI::AttributeHandle		GetBeamDirectionRtiId() { return m_BeamDirectionHandle; };
	static RTI::AttributeHandle		GetWorkStatusRtiId() { return m_WorkStatusHandle; };
	static RTI::AttributeHandle		GetDetectionRtiId() { return m_DetectionHandle; };
	static RTI::AttributeHandle		GetPrivilegeToDeleteRtiId() { return m_PrivilegeToDeleteHandle; };
	//-----------------------------
	// Mutator Methods
	//-----------------------------
	void SetName(const char* name);
	void SetStationPosition(double *StationPosition);
	void SetBeamDirection(double *BeamDirection);
	void SetWorkStatus(const char* WorkStatus);
	void SetDetection(double *Detection);

	void SetOwnBeamDirection(RTI::Boolean status) { ms_OwnBeamDirection = status; };
	void SetOwnWorkStatus(RTI::Boolean status) { ms_OwnWorkStatus = status; };
	void SetOwnDetection(RTI::Boolean status) { ms_OwnDetection = status; };
	void SetOwnPrivilegeToDelete(RTI::Boolean status) { ms_OwnPrivilegeToDelete = status; };

	void SetTransferBeamDirection(RTI::Boolean status) { ms_TransferBeamDirection = status; };
	void SetTransferWorkStatus(RTI::Boolean status) { ms_TransferWorkStatus = status; };
	void SetTransferDetection(RTI::Boolean status) { ms_TransferDetection = status; };
	void SetTransferPrivilegeToDelete(RTI::Boolean status) { ms_TransferPrivilegeToDelete = status; };

	void SetBeamDirectionUpdate(RTI::Boolean status) { ms_BeamDirectionUpdate = status; };
	void SetWorkStatusUpdate(RTI::Boolean status) { ms_WorkStatusUpdate = status; };
	void SetDetectionUpdate(RTI::Boolean status) { ms_DetectionUpdate = status; };
	void SetInstanceDeleted(RTI::Boolean status) { ms_InstanceDeleted = status; };

	void SetUpdateControl(RTI::Boolean status, const RTI::AttributeHandleSet& theAttrHandles);
	static void SetRegistration(RTI::Boolean status) { ms_doRegistry = status; };
	//-----------------------------
	// static members
	//-----------------------------
	static Radar*		ms_RadarExtent[4];
	//static unsigned int		ms_extentCardinality;
	static int		ms_extentCardinality;
private:
	char* ms_name;
	RTI::ObjectHandle	m_InstanceId; //对象实例句柄

									  /*对象实例自身状态*/
	RTI::Double	   m_StationPosition[3];
	RTI::Double	   m_BeamDirection[2]; 
	char*          m_WorkStatus; 
	RTI::Double	   m_Detection[3]; 


								  /*所有权拥有标志*/
	RTI::Boolean   ms_OwnBeamDirection;
	RTI::Boolean   ms_OwnWorkStatus;
	RTI::Boolean   ms_OwnDetection;
	RTI::Boolean   ms_OwnPrivilegeToDelete;

	/*所有权转移标志*/
	RTI::Boolean  ms_TransferBeamDirection;
	RTI::Boolean  ms_TransferWorkStatus;
	RTI::Boolean  ms_TransferDetection;
	RTI::Boolean  ms_TransferPrivilegeToDelete;

	/*对象实例更新标志*/
	RTI::Boolean  ms_BeamDirectionUpdate;
	RTI::Boolean  ms_WorkStatusUpdate;
	RTI::Boolean  ms_DetectionUpdate;
	RTI::Boolean  ms_InstanceDeleted;
	//-----------------------------
	// static members data
	//-----------------------------
	static RTI::Boolean				ms_doRegistry;

	/*对象类及属性句柄*/
	static RTI::ObjectClassHandle	m_RadarHandle;
	static RTI::AttributeHandle		m_StationPositionHandle;
	static RTI::AttributeHandle		m_BeamDirectionHandle;
	static RTI::AttributeHandle		m_WorkStatusHandle;
	static RTI::AttributeHandle		m_DetectionHandle;
	static RTI::AttributeHandle		m_PrivilegeToDeleteHandle;

	/*对象类及属性名称*/
	static char* const				ms_RadarTypeStr;
	static char* const				ms_StationPositionTypeStr;
	static char* const				ms_BeamDirectionTypeStr;
	static char* const				ms_WorkStatusTypeStr;
	static char* const				ms_DetectionTypeStr;
	static char* const				ms_PrivilegeToDeleteTypeStr;
	static RTI::RTIambassador*		ms_rtiAmb;
};

#endif
