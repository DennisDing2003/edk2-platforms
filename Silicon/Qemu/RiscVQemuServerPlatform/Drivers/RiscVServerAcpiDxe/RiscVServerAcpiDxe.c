/** @file
  Sample ACPI Platform Driver

  Copyright (c) 2008 - 2011, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiDxe.h>

#include <Protocol/AcpiTable.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/FdtClient.h>

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HardwareInfoLib.h>

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/RiscVServerAcpi.h>
//#include "UpdateAcpiTable.h"

static UINTN  mCpuId;
static UINTN  mCacheId;

/**
  Locate the first instance of a protocol.  If the protocol requested is an
  FV protocol, then it will return the first FV that contains the ACPI table
  storage file.

  @param  Instance      Return pointer to the first instance of the protocol

  @return EFI_SUCCESS           The function completed successfully.
  @return EFI_NOT_FOUND         The protocol could not be located.
  @return EFI_OUT_OF_RESOURCES  There are not enough resources to find the protocol.

**/
EFI_STATUS
LocateFvInstanceWithTables (
  OUT EFI_FIRMWARE_VOLUME2_PROTOCOL **Instance
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *FvInstance;

  FvStatus = 0;

  //
  // Locate protocol.
  //
  DEBUG ((DEBUG_INFO, "加载ACPI：gBS: %p, gEfiFirmwareVolume2ProtocolGuid: %g\n", gBS, &gEfiFirmwareVolume2ProtocolGuid));
  Status = gBS->LocateHandleBuffer (
                   ByProtocol,
                   &gEfiFirmwareVolume2ProtocolGuid,
                   NULL,
                   &NumberOfHandles,
                   &HandleBuffer
                   );
  DEBUG ((DEBUG_INFO, "加载ACPI：LocateHandleBuffer Status: %r, NumberOfHandles: %d, HandleBuffer: %p\n", Status, NumberOfHandles, HandleBuffer));
  if (EFI_ERROR (Status)) {
    //
    // Defined errors at this time are not found and out of resources.
    //
    return Status;
  }



  //
  // Looking for FV with ACPI storage file
  //

  for (Index = 0; Index < NumberOfHandles; Index++) {
    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    DEBUG ((DEBUG_INFO, "加载ACPI：HandleBuffer[%d]: %p\n", Index, HandleBuffer[Index]));
    Status = gBS->HandleProtocol (
                     HandleBuffer[Index],
                     &gEfiFirmwareVolume2ProtocolGuid,
                     (VOID**) &FvInstance
                     );
    DEBUG ((DEBUG_INFO, "加载ACPI：HandleProtocol Status: %r, FvInstance: %p\n", Status, FvInstance));
    ASSERT_EFI_ERROR (Status);

    //
    // See if it has the ACPI storage file
    //
    EFI_GUID *AcpiTableStorageFileGuid = (EFI_GUID*)PcdGetPtr (PcdAcpiTableStorageFile);//new added code
    DEBUG ((DEBUG_INFO, "加载ACPI：PcdAcpiTableStorageFile GUID: %g\n", AcpiTableStorageFileGuid));
    DEBUG ((DEBUG_INFO, "加载ACPI：FvInstance->ReadFile: %p\n", FvInstance->ReadFile));
    Status = FvInstance->ReadFile (
                           FvInstance,
                           (EFI_GUID*)PcdGetPtr (PcdAcpiTableStorageFile),
                           NULL,
                           &Size,
                           &FileType,
                           &Attributes,
                           &FvStatus
                           );
    DEBUG ((DEBUG_INFO, "加载ACPI：ReadFile Status: %r, Size: %d\n", Status, Size));
    //
    // If we found it, then we are done
    //
    if (Status == EFI_SUCCESS) {
      *Instance = FvInstance;
      break;
    }
  }

  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  //

  //
  // Free any allocated buffers
  //
  gBS->FreePool (HandleBuffer);

  return Status;
}


