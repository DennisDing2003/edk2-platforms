## @file
# Includable build options
#
# @copyright
# Copyright (C) 2008 Intel Corporation.
#
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

[BuildOptions.Common.EDKII]
# Append build options for EDK and EDKII drivers (= is Append, == is Replace)
!if $(CRB_FLAG_ENABLE) == TRUE
  DEFINE CRB_EDKII_BUILD_OPTIONS = -D CRB_FLAG
!else
  DEFINE CRB_EDKII_BUILD_OPTIONS =
!endif

!if $(DEBUG_FLAGS_ENABLE) == TRUE
  DEFINE EDKII_DEBUG_BUILD_OPTIONS = -D DEBUG_CODE_BLOCK=1 -D PLATFORM_VARIABLE_ATTRIBUTES=0x3
!else
  DEFINE EDKII_DEBUG_BUILD_OPTIONS = -D SILENT_MODE -D PLATFORM_VARIABLE_ATTRIBUTES=0x3
!endif

!if $(SPARING_SCRATCHPAD_ENABLE) == TRUE
  DEFINE SPARING_SCRATCHPAD_OPTION = -D SPARING_SCRATCHPAD_SUPPORT
!else
  DEFINE SPARING_SCRATCHPAD_OPTIONS =
!endif

!if $(SCRATCHPAD_DEBUG) == TRUE
  DEFINE SCRATCHPAD_DEBUG_OPTION = -D SCRATCHPAD_DEBUG
!else
  DEFINE SCRATCHPAD_DEBUG_OPTION =
!endif

!if $(PCH_SERVER_BIOS_ENABLE) == TRUE
  DEFINE PCH_BUILD_OPTION = -DPCH_SERVER_BIOS_FLAG=1
!else
  DEFINE PCH_BUILD_OPTION =
!endif

!if $(SERVER_BIOS_ENABLE) == TRUE
  DEFINE SERVER_BUILD_OPTION = -DSERVER_BIOS_FLAG=1
!else
  DEFINE SERVER_BUILD_OPTION =
!endif

DEFINE SC_PATH = -D SC_PATH="Pch/SouthClusterLbg"

DEFINE ME_PATH = -D ME_PATH="Me/MeSps.4"

DEFINE IE_PATH = -D IE_PATH="Ie/v1"

DEFINE NVDIMM_OPTIONS =

!if $(CPUTARGET) == "ICX"
  DEFINE CPU_TYPE_OPTIONS  = -D ICX_HOST -D A0_HOST -D B0_HOST
  DEFINE MAX_IMC_CH_OPTIONS = -D MAX_IMC=4 -D MAX_MC_CH=2
!elseif $(CPUTARGET) == "CPX"
  DEFINE CPU_TYPE_OPTIONS  = -D SKX_HOST -D CLX_HOST -D CPX_HOST -D A0_HOST -D B0_HOST
  DEFINE MAX_IMC_CH_OPTIONS = -D MAX_IMC=2 -D MAX_MC_CH=3
!endif

DEFINE MAX_SOCKET_CORE_THREAD_OPTIONS = -D MAX_SOCKET=$(MAX_SOCKET) -D MAX_CORE=$(MAX_CORE) -D MAX_THREAD=$(MAX_THREAD)

DEFINE MRC_OPTIONS = -D LRDIMM_SUPPORT -D DDRT_SUPPORT

DEFINE MAX_SAD_RULE_OPTION = -D MAX_SAD_RULES=24 -D MAX_DRAM_CLUSTERS=1

DEFINE LT_BUILD_OPTIONS = -D LT_FLAG

DEFINE FSP_BUILD_OPTIONS = -D FSP_DISPATCH_MODE_ENABLE=1

#
# MAX_KTI_PORTS needs to be updated based on the silicon type
#
!if $(CPUTARGET) == "CPX"
  DEFINE KTI_OPTIONS = -D MAX_KTI_PORTS=6
!else
  DEFINE KTI_OPTIONS = -D MAX_KTI_PORTS=3
!endif

DEFINE IIO_STACK_OPTIONS = -D MAX_IIO_STACK=6 -D MAX_LOGIC_IIO_STACK=8

DEFINE PCH_BIOS_BUILD_OPTIONS = $(PCH_BUILD_OPTION) $(SC_PATH) $(SERVER_BUILD_OPTION)

