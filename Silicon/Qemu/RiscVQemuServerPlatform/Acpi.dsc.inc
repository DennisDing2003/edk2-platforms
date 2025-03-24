#/** @file
#
#  Copyright (c) 2024, Academy of Intelligent Innovation, Shandong Universiy, China.P.R. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
#**/

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag]
  gEfiMdeModulePkgTokenSpaceGuid.PcdInstallAcpiSdtProtocol|TRUE

[PcdsFixedAtBuild.common]
  # Define PCD entries for ACPI support
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|"SDU-AII"
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemTableId|0x2020202050535652 # RVSP
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemRevision|0x20180226 # Revision Code
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorId|0x41554453 # SDUA
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultCreatorRevision|1

################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################

[Components.common]
  #
  # ACPI support components for this platform
  #
  MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf
  MdeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
  Silicon/Qemu/RiscVQemuServerPlatform/AcpiTables/AcpiTables.inf
  # lack of RiscVServerAcpiDxe.inf

################################################################################
#
# Library Section - list of the library classes required for this platform
#
################################################################################

[LibraryClasses]
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  AcpiLib|EmbeddedPkg/Library/AcpiLib/AcpiLib.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLibDevicePathProtocol/UefiDevicePathLibDevicePathProtocol.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf

################################################################################
#
# Pcd Section - other general PCD Entries if needed
#
################################################################################

[PcdsFixedAtBuild.other]
  # Add other platform-specific PCD definitions here if required