/**
  This function calculates and updates an UINT8 checksum.

  @param  Buffer          Pointer to buffer to checksum
  @param  Size            Number of bytes to checksum

**/
VOID
AcpiPlatformChecksum (
  IN UINT8      *Buffer,
  IN UINTN      Size
  )
{
  UINTN ChecksumOffset;

  ChecksumOffset = OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum);

  //
  // Set checksum to 0 first
  //
  Buffer[ChecksumOffset] = 0;

  //
  // Update checksum value
  //
  Buffer[ChecksumOffset] = CalculateCheckSum8(Buffer, Size);
}

STATIC VOID
AcpiPpttFillCacheSizeInfo (
  EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE  *Node,
  UINT32                             Level,
  BOOLEAN                            DataCache,
  BOOLEAN                            UnifiedCache
  )
{
  FDT_CLIENT_PROTOCOL *FdtClient;
  INT32               CacheNode;
  EFI_STATUS          Status;
  UINT32              CacheSize, CacheSets, CacheBlockSize, CacheWays;
  UINT8               CacheType, AllocationType, WritePolicy;

  Status = gBS->LocateProtocol (&gFdtClientProtocolGuid, NULL, (VOID **)&FdtClient);
  if (EFI_ERROR (Status)) {
    goto UseDefault;
  }

  Status = FdtClient->FindCompatibleNode (FdtClient, "cache", &CacheNode);
  if (EFI_ERROR (Status)) {
    goto UseDefault;
  }

  CacheSize = 64 * 1024; // 64KB
  CacheSets = 512;
  CacheBlockSize = 64;
  CacheWays = 2;

  Node->Flags.LineSizeValid      = 1;
  Node->Flags.NumberOfSetsValid  = 1;
  Node->Flags.AssociativityValid = 1;
  Node->Flags.SizePropertyValid  = 1;
  Node->Flags.CacheTypeValid     = 1;

  Node->Size = CacheSize;
  Node->NumberOfSets = CacheSets;
  Node->Associativity = CacheWays;
  Node->LineSize = CacheBlockSize;

  // 解析宏展开后的值并赋值给 Attributes 结构体的字段
  if (UnifiedCache) {
    UINT8 AttrValues[] = PPTT_UNIFIED_CACHE_ATTR;
    CacheType = AttrValues[1];      // CacheType
    AllocationType = AttrValues[0]; // AllocationType
    WritePolicy = AttrValues[2];    // WritePolicy
  } else if (DataCache) {
    UINT8 AttrValues[] = PPTT_DATA_CACHE_ATTR;
    CacheType = AttrValues[1];
    AllocationType = AttrValues[0];
    WritePolicy = AttrValues[2];
  } else {
    UINT8 AttrValues[] = PPTT_INST_CACHE_ATTR;
    CacheType = AttrValues[1];
    AllocationType = AttrValues[0];
    WritePolicy = AttrValues[2];
  }

  // 赋值给 Attributes 结构体的字段
  Node->Attributes.CacheType = CacheType;
  Node->Attributes.AllocationType = AllocationType;
  Node->Attributes.WritePolicy = WritePolicy;
  Node->Attributes.Reserved = 0; // 保留字段清零
  return;

UseDefault:
  Node->Size = 64 * 1024; // 64KB
  Node->NumberOfSets = 512;
  Node->Associativity = 2;
  Node->LineSize = 64;
  Node->Flags.LineSizeValid      = 1;
  Node->Flags.NumberOfSetsValid  = 1;
  Node->Flags.AssociativityValid = 1;
  Node->Flags.SizePropertyValid  = 1;
  Node->Flags.CacheTypeValid     = 1;

  // 解析宏展开后的值并赋值给 Attributes 结构体的字段
  if (UnifiedCache) {
    UINT8 AttrValues[] = PPTT_UNIFIED_CACHE_ATTR;
    CacheType = AttrValues[1];
    AllocationType = AttrValues[0];
    WritePolicy = AttrValues[2];
  } else if (DataCache) {
    UINT8 AttrValues[] = PPTT_DATA_CACHE_ATTR;
    CacheType = AttrValues[1];
    AllocationType = AttrValues[0];
    WritePolicy = AttrValues[2];
  } else {
    UINT8 AttrValues[] = PPTT_INST_CACHE_ATTR;
    CacheType = AttrValues[1];
    AllocationType = AttrValues[0];
    WritePolicy = AttrValues[2];
  }

  // 赋值给 Attributes 结构体的字段
  Node->Attributes.CacheType = CacheType;
  Node->Attributes.AllocationType = AllocationType;
  Node->Attributes.WritePolicy = WritePolicy;
  Node->Attributes.Reserved = 0; // 保留字段清零
}

