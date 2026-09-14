/* Class KisStrokeStrategyUndoCommandBased - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisStrokeStrategyUndoCommandBased @ 00206ac0 ======

void __thiscall
KisStrokeStrategyUndoCommandBased::KisStrokeStrategyUndoCommandBased
          (KisStrokeStrategyUndoCommandBased *this,KUndo2MagicString *param_1,bool param_2,
          KisStrokeUndoFacade *param_3,QSharedPointer param_4,QSharedPointer param_5)

{
  (*(code *)PTR_KisStrokeStrategyUndoCommandBased_0083b030)();
  return;
}



// ====== KisStrokeStrategyUndoCommandBased @ 0020d700 ======

void __thiscall
KisStrokeStrategyUndoCommandBased::KisStrokeStrategyUndoCommandBased
          (KisStrokeStrategyUndoCommandBased *this,KUndo2MagicString *param_1,bool param_2,
          KisStrokeUndoFacade *param_3,QSharedPointer param_4,QSharedPointer param_5)

{
  (*(code *)PTR_KisStrokeStrategyUndoCommandBased_0083e650)();
  return;
}



// ====== KisStrokeStrategyUndoCommandBased @ 004e85e0 ======

/* KisStrokeStrategyUndoCommandBased::KisStrokeStrategyUndoCommandBased(KUndo2MagicString const&,
   bool, KisStrokeUndoFacade*, QSharedPointer<KUndo2Command>, QSharedPointer<KUndo2Command>) */

void __thiscall
KisStrokeStrategyUndoCommandBased::KisStrokeStrategyUndoCommandBased
          (KisStrokeStrategyUndoCommandBased *this,KUndo2MagicString *param_1,bool param_2,
          KisStrokeUndoFacade *param_3,QSharedPointer param_4,QSharedPointer param_5)

{
  int *piVar1;
  undefined8 uVar2;
  undefined *puVar3;
  undefined4 in_register_00000084;
  undefined4 in_register_0000008c;
  long in_FS_OFFSET;
  undefined4 local_48 [2];
  char *local_40;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  local_48[0] = 0x19;
  local_40 = "STROKE_UNDO_COMMAND_BASED";
  KisRunnableBasedStrokeStrategy::KisRunnableBasedStrokeStrategy
            ((KisRunnableBasedStrokeStrategy *)this,(QLatin1String *)local_48,param_1);
  puVar3 = PTR_vtable_00836f00;
  this[0x68] = (KisStrokeStrategyUndoCommandBased)param_2;
  *(undefined **)this = puVar3 + 0x10;
  uVar2 = ((undefined8 *)CONCAT44(in_register_00000084,param_4))[1];
  *(undefined8 *)(this + 0x70) = *(undefined8 *)CONCAT44(in_register_00000084,param_4);
  *(undefined8 *)(this + 0x78) = uVar2;
  piVar1 = *(int **)(this + 0x78);
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
    LOCK();
    *(int *)(*(long *)(this + 0x78) + 4) = *(int *)(*(long *)(this + 0x78) + 4) + 1;
    UNLOCK();
  }
  uVar2 = ((undefined8 *)CONCAT44(in_register_0000008c,param_5))[1];
  *(undefined8 *)(this + 0x80) = *(undefined8 *)CONCAT44(in_register_0000008c,param_5);
  *(undefined8 *)(this + 0x88) = uVar2;
  piVar1 = *(int **)(this + 0x88);
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
    LOCK();
    *(int *)(*(long *)(this + 0x88) + 4) = *(int *)(*(long *)(this + 0x88) + 4) + 1;
    UNLOCK();
  }
  *(KisStrokeUndoFacade **)(this + 0x90) = param_3;
  *(undefined (*) [16])(this + 0xa8) = (undefined  [16])0x0;
  *(undefined8 *)(this + 0x98) = 0;
  *(undefined4 *)(this + 0xa0) = 0xffffffff;
                    /* try { // try from 004e86c4 to 004e8716 has its CatchHandler @ 004e8739 */
  KisSimpleStrokeStrategy::enableJob((KisSimpleStrokeStrategy *)this,0,true,1,0);
  KisSimpleStrokeStrategy::enableJob((KisSimpleStrokeStrategy *)this,2,true,1,0);
  KisSimpleStrokeStrategy::enableJob((KisSimpleStrokeStrategy *)this,1,true,1,0);
  KisSimpleStrokeStrategy::enableJob((KisSimpleStrokeStrategy *)this,3,true,1,0);
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisStrokeStrategyUndoCommandBased @ 004e8750 ======

/* KisStrokeStrategyUndoCommandBased::KisStrokeStrategyUndoCommandBased(KisStrokeStrategyUndoCommandBased
   const&) */

void __thiscall
KisStrokeStrategyUndoCommandBased::KisStrokeStrategyUndoCommandBased
          (KisStrokeStrategyUndoCommandBased *this,KisStrokeStrategyUndoCommandBased *param_1)

{
  int *piVar1;
  undefined8 uVar2;
  long lVar3;
  undefined *puVar4;
  
  KisRunnableBasedStrokeStrategy::KisRunnableBasedStrokeStrategy
            ((KisRunnableBasedStrokeStrategy *)this,(KisRunnableBasedStrokeStrategy *)param_1);
  puVar4 = PTR_vtable_00836f00;
  this[0x68] = (KisStrokeStrategyUndoCommandBased)0x0;
  *(undefined **)this = puVar4 + 0x10;
  uVar2 = *(undefined8 *)(param_1 + 0x78);
  piVar1 = *(int **)(param_1 + 0x78);
  *(undefined8 *)(this + 0x70) = *(undefined8 *)(param_1 + 0x70);
  *(undefined8 *)(this + 0x78) = uVar2;
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
    LOCK();
    *(int *)(*(long *)(this + 0x78) + 4) = *(int *)(*(long *)(this + 0x78) + 4) + 1;
    UNLOCK();
  }
  uVar2 = *(undefined8 *)(param_1 + 0x88);
  piVar1 = *(int **)(param_1 + 0x88);
  *(undefined8 *)(this + 0x80) = *(undefined8 *)(param_1 + 0x80);
  *(undefined8 *)(this + 0x88) = uVar2;
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
    LOCK();
    *(int *)(*(long *)(this + 0x88) + 4) = *(int *)(*(long *)(this + 0x88) + 4) + 1;
    UNLOCK();
  }
  uVar2 = *(undefined8 *)(param_1 + 0x90);
  *(undefined8 *)(this + 0x98) = 0;
  *(undefined (*) [16])(this + 0xa8) = (undefined  [16])0x0;
  lVar3 = *(long *)(param_1 + 0xb0);
  *(undefined8 *)(this + 0x90) = uVar2;
  if ((lVar3 == 0) && (param_1[0x68] == (KisStrokeStrategyUndoCommandBased)0x0)) {
    return;
  }
                    /* try { // try from 004e8802 to 004e8806 has its CatchHandler @ 004e881d */
  kis_assert_recoverable
            ("!rhs.m_macroCommand && !rhs.m_undo && \"After the stroke has been started, no copying must happen\""
             ,"/builds/graphics/krita/libs/image/kis_stroke_strategy_undo_command_based.cpp",0x31);
  return;
}



