/* Class KisSelectionBasedProcessingHelper - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSelectionBasedProcessingHelper @ 0020b400 ======

void __thiscall
KisSelectionBasedProcessingHelper::KisSelectionBasedProcessingHelper
          (KisSelectionBasedProcessingHelper *this,KisSharedPtr param_1,function param_2)

{
  (*(code *)PTR_KisSelectionBasedProcessingHelper_0083d4d0)();
  return;
}



// ====== KisSelectionBasedProcessingHelper @ 0037e550 ======

/* KisSelectionBasedProcessingHelper::KisSelectionBasedProcessingHelper(KisSharedPtr<KisSelection>,
   std::function<void (KisSharedPtr<KisPaintDevice>)>) */

void __thiscall
KisSelectionBasedProcessingHelper::KisSelectionBasedProcessingHelper
          (KisSelectionBasedProcessingHelper *this,KisSharedPtr param_1,function param_2)

{
  long lVar1;
  code *pcVar2;
  undefined8 uVar3;
  undefined4 in_register_00000014;
  long lVar4;
  undefined4 in_register_00000034;
  
  lVar4 = CONCAT44(in_register_00000014,param_2);
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)this = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 8) = *(int *)(lVar1 + 8) + 1;
    UNLOCK();
  }
  pcVar2 = *(code **)(lVar4 + 0x10);
  *(undefined8 *)(this + 8) = 0;
  *(undefined8 *)(this + 0x20) = 0;
  *(undefined8 *)(this + 0x28) = 0;
  *(undefined (*) [16])(this + 0x10) = (undefined  [16])0x0;
  if (pcVar2 != (code *)0x0) {
                    /* try { // try from 0037e5a6 to 0037e5a7 has its CatchHandler @ 0037e5b6 */
    (*pcVar2)(this + 0x10,lVar4,2);
    uVar3 = *(undefined8 *)(lVar4 + 0x18);
    *(undefined8 *)(this + 0x20) = *(undefined8 *)(lVar4 + 0x10);
    *(undefined8 *)(this + 0x28) = uVar3;
  }
  return;
}