DEFINE EDKII_DSC_FEATURE_BUILD_OPTIONS = $(CRB_EDKII_BUILD_OPTIONS) $(EDKII_DEBUG_BUILD_OPTIONS) $(PCH_BIOS_BUILD_OPTIONS) $(MAX_SOCKET_CORE_THREAD_OPTIONS) $(MAX_IMC_CH_OPTIONS) $(MAX_SAD_RULE_OPTION) $(KTI_OPTIONS) $(IIO_STACK_OPTIONS) $(LT_BUILD_OPTIONS) $(SPARING_SCRATCHPAD_OPTION) $(SCRATCHPAD_DEBUG_OPTION) $(NVDIMM_OPTIONS) -D EFI_PCI_IOV_SUPPORT -D WHEA_SUPPORT $(CPU_TYPE_OPTIONS) -D MMCFG_BASE_ADDRESS=0x80000000 -D DISABLE_NEW_DEPRECATED_INTERFACES $(MRC_OPTIONS) $(FSP_BUILD_OPTIONS)

DEFINE IE_OPTIONS = $(IE_PATH)

!if $(LINUX_GCC_BUILD) == TRUE
  DEFINE EDK2_LINUX_BUILD_OPTIONS = -D EDK2_CTE_BUILD
!else
  DEFINE EDK2_LINUX_BUILD_OPTIONS =
!endif

DEFINE EDKII_DSC_FEATURE_BUILD_OPTIONS = $(EDKII_DSC_FEATURE_BUILD_OPTIONS) $(EDK2_LINUX_BUILD_OPTIONS) $(IE_OPTIONS)

DEFINE ME_OPTIONS = -DSPS_VERSION=4 $(ME_PATH)

DEFINE ASPEED_ENABLE_BUILD_OPTIONS = -D ASPEED_ENABLE -D ESPI_ENABLE

DEFINE EDKII_DSC_FEATURE_BUILD_OPTIONS = $(EDKII_DSC_FEATURE_BUILD_OPTIONS) $(ME_OPTIONS) $(ASPEED_ENABLE_BUILD_OPTIONS)

  MSFT:*_*_*_CC_FLAGS= $(EDKII_DSC_FEATURE_BUILD_OPTIONS) /wd4819
  GCC:*_*_*_CC_FLAGS= $(EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_*_VFRPP_FLAGS = $(EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_*_APP_FLAGS   = $(EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_*_PP_FLAGS    = $(EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_*_ASLPP_FLAGS = $(EDKII_DSC_FEATURE_BUILD_OPTIONS)
  *_*_*_ASLCC_FLAGS = $(EDKII_DSC_FEATURE_BUILD_OPTIONS)


#
# Enable source level debugging for RELEASE build
#
!if $(TARGET) == "RELEASE"
  DEFINE EDKII_RELEASE_SRCDBG_ASM_BUILD_OPTIONS   =
  DEFINE EDKII_RELEASE_SRCDBG_CC_BUILD_OPTIONS    =
  DEFINE EDKII_RELEASE_SRCDBG_DLINK_BUILD_OPTIONS =

  MSFT:*_*_*_ASM_FLAGS   = $(EDKII_RELEASE_SRCDBG_ASM_BUILD_OPTIONS) /Zi
  MSFT:*_*_*_CC_FLAGS    = $(EDKII_RELEASE_SRCDBG_CC_BUILD_OPTIONS) /Z7
  MSFT:*_*_*_DLINK_FLAGS = $(EDKII_RELEASE_SRCDBG_DLINK_BUILD_OPTIONS) /DEBUG
  GCC:*_*_*_ASM_FLAGS    = $(EDKII_RELEASE_SRCDBG_ASM_BUILD_OPTIONS)
  GCC:*_*_*_CC_FLAGS     = $(EDKII_RELEASE_SRCDBG_CC_BUILD_OPTIONS)
  GCC:*_*_*_DLINK_FLAGS  = $(EDKII_RELEASE_SRCDBG_DLINK_BUILD_OPTIONS)
!endif

#
# Override the VFR compile flags to speed the build time
#
*_*_*_VFR_FLAGS                     ==  -n

#
# Disable remarks, enable warnings as errors, disable integer optimization,
#
  *_*_*_ASL_FLAGS = -vr -we -oi

#
# add to the build options for DXE/SMM drivers to remove the log message:
# !!!!!!!!  InsertImageRecord - Section Alignment(0x20) is not 4K  !!!!!!!!
#
[BuildOptions.common.EDKII.DXE_RUNTIME_DRIVER, BuildOptions.common.EDKII.DXE_SMM_DRIVER, BuildOptions.common.EDKII.SMM_CORE]
   MSFT:*_*_*_DLINK_FLAGS = /ALIGN:4096

[BuildOptions]
  GCC:*_GCC5_*_CC_FLAGS = -Wno-overflow -Wno-discarded-qualifiers -Wno-unused-variable -Wno-unused-but-set-variable -Wno-incompatible-pointer-types -mabi=ms
  GCC:*_GCC5_IA32_DLINK_FLAGS = -z common-page-size=0x20 -z muldefs
  GCC:*_GCC5_X64_DLINK_FLAGS  = -z common-page-size=0x20 -z muldefs
  MSFT:*_*_*_CC_FLAGS = /FAsc
