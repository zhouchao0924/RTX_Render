
#pragma once

#include "IClass.h"
#include "IMeshObject.h"

enum EKeyCodes
{
	// key board
	EKey_0,
	EKey_1,
	EKey_2,
	EKey_3,
	EKey_4,
	EKey_5,
	EKey_6,
	EKey_7,
	EKey_8,
	EKey_9,
	EKey_A,
	EKey_B,
	EKey_C,
	EKey_D,
	EKey_E,
	EKey_F,
	EKey_G,
	EKey_H,
	EKey_I,
	EKey_J,
	EKey_K,
	EKey_L,
	EKey_M,
	EKey_N,
	EKey_O,
	EKey_P,
	EKey_Q,
	EKey_R,
	EKey_S,
	EKey_T,
	EKey_U,
	EKey_V,
	EKey_W,
	EKey_X,
	EKey_Y,
	EKey_Z,
	EKey_F1,
	EKey_F2,
	EKey_F3,
	EKey_F4,
	EKey_F5,
	EKey_F6,
	EKey_F7,
	EKey_F8,
	EKey_F9,
	EKey_F10,
	EKey_F11,
	EKey_F12,
	EKey_ALT,
	EKey_ESC,
	EKey_SHIFT,
	EKey_CTRL,
	EKey_SPACE,
	EKey_DEL,

	EKey_BACK,
	EKey_MENU,

	EKey_None,

	//mouse
	EKey_LMOUSE,
	EKey_RMOUSE,
	EKey_MMOUSE,
	EKey_MOUSE,

	//touch
	EKey_TOUCHBEGAN,
	EKey_TOUCHMOVED,
	EKey_TOUCHENDED,
	EKey_TOUCHCANCELLED,

	//acceleration
	EKey_Acceleration,

	//Occupy
	EKey_OCCUPY,

	//char
	EKey_CHAR,

	//mouse tii
	EKey_MAX,

	//pad
	EKey_DPAD_DOWN,
	EKey_DPAD_UP,
	EKey_DPAD_LEFT,
	EKey_DPAD_RIGHT,
};

enum EKeyState
{
	EKSMousePressed,
	EKSMouseReleased,
	EKSKeyPressed,
	EKSKeyReleased,
	EKSMouseMoving,
};

enum ECameraMode
{
	ECamera_Fly,
	ECamera_2D
};

class IEditMode;
class IBuildingEditor;
typedef std::function<void()> LifeFuncType;
typedef std::function<void(IBuildingEditor *, EKeyCodes, EKeyState)> InputFuncType;

class IOperation
{
public:
	virtual const char *GetName() = 0;
	virtual IEditMode  *GetMode() = 0;
	virtual IBuildingEditor *GetEditor() = 0;
	virtual void RegisterFunction(InputFuncType FuncKey, InputFuncType MouseKey, LifeFuncType ActiveFunc, LifeFuncType DeactiveFunc) = 0;
};

class IEditMode
{
public:
	virtual const char *GetName() = 0;
	virtual IBuildingEditor *GetEditor() = 0;
	virtual void RegisterOperation(IOperation *Operation) = 0;
	virtual void RegisterFunction(InputFuncType FuncKey, InputFuncType MouseKey, LifeFuncType ActiveFunc, LifeFuncType DeactiveFunc) = 0;
	virtual IOperation *ActiveOperation(const char *InType) = 0;
	virtual IOperation *GetOperation(const char *InType) = 0;
	virtual void CancleOperation() = 0;
};

class ISuite;
class IBuildingEditor
{
public:
	class Dependency
	{
	public:
		virtual void Draw(IObject *Obj, bool bSelected) = 0;
		virtual kPoint GetWorldPositionAtCursor() = 0;
		virtual void OnSelectedChanged(ObjectID ObjID, bool bSelected) = 0;
		virtual void DrawLine(kPoint *Line, int Num, kColor Color, float Z) = 0;
		virtual void DrawLine(kVector3D *Line, int Num, kColor Color, bool bTransient) = 0;
		virtual void DrawLine(const kVector3D &Start, const kVector3D &End, kColor Color, bool bTransient) = 0;
		virtual void DrawDoor(bool bSelected, const kPoint &Loc, const kPoint &Forward, const kPoint &Right, float Width, float Z) = 0;
		virtual void DrawWindow(bool bSelected, const kPoint &Loc, const kPoint &Forward, const kPoint &Right, float Width, float Z) = 0;
		virtual void DrawCornerWindow(bool bSelected, const kPoint &Location, IObject *pWall0, IObject *pWall1, float Width0, float Width1, float Z) = 0;
		virtual bool HitTest3DByCursor(kVector3D &OutLocation, kVector3D &OutNormal, ObjectID &OutObjID) = 0;
	};

	virtual ISuite *GetSuite() = 0;
	virtual void SetSuite(ISuite *Suite) = 0;
	virtual bool IsKeyDown(EKeyCodes Key) = 0;
	virtual void InputKey(EKeyCodes Key, EKeyState State) = 0;
	virtual void InputMouse(EKeyCodes Key) = 0;
	virtual kArray<ObjectID> GetSelected() = 0;
	virtual bool IsSelected(ObjectID ObjID) = 0;
	virtual void Draw() = 0;

	virtual void DrawDotLine(const kPoint &Start, const kPoint &End, kColor Color, float Z = 0) = 0;
	virtual void DrawBox(const kPoint &Center, const kPoint &HalfExt, const kColor &Color, float Z = 0) = 0;
	virtual void DrawCircle(const kPoint &Center, float Radius, int NumSegment, const kColor &Color, float Z = 0) = 0;
	virtual void DrawArc(const kPoint &Center, const kPoint &Forward, const kPoint &Right, float Radius, float StartRad, float EndRad, int NumSegment, const kColor &Color, float Z = 0) = 0;
	virtual void DrawBox3D(const kBox3D &Box, const kColor &Color, bool bTransient) = 0;

	virtual kVector3D GetWorldPositionAtMouse() = 0;
	virtual void SetMode(const char *Mode) = 0;
	virtual IEditMode  *GetActiveMode() = 0;
	virtual bool SetOperation(const char *Operation)  = 0;
	virtual void SetDependency(Dependency *Dep) = 0;
	virtual void SetPaintModel(const char *ResID) = 0;
};


