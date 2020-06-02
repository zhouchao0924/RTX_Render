
#pragma once

#include "IBuildingPlugin.h"

#define  PoleType  PLUGIN_SECTION_0

class Pole : public IPluginObject
{
public:
	int GetType() { return PoleType; }
	unsigned int GetVersion() { return 0; }
	IValue *GetFunctionProperty(const char *name) override;
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

	void SetLocation(kPoint loc);
	kPoint GetLocation();

	/*void SetWallForw(kPoint WalllForw);
	kPoint GetWallForw();

	void SetWallID(std::vector<int> WallArr);
	std::vector<int> GetWallId();*/

	void SetOrigBorder(std::vector<kVector3D> OrigBorderArr);
	std::vector<kVector3D> GetOrigBorder();

	void SetBorder(std::vector<kVector3D> BorderArr);
	std::vector<kVector3D> GetBorder();

	/*void SetPoleSurfArea(float area);
	float GetPoleArea();

	void SetWallArea(kPoint wallArea);
	kPoint GetWallArea();

	void SetDirection(kPoint dir);
	kPoint GetDirection();*/

	void SetMaterialUris(std::vector<std::string> MatUris);
private:
	int PoleID;
	float Width;
	float Length;
	float Height;
	float OrigAngle;
	float CurAngle;
	kVector3D DefaultSize;
	kPoint DefaultLoc;
	kPoint Forward;
	kPoint Location;
	kPoint CurWallForward;
	std::vector<int> WallIDs;
	float FlueSurfArea;
	kPoint WallArea;
	std::vector<kVector3D> OrigBorderVer;
	std::vector<kVector3D> BorderVertex;
	bool IsWidth;
	bool IsAngle;

	//define vector memory section material data
	std::vector<int> SectionIndex;
	std::vector<std::string> SectMatUris;
};

class PoleDesc :public IPluginObjectDesc
{
public:
	bool IsResource() { return false; }
	const char *GetObjectClassName() { return "Pole";  }
	int	 GetObjectType() override { return PoleType; }
	IPluginObject *CreateObject()  override;
	void DestroyObject(IPluginObject *pObj) override;
};


