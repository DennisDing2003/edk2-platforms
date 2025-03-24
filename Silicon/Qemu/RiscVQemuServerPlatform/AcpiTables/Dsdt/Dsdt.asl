/** @file
*  Differentiated System Description Table Fields (DSDT).
*
*  Copyright (c) 2024, Academy of Intelligent Innovation, Shandong Universiy, China.P.R. All rights reserved.<BR>
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <IndustryStandard/Acpi65.h>
#include <IndustryStandard/RiscVServerAcpi.h>

DefinitionBlock("DsdtTable.aml", "DSDT",
                 EFI_ACPI_6_5_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_REVISION,
                 "SDUAII", "RVSP    ", EFI_ACPI_RISCV_OEM_REVISION)
{
    include("Cpu.asl")
    include("Ethernet.asl")
    include("Intc.asl")
    include("Pci.asl")
    //UART
    Device (COM0){
        Name (_HID, "RV200003")
        Name (_UID, Zero)
        Name (_CRS, ResourceTemplate () {
          Memory32Fixed (ReadWrite,
                         FixedPcdGet32 (PcdSerialRegisterBase),
                         0x00001000)
          Interrupt (ResourceConsumer, Level, ActiveHigh, Exclusive) { 10 }
        })
        Method (_STA) {
          Return (0xF)
        }
    }
  
     
}