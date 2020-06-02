#pragma once

#include "IBuildingPlugin.h"

#define PillarType PLUGIN_SECTION_0+2

class Pillar :public IPluginObject
{
public:
	int GetType() { return PillarType; }
	unsigned int GetVersion() { return 0; }
	IValue* GetFunctionProperty(const char *name) override;
	bool SetFunctionProperty(const char *name, const IValue *Value) override;
	void Serialize(ISerialize &Ar, unsigned int Ver) override;

	void SetWidth(float width);
	float GetWidth();

	void SetLength(float length);
	float GetLength();

	void SetHeight(float height);
	float GetHeight();

	void SetOrigAngle(float angle);
	float GetOrigAngle();

	/*void SetCurWallAngle(float curAngle);
	float GetCurWallAngle();

	void SetIsWidth(bool close);
	bool GetIsWidth();*/

	void SetIsAngle(bool isSet);
	bool GetIsAngle();

	void SetSize(kVector3D size);
	kVector3D GetSize();

	void SetDefaultLoc(kPoint DefLoc);
	kPoint GetDefaultLoc();

	/*void SetAnchor(bool IsAnch);
	bool GetAnchor();*/

	void SetLocation(kPoint loc);
	kPoint GetLocation();

	/*void SetWallForw(kPoint WalllForw);
	kPoint GetWallForw();

	void SetWallID(std::vector<int> wallId);
	std::vector<int> GetWallId();*/

	void SetOrigBorder(std::vector<kVector3D> OrigBorderArr);
	std::vector<kVector3D> GetOrigBorder();

	void SetBorder(std::vector<kVector3D> BorderArr);
	std::vector<kVector3D> GetBorder();

	/*void SetPillarSurfArea(float area);
	float GetPillarArea();

	void SetWallArea(kPoint wallArea);
	kPoint GetWallArea();

	void SetDirection(kPoint dir);
	kPoint GetDirection();*/
private:
	int PillarID;
	float Width;
	float Height;
	float Length;
	float OrigAngle;
	float CurAngle;
	bool IsAnchor;
	kPoint Location;
	std::vector<int> WallIDs;
	float PillarSurfArea;
	kPoint PillarDirVect;
	kPoint WallArea;
	kVector3D DefaultSize;
	kPoint DefaultLoc;
	kPoint CurWallForward;
	std::vector<kVector3D> OrigBorderVer;
	std::vector<kVector3D> BorderVertex;
	bool IsWidth;
	bool IsAngle;
};

class PillarDesc : public IPluginObjectDesc
{
public:
	bool IsResource() { return false; }
	const char *GetObjectClassName() { return "Pillar"; }
	int GetObjectType() { return PillarType; }
	IPluginObject *CreateObject() override;
	void DestroyObject(IPluginObject *pObj) override;
};