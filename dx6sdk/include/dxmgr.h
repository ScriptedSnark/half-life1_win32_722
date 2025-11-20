/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __dxmgr_h__
#define __dxmgr_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IEnumDirectDraws_FWD_DEFINED__
#define __IEnumDirectDraws_FWD_DEFINED__
typedef interface IEnumDirectDraws IEnumDirectDraws;
#endif 	/* __IEnumDirectDraws_FWD_DEFINED__ */


#ifndef __IEnumPixelFormats_FWD_DEFINED__
#define __IEnumPixelFormats_FWD_DEFINED__
typedef interface IEnumPixelFormats IEnumPixelFormats;
#endif 	/* __IEnumPixelFormats_FWD_DEFINED__ */


#ifndef __IEnumDisplayModes_FWD_DEFINED__
#define __IEnumDisplayModes_FWD_DEFINED__
typedef interface IEnumDisplayModes IEnumDisplayModes;
#endif 	/* __IEnumDisplayModes_FWD_DEFINED__ */


#ifndef __IDXMgr_FWD_DEFINED__
#define __IDXMgr_FWD_DEFINED__
typedef interface IDXMgr IDXMgr;
#endif 	/* __IDXMgr_FWD_DEFINED__ */


#ifndef __DXMgr_FWD_DEFINED__
#define __DXMgr_FWD_DEFINED__

#ifdef __cplusplus
typedef class DXMgr DXMgr;
#else
typedef struct DXMgr DXMgr;
#endif /* __cplusplus */

#endif 	/* __DXMgr_FWD_DEFINED__ */


#ifndef __EnumDirectDraws_FWD_DEFINED__
#define __EnumDirectDraws_FWD_DEFINED__

#ifdef __cplusplus
typedef class EnumDirectDraws EnumDirectDraws;
#else
typedef struct EnumDirectDraws EnumDirectDraws;
#endif /* __cplusplus */

#endif 	/* __EnumDirectDraws_FWD_DEFINED__ */


#ifndef __EnumPixelFormats_FWD_DEFINED__
#define __EnumPixelFormats_FWD_DEFINED__

#ifdef __cplusplus
typedef class EnumPixelFormats EnumPixelFormats;
#else
typedef struct EnumPixelFormats EnumPixelFormats;
#endif /* __cplusplus */

#endif 	/* __EnumPixelFormats_FWD_DEFINED__ */


#ifndef __EnumDisplayModes_FWD_DEFINED__
#define __EnumDisplayModes_FWD_DEFINED__

#ifdef __cplusplus
typedef class EnumDisplayModes EnumDisplayModes;
#else
typedef struct EnumDisplayModes EnumDisplayModes;
#endif /* __cplusplus */

#endif 	/* __EnumDisplayModes_FWD_DEFINED__ */


/* header files for imported files */
#include "wtypes.h"
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_dxmgr_0000 */
/* [local] */ 

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
////////////////////////////////////////////////////////////////////////////////
//
// File: DXMgr.h
//
// Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#include <ddraw.h>
#include <d3d.h>
#include <d3dtypes.h>
#include <d3dcaps.h>


typedef struct  _DXMGRMODEDESC
    {
    DWORD dwWidth;
    DWORD dwHeight;
    DWORD dwBPP;
    DWORD dwRefreshRate;
    }	DXMGRMODEDESC;

typedef struct _DXMGRMODEDESC __RPC_FAR *PDXMGRMODEDESC;

typedef struct  _DXMGRENABLE
    {
    UINT uFlags;
    UINT uDDSCLFlags;
    HWND hwndDevice;
    DXMGRMODEDESC DisplayMode;
    UINT cBackBuffers;
    IID iidDeviceType;
    DDPIXELFORMAT ddpfZBuffer;
    float fNear;
    float fFar;
    }	DXMGRENABLE;

typedef struct _DXMGRENABLE __RPC_FAR *PDXMGRENABLE;

