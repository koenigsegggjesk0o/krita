/* Class KisStrokeStrategy - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisStrokeStrategy @ 00202320 ======

void __thiscall
KisStrokeStrategy::KisStrokeStrategy
          (KisStrokeStrategy *this,QLatin1String *param_1,KUndo2MagicString *param_2)

{
  (*(code *)PTR_KisStrokeStrategy_00838c60)();
  return;
}



// ====== KisStrokeStrategy @ 002081e0 ======

void __thiscall
KisStrokeStrategy::KisStrokeStrategy(KisStrokeStrategy *this,KisStrokeStrategy *param_1)

{
  (*(code *)PTR_KisStrokeStrategy_0083bbc0)();
  return;
}



// ====== KisStrokeStrategy @ 0020b910 ======

void __thiscall
KisStrokeStrategy::KisStrokeStrategy
          (KisStrokeStrategy *this,QLatin1String *param_1,KUndo2MagicString *param_2)

{
  (*(code *)PTR_KisStrokeStrategy_0083d758)();
  return;
}



// ====== KisStrokeStrategy @ 004eb380 ======

/* KisStrokeStrategy::KisStrokeStrategy(QLatin1String const&, KUndo2MagicString const&) */

void __thiscall
KisStrokeStrategy::KisStrokeStrategy
          (KisStrokeStrategy *this,QLatin1String *param_1,KUndo2MagicString *param_2)

{
  int *piVar1;
  undefined8 uVar2;
  undefined *puVar3;
  
  uVar2 = *(undefined8 *)(param_1 + 8);
  piVar1 = *(int **)param_2;
  puVar3 = PTR_vtable_00837ca0 + 0x10;
  *(undefined8 *)(this + 0x18) = *(undefined8 *)param_1;
  *(undefined8 *)(this + 0x20) = uVar2;
  *(undefined **)this = puVar3;
  uVar2 = DAT_00730f78;
  *(int **)(this + 0x28) = piVar1;
  *(undefined8 *)(this + 8) = uVar2;
  *(undefined8 *)(this + 0x10) = DAT_007227d0;
  if (1 < *piVar1 + 1U) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  *(undefined8 *)(this + 0x40) = 0;
  *(undefined (*) [16])(this + 0x30) = (undefined  [16])0x0;
  return;
}



// ====== KisStrokeStrategy @ 004eb5e0 ======

/* KisStrokeStrategy::KisStrokeStrategy(KisStrokeStrategy const&) */

void __thiscall
KisStrokeStrategy::KisStrokeStrategy(KisStrokeStrategy *this,KisStrokeStrategy *param_1)

{
  int *piVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined *puVar5;
  
  uVar3 = *(undefined8 *)(param_1 + 0x18);
  uVar4 = *(undefined8 *)(param_1 + 0x20);
  piVar1 = *(int **)(param_1 + 0x28);
  puVar5 = PTR_vtable_00837ca0 + 0x10;
  *(undefined8 *)(this + 0x10) = *(undefined8 *)(param_1 + 0x10);
  *(undefined **)this = puVar5;
  uVar2 = *(undefined8 *)(param_1 + 8);
  *(undefined8 *)(this + 0x18) = uVar3;
  *(undefined8 *)(this + 0x20) = uVar4;
  *(undefined8 *)(this + 8) = uVar2;
  *(int **)(this + 0x28) = piVar1;
  if (1 < *piVar1 + 1U) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  *(undefined8 *)(this + 0x40) = 0;
  *(undefined (*) [16])(this + 0x30) = (undefined  [16])0x0;
  if ((*(long *)(param_1 + 0x30) != 0) &&
     (((*(int *)(*(long *)(param_1 + 0x30) + 4) != 0 && (*(long *)(param_1 + 0x38) != 0)) ||
      (*(long *)(this + 0x40) != 0)))) {
                    /* try { // try from 004eb662 to 004eb666 has its CatchHandler @ 004eb686 */
    kis_assert_recoverable
              ("!rhs.m_strokeId && !m_mutatedJobsInterface && \"After the stroke has been started, no copying must happen\""
               ,"/builds/graphics/krita/libs/image/kis_stroke_strategy.cpp",0x2b);
  }
  return;
}



