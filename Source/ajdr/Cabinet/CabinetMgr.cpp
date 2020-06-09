
#include "CabinetMgr.h"
#include "CabinetArea.h"
//#include "Actor/CabinetNode.h"

UCabinetMgr *UCabinetMgr::s_CabinetMgr = nullptr;

UCabinetMgr::UCabinetMgr(const FObjectInitializer &ObjectInitilizer)
	: Super(ObjectInitilizer)
{
	mIdx = 0;
	mSplineIdx = 0;
}

UCabinetMgr *UCabinetMgr::Get()
{
	return s_CabinetMgr;
}
UCabinetMgr& UCabinetMgr::Instance()
{
	check(s_CabinetMgr);
	return *s_CabinetMgr;
}

UWorld *UCabinetMgr::GetWorld() const
{
	UObject *Outer = GetOuter();
	return Outer ? Outer->GetWorld() : nullptr;
}

UCabinetMgr *UCabinetMgr::CreateInstance(UObject *Outer)
{
	check(!s_CabinetMgr);
	s_CabinetMgr = NewObject<UCabinetMgr>(Outer,TEXT("CabinetMgr"));
	s_CabinetMgr->AddToRoot();
	return s_CabinetMgr;
}

bool UCabinetMgr::Init()
{
	return true;
}

void UCabinetMgr::UnInit()
{
	if (s_CabinetMgr)
	{
		gCabinetAreaMgr.Clear();
		s_CabinetMgr->RemoveFromRoot();
		s_CabinetMgr->MarkPendingKill();
		s_CabinetMgr = nullptr;
	}
}
ASplineActor* UCabinetMgr::NewSpline()
{
	ASplineActor* pSline = nullptr;
	UWorld *MyWorld = GetWorld();
	if (MyWorld)
	{
		pSline = MyWorld->SpawnActor<ASplineActor>(ASplineActor::StaticClass());
		pSline->mId = ++mSplineIdx;
	}
	return pSline;
}

ATableActor* UCabinetMgr::NewTable()
{
	ATableActor* pTable = nullptr;
	UWorld *MyWorld = GetWorld();
	if (MyWorld)
	{
		pTable = MyWorld->SpawnActor<ATableActor>(ATableActor::StaticClass());
	}
	return pTable;
}