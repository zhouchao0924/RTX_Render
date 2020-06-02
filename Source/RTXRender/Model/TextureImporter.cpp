
#include "TextureImporter.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "DDSLoader.h"
#include "ModelFile.h"
#include "ModelCompress.h"

// .PCX file header.
#pragma pack(push,1)
class FPCXFileHeader
{
public:
	uint8	Manufacturer;		// Always 10.
	uint8	Version;			// PCX file version.
	uint8	Encoding;			// 1=run-length, 0=none.
	uint8	BitsPerPixel;		// 1,2,4, or 8.
	uint16	XMin;				// Dimensions of the image.
	uint16	YMin;				// Dimensions of the image.
	uint16	XMax;				// Dimensions of the image.
	uint16	YMax;				// Dimensions of the image.
	uint16	XDotsPerInch;		// Horizontal printer resolution.
	uint16	YDotsPerInch;		// Vertical printer resolution.
	uint8	OldColorMap[48];	// Old colormap info data.
	uint8	Reserved1;			// Must be 0.
	uint8	NumPlanes;			// Number of color planes (1, 3, 4, etc).
	uint16	BytesPerLine;		// Number of bytes per scanline.
	uint16	PaletteType;		// How to interpret palette: 1=color, 2=gray.
	uint16	HScreenSize;		// Horizontal monitor size.
	uint16	VScreenSize;		// Vertical monitor size.
	uint8	Reserved2[54];		// Must be 0.
	friend FArchive& operator<<(FArchive& Ar, FPCXFileHeader& H)
	{
		Ar << H.Manufacturer << H.Version << H.Encoding << H.BitsPerPixel;
		Ar << H.XMin << H.YMin << H.XMax << H.YMax << H.XDotsPerInch << H.YDotsPerInch;
		for (int32 i = 0; i<ARRAY_COUNT(H.OldColorMap); i++)
			Ar << H.OldColorMap[i];
		Ar << H.Reserved1 << H.NumPlanes;
		Ar << H.BytesPerLine << H.PaletteType << H.HScreenSize << H.VScreenSize;
		for (int32 i = 0; i<ARRAY_COUNT(H.Reserved2); i++)
			Ar << H.Reserved2[i];
		return Ar;
	}
};

struct FTGAFileFooter
{
	uint32 ExtensionAreaOffset;
	uint32 DeveloperDirectoryOffset;
	uint8 Signature[16];
	uint8 TrailingPeriod;
	uint8 NullTerminator;
};

struct FPSDFileHeader
{
	int32     Signature;      // 8BPS
	int16   Version;        // Version
	int16   nChannels;      // Number of Channels (3=RGB) (4=RGBA)
	int32     Height;         // Number of Image Rows
	int32     Width;          // Number of Image Columns
	int16   Depth;          // Number of Bits per Channel
	int16   Mode;           // Image Mode (0=Bitmap)(1=Grayscale)(2=Indexed)(3=RGB)(4=CYMK)(7=Multichannel)
	uint8    Pad[6];         // Padding

							 /**
							 * @return Whether file has a valid signature
							 */
	bool IsValid(void)
	{
		// Fail on bad signature
		if (Signature != 0x38425053)
			return false;

		return true;
	}

	/**
	* @return Whether file has a supported version
	*/
	bool IsSupported(void)
	{
		// Fail on bad version
		if (Version != 1)
			return false;
		// Fail on anything other than 3 or 4 channels
		if ((nChannels != 3) && (nChannels != 4))
			return false;
		// Fail on anything other than 8 Bits/channel or 16 Bits/channel  
		if ((Depth != 8) && (Depth != 16))
			return false;
		// Fail on anything other than RGB
		// We can add support for indexed later if needed.
		if (Mode != 3)
			return false;

		return true;
	}
};

#pragma pack(pop)

#pragma pack(push,1)

struct FTGAFileHeader
{
	uint8 IdFieldLength;
	uint8 ColorMapType;
	uint8 ImageTypeCode;		// 2 for uncompressed RGB format
	uint16 ColorMapOrigin;
	uint16 ColorMapLength;
	uint8 ColorMapEntrySize;
	uint16 XOrigin;
	uint16 YOrigin;
	uint16 Width;
	uint16 Height;
	uint8 BitsPerPixel;
	uint8 ImageDescriptor;
	friend FArchive& operator<<(FArchive& Ar, FTGAFileHeader& H)
	{
		Ar << H.IdFieldLength << H.ColorMapType << H.ImageTypeCode;
		Ar << H.ColorMapOrigin << H.ColorMapLength << H.ColorMapEntrySize;
		Ar << H.XOrigin << H.YOrigin << H.Width << H.Height << H.BitsPerPixel;
		Ar << H.ImageDescriptor;
		return Ar;
	}
};

#pragma pack(pop)

static void psd_GetPSDHeader(const uint8* Buffer, FPSDFileHeader& Info)
{
	Info.Signature = ((int32)Buffer[0] << 24) +
		((int32)Buffer[1] << 16) +
		((int32)Buffer[2] << 8) +
		((int32)Buffer[3] << 0);
	Info.Version = ((int32)Buffer[4] << 8) +
		((int32)Buffer[5] << 0);
	Info.nChannels = ((int32)Buffer[12] << 8) +
		((int32)Buffer[13] << 0);
	Info.Height = ((int32)Buffer[14] << 24) +
		((int32)Buffer[15] << 16) +
		((int32)Buffer[16] << 8) +
		((int32)Buffer[17] << 0);
	Info.Width = ((int32)Buffer[18] << 24) +
		((int32)Buffer[19] << 16) +
		((int32)Buffer[20] << 8) +
		((int32)Buffer[21] << 0);
	Info.Depth = ((int32)Buffer[22] << 8) +
		((int32)Buffer[23] << 0);
	Info.Mode = ((int32)Buffer[24] << 8) +
		((int32)Buffer[25] << 0);
}


