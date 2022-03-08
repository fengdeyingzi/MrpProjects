#ifndef _CONFIG_H
#define _CONFIG_H

/**
 * \brief Define to non-zero to indicate the system is big-endian
 */
#ifndef BIG_ENDIAN
#ifdef MR_SPREADTRUM_MOD
#define BIG_ENDIAN	1
#else
#define BIG_ENDIAN	0
#endif
#endif

/**
 * \brief Define to non-zero to indicate the system is little-endian
 */
#ifndef LITTLE_ENDIAN
#ifdef MR_SPREADTRUM_MOD
#define LITTLE_ENDIAN	0
#else
#define LITTLE_ENDIAN	1
#endif
#endif

/**
 * \brief if this macro defined, network module will not compiled
 */
#ifndef OMIT_NETWORK_MODULE
#define OMIT_NETWORK_MODULE	0
#endif

/**
 * \brief if this macro defined, WM_KEYDOWNREPEAT event enabled.
 */
 #ifndef ENABLE_SGL_KEYREPEAT
#define ENABLE_SGL_KEYREPEAT
#endif

#endif //_CONFIG_H

