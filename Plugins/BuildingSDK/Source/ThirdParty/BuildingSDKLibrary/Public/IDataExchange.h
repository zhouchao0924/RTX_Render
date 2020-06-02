
#pragma once

class IDataExchange
{
public:
	virtual ~IDataExchange() {}
	virtual IObject *ImportFBX(const char *Filename) = 0;
	virtual void ExportGLTF(IObject *Object, const char *Filenname) = 0;
	virtual void ExportGLTF(ISuite *Suite, const char *Filename) = 0;
	virtual void ExportScene(class ISuite *Suite, const char *Directory, bool bCompressed = false) = 0;
	virtual const char *ExportObjectToMX(IObject *Object, const char *Directory, bool bCompressed = false) = 0;
	virtual const char *ExportObjectToFBX(IObject *Object, const char *Directory) = 0;
};

