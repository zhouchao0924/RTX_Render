
#pragma once

#include "ResAsyncTask.h"

class UModelFile;
class UCompoundModelFile;
class UModelImporter;
class FImportAsyncTask : public FDRAsyncTask
{
public:
	FImportAsyncTask(UModelFile *InModelFile, UModelFile *InUpdateModelFile, UModelImporter *InImporter, const FString &InFilename);
	FImportAsyncTask(UCompoundModelFile *InModelFile, UCompoundModelFile *InUpdateModelFile, UModelImporter *InImporter, const FString &InFilename);
	void DoWork();
	void ExecuteDone() override;
	virtual bool IsNeedAsyncThread() { return true; }
	void AddReferencedObjects(FReferenceCollector& Collector) override;
	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FImportAsyncTask, STATGROUP_ThreadPoolAsyncTasks); }
protected:
// 	UCompoundModelFile *CompoundModelFile;
	UModelFile *ModelFile;

// 	UCompoundModelFile *UpdateCompoundModelFile;
	UModelFile *UpdateModelFile;

	UModelImporter *Importer;
	FString Filename;
};