static bool psd_ReadData(uint8* pOut, const uint8*& pBuffer, FPSDFileHeader& Info)
{
	const uint8* pPlane = nullptr;
	const uint8* pRowTable = nullptr;
	int32         iPlane;
	int16       CompressionType;
	int32         iPixel;
	int32         iRow;
	int32         CompressedBytes;
	int32         iByte;
	int32         Count;
	uint8        Value;

	// Double check to make sure this is a valid request
	if (!Info.IsValid() || !Info.IsSupported())
	{
		return false;
	}

	const uint8* pCur = pBuffer + sizeof(FPSDFileHeader);
	int32         NPixels = Info.Width * Info.Height;

	int32  ClutSize = ((int32)pCur[0] << 24) +
		((int32)pCur[1] << 16) +
		((int32)pCur[2] << 8) +
		((int32)pCur[3] << 0);
	pCur += 4;
	pCur += ClutSize;

	// Skip Image Resource Section
	int32 ImageResourceSize = ((int32)pCur[0] << 24) +
		((int32)pCur[1] << 16) +
		((int32)pCur[2] << 8) +
		((int32)pCur[3] << 0);
	pCur += 4 + ImageResourceSize;

	// Skip Layer and Mask Section
	int32 LayerAndMaskSize = ((int32)pCur[0] << 24) +
		((int32)pCur[1] << 16) +
		((int32)pCur[2] << 8) +
		((int32)pCur[3] << 0);
	pCur += 4 + LayerAndMaskSize;

	// Determine number of bytes per pixel
	int32 BytesPerPixel = 3;
	const int32 BytesPerChannel = (Info.Depth / 8);
	switch (Info.Mode)
	{
	case 2:
		BytesPerPixel = 1;
		return false;  // until we support indexed...
		break;
	case 3:
		if (Info.nChannels == 3)
			BytesPerPixel = 3 * BytesPerChannel;
		else
			BytesPerPixel = 4 * BytesPerChannel;
		break;
	default:
		return false;
		break;
	}

	// Get Compression Type
	CompressionType = ((int32)pCur[0] << 8) + ((int32)pCur[1] << 0);
	pCur += 2;

	// Fail on 16 Bits/channel with RLE. This can occur when the file is not saved with 'Maximize Compatibility'. Compression doesn't appear to be standard.
	if (CompressionType == 1 && Info.Depth == 16)
	{
		return false;
	}

	// If no alpha channel, set alpha to opaque (255 or 65536).
	if (Info.nChannels == 3)
	{
		if (Info.Depth == 8)
		{
			const uint32 Channels = 4;
			const uint32 BufferSize = Info.Width * Info.Height * Channels * sizeof(uint8);
			FMemory::Memset(pOut, 0xff, BufferSize);
		}
		else if (Info.Depth == 16)
		{
			const uint32 Channels = 4;
			const uint32 BufferSize = Info.Width * Info.Height * Channels * sizeof(uint16);
			FMemory::Memset(pOut, 0xff, BufferSize);
		}
	}

	// Uncompressed?
	if (CompressionType == 0)
	{
		if (Info.Depth == 8)
		{
			FColor* Dest = (FColor*)pOut;
			for (int32 Pixel = 0; Pixel < NPixels; Pixel++)
			{
				Dest[Pixel].R = pCur[NPixels * 0 + Pixel];
				Dest[Pixel].G = pCur[NPixels * 1 + Pixel];
				Dest[Pixel].B = pCur[NPixels * 2 + Pixel];
				if (Info.nChannels == 4)
				{
					Dest[Pixel].A = pCur[NPixels * 3 + Pixel];
				}
			}
		}
		else if (Info.Depth == 16)
		{
			uint32 SrcOffset = 0;

			// Loop through the planes	
			for (iPlane = 0; iPlane < Info.nChannels; iPlane++)
			{
				uint16* Dest = (uint16*)pOut;
				uint32 ChannelOffset = iPlane;

				for (int32 Pixel = 0; Pixel < NPixels; Pixel++)
				{
					Dest[ChannelOffset] = ((pCur[SrcOffset] << 8) + (pCur[SrcOffset + 1] << 0));

					//Increment offsets
					ChannelOffset += 4;
					SrcOffset += BytesPerChannel;
				}

			}
		}
	}
	// RLE?
	else if (CompressionType == 1)
	{
		// Setup RowTable
		pRowTable = pCur;
		pCur += Info.nChannels*Info.Height * 2;

		FColor* Dest = (FColor*)pOut;

		// Loop through the planes
		for (iPlane = 0; iPlane < Info.nChannels; iPlane++)
		{
			int32 iWritePlane = iPlane;
			if (iWritePlane > BytesPerPixel - 1) iWritePlane = BytesPerPixel - 1;

			// Loop through the rows
			for (iRow = 0; iRow < Info.Height; iRow++)
			{
				// Load a row
				CompressedBytes = (pRowTable[(iPlane*Info.Height + iRow) * 2] << 8) +
					(pRowTable[(iPlane*Info.Height + iRow) * 2 + 1] << 0);

				// Setup Plane
				pPlane = pCur;
				pCur += CompressedBytes;

				// Decompress Row
				iPixel = 0;
				iByte = 0;
				while ((iPixel < Info.Width) && (iByte < CompressedBytes))
				{
					int8 code = (int8)pPlane[iByte++];

					// Is it a repeat?
					if (code < 0)
					{
						Count = -(int32)code + 1;
						Value = pPlane[iByte++];
						while (Count-- > 0)
						{
							int32 idx = (iPixel)+(iRow*Info.Width);
							switch (iWritePlane)
							{
							case 0: Dest[idx].R = Value; break;
							case 1: Dest[idx].G = Value; break;
							case 2: Dest[idx].B = Value; break;
							case 3: Dest[idx].A = Value; break;
							}
							iPixel++;
						}
					}
					// Must be a literal then
					else
					{
						Count = (int32)code + 1;
						while (Count-- > 0)
						{
							Value = pPlane[iByte++];
							int32 idx = (iPixel)+(iRow*Info.Width);

							switch (iWritePlane)
							{
							case 0: Dest[idx].R = Value; break;
							case 1: Dest[idx].G = Value; break;
							case 2: Dest[idx].B = Value; break;
							case 3: Dest[idx].A = Value; break;
							}
							iPixel++;
						}
					}
				}

				// Confirm that we decoded the right number of bytes
				check(iByte == CompressedBytes);
				check(iPixel == Info.Width);
			}
		}
	}
	else
		return false;

	// Success!
	return(true);
}

