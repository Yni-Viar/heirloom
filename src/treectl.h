/********************************************************************

   treectl.h

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#pragma once

#define SHOVEIT(x) (MAKELONG((x), 0))

/* Tree Node Flags. */
#define TF_LASTLEVELENTRY 0x01
#define TF_HASCHILDREN 0x02
#define TF_EXPANDED 0x04
#define TF_DISABLED 0x08

#define TF_LOWERCASE 0x20

typedef struct tagDNODE {
    struct tagDNODE* pParent;
    BYTE wFlags;
    BYTE nLevels;
    DWORD dwNetType;
    DWORD dwExtent;
    DWORD dwAttribs;
    WCHAR szName[1];  // variable length field
} DNODE;
typedef DNODE* PDNODE;

void GetTreePath(PDNODE pNode, LPWSTR szDest);
void SetNodeAttribs(PDNODE pNode, LPWSTR szPath);
