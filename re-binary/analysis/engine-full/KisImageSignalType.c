/* Class KisImageSignalType - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisImageSignalType @ 00205220 ======

void __thiscall
KisImageSignalType::KisImageSignalType(KisImageSignalType *this,KisImageSignalTypeEnum param_1)

{
  (*(code *)PTR_KisImageSignalType_0083a3e0)();
  return;
}



// ====== KisImageSignalType @ 00205610 ======

void __thiscall KisImageSignalType::KisImageSignalType(void)

{
  (*(code *)PTR_KisImageSignalType_0083a5d8)();
  return;
}



// ====== KisImageSignalType @ 00207590 ======

void __thiscall KisImageSignalType::KisImageSignalType(void)

{
  (*(code *)PTR_KisImageSignalType_0083b598)();
  return;
}



// ====== KisImageSignalType @ 0020bdd0 ======

void __thiscall KisImageSignalType::KisImageSignalType(KisImageSignalType *this)

{
  (*(code *)PTR_KisImageSignalType_0083d9b8)();
  return;
}



// ====== KisImageSignalType @ 00529420 ======

/* KisImageSignalType::KisImageSignalType() */

void __thiscall KisImageSignalType::KisImageSignalType(KisImageSignalType *this)

{
  ComplexSizeChangedSignal::ComplexSizeChangedSignal((ComplexSizeChangedSignal *)(this + 8));
  ComplexNodeReselectionSignal::ComplexNodeReselectionSignal
            ((ComplexNodeReselectionSignal *)(this + 0x28));
  return;
}



// ====== KisImageSignalType @ 00529440 ======

/* KisImageSignalType::KisImageSignalType(KisImageSignalTypeEnum) */

void __thiscall
KisImageSignalType::KisImageSignalType(KisImageSignalType *this,KisImageSignalTypeEnum param_1)

{
  *(KisImageSignalTypeEnum *)this = param_1;
  ComplexSizeChangedSignal::ComplexSizeChangedSignal((ComplexSizeChangedSignal *)(this + 8));
  ComplexNodeReselectionSignal::ComplexNodeReselectionSignal
            ((ComplexNodeReselectionSignal *)(this + 0x28));
  return;
}



// ====== KisImageSignalType @ 00529460 ======

/* KisImageSignalType::KisImageSignalType(ComplexSizeChangedSignal) */

void __thiscall KisImageSignalType::KisImageSignalType(void *this)

{
  undefined8 param_11;
  undefined8 in_stack_00000010;
  undefined8 param_12;
  undefined8 in_stack_00000020;
  
  *(undefined4 *)this = 2;
  *(undefined8 *)((long)this + 8) = param_11;
  *(undefined8 *)((long)this + 0x10) = in_stack_00000010;
  *(undefined8 *)((long)this + 0x18) = param_12;
  *(undefined8 *)((long)this + 0x20) = in_stack_00000020;
  ComplexNodeReselectionSignal::ComplexNodeReselectionSignal
            ((ComplexNodeReselectionSignal *)((long)this + 0x28));
  return;
}



// ====== KisImageSignalType @ 005297c0 ======

/* KisImageSignalType::KisImageSignalType(ComplexNodeReselectionSignal) */

void __thiscall KisImageSignalType::KisImageSignalType(KisImageSignalType *this,long *param_2)

{
  long lVar1;
  
  *(undefined4 *)this = 6;
  ComplexSizeChangedSignal::ComplexSizeChangedSignal((ComplexSizeChangedSignal *)(this + 8));
  lVar1 = *param_2;
  *(long *)(this + 0x28) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
                    /* try { // try from 005297fb to 005297ff has its CatchHandler @ 00529824 */
  FUN_002de570(this + 0x30,param_2 + 1);
  lVar1 = param_2[2];
  *(long *)(this + 0x38) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
                    /* try { // try from 0052981a to 0052981e has its CatchHandler @ 00529830 */
  FUN_002de570(this + 0x40,param_2 + 3);
  return;
}



