#pragma once

//---------------------------------------------------------------------------//
//
// ClassFactory.hpp
//
//---------------------------------------------------------------------------//

#include <windows.h>

//---------------------------------------------------------------------------//

extern ULONG g_cLocks = 0;

//---------------------------------------------------------------------------//

template <class T>
class CClassFactory : public IClassFactory
{
public:
    CClassFactory();
    ~CClassFactory();

public:
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject) override;
    ULONG   __stdcall AddRef() override;
    ULONG   __stdcall Release() override;

    HRESULT __stdcall CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override;
    HRESULT __stdcall LockServer(BOOL fLock) override;

private:
    CClassFactory(const CClassFactory&)             = delete;
    CClassFactory(CClassFactory&&)                  = delete;
    CClassFactory& operator =(const CClassFactory&) = delete;
    CClassFactory& operator =(CClassFactory&&)      = delete;
};

//---------------------------------------------------------------------------//

template <class T> CClassFactory<T>::CClassFactory()
{
    static_assert(std::is_base_of<IUnknown, T>::value, "<T> needs to be IUnknown based");
}

//---------------------------------------------------------------------------//

template <class T> CClassFactory<T>::~CClassFactory()
{
}

//---------------------------------------------------------------------------//

template <class T> STDMETHODIMP CClassFactory<T>::QueryInterface
(
    REFIID riid,
    void** ppvObject
)
{
    if ( nullptr == ppvObject )
    {
        return E_POINTER;
    }

    *ppvObject = nullptr;

    if ( IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory) )
    {
        *ppvObject = static_cast<IClassFactory*>(this);
    }
    else
    {
        return E_NOINTERFACE;
    }

    this->AddRef();

    return S_OK;
}

//---------------------------------------------------------------------------//

template <class T> STDMETHODIMP_(ULONG) CClassFactory<T>::AddRef()
{
    this->LockServer(TRUE);

    return 2;
}

//---------------------------------------------------------------------------//

template <class T> STDMETHODIMP_(ULONG) CClassFactory<T>::Release()
{
    this->LockServer(FALSE);

    return 1;
}

//---------------------------------------------------------------------------//

template<class T> STDMETHODIMP CClassFactory<T>::CreateInstance
(
     IUnknown* pUnkOuter,
     REFIID    riid,
     void**    ppvObject
)
{
    UNREFERENCED_PARAMETER(pUnkOuter);

    if ( nullptr == ppvObject )
    {
        return E_POINTER;
    }

    *ppvObject = nullptr;

    const auto comp = new T;
    if ( nullptr == comp )
    {
        return E_OUTOFMEMORY;
    }

    const auto hr = comp->QueryInterface(riid, ppvObject);
    comp->Release();

    return hr;
}

//---------------------------------------------------------------------------//

template <class T> STDMETHODIMP CClassFactory<T>::LockServer
(
    BOOL bLock
)
{
    if ( bLock )
    {
         ::InterlockedIncrement(&g_cLocks);
    }
    else
    {
        ::InterlockedDecrement(&g_cLocks);
    }

     return S_OK;
}

//---------------------------------------------------------------------------//

// ClassFactory.hpp