void DecompressTGA_RLE_32bpp(const FTGAFileHeader* TGA, uint32* TextureData)
{
	uint8*	IdData = (uint8*)TGA + sizeof(FTGAFileHeader);
	uint8*	ColorMap = IdData + TGA->IdFieldLength;
	uint8*	ImageData = (uint8*)(ColorMap + (TGA->ColorMapEntrySize + 4) / 8 * TGA->ColorMapLength);
	uint32	Pixel = 0;
	int32     RLERun = 0;
	int32     RAWRun = 0;

	for (int32 Y = TGA->Height - 1; Y >= 0; Y--) // Y-flipped.
	{
		for (int32 X = 0; X < TGA->Width; X++)
		{
			if (RLERun > 0)
			{
				RLERun--;  // reuse current Pixel data.
			}
			else if (RAWRun == 0) // new raw pixel or RLE-run.
			{
				uint8 RLEChunk = *(ImageData++);
				if (RLEChunk & 0x80)
				{
					RLERun = (RLEChunk & 0x7F) + 1;
					RAWRun = 1;
				}
				else
				{
					RAWRun = (RLEChunk & 0x7F) + 1;
				}
			}
			// Retrieve new pixel data - raw run or single pixel for RLE stretch.
			if (RAWRun > 0)
			{
				Pixel = *(uint32*)ImageData; // RGBA 32-bit dword.
				ImageData += 4;
				RAWRun--;
				RLERun--;
			}
			// Store.
			*((TextureData + Y*TGA->Width) + X) = Pixel;
		}
	}
}

void DecompressTGA_RLE_24bpp(const FTGAFileHeader* TGA, uint32* TextureData)
{
	uint8*	IdData = (uint8*)TGA + sizeof(FTGAFileHeader);
	uint8*	ColorMap = IdData + TGA->IdFieldLength;
	uint8*	ImageData = (uint8*)(ColorMap + (TGA->ColorMapEntrySize + 4) / 8 * TGA->ColorMapLength);
	uint8    Pixel[4] = {};
	int32     RLERun = 0;
	int32     RAWRun = 0;

	for (int32 Y = TGA->Height - 1; Y >= 0; Y--) // Y-flipped.
	{
		for (int32 X = 0; X < TGA->Width; X++)
		{
			if (RLERun > 0)
				RLERun--;  // reuse current Pixel data.
			else if (RAWRun == 0) // new raw pixel or RLE-run.
			{
				uint8 RLEChunk = *(ImageData++);
				if (RLEChunk & 0x80)
				{
					RLERun = (RLEChunk & 0x7F) + 1;
					RAWRun = 1;
				}
				else
				{
					RAWRun = (RLEChunk & 0x7F) + 1;
				}
			}
			// Retrieve new pixel data - raw run or single pixel for RLE stretch.
			if (RAWRun > 0)
			{
				Pixel[0] = *(ImageData++);
				Pixel[1] = *(ImageData++);
				Pixel[2] = *(ImageData++);
				Pixel[3] = 255;
				RAWRun--;
				RLERun--;
			}
			// Store.
			*((TextureData + Y*TGA->Width) + X) = *(uint32*)&Pixel;
		}
	}
}

void DecompressTGA_RLE_16bpp(const FTGAFileHeader* TGA, uint32* TextureData)
{
	uint8*	IdData = (uint8*)TGA + sizeof(FTGAFileHeader);
	uint8*	ColorMap = IdData + TGA->IdFieldLength;
	uint16*	ImageData = (uint16*)(ColorMap + (TGA->ColorMapEntrySize + 4) / 8 * TGA->ColorMapLength);
	uint16  FilePixel = 0;
	uint32	TexturePixel = 0;
	int32     RLERun = 0;
	int32     RAWRun = 0;

	for (int32 Y = TGA->Height - 1; Y >= 0; Y--) // Y-flipped.
	{
		for (int32 X = 0; X < TGA->Width; X++)
		{
			if (RLERun > 0)
				RLERun--;  // reuse current Pixel data.
			else if (RAWRun == 0) // new raw pixel or RLE-run.
			{
				uint8 RLEChunk = *((uint8*)ImageData);
				ImageData = (uint16*)(((uint8*)ImageData) + 1);
				if (RLEChunk & 0x80)
				{
					RLERun = (RLEChunk & 0x7F) + 1;
					RAWRun = 1;
				}
				else
				{
					RAWRun = (RLEChunk & 0x7F) + 1;
				}
			}
			// Retrieve new pixel data - raw run or single pixel for RLE stretch.
			if (RAWRun > 0)
			{
				FilePixel = *(ImageData++);
				RAWRun--;
				RLERun--;
			}
			// Convert file format A1R5G5B5 into pixel format B8G8R8B8
			TexturePixel = (FilePixel & 0x001F) << 3;
			TexturePixel |= (FilePixel & 0x03E0) << 6;
			TexturePixel |= (FilePixel & 0x7C00) << 9;
			TexturePixel |= (FilePixel & 0x8000) << 16;
			// Store.
			*((TextureData + Y*TGA->Width) + X) = TexturePixel;
		}
	}
}

void DecompressTGA_32bpp(const FTGAFileHeader* TGA, uint32* TextureData)
{

	uint8*	IdData = (uint8*)TGA + sizeof(FTGAFileHeader);
	uint8*	ColorMap = IdData + TGA->IdFieldLength;
	uint32*	ImageData = (uint32*)(ColorMap + (TGA->ColorMapEntrySize + 4) / 8 * TGA->ColorMapLength);

	for (int32 Y = 0; Y < TGA->Height; Y++)
	{
		FMemory::Memcpy(TextureData + Y * TGA->Width, ImageData + (TGA->Height - Y - 1) * TGA->Width, TGA->Width * 4);
	}
}

void DecompressTGA_16bpp(const FTGAFileHeader* TGA, uint32* TextureData)
{
	uint8*	IdData = (uint8*)TGA + sizeof(FTGAFileHeader);
	uint8*	ColorMap = IdData + TGA->IdFieldLength;
	uint16*	ImageData = (uint16*)(ColorMap + (TGA->ColorMapEntrySize + 4) / 8 * TGA->ColorMapLength);
	uint16    FilePixel = 0;
	uint32	TexturePixel = 0;

	for (int32 Y = TGA->Height - 1; Y >= 0; Y--)
	{
		for (int32 X = 0; X < TGA->Width; X++)
		{
			FilePixel = *ImageData++;
			// Convert file format A1R5G5B5 into pixel format B8G8R8A8
			TexturePixel = (FilePixel & 0x001F) << 3;
			TexturePixel |= (FilePixel & 0x03E0) << 6;
			TexturePixel |= (FilePixel & 0x7C00) << 9;
			TexturePixel |= (FilePixel & 0x8000) << 16;
			// Store.
			*((TextureData + Y*TGA->Width) + X) = TexturePixel;
		}
	}
}

