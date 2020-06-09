#pragma once
#include "CoreMinimal.h"
#include "../base/common.h"
#include "../CabinetShell.h"
#include "../base/FrameEdge.h"
#include "../CabinetGlobal.h"
#include "../CabinetArea.h"

struct TSetEdge: public PolyEdgeLib::TSegment
{
	virtual bool  Init();
	int32 mId;
	int32 mLen;
	int32 mHead;
	int32 mTail;
	int32 mSetRet;
	FString	mErrorMsg;
	TArray<FCabinetSet> mCabinets;
	bool MergeNext();
	bool MergePre();
	TSetEdge* PreSeg() { return (TSetEdge*)mPreSeg; }
	TSetEdge* NextSeg() { return (TSetEdge*)mNextSeg; }

	bool  ReSetClear();
	bool  ReSetClearNormal();
	int32 GetRemainLen() { return mTail - mHead; }
	int32 ReseatRemain() { mHead = 0; mTail = mLen; return (mTail - mHead); }
	int32 ReseatRemain(int nHead, int nTail) { mHead = nHead; mTail = nTail; return (mTail - mHead); }
	//
	bool AdjustRemainSize();
	int32  AdjustCabinets();
	bool AdjustCabinets(TArray<int32>& Cabinets,int nBaseSize, int nTotalLen);
	FCabinetSet& SetCabinetPos(FCabinetSet& Slot);

	FCabinetSet& SetCabinetPos(FCabinetSet::eAlignType eAlign, FCabinetSet::eFillType eFillType, FCabinetRes Res, float fScaleX = 1.0, float fScaleY = 1.0);
	FCabinetSet& SetCabinetVer(FCabinetSet::eAlignType eAlign, FCabinetRes Res, int32 XLen);
	bool SetCabinetsPos();

	bool FillDist(float len, bool bHead);
	bool FillFake(float len, bool bHead);
	bool FillFake(float len, FCabinetSet::eAlignType eAlign);
	bool SetFillDist(float len, bool bHead);
	//
	float HeadStateDist(uint32 nBit, uint32 nExpactBit = 0);
	float TailStateDist(uint32 nBit, uint32 nExpactBit = 0);
	bool  AdjustTurnDir(FCabinetSet& Cabinet);
	int32 GetCabinet(FCabinetSet::eAlignType eAlign);
};
inline bool IsTurn(eCornerType eType){	return (eType == eCorner90 || eType ==eCorner270);}