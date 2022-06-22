#ifndef Target_h
#define Target_h

#if defined(_MSC_VER)
#define RTI_USES_STD_FSTREAM
#endif // defined(_MSC_VER)
#include <RTI.hh>
#if defined(RTI_USES_STD_FSTREAM)
using namespace std;
using std::ostream;
#endif // defined(_MSC_VER)
#include <fedtime.hh>
#include "engine.h" 
#include <vector>

class Target
{
public:
	Target();
	Target(RTI::ObjectHandle id);
	virtual ~Target();
	//-------------------------
	// Methods acting on the RTI
	//--------------------------
	static Target*  Find(RTI::ObjectHandle objectId);
	static void		Init(RTI::RTIambassador* rtiAmb);
	void			Publishing();
	void			Subscribing();
	void			Register();
	void			TargetUpdate(Target* pTarget);
	void			DeleteInstance(Target* pStudent);
	void			ProcessOwnership(Target* myTarget, RTI::Boolean defector, int loop) {};
	
	//--------------------------
	// Matlab methods
	//--------------------------

	void			SetTargetTakeOff(Target* myTarget,Engine* ep);
	void			MATLABTargetMove(Target* myTarget, Engine* ep);
	int				setMATLABTarget(const int&  mode, const double& rcs, const double* tgt_Position, const double* tgt_v, const double* tgt_a, Engine *ep);
	//-----------------------------------------------------------------
	// Friends of Country
	//-----------------------------------------------------------------
	friend ostream& operator << (ostream &s, Target &v);
	friend ostream& operator << (ostream &s, Target *v);

	//-----------------------------
	// Accessor Methods
	//-----------------------------
	const char*			GetName() { return ms_name; };
	RTI::ObjectHandle&	GetInstanceId() { return m_InstanceId; };
	RTI::Double*		GetPosition() { return m_Position; };
	RTI::Double*		GetVelocity() { return m_Velocity; };
	RTI::Double*		GetAcceleration() { return m_Acceleration; };

	RTI::Boolean			GetOwnPosition() { return ms_OwnPosition; };
	RTI::Boolean			GetOwnVelocity() { return ms_OwnVelocity; };
	RTI::Boolean			GetOwnAcceleration() { return ms_OwnAcceleration; };
	RTI::Boolean			GetOwnPrivilegeToDelete() { return ms_OwnPrivilegeToDelete; };

	RTI::Boolean			GetTransferPosition() { return ms_TransferPosition; };
	RTI::Boolean			GetTransferVelocity() { return ms_TransferVelocity; };
	RTI::Boolean			GetTransferAcceleration() { return ms_TransferAcceleration; };
	RTI::Boolean			GetTransferPrivilegeToDelete() { return ms_TransferPrivilegeToDelete; };

	RTI::Boolean			GetPositionUpdate() { return ms_PositionUpdate; };
	RTI::Boolean			GetVelocityUpdate() { return ms_VelocityUpdate; };
	RTI::Boolean			GetAccelerationUpdate() { return ms_AccelerationUpdate; };
	RTI::Boolean			GetInstanceDeleted() { return ms_InstanceDeleted; };

	//-----------------------------
	// static Accessor Methods
	//-----------------------------
	static RTI::Boolean				GetRegistration() { return ms_doRegistry; };
	static RTI::ObjectClassHandle	GetTargetRtiId() { return m_TargetHandle; };
	static RTI::AttributeHandle		GetPositionRtiId() { return m_PositionHandle; };
	static RTI::AttributeHandle		GetVelocityRtiId() { return m_VelocityHandle; };
	static RTI::AttributeHandle		GetAccelerationRtiId() { return m_AccelerationHandle; };
	static RTI::AttributeHandle		GetPrivilegeToDeleteRtiId() { return m_PrivilegeToDeleteHandle; };

	//-----------------------------
	// Mutator Methods
	//-----------------------------
	void SetName(const char* name);
	void SetPosition(double *Position);
	void SetVelocity(double *Velocity);
	void SetAcceleration(double *Acceleration);

	void SetOwnPosition(RTI::Boolean status) { ms_OwnPosition = status; };
	void SetOwnVelocity(RTI::Boolean status) { ms_OwnVelocity = status; };
	void SetOwnAcceleration(RTI::Boolean status) { ms_OwnAcceleration = status; };
	void SetOwnPrivilegeToDelete(RTI::Boolean status) { ms_OwnPrivilegeToDelete = status; };

	void SetTransferPosition(RTI::Boolean status) { ms_TransferPosition = status; };
	void SetTransferVelocity(RTI::Boolean status) { ms_TransferVelocity = status; };
	void SetTransferAcceleration(RTI::Boolean status) { ms_TransferAcceleration = status; };
	void SetTransferPrivilegeToDelete(RTI::Boolean status) { ms_TransferPrivilegeToDelete = status; };

	void SetPositionUpdate(RTI::Boolean status) { ms_PositionUpdate = status; };
	void SetVelocityUpdate(RTI::Boolean status) { ms_VelocityUpdate = status; };
	void SetAccelerationUpdate(RTI::Boolean status) { ms_AccelerationUpdate = status; };
	void SetInstanceDeleted(RTI::Boolean status) { ms_InstanceDeleted = status; };

	void SetUpdateControl(RTI::Boolean status, const RTI::AttributeHandleSet& theAttrHandles);
	static void SetRegistration(RTI::Boolean status) { ms_doRegistry = status; };

	//-----------------------------
	// static members
	//-----------------------------
	static Target*		ms_TargetExtent;
	//static unsigned int		ms_extentCardinality;
	//static int		ms_extentCardinality;

private:
	char* ms_name;
	RTI::ObjectHandle	m_InstanceId; //对象实例句柄

	/*对象实例自身状态*/
	RTI::Double m_Position[3];
	RTI::Double m_Velocity[3];
	RTI::Double m_Acceleration[3];

	/*所有权拥有标志*/
	RTI::Boolean   ms_OwnPosition;
	RTI::Boolean   ms_OwnVelocity;
	RTI::Boolean   ms_OwnAcceleration;
	RTI::Boolean   ms_OwnPrivilegeToDelete;

	/*所有权转移标志*/
	RTI::Boolean  ms_TransferPosition;
	RTI::Boolean  ms_TransferVelocity;
	RTI::Boolean  ms_TransferAcceleration;
	RTI::Boolean  ms_TransferPrivilegeToDelete;

	/*对象实例更新标志*/
	RTI::Boolean  ms_PositionUpdate;
	RTI::Boolean  ms_VelocityUpdate;
	RTI::Boolean  ms_AccelerationUpdate;
	RTI::Boolean  ms_InstanceDeleted;

	//-----------------------------
	// static members data
	//-----------------------------
	static RTI::Boolean				ms_doRegistry;

	/*对象类及属性句柄*/
	static RTI::ObjectClassHandle	m_TargetHandle;
	static RTI::AttributeHandle		m_PositionHandle;
	static RTI::AttributeHandle		m_VelocityHandle;
	static RTI::AttributeHandle		m_AccelerationHandle;
	static RTI::AttributeHandle		m_PrivilegeToDeleteHandle;

	/*对象类及属性名称*/
	static char* const				ms_TargetTypeStr;
	static char* const				ms_PositionTypeStr;
	static char* const				ms_VelocityTypeStr;
	static char* const				ms_AccelerationTypeStr;
	static char* const				ms_PrivilegeToDeleteTypeStr;
	static RTI::RTIambassador*		ms_rtiAmb;

};






#endif