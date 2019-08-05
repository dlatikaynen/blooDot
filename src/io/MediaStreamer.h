#pragma once

class MediaStreamer
{
private:
    WAVEFORMATEX    m_waveFormat;
    uint32          m_maxStreamLengthInBytes;

public:
    Microsoft::WRL::ComPtr<IMFSourceReader> m_reader;
    Microsoft::WRL::ComPtr<IMFMediaType> m_audioType;

public:
    MediaStreamer();
    ~MediaStreamer();

    WAVEFORMATEX& GetOutputWaveFormatEx()
    {
        return m_waveFormat;
    }

    UINT32 GetMaxStreamLengthInBytes()
    {
        return m_maxStreamLengthInBytes;
    }

    void Initialize(_In_ const WCHAR* url);
    bool GetNextBuffer(uint8* buffer, uint32 maxBufferSize, uint32* bufferLength);
    void ReadAll(uint8* buffer, uint32 maxBufferSize, uint32* bufferLength);
    void Restart();
};