void DecompressTGA_24bpp(const FTGAFileHeader* TGA, uint32* TextureData)
{
	uint8*	IdData = (uint8*)TGA + sizeof(FTGAFileHeader);
	uint8*	ColorMap = IdData + TGA->IdFieldLength;
	uint8*	ImageData = (uint8*)(ColorMap + (TGA->ColorMapEntrySize + 4) / 8 * TGA->ColorMapLength);
	uint8    Pixel[4];

	for (int32 Y = 0; Y < TGA->Height; Y++)
	{
		for (int32 X = 0; X < TGA->Width; X++)
		{
			Pixel[0] = *((ImageData + (TGA->Height - Y - 1)*TGA->Width * 3) + X * 3 + 0);
			Pixel[1] = *((ImageData + (TGA->Height - Y - 1)*TGA->Width * 3) + X * 3 + 1);
			Pixel[2] = *((ImageData + (TGA->Height - Y - 1)*TGA->Width * 3) + X * 3 + 2);
			Pixel[3] = 255;
			*((TextureData + Y*TGA->Width) + X) = *(uint32*)&Pixel;
		}
	}
}

void DecompressTGA_8bpp(const FTGAFileHeader* TGA, uint8* TextureData)
{
	const uint8*  const IdData = (uint8*)TGA + sizeof(FTGAFileHeader);
	const uint8*  const ColorMap = IdData + TGA->IdFieldLength;
	const uint8*  const ImageData = (uint8*)(ColorMap + (TGA->ColorMapEntrySize + 4) / 8 * TGA->ColorMapLength);

	int32 RevY = 0;
	for (int32 Y = TGA->Height - 1; Y >= 0; --Y)
	{
		const uint8* ImageCol = ImageData + (Y * TGA->Width);
		uint8* TextureCol = TextureData + (RevY++ * TGA->Width);
		FMemory::Memcpy(TextureCol, ImageCol, TGA->Width);
	}
}

bool DecompressTGA_helper(
	const FTGAFileHeader* TGA,
	uint32*& TextureData,
	const int32 TextureDataSize)
{
	if (TGA->ImageTypeCode == 10) // 10 = RLE compressed 
	{
		// RLE compression: CHUNKS: 1 -byte header, high bit 0 = raw, 1 = compressed
		// bits 0-6 are a 7-bit count; count+1 = number of raw pixels following, or rle pixels to be expanded. 
		if (TGA->BitsPerPixel == 32)
		{
			DecompressTGA_RLE_32bpp(TGA, TextureData);
		}
		else if (TGA->BitsPerPixel == 24)
		{
			DecompressTGA_RLE_24bpp(TGA, TextureData);
		}
		else if (TGA->BitsPerPixel == 16)
		{
			DecompressTGA_RLE_16bpp(TGA, TextureData);
		}
		else
		{
			return false;
		}
	}
	else if (TGA->ImageTypeCode == 2) // 2 = Uncompressed RGB
	{
		if (TGA->BitsPerPixel == 32)
		{
			DecompressTGA_32bpp(TGA, TextureData);
		}
		else if (TGA->BitsPerPixel == 16)
		{
			DecompressTGA_16bpp(TGA, TextureData);
		}
		else if (TGA->BitsPerPixel == 24)
		{
			DecompressTGA_24bpp(TGA, TextureData);
		}
		else
		{
			return false;
		}
	}
	// Support for alpha stored as pseudo-color 8-bit TGA
	else if (TGA->ColorMapType == 1 && TGA->ImageTypeCode == 1 && TGA->BitsPerPixel == 8)
	{
		DecompressTGA_8bpp(TGA, (uint8*)TextureData);
	}
	// standard grayscale
	else if (TGA->ColorMapType == 0 && TGA->ImageTypeCode == 3 && TGA->BitsPerPixel == 8)
	{
		DecompressTGA_8bpp(TGA, (uint8*)TextureData);
	}
	else
	{
		return false;
	}

	// Flip the image data if the flip bits are set in the TGA header.
	bool FlipX = (TGA->ImageDescriptor & 0x10) ? 1 : 0;
	bool FlipY = (TGA->ImageDescriptor & 0x20) ? 1 : 0;
	if (FlipY || FlipX)
	{
		TArray<uint8> FlippedData;
		FlippedData.AddUninitialized(TextureDataSize);

		int32 NumBlocksX = TGA->Width;
		int32 NumBlocksY = TGA->Height;
		int32 BlockBytes = TGA->BitsPerPixel == 8 ? 1 : 4;

		uint8* MipData = (uint8*)TextureData;

		for (int32 Y = 0; Y < NumBlocksY; Y++)
		{
			for (int32 X = 0; X < NumBlocksX; X++)
			{
				int32 DestX = FlipX ? (NumBlocksX - X - 1) : X;
				int32 DestY = FlipY ? (NumBlocksY - Y - 1) : Y;
				FMemory::Memcpy(
					&FlippedData[(DestX + DestY * NumBlocksX) * BlockBytes],
					&MipData[(X + Y * NumBlocksX) * BlockBytes],
					BlockBytes
				);
			}
		}
		FMemory::Memcpy(MipData, FlippedData.GetData(), FlippedData.Num());
	}

	return true;
}

UTextureImporter::UTextureImporter(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
}

