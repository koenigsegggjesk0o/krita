/* Class KisInterstrokeDataTransactionWrapperFactory - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisInterstrokeDataTransactionWrapperFactory @ 0020ac40 ======

void __thiscall
KisInterstrokeDataTransactionWrapperFactory::KisInterstrokeDataTransactionWrapperFactory
          (KisInterstrokeDataTransactionWrapperFactory *this,KisInterstrokeDataFactory *param_1,
          bool param_2)

{
  (*(code *)PTR_KisInterstrokeDataTransactionWrapperFactory_0083d0f0)();
  return;
}



// ====== KisInterstrokeDataTransactionWrapperFactory @ 006019e0 ======

/* KisInterstrokeDataTransactionWrapperFactory::KisInterstrokeDataTransactionWrapperFactory(KisInterstrokeDataFactory*,
   bool) */

void __thiscall
KisInterstrokeDataTransactionWrapperFactory::KisInterstrokeDataTransactionWrapperFactory
          (KisInterstrokeDataTransactionWrapperFactory *this,KisInterstrokeDataFactory *param_1,
          bool param_2)

{
  undefined8 *puVar1;
  
  *(undefined **)this = PTR_vtable_00837970 + 0x10;
                    /* try { // try from 00601a03 to 00601a07 has its CatchHandler @ 00601a20 */
  puVar1 = (undefined8 *)operator_new(0x18);
  puVar1[1] = 0;
  *(undefined8 **)(this + 8) = puVar1;
  *puVar1 = param_1;
  *(bool *)(puVar1 + 2) = param_2;
  return;
}



