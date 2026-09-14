/* Class KisPaintOp - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPaintOp @ 00336c80 ======

/* KisPaintOp::KisPaintOp(KisPainter*) */

void __thiscall KisPaintOp::KisPaintOp(KisPaintOp *this,KisPainter *param_1)

{
  undefined8 uVar1;
  undefined8 *puVar2;
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837ae8 + 0x10;
                    /* try { // try from 00336cad to 00336cb1 has its CatchHandler @ 00336cd9 */
  puVar2 = (undefined8 *)operator_new(0x28);
  uVar1 = DAT_007227c0;
  *puVar2 = this;
  puVar2[1] = 0;
  *(undefined *)(puVar2 + 3) = 0;
  puVar2[4] = uVar1;
  *(undefined8 **)(this + 0x18) = puVar2;
  puVar2[2] = param_1;
  return;
}



