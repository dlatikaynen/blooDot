#pragma once
#include <string>
#include <windows.h>
#include <dwrite.h>
#include <d2d1.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <memory>
#include <vector>

typedef std::vector<std::wstring> MFCollection;

template <typename InterfaceType> inline void SafeRelease(InterfaceType** currentObject)
{
	if (*currentObject != NULL)
	{
		(*currentObject)->Release();
		*currentObject = NULL;
	}
}


template <typename InterfaceType> inline InterfaceType* SafeAcquire(InterfaceType* newObject)
{
	if (newObject != NULL)
	{
		newObject->AddRef();
	}

	return newObject;
}

class MFFontCollectionLoader : public IDWriteFontCollectionLoader
{
public:
    MFFontCollectionLoader() : refCount_(0) { }
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT STDMETHODCALLTYPE CreateEnumeratorFromKey(IDWriteFactory* factory, void const* collectionKey, UINT32 collectionKeySize, OUT IDWriteFontFileEnumerator** fontFileEnumerator);

	static IDWriteFontCollectionLoader* GetLoader()
    {
        return instance_;
    }

    static bool IsLoaderInitialized()
    {
        return instance_ != NULL;
    }

private:
    ULONG refCount_;
    static IDWriteFontCollectionLoader* instance_;
};

class MFFontFileEnumerator : public IDWriteFontFileEnumerator
{
public:
    MFFontFileEnumerator(IDWriteFactory* factory);
    HRESULT Initialize(UINT const* collectionKey, UINT32 keySize);

    ~MFFontFileEnumerator()
    {
        SafeRelease(&currentFile_);
        SafeRelease(&factory_);
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT STDMETHODCALLTYPE MoveNext(OUT BOOL* hasCurrentFile);
    virtual HRESULT STDMETHODCALLTYPE GetCurrentFontFile(OUT IDWriteFontFile** fontFile);

private:
    ULONG refCount_;
    IDWriteFactory* factory_;
    IDWriteFontFile* currentFile_;
    std::vector<std::wstring> filePaths_;
    size_t nextIndex_;
};

class MFFontContext
{
public:
    MFFontContext(IDWriteFactory *pFactory);
    ~MFFontContext();
    HRESULT Initialize();
    HRESULT CreateFontCollection(MFCollection &newCollection, OUT IDWriteFontCollection** result);

private:    
    MFFontContext(MFFontContext const&);
    void operator=(MFFontContext const&);
    HRESULT InitializeInternal();
    IDWriteFactory *g_dwriteFactory;
    static std::vector<unsigned int> cKeys;
    HRESULT hr_;
};

class MFFontGlobals
{
public:
    MFFontGlobals() { }
    static unsigned int push(MFCollection &addCollection)
    {
		unsigned ret = static_cast<unsigned>(fontCollections.size());
        fontCollections.push_back(addCollection);
        return ret;
    }

    static std::vector<MFCollection>& collections()
    {
        return fontCollections;
    }

private:
    static std::vector<MFCollection> fontCollections;
};
