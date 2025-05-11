/********************************************************************

   wfdocb.h

   Doc Bucket header

   Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License.

********************************************************************/

#pragma once

//
// Doc prototypes; typdefs
//

typedef struct _DOC_BUCKET** PPDOCBUCKET;
typedef struct _DOC_BUCKET* PDOCBUCKET;

PPDOCBUCKET DocConstruct();
void DocDestruct(PPDOCBUCKET ppDocBucket);
int DocInsert(PPDOCBUCKET ppDocBucket, LPWSTR lpszExt, LPWSTR lpszFileIcon);
PDOCBUCKET DocFind(PPDOCBUCKET ppDocBucket, LPWSTR lpszExt);
HICON DocGetIcon(PDOCBUCKET pDocBucket);
