/** @file
*
*  Copyright (c) 2025, Academy of Intelligent Innovation, Shandong Universiy, China.P.R.. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef __RISCV_ACPI_HEADER__
#define __RISCV_ACPI_HEADER__

#include <IndustryStandard/Acpi.h>

//
// ACPI table information used to initialize tables.
//
#define EFI_ACPI_RISCV_OEM_ID           'S','D','U','A','I','I'
#define EFI_ACPI_RISCV_OEM_TABLE_ID     SIGNATURE_64 ('S','D','U','A',' ',' ',' ',' ')
#define EFI_ACPI_RISCV_OEM_REVISION     0x20250407
#define EFI_ACPI_RISCV_CREATOR_ID       SIGNATURE_32('S','D','U','A')
#define EFI_ACPI_RISCV_CREATOR_REVISION 0x00000099

//
// "RHCT" RISC-V Hart Capabilities Table
//
#define EFI_ACPI_6_6_RISCV_HART_CAPABILITIES_TABLE_SIGNATURE  SIGNATURE_32('R', 'H', 'C', 'T')
//
// RHCT Revision (as defined in ACPI 6.5 spec.)
//
#define EFI_ACPI_6_6_RISCV_HART_CAPABILITIES_TABLE_REVISION  0x01

// ISA string node structure
typedef struct {
  UINT16     Type;
  UINT16     Length;
  UINT16     Revision;
  UINT16     ISALen;
  CHAR8      ISAStr[13];
} EFI_ACPI_6_5_ISA_STRING_NODE_STRUCTURE;

// CMO node structure
typedef struct {
  UINT16     Type;
  UINT16     Length;
  UINT16     Revision;
  UINT8      Reserved;
  UINT8      CBOMBlkSize;
  UINT8      CBOPBlkSize;
  UINT8      CBOZBlkSize;
} EFI_ACPI_6_5_CMO_NODE_STRUCTURE;

// MMU node structure
typedef struct {
  UINT16     Type;
  UINT16     Length;
  UINT16     Revision;
  UINT8      Reserved;
  UINT8      MMUType;
} EFI_ACPI_6_5_MMU_NODE_STRUCTURE;

// Hart Info Node Structure
typedef struct {
  UINT16     Type;
  UINT16     Length;
  UINT16     Revision;
  UINT16     NumOffset;
  UINT32     AcpiProcessorUid;
  UINT32     Offsets[3];
} EFI_ACPI_6_5_HART_INFO_NODE_STRUCTURE;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER    Header;
  UINT32                         Flags;
  UINT64                         TimeBaseFreq;
  UINT32                         NumNodes;
  UINT32                         OffsetNodes;
} EFI_ACPI_6_5_RISCV_HART_CAPABILITIES_TABLE_HEADER;

// RHCT Node[N] starts at offset 56
#define RHCT_NODE_ARRAY_OFFSET     56
#define RHCT_ISA_STRING_NODE_SIZE  sizeof(EFI_ACPI_6_5_ISA_STRING_NODE_STRUCTURE)
#define RHCT_CMO_NODE_SIZE         sizeof(EFI_ACPI_6_5_CMO_NODE_STRUCTURE)

// EFI_ACPI_6_6_HART_INFO_NODE_INIT
#define EFI_ACPI_6_5_HART_INFO_NODE_INIT(AcpiProcessorUid) {                   \
  65535,                                                                     \
  sizeof (EFI_ACPI_6_5_HART_INFO_NODE_STRUCTURE),                            \
  1,                                                                         \
  3,                                                                         \
  AcpiProcessorUid,                                                          \
  {                                                                          \
    RHCT_NODE_ARRAY_OFFSET,                                                  \
    RHCT_NODE_ARRAY_OFFSET + RHCT_ISA_STRING_NODE_SIZE,                      \
    RHCT_NODE_ARRAY_OFFSET + RHCT_ISA_STRING_NODE_SIZE + RHCT_CMO_NODE_SIZE  \
  }                                                                          \
}

// A macro to initialise the common header part of EFI ACPI tables as defined by
// EFI_ACPI_DESCRIPTION_HEADER structure.
#define RISCV_ACPI_HEADER(Signature, Type, Revision) {            \
    Signature,                      /* UINT32  Signature */           \
    sizeof (Type),                  /* UINT32  Length */              \
    Revision,                       /* UINT8   Revision */           \
    0,                              /* UINT8   Checksum */           \
    { EFI_ACPI_RISCV_OEM_ID }, /* UINT8   OemId[6] */           \
    EFI_ACPI_RISCV_OEM_TABLE_ID, /* UINT64  OemTableId */    \
    EFI_ACPI_RISCV_OEM_REVISION, /* UINT32  OemRevision */   \
    EFI_ACPI_RISCV_CREATOR_ID, /* UINT32  CreatorId */      \
    EFI_ACPI_RISCV_CREATOR_REVISION /* UINT32  CreatorRevision */ \
}