bool UTextureImporter::DecompressTGA(const FTGAFileHeader*	TGA, FTextureSourceInfo &TextureSourceInfo)
{
	if (TGA->ColorMapType == 1 && TGA->ImageTypeCode == 1 && TGA->BitsPerPixel == 8)
	{
		// Notes: The Scaleform GFx exporter (dll) strips all font glyphs into a single 8-bit texture.
		// The targa format uses this for a palette index; GFx uses a palette of (i,i,i,i) so the index
		// is also the alpha value.
		//
		// We store the image as PF_G8, where it will be used as alpha in the Glyph shader.
		TextureSourceInfo.SizeX = TGA->Width;
		TextureSourceInfo.SizeY = TGA->Height;
		TextureSourceInfo.SourceFormat = TSF_G8;
		TextureSourceInfo.CompressionSettings = TC_Grayscale;
	}
	else if (TGA->ColorMapType == 0 && TGA->ImageTypeCode == 3 && TGA->BitsPerPixel == 8)
	{
		TextureSourceInfo.SizeX = TGA->Width;
		TextureSourceInfo.SizeY = TGA->Height;
		TextureSourceInfo.SourceFormat = TSF_G8;
		TextureSourceInfo.CompressionSettings = TC_Grayscale;
	}
	else
	{
		if (TGA->ImageTypeCode == 10) // 10 = RLE compressed 
		{
			if (TGA->BitsPerPixel != 32 &&
				TGA->BitsPerPixel != 24 &&
				TGA->BitsPerPixel != 16)
			{
				return false;
			}
		}
		else
		{
			if (TGA->BitsPerPixel != 32 &&
				TGA->BitsPerPixel != 16 &&
				TGA->BitsPerPixel != 24)
			{
				return false;
			}
		}

		TextureSourceInfo.SizeX = TGA->Width;
		TextureSourceInfo.SizeY = TGA->Height;
		TextureSourceInfo.SourceFormat = TSF_BGRA8;
	}

	int32 TextureDataSize = TextureSourceInfo.SizeX * TextureSourceInfo.SizeY * 4;
	TextureSourceInfo.SetNum(TextureDataSize);

	uint32* TextureData = (uint32*)TextureSourceInfo.Data.GetData();

	return DecompressTGA_helper(TGA, TextureData, TextureDataSize);
}

bool UTextureImporter::IsImportResolutionValid(int32 Width, int32 Height, bool bAllowNonPowerOfTwo)
{
	// Calculate the maximum supported resolution utilizing the global max texture mip count
	// (Note, have to subtract 1 because 1x1 is a valid mip-size; this means a GMaxTextureMipCount of 4 means a max resolution of 8x8, not 2^4 = 16x16)
	const int32 MaximumSupportedResolution = 1 << (GMaxTextureMipCount - 1);

	bool bValid = true;

	// Check if the texture is above the supported resolution and prompt the user if they wish to continue if it is
	if (Width > MaximumSupportedResolution || Height > MaximumSupportedResolution)
	{
		if (EAppReturnType::Yes != FMessageDialog::Open(EAppMsgType::YesNo, FText::Format(
			NSLOCTEXT("UnrealEd", "Warning_LargeTextureImport", "Attempting to import {0} x {1} texture, proceed?\nLargest supported texture size: {2} x {3}"),
			FText::AsNumber(Width), FText::AsNumber(Height), FText::AsNumber(MaximumSupportedResolution), FText::AsNumber(MaximumSupportedResolution))))
		{
			bValid = false;
		}
	}

	const bool bIsPowerOfTwo = FMath::IsPowerOfTwo(Width) && FMath::IsPowerOfTwo(Height);
	// Check if the texture dimensions are powers of two
	if (!bAllowNonPowerOfTwo && !bIsPowerOfTwo)
	{
		bValid = false;
	}

	return bValid;
}

UTexture2D* UTextureImporter::CreateTexture2D(UObject* InParent, FName Name, EObjectFlags Flags)
{
	UTexture2D* NewTexture = NewObject<UTexture2D>(InParent, Name, Flags);
	return NewTexture;
}

UTextureCube* UTextureImporter::CreateTextureCube(UObject* InParent, FName Name, EObjectFlags Flags)
{
	UTextureCube* NewTextureCube = NewObject<UTextureCube>(InParent, Name, Flags);
	return NewTextureCube;
}

template<typename PixelDataType, typename ColorDataType, int32 RIdx, int32 GIdx, int32 BIdx, int32 AIdx> class PNGDataFill
{
public:

	PNGDataFill(const FTextureSource& TextureSource, uint8* SourceTextureData)
		: SourceData(reinterpret_cast<PixelDataType*>(SourceTextureData))
		, TextureWidth(TextureSource.GetSizeX())
		, TextureHeight(TextureSource.GetSizeY())
	{
	}

	void ProcessData()
	{
		ClearZeroAlphaData();
		HorizontalPass(1);
		HorizontalPass(-1);
		VerticalPass(1);
		VerticalPass(-1);
	}

	void ClearZeroAlphaData()
	{
		for (int32 Y = 0; Y < TextureHeight; ++Y)
		{
			for (int32 X = 0; X < TextureWidth; ++X)
			{
				PixelDataType* PixelData = SourceData + (Y * TextureWidth + X) * 4;

				if (PixelData[AIdx] == 0)
				{
					ColorDataType* ColorData = reinterpret_cast<ColorDataType*>(PixelData);
					*ColorData = 0;
				}
			}
		}
	}

	void HorizontalPass(int32 XStep)
	{
		const int32 XStart = XStep > 0 ? 0 : TextureWidth - 1;
		const int32 XEnd = XStep > 0 ? TextureWidth : -1;

		for (int32 Y = 0; Y < TextureHeight; ++Y)
		{
			uint8 FillRed = 0, FillGreen = 0, FillBlue = 0;
			bool bHaveFillColor = false;

			for (int32 X = XStart; X != XEnd; X += XStep)
			{
				PixelDataType* PixelData = SourceData + (Y * TextureWidth + X) * 4;
				const ColorDataType* ColorData = reinterpret_cast<const ColorDataType*>(PixelData);

				if (*ColorData == 0)
				{
					if (bHaveFillColor)
					{
						PixelData[RIdx] = FillRed;
						PixelData[GIdx] = FillGreen;
						PixelData[BIdx] = FillBlue;
					}
				}
				else
				{
					bHaveFillColor = true;
					FillRed = PixelData[RIdx];
					FillGreen = PixelData[GIdx];
					FillBlue = PixelData[BIdx];
				}
			}
		}
	}

	void VerticalPass(int32 YStep)
	{
		const int32 YStart = YStep > 0 ? 0 : TextureHeight - 1;
		const int32 YEnd = YStep > 0 ? TextureHeight : -1;

		for (int32 X = 0; X < TextureWidth; ++X)
		{
			uint8 FillRed = 0, FillGreen = 0, FillBlue = 0;
			bool bHaveFillColor = false;

			for (int32 Y = YStart; Y != YEnd; Y += YStep)
			{
				PixelDataType* PixelData = SourceData + (Y * TextureWidth + X) * 4;
				const ColorDataType* ColorData = reinterpret_cast<const ColorDataType*>(PixelData);

				if (*ColorData == 0)
				{
					if (bHaveFillColor)
					{
						PixelData[RIdx] = FillRed;
						PixelData[GIdx] = FillGreen;
						PixelData[BIdx] = FillBlue;
					}
				}
				else
				{
					bHaveFillColor = true;
					FillRed = PixelData[RIdx];
					FillGreen = PixelData[GIdx];
					FillBlue = PixelData[BIdx];
				}
			}
		}
	}

	PixelDataType* SourceData;
	int32 TextureWidth;
	int32 TextureHeight;
};


