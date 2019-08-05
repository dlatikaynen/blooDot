#pragma once

void CreateDDSTextureFromMemory(
    _In_ ID3D11Device* d3dDevice,
    _In_reads_bytes_(ddsDataSize) const uint8_t* ddsData,
    _In_ size_t ddsDataSize,
    _Outptr_opt_ ID3D11Resource** texture,
    _Outptr_opt_ ID3D11ShaderResourceView** textureView,
    _In_ size_t maxsize = 0,
    _Out_opt_ D2D1_ALPHA_MODE* alphaMode = nullptr
);

void CreateDDSTextureFromMemoryEx(
    _In_ ID3D11Device* d3dDevice,
    _In_reads_bytes_(ddsDataSize) const uint8_t* ddsData,
    _In_ size_t ddsDataSize,
    _In_ size_t maxsize,
    _In_ D3D11_USAGE usage,
    _In_ unsigned int bindFlags,
    _In_ unsigned int cpuAccessFlags,
    _In_ unsigned int miscFlags,
    _In_ bool forceSRGB,
    _Outptr_opt_ ID3D11Resource** texture,
    _Outptr_opt_ ID3D11ShaderResourceView** textureView,
    _Out_opt_ D2D1_ALPHA_MODE* alphaMode = nullptr
);
