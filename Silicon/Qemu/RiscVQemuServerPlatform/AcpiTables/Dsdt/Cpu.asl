/** @file
*  Differentiated System Description Table Fields (DSDT)
*
*  Copyright (c) 2023, Academy of Intelligent Innovation, Shandong Universiy, China.P.R. All rights reserved.<BR>
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

Scope(_SB)
{
  Method (_OSC,4) {
    CreateDWordField(Arg3,0,CDW1)
    CreateDWordField(Arg3,4,CDW2)

    If (LEqual(Arg0,ToUUID("0811B06E-4A27-44F9-8D60-3CBBC22E7B48"))) {
      If (LNotEqual(Arg1,One)) {
        Or (CDW1,0x0A,CDW1)
      } Else {
        And (CDW2,0xC0,CDW2)
      }
      Return (Arg3)
    } Else {
      Or (CDW1,0x6,CDW1) // Unrecognized UUID
      Return (Arg3)
    }
  }

  Name (CLPI, Package () {
    0, 0, 1,
    Package () { 3500, 100, 1, 0, 100, 1, 0x1000000080000000, "RISC-V NONRET_DEFAULT" }
  })

  Name (PLPI, Package () {
    0, 0, 3,
    Package () { 1, 1, 1, 0, 100, 0, 0x0000000000000000, "RISC-V WFI" },
    Package () { 10, 10, 1, 0, 100, 1, 0x1000000000000000, "RISC-V RET_DEFAULT" },
    Package () { 3500, 100, 1, 0, 100, 1, 0x1000000080000000, "RISC-V NONRET_DEFAULT" }
  })

  Device (CL00) {
    Name (_HID, "ACPI0007") // 统一 CPU 设备 ID
    Name (_UID, 0)
    Method (_LPI, 0, NotSerialized) {
      Return (\_SB.CLPI)
    }

    Device (CP00) {
      Name (_HID, "ACPI0007")
      Name (_UID, 0)
      Name (_STA, 0xF)

      Name (_MAT, Buffer() {
        0x18, 0x24, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* Hart ID */
        0x00, 0x00, 0x00, 0x00,                         /* AcpiProcessorUid */
        0x00, 0x00, 0x00, 0x00,                         /* External Interrupt Controller ID */
        0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* IMSIC Base address */
        0x00, 0x00, 0x00, 0x00
      })

      Name (_PSD, Package () {
        Package () { 5, 0, 0, 0xFD, 2 }
      })

      Method (_LPI, 0, NotSerialized) {
        Return (\_SB.PLPI)
      }
    }

    Device (CP01) {
      Name (_HID, "ACPI0007")
      Name (_UID, 1)
      Name (_STA, 0xF)

      Name (_MAT, Buffer() {
        0x18, 0x24, 0x01, 0x00,
        0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, /* Hart ID */
        0x00, 0x00, 0x00, 0x01,                         /* AcpiProcessorUid */
        0x00, 0x00, 0x00, 0x01,                         /* External Interrupt Controller ID */
        0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* IMSIC Base address */
        0x00, 0x00, 0x00, 0x00
      })

      Name (_PSD, Package () {
        Package () { 5, 0, 0, 0xFD, 2 }
      })

      Method (_LPI, 0, NotSerialized) {
        Return (\_SB.PLPI)
      }
    }
  }
}
