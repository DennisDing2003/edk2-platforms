/** @file

 Copyright (c) 2019, Linaro Ltd. All rights reserved
 Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>

 SPDX-License-Identifier: BSD-2-Clause-Patent

 **/

 #include <Base.h>
 #include <Library/BaseLib.h>
 #include <Library/UefiBootServicesTableLib.h>
 #include <Protocol/FdtClient.h>
 #include <PiDxe.h>
 #include <Library/VirtNorFlashPlatformLib.h>
 #include <Library/DebugLib.h>

 #define QEMU_NOR_BLOCK_SIZE  SIZE_256KB

 EFI_STATUS
 VirtNorFlashPlatformInitialization (
   VOID
   )
 {
     return EFI_SUCCESS;
 }

 STATIC VIRT_NOR_FLASH_DESCRIPTION  mNorFlashDevices = {
     FixedPcdGet64 (PcdVariableFdBaseAddress),//0x22000000,
     FixedPcdGet64 (PcdRvspFlashNvStorageBase),//0x22000000,
     FixedPcdGet64 (PcdVariableFdSize),//0x2000000,
     QEMU_NOR_BLOCK_SIZE
 };

 EFI_STATUS
 VirtNorFlashPlatformGetDevices (
   OUT VIRT_NOR_FLASH_DESCRIPTION  **NorFlashDescriptions,
   OUT UINT32                      *Count
   )
 {
    *NorFlashDescriptions = &mNorFlashDevices;
    *Count                = 1;  // 返回两个Flash设备
    return EFI_SUCCESS;
 }