typedef struct  _DXMGROBJECTDESC
    {
    GUID guid;
    LPSTR lpszDriverName;
    LPSTR lpszDriverDesc;
    HMONITOR hMonitor;
    }	DXMGROBJECTDESC;

typedef struct _DXMGROBJECTDESC __RPC_FAR *PDXMGROBJECTDESC;

extern HRESULT WINAPI DXMgrCreate( REFIID, LPVOID *ppDXMgr );
#define DXMGRENABLE_FULLSCREEN      0x00000001L
#define DXMGRENABLE_DISPLAYMODE     0x00000002L
#define DXMGRENABLE_BACKBUFFERCOUNT 0x00000004L
#define DXMGRENABLE_3D              0x00000008L
#define DXMGRENABLE_ZBUFFER         0x00000010L
#define DXMGRENABLE_DDPFZBUFFER     0x00000020L
#define DXMGRENABLE_DEVICETYPE      0x00000040L
#define DXMGRENABLE_FORCEVIDEOMEMORY 0x00000080L
typedef struct  _DXMGRENABLEINFO
    {
    UINT uWidth;
    UINT uHeight;
    DXMGRMODEDESC DisplayMode;
    DDPIXELFORMAT ddpfPrimary;
    DDPIXELFORMAT ddpfZBuffer;
    IID iidDeviceType;
    }	DXMGRENABLEINFO;

typedef DXMGRENABLEINFO __RPC_FAR *PDXMGRENABLEINFO;

#define MAKE_DXMGRERR( code ) MAKE_HRESULT( 1, FACILITY_ITF, code )
#define DXMGRERR_NODIRECTX6         MAKE_DXMGRERR( 0x201 )
#define DXMGRERR_NOTINITIALIZED     MAKE_DXMGRERR( 0x206 )
#define DXMGRERR_NOTENABLED         MAKE_DXMGRERR( 0x207 )
#define DXMGRERR_ALREADYINITIALIZED MAKE_DXMGRERR( 0x208 )
#define DXMGRERR_ALREADYENABLED     MAKE_DXMGRERR( 0x209 )
#define DXMGRERR_INVALIDFOCUSWINDOW MAKE_DXMGRERR( 0x20a )
#define DXMGRERR_WINDOWEDREQUIRED   MAKE_DXMGRERR( 0x20f )
#define DXMGRERR_FULLSCREENREQUIRED MAKE_DXMGRERR( 0x210 )
#define DXMGRERR_3DREQUIRED         MAKE_DXMGRERR( 0x211 )
#define DXMGRERR_BADPIXELFORMAT     MAKE_DXMGRERR( 0x214 )
#ifndef MIDL_INTERFACE
#if _MSC_VER >= 1100
#define DECLSPEC_UUID(x)    __declspec(uuid(x))
#define MIDL_INTERFACE(x)   struct __declspec(uuid(x)) __declspec(novtable)
#else
#define DECLSPEC_UUID(x)
#define MIDL_INTERFACE(x)   struct
#endif

#if _MSC_VER >= 1100
#define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8)  \
  EXTERN_C const IID __declspec(selectany) itf = {l1,s1,s2,{c1,c2,c3,c4,c5,c6,c7,c8}}
#else
#define EXTERN_GUID(itf,l1,s1,s2,c1,c2,c3,c4,c5,c6,c7,c8) EXTERN_C const IID itf
#endif
#endif


extern RPC_IF_HANDLE __MIDL_itf_dxmgr_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dxmgr_0000_v0_0_s_ifspec;

#ifndef __IEnumDirectDraws_INTERFACE_DEFINED__
#define __IEnumDirectDraws_INTERFACE_DEFINED__