void UTextureImporter::FillZeroAlphaPNGData(const FTextureSourceInfo& TextureSource, uint8* SourceData)
{
	const ETextureSourceFormat SourceFormat = TextureSource.SourceFormat;


// 	switch (SourceFormat)
// 	{
// 		case TSF_BGRA8:
// 		{
// 			PNGDataFill<uint8, uint32, 2, 1, 0, 3> PNGFill(TextureSource, SourceData);
// 			PNGFill.ProcessData();
// 			break;
// 		}
// 
// 		case TSF_RGBA16:
// 		{
// 			PNGDataFill<uint16, uint64, 0, 1, 2, 3> PNGFill(TextureSource, SourceData);
// 			PNGFill.ProcessData();
// 			break;
// 		}
// 	}
}


FTextureSourceInfo *UTextureImporter::ImportFromFile(UObject *InParent, ETexSlot TexSlot, const FString &Filename)
{
	FString FileExtension = FPaths::GetExtension(Filename);
	FString Name = FPaths::GetBaseFilename(Filename) + TEXT("_Importer");

	TArray<uint8> Data;
	if (!FFileHelper::LoadFileToArray(Data, *Filename))
	{
		return NULL;
	}

	Data.Add(0);
	const uint8 *Ptr = &Data[0];

	FTextureSourceInfo *TextureInfo = ImportTexture(*FileExtension, Ptr, Ptr + Data.Num() - 1);
	if (TextureInfo)
	{
		CompressUtil::CompressTexture(TextureInfo, TexSlot);
	}

	return TextureInfo;
}

