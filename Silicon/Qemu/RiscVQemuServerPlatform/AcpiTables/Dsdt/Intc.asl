/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2025, Academy of Intelligent Innovation, Shandong Universiy, China.P.R. All rights reserved.<BR>

**/

Scope(_SB)
{
  Device (INTC) {        // Top intc
    Name(_HID, "RV200002")
    Name(_CRS, ResourceTemplate() {
        QWordMemory (
            ResourceProducer, PosDecode,
            MinFixed, MaxFixed,
            NonCacheable, ReadWrite,
            0x0,                         // Granularity
            0x30000000,                  // Min Base Address
            0x3FFFFFFF,                  // Max Base Address
            0x0000000000,                // Translate
            0x0010000000                   // Length (256MB)
        )//ECAM

        QWordMemory (
            ResourceProducer, PosDecode,
            MinFixed, MaxFixed,
            NonCacheable, ReadWrite,
            0x0,                         // Granularity
            0x40000000,                  // Min Base Address
            0x7FFFFFFF,                  // Max Base Address
            0x0000000000,                // Translate
            0x40000000                   // Length (1GB)
        )// 32-bit PCI MMIO (Enternet、AHCI)

        QWordMemory (
            ResourceProducer, PosDecode,
            MinFixed, MaxFixed,
            NonCacheable, ReadWrite,
            0x0,                         // Granularity
            0x10000000000,               // Min Base Address
            0x1FFFFFFFFFF,               // Max Base Address
            0x0000000000,                // Translate
            0x10000000000                // Length (64GB)
        )// 64-bit PCI MMIO (用于 xHCI)

      Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive,,,) {
        11, 12, 13, 14, 15, 16, 17,
      }
    })

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () { "top-intc-id", 0 },
        Package () { "for-msi", 1 },
        Package () { "reg-bitwidth", 32 },
      }
    })
  }
}