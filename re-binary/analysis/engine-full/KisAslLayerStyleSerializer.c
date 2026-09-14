/* Class KisAslLayerStyleSerializer - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisAslLayerStyleSerializer @ 00209330 ======

void __thiscall
KisAslLayerStyleSerializer::KisAslLayerStyleSerializer(KisAslLayerStyleSerializer *this)

{
  (*(code *)PTR_KisAslLayerStyleSerializer_0083c468)();
  return;
}



// ====== KisAslLayerStyleSerializer @ 006b2d90 ======

/* KisAslLayerStyleSerializer::KisAslLayerStyleSerializer() */

void __thiscall
KisAslLayerStyleSerializer::KisAslLayerStyleSerializer(KisAslLayerStyleSerializer *this)

{
  undefined *puVar1;
  KisLocalStrokeResources *this_00;
  undefined4 *puVar2;
  
  puVar1 = PTR_shared_null_00836c40;
  *(undefined **)this = PTR_shared_null_00836c40;
                    /* try { // try from 006b2db2 to 006b2db6 has its CatchHandler @ 006b2e22 */
  KisAslCallbackObjectCatcher::KisAslCallbackObjectCatcher
            ((KisAslCallbackObjectCatcher *)(this + 8));
  *(undefined **)(this + 0x30) = puVar1;
  puVar1 = PTR_shared_null_008377d0;
  *(undefined2 *)(this + 0x38) = 0x100;
  *(undefined **)(this + 0x20) = puVar1;
  *(undefined **)(this + 0x28) = puVar1;
                    /* try { // try from 006b2dd9 to 006b2ddd has its CatchHandler @ 006b2e3a */
  this_00 = (KisLocalStrokeResources *)operator_new(0x10);
                    /* try { // try from 006b2de4 to 006b2de8 has its CatchHandler @ 006b2e2e */
  KisLocalStrokeResources::KisLocalStrokeResources(this_00);
  *(KisLocalStrokeResources **)(this + 0x40) = this_00;
                    /* try { // try from 006b2df2 to 006b2df6 has its CatchHandler @ 006b2e3a */
  puVar2 = (undefined4 *)operator_new(0x18);
  *(KisLocalStrokeResources **)(puVar2 + 4) = this_00;
  *(code **)(puVar2 + 2) = FUN_003407b0;
  puVar2[1] = 1;
  *puVar2 = 1;
  *(undefined4 **)(this + 0x48) = puVar2;
  return;
}



