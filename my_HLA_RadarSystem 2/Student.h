#ifndef Student_h
#define Student_h

#if defined(_MSC_VER)
#define RTI_USES_STD_FSTREAM
#endif // defined(_MSC_VER)
#include <RTI.hh>
#if defined(RTI_USES_STD_FSTREAM)
using std::ostream;
#endif // defined(_MSC_VER)
#include <fedtime.hh>

#define MAX_FIGHTGAME_FEDERATES 21
class Splat;
class Student;

class Student
{
public:
	Student(char* name);
	Student(RTI::ObjectHandle id);
	virtual ~Student();
	//-------------------------
	// Methods acting on the RTI
	//--------------------------
	static Student* Find( RTI::ObjectHandle objectId );
	static void		Init(RTI::RTIambassador* rtiAmb);
	void			Publishing();
	void			Subscribing();
	void			Register();
	void			StudentUpdate(Student* pStudent);
	void			ProcessOwnership(Student* myStudent,RTI::Boolean defector,int loop);
	void			Shoot(Student* myStudent,Splat* mySplat);
	void			DeleteInstance(Student* pStudent);
	//-----------------------------------------------------------------
   // Friends of Country
   //-----------------------------------------------------------------
   friend ostream& operator << ( ostream &s, Student &v );
   friend ostream& operator << ( ostream &s, Student *v );

	//void			PrintStudent(Student* myStudent);
	//-----------------------------
	// Accessor Methods
	//-----------------------------
	const char*			GetName(){return ms_name;};
	RTI::ObjectHandle&	GetInstanceId(){return m_InstanceId;};
	RTI::Double&		GetLunchMoney(){return m_LunchMoney;};
	RTI::ULong&			GetAmmoAmount(){return m_AmmoAmount;};
	RTI::Double&		GetCleanliness(){return m_Cleanliness;};
	RTI::Double&		GetEnsuingMess(){return m_EnsuingMess;};
	
	RTI::Boolean			GetOwnLunchMoney(){return ms_OwnLunchMoney;};
	RTI::Boolean			GetOwnAmmoAmount(){return ms_OwnAmmoAmount;};
	RTI::Boolean			GetOwnCleanliness(){return ms_OwnCleanliness;};
	RTI::Boolean			GetOwnPrivilegeToDelete(){return ms_OwnPrivilegeToDelete;};

	RTI::Boolean			GetTransferLunchMoney(){return ms_TransferLunchMoney;};
	RTI::Boolean			GetTransferAmmoAmount(){return ms_TransferAmmoAmount;};
	RTI::Boolean			GetTransferCleanliness(){return ms_TransferCleanliness;};
	RTI::Boolean			GetTransferPrivilegeToDelete(){return ms_TransferPrivilegeToDelete;};

	RTI::Boolean			GetLunchMoneyUpdate(){return ms_LunchMoneyUpdate;};
	RTI::Boolean			GetAmmoAmountUpdate(){return ms_AmmoAmountUpdate;};
	RTI::Boolean			GetCleanlinessUpdate(){return ms_CleanlinessUpdate;};
	RTI::Boolean			GetInstanceDeleted(){return ms_InstanceDeleted;};
	//-----------------------------
	// static Accessor Methods
	//-----------------------------
	static RTI::Boolean				GetRegistration(){return ms_doRegistry;};
	static RTI::ObjectClassHandle	GetStudentRtiId(){return m_StudentHandle;};
	static RTI::AttributeHandle		GetLunchMoneyRtiId(){return m_LunchMoneyHandle;};
	static RTI::AttributeHandle		GetAmmoAmountRtiId(){return m_AmmoAmountHandle;};
	static RTI::AttributeHandle		GetCleanlinessRtiId(){return m_CleanlinessHandle;};
	static RTI::AttributeHandle		GetPrivilegeToDeleteRtiId(){return m_PrivilegeToDeleteHandle;};
	//-----------------------------
	// Mutator Methods
	//-----------------------------
	void SetName( const char* name);
	void SetLunchMoney(const RTI::Double LunchMoney);
	void SetAmmoAmount(const RTI::ULong AmmoAmount);
	void SetCleanliness(const RTI::Double Cleanliness);
	void SetEnsuingMess(const RTI::Double EnsuingMess);

