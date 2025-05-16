/********************************************************************

   wnetcaps.h

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#pragma once

//
// Prototypes
//
BOOL WNetStat(int nIndex);

//
// There must be < 32 controls here.
//
#define NS_NULL 0x0
#define NS_REFRESH 0x1
#define NS_CONNECT 0x2
#define NS_CONNECTDLG 0x4
#define NS_SHAREDLG 0x8
#define NS_PROPERTYDLG 0x10