//
// PPTT processor structure flags for different SoC components as defined in
// ACPI 6.5 specification
//

// Processor structure flags for SoC package
#define PPTT_PROCESSOR_PACKAGE_FLAGS                                           \
  {                                                                            \
    EFI_ACPI_6_5_PPTT_PACKAGE_PHYSICAL,                                        \
    EFI_ACPI_6_5_PPTT_PROCESSOR_ID_INVALID,                                    \
    EFI_ACPI_6_5_PPTT_PROCESSOR_IS_NOT_THREAD,                                 \
    EFI_ACPI_6_5_PPTT_NODE_IS_NOT_LEAF,                                        \
    EFI_ACPI_6_5_PPTT_IMPLEMENTATION_IDENTICAL                                 \
  }

// Processor structure flags for cluster
#define PPTT_PROCESSOR_CLUSTER_FLAGS                                           \
  {                                                                            \
    EFI_ACPI_6_5_PPTT_PACKAGE_NOT_PHYSICAL,                                    \
    EFI_ACPI_6_5_PPTT_PROCESSOR_ID_VALID,                                      \
    EFI_ACPI_6_5_PPTT_PROCESSOR_IS_NOT_THREAD,                                 \
    EFI_ACPI_6_5_PPTT_NODE_IS_NOT_LEAF,                                        \
    EFI_ACPI_6_5_PPTT_IMPLEMENTATION_IDENTICAL                                 \
  }

// Processor structure flags for cluster with multi-thread core
#define PPTT_PROCESSOR_CLUSTER_THREADED_FLAGS                                  \
  {                                                                            \
    EFI_ACPI_6_5_PPTT_PACKAGE_NOT_PHYSICAL,                                    \
    EFI_ACPI_6_5_PPTT_PROCESSOR_ID_INVALID,                                    \
    EFI_ACPI_6_5_PPTT_PROCESSOR_IS_NOT_THREAD,                                 \
    EFI_ACPI_6_5_PPTT_NODE_IS_NOT_LEAF,                                        \
    EFI_ACPI_6_5_PPTT_IMPLEMENTATION_IDENTICAL                                 \
  }

// Processor structure flags for single-thread core
#define PPTT_PROCESSOR_CORE_FLAGS                                              \
  {                                                                            \
    EFI_ACPI_6_5_PPTT_PACKAGE_NOT_PHYSICAL,                                    \
    EFI_ACPI_6_5_PPTT_PROCESSOR_ID_VALID,                                      \
    EFI_ACPI_6_5_PPTT_PROCESSOR_IS_NOT_THREAD,                                 \
    EFI_ACPI_6_5_PPTT_NODE_IS_LEAF                                             \
  }

// Processor structure flags for multi-thread core
#define PPTT_PROCESSOR_CORE_THREADED_FLAGS                                     \
  {                                                                            \
    EFI_ACPI_6_5_PPTT_PACKAGE_NOT_PHYSICAL,                                    \
    EFI_ACPI_6_5_PPTT_PROCESSOR_ID_INVALID,                                    \
    EFI_ACPI_6_5_PPTT_PROCESSOR_IS_NOT_THREAD,                                 \
    EFI_ACPI_6_5_PPTT_NODE_IS_NOT_LEAF,                                        \
    EFI_ACPI_6_5_PPTT_IMPLEMENTATION_IDENTICAL                                 \
  }

// Processor structure flags for CPU thread
#define PPTT_PROCESSOR_THREAD_FLAGS                                            \
  {                                                                            \
    EFI_ACPI_6_5_PPTT_PACKAGE_NOT_PHYSICAL,                                    \
    EFI_ACPI_6_5_PPTT_PROCESSOR_ID_VALID,                                      \
    EFI_ACPI_6_5_PPTT_PROCESSOR_IS_THREAD,                                     \
    EFI_ACPI_6_5_PPTT_NODE_IS_LEAF                                             \
  }

