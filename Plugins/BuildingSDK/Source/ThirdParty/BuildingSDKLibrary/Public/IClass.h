
#pragma once

#include "ISerialize.h"
#include "IProperty.h"
#include "Math/kXform.h"
#include "Math/kVector3D.h"
#include <functional>
#include "ObjectTypes.h"
#include "IModifier.h"
#include "ISceneManager.h"

#define  INVALID_OBJID -1
typedef	 int  ObjectID;
class IMeshObject;
class IObject;
class IPluginObjectDesc;

class IObjectDesc
{
public:
	virtual bool IsA(int Type) = 0;
	virtual const char *GetObjectClassName() = 0;
	virtual void AddProperty(const char *InName, IProperty *Prop, bool bReadOnly) = 0;
	virtual void AddProperty(const char *InName, EVarType VarType, const PropertyFunctor &functor, bool bReadOnly) = 0;
	virtual int GetNumberOfProperty() = 0;
	virtual class IProperty *GetProperty(int index) = 0;
	virtual class IProperty *GetProperty(const char *Propname) = 0;
	virtual IObjectDesc *GetSuperDesc() = 0;
	virtual IObject *CreateObject() = 0;
	virtual void DestroyObject(IObject *pObj) = 0;
	virtual EObjectType GetObjectType() = 0;
	virtual IPluginObjectDesc *GetPluginObjectDesc() = 0;
};

class IAttributable
{
public:
	virtual int GetNumberOfProperties() = 0;
	virtual IProperty *GetProperty(int Index) = 0;
	virtual IProperty *GetProperty(const char  *Name) = 0;
	virtual IValue &GetPropertyValue(const char  *Name) = 0;
	virtual void SetPropertyValue(const char *Name, const IValue *Value) = 0;
	
	virtual void SetPropertyInt(const char *name, int value) = 0;
	virtual void SetPropertyFloat(const char *name, float value) = 0;
	virtual void SetPropertyBool(const char *name, bool value) = 0;
	virtual void SetPropertyColor(const char *name, kColor value) = 0;
	virtual void SetPropertyVector2D(const char *name, const kPoint &value) = 0;
	virtual void SetPropertyVector3D(const char *name, const kVector3D &value) = 0;
	virtual void SetPropertyVector4D(const char *name, const kVector4D &value) = 0;
	virtual void SetPropertyRotator(const char *name, const kRotation &value) = 0;
	virtual void SetPropertyBounds(const char *name, const kBox3D &value) = 0;
	virtual void SetPropertyIntArray(const char *name, const kArray<int> &value) = 0;
	virtual void SetPropertyVector2DArray(const char *name, const kArray<kPoint> &value) = 0;
	virtual void SetPropertyVector3DArray(const char *name, const kArray<kVector3D> &value) = 0;
	virtual void SetPropertyPlane(const char *name, const kPlane3D &value) = 0;
	virtual void SetPropertyString(const char *name, const char *value) = 0;

	virtual int GetPropertyInt(const char *name, int defValue = 0) = 0;
	virtual float GetPropertyFloat(const char *name, float defValue = 0) = 0;
	virtual bool GetPropertyBool(const char *name, bool defValue = false) = 0;
	virtual kColor GetPropertyColor(const char *name, kColor defValue = kColor(0)) = 0;
	virtual kPoint GetPropertyVector2D(const char *name, const kPoint &defValue = kPoint(0)) = 0;
	virtual kVector3D GetPropertyVector3D(const char *name, const kVector3D &defValue = kVector3D(0)) = 0;
	virtual kVector4D GetPropertyVector4D(const char *name, const kVector4D &defValue = kVector4D(0)) = 0;
	virtual kRotation GetPropertyRotator(const char *name, const kRotation &defValue = kRotation(0,0,0)) = 0;
	virtual kBox3D GetPropertyBounds(const char *name, const kBox3D &defValue = kBox3D()) = 0;
	virtual kArray<int> GetPropertyIntArray(const char *name) = 0;
	virtual kArray<kPoint> GetPropertyVector2DArray(const char *name) = 0;
	virtual kArray<kVector3D> GetPropertyVector3DArray(const char *name) = 0;
	virtual kPlane3D GetPropertyPlane(const char *name, const kPlane3D &defValue = kPlane3D()) = 0;
	virtual const char *GetPropertyString(const char *name) = 0;
};

