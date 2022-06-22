#include "Student.h"
#include "Splat.h"
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
#endif

#define STUDENT_NUM 10

extern RTI::RTIambassador	rtiAmb;
extern Student*		myStudent;
extern RTIfedTime Lookahead;
extern RTIfedTime CurrentTime;
extern RTI::Boolean	Regulating;

RTI::RTIambassador*	Student::ms_rtiAmb=NULL;
RTI::Boolean		Student::ms_doRegistry=RTI::RTI_FALSE;
RTI::ObjectClassHandle	Student::m_StudentHandle=0;
RTI::AttributeHandle	Student::m_LunchMoneyHandle=0;
RTI::AttributeHandle	Student::m_AmmoAmountHandle=0;
RTI::AttributeHandle	Student::m_CleanlinessHandle=0;
RTI::AttributeHandle	Student::m_PrivilegeToDeleteHandle=0;

char* const		Student::ms_StudentTypeStr="Student";
char* const		Student::ms_LunchMoneyTypeStr="LunchMoney";
char* const		Student::ms_AmmoAmountTypeStr="AmmoAmount";
char* const		Student::ms_CleanlinessTypeStr="Cleanliness";
char* const		Student::ms_PrivilegeToDeleteTypeStr="PrivilegeToDelete";

Student*		Student::ms_StudentExtent[MAX_FIGHTGAME_FEDERATES]=
{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
int		Student::ms_extentCardinality=0;

//////////////////////////////////////////////////////////////////////
//                     Student类初始化
//////////////////////////////////////////////////////////////////////

Student::Student(char* name)
	:ms_name(NULL),
	m_LunchMoney(20),
	m_AmmoAmount(0),
	m_Cleanliness(100),
	m_EnsuingMess(10),
	ms_OwnLunchMoney(RTI::RTI_TRUE),
	ms_OwnAmmoAmount(RTI::RTI_TRUE),
	ms_OwnCleanliness(RTI::RTI_TRUE),
	ms_OwnPrivilegeToDelete(RTI::RTI_TRUE),
	ms_TransferLunchMoney(RTI::RTI_FALSE),
	ms_TransferAmmoAmount(RTI::RTI_FALSE),
	ms_TransferCleanliness(RTI::RTI_FALSE),
	ms_TransferPrivilegeToDelete(RTI::RTI_FALSE),
	ms_LunchMoneyUpdate(RTI::RTI_FALSE),
	ms_AmmoAmountUpdate(RTI::RTI_FALSE),
	ms_CleanlinessUpdate(RTI::RTI_FALSE),
	ms_InstanceDeleted(RTI::RTI_FALSE)
{
	cout<<"***Student() OK"<<endl<<endl;

	Student::ms_StudentExtent[ Student::ms_extentCardinality++ ] = this;

	this->SetName(name);
	this->SetLunchMoney(20);
	this->SetAmmoAmount(0);
	this->SetCleanliness(100);
	this->SetEnsuingMess(10);
}

Student::Student(RTI::ObjectHandle id)
	:ms_name(NULL),
	m_InstanceId(id),
	m_LunchMoney(20),
	m_AmmoAmount(0),
	m_Cleanliness(100),
	m_EnsuingMess(10),
	ms_OwnLunchMoney(RTI::RTI_FALSE),
	ms_OwnAmmoAmount(RTI::RTI_FALSE),
	ms_OwnCleanliness(RTI::RTI_FALSE),
	ms_OwnPrivilegeToDelete(RTI::RTI_FALSE),
	ms_TransferLunchMoney(RTI::RTI_FALSE),
	ms_TransferAmmoAmount(RTI::RTI_FALSE),
	ms_TransferCleanliness(RTI::RTI_FALSE),
	ms_TransferPrivilegeToDelete(RTI::RTI_FALSE),
	ms_LunchMoneyUpdate(RTI::RTI_FALSE),
	ms_AmmoAmountUpdate(RTI::RTI_FALSE),
	ms_CleanlinessUpdate(RTI::RTI_FALSE),
	ms_InstanceDeleted(RTI::RTI_FALSE)
{
	cout<<"***Student(id) OK"<<endl<<endl;

	Student::ms_StudentExtent[ Student::ms_extentCardinality++ ] = this;

	this->SetName("two");
	this->SetLunchMoney(20);
	this->SetAmmoAmount(0);
	this->SetCleanliness(100);
	this->SetEnsuingMess(10);
}

Student::~Student()
{
	cout<<"***~Student() OK"<<endl<<endl;


	Student *pStudent = NULL; 
	int idx=0;

	for(idx=0;idx<Student::ms_extentCardinality;idx++)
	{
		pStudent = Student::ms_StudentExtent[idx];   
  
        if (pStudent && pStudent->GetInstanceId() == this->GetInstanceId())   
        {   
			break;   
        }  
	}
	if (pStudent)   
    {   
        for (int i=idx;(i<Student::ms_extentCardinality) && (Student::ms_StudentExtent[i]!=NULL); i++)   
        {   
			Student::ms_StudentExtent[i] = Student::ms_StudentExtent[i+1];   
        }   
        Student::ms_extentCardinality = Student::ms_extentCardinality - 1;   
        if (ms_rtiAmb && idx == 0)   
        {   
			ms_rtiAmb->queryLookahead(Lookahead);
            (void)ms_rtiAmb->deleteObjectInstance(this->GetInstanceId(),CurrentTime+Lookahead,"Student Quits");   
        }
		else
		{
		}
    }   
    delete [] ms_name;
}


void	Student::Init(RTI::RTIambassador* rtiAmb)
{
	cout<<"***Init() OK"<<endl<<endl;

	ms_rtiAmb = rtiAmb;
	if(ms_rtiAmb)
	{
		try
		{
			m_StudentHandle = ms_rtiAmb->getObjectClassHandle(ms_StudentTypeStr);
			m_AmmoAmountHandle= ms_rtiAmb->getAttributeHandle(ms_AmmoAmountTypeStr,m_StudentHandle);
			m_LunchMoneyHandle = ms_rtiAmb->getAttributeHandle(ms_LunchMoneyTypeStr,m_StudentHandle);
			m_CleanlinessHandle = ms_rtiAmb->getAttributeHandle(ms_CleanlinessTypeStr,m_StudentHandle);
			m_PrivilegeToDeleteHandle = ms_rtiAmb->getAttributeHandle(ms_PrivilegeToDeleteTypeStr,m_StudentHandle);

			cout<<"***acquired  class attribute handles"<<endl;
			cout<<"***StudentHandle:"<<m_StudentHandle<<endl;
			cout<<"***AmmoAmountHandle:"<<m_AmmoAmountHandle<<endl;
			cout<<"***LunchMoneyHandle:"<<m_LunchMoneyHandle<<endl;
			cout<<"***CleanlinessHandle:"<<m_CleanlinessHandle<<endl;
			cout<<"***PrivilegeToDeleteHandle:"<<m_PrivilegeToDeleteHandle<<endl<<endl;
		}
		catch ( RTI::Exception& e )
        {
          cout << e << endl;
          exit(1);
        }
	}
}
//////////////////////////////////////////////////////////////////////
//                          声明：公布、订购
//////////////////////////////////////////////////////////////////////
void	Student::Publishing()
{
	cout<<"***Publishing() OK"<<endl<<endl;

	if(ms_rtiAmb)
	{
		RTI::AttributeHandleSet* StudentAttributes;

		try
		{
			StudentAttributes = RTI::AttributeHandleSetFactory::create(4);
			StudentAttributes->add(m_AmmoAmountHandle);
			StudentAttributes->add(m_LunchMoneyHandle);
			StudentAttributes->add(m_CleanlinessHandle);
			StudentAttributes->add(m_PrivilegeToDeleteHandle);
		}
		catch ( RTI::Exception& e )
        {
          cout << e << endl;
          exit(1);
        }
		try
		{
			cout<<"*** published objectclass student"<<endl<<endl;
			ms_rtiAmb->publishObjectClass(m_StudentHandle,*StudentAttributes);
		}
		catch ( RTI::Exception& e )
        {
          cout << e << endl;
          exit(1);
        }
		if(StudentAttributes)
		{
			StudentAttributes->empty();
			delete StudentAttributes;
		}
	}
}

void	Student::Subscribing()
{
	cout<<"***Subscribing() OK"<<endl<<endl;

	if(ms_rtiAmb)
	{
		RTI::AttributeHandleSet* StudentAttributes;
		
		try
		{
			StudentAttributes = RTI::AttributeHandleSetFactory::create(4);
			StudentAttributes->add(m_AmmoAmountHandle);
			StudentAttributes->add(m_LunchMoneyHandle);
			StudentAttributes->add(m_CleanlinessHandle);
			StudentAttributes->add(m_PrivilegeToDeleteHandle);
		}
		catch ( RTI::Exception& e )
        {
          cout << e << endl;
          exit(1);
        }
		try
		{
			cout<<"*** Subscribe objectclass student"<<endl<<endl;
			ms_rtiAmb->subscribeObjectClassAttributes(m_StudentHandle,*StudentAttributes);
		}
		catch ( RTI::Exception& e )
        {
          cout << e << endl;
          exit(1);
        }
		if(StudentAttributes)
		{
			StudentAttributes->empty();
			delete StudentAttributes;
		}
	}
}



void	Student::Register()
{
	cout<<"***Register() OK"<<endl<<endl;

	if(ms_rtiAmb)
	{
		if(!ms_doRegistry)
		{
			cout<<"*** because no one subscrib student class,so tick time!"<<endl;
		}
		while(!ms_doRegistry)
		{
			ms_rtiAmb->tick(0.01,2.0);
		}
		for(int n=0;n<STUDENT_NUM;n++)
		{
			try
			{
				(this->ms_StudentExtent[n])->m_InstanceId = 
					ms_rtiAmb->registerObjectInstance(
					(this->ms_StudentExtent[n])->GetStudentRtiId(),
					(this->ms_StudentExtent[n])->GetName());
				cout<<"*** registered student:"
					<<(this->ms_StudentExtent[n])->GetInstanceId()
					<<"Name:"<<(this->ms_StudentExtent[n])->GetName()<<endl;
				ms_rtiAmb->tick(0.01,2.0);
			}
			catch ( RTI::Exception& e )
			{
			  cout << e << endl;
			  exit(1);
			}
		}
	}
}

void	Student::DeleteInstance(Student* myStudent)
{
	cout<<"***DeleteInstance() OK"<<endl<<endl;


	Student* pStudent = NULL;
	for(int idx=0;idx<myStudent->ms_extentCardinality;idx++)
	{
		pStudent = myStudent->ms_StudentExtent[idx];
		if(pStudent->GetInstanceDeleted() && pStudent->GetOwnPrivilegeToDelete())
		{
			try
			{
				cout<<endl;
				cout<<"*****************************"<<endl;
				cout<<"*** delete object:"<<pStudent->GetName()<<endl;
				cout<<"*****************************"<<endl;
				if(Regulating)
				{
					ms_rtiAmb->queryLookahead(Lookahead);
					ms_rtiAmb->deleteObjectInstance(pStudent->GetInstanceId(),CurrentTime+Lookahead,"Student Quits");
					ms_rtiAmb->tick(0.01,2.0);
				}
				else
				{
					ms_rtiAmb->deleteObjectInstance(pStudent->GetInstanceId(),"Student Quits");
					ms_rtiAmb->tick(0.01,2.0);
				}
			}
			catch ( RTI::Exception& e )
			{
			  cout << e << endl;
			  exit(1);
			}
			pStudent->SetOwnPrivilegeToDelete(RTI::RTI_FALSE);
		}
	}
}

void	Student::StudentUpdate(Student* pStudent)
{
	cout<<"****StudentUpdate() OK"<<endl;

	int idx=0;
	RTI::Double			LunchMoney=0;
	RTI::ULong			AmmoAmount=0;
	RTI::Double			Cleanliness=0;
	RTI::AttributeHandleValuePairSet* attrs = NULL;

	try
	{
		attrs = RTI::AttributeSetFactory::create(3);
		if(pStudent->GetLunchMoneyUpdate())
		{
			LunchMoney = pStudent->GetLunchMoney();
			attrs->add(pStudent->GetLunchMoneyRtiId(),(char*)&LunchMoney,sizeof(LunchMoney));
			pStudent->SetLunchMoneyUpdate(RTI::RTI_FALSE);
		}
		if(pStudent->GetAmmoAmountUpdate())
		{
			AmmoAmount = pStudent->GetAmmoAmount();
			attrs->add(pStudent->GetAmmoAmountRtiId(),(char*)&AmmoAmount,sizeof(AmmoAmount));
			pStudent->SetAmmoAmountUpdate(RTI::RTI_FALSE);
		}
		if(pStudent->GetCleanlinessUpdate())
		{
			Cleanliness = pStudent->GetCleanliness();
			attrs->add(pStudent->GetCleanlinessRtiId(),(char*)&Cleanliness,sizeof(Cleanliness));
			pStudent->SetCleanlinessUpdate(RTI::RTI_FALSE);
		}
		if(attrs->size())
		{
			cout<<"***update"<<pStudent->GetName()<<endl;
			if(Regulating)
			{
				rtiAmb.queryLookahead(Lookahead);
				rtiAmb.updateAttributeValues(pStudent->GetInstanceId(),
											*attrs,
											CurrentTime+Lookahead,
											"StudentUpdate");
				rtiAmb.tick(0.1,2.0);
			}
			else
			{
				rtiAmb.updateAttributeValues(pStudent->GetInstanceId(),
											*attrs,
											"StudentUpdate");
				ms_rtiAmb->tick(0.1,2.0);
			}
		}
		if(attrs)
		{
			attrs->empty();
			delete attrs;
		}
	}
	catch ( RTI::Exception& e )
    {
		cout << e << endl;
		exit(1);
    }
}

void	Student::ProcessOwnership(Student* myStudent,RTI::Boolean defector,int loop)
{
	cout<<"****ProcessOwnership loop:"<<loop<<endl;

	int i;
	Student* pStudent = NULL;

	/*己方学生发生叛变，实例属性所有权“推”模式转移*/
	if(defector && loop==10)
	{
		int defectorIdx=-1;

		/*查找已方第一个未删除的学生实例
		1：该对象实例在联邦执行中仍然存在
		2：联邦成员拥有对象实例的所有属性所有权
		3：对象实例属性所有权处于未转移状态*/
		for(i=0;i<myStudent->ms_extentCardinality;i++)
		{
			pStudent = myStudent->ms_StudentExtent[i];
			if(!pStudent->GetInstanceDeleted()&&
				pStudent->GetOwnLunchMoney()&&
				!pStudent->GetTransferLunchMoney()&&
				pStudent->GetOwnAmmoAmount()&&
				!pStudent->GetTransferAmmoAmount()&&
				pStudent->GetOwnCleanliness()&&
				!pStudent->GetTransferCleanliness()&&
				pStudent->GetOwnPrivilegeToDelete()&&
				!pStudent->GetTransferPrivilegeToDelete()
				)
			{
				defectorIdx=i;
				break;
			}
		}

		/*请求转让该学生所有实例属性的所有权*/
		if(defectorIdx>-1)
		{
			try
			{
				cout<<"****************************************"<<endl;
				cout<<"****initiating divestiture of"<<pStudent->GetName()<<"STUDENT NUM:"<<defectorIdx<<endl;
				cout<<"****************************************"<<endl;
				RTI::AttributeHandleSet* attrs=NULL;
				try
				{
					attrs = RTI::AttributeHandleSetFactory::create(4);
					attrs->add(pStudent->GetLunchMoneyRtiId());
					attrs->add(pStudent->GetAmmoAmountRtiId());
					attrs->add(pStudent->GetCleanlinessRtiId());
					attrs->add(pStudent->GetPrivilegeToDeleteRtiId());
				}
				catch ( RTI::Exception& e )
				{
					cout << e << endl;
					exit(1);
				}
				rtiAmb.negotiatedAttributeOwnershipDivestiture(
					pStudent->GetInstanceId(),
					*attrs,
					"Divest");
				rtiAmb.tick(0.01,2.0);
				attrs->empty();
				delete attrs;
			}
			catch ( RTI::Exception& e )
			{
				cout << e << endl;
			}
		}
		else
		{
			cout<<"****************************************"<<endl;
			cout<<"****could not find a student to divest!***"<<endl;
			cout<<"****************************************"<<endl;
		}
	}

	/*敌方学生发生叛变，实例属性所有权“拉”模式转移*/
	else if(defector && loop==30)
	{
		int defectorIdx=-1;

		/*查找对方第一个未删除的学生实例
		1：该对象实例在联邦执行中仍然存在
		2：联邦成员不拥有对象实例的所有属性所有权
		3：对象实例属性所有权处于未转移状态*/
		for(i=0;i<myStudent->ms_extentCardinality;i++)
		{
			pStudent = myStudent->ms_StudentExtent[i];
			if(!pStudent->GetInstanceDeleted()&&
				!pStudent->GetOwnLunchMoney()&&
				!pStudent->GetTransferLunchMoney()&&
				!pStudent->GetOwnAmmoAmount()&&
				!pStudent->GetTransferAmmoAmount()&&
				!pStudent->GetOwnCleanliness()&&
				!pStudent->GetTransferCleanliness()&&
				!pStudent->GetOwnPrivilegeToDelete()&&
				!pStudent->GetTransferPrivilegeToDelete())
			{
				defectorIdx=i;
				break;
			}
		}

		/*请求获取该学生所有实例属性的所有权*/
		if(defectorIdx>-1)
		{
			try
			{
				cout<<"****************************************"<<endl;
				cout<<"****initiating acquisition of"<<pStudent->GetName()<<"STUDENT NUM:"<<defectorIdx<<endl;
				cout<<"****************************************"<<endl;
				RTI::AttributeHandleSet* attrs=NULL;
				try
				{
					attrs = RTI::AttributeHandleSetFactory::create(4);
					attrs->add(pStudent->GetLunchMoneyRtiId());
					attrs->add(pStudent->GetAmmoAmountRtiId());
					attrs->add(pStudent->GetCleanlinessRtiId());
					attrs->add(pStudent->GetPrivilegeToDeleteRtiId());
				}
				catch ( RTI::Exception& e )
				{
					cout << e << endl;
					exit(1);
				}
				rtiAmb.attributeOwnershipAcquisition(
					pStudent->GetInstanceId(),
					*attrs,
					"Acquire");
				rtiAmb.tick(0.01,2.0);
				attrs->empty();
				delete attrs;
			}
			catch ( RTI::Exception& e )
			{
				cout << e << endl;
			}
		}
		else
		{
			cout<<"****************************************"<<endl;
			cout<<"****could not find a student to Acquire!***"<<endl;
			cout<<"****************************************"<<endl;
		}
	}

    /*属性所有权转移响应处理*/

	/*“拉”模式属性所有权释放响应，在收到其他联邦成员获取对象属性
	所有权请求后，向请求方发送所有权释放响应*/
	for(i=0;i<myStudent->ms_extentCardinality;i++)
	{
		pStudent = myStudent->ms_StudentExtent[i];

		/*查找所有需要进行释放响应处理的对象实例：
		1：该对象实例在联邦执行中仍然存在
		2：联邦成员拥有对象实例的所有属性所有权
		3：对象实例属性所有权处于转移状态*/
		if(!pStudent->GetInstanceDeleted()&&(
			(pStudent->GetOwnLunchMoney()&&
			 pStudent->GetTransferLunchMoney())||
			(pStudent->GetOwnAmmoAmount()&&
			 pStudent->GetTransferAmmoAmount())||
			(pStudent->GetOwnCleanliness()&&
			 pStudent->GetTransferCleanliness())||
			(pStudent->GetOwnPrivilegeToDelete()&&
			 pStudent->GetTransferPrivilegeToDelete())))
		{
		try
			{
				cout<<"****************************************"<<endl;
				cout<<"****initiating release of"<<pStudent->GetName()<<endl;
				cout<<"****************************************"<<endl;
				RTI::AttributeHandleSet* attrs=NULL;
				try
				{
					attrs = RTI::AttributeHandleSetFactory::create(4);
					if(pStudent->GetTransferLunchMoney())
					{
						attrs->add(pStudent->GetLunchMoneyRtiId());
						pStudent->SetTransferLunchMoney(RTI::RTI_FALSE);
						cout<<"*** TransferLunchMoney=FALSE"<<endl;
					}
					if(pStudent->GetTransferAmmoAmount())
					{
						attrs->add(pStudent->GetAmmoAmountRtiId());
						pStudent->SetTransferAmmoAmount(RTI::RTI_FALSE);
						cout<<"*** TransferAmmoAmount=FALSE"<<endl;
					}
					if(pStudent->GetTransferCleanliness())
					{
						attrs->add(pStudent->GetCleanlinessRtiId());
						pStudent->SetTransferCleanliness(RTI::RTI_FALSE);
						cout<<"*** TransferCleanliness=FALSE"<<endl;
					}
					if(pStudent->GetTransferPrivilegeToDelete())
					{
						attrs->add(pStudent->GetPrivilegeToDeleteRtiId());
						pStudent->SetTransferPrivilegeToDelete(RTI::RTI_FALSE);
						cout<<"*** TransferPrivilegeToDelete=FALSE"<<endl;
					}
					
				}
				catch ( RTI::Exception& e )
				{
					cout << e << endl;
					exit(1);
				}
				rtiAmb.negotiatedAttributeOwnershipDivestiture(
					pStudent->GetInstanceId(),
					*attrs,
					"Release");
				rtiAmb.tick(0.01,2.0);
				attrs->empty();
				delete attrs;
			}
			catch ( RTI::Exception& e )
			{
				cout << e << endl;
			}
		}
	}

	/*“推”模式属性所有权请求响应，实例属性所有权为其他联邦成员拥有，
	其他联邦成员发出释放请求，本联邦发送希望获取属性所有权请求*/
	for(i=0;i<myStudent->ms_extentCardinality;i++)
	{
		pStudent = myStudent->ms_StudentExtent[i];

		/*查找所有需要进行获取响应处理的对象实例：
		1：该对象实例在联邦执行中仍然存在
		2：联邦成员未拥有对象实例的所有属性所有权
		3：对象实例属性所有权处于转移状态*/
		if(!pStudent->GetInstanceDeleted()&&
		   ((!pStudent->GetOwnLunchMoney()&&
			 pStudent->GetTransferLunchMoney())||
			(!pStudent->GetOwnAmmoAmount()&&
			 pStudent->GetTransferAmmoAmount())||
			(!pStudent->GetOwnCleanliness()&&
			 pStudent->GetTransferCleanliness())||
			(!pStudent->GetOwnPrivilegeToDelete()&&
			 pStudent->GetTransferPrivilegeToDelete())))
		{
			try
			{
				cout<<"****************************************"<<endl;
				cout<<"****attempting acquire of"<<pStudent->GetName()<<endl;
				cout<<"****************************************"<<endl;
				RTI::AttributeHandleSet* attrs=NULL;
				try
				{
					attrs = RTI::AttributeHandleSetFactory::create(4);
					if(pStudent->GetTransferLunchMoney())
					{
						attrs->add(pStudent->GetLunchMoneyRtiId());
						pStudent->SetTransferLunchMoney(RTI::RTI_FALSE);
						cout<<"*** TransferLunchMoney=FALSE"<<endl;
					}
					if(pStudent->GetTransferAmmoAmount())
					{
						attrs->add(pStudent->GetAmmoAmountRtiId());
						pStudent->SetTransferAmmoAmount(RTI::RTI_FALSE);
						cout<<"*** TransferAmmoAmount=FALSE"<<endl;
					}
					if(pStudent->GetTransferCleanliness())
					{
						attrs->add(pStudent->GetCleanlinessRtiId());
						pStudent->SetTransferCleanliness(RTI::RTI_FALSE);
						cout<<"*** TransferCleanliness=FALSE"<<endl;
					}
					if(pStudent->GetTransferPrivilegeToDelete())
					{
						attrs->add(pStudent->GetPrivilegeToDeleteRtiId());
						pStudent->SetTransferPrivilegeToDelete(RTI::RTI_FALSE);
						cout<<"*** TransferPrivilegeToDelete=FALSE"<<endl;
					}
					
				}
				catch ( RTI::Exception& e )
				{
					cout << e << endl;
					exit(1);
				}
				rtiAmb.attributeOwnershipAcquisition(
					pStudent->GetInstanceId(),
					*attrs,
					"defector");
				rtiAmb.tick(0.01,2.0);
				attrs->empty();
				delete attrs;
			}
			catch ( RTI::Exception& e )
			{
				cout << e << endl;
			}
		}
	}
}

void Student::SetName( const char* name)
{
    cout<<"****SetName()"<<endl;

   if (ms_name != name)
   {
     // Delete the existing memory
     delete [] ms_name;
     
     // Allocate appropriate size string and copy data
     if ( name && strlen( name ) > 0 )
     {
       ms_name = new char[ strlen(name) + 1 ];
       strcpy( ms_name, name );
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

Student* Student::Find( RTI::ObjectHandle objectId )
{
   cout<<"****Find()"<<endl;

   Student *pStudent = NULL;

   for (int i = 0; i < Student::ms_extentCardinality; i++ )
   {
      pStudent = Student::ms_StudentExtent[i];

      if ( pStudent && pStudent->GetInstanceId() == objectId )
	  {
		break;
      } 
	  else 
	  {
		pStudent = NULL;
      }
   }

   return pStudent;
}

void Student::SetLunchMoney(const RTI::Double LunchMoney)
{
	cout<<"****SetLunchMoney()"<<endl;

	m_LunchMoney=LunchMoney;
}
void Student::SetAmmoAmount(const RTI::ULong AmmoAmount)
{
	cout<<"****SetAmmoAmount()"<<endl;

	m_AmmoAmount=AmmoAmount;
}
void Student::SetCleanliness(const RTI::Double Cleanliness)
{
	cout<<"****SetCleanliness()"<<endl;

	m_Cleanliness=Cleanliness;
}
void Student::SetEnsuingMess(const RTI::Double EnsuingMess)
{
	cout<<"****SetEnsuingMess()"<<endl;

	m_EnsuingMess=EnsuingMess;
}

void Student::SetUpdateControl(RTI::Boolean status,const RTI::AttributeHandleSet& theAttrHandles)
{   
		cout<<"****SetUpdateControl()"<<endl;

        RTI::AttributeHandle attrHandle;   
  
        for (unsigned int i = 0; i < theAttrHandles.size(); i++)   
        {   
                attrHandle = theAttrHandles.getHandle(i);   
                if (attrHandle==Student::GetLunchMoneyRtiId())   
                {   
                        ms_LunchMoneyUpdate = status;   
  
                        const char *pStr = ms_LunchMoneyUpdate ? "ON" : "OFF";   
                        cout << "FED_HW: Turning Student. LunchMoney Updates "  
                                 << pStr <<" for object " << this->GetInstanceId()   
                                 << ". "<<endl;   
                }    
                else if(attrHandle == Student::GetAmmoAmountRtiId())   
                {   
                        ms_AmmoAmountUpdate = status;   
  
                        const char *pStr = ms_AmmoAmountUpdate ? "ON" : "OFF";   
                        cout << "FED_HW: Turning Student. AmmoAmount Updates "  
                                 << pStr <<" for object " << this->GetInstanceId()   
                                 << ". "<<endl;   
                           
                } 
				else if(attrHandle == Student::GetCleanlinessRtiId())   
                {   
                        ms_CleanlinessUpdate = status;   
  
                        const char *pStr = ms_CleanlinessUpdate ? "ON" : "OFF";   
                        cout << "FED_HW: Turning Student. Cleanliness Updates "  
                                 << pStr <<" for object " << this->GetInstanceId()   
                                 << ". "<<endl;   
                           
                }   
        }   
}
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
ostream& operator << ( ostream &s, Student &v )
{
	cout<<"****ostream& operator"<<endl;

   const char* name = v.GetName();
   if (name == 0)
           name = "(unknown)";

   s << "Name: " << name
     << " LunchMoney: " << v.GetLunchMoney()
	 << " AmmoAmount: " << v.GetAmmoAmount()
	 << " Cleanliness: " << v.GetCleanliness()
	 << " EnsuingMess: " << v.GetEnsuingMess();

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
ostream &operator << ( ostream &s, Student *v ) 
{
   if ( !v )
      return s;
   else
   {
      s << *v;
   }

   return s;
}

/*己方第一个学生购买弹药攻打敌方第一个学生*/
void	Student::Shoot(Student* myStudent,Splat* mySplat)
{
		cout<<"****Shoot()"<<endl;

		int i;
		Student* pStudent = NULL;
		Student* pStudent2 = NULL;
		int defectorIdx=-1;

		/*查找已方学生队列中的第一个学生*/
		for(i=0;i<myStudent->ms_extentCardinality;i++)
		{
			pStudent = myStudent->ms_StudentExtent[i];
			if(!pStudent->GetInstanceDeleted()&&
				pStudent->GetOwnLunchMoney()&&
				!pStudent->GetTransferLunchMoney()&&
				pStudent->GetOwnAmmoAmount()&&
				!pStudent->GetTransferAmmoAmount()&&
				pStudent->GetOwnCleanliness()&&
				!pStudent->GetTransferCleanliness()&&
				pStudent->GetOwnPrivilegeToDelete()&&
				!pStudent->GetTransferPrivilegeToDelete()
				)
			{
				defectorIdx=i;
				break;
			}
		}

		/*己方还有存活的学生，则开火攻击*/
		if(defectorIdx > -1)
		{
			cout<<"***查找已方第一个学生队列中的学生:"<<pStudent->GetName()<<"学生号:"<<defectorIdx<<endl;
			if(pStudent->GetLunchMoney() >=1.0)
			{
				pStudent->SetLunchMoney(0);
				pStudent->SetAmmoAmount(10);//购买弹药
				return;
			}

			else
			{
				if(pStudent->GetAmmoAmount() > 0)
				{
					int defectorIdx=-1;
					
					/*查找对方第一个学生队列中的学生*/
					for(i=0;i<myStudent->ms_extentCardinality;i++)
					{
						pStudent2 = myStudent->ms_StudentExtent[i];
						if(!pStudent2->GetInstanceDeleted()&&
							!pStudent2->GetOwnLunchMoney()&&
							!pStudent2->GetTransferLunchMoney()&&
							!pStudent2->GetOwnAmmoAmount()&&
							!pStudent2->GetTransferAmmoAmount()&&
							!pStudent2->GetOwnCleanliness()&&
							!pStudent2->GetTransferCleanliness()&&
							!pStudent2->GetOwnPrivilegeToDelete()&&
							!pStudent2->GetTransferPrivilegeToDelete())
						{
							defectorIdx=i;
							break;
						}
					}
					if(defectorIdx > -1)
					{
						cout<<"***查找对方第一个学生队列中的学生:"<<pStudent2->GetName()<<"学生号:"<<defectorIdx<<endl;
						mySplat->Fire(pStudent2,pStudent->GetEnsuingMess()); //发送开火交互实例
						myStudent->StudentUpdate(pStudent);
					}
					else
					{
						cout<<"no find enemy alive student"<<endl;
						return;
					}
				}
				else
				{
					return;
				}
			}
		}
		else
		{
			cout<<"no find our alive student"<<endl;
			return;
		}
}