STATIC
UINT32
AddCoresToPpttTable (
  UINT8        *New,
  UINT32       ClusterOffset,
  CpuTopology  CpuTopo
  )
{
  UINT32  L1DCacheOffset;
  UINT32  L1ICacheOffset;
  UINT32  L2CacheOffset;
  UINT32  CoreOffset;
  UINT32  Offset;
  UINT32  CoreCpuId;
  UINT32  CoreIndex;
  UINT32  ThreadIndex;
  UINT32  *PrivateResourcePtr;

  EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR_FLAGS  CoreFlags = {
    EFI_ACPI_6_5_PPTT_PACKAGE_NOT_PHYSICAL,
    EFI_ACPI_6_5_PPTT_PROCESSOR_ID_VALID,
    EFI_ACPI_6_5_PPTT_PROCESSOR_IS_NOT_THREAD,
    EFI_ACPI_6_5_PPTT_NODE_IS_LEAF,
    EFI_ACPI_6_5_PPTT_IMPLEMENTATION_IDENTICAL
  };

  if (CpuTopo.Threads > 1) {
    // The Thread structure is the leaf structure, adjust the value of CoreFlags.
    CoreFlags.AcpiProcessorIdValid = EFI_ACPI_6_5_PPTT_PROCESSOR_ID_INVALID;
    CoreFlags.NodeIsALeaf          = EFI_ACPI_6_5_PPTT_NODE_IS_NOT_LEAF;
  }

  EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR_FLAGS  ThreadFlags = {
    EFI_ACPI_6_5_PPTT_PACKAGE_NOT_PHYSICAL,
    EFI_ACPI_6_5_PPTT_PROCESSOR_ID_VALID,
    EFI_ACPI_6_5_PPTT_PROCESSOR_IS_THREAD,
    EFI_ACPI_6_5_PPTT_NODE_IS_LEAF,
    EFI_ACPI_6_5_PPTT_IMPLEMENTATION_IDENTICAL
  };

  EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE  L1DCache = RISCVQEMU_ACPI_PPTT_L1_D_CACHE_STRUCT;
  EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE  L1ICache = RISCVQEMU_ACPI_PPTT_L1_I_CACHE_STRUCT;
  EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE  L2Cache  = RISCVQEMU_ACPI_PPTT_L2_CACHE_STRUCT;

  AcpiPpttFillCacheSizeInfo (&L1DCache, 1, TRUE, FALSE);
  AcpiPpttFillCacheSizeInfo (&L1ICache, 1, FALSE, FALSE);
  AcpiPpttFillCacheSizeInfo (&L2Cache, 2, FALSE, TRUE);

  CoreOffset = ClusterOffset + sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR);
  Offset     = CoreOffset;

  for (CoreIndex = 0; CoreIndex < CpuTopo.Cores; CoreIndex++) {
    if (CpuTopo.Threads == 1) {
      CoreCpuId = mCpuId;
    } else {
      CoreCpuId = 0;
    }

    // space for Core + PrivateResourcePtr
    Offset += sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR);
    Offset += sizeof (UINT32) * 2;

    L1DCacheOffset = Offset;
    L1ICacheOffset = L1DCacheOffset + sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE);
    L2CacheOffset  = L1ICacheOffset + sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE);

    EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR  Core = RISCVQEMU_ACPI_PROCESSOR_HIERARCHY_NODE_STRUCTURE_INIT (
                                                    CoreFlags,
                                                    ClusterOffset,
                                                    CoreCpuId,
                                                    2
                                                    );

    CopyMem (New, &Core, sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR));
    New += sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR);

    PrivateResourcePtr    = (UINT32 *)New;
    PrivateResourcePtr[0] = L1DCacheOffset;
    PrivateResourcePtr[1] = L1ICacheOffset;
    New                  += (2 * sizeof (UINT32));

    // Add L1 D Cache structure
    L1DCache.CacheId = mCacheId++;
    CopyMem (New, &L1DCache, sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE));
    ((EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE *)New)->NextLevelOfCache = L2CacheOffset;
    New                                                         += sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE);

    // Add L1 I Cache structure
    L1ICache.CacheId = mCacheId++;
    CopyMem (New, &L1ICache, sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE));
    ((EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE *)New)->NextLevelOfCache = L2CacheOffset;
    New                                                         += sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE);

    // Add L2 Cache structure
    L2Cache.CacheId = mCacheId++;
    CopyMem (New, &L2Cache, sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE));
    New += sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE);

    Offset += sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE) * 3;

    if (CpuTopo.Threads == 1) {
      mCpuId++;
    } else {
      // Add the Thread PPTT structure
      for (ThreadIndex = 0; ThreadIndex < CpuTopo.Threads; ThreadIndex++) {
        EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR  Thread = RISCVQEMU_ACPI_PROCESSOR_HIERARCHY_NODE_STRUCTURE_INIT (
                                                          ThreadFlags,
                                                          CoreOffset,
                                                          mCpuId,
                                                          0
                                                          );
        CopyMem (New, &Thread, sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR));
        New += sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR);
        mCpuId++;
      }

      Offset +=  CpuTopo.Threads * sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR);
    }

    CoreOffset = Offset;
  }

  return CoreOffset - ClusterOffset;
}

