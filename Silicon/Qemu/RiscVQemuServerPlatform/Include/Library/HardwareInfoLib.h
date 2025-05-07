/** @file
*
*  Copyright (c) 2024, Linaro Ltd. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#ifndef HARDWARE_INFO_LIB
#define HARDWARE_INFO_LIB

typedef struct {
  UINT32    Sockets;
  UINT32    Clusters;
  UINT32    Cores;
  UINT32    Threads;
} CpuTopology;

/**
  Get CPU count from information passed by Qemu.

**/
UINT32
GetCpuCount (
  VOID
  );

/**
  Get cpu topology (sockets, clusters, cores, threads) from qemu.

  @param [out]  CpuTopo     A pointer to the cpu topology.


  @retval                   the information of cpu topology.
**/
EFI_STATUS
GetCpuTopology (
  OUT CpuTopology  *CpuTopo
  );

/**
  Get the number of memory node from device tree passed by Qemu.

  @retval                   the number of memory nodes.
**/
UINT32
GetMemNodeCount (
  VOID
  );

#endif /* HARDWARE_INFO_LIB */