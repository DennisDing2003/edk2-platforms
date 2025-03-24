/** @file
*
*  Copyright (c) Academy of Intelligent Innovation, Shandong Universiy, China.P.R. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef RISCVSERVERPLATFORM_VERSION_H
#define RISCVSERVERPLATFORM_VERSION_H

/*
 * Compare PlatformVersion
 *
 */

#define PLATFORM_VERSION_LESS_THAN(Major, Minor)  (    \
  (                                                    \
    ( PcdGet32 (PcdPlatformVersionMajor) < Major)   || \
    (                                                  \
      ( PcdGet32 (PcdPlatformVersionMajor) == Major) && \
      ( PcdGet32 (PcdPlatformVersionMinor) < Minor)    \
    )                                                  \
  )                                                    \
)
#endif