enum ETexSlot
{
	eDiffuse,
	eSpecular,
	eEmissive,
	eRoughness,
	eMetallic,
	eOpacity,
	eNormal,
	eMax
};

enum ESourceImageFormat
{
	Image_A8R8G8B8 = 0,
	Image_A8 = 7,
	Image_PNG = 8,
	Image_TGA = 9,
	Image_JPG = 10,
	Image_Unkown,
};

enum EChannelMask
{
	EChannelNone = 0x0,
	EChannelGeometry = 0x1,
	EChannelSurface = 0x2,
	EChannelTransform = 0x4,
	EChannelUserData = 0x8,
	EChannelAll = 0xffffffff,
};

class INode;
class IGroup;
class IObject;
class IResource;
class IMeshObject;
class IObjectFactory;
class ISurfaceObject;
class IPluginObject;
class IMeshBuilder;
class ISurfaceTexture;
class IPluginObjectDesc;

class IObjectProxy
{
public:
	virtual ~IObjectProxy() {}
	virtual void OnAddObject(IObject *RawObj) = 0;
	virtual void OnDeleteObject(IObject *RawObj) = 0;
	virtual void OnUpdateObject(IObject *RawObj, unsigned int ChannelMask) = 0;
	virtual void OnUpdateSurfaceValue(IObject *RawObj, int SectionIndex, ObjectID Surface) = 0;
};

enum EFuncType
{
	ELoaded,
	EAddToScene,
	EUpdate,
	EUpdateGroup,
	EUpdateResource,
	EUpdateVisible,
	EUpdateTransform,
	EUpdateMaterial,
	ERemoveFromScene
};

typedef std::function<void(IObject *, EFuncType, IValue *)> FuncChangedCallback;

class IWatcherPtr
{
public:
	virtual void Link(IObject *Obj) = 0;
	virtual void Unlink() = 0;
	virtual void Destroy() = 0;
	virtual IObject *Get() = 0;
	virtual IObject *Get() const = 0;
	virtual void SetWatcher(FuncChangedCallback Func) = 0;
};

class IObject :public IAttributable
{
public:
	virtual ~IObject(){ }
	
	virtual ObjectID	  GetID() = 0;
	virtual void		  SetID(ObjectID ID) = 0;
	virtual IWatcherPtr   *GetWatcher() = 0;
	virtual class ISuite  *GetSuite() = 0;
	virtual IPluginObject *GetPlugin() = 0;
	virtual void *Query(const char *Name) = 0;
	virtual bool IsA(EObjectType Type) = 0;
	virtual bool IsRoot() = 0;
	virtual void Serialize(ISerialize &Ar, unsigned int Ver) = 0;
	virtual unsigned int GetVersion() = 0;
	virtual bool IsDeletable() = 0;
	virtual void Delete() = 0;

	virtual IValue &Call(const char *FunctionName, IValue &ParamArrays) = 0;
	virtual void NotifyWatcher(EFuncType FuncType, IValue *pValue = nullptr) = 0;
	virtual void OnPostCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void MarkFlags(unsigned int Mark) = 0;
	virtual bool HasFlag(unsigned int Mark) = 0;
	virtual kArray<const char*> GetResDependence() = 0;

	virtual int  GetMeshCount() = 0;
	virtual int	 GetMeshIndex(IMeshObject *MeshObj) = 0;
	virtual IMeshObject *GetMeshObject(int MeshIndex = 0, bool bBuildIfNotExist = true) = 0;
	virtual IMeshObject *GetMeshObject(const char *Name, bool bBuildIfNotExist = true) = 0;

	virtual int	GetSurfaceCount() = 0;
	virtual ISurfaceObject *GetSurfaceObject(int SurfaceIndex) = 0;	
	virtual ISurfaceObject *GetSurfaceObject(const char *Name) = 0;
	virtual ObjectID SetSurfaceUri(IMeshObject *Mesh, int MaterialType, const char *Uri, int SectionIndex) = 0;
	virtual void SetSurface(IMeshObject *Mesh, ObjectID SurfaceID, int SectionIndex) = 0;

	virtual int   GetTextureCount() = 0;
	virtual ISurfaceTexture *GetTexture(int iTex) = 0;

