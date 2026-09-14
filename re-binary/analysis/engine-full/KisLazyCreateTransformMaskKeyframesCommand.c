/* Class KisLazyCreateTransformMaskKeyframesCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLazyCreateTransformMaskKeyframesCommand @ 00376b80 ======

/* KisLazyCreateTransformMaskKeyframesCommand::KisLazyCreateTransformMaskKeyframesCommand(KisSharedPtr<KisTransformMask>,
   KUndo2Command*) */

void __thiscall
KisLazyCreateTransformMaskKeyframesCommand::KisLazyCreateTransformMaskKeyframesCommand
          (KisLazyCreateTransformMaskKeyframesCommand *this,KisSharedPtr param_1,
          KUndo2Command *param_2)

{
  long lVar1;
  undefined4 in_register_00000034;
  
  KisCommandUtils::AggregateCommand::AggregateCommand((AggregateCommand *)this,param_2);
  *(undefined **)this = PTR_vtable_00836ef0 + 0x10;
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x48) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
  return;
}



