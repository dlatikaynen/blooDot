#pragma once

#include <ppltasks.h>

// A simple reader/writer class that provides support for reading and writing
// files on disk. Provides synchronous and asynchronous methods.
ref class BasicReaderWriter
{
private:
    Windows::Storage::StorageFolder^ m_location;

internal:
    BasicReaderWriter();
    BasicReaderWriter(
        _In_ Windows::Storage::StorageFolder^ folder
    );

    Platform::Array<byte>^ ReadData(
        _In_ Platform::String^ filename
    );

    concurrency::task<Platform::Array<byte>^> ReadDataAsync(
        _In_ Platform::String^ filename
    );

	/*template <typename T>
    uint32 WriteData(
        _In_ Microsoft::WRL::Wrappers::FileHandle* fileHandle,
        _In_ const T fileData
    );
*/
	template<typename T>
	uint32 WriteData(Microsoft::WRL::Wrappers::FileHandle* fileHandle, const T fileData)
	{
		DWORD numBytesWritten;
		if (
			!WriteFile(
				fileHandle->Get(),
				reinterpret_cast<void *>(fileData),
				sizeof(fileData),
				&numBytesWritten,
				nullptr
			) ||
			numBytesWritten != sizeof(fileData)
			)
		{
			throw ref new Platform::FailureException();
		}

		return numBytesWritten;
	}

	uint32 WriteData(
		_In_ Platform::String^ filename,
		_In_ const Platform::Array<byte>^ fileData
	);

    concurrency::task<void> WriteDataAsync(
        _In_ Platform::String^ filename,
        _In_ const Platform::Array<byte>^ fileData
        );
};

