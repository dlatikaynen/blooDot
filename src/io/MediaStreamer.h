#pragma once

class MediaStreamer
{
public:
    MediaStreamer();
    ~MediaStreamer();

	WAVEFORMATEX& GetOutputWaveFormatEx();
	UINT32 GetMaxStreamLengthInBytes();
    void Initialize(_In_ const WCHAR* url);
    bool GetNextBuffer(uint8* buffer, uint32 maxBufferSize, uint32* bufferLength);
    void ReadAll(uint8* buffer, uint32 maxBufferSize, uint32* bufferLength);
    void Restart();

protected:
	WAVEFORMATEX    m_waveFormat;
	uint32          m_maxStreamLengthInBytes;
	Microsoft::WRL::ComPtr<IMFSourceReader> m_reader;
	Microsoft::WRL::ComPtr<IMFMediaType> m_audioType;
};