	void SetOwnLunchMoney(RTI::Boolean status){ms_OwnLunchMoney = status;};
	void SetOwnAmmoAmount(RTI::Boolean status){ms_OwnAmmoAmount = status;};
	void SetOwnCleanliness(RTI::Boolean status){ms_OwnCleanliness = status;};
	void SetOwnPrivilegeToDelete(RTI::Boolean status){ms_OwnPrivilegeToDelete = status;};

	void SetTransferLunchMoney(RTI::Boolean status){ms_TransferLunchMoney = status;};
	void SetTransferAmmoAmount(RTI::Boolean status){ms_TransferAmmoAmount = status;};
	void SetTransferCleanliness(RTI::Boolean status){ms_TransferCleanliness = status;};
	void SetTransferPrivilegeToDelete(RTI::Boolean status){ms_TransferPrivilegeToDelete = status;};

	void SetLunchMoneyUpdate(RTI::Boolean status){ms_LunchMoneyUpdate = status;};
	void SetAmmoAmountUpdate(RTI::Boolean status){ms_AmmoAmountUpdate = status;};
	void SetCleanlinessUpdate(RTI::Boolean status){ms_CleanlinessUpdate = status;};
	void SetInstanceDeleted(RTI::Boolean status){ms_InstanceDeleted = status;};

	void SetUpdateControl(RTI::Boolean status,const RTI::AttributeHandleSet& theAttrHandles);
	static void SetRegistration(RTI::Boolean status){ms_doRegistry = status;};
	//-----------------------------
	// static members
	//-----------------------------
	static Student*		ms_StudentExtent[MAX_FIGHTGAME_FEDERATES];
	//static unsigned int		ms_extentCardinality;
	static int		ms_extentCardinality;
private:
	char* ms_name;
	RTI::ObjectHandle	m_InstanceId; //对象实例句柄

	/*对象实例自身状态*/
	RTI::Double	   m_LunchMoney; //午餐费（初始为20）
	RTI::ULong	   m_AmmoAmount; //弹药数量（初始为0）
	RTI::Double	   m_Cleanliness; //生命力指数（初始为100）
	RTI::Double	   m_EnsuingMess; //每次开伙的价值（对方污点数增加10，同时生命力减少10）
	
	/*所有权拥有标志*/
	RTI::Boolean   ms_OwnLunchMoney;
	RTI::Boolean   ms_OwnAmmoAmount;
	RTI::Boolean   ms_OwnCleanliness;
	RTI::Boolean   ms_OwnPrivilegeToDelete		;

	/*所有权转移标志*/
	RTI::Boolean  ms_TransferLunchMoney;			
	RTI::Boolean  ms_TransferAmmoAmount;			
	RTI::Boolean  ms_TransferCleanliness;			
	RTI::Boolean  ms_TransferPrivilegeToDelete;			

	/*对象实例更新标志*/
	RTI::Boolean  ms_LunchMoneyUpdate;			
	RTI::Boolean  ms_AmmoAmountUpdate;			
	RTI::Boolean  ms_CleanlinessUpdate;			
	RTI::Boolean  ms_InstanceDeleted;			
	//-----------------------------
	// static members data
	//-----------------------------
	static RTI::Boolean				ms_doRegistry;

	/*对象类及属性句柄*/
	static RTI::ObjectClassHandle	m_StudentHandle;
	static RTI::AttributeHandle		m_LunchMoneyHandle;
	static RTI::AttributeHandle		m_AmmoAmountHandle;
	static RTI::AttributeHandle		m_CleanlinessHandle;
	static RTI::AttributeHandle		m_PrivilegeToDeleteHandle;

	/*对象类及属性名称*/
	static char* const				ms_StudentTypeStr;
	static char* const				ms_LunchMoneyTypeStr;
	static char* const				ms_AmmoAmountTypeStr;
	static char* const				ms_CleanlinessTypeStr;
	static char* const				ms_PrivilegeToDeleteTypeStr;
	static RTI::RTIambassador*		ms_rtiAmb;
};

#endif