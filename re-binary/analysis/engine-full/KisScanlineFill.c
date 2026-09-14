/* Class KisScanlineFill - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisScanlineFill @ 00201490 ======

void __thiscall
KisScanlineFill::KisScanlineFill
          (KisScanlineFill *this,KisSharedPtr param_1,QPoint *param_2,QRect *param_3)

{
  (*(code *)PTR_KisScanlineFill_00838518)();
  return;
}



// ====== KisScanlineFill @ 003acab0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisScanlineFill::KisScanlineFill(KisSharedPtr<KisPaintDevice>, QPoint const&, QRect const&) */

void __thiscall
KisScanlineFill::KisScanlineFill
          (KisScanlineFill *this,KisSharedPtr param_1,QPoint *param_2,QRect *param_3)

{
  long *plVar1;
  long lVar2;
  long lVar3;
  long lVar4;
  undefined *puVar5;
  long *plVar6;
  undefined4 in_register_00000034;
  long *plVar7;
  
  plVar6 = (long *)operator_new(0x88);
  *plVar6 = 0;
  plVar6[3] = -1;
  *(undefined (*) [16])(plVar6 + 1) = (undefined  [16])0x0;
                    /* try { // try from 003acaf0 to 003acaf4 has its CatchHandler @ 003acb98 */
  KisFillIntervalMap::KisFillIntervalMap((KisFillIntervalMap *)(plVar6 + 6));
  puVar5 = PTR_shared_null_008377d0;
  plVar7 = (long *)*plVar6;
  plVar6[9] = 0;
  lVar3 = DAT_00721778;
  lVar2 = _DAT_00721770;
  plVar6[0xe] = 0;
  plVar6[7] = (long)puVar5;
  plVar1 = *(long **)CONCAT44(in_register_00000034,param_1);
  plVar6[10] = lVar2;
  plVar6[0xb] = lVar3;
  plVar6[0x10] = 0;
  *(long **)this = plVar6;
  *(undefined (*) [16])(plVar6 + 0xc) = (undefined  [16])0x0;
  if (plVar1 != plVar7) {
    if (plVar1 != (long *)0x0) {
      LOCK();
      *(int *)(plVar1 + 2) = *(int *)(plVar1 + 2) + 1;
      UNLOCK();
      plVar7 = (long *)*plVar6;
    }
    *plVar6 = (long)plVar1;
    if (plVar7 != (long *)0x0) {
      LOCK();
      plVar1 = plVar7 + 2;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar7 + 0x20))();
      }
    }
    plVar6 = *(long **)this;
  }
  lVar2 = *(long *)param_2;
  lVar3 = *(long *)param_3;
  lVar4 = *(long *)(param_3 + 8);
  *(undefined4 *)(plVar6 + 5) = 1;
  plVar6[4] = 0;
  plVar6[1] = lVar2;
  *(undefined4 *)(plVar6 + 8) = 0;
  plVar6[2] = lVar3;
  plVar6[3] = lVar4;
  return;
}



