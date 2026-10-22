// Memory map of the Lenovo TB128FU (SM6225, bengal, Khaje IDP).
//
// The reservations are the /reserved-memory nodes of the live device tree; the
// free DRAM ranges are the System RAM the running kernel reports in
// /proc/iomem. The firmware keeps the stock ABL's UEFI window below 0x60000000
// and puts DarwinBoot's window in the 0x80000000..0x1c0000000 bank.
#include <Library/MemoryMapLib.h>

STATIC
EFI_MEMORY_REGION_DESCRIPTOR
gMemoryDescriptor[] = {
  // Name, Address, Length, HobOption, ResourceType, ResourceAttribute, MemoryType, ArmAttribute

  // DDR Regions
  {"Kernel",             0x40000000, 0x05700000, AddMem, MEM_RES, SYS_MEM_CAP, Reserv, WRITE_BACK_XN},
  {"HYP",                0x45700000, 0x00600000, AddMem, SYS_MEM, SYS_MEM_CAP, Reserv, WRITE_BACK_XN},
  {"Boot Info",          0x45D00000, 0x00020000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK_XN},
  {"XBL AOP",            0x45E00000, 0x00140000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"Sec Apps",           0x45FFF000, 0x00001000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"SMEM",               0x46000000, 0x00200000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"CDSP Secure",        0x46200000, 0x01E00000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"PIL Reserved",       0x4AB00000, 0x00AB7000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"DRAM 0x48000000",    0x48000000, 0x02B00000, AddMem, SYS_MEM, SYS_MEM_CAP, Conv,   WRITE_BACK_XN},
  {"DRAM 0x55617000",    0x55617000, 0x069E9000, AddMem, SYS_MEM, SYS_MEM_CAP, Conv,   WRITE_BACK_XN},
  {"Display Reserved",   0x5C000000, 0x00F00000, AddMem, MEM_RES, SYS_MEM_CAP, Reserv, WRITE_THROUGH_XN},
  {"DFPS Data",          0x5CF00000, 0x00100000, AddMem, MEM_RES, SYS_MEM_CAP, Reserv, WRITE_THROUGH_XN},
  {"DXE Heap",           0x5D000000, 0x02800000, AddMem, SYS_MEM, SYS_MEM_CAP, Conv,   WRITE_BACK_XN},
  {"UEFI FD",            0x5FC00000, 0x00300000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK},
  {"SEC Heap",           0x5FF00000, 0x0008C000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK_XN},
  {"MMU PageTables",     0x5FF8D000, 0x00003000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK_XN},
  {"UEFI Stack",         0x5FF90000, 0x00040000, AddMem, SYS_MEM, SYS_MEM_CAP, BsData, WRITE_BACK_XN},
  {"Log Buffer",         0x5FFF7000, 0x00008000, AddMem, SYS_MEM, SYS_MEM_CAP, RtData, WRITE_BACK_XN},
  {"Info Blk",           0x5FFFF000, 0x00001000, AddMem, SYS_MEM, SYS_MEM_CAP, RtData, WRITE_BACK_XN},
  {"Removed Region",     0x60000000, 0x03900000, AddMem, MEM_RES, UNCACHEABLE, Reserv, UNCACHED_UNBUFFERED_XN},
  {"DRAM 0x63900000",    0x63900000, 0x19300000, AddMem, SYS_MEM, SYS_MEM_CAP, Conv,   WRITE_BACK_XN},
  // The boot image is loaded at 0x80000000 (kernel + appended DTB, 16 MiB) and
  // its ramdisk at 0x81000000 (the 56 MiB UniXNU payload volume). Reserve both
  // before the DXE allocator or DarwinBoot can write over the volume while the
  // FAT driver is still reading it. The rest of the second and third /memory
  // banks stays conventional for DarwinBoot's window.
  {"Boot Image",         0x80000000, 0x01000000, AddMem, MEM_RES, SYS_MEM_CAP, Reserv, WRITE_BACK_XN},
  {"Ramdisk",            0x81000000, 0x03800000, AddMem, MEM_RES, SYS_MEM_CAP, Reserv, WRITE_BACK_XN},
  {"DRAM 0x84800000",    0x84800000, 0x13B800000, AddMem, SYS_MEM, SYS_MEM_CAP, Conv,  WRITE_BACK_XN},

  // Other Memory Regions
  {"IMEM Base",          0x0C100000, 0x00026000, NoHob,  MMAP_IO, INITIALIZED, Conv, NS_DEVICE},
  {"IMEM Cookie Base",   0x0C125000, 0x00001000, AddDev, MMAP_IO, INITIALIZED, Conv, NS_DEVICE},
  {"RPM_SS_MSG_RAM",     0x045F0000, 0x00007000, NoHob,  MMAP_IO, INITIALIZED, Conv, NS_DEVICE},

  // Register Regions
  {"TCSR_TCSR_REGS",     0x003C0000, 0x00040000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"TLMM_NORTH",         0x00400000, 0x00100000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"TLMM_WEST",          0x00500000, 0x00300000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"TLMM_SOUTH",         0x00900000, 0x00300000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"TLMM_EAST",          0x00D00000, 0x00300000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"GCC CLK CTL",        0x01400000, 0x00200000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"USB2",               0x01610000, 0x00010000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"MMCX_CPR3",          0x01648000, 0x00008000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"CRYPTO0 CRYPTO",     0x01B00000, 0x00040000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"SECURITY CONTROL",   0x01B40000, 0x00010000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"PRNG_CFG_PRNG",      0x01B50000, 0x00010000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"PMIC ARB SPMI",      0x01C00000, 0x02800000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"SLP_CNTR",           0x04403000, 0x00001000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"PSHOLD",             0x0440B000, 0x00001000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"TSENS0",             0x04410000, 0x00001000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"TSENS0_TM",          0x04411000, 0x00001000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"MCCC_MCCC_MSTR",     0x0447D000, 0x00001000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"UFS UFS REGS",       0x04800000, 0x00020000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"QUPV3_0_GSI",        0x04A00000, 0x000D0000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"USB30_PRIM",         0x04E00000, 0x00200000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"MDSS",               0x05E00000, 0x00200000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"SMMU",               0x0C600000, 0x00080000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"APSS_WDT_TMR1",      0x0F017000, 0x00001000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"QTIMER",             0x0F020000, 0x00110000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"APSS_GIC500_GICD",   0x0F200000, 0x00010000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"APSS_GIC500_GICR",   0x0F300000, 0x00100000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE},
  {"APSS_ACTPM_WRAP",    0x0F500000, 0x000B0000, AddDev, MMAP_IO, UNCACHEABLE, MmIO, NS_DEVICE}
};

VOID
GetMemoryMap (
  OUT EFI_MEMORY_REGION_DESCRIPTOR **MemoryDescriptor,
  OUT UINT8                         *MemoryDescriptorCount)
{
  // Pass Data
  *MemoryDescriptor      = gMemoryDescriptor;
  *MemoryDescriptorCount = ARRAY_SIZE (gMemoryDescriptor);
}
