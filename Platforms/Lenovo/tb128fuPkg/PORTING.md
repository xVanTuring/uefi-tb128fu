# TB128FU for UniXNU

> **Work in progress.** Prepared with the tablet under Android; nothing here has run on a
> Lenovo TB128FU yet.

This branch adds the Lenovo Xiaoxin Pad 2022 (TB128FU, SM6225 bengal) to the
Mu-Silicium tree and integrates UniXNU. The board file, the ADT generator and
the build steps are in the `UniXNU-tb128fu` device repository.

## Why there are no vendor drivers

The TB128FU `abl` partition is a 32-bit ARM ELF that wraps one AArch64
`LinuxLoader` PE. Its inner firmware volume has a single `LinuxLoader` file;
clock, PMIC, UFS, display and touch are linked into that one image. The
`imagefv` firmware volume holds only the Lenovo/Motorola boot logos, and
`xbl`/`uefisecapp` carry no usable 64-bit DXE modules. Unlike fog/spes/tapas,
there is nothing to extract into `Binaries/tb128fu`.

The platform therefore follows sheng's `SHENG_VENDOR_HW = 0` path: the Mu
drivers only. `SimpleFbDxe` publishes the `cont_splash_region` that ABL leaves
scanning out; no vendor clock, PMIC, UFS, touch or button driver is present.

## Changes

* `Platforms/Lenovo/tb128fuPkg/tb128fu.dsc` — DivarPkg (SM6225), one stack at
  0x5ff90000, SimpleFbDxe 1200x2000, `PcdBuildXblHobs = FALSE` (the ABL leaves
  no stock DXE scheduler HOBs). It builds UniXNU, UniXNUDxe,
  `ShengBootMemoryDxe` and DarwinBoot, and maps UniXNULib and FdtLib.
* `tb128fu.fdf` — the Mu-only DXE set plus UniXNU and UniXNUDxe. No vendor raw
  files, no RamManagerDxe, no MassStorage.
* `Include/DXE.inc` — GIC, timer, console, FAT, `ShengBootMemoryDxe` (publishes
  the boot image's ramdisk as a read-only block device; every range comes from
  the device tree) and DarwinBoot (part of the firmware volume, because this
  build's image verification refuses a driver loaded from a volume).
* `SiliciumPkg/Library/MsBootOptionsLib` — registers the UniXNU application
  (`9B665204-A133-47A2-9396-9E2B370A4E25`) first. The default options name
  MsBootPolicy, UFP and MassStorage files that a Mu-only FV does not carry, so
  without this the BDS has no valid boot option. The observed
  `GetPlatformBootGraphic Status: NotFound` is the absent `BootLogo.bmp` and is
  not fatal.
* `Library/MemoryMapLib` — the TB128FU reserved-memory table from the live
  device tree and the free runs from `/proc/iomem`:
  * kernel 0x40000000+0x05700000;
  * HYP 0x45700000+0x00600000, XBL AOP 0x45e00000+0x00140000, boot info and
    SMEM, CDSP secure 0x46200000+0x01e00000;
  * PIL/modem/video/WLAN/CDSP/ADSP/IPA/GPU 0x4ab00000..0x55617000;
  * display 0x5c000000+0x00f00000 and DFPS 0x5cf00000+0x00100000;
  * the firmware window: DXE heap 0x5d000000+0x02800000, FD 0x5fc00000+0x00300000,
    SEC heap, page tables, stack, log and info block up to 0x60000000;
  * removed region 0x60000000+0x03900000 (no-map in the DT);
  * free DRAM 0x48000000+0x02b00000, 0x55617000+0x069e9000,
    0x63900000+0x19300000 and the second/third banks 0x80000000+0x140000000.
* `Library/ConfigurationMapLib` — the SM6225 numeric configuration of fogPkg;
  the board-specific entries agree with the live tree (SharedIMEMBaseAddr
  0x0c125000, TzAppsRegnAddr 0x61800000+0x02100000, DloadCookieAddr 0x003d3000).
* `Resources/Configs/tb128fu.toml` — FD base/size and the header v1 boot image.
* `Resources/DTBs/tb128fu.dtb` — the merged live Khaje IDP tree, with
  `/chosen/bootargs` and the stale initrd properties removed (the bootargs held
  the tablet serial numbers).

## Build

```sh
cd uefi-tb128fu
git submodule update --init --depth 1 -- Binaries Common/Mu Common/Mu_OEM_Sample Silicium-ACPI Silicon/Silicium/OpensslPkg/Library/OpensslLib/openssl

cd ../UniXNU
export UNIXNU_UEFI=$PWD/../uefi-tb128fu
./unixnu build firmware -D ../UniXNU-tb128fu
```

The result is `uefi-tb128fu/Build/tb128fuPkg/DEBUG_CLANGPDB/FV/SILICIUM_UEFI.fd`,
3 MiB. The preflight `scripts/check-tb128fu.py` of the device repository reads
this checkout and checks the memory map, the FD/stack placement and the splash
region.

## Unverified

* Whether ABL keeps the hx83102p panel scanning out after handing over to a
  custom boot image, so the Mu-only `SimpleFbDxe` and the UniXNU menu are
  visible.
* Whether ABL accepts a 192 MiB ramdisk in a header v1 image and places it at
  the requested 0x81000000 (or uses the stock 0x40000000 kernel window, which is
  too small and overlaps reservations).
* Whether the GIC spurious-interrupt patch is needed on a Mu-only build.
* Whether `ShengBootMemoryDxe`'s 1 GiB DT-hole top-up is relevant here, since
  this memory map already publishes the free DRAM banks.
* Whether the firmware FD fits the stock 3 MiB window with the full UniXNU
  package; the current build is 3 MiB by construction, and the compressed
  FvMain is about 0.73 MiB, so there is no pressure.
