
#pragma once

#include "ISerialize.h"
#include "IMeshObject.h"

class FTextureSourceInfo
{
public:
	struct FCompressedImage2D
	{
		std::vector<char> RawData;
		int SizeX;
		int SizeY;
		char  PixelFormat; // EPixelFormat, opaque to avoid dependencies on Engine headers.
	};
	FTextureSourceInfo() { Reset(); }
	void Reset();
	void Init(int Width, int Height, ETextureFormat InFormat, bool bSRGB, const char *InData, int NumBytes);
	void Serialize(ISerialize &Ar, int Ver);
	void SetNum(int Num) { Data.resize(Num); }
	std::vector<FCompressedImage2D>	CompressedImages;
	ETextureFormat				SourceFormat;
	int							SizeX, SizeY;
	std::vector<char>			Data;
	bool						SRGB;
	bool						CompressionNoAlpha;
	bool						bDitherMipMapAlpha;
	bool						bUseLegacyGamma;
};

struct FModelTexture
{
	std::string				Name;
	FTextureSourceInfo		Source;
	void CopyFrom(FModelTexture &oth);
	void SetData(FTextureSourceInfo &SourceInfo);
	void Serialize(ISerialize &Ar, int Ver);

	static unsigned long PackRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	static void BlockDecompressImageDXT1(unsigned long width, unsigned long height, const unsigned char *blockStorage, unsigned long *image);
	static void DecompressBlockDXT1(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image);
	static void DecompressBlockDXT5(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image);
	static void BlockDecompressImageDXT5(unsigned long width, unsigned long height, const unsigned char *blockStorage, unsigned long *image);
};

