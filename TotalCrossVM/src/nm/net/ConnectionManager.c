/*********************************************************************************
 *  TotalCross Software Development Kit                                          *
 *  Copyright (C) 2000-2012 SuperWaba Ltda.                                      *
 *  All Rights Reserved                                                          *
 *                                                                               *
 *  This library and virtual machine is distributed in the hope that it will     *
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                         *
 *                                                                               *
 *********************************************************************************/



#include "Net.h"

#if defined (WIN32) || defined (WINCE)
 #include "win/ConnectionManager_c.h"
#elif defined (ANDROID)
 #include "android/ConnectionManager_c.h"
#endif

// static fields
#define ConnectionManager_connRef(c)      getStaticFieldObject(c, "connRef")

//////////////////////////////////////////////////////////////////////////
TC_API void tnCM_loadResources(NMParams p) // totalcross/net/ConnectionManager native private void loadResources();
{
   if (connMgrClass == null)
      connMgrClass = loadClass(p->currentContext, "totalcross.net.ConnectionManager", true);
#if defined (WINCE)
   CmLoadResources(p->currentContext);
#endif
}
//////////////////////////////////////////////////////////////////////////
TC_API void tnCM_setDefaultConfiguration_is(NMParams p) // totalcross/net/ConnectionManager native public static void setDefaultConfiguration(int type, String cfg) throws totalcross.io.IOException;
{
   int32 type = p->i32[0];
   Object connCfg = p->obj[0];
#if defined (WINCE)
   TCHARP szConnCfg = null;
   Err err;

   switch (type)
   {
      case CM_CELLULAR:
      {
         if (connCfg != null)
            szConnCfg = String2TCHARP(connCfg);
         err = CmGprsConfigure(p->currentContext, szConnCfg);
         if (szConnCfg != null)
            xfree(szConnCfg);
         if (err != NO_ERROR)
            throwExceptionWithCode(p->currentContext, IOException, err);
      } break;
      default: throwIllegalArgumentExceptionI(p->currentContext, "type", type); break;
   }
#elif defined (ANDROID)
   JNIEnv* env = getJNIEnv();
   jmethodID setDefaultConfMethod = (*env)->GetStaticMethodID(env, jConnectionManager4A, "setDefaultConfiguration", "(ILjava/lang/String;)V");
   jstring szConnCfg = !connCfg ? null : (*env)->NewString(env, String_charsStart(connCfg), String_charsLen(connCfg));
   (*env)->CallStaticVoidMethod(env, jConnectionManager4A, setDefaultConfMethod, type, szConnCfg);
   (*env)->DeleteLocalRef(env, jConnectionManager4A);
   if (szConnCfg) (*env)->DeleteLocalRef(env, szConnCfg);
#else
   UNUSED(p)
#endif
}
//////////////////////////////////////////////////////////////////////////
TC_API void tnCM_open(NMParams p) // totalcross/net/ConnectionManager native public static void open() throws totalcross.io.IOException;
{
#if 0 //defined (WINCE) - disabled for this release
   Object* connRef;
   NATIVE_CONNECTION* connHandle;
   bool wasSuccessful = false;
   Err err;

   connRef = ConnectionManager_connRef(connMgrClass);
   *connRef = createByteArray(p->currentContext, sizeof(NATIVE_CONNECTION));
   if (*connRef != null)
   {
      connHandle = (NATIVE_CONNECTION*) ARRAYOBJ_START(*connRef);
      err = CmOpen(p->currentContext, connHandle, -1, &wasSuccessful);
      if (err != NO_ERROR)
         throwExceptionWithCode(p->currentContext, IOException, err);
      setObjectLock(*connRef, UNLOCKED);
   }
#else
   //UNUSED(p)
#endif
}
//////////////////////////////////////////////////////////////////////////
TC_API void tnCM_open_i(NMParams p) // totalcross/net/ConnectionManager native public static void open(int type) throws totalcross.io.IOException;
{
#if defined (WINCE)
   int type = p->i32[0];
   Object* connRef;
   NATIVE_CONNECTION* connHandle;
   bool wasSuccessful = false;
   Err err;

   switch (type)
   {
      case CM_CRADLE:
      {
         // do nothing for now.
      } break;
      case CM_WIFI:
      {
         // flsobral@tc115: do nothing, we'll assume wifi is already working.
      } break;
      case CM_CELLULAR:
      {
         connRef = ConnectionManager_connRef(connMgrClass);
         *connRef = createByteArray(p->currentContext, sizeof(NATIVE_CONNECTION));
         if (*connRef != null)
         {
            connHandle = (NATIVE_CONNECTION*) ARRAYOBJ_START(*connRef);
            if ((err = CmGprsOpen(p->currentContext, connHandle, -1, &wasSuccessful)) != NO_ERROR)
               throwExceptionWithCode(p->currentContext, IOException, err);
            setObjectLock(*connRef, UNLOCKED);
         }
      } break;

      default: throwIllegalArgumentExceptionI(p->currentContext, "type", type); break;
   }
#else
   UNUSED(p)
#endif
}
//////////////////////////////////////////////////////////////////////////
TC_API void tnCM_nativeClose(NMParams p) // totalcross/net/ConnectionManager native private static void nativeClose() throws totalcross.io.IOException;
{
#if defined (WINCE)
   Err err;
   Object* connRef = ConnectionManager_connRef(connMgrClass);
   NATIVE_CONNECTION* connHandle;

   if (*connRef != null)
   {
      connHandle = (NATIVE_CONNECTION*) ARRAYOBJ_START(*connRef);
      err = CmClose(p->currentContext, connHandle);
      *connRef = null;
      if (err != NO_ERROR)
         throwExceptionWithCode(p->currentContext, IOException, err);
   }
#endif
}
//////////////////////////////////////////////////////////////////////////
TC_API void tnCM_getHostAddress_s(NMParams p) // totalcross/net/ConnectionManager native public static String getHostAddress(String host) throws totalcross.net.UnknownHostException;
{
#if defined (WINCE) || defined (ANDROID)
   Object hostName = p->obj[0];
   CharP szHostName = null;
   char szHostAddress[40];
   Err err;

   *szHostAddress = 0;

   if (hostName == null)
      throwNullArgumentException(p->currentContext, "host");
   else
   {
      szHostName = String2CharP(hostName);
      if ((err = CmGetHostAddress(szHostName, szHostAddress)) != NO_ERROR)
         throwExceptionWithCode(p->currentContext, UnknownHostException, err); // flsobral@tc120: now we throw an exception
      else if (xstrlen(szHostAddress) > 0) //flsobral@tc115_43: must return null if the host address is not found.
      {
         p->retO = createStringObjectFromCharP(p->currentContext, szHostAddress, -1);
         setObjectLock(p->retO, UNLOCKED);
      }
      xfree(szHostName);
   }
#endif
}
//////////////////////////////////////////////////////////////////////////
TC_API void tnCM_getHostName_s(NMParams p) // totalcross/net/ConnectionManager native public static String getHostName(String host) throws totalcross.net.UnknownHostException;
{
#if defined (WINCE) || defined (ANDROID)
   Object hostAddress = p->obj[0];
   CharP szHostAddress = null;
   char szHostName[128];
   Err err;

   *szHostName = 0;

   if (hostAddress == null)
      throwNullArgumentException(p->currentContext, "hostAddress");
   else
   {
      szHostAddress = String2CharP(hostAddress);
      if ((err = CmGetHostName(szHostAddress, szHostName)) != NO_ERROR)
         throwExceptionWithCode(p->currentContext, UnknownHostException, err); // flsobral@tc120_XX: now we throw an exception
      else
      {
         p->retO = createStringObjectFromCharP(p->currentContext, szHostName, -1);
         setObjectLock(p->retO, UNLOCKED);
      }
      xfree(szHostAddress);
   }
#endif
}
//////////////////////////////////////////////////////////////////////////
TC_API void tnCM_getLocalHost(NMParams p) // totalcross/net/ConnectionManager native public static String getLocalHost() throws totalcross.net.UnknownHostException;
{
#if defined(WINCE) || defined(WIN32) || defined(ANDROID)
   char szHostAddress[16];

   if (CmGetLocalHost(szHostAddress) != NO_ERROR)
      xstrcpy(szHostAddress, "127.0.0.1");
   p->retO = createStringObjectFromCharP(p->currentContext, szHostAddress, -1);
#else
   p->retO = createStringObjectFromCharP(p->currentContext, "127.0.0.1", -1);
#endif
   setObjectLock(p->retO, UNLOCKED);
}
//////////////////////////////////////////////////////////////////////////
TC_API void tnCM_isAvailable_i(NMParams p) // totalcross/net/ConnectionManager native public static boolean isAvailable(int type) throws totalcross.io.IOException;
{
   int32 type = p->i32[0];

#if defined (WINCE)
   p->retI = false;
   if (type == CM_CRADLE)
   {
      if (CmIsAvailable(type))
      {
         char szHostAddress[16];
         Err err = CmGetLocalHost(szHostAddress);
         if (!strEq(szHostAddress, "127.0.0.1"))
            p->retI = true;
      }
   }
   else
      p->retI = CmIsAvailable(type);
#else
   p->retI = true;
#endif
}
//////////////////////////////////////////////////////////////////////////
TC_API void tnCM_releaseResources(NMParams p) // totalcross/net/ConnectionManager native private void releaseResources();
{
#if defined (WINCE)
   CmReleaseResources();
#endif
}

#ifdef ENABLE_TEST_SUITE
//#include "ConnectionManager_test.h"
#endif
