# Kernel Size & Bootloader Sector Calculations

This document explains the relationship between the kernel binary size, the number
of disk sectors the bootloader must load, and the minimum OS image size. Getting
these numbers wrong causes silent boot failures (black screen or "Disk read failed").

---

## Key Numbers

| Value | Formula | Current |
|---|---|---|
| Sector size | fixed by x86 BIOS | 512 bytes |
| Kernel binary size | `wc -c build/kernel.bin` | 4196 bytes |
| Minimum sectors to load | `ceil(kernel_bytes / 512)` | `ceil(4196 / 512)` = **9** |
| Sectors loaded (with headroom) | chosen value in `boot.asm` | **16** |
| Bytes loaded by bootloader | `sectors × 512` | `16 × 512` = 8192 bytes |
| Minimum OS image size | `(1 + sectors_loaded) × 512` | `(1 + 16) × 512` = **8704 bytes** |

The `+1` in the image size accounts for the bootloader itself occupying sector 1.

---

## Where the Numbers Live in Code

### `boot/boot.asm` — sectors to load
```asm
mov al, 16   ; how many 512-byte sectors to read from disk into memory
```
- This must satisfy: `al >= ceil(kernel_binary_bytes / 512)`
- The kernel is loaded to address `0x1000`, so the highest byte loaded lands at:
  `0x1000 + (al × 512) - 1`

### `Makefile` — minimum OS image padding
```makefile
if [ $$SIZE -lt 8704 ]; then
    dd if=/dev/zero bs=1 count=$$((8704 - $$SIZE)) >> $(OS_IMAGE)
fi
```
- Must satisfy: `padding_threshold >= (1 + sectors_in_boot_asm) × 512`
- If the image is smaller than what the bootloader tries to read, the BIOS
  `int 0x13` call sets the carry flag and the bootloader prints
  `"ERROR: Disk read failed!"`.

---

## ELF Section Layout

The kernel ELF is linked with `-Ttext 0x1000`. Sections are placed in this order:

```
Address     Section    Description
---------   --------   ------------------------------------------
0x1000      .text      Executable code (entry, ISR stubs, C code)
0x????      .rodata    Read-only data: string literals, etc.
0x????      .bss       Zero-initialized globals (NOT in binary file)
```

All of `.text` and `.rodata` must land below `0x1000 + (sectors × 512)`.

Check the current layout at any time:
```bash
i686-elf-objdump -h build/kernel.elf
```

---

## How to Update When the Kernel Grows

1. Build the kernel and measure the binary:
   ```bash
   make kernel
   wc -c build/kernel.bin
   ```

2. Calculate the minimum sectors:
   ```
   sectors_needed = ceil(binary_bytes / 512)
   ```

3. Choose a load count with headroom (e.g. `sectors_needed + 4`), update `boot.asm`:
   ```asm
   mov al, <new_value>
   ```

4. Update the Makefile padding threshold:
   ```
   new_threshold = (1 + new_sector_count) × 512
   ```
   ```makefile
   if [ $$SIZE -lt <new_threshold> ]; then
   ```

---

## What Goes Wrong When These Are Mismatched

| Mismatch | Symptom |
|---|---|
| `sectors_loaded < ceil(kernel_bytes / 512)` | Tail of `.text` or `.rodata` is not in memory. Executing unloaded code runs zeros (`0x00 0x00` = `add %al,(%eax)`), which dereferences address 0 → triple fault → **black screen immediately on boot** |
| `image_size < (1 + sectors_loaded) × 512` | BIOS `int 0x13` tries to read past end of disk → carry flag set → **"ERROR: Disk read failed!"** |
| `.bss` section beyond loaded range | Zero-initialized globals are at unloaded addresses. Works in QEMU (memory is zeroed) but may silently corrupt state on real hardware |

---

## Real Example: The `<` and `>` Bug

Adding `case scanComma: c = '<';` and `case scanDot: c = '>';` to `keyboard.c`
grew `.text` from `0xFDF` bytes to `0x1017` bytes — crossing the 4096-byte
(8-sector) boundary that was previously configured.

```
Before:  .text ends at 0x1FDE  →  all within 8 sectors (0x1000–0x1FFF) ✓
After:   .text ends at 0x2017  →  last 24 bytes (0x2000–0x2017) NOT loaded ✗
```

The CPU executed `0x00 0x00` bytes at `0x2000` instead of real instructions,
causing an immediate triple fault before any output appeared.

**Fix:** increased `boot.asm` from `mov al, 8` to `mov al, 16` and the Makefile
padding from `5120` to `8704`.