FTextureSourceInfo * UTextureImporter::ImportTexture(const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd)
{
	static FTextureSourceInfo  s_TextureInfo;
	s_TextureInfo.Reset();

	bool bAllowNonPowerOfTwo = false;
	EObjectFlags Flags = RF_Transient;

	const int32 Length = BufferEnd - Buffer;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	//
	// PNG
	//
	TSharedPtr<IImageWrapper> PngImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	if (PngImageWrapper.IsValid() && PngImageWrapper->SetCompressed(Buffer, Length))
	{
		if (!IsImportResolutionValid(PngImageWrapper->GetWidth(), PngImageWrapper->GetHeight(), bAllowNonPowerOfTwo))
		{
			return NULL;
		}

		// Select the texture's source format
		ETextureSourceFormat TextureFormat = TSF_Invalid;
		int32 BitDepth = PngImageWrapper->GetBitDepth();
		ERGBFormat Format = PngImageWrapper->GetFormat();
		if (Format == ERGBFormat::Gray)
		{
			if (BitDepth <= 8)
			{
				TextureFormat = TSF_G8;
				Format = ERGBFormat::Gray;
				BitDepth = 8;
			}
			else if (BitDepth == 16)
			{
				// TODO: TSF_G16?
				TextureFormat = TSF_RGBA16;
				Format = ERGBFormat::RGBA;
				BitDepth = 16;
			}
		}
		else if (Format == ERGBFormat::RGBA || Format == ERGBFormat::BGRA)
		{
			if (BitDepth <= 8)
			{
				TextureFormat = TSF_BGRA8;
				Format = ERGBFormat::BGRA;
				BitDepth = 8;
			}
			else if (BitDepth == 16)
			{
				TextureFormat = TSF_RGBA16;
				Format = ERGBFormat::RGBA;
				BitDepth = 16;
			}
		}

		if (TextureFormat == TSF_Invalid)
		{
			return nullptr;
		}

// 		s_TextureInfo.SizeX = PngImageWrapper->GetWidth();
// 		s_TextureInfo.SizeY = PngImageWrapper->GetHeight();
// 		s_TextureInfo.SourceFormat = TextureFormat;
// 		s_TextureInfo.SRGB = BitDepth < 16;

		const TArray<uint8>* RawPNG = nullptr;
		if (!PngImageWrapper->GetRaw(Format, BitDepth, RawPNG))
		{
			return nullptr;
		}

		if(TextureFormat==TSF_RGBA16)
		{
			uint16 *pSrc = (uint16 *)RawPNG->GetData();
			int32 NumPixel = PngImageWrapper->GetWidth() * PngImageWrapper->GetHeight();
			
			TArray<uint8> ImageData;
			ImageData.SetNum(4*NumPixel);
			uint8 *pDst = ImageData.GetData();

			for (int32 i = 0; i < NumPixel; ++i)
			{
				pDst[4 * i + 0] = FMath::Clamp(FMath::FloorToInt(255.9999f*pSrc[4 * i + 2] / 65535.0f), 0, 255);
				pDst[4 * i + 1] = FMath::Clamp(FMath::FloorToInt(255.9999f*pSrc[4 * i + 1] / 65535.0f), 0, 255);
				pDst[4 * i + 2] = FMath::Clamp(FMath::FloorToInt(255.9999f*pSrc[4 * i + 0] / 65535.0f), 0, 255);
				pDst[4 * i + 3] = FMath::Clamp(FMath::FloorToInt(255.9999f*pSrc[4 * i + 3] / 65535.0f), 0, 255);
			}
			
			BitDepth = 8;
			TextureFormat = TSF_BGRA8;

			s_TextureInfo.Init(PngImageWrapper->GetWidth(), PngImageWrapper->GetHeight(), TextureFormat, BitDepth < 16, ImageData.GetData(), ImageData.Num());
		}
		else
		{
			s_TextureInfo.Init(PngImageWrapper->GetWidth(), PngImageWrapper->GetHeight(), TextureFormat, BitDepth < 16, RawPNG->GetData(), RawPNG->Num());
		}

// 		uint8* MipData = s_TextureInfo.Data.GetData();
// 
// 		s_TextureInfo.Data.SetNum(RawPNG->Num());
// 		FMemory::Memcpy(MipData, RawPNG->GetData(), RawPNG->Num());
// 		// Replace the pixels with 0.0 alpha with a color value from the nearest neighboring color which has a non-zero alpha
// 		FillZeroAlphaPNGData(s_TextureInfo, MipData);

		return &s_TextureInfo;
	}
	//
	// JPEG
	//
	TSharedPtr<IImageWrapper> JpegImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	if (JpegImageWrapper.IsValid() && JpegImageWrapper->SetCompressed(Buffer, Length))
	{
		if (!IsImportResolutionValid(JpegImageWrapper->GetWidth(), JpegImageWrapper->GetHeight(), bAllowNonPowerOfTwo))
		{
			return nullptr;
		}

		// Select the texture's source format
		ETextureSourceFormat TextureFormat = TSF_Invalid;
		int32 BitDepth = JpegImageWrapper->GetBitDepth();
		ERGBFormat Format = JpegImageWrapper->GetFormat();

		if (Format == ERGBFormat::Gray)
		{
			if (BitDepth <= 8)
			{
				TextureFormat = TSF_G8;
				Format = ERGBFormat::Gray;
				BitDepth = 8;
			}
		}
		else if (Format == ERGBFormat::RGBA)
		{
			if (BitDepth <= 8)
			{
				TextureFormat = TSF_BGRA8;
				Format = ERGBFormat::BGRA;
				BitDepth = 8;
			}
		}

		if (TextureFormat == TSF_Invalid)
		{
			return nullptr;
		}

		const TArray<uint8>* RawJPEG = nullptr;
		if (!JpegImageWrapper->GetRaw(Format, BitDepth, RawJPEG))
		{
			return nullptr;
		}

		s_TextureInfo.Init( JpegImageWrapper->GetWidth(), 
							JpegImageWrapper->GetHeight(),
							TextureFormat, 
							BitDepth < 16, 
							RawJPEG->GetData(), RawJPEG->Num());

		return &s_TextureInfo;
	}

	//
	// EXR
	//
	TSharedPtr<IImageWrapper> ExrImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
	if (ExrImageWrapper.IsValid() && ExrImageWrapper->SetCompressed(Buffer, Length))
	{
		int32 Width = ExrImageWrapper->GetWidth();
		int32 Height = ExrImageWrapper->GetHeight();

		if (!IsImportResolutionValid(Width, Height, bAllowNonPowerOfTwo))
		{
			return nullptr;
		}

		// Select the texture's source format
		ETextureSourceFormat TextureFormat = TSF_Invalid;
		int32 BitDepth = ExrImageWrapper->GetBitDepth();
		ERGBFormat Format = ExrImageWrapper->GetFormat();

		if (Format == ERGBFormat::RGBA && BitDepth == 16)
		{
			TextureFormat = TSF_RGBA16F;
			Format = ERGBFormat::BGRA;
		}

		if (TextureFormat == TSF_Invalid)
		{
			return nullptr;
		}

		const TArray<uint8>* Raw = nullptr;
		if (!ExrImageWrapper->GetRaw(Format, BitDepth, Raw))
		{
			return nullptr;
		}

		s_TextureInfo.Init(Width, Height, TextureFormat, false, Raw->GetData(), Raw->Num(), TC_HDR);

		return &s_TextureInfo;
	}
	//
	// BMP
	//
	TSharedPtr<IImageWrapper> BmpImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	if (BmpImageWrapper.IsValid() && BmpImageWrapper->SetCompressed(Buffer, Length))
	{
		// Check the resolution of the imported texture to ensure validity
		if (!IsImportResolutionValid(BmpImageWrapper->GetWidth(), BmpImageWrapper->GetHeight(), bAllowNonPowerOfTwo))
		{
			return nullptr;
		}

		const TArray<uint8>* RawBMP = nullptr;
		if (!BmpImageWrapper->GetRaw(BmpImageWrapper->GetFormat(), BmpImageWrapper->GetBitDepth(), RawBMP))
		{
			return nullptr;
		}

		s_TextureInfo.Init(BmpImageWrapper->GetWidth(), BmpImageWrapper->GetHeight(), TSF_BGRA8, false, RawBMP->GetData(), RawBMP->Num());
		
		return &s_TextureInfo;
	}

	//
	// TGA
	//
	// Support for alpha stored as pseudo-color 8-bit TGA
	const FTGAFileHeader*    TGA = (FTGAFileHeader *)Buffer;
	if (Length >= sizeof(FTGAFileHeader) &&
	((TGA->ColorMapType == 0 && TGA->ImageTypeCode == 2) ||
		// ImageTypeCode 3 is greyscale
		(TGA->ColorMapType == 0 && TGA->ImageTypeCode == 3) ||
		(TGA->ColorMapType == 0 && TGA->ImageTypeCode == 10) ||
		(TGA->ColorMapType == 1 && TGA->ImageTypeCode == 1 && TGA->BitsPerPixel == 8)))
	{
		// Check the resolution of the imported texture to ensure validity
		if (!IsImportResolutionValid(TGA->Width, TGA->Height, bAllowNonPowerOfTwo))
		{
			return nullptr;
		}

		bool bDecompressOK = DecompressTGA(TGA, s_TextureInfo);

		if (bDecompressOK && s_TextureInfo.CompressionSettings == TC_Grayscale && TGA->ImageTypeCode == 3)
		{
			// default grayscales to linear as they wont get compression otherwise and are commonly used as masks
			s_TextureInfo.SRGB = false;
		}

		return &s_TextureInfo;
	}
	
	//
	// PSD File
	//
	FPSDFileHeader			 psdhdr;
	if (Length > sizeof(FPSDFileHeader))
	{
		psd_GetPSDHeader(Buffer, psdhdr);
	}
	if (psdhdr.IsValid())
	{
		// Check the resolution of the imported texture to ensure validity
		if (!IsImportResolutionValid(psdhdr.Width, psdhdr.Height, bAllowNonPowerOfTwo))
		{
			return nullptr;
		}
		if (!psdhdr.IsSupported())
		{
			return nullptr;
		}

		// Select the texture's source format
		ETextureSourceFormat TextureFormat = TSF_Invalid;
		if (psdhdr.Depth == 8)
		{
			TextureFormat = TSF_BGRA8;
		}
		else if (psdhdr.Depth == 16)
		{
			TextureFormat = TSF_RGBA16;
		}

		if (TextureFormat == TSF_Invalid)
		{
			return nullptr;
		}

		s_TextureInfo.SizeX = psdhdr.Width;
		s_TextureInfo.SizeY = psdhdr.Height;
		s_TextureInfo.SourceFormat = TextureFormat;
		s_TextureInfo.SetNum(psdhdr.Width*psdhdr.Height*psdhdr.Depth / sizeof(uint8));


		uint8* Dst = (uint8*)s_TextureInfo.Data.GetData();

		if (!psd_ReadData(Dst, Buffer, psdhdr))
		{
			return nullptr;
		}
		
		return &s_TextureInfo;
	}
	
	return nullptr;
}