/*
 * A function that adds the PPTT ACPI table.
 */
 EFI_STATUS
 AddPpttTable (
   IN EFI_ACPI_TABLE_PROTOCOL  *AcpiTable
   )
 {
   EFI_STATUS            Status;
   UINTN                 TableHandle;
   UINT32                TableSize;
   UINT32                CoresPartSize;
   UINT32                SocketIndex;
   UINT32                ClusterIndex;
   UINT32                SocketOffset;
   UINT32                ClusterOffset;
   EFI_PHYSICAL_ADDRESS  PageAddress;
   UINT8                 *New;
   CpuTopology           CpuTopo;

   GetCpuTopology (&CpuTopo);

   EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR_FLAGS  SocketFlags = {
     EFI_ACPI_6_5_PPTT_PACKAGE_PHYSICAL,
     EFI_ACPI_6_5_PPTT_PROCESSOR_ID_INVALID,
     EFI_ACPI_6_5_PPTT_PROCESSOR_IS_NOT_THREAD,
     EFI_ACPI_6_5_PPTT_NODE_IS_NOT_LEAF,
     EFI_ACPI_6_5_PPTT_IMPLEMENTATION_IDENTICAL
   };

   EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR_FLAGS  ClusterFlags = {
     EFI_ACPI_6_5_PPTT_PACKAGE_NOT_PHYSICAL,
     EFI_ACPI_6_5_PPTT_PROCESSOR_ID_INVALID,
     EFI_ACPI_6_5_PPTT_PROCESSOR_IS_NOT_THREAD,
     EFI_ACPI_6_5_PPTT_NODE_IS_NOT_LEAF,
     EFI_ACPI_6_5_PPTT_IMPLEMENTATION_IDENTICAL
   };

   EFI_ACPI_DESCRIPTION_HEADER  Header =
     RISCV_ACPI_HEADER (
       EFI_ACPI_6_5_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_STRUCTURE_SIGNATURE,
       EFI_ACPI_DESCRIPTION_HEADER,
       EFI_ACPI_6_5_PROCESSOR_PROPERTIES_TOPOLOGY_TABLE_REVISION
       );

   TableSize = sizeof (EFI_ACPI_DESCRIPTION_HEADER) +
               CpuTopo.Sockets * (sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR) +
                                  CpuTopo.Clusters * (sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR) +
                                                      CpuTopo.Cores * (sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR) +
                                                                       sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE) * 3 +
                                                                       sizeof (UINT32) * 2)));

   if (CpuTopo.Threads > 1) {
     TableSize += CpuTopo.Sockets * CpuTopo.Clusters * CpuTopo.Cores * CpuTopo.Threads *
                  sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR);
   }

   Status = gBS->AllocatePages (
                   AllocateAnyPages,
                   EfiACPIReclaimMemory,
                   EFI_SIZE_TO_PAGES (TableSize),
                   &PageAddress
                   );
   if (EFI_ERROR (Status)) {
     DEBUG ((DEBUG_ERROR, "Failed to allocate pages for PPTT table\n"));
     return EFI_OUT_OF_RESOURCES;
   }

   New = (UINT8 *)(UINTN)PageAddress;
   ZeroMem (New, TableSize);

   // Add the ACPI Description table header
   CopyMem (New, &Header, sizeof (EFI_ACPI_DESCRIPTION_HEADER));
   ((EFI_ACPI_DESCRIPTION_HEADER *)New)->Length = TableSize;
   New                                         += sizeof (EFI_ACPI_DESCRIPTION_HEADER);

   mCpuId   = 0;
   mCacheId = 1;     // 0 is not a valid Cache ID.

   SocketOffset = sizeof (EFI_ACPI_DESCRIPTION_HEADER);
   for (SocketIndex = 0; SocketIndex < CpuTopo.Sockets; SocketIndex++) {
     // Add the Socket PPTT structure
     EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR  Socket = RISCVQEMU_ACPI_PROCESSOR_HIERARCHY_NODE_STRUCTURE_INIT (
                                                       SocketFlags,
                                                       0,
                                                       0,
                                                       0
                                                       );
     CopyMem (New, &Socket, sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR));
     New += sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR);

     ClusterOffset = SocketOffset + sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR);
     for (ClusterIndex = 0; ClusterIndex < CpuTopo.Clusters; ClusterIndex++) {
       // Add the Cluster PPTT structure
       EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR  Cluster = RISCVQEMU_ACPI_PROCESSOR_HIERARCHY_NODE_STRUCTURE_INIT (
                                                          ClusterFlags,
                                                          SocketOffset,
                                                          0,
                                                          0
                                                          );
       CopyMem (New, &Cluster, sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR));
       New += sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR);

       CoresPartSize  = AddCoresToPpttTable (New, ClusterOffset, CpuTopo);
       ClusterOffset += CoresPartSize;
       New           += CoresPartSize - sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR);
     }

     SocketOffset = ClusterOffset;
   }

   // Perform Checksum
   AcpiPlatformChecksum ((UINT8 *)PageAddress, TableSize);

   Status = AcpiTable->InstallAcpiTable (
                         AcpiTable,
                         (EFI_ACPI_COMMON_HEADER *)PageAddress,
                         TableSize,
                         &TableHandle
                         );
   if (EFI_ERROR (Status)) {
     DEBUG ((DEBUG_ERROR, "Failed to install PPTT table\n"));
   }

   return Status;
 }

