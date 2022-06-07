#pragma once
#include <d3d9.h>

struct Texture_t
{
	std::byte			pad0[0xC];		// 0x0000
	IDirect3DTexture9* lpRawTexture;	// 0x000C
};

class ITexture
{
private:
	std::byte	pad0[0x50];		 // 0x0000
public:
	Texture_t** pTextureHandles; // 0x0050

	int GetActualWidth()
	{
		return memory::CallVFunc<int>(this, 3);
	}

	int GetActualHeight()
	{
		return memory::CallVFunc<int>(this, 4);
	}

	void IncrementReferenceCount()
	{
		memory::CallVFunc<void>(this, 10);
	}

	void DecrementReferenceCount()
	{
		memory::CallVFunc<void>(this, 11);
	}
};
