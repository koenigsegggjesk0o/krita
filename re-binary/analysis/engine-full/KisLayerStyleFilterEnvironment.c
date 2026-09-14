/* Class KisLayerStyleFilterEnvironment - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLayerStyleFilterEnvironment @ 002086d0 ======

void __thiscall
KisLayerStyleFilterEnvironment::KisLayerStyleFilterEnvironment
          (KisLayerStyleFilterEnvironment *this,KisLayer *param_1)

{
  (*(code *)PTR_KisLayerStyleFilterEnvironment_0083be38)();
  return;
}



// ====== KisLayerStyleFilterEnvironment @ 006719d0 ======

/* KisLayerStyleFilterEnvironment::KisLayerStyleFilterEnvironment(KisLayer*) */

void __thiscall
KisLayerStyleFilterEnvironment::KisLayerStyleFilterEnvironment
          (KisLayerStyleFilterEnvironment *this,KisLayer *param_1)

{
  undefined8 *puVar1;
  
  puVar1 = (undefined8 *)operator_new(0x50);
  puVar1[3] = 0;
  puVar1[4] = 0;
  puVar1[7] = 0;
  *(undefined (*) [16])(puVar1 + 1) = (undefined  [16])0x0;
  *(undefined (*) [16])(puVar1 + 5) = (undefined  [16])0x0;
                    /* try { // try from 00671a13 to 00671a17 has its CatchHandler @ 00671a24 */
  KisLocalStrokeResources::KisLocalStrokeResources((KisLocalStrokeResources *)(puVar1 + 8));
  *(undefined8 **)this = puVar1;
  *puVar1 = param_1;
  return;
}