/* interface IEnumDirectDraws */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IEnumDirectDraws;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5FF13307-FCB9-11D1-B233-0000F81FDF9E")
    IEnumDirectDraws : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ UINT celt,
            /* [out] */ PDXMGROBJECTDESC pDDObjs,
            /* [out] */ UINT __RPC_FAR *pceltFetched) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ UINT celt) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDirectDraws __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDirectDrawsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumDirectDraws __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumDirectDraws __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumDirectDraws __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumDirectDraws __RPC_FAR * This,
            /* [in] */ UINT celt,
            /* [out] */ PDXMGROBJECTDESC pDDObjs,
            /* [out] */ UINT __RPC_FAR *pceltFetched);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumDirectDraws __RPC_FAR * This,
            /* [in] */ UINT celt);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumDirectDraws __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumDirectDraws __RPC_FAR * This,
            /* [out] */ IEnumDirectDraws __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumDirectDrawsVtbl;

    interface IEnumDirectDraws
    {
        CONST_VTBL struct IEnumDirectDrawsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDirectDraws_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDirectDraws_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDirectDraws_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDirectDraws_Next(This,celt,pDDObjs,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,pDDObjs,pceltFetched)

#define IEnumDirectDraws_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDirectDraws_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDirectDraws_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumDirectDraws_Next_Proxy( 
    IEnumDirectDraws __RPC_FAR * This,
    /* [in] */ UINT celt,
    /* [out] */ PDXMGROBJECTDESC pDDObjs,
    /* [out] */ UINT __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumDirectDraws_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumDirectDraws_Skip_Proxy( 
    IEnumDirectDraws __RPC_FAR * This,
    /* [in] */ UINT celt);


void __RPC_STUB IEnumDirectDraws_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumDirectDraws_Reset_Proxy( 
    IEnumDirectDraws __RPC_FAR * This);


void __RPC_STUB IEnumDirectDraws_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumDirectDraws_Clone_Proxy( 
    IEnumDirectDraws __RPC_FAR * This,
    /* [out] */ IEnumDirectDraws __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumDirectDraws_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDirectDraws_INTERFACE_DEFINED__ */


#ifndef __IEnumPixelFormats_INTERFACE_DEFINED__
#define __IEnumPixelFormats_INTERFACE_DEFINED__

/* interface IEnumPixelFormats */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IEnumPixelFormats;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5FF13337-FCB9-11D1-B233-0000F81FDF9E")
    IEnumPixelFormats : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ UINT celt,
            /* [out] */ LPDDPIXELFORMAT __MIDL_0011,
            /* [out] */ UINT __RPC_FAR *pceltFetched) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ UINT celt) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumPixelFormats __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumPixelFormatsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumPixelFormats __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumPixelFormats __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumPixelFormats __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumPixelFormats __RPC_FAR * This,
            /* [in] */ UINT celt,
            /* [out] */ LPDDPIXELFORMAT __MIDL_0011,
            /* [out] */ UINT __RPC_FAR *pceltFetched);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumPixelFormats __RPC_FAR * This,
            /* [in] */ UINT celt);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumPixelFormats __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumPixelFormats __RPC_FAR * This,
            /* [out] */ IEnumPixelFormats __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumPixelFormatsVtbl;

    interface IEnumPixelFormats
    {
        CONST_VTBL struct IEnumPixelFormatsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumPixelFormats_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumPixelFormats_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumPixelFormats_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumPixelFormats_Next(This,celt,__MIDL_0011,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,__MIDL_0011,pceltFetched)

#define IEnumPixelFormats_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumPixelFormats_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumPixelFormats_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumPixelFormats_Next_Proxy( 
    IEnumPixelFormats __RPC_FAR * This,
    /* [in] */ UINT celt,
    /* [out] */ LPDDPIXELFORMAT __MIDL_0011,
    /* [out] */ UINT __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumPixelFormats_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumPixelFormats_Skip_Proxy( 
    IEnumPixelFormats __RPC_FAR * This,
    /* [in] */ UINT celt);


void __RPC_STUB IEnumPixelFormats_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumPixelFormats_Reset_Proxy( 
    IEnumPixelFormats __RPC_FAR * This);


void __RPC_STUB IEnumPixelFormats_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumPixelFormats_Clone_Proxy( 
    IEnumPixelFormats __RPC_FAR * This,
    /* [out] */ IEnumPixelFormats __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumPixelFormats_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumPixelFormats_INTERFACE_DEFINED__ */


#ifndef __IEnumDisplayModes_INTERFACE_DEFINED__
#define __IEnumDisplayModes_INTERFACE_DEFINED__

/* interface IEnumDisplayModes */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IEnumDisplayModes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B4E847C0-FF1A-11D1-B233-0000F81FDF9E")
    IEnumDisplayModes : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ UINT celt,
            /* [out] */ PDXMGRMODEDESC __MIDL_0012,
            /* [out] */ UINT __RPC_FAR *pceltFetched) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ UINT celt) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDisplayModes __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDisplayModesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumDisplayModes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumDisplayModes __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumDisplayModes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumDisplayModes __RPC_FAR * This,
            /* [in] */ UINT celt,
            /* [out] */ PDXMGRMODEDESC __MIDL_0012,
            /* [out] */ UINT __RPC_FAR *pceltFetched);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumDisplayModes __RPC_FAR * This,
            /* [in] */ UINT celt);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumDisplayModes __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumDisplayModes __RPC_FAR * This,
            /* [out] */ IEnumDisplayModes __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumDisplayModesVtbl;

    interface IEnumDisplayModes
    {
        CONST_VTBL struct IEnumDisplayModesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDisplayModes_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDisplayModes_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDisplayModes_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDisplayModes_Next(This,celt,__MIDL_0012,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,__MIDL_0012,pceltFetched)

#define IEnumDisplayModes_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDisplayModes_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDisplayModes_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumDisplayModes_Next_Proxy( 
    IEnumDisplayModes __RPC_FAR * This,
    /* [in] */ UINT celt,
    /* [out] */ PDXMGRMODEDESC __MIDL_0012,
    /* [out] */ UINT __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumDisplayModes_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumDisplayModes_Skip_Proxy( 
    IEnumDisplayModes __RPC_FAR * This,
    /* [in] */ UINT celt);


void __RPC_STUB IEnumDisplayModes_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumDisplayModes_Reset_Proxy( 
    IEnumDisplayModes __RPC_FAR * This);


void __RPC_STUB IEnumDisplayModes_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumDisplayModes_Clone_Proxy( 
    IEnumDisplayModes __RPC_FAR * This,
    /* [out] */ IEnumDisplayModes __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumDisplayModes_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDisplayModes_INTERFACE_DEFINED__ */


#ifndef __IDXMgr_INTERFACE_DEFINED__
#define __IDXMgr_INTERFACE_DEFINED__

/* interface IDXMgr */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IDXMgr;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("65E51761-EBA8-11D1-AA78-00600814AAE9")
    IDXMgr : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clear( 
            /* [in] */ UINT cNumRects,
            /* [in] */ PRECT pRect,
            /* [in] */ UINT D3DClearFlags,
            /* [in] */ D3DCOLOR ClearColor,
            /* [in] */ D3DVALUE dvZClearValue,
            /* [in] */ UINT StencilClearValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateEnumDirectDraws( 
            /* [out] */ IEnumDirectDraws __RPC_FAR *__RPC_FAR *__MIDL_0013,
            /* [in] */ UINT Flags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateEnumDisplayModes( 
            /* [out] */ IEnumDisplayModes __RPC_FAR *__RPC_FAR *__MIDL_0014,
            /* [in] */ UINT Flags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateEnumTextureFormats( 
            /* [out] */ IEnumPixelFormats __RPC_FAR *__RPC_FAR *__MIDL_0015,
            /* [in] */ UINT Flags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateEnumZBufferFormats( 
            /* [out] */ IEnumPixelFormats __RPC_FAR *__RPC_FAR *__MIDL_0016,
            /* [in] */ const IID __RPC_FAR *piidDeviceType,
            /* [in] */ UINT Flags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Enable( 
            /* [in] */ PDXMGRENABLE pEnable) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Disable( 
            /* [in] */ UINT flags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FlipToGDISurface( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDeviceCaps( 
            /* [out] */ LPD3DDEVICEDESC pDesc) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetEnableInfo( 
            /* [out] */ PDXMGRENABLEINFO pEnableInfo) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetFlags( 
            /* [in] */ UINT uWhich,
            /* [out] */ UINT __RPC_FAR *puFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetFocusWindow( 
            /* [out] */ HWND __RPC_FAR *phwnd) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDeviceWindow( 
            /* [out] */ HWND __RPC_FAR *phwnd) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDirectDraw( 
            /* [out] */ LPDIRECTDRAW4 __RPC_FAR *ppDD) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetPrimary( 
            /* [out] */ LPDIRECTDRAWSURFACE4 __RPC_FAR *ppDDS) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetBackBuffer( 
            /* [out] */ LPDIRECTDRAWSURFACE4 __RPC_FAR *ppDDS) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetZBuffer( 
            /* [out] */ LPDIRECTDRAWSURFACE4 __RPC_FAR *ppDDS) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDirect3D( 
            /* [out] */ LPDIRECT3D3 __RPC_FAR *ppD3D) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDirect3DDevice( 
            /* [out] */ LPDIRECT3DDEVICE3 __RPC_FAR *ppDevice) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ LPGUID pDDGuid,
            /* [in] */ HWND hwndFocus,
            /* [in] */ UINT flags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Initialize2( 
            /* [in] */ LPDIRECTDRAW4 pDD,
            /* [in] */ HWND hwndFocus,
            /* [in] */ UINT flags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RestoreSurfaces( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Update( 
            /* [in] */ UINT uFlags,
            /* [in] */ PRECT pRect) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDXMgrVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDXMgr __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDXMgr __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDXMgr __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clear )( 
            IDXMgr __RPC_FAR * This,
            /* [in] */ UINT cNumRects,
            /* [in] */ PRECT pRect,
            /* [in] */ UINT D3DClearFlags,
            /* [in] */ D3DCOLOR ClearColor,
            /* [in] */ D3DVALUE dvZClearValue,
            /* [in] */ UINT StencilClearValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateEnumDirectDraws )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ IEnumDirectDraws __RPC_FAR *__RPC_FAR *__MIDL_0013,
            /* [in] */ UINT Flags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateEnumDisplayModes )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ IEnumDisplayModes __RPC_FAR *__RPC_FAR *__MIDL_0014,
            /* [in] */ UINT Flags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateEnumTextureFormats )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ IEnumPixelFormats __RPC_FAR *__RPC_FAR *__MIDL_0015,
            /* [in] */ UINT Flags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateEnumZBufferFormats )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ IEnumPixelFormats __RPC_FAR *__RPC_FAR *__MIDL_0016,
            /* [in] */ const IID __RPC_FAR *piidDeviceType,
            /* [in] */ UINT Flags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Enable )( 
            IDXMgr __RPC_FAR * This,
            /* [in] */ PDXMGRENABLE pEnable);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disable )( 
            IDXMgr __RPC_FAR * This,
            /* [in] */ UINT flags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FlipToGDISurface )( 
            IDXMgr __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDeviceCaps )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ LPD3DDEVICEDESC pDesc);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEnableInfo )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ PDXMGRENABLEINFO pEnableInfo);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFlags )( 
            IDXMgr __RPC_FAR * This,
            /* [in] */ UINT uWhich,
            /* [out] */ UINT __RPC_FAR *puFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetFocusWindow )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ HWND __RPC_FAR *phwnd);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDeviceWindow )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ HWND __RPC_FAR *phwnd);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDirectDraw )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ LPDIRECTDRAW4 __RPC_FAR *ppDD);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPrimary )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ LPDIRECTDRAWSURFACE4 __RPC_FAR *ppDDS);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetBackBuffer )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ LPDIRECTDRAWSURFACE4 __RPC_FAR *ppDDS);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetZBuffer )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ LPDIRECTDRAWSURFACE4 __RPC_FAR *ppDDS);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDirect3D )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ LPDIRECT3D3 __RPC_FAR *ppD3D);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDirect3DDevice )( 
            IDXMgr __RPC_FAR * This,
            /* [out] */ LPDIRECT3DDEVICE3 __RPC_FAR *ppDevice);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Initialize )( 
            IDXMgr __RPC_FAR * This,
            /* [in] */ LPGUID pDDGuid,
            /* [in] */ HWND hwndFocus,
            /* [in] */ UINT flags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Initialize2 )( 
            IDXMgr __RPC_FAR * This,
            /* [in] */ LPDIRECTDRAW4 pDD,
            /* [in] */ HWND hwndFocus,
            /* [in] */ UINT flags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RestoreSurfaces )( 
            IDXMgr __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            IDXMgr __RPC_FAR * This,
            /* [in] */ UINT uFlags,
            /* [in] */ PRECT pRect);
        
        END_INTERFACE
    } IDXMgrVtbl;

    interface IDXMgr
    {
        CONST_VTBL struct IDXMgrVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDXMgr_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDXMgr_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDXMgr_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDXMgr_Clear(This,cNumRects,pRect,D3DClearFlags,ClearColor,dvZClearValue,StencilClearValue)	\
    (This)->lpVtbl -> Clear(This,cNumRects,pRect,D3DClearFlags,ClearColor,dvZClearValue,StencilClearValue)

#define IDXMgr_CreateEnumDirectDraws(This,__MIDL_0013,Flags)	\
    (This)->lpVtbl -> CreateEnumDirectDraws(This,__MIDL_0013,Flags)

#define IDXMgr_CreateEnumDisplayModes(This,__MIDL_0014,Flags)	\
    (This)->lpVtbl -> CreateEnumDisplayModes(This,__MIDL_0014,Flags)

#define IDXMgr_CreateEnumTextureFormats(This,__MIDL_0015,Flags)	\
    (This)->lpVtbl -> CreateEnumTextureFormats(This,__MIDL_0015,Flags)

#define IDXMgr_CreateEnumZBufferFormats(This,__MIDL_0016,piidDeviceType,Flags)	\
    (This)->lpVtbl -> CreateEnumZBufferFormats(This,__MIDL_0016,piidDeviceType,Flags)

#define IDXMgr_Enable(This,pEnable)	\
    (This)->lpVtbl -> Enable(This,pEnable)

#define IDXMgr_Disable(This,flags)	\
    (This)->lpVtbl -> Disable(This,flags)

#define IDXMgr_FlipToGDISurface(This)	\
    (This)->lpVtbl -> FlipToGDISurface(This)

#define IDXMgr_GetDeviceCaps(This,pDesc)	\
    (This)->lpVtbl -> GetDeviceCaps(This,pDesc)

#define IDXMgr_GetEnableInfo(This,pEnableInfo)	\
    (This)->lpVtbl -> GetEnableInfo(This,pEnableInfo)

#define IDXMgr_GetFlags(This,uWhich,puFlags)	\
    (This)->lpVtbl -> GetFlags(This,uWhich,puFlags)

#define IDXMgr_GetFocusWindow(This,phwnd)	\
    (This)->lpVtbl -> GetFocusWindow(This,phwnd)

#define IDXMgr_GetDeviceWindow(This,phwnd)	\
    (This)->lpVtbl -> GetDeviceWindow(This,phwnd)

#define IDXMgr_GetDirectDraw(This,ppDD)	\
    (This)->lpVtbl -> GetDirectDraw(This,ppDD)

#define IDXMgr_GetPrimary(This,ppDDS)	\
    (This)->lpVtbl -> GetPrimary(This,ppDDS)

#define IDXMgr_GetBackBuffer(This,ppDDS)	\
    (This)->lpVtbl -> GetBackBuffer(This,ppDDS)

#define IDXMgr_GetZBuffer(This,ppDDS)	\
    (This)->lpVtbl -> GetZBuffer(This,ppDDS)

#define IDXMgr_GetDirect3D(This,ppD3D)	\
    (This)->lpVtbl -> GetDirect3D(This,ppD3D)

#define IDXMgr_GetDirect3DDevice(This,ppDevice)	\
    (This)->lpVtbl -> GetDirect3DDevice(This,ppDevice)

#define IDXMgr_Initialize(This,pDDGuid,hwndFocus,flags)	\
    (This)->lpVtbl -> Initialize(This,pDDGuid,hwndFocus,flags)

#define IDXMgr_Initialize2(This,pDD,hwndFocus,flags)	\
    (This)->lpVtbl -> Initialize2(This,pDD,hwndFocus,flags)

#define IDXMgr_RestoreSurfaces(This)	\
    (This)->lpVtbl -> RestoreSurfaces(This)

#define IDXMgr_Update(This,uFlags,pRect)	\
    (This)->lpVtbl -> Update(This,uFlags,pRect)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_Clear_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [in] */ UINT cNumRects,
    /* [in] */ PRECT pRect,
    /* [in] */ UINT D3DClearFlags,
    /* [in] */ D3DCOLOR ClearColor,
    /* [in] */ D3DVALUE dvZClearValue,
    /* [in] */ UINT StencilClearValue);


