/** @file
*
*  Copyright (c) 2021, NUVIA Inc. All rights reserved.
*  Copyright (c) 2024, Linaro Ltd. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/ResetSystemLib.h>
#include <Library/HardwareInfoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <IndustryStandard/RiscVServerAcpi.h>

#include <Protocol/FdtClient.h>

/**
  Get CPU count from information passed by DeviceTree

**/
UINT32
GetCpuCount (
  VOID
  )
{
  FDT_CLIENT_PROTOCOL  *FdtClient;
  EFI_STATUS           Status;
  EFI_STATUS           FindNodeStatus;
  UINT32               CpuCount = 0;
  INT32                Node;
  UINT32               PropSize;
  CONST CHAR8         *DeviceType;

  // 获取 FDT 客户端协议
  Status = gBS->LocateProtocol (
                  &gFdtClientProtocolGuid,
                  NULL,
                  (VOID **)&FdtClient
                );
  if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "定位设备树失败: %r\n", Status));
      return 0;
  }

  // 遍历 /cpus 下的所有 CPU 节点
  CpuCount = 0;
  for (FindNodeStatus = FdtClient->FindCompatibleNode (FdtClient, "riscv", &Node);
       !EFI_ERROR (FindNodeStatus);
       FindNodeStatus = FdtClient->FindNextCompatibleNode (FdtClient, "riscv", Node, &Node))
  {
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: 获取cpu节点失败: %r\n", __func__, Status));
      return Status;
    }
    Status = FdtClient->GetNodeProperty (
                          FdtClient,
                          Node,
                          "device_type",
                          (CONST VOID **)&DeviceType,
                          &PropSize
                          );
    if (!EFI_ERROR (Status) && AsciiStrCmp (DeviceType, "cpu") == 0) {
      CpuCount++;
    }
  }

  DEBUG ((DEBUG_INFO, "检测到的CPU数量: %d\n", CpuCount));
  return CpuCount;
}

EFI_STATUS
GetCpuTopology (
  OUT CpuTopology  *CpuTopo
  )
{
  FDT_CLIENT_PROTOCOL  *FdtClient;
  EFI_STATUS           Status;
  EFI_STATUS           FindNodeStatus;
  INT32                Node;
  UINT32               TotalCores = 0;
  UINT32               Sockets = 0;
  UINT32               Clusters = 0;
  CONST CHAR8         *DeviceType;
  UINT32               PropSize;

  if (CpuTopo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  // 初始化
  CpuTopo->Sockets  = 0;
  CpuTopo->Clusters = 0;
  CpuTopo->Cores    = 0;
  CpuTopo->Threads  = 0;

  // 获取 FDT 协议
  Status = gBS->LocateProtocol (
                  &gFdtClientProtocolGuid,
                  NULL,
                  (VOID **)&FdtClient
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: 获取设备树地址失败: %r\n", __func__, Status));
    return Status;
  }

  // 1. 遍历所有 compatible = "riscv" 的节点，计算核心数量
  DEBUG ((DEBUG_INFO, "%a: 开始扫描CPU节点\n", __func__));
  for (FindNodeStatus = FdtClient->FindCompatibleNode (FdtClient, "riscv", &Node);
       !EFI_ERROR (FindNodeStatus);
       FindNodeStatus = FdtClient->FindNextCompatibleNode (FdtClient, "riscv", Node, &Node))
  {
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a: 获取cpu节点失败: %r\n", __func__, Status));
      return Status;
    }
    Status = FdtClient->GetNodeProperty (
                          FdtClient,
                          Node,
                          "device_type",
                          (CONST VOID **)&DeviceType,
                          &PropSize
                          );
    if (!EFI_ERROR (Status) && AsciiStrCmp (DeviceType, "cpu") == 0) {
      TotalCores++;
    }
  }

  if (TotalCores == 0) {
    DEBUG ((DEBUG_WARN, "%a: 没有找到CPU节点\n", __func__));
    return EFI_NOT_FOUND;
  }

  CpuTopo->Cores = TotalCores;

  // 2. 尝试找 cpu-map 节点（-smp改变的只是Core的数量， 无法改变Socket和Cluster的数量）
  Sockets = 1;
  Clusters = 1;

  // 构造输出
  if (Sockets > 0 && Clusters > 0) {
    CpuTopo->Sockets  = Sockets;
    CpuTopo->Clusters = Clusters;
    CpuTopo->Cores    = TotalCores / Clusters;
  } else {
    CpuTopo->Sockets  = 1;
    CpuTopo->Clusters = (TotalCores > 1) ? 2 : 1;
    CpuTopo->Cores    = TotalCores / CpuTopo->Clusters;
  }

  DEBUG ((DEBUG_INFO, "%a: Topo: sockets=%u, clusters=%u, cores=%u, threads=%u\n",
          __func__, CpuTopo->Sockets, CpuTopo->Clusters, CpuTopo->Cores, CpuTopo->Threads));

  return EFI_SUCCESS;
}