/*
 * A function that add the MADT ACPI table.
  IN EFI_ACPI_COMMON_HEADER    *CurrentTable
 */
EFI_STATUS
AddMadtTable (
  IN EFI_ACPI_TABLE_PROTOCOL  *AcpiTable
  )
{
  EFI_STATUS            Status;
  UINTN                 TableHandle;
  UINT32                TableSize;
  EFI_PHYSICAL_ADDRESS  PageAddress;
  UINT8                 *New;
  UINT32                NumCores = 0;
  UINT32                CoreIndex;
  FDT_CLIENT_PROTOCOL  *FdtClient;
  UINT32                PropSize;
  CONST VOID           *Property;

  // Initialize MADT ACPI Header
  EFI_ACPI_6_5_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER  Header = {
    RISCV_ACPI_HEADER (
      EFI_ACPI_6_5_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE,
      EFI_ACPI_6_5_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER,
      EFI_ACPI_6_5_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION
      ),
    0, 0
  };

  // Locate FDT Client Protocol
  Status = gBS->LocateProtocol (&gFdtClientProtocolGuid, NULL, (VOID **)&FdtClient);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "找不到FdtClientProtocol，无法生成MADT表\n"));
    return Status;
  }

  NumCores = GetCpuCount ();

  // Calculate the new table size based on the number of cores
  TableSize = sizeof (EFI_ACPI_6_5_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER) +
              (sizeof (EFI_ACPI_6_6_RINTC_STRUCTURE) * NumCores);

  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiACPIReclaimMemory,
                  EFI_SIZE_TO_PAGES (TableSize),
                  &PageAddress
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "安装MADT表失败\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  New = (UINT8 *)(UINTN)PageAddress;
  ZeroMem (New, TableSize);

  // Add the  ACPI Description table header
  CopyMem (New, &Header, sizeof (EFI_ACPI_6_5_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER));
  ((EFI_ACPI_DESCRIPTION_HEADER *)New)->Length = TableSize;
  New                                         += sizeof (EFI_ACPI_6_5_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);

  // 遍历填充 RINTC 结构
  INT32 CpuNode = 0;
  //CoreIndex = 0;
  while (TRUE) {
    Status = FdtClient->FindNextCompatibleNode (FdtClient, "riscv", CpuNode, &CpuNode);
    if (EFI_ERROR (Status)) {
      break;
    }

    Status = FdtClient->GetNodeProperty (FdtClient, CpuNode, "status", &Property, &PropSize);
    if (!EFI_ERROR (Status) && (AsciiStrCmp ((CONST CHAR8 *)Property, "okay") != 0)) {
      continue;
    }

    // 读reg拿HartID
    Status = FdtClient->GetNodeProperty (FdtClient, CpuNode, "reg", &Property, &PropSize);
    if (EFI_ERROR (Status) || PropSize != sizeof(UINT32)) {
      continue;
    }
    UINT64 HartId = SwapBytes32 (*(UINT32 *)Property);
    DEBUG ((DEBUG_INFO, "HartId的值为: %d\n", HartId));

    Status = FdtClient->GetNodeProperty (FdtClient, CpuNode, "phandle", &Property, &PropSize);
    CoreIndex = SwapBytes32 (*(UINT32 *)Property);
    if (EFI_ERROR (Status) || PropSize != sizeof(UINT32)) {
      continue;
    }
    DEBUG ((DEBUG_INFO, "CoreIndex的值为: %d\n", CoreIndex));

    EFI_ACPI_6_6_RINTC_STRUCTURE *Rintc;
    Rintc = (EFI_ACPI_6_6_RINTC_STRUCTURE *)New;

    // 创建一个临时结构并初始化
    UINT32 Flags = 1; // 默认 enabled
    Status = FdtClient->GetNodeProperty (FdtClient, CpuNode, "status", &Property, &PropSize);
    if (!EFI_ERROR (Status) && (AsciiStrCmp ((CONST CHAR8 *)Property, "okay") != 0)) {
      Flags = 0; // disabled
    }
    EFI_ACPI_6_6_RINTC_STRUCTURE TempRintc = EFI_ACPI_6_6_RINTC_STRUCTURE_INIT (
      Flags,                        // Flags (enabled)
      HartId,                   // HartID
      CoreIndex,                // AcpiProcessorUid
      ACPI_BUILD_INTC_ID(0, CoreIndex), // External Interrupt ID
      0,                        // IMSIC base address
      0                         // IMSIC size
    );

    // 将临时结构内容复制到目标内存
    CopyMem(Rintc, &TempRintc, sizeof(EFI_ACPI_6_6_RINTC_STRUCTURE));

    New += sizeof (EFI_ACPI_6_6_RINTC_STRUCTURE);
    //CoreIndex++;
  }

  AcpiPlatformChecksum ((UINT8 *)PageAddress, TableSize);

  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        (EFI_ACPI_COMMON_HEADER *)PageAddress,
                        TableSize,
                        &TableHandle
                        );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "安装MADT表失败\n"));
  }

  return Status;
}