void __RPC_STUB IDXMgr_Clear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_CreateEnumDirectDraws_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ IEnumDirectDraws __RPC_FAR *__RPC_FAR *__MIDL_0013,
    /* [in] */ UINT Flags);


void __RPC_STUB IDXMgr_CreateEnumDirectDraws_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_CreateEnumDisplayModes_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ IEnumDisplayModes __RPC_FAR *__RPC_FAR *__MIDL_0014,
    /* [in] */ UINT Flags);


void __RPC_STUB IDXMgr_CreateEnumDisplayModes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_CreateEnumTextureFormats_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ IEnumPixelFormats __RPC_FAR *__RPC_FAR *__MIDL_0015,
    /* [in] */ UINT Flags);


void __RPC_STUB IDXMgr_CreateEnumTextureFormats_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_CreateEnumZBufferFormats_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ IEnumPixelFormats __RPC_FAR *__RPC_FAR *__MIDL_0016,
    /* [in] */ const IID __RPC_FAR *piidDeviceType,
    /* [in] */ UINT Flags);


void __RPC_STUB IDXMgr_CreateEnumZBufferFormats_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_Enable_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [in] */ PDXMGRENABLE pEnable);


void __RPC_STUB IDXMgr_Enable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_Disable_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [in] */ UINT flags);