// PPTT cache structure flags as defined in ACPI 6.5 Specification
#define PPTT_CACHE_STRUCTURE_FLAGS                                             \
  {                                                                            \
    EFI_ACPI_6_5_PPTT_CACHE_SIZE_VALID,                                        \
    EFI_ACPI_6_5_PPTT_NUMBER_OF_SETS_VALID,                                    \
    EFI_ACPI_6_5_PPTT_ASSOCIATIVITY_VALID,                                     \
    EFI_ACPI_6_5_PPTT_ALLOCATION_TYPE_VALID,                                   \
    EFI_ACPI_6_5_PPTT_CACHE_TYPE_VALID,                                        \
    EFI_ACPI_6_5_PPTT_WRITE_POLICY_VALID,                                      \
    EFI_ACPI_6_5_PPTT_LINE_SIZE_VALID,                                         \
    EFI_ACPI_6_5_PPTT_CACHE_ID_VALID                                           \
  }

// PPTT cache attributes for data cache
#define PPTT_DATA_CACHE_ATTR                                                   \
  {                                                                            \
    EFI_ACPI_6_5_CACHE_ATTRIBUTES_ALLOCATION_READ_WRITE,                       \
    EFI_ACPI_6_5_CACHE_ATTRIBUTES_CACHE_TYPE_DATA,                             \
    EFI_ACPI_6_5_CACHE_ATTRIBUTES_WRITE_POLICY_WRITE_BACK                      \
  }

// PPTT cache attributes for instruction cache
#define PPTT_INST_CACHE_ATTR                                                   \
  {                                                                            \
    EFI_ACPI_6_5_CACHE_ATTRIBUTES_ALLOCATION_READ,                             \
    EFI_ACPI_6_5_CACHE_ATTRIBUTES_CACHE_TYPE_INSTRUCTION,                      \
    EFI_ACPI_6_5_CACHE_ATTRIBUTES_WRITE_POLICY_WRITE_BACK                      \
  }

// PPTT cache attributes for unified cache
#define PPTT_UNIFIED_CACHE_ATTR                                                \
  {                                                                            \
    EFI_ACPI_6_5_CACHE_ATTRIBUTES_ALLOCATION_READ_WRITE,                       \
    EFI_ACPI_6_5_CACHE_ATTRIBUTES_CACHE_TYPE_UNIFIED,                          \
    EFI_ACPI_6_5_CACHE_ATTRIBUTES_WRITE_POLICY_WRITE_BACK                      \
  }

#define RISCVQEMU_ACPI_PPTT_L1_D_CACHE_STRUCT  {                                \
    EFI_ACPI_6_5_PPTT_TYPE_CACHE,                                              \
    sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE),                                \
    { EFI_ACPI_RESERVED_BYTE, EFI_ACPI_RESERVED_BYTE },                        \
    {                                                                          \
      1,                       /* SizePropertyValid */                         \
      1,                       /* NumberOfSetsValid */                         \
      1,                       /* AssociativityValid */                        \
      1,                       /* AllocationTypeValid */                       \
      1,                       /* CacheTypeValid */                            \
      1,                       /* WritePolicyValid */                          \
      1,                       /* LineSizeValid */                             \
      1,                       /* CacheIdValid */                              \
    },                                                                         \
    0,                         /* NextLevelOfCache */                          \
    0,                         /* Size */                                      \
    0,                         /* NumberOfSets */                              \
    0,                         /* Associativity */                             \
    {                                                                          \
      EFI_ACPI_6_5_CACHE_ATTRIBUTES_ALLOCATION_READ_WRITE,                     \
      EFI_ACPI_6_5_CACHE_ATTRIBUTES_CACHE_TYPE_DATA,                           \
      EFI_ACPI_6_5_CACHE_ATTRIBUTES_WRITE_POLICY_WRITE_BACK,                   \
    },                                                                         \
    64,                        /* LineSize */                                  \
    0                          /* CacheId */                                   \
  }

#define RISCVQEMU_ACPI_PPTT_L1_I_CACHE_STRUCT  {                                \
    EFI_ACPI_6_5_PPTT_TYPE_CACHE,                                              \
    sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE),                                \
    { EFI_ACPI_RESERVED_BYTE, EFI_ACPI_RESERVED_BYTE },                        \
    {                                                                          \
      1,                       /* SizePropertyValid */                         \
      1,                       /* NumberOfSetsValid */                         \
      1,                       /* AssociativityValid */                        \
      1,                       /* AllocationTypeValid */                       \
      1,                       /* CacheTypeValid */                            \
      1,                       /* WritePolicyValid */                          \
      1,                       /* LineSizeValid */                             \
      1,                       /* CacheIdValid */                              \
    },                                                                         \
    0,                         /* NextLevelOfCache */                          \
    0,                         /* Size */                                      \
    0,                         /* NumberOfSets */                              \
    0,                         /* Associativity */                             \
    {                                                                          \
      EFI_ACPI_6_5_CACHE_ATTRIBUTES_ALLOCATION_READ,                           \
      EFI_ACPI_6_5_CACHE_ATTRIBUTES_CACHE_TYPE_INSTRUCTION,                    \
      0,                                                                       \
    },                                                                         \
    64,                        /* LineSize */                                  \
    0                          /* CacheId */                                   \
  }

