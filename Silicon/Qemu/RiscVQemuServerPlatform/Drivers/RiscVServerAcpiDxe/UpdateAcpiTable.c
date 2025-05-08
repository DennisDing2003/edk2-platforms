/** @file
  Copyright (c) 2016, Hisilicon Limited. All rights reserved.
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <IndustryStandard/Acpi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Protocol/FdtClient.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <IndustryStandard/RiscVServerAcpi.h>

#define CORECOUNT(X) ((X) * CORE_NUM_PER_SOCKET)

EFI_STATUS
UpdateAcpiTable (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER      *TableHeader
)
{

  EFI_STATUS Status = EFI_SUCCESS;
/*
  switch (TableHeader->Signature) {

  case EFI_ACPI_6_5_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE:
    Status = UpdateSrat ((EFI_ACPI_STATIC_RESOURCE_AFFINITY_TABLE *) TableHeader);
    break;

  case EFI_ACPI_6_5_SYSTEM_LOCALITY_INFORMATION_TABLE_SIGNATURE:
    Status = UpdateSlit (TableHeader);
    break;
  }
*/
  return Status;
}