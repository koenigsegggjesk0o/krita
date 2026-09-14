/* Class KisRunnableBasedStrokeStrategy - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisRunnableBasedStrokeStrategy @ 00203190 ======

void __thiscall
KisRunnableBasedStrokeStrategy::KisRunnableBasedStrokeStrategy
          (KisRunnableBasedStrokeStrategy *this,QLatin1String *param_1,KUndo2MagicString *param_2)

{
  (*(code *)PTR_KisRunnableBasedStrokeStrategy_00839398)();
  return;
}



// ====== KisRunnableBasedStrokeStrategy @ 0020cd40 ======

void __thiscall
KisRunnableBasedStrokeStrategy::KisRunnableBasedStrokeStrategy
          (KisRunnableBasedStrokeStrategy *this,KisRunnableBasedStrokeStrategy *param_1)

{
  (*(code *)PTR_KisRunnableBasedStrokeStrategy_0083e170)();
  return;
}



// ====== KisRunnableBasedStrokeStrategy @ 004ea590 ======

/* KisRunnableBasedStrokeStrategy::KisRunnableBasedStrokeStrategy(QLatin1String const&,
   KUndo2MagicString const&) */

void __thiscall
KisRunnableBasedStrokeStrategy::KisRunnableBasedStrokeStrategy
          (KisRunnableBasedStrokeStrategy *this,QLatin1String *param_1,KUndo2MagicString *param_2)

{
  undefined *puVar1;
  undefined8 *puVar2;
  
  KisSimpleStrokeStrategy::KisSimpleStrokeStrategy((KisSimpleStrokeStrategy *)this,param_1,param_2);
  *(undefined **)this = PTR_vtable_00837c60 + 0x10;
                    /* try { // try from 004ea5b5 to 004ea5b9 has its CatchHandler @ 004ea5d7 */
  puVar2 = (undefined8 *)operator_new(0x10);
  puVar1 = PTR_vtable_00836bf0;
  puVar2[1] = this;
  *(undefined8 **)(this + 0x60) = puVar2;
  *puVar2 = puVar1 + 0x10;
  return;
}



// ====== KisRunnableBasedStrokeStrategy @ 004ea5f0 ======

/* KisRunnableBasedStrokeStrategy::KisRunnableBasedStrokeStrategy(KisRunnableBasedStrokeStrategy
   const&) */

void __thiscall
KisRunnableBasedStrokeStrategy::KisRunnableBasedStrokeStrategy
          (KisRunnableBasedStrokeStrategy *this,KisRunnableBasedStrokeStrategy *param_1)

{
  undefined *puVar1;
  undefined8 *puVar2;
  
  KisSimpleStrokeStrategy::KisSimpleStrokeStrategy
            ((KisSimpleStrokeStrategy *)this,(KisSimpleStrokeStrategy *)param_1);
  *(undefined **)this = PTR_vtable_00837c60 + 0x10;
                    /* try { // try from 004ea615 to 004ea619 has its CatchHandler @ 004ea637 */
  puVar2 = (undefined8 *)operator_new(0x10);
  puVar1 = PTR_vtable_00836bf0;
  puVar2[1] = this;
  *(undefined8 **)(this + 0x60) = puVar2;
  *puVar2 = puVar1 + 0x10;
  return;
}