void __RPC_STUB IDXMgr_Disable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_FlipToGDISurface_Proxy( 
    IDXMgr __RPC_FAR * This);


void __RPC_STUB IDXMgr_FlipToGDISurface_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_GetDeviceCaps_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ LPD3DDEVICEDESC pDesc);


void __RPC_STUB IDXMgr_GetDeviceCaps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_GetEnableInfo_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ PDXMGRENABLEINFO pEnableInfo);


void __RPC_STUB IDXMgr_GetEnableInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_GetFlags_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [in] */ UINT uWhich,
    /* [out] */ UINT __RPC_FAR *puFlags);


void __RPC_STUB IDXMgr_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_GetFocusWindow_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ HWND __RPC_FAR *phwnd);


void __RPC_STUB IDXMgr_GetFocusWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_GetDeviceWindow_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ HWND __RPC_FAR *phwnd);


void __RPC_STUB IDXMgr_GetDeviceWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_GetDirectDraw_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ LPDIRECTDRAW4 __RPC_FAR *ppDD);


void __RPC_STUB IDXMgr_GetDirectDraw_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_GetPrimary_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ LPDIRECTDRAWSURFACE4 __RPC_FAR *ppDDS);


void __RPC_STUB IDXMgr_GetPrimary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_GetBackBuffer_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ LPDIRECTDRAWSURFACE4 __RPC_FAR *ppDDS);


