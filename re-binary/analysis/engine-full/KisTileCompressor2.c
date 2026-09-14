/* Class KisTileCompressor2 - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisTileCompressor2 @ 0020a2e0 ======

void __thiscall KisTileCompressor2::KisTileCompressor2(KisTileCompressor2 *this)

{
  (*(code *)PTR_KisTileCompressor2_0083cc40)();
  return;
}



// ====== KisTileCompressor2 @ 00305960 ======

/* KisTileCompressor2::KisTileCompressor2() */

void __thiscall KisTileCompressor2::KisTileCompressor2(KisTileCompressor2 *this)

{
  undefined *puVar1;
  undefined *puVar2;
  KisLzfCompression *this_00;
  
  puVar1 = PTR_shared_null_008377d0;
  KisAbstractTileCompressor::KisAbstractTileCompressor((KisAbstractTileCompressor *)this);
  puVar2 = PTR_vtable_00837e58 + 0x10;
  *(undefined **)(this + 0x18) = puVar1;
  *(undefined **)(this + 0x20) = puVar1;
  *(undefined **)this = puVar2;
  *(undefined **)(this + 0x28) = PTR_shared_null_008377d0;
                    /* try { // try from 003059ab to 003059af has its CatchHandler @ 003059c8 */
  this_00 = (KisLzfCompression *)operator_new(8);
                    /* try { // try from 003059b6 to 003059ba has its CatchHandler @ 003059d4 */
  KisLzfCompression::KisLzfCompression(this_00);
  *(KisLzfCompression **)(this + 0x30) = this_00;
  return;
}



