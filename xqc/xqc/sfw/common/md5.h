#ifndef _MD5_H
#define _MD5_H

#include "types.h"

/**
 * \brief wraper of above functions
 *
 * \param szSour the source string
 * \param iLen the source string length
 * \param[out] szDest 16 bytes memory for output
 */
VOID MD5(BYTE *szSour, int iLen, BYTE *szDest);

#endif /*_MD5_H*/