void __RPC_STUB IDXMgr_GetBackBuffer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_GetZBuffer_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ LPDIRECTDRAWSURFACE4 __RPC_FAR *ppDDS);


void __RPC_STUB IDXMgr_GetZBuffer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_GetDirect3D_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ LPDIRECT3D3 __RPC_FAR *ppD3D);


void __RPC_STUB IDXMgr_GetDirect3D_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_GetDirect3DDevice_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [out] */ LPDIRECT3DDEVICE3 __RPC_FAR *ppDevice);


void __RPC_STUB IDXMgr_GetDirect3DDevice_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_Initialize_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [in] */ LPGUID pDDGuid,
    /* [in] */ HWND hwndFocus,
    /* [in] */ UINT flags);


void __RPC_STUB IDXMgr_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_Initialize2_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [in] */ LPDIRECTDRAW4 pDD,
    /* [in] */ HWND hwndFocus,
    /* [in] */ UINT flags);


void __RPC_STUB IDXMgr_Initialize2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_RestoreSurfaces_Proxy( 
    IDXMgr __RPC_FAR * This);


void __RPC_STUB IDXMgr_RestoreSurfaces_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDXMgr_Update_Proxy( 
    IDXMgr __RPC_FAR * This,
    /* [in] */ UINT uFlags,
    /* [in] */ PRECT pRect);