EFI_STATUS
AddRhctTable (
  IN EFI_ACPI_TABLE_PROTOCOL  *AcpiTable
  )
{
  EFI_STATUS            Status;
  UINTN                 TableHandle;
  UINT32                TableSize;
  EFI_PHYSICAL_ADDRESS  PageAddress;
  UINT8                 *New;
  UINT32                NumCores = 0;
  UINT32                PropSize;
  CONST VOID           *Property;
  FDT_CLIENT_PROTOCOL  *FdtClient;

  NumCores = GetCpuCount();

  // Locate FDT Client Protocol
  Status = gBS->LocateProtocol (&gFdtClientProtocolGuid, NULL, (VOID **)&FdtClient);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "找不到FdtClientProtocol，无法生成RHCT表\n"));
    return Status;
  }

  // Multiple APIC Description Table
  #pragma pack (1)

  typedef struct {
    EFI_ACPI_6_5_RISCV_HART_CAPABILITIES_TABLE_HEADER    Header;
    EFI_ACPI_6_5_ISA_STRING_NODE_STRUCTURE               ISAStrNode;
    EFI_ACPI_6_5_CMO_NODE_STRUCTURE                      CMONode;
    EFI_ACPI_6_5_MMU_NODE_STRUCTURE                      MMUNode;
    EFI_ACPI_6_5_HART_INFO_NODE_STRUCTURE                HartInfoNode[NumCores];
  } EFI_ACPI_6_5_RISCV_HART_CAPABILITIES_TABLE;

  #pragma pack ()

  // Initialize RHCT ACPI Header
  EFI_ACPI_6_5_RISCV_HART_CAPABILITIES_TABLE_HEADER Header = {
    RISCV_ACPI_HEADER (
      EFI_ACPI_6_6_RISCV_HART_CAPABILITIES_TABLE_SIGNATURE,
      EFI_ACPI_6_5_RISCV_HART_CAPABILITIES_TABLE,
      EFI_ACPI_6_6_RISCV_HART_CAPABILITIES_TABLE_REVISION
    ),
    // RHCT specific fields
    0,                      // Flags
    10000000,               // Time Base Frequency
    3+NumCores,             // Number of RHCT nodes
    RHCT_NODE_ARRAY_OFFSET  // Offset to the RHCT node array
  };

  // Calculate the new table size based on the number of cores
  TableSize = sizeof (EFI_ACPI_6_5_RISCV_HART_CAPABILITIES_TABLE_HEADER) +
              sizeof (EFI_ACPI_6_5_ISA_STRING_NODE_STRUCTURE) +
              sizeof (EFI_ACPI_6_5_CMO_NODE_STRUCTURE) +
              sizeof (EFI_ACPI_6_5_MMU_NODE_STRUCTURE) +
              (sizeof (EFI_ACPI_6_5_HART_INFO_NODE_STRUCTURE) * NumCores);

  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiACPIReclaimMemory,
                  EFI_SIZE_TO_PAGES (TableSize),
                  &PageAddress
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "安装RHCT表失败\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  New = (UINT8 *)(UINTN)PageAddress;
  ZeroMem (New, TableSize);

  // Add the ACPI Description table header
  CopyMem (New, &Header, sizeof (EFI_ACPI_6_5_RISCV_HART_CAPABILITIES_TABLE_HEADER));
  ((EFI_ACPI_DESCRIPTION_HEADER *)New)->Length = TableSize;
  New                                         += sizeof (EFI_ACPI_6_5_RISCV_HART_CAPABILITIES_TABLE_HEADER);

  EFI_ACPI_6_5_ISA_STRING_NODE_STRUCTURE *IsaPointer;
  IsaPointer = (EFI_ACPI_6_5_ISA_STRING_NODE_STRUCTURE *)New;
  EFI_ACPI_6_5_ISA_STRING_NODE_STRUCTURE TempIsa = {
    0,
    21,
    2,//NumCores - 1,
    13,
    "rv64imafdcvhg"
  };
  CopyMem(IsaPointer, &TempIsa, sizeof(EFI_ACPI_6_5_ISA_STRING_NODE_STRUCTURE));
  New += sizeof (EFI_ACPI_6_5_ISA_STRING_NODE_STRUCTURE);

  EFI_ACPI_6_5_CMO_NODE_STRUCTURE *CmoPointer;
  CmoPointer = (EFI_ACPI_6_5_CMO_NODE_STRUCTURE *)New;
  EFI_ACPI_6_5_CMO_NODE_STRUCTURE TempCmo = {
    1,
    10,
    2,//NumCores - 1,
    0,
    6,
    6,
    6
  };
  CopyMem(CmoPointer, &TempCmo, sizeof(EFI_ACPI_6_5_CMO_NODE_STRUCTURE));
  New += sizeof (EFI_ACPI_6_5_CMO_NODE_STRUCTURE);

  EFI_ACPI_6_5_MMU_NODE_STRUCTURE *MmuPointer;
  MmuPointer = (EFI_ACPI_6_5_MMU_NODE_STRUCTURE *)New;
  EFI_ACPI_6_5_MMU_NODE_STRUCTURE TempMmu = {
    2,
    8,
    2,//NumCores - 1,
    0,
    0
  };
  CopyMem(MmuPointer, &TempMmu, sizeof(EFI_ACPI_6_5_MMU_NODE_STRUCTURE));
  New += sizeof (EFI_ACPI_6_5_MMU_NODE_STRUCTURE);

  INT32 CpuNode = 0;
  while (TRUE)
  {
    Status = FdtClient->FindNextCompatibleNode (FdtClient, "riscv", CpuNode, &CpuNode);
    if (EFI_ERROR (Status)) {
      break;
    }
    Status = FdtClient->GetNodeProperty (FdtClient, CpuNode, "phandle", &Property, &PropSize);
    UINT32 CoreIndex = SwapBytes32 (*(UINT32 *)Property);
    if (EFI_ERROR (Status) || PropSize != sizeof(UINT32)) {
      continue;
    }

    EFI_ACPI_6_5_HART_INFO_NODE_STRUCTURE *HartinfoPointer;
    HartinfoPointer = (EFI_ACPI_6_5_HART_INFO_NODE_STRUCTURE *)New;
    EFI_ACPI_6_5_HART_INFO_NODE_STRUCTURE TempHartinfo = EFI_ACPI_6_5_HART_INFO_NODE_INIT(CoreIndex);
    CopyMem(HartinfoPointer, &TempHartinfo, sizeof(EFI_ACPI_6_5_HART_INFO_NODE_STRUCTURE));
    New += sizeof(EFI_ACPI_6_5_HART_INFO_NODE_STRUCTURE);
  }

  AcpiPlatformChecksum ((UINT8 *)PageAddress, TableSize);

  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        (EFI_ACPI_COMMON_HEADER *)PageAddress,
                        TableSize,
                        &TableHandle
                        );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "安装RHCT表失败\n"));
  }

  return Status;
}

/**
  Entrypoint of Acpi Platform driver.

  @param  ImageHandle
  @param  SystemTable

  @return EFI_SUCCESS
  @return EFI_LOAD_ERROR
  @return EFI_OUT_OF_RESOURCES

**/
EFI_STATUS
EFIAPI
AcpiPlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_ACPI_TABLE_PROTOCOL        *AcpiTable;
  INTN                           Instance;
  EFI_ACPI_COMMON_HEADER         *CurrentTable;
  UINTN                          TableHandle;

  Instance     = 0;
  CurrentTable = NULL;
  TableHandle  = 0;

  //
  // Find the AcpiTable protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID**)&AcpiTable);
  DEBUG ((DEBUG_INFO, "加载ACPI：Locate EFI_ACPI_TABLE_PROTOCOL Status: %r\n", Status));
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }

  //Status = AddPpttTable (AcpiTable);
  //if (EFI_ERROR (Status)) {
  //  DEBUG ((DEBUG_ERROR, "安装PPTT表失败\n"));
  //}

  Status = AddMadtTable (AcpiTable);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "安装MADT表失败\n"));
  }

  Status = AddRhctTable (AcpiTable);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "安装RHCT表失败\n"));
  }

  return Status;
}