bool UTextureImporter::ExportTGA(int32 SizeX, int32 SizeY, const TArray<uint8> &Data, const FString &Filename, ETextureSourceFormat Format/*= TSF_BGRA8*/)
{
	if ((Format != TSF_BGRA8 && Format != TSF_RGBA16))
	{
		return false;
	}
	
	return ExportTGA(SizeX, SizeY, (uint8 *)Data.GetData(), Format==TSF_RGBA16, Filename);
}

bool UTextureImporter::ExportTextureTGA(UTexture2D *TexImage, const FString &Filename)
{
	bool bExportSuccess = false;

	if (TexImage && TexImage->PlatformData)
	{
		FTexture2DMipMap &MipData =TexImage->PlatformData->Mips[0];
		if (TexImage->GetPixelFormat() == TSF_BGRA8)
		{
			int32 SizeX = TexImage->GetSurfaceWidth();
			int32 SizeY = TexImage->GetSurfaceHeight();

			uint8 *pData = (uint8 *)MipData.BulkData.Lock(LOCK_READ_ONLY);
			bExportSuccess = ExportTGA(SizeX, SizeY, pData, TexImage->GetPixelFormat() == TSF_RGBA16, Filename);
			MipData.BulkData.Unlock();
		}
	}

	return bExportSuccess;
}

bool UTextureImporter::ExportTGA(FModelTexture *Texture, const FString &Filename)
{
	if ((Texture->Source.SourceFormat != TSF_BGRA8 && Texture->Source.SourceFormat != TSF_RGBA16))
	{
		return false;
	}
	
	return ExportTGA(Texture->Source.SizeX, Texture->Source.SizeY, Texture->Source.Data.GetData(), Texture->Source.SourceFormat == TSF_RGBA16, Filename);
}

bool UTextureImporter::ExportTGA(int32 SizeX, int32 SizeY, uint8 *Data, bool bIsRGBA16, const FString &Filename)
{
	FArchive *Writer = IFileManager::Get().CreateFileWriter(*Filename);
	if (Writer)
	{
		FArchive &Ar = *Writer;

		const int32 BytesPerPixel = bIsRGBA16 ? 8 : 4;

		// If we should export the file with no alpha info.  
// 		// If the texture is compressed with no alpha we should definitely not export an alpha channel
		bool bExportWithAlpha = true; //!Texture->Source.CompressionNoAlpha;
// 		if (bExportWithAlpha)
// 		{
// 			// If the texture isn't compressed with no alpha scan the texture to see if the alpha values are all 255 which means we can skip exporting it.
// 			// This is a relatively slow process but we are just exporting textures 
// 			bExportWithAlpha = false;
// 			const int32 AlphaOffset = bIsRGBA16 ? 7 : 3;
// 			for (int32 Y = SizeY - 1; Y >= 0; --Y)
// 			{
// 				const uint8* Color = Data + Y * SizeX * BytesPerPixel;
// 				for (int32 X = SizeX; X > 0; --X)
// 				{
// 					// Skip color info
// 					Color += AlphaOffset;
// 					// Get Alpha value then increment the pointer past it for the next pixel
// 					uint8 Alpha = *Color++;
// 					if (Alpha != 255)
// 					{
// 						// When a texture is imported with no alpha, the alpha bits are set to 255
// 						// So if the texture has non 255 alpha values, the texture is a valid alpha channel
// 						bExportWithAlpha = true;
// 						break;
// 					}
// 				}
// 				if (bExportWithAlpha)
// 				{
// 					break;
// 				}
// 			}
// 		}

		const int32 OriginalWidth = SizeX;
		const int32 OriginalHeight = SizeY;

		FTGAFileHeader TGA;
		FMemory::Memzero(&TGA, sizeof(TGA));
		TGA.ImageTypeCode = 2;
		TGA.BitsPerPixel = bExportWithAlpha ? 32 : 24;
		TGA.Height = OriginalHeight;
		TGA.Width = OriginalWidth;
		Ar.Serialize(&TGA, sizeof(TGA));

		if (bExportWithAlpha && !bIsRGBA16)
		{
			Ar.Serialize(Data, OriginalHeight*OriginalWidth * 4);
// 			for (int32 Y = 0; Y < OriginalHeight; Y++)
// 			{
// 				// If we aren't skipping alpha channels we can serialize each line
// 				Ar.Serialize((void *)Data[(OriginalHeight - Y - 1) * OriginalWidth * 4], OriginalWidth * 4);
// 			}
		}
		else
		{
			// Serialize each pixel
			for (int32 Y = OriginalHeight - 1; Y >= 0; --Y)
			{
				uint8* Color = (uint8*)Data[Y * OriginalWidth * BytesPerPixel];
				for (int32 X = OriginalWidth; X > 0; --X)
				{
					if (bIsRGBA16)
					{
						Ar << Color[1];
						Ar << Color[3];
						Ar << Color[5];
						if (bExportWithAlpha)
						{
							Ar << Color[7];
						}
						Color += 8;
					}
					else
					{
						Ar << *Color++;
						Ar << *Color++;
						Ar << *Color++;
						// Skip alpha channel since we are exporting with no alpha
						Color++;
					}
				}
			}
		}

		FTGAFileFooter Ftr;
		FMemory::Memzero(&Ftr, sizeof(Ftr));
		FMemory::Memcpy(Ftr.Signature, "TRUEVISION-XFILE", 16);
		Ftr.TrailingPeriod = '.';
		Ar.Serialize(&Ftr, sizeof(Ftr));

		Writer->Close();

		return true;
	}

	return false;
}