	virtual bool  HitTest(const kVector3D &RayStart, const kVector3D &RayDir, ObjectID &OutSurfaceID, int *OutSectionIndex = nullptr, kVector3D *OutPosition = nullptr, kVector3D *OutNormal = nullptr, float RayLen = 100000.0f) = 0;
	virtual EObjectType GetType() = 0;
	virtual IObjectDesc *GetDesc() = 0;
	virtual bool Move(const kPoint &DeltaMove) = 0;
	virtual bool GetMeshIn2DSpace(IMeshObject *&meshObj, int &SectionIndex) = 0;

	virtual IValue *GetValueArray() = 0;
	virtual IValue *FindValue(const char *name) = 0;
	virtual void SetValue(const char *name, IValue *value) = 0;
	
	virtual bool MarkNeedUpdate(EChannelMask Mask = EChannelAll) = 0;
	virtual void SetProxy(IObjectProxy *Proxy) = 0;
	virtual IObjectProxy *GetProxy() = 0;

	virtual IObject *GetParent() = 0;
	virtual int GetNumberOfChild() = 0;
	virtual IObject *GetChild(int ChildIndex) = 0;
 	virtual kXform GetTransform() = 0;
	virtual void SetTransform(const kXform &Transform) = 0;

	virtual IModifierStack *GetModifierStack() = 0;
	virtual IObjectFactory *GetObjFactory() = 0;
	virtual IResource	 *GetResource() = 0;
	virtual IMeshBuilder *BeginMesh() = 0;
	virtual kBox3D	GetBoundingBox() = 0;
	virtual INode  *GetNode() = 0;
	virtual IGroup *GetGroup() = 0;
	virtual SceneID GetSceneID() = 0;

	virtual void SetInt(const char *name, int value) = 0;
	virtual void SetFloat(const char *name, float value) = 0;
	virtual void SetBool(const char *name, bool value) = 0;
	virtual void SetColor(const char *name, kColor value) = 0;
	virtual void SetVector2D(const char *name, const kPoint &value) = 0;
	virtual void SetVector3D(const char *name, const kVector3D &value) = 0;
	virtual void SetVector4D(const char *name, const kVector4D &value) = 0;
	virtual void SetRotator(const char *name, const kRotation &value) = 0;
	virtual void SetBounds(const char *name, const kBox3D &value) = 0;
	virtual void SetIntArray(const char *name, const kArray<int> &value) = 0;
	virtual void SetVector2DArray(const char *name, const kArray<kPoint> &value) = 0;
	virtual void SetVector3DArray(const char *name, const kArray<kVector3D> &value) = 0;
	virtual void SetPlane(const char *name, const kPlane3D &value) = 0;
	virtual void SetString(const char *name, const char *value) = 0;

	virtual int GetInt(const char *name, int defValue = 0) = 0;
	virtual float GetFloat(const char *name, float defValue = 0) = 0;
	virtual bool GetBool(const char *name, bool defValue = false) = 0;
	virtual kColor GetColor(const char *name, kColor defValue = kColor(0)) = 0;
	virtual kPoint GetVector2D(const char *name, const kPoint &defValue = kPoint(0)) = 0;
	virtual kVector3D GetVector3D(const char *name, const kVector3D &defValue = kVector3D(0)) = 0;
	virtual kVector4D GetVector4D(const char *name, const kVector4D &defValue = kVector4D(0)) = 0;
	virtual kRotation GetRotator(const char *name, const kRotation &defValue = kRotation(0, 0, 0)) = 0;
	virtual kBox3D GetBounds(const char *name, const kBox3D &defValue = kBox3D()) = 0;
	virtual kArray<int> GetIntArray(const char *name) = 0;
	virtual kArray<kPoint> GetVector2DArray(const char *name) = 0;
	virtual kArray<kVector3D> GetVector3DArray(const char *name) = 0;
	virtual kPlane3D GetPlane(const char *name, const kPlane3D &defValue = kPlane3D()) = 0;
	virtual const char *GetString(const char *name) = 0;

	template<class T>
	T *GetInterface(const char *Name)
	{
		return (T *)Query(Name);
	}
};

class IClassLibrary
{
public:
	virtual bool Register(IObjectDesc *ObjectClass) = 0;
	virtual bool Register(IPluginObjectDesc *ObjectClass) = 0;
	virtual IObject *CreateObject(EObjectType Type) = 0;
	virtual void DestroyObject(IObject *pObj) = 0;
	virtual IObjectDesc *GetDesc(EObjectType Type) = 0;
	virtual IPluginObjectDesc *GetPluginDesc(EObjectType Type) = 0;
};