#define RISCVQEMU_ACPI_PPTT_L2_CACHE_STRUCT  {                                  \
    EFI_ACPI_6_5_PPTT_TYPE_CACHE,                                              \
    sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_CACHE),                                \
    { EFI_ACPI_RESERVED_BYTE, EFI_ACPI_RESERVED_BYTE },                        \
    {                                                                          \
      1,                     /* SizePropertyValid */                           \
      1,                     /* NumberOfSetsValid */                           \
      1,                     /* AssociativityValid */                          \
      1,                     /* AllocationTypeValid */                         \
      1,                     /* CacheTypeValid */                              \
      1,                     /* WritePolicyValid */                            \
      1,                     /* LineSizeValid */                               \
      1,                     /* CacheIdValid */                                \
    },                                                                         \
    0,                         /* NextLevelOfCache */                          \
    0,                         /* Size */                                      \
    0,                         /* NumberOfSets */                              \
    0,                         /* Associativity */                             \
    {                                                                          \
      EFI_ACPI_6_5_CACHE_ATTRIBUTES_ALLOCATION_READ_WRITE,                     \
      EFI_ACPI_6_5_CACHE_ATTRIBUTES_CACHE_TYPE_UNIFIED,                        \
      EFI_ACPI_6_5_CACHE_ATTRIBUTES_WRITE_POLICY_WRITE_BACK,                   \
    },                                                                         \
    64,                      /* LineSize */                                    \
    0                        /* CacheId */                                     \
  }

#define RISCVQEMU_ACPI_PROCESSOR_HIERARCHY_NODE_STRUCTURE_INIT(Flags, Parent, ACPIProcessorID, NumberOfPrivateResources)             \
{                                                                                                                                 \
  EFI_ACPI_6_5_PPTT_TYPE_PROCESSOR,                                                            /* Type */                         \
  sizeof (EFI_ACPI_6_5_PPTT_STRUCTURE_PROCESSOR) + NumberOfPrivateResources * sizeof (UINT32), /* Length */                       \
  { EFI_ACPI_RESERVED_BYTE, EFI_ACPI_RESERVED_BYTE },                                          /* Reserved */                     \
  Flags,                                                                                       /* Flags */                        \
  Parent,                                                                                      /* Parent */                       \
  ACPIProcessorID,                                                                             /* ACPI Processor ID */            \
  NumberOfPrivateResources                                                                     /* Number of private resources */  \
}

// RINTC Structure
typedef struct {
  UINT8     Type;
  UINT8     Length;
  UINT8     Version;
  UINT8     Reserved;
  UINT32    Flags;
  UINT64    HartId;
  UINT32    AcpiProcessorUid;
  UINT32    ExternalInterruptId;
  UINT64    IMSICBaseAddress;
  UINT32    IMSICSize;
} EFI_ACPI_6_6_RINTC_STRUCTURE;

#define ACPI_BUILD_INTC_ID(socket, index) ((socket << 24) | (index))

// EFI_ACPI_6_6_RINTC_STRUCTURE
#define EFI_ACPI_6_6_RINTC_STRUCTURE_INIT(Flags, HartId, AcpiCpuUid,               \
  ExternalInterruptId, IMSICBase, IMSICSize) {                                     \
    0x18,                                   /* Type */                             \
    sizeof (EFI_ACPI_6_6_RINTC_STRUCTURE),  /* Length */                           \
    1,                                      /* Version */                          \
    EFI_ACPI_RESERVED_BYTE,                 /* Reserved */                         \
    Flags,                                  /* Flags */                            \
    HartId,                                 /* Hart ID */                          \
    AcpiCpuUid,                             /* AcpiProcessorUid */                 \
    ExternalInterruptId,                    /* External Interrupt Controller ID */ \
    IMSICBase,                              /* IMSIC Base address */               \
    IMSICSize,                              /* IMSIC Size */                       \
  }

#endif