/* Class KisAslStorage - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisAslStorage @ 002064d0 ======

void __thiscall KisAslStorage::KisAslStorage(KisAslStorage *this,QString *param_1)

{
  (*(code *)PTR_KisAslStorage_0083ad38)();
  return;
}



// ====== KisAslStorage @ 006cc580 ======

/* KisAslStorage::KisAslStorage(QString const&) */

void __thiscall KisAslStorage::KisAslStorage(KisAslStorage *this,QString *param_1)

{
  KisAslLayerStyleSerializer *this_00;
  undefined4 *puVar1;
  
  KisStoragePlugin::KisStoragePlugin((KisStoragePlugin *)this,(QString *)param_1);
  *(undefined **)this = PTR_vtable_00836bd0 + 0x10;
                    /* try { // try from 006cc5a3 to 006cc5a7 has its CatchHandler @ 006cc5e6 */
  this_00 = (KisAslLayerStyleSerializer *)operator_new(0x50);
                    /* try { // try from 006cc5ae to 006cc5b2 has its CatchHandler @ 006cc5f2 */
  KisAslLayerStyleSerializer::KisAslLayerStyleSerializer(this_00);
  *(KisAslLayerStyleSerializer **)(this + 0x10) = this_00;
                    /* try { // try from 006cc5bc to 006cc5c0 has its CatchHandler @ 006cc5e6 */
  puVar1 = (undefined4 *)operator_new(0x18);
  *(KisAslLayerStyleSerializer **)(puVar1 + 4) = this_00;
  *(code **)(puVar1 + 2) = FUN_006ccc60;
  puVar1[1] = 1;
  *puVar1 = 1;
  *(undefined4 **)(this + 0x18) = puVar1;
  return;
}



