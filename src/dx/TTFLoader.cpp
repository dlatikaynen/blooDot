#include "TTFLoader.h"

IDWriteFontCollectionLoader* MFFontCollectionLoader::instance_(new(std::nothrow) MFFontCollectionLoader());

HRESULT STDMETHODCALLTYPE MFFontCollectionLoader::QueryInterface(REFIID iid, void** ppvObject)
{
    if (iid == IID_IUnknown || iid == __uuidof(IDWriteFontCollectionLoader))
    {
        *ppvObject = this;
        this->AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE MFFontCollectionLoader::AddRef()
{
    return InterlockedIncrement(&this->refCount_);
}

ULONG STDMETHODCALLTYPE MFFontCollectionLoader::Release()
{
    ULONG newCount = InterlockedDecrement(&this->refCount_);
	if (newCount == 0)
	{
		delete this;
	}

    return newCount;
}

HRESULT STDMETHODCALLTYPE MFFontCollectionLoader::CreateEnumeratorFromKey(IDWriteFactory* factory, void const* collectionKey, UINT32 collectionKeySize, OUT IDWriteFontFileEnumerator** fontFileEnumerator)
{
    *fontFileEnumerator = NULL;
    HRESULT hr = S_OK;
	if (collectionKeySize % sizeof(UINT) != 0)
	{
		return E_INVALIDARG;
	}

    MFFontFileEnumerator* enumerator = new(std::nothrow) MFFontFileEnumerator(factory);
	if (enumerator == NULL)
	{
		return E_OUTOFMEMORY;
	}

    UINT const* mfCollectionKey = static_cast<UINT const*>(collectionKey);
    UINT32 const mfKeySize = collectionKeySize;
    hr = enumerator->Initialize(mfCollectionKey, mfKeySize);
    if (FAILED(hr))
    {
        delete enumerator;
        return hr;
    }

    *fontFileEnumerator = SafeAcquire(enumerator);
    return hr;
}

MFFontFileEnumerator::MFFontFileEnumerator(IDWriteFactory* factory) :
	refCount_(0),
    factory_(SafeAcquire(factory)),
    currentFile_(),
    nextIndex_(0)
{ }

HRESULT MFFontFileEnumerator::Initialize(UINT const* collectionKey, UINT32 keySize)
{
    try
    {
        UINT cPos = *collectionKey;
        for (MFCollection::iterator it = MFFontGlobals::collections()[cPos].begin(); it != MFFontGlobals::collections()[cPos].end(); ++it)
        {
			this->filePaths_.push_back(it->c_str());
        }
    }
    catch (...)
    {
		try
		{
			throw;
		}
		catch (std::bad_alloc&)
		{
			return E_OUTOFMEMORY;
		}
		catch (...)
		{
			return E_FAIL;
		}
	}

    return S_OK;
}

HRESULT STDMETHODCALLTYPE MFFontFileEnumerator::QueryInterface(REFIID iid, void** ppvObject)
{
    if (iid == IID_IUnknown || iid == __uuidof(IDWriteFontFileEnumerator))
    {
        *ppvObject = this;
        this->AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE MFFontFileEnumerator::AddRef()
{
    return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE MFFontFileEnumerator::Release()
{
    ULONG newCount = InterlockedDecrement(&refCount_);
	if (newCount == 0)
	{
		delete this;
	}

    return newCount;
}

HRESULT STDMETHODCALLTYPE MFFontFileEnumerator::MoveNext(OUT BOOL* hasCurrentFile)
{
    HRESULT hr = S_OK;
    *hasCurrentFile = FALSE;
    SafeRelease(&this->currentFile_);
    if (this->nextIndex_ < this->filePaths_.size())
    {
        hr = factory_->CreateFontFileReference(filePaths_[this->nextIndex_].c_str(), NULL, &this->currentFile_);
        if (SUCCEEDED(hr))
        {
            *hasCurrentFile = TRUE;
            ++this->nextIndex_;
        }
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE MFFontFileEnumerator::GetCurrentFontFile(OUT IDWriteFontFile** fontFile)
{
    *fontFile = SafeAcquire(this->currentFile_);
    return (this->currentFile_ != NULL) ? S_OK : E_FAIL;
}

MFFontContext::MFFontContext(IDWriteFactory *pFactory) : hr_(S_FALSE), g_dwriteFactory(pFactory) { }

MFFontContext::~MFFontContext()
{
    g_dwriteFactory->UnregisterFontCollectionLoader(MFFontCollectionLoader::GetLoader());
}

HRESULT MFFontContext::Initialize()
{
    if (this->hr_ == S_FALSE)
    {
		this->hr_ = this->InitializeInternal();
    }

    return this->hr_;
}

HRESULT MFFontContext::InitializeInternal()
{
    HRESULT hr = S_OK;
    if (!MFFontCollectionLoader::IsLoaderInitialized())
    {
        return E_FAIL;
    }

    hr = this->g_dwriteFactory->RegisterFontCollectionLoader(MFFontCollectionLoader::GetLoader());
    return hr;
}

HRESULT MFFontContext::CreateFontCollection(MFCollection &newCollection, OUT IDWriteFontCollection** result)
{
    *result = NULL;
    UINT collectionKey = MFFontGlobals::push(newCollection);
	this->cKeys.push_back(collectionKey);
    const void *fontCollectionKey = &this->cKeys.back();
    UINT32 keySize = sizeof(collectionKey);
	HRESULT hr = this->Initialize();
	if (FAILED(hr))
	{
		return hr;
	}

    hr = g_dwriteFactory->CreateCustomFontCollection(MFFontCollectionLoader::GetLoader(), fontCollectionKey, keySize, result);
    return hr;
}

std::vector<unsigned> MFFontContext::cKeys = std::vector<unsigned>(0);
std::vector<MFCollection> MFFontGlobals::fontCollections = std::vector<MFCollection>(0);