void __RPC_STUB IDXMgr_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDXMgr_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_dxmgr_0235 */
/* [local] */ 

#define DXMGRGF_INIT      0x00000001L
#define DXMGRGF_ENABLE    0x00000002L
#define DXMGREDD_ATTACHEDSECONDARYDEVICES   0x00000001L
#define DXMGREDD_DETACHEDSECONDARYDEVICES   0x00000002L
#define DXMGREDD_NONDISPLAYDEVICES          0x00000004L
#define DXMGRUPDATE_DONOTWAIT    0x00000001L
#define DXMGRUPDATE_FORCEBLT 0x00000002L


extern RPC_IF_HANDLE __MIDL_itf_dxmgr_0235_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dxmgr_0235_v0_0_s_ifspec;


#ifndef __DXMGRLib_LIBRARY_DEFINED__
#define __DXMGRLib_LIBRARY_DEFINED__

/* library DXMGRLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DXMGRLib;

EXTERN_C const CLSID CLSID_DXMgr;

#ifdef __cplusplus

class DECLSPEC_UUID("65E51762-EBA8-11D1-AA78-00600814AAE9")
DXMgr;
#endif

EXTERN_C const CLSID CLSID_EnumDirectDraws;

#ifdef __cplusplus

class DECLSPEC_UUID("5FF13308-FCB9-11D1-B233-0000F81FDF9E")
EnumDirectDraws;
#endif

EXTERN_C const CLSID CLSID_EnumPixelFormats;

#ifdef __cplusplus

class DECLSPEC_UUID("5FF13338-FCB9-11D1-B233-0000F81FDF9E")
EnumPixelFormats;
#endif

EXTERN_C const CLSID CLSID_EnumDisplayModes;

#ifdef __cplusplus

class DECLSPEC_UUID("B4E847C1-FF1A-11D1-B233-0000F81FDF9E")
EnumDisplayModes;
#endif
#endif /* __DXMGRLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long __RPC_FAR *, unsigned long            , HWND __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  HWND_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, HWND __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  HWND_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, HWND __RPC_FAR * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long __RPC_FAR *, HWND __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
