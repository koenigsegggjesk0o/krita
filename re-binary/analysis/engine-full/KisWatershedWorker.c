/* Class KisWatershedWorker - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisWatershedWorker @ 0020baf0 ======

void __thiscall
KisWatershedWorker::KisWatershedWorker
          (KisWatershedWorker *this,KisSharedPtr param_1,KisSharedPtr param_2,QRect *param_3,
          KoUpdater *param_4)

{
  (*(code *)PTR_KisWatershedWorker_0083d848)();
  return;
}



// ====== KisWatershedWorker @ 00442380 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisWatershedWorker::KisWatershedWorker(KisSharedPtr<KisPaintDevice>,
   KisSharedPtr<KisPaintDevice>, QRect const&, KoUpdater*) */

void __thiscall
KisWatershedWorker::KisWatershedWorker
          (KisWatershedWorker *this,KisSharedPtr param_1,KisSharedPtr param_2,QRect *param_3,
          KoUpdater *param_4)

{
  long lVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  undefined8 uVar4;
  undefined *puVar5;
  int iVar6;
  undefined (*pauVar7) [16];
  long *plVar8;
  KisPaintDevice *pKVar9;
  QString *pQVar10;
  KoColorSpace *pKVar11;
  undefined4 in_register_00000014;
  undefined4 in_register_00000034;
  long lVar12;
  long in_FS_OFFSET;
  undefined *local_40;
  undefined *local_38;
  long local_30;
  
  puVar5 = PTR_shared_null_008377d0;
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  pauVar7 = (undefined (*) [16])operator_new(0xa0);
  uVar4 = DAT_0072cfe8;
  uVar3 = DAT_00721778;
  uVar2 = _DAT_00721770;
  *(undefined8 *)pauVar7[3] = 0;
  *(undefined **)pauVar7[2] = puVar5;
  *(undefined **)(pauVar7[2] + 8) = puVar5;
  pKVar9 = *(KisPaintDevice **)CONCAT44(in_register_00000034,param_1);
  *(undefined8 *)pauVar7[4] = 0;
  *(undefined8 *)(pauVar7[4] + 8) = 0;
  *(undefined8 *)pauVar7[5] = 0;
  *(undefined8 *)(pauVar7[7] + 8) = uVar4;
  pauVar7[8][0] = 0;
  *(undefined8 *)(pauVar7[9] + 8) = 0;
  *(undefined (**) [16])this = pauVar7;
  *pauVar7 = (undefined  [16])0x0;
  *(undefined8 *)pauVar7[1] = uVar2;
  *(undefined8 *)(pauVar7[1] + 8) = uVar3;
  *(undefined **)(pauVar7[5] + 8) = pauVar7[5] + 8;
  *(undefined **)pauVar7[6] = pauVar7[5] + 8;
  *(undefined (*) [16])(pauVar7[6] + 8) = (undefined  [16])0x0;
  *(undefined (*) [16])(pauVar7[8] + 8) = (undefined  [16])0x0;
                    /* try { // try from 00442445 to 00442500 has its CatchHandler @ 00442596 */
  plVar8 = (long *)KisPaintDevice::colorSpace(pKVar9);
  iVar6 = (**(code **)(*plVar8 + 0x30))(plVar8);
  if (iVar6 == 1) {
    plVar8 = *(long **)this;
    lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
    lVar12 = *plVar8;
    plVar8[0x13] = (long)param_4;
    if (lVar1 != lVar12) {
      if (lVar1 != 0) {
        LOCK();
        *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
        UNLOCK();
        lVar12 = *plVar8;
      }
      *plVar8 = lVar1;
      FUN_00439f40(lVar12);
      plVar8 = *(long **)this;
    }
    lVar1 = *(long *)CONCAT44(in_register_00000014,param_2);
    lVar12 = plVar8[1];
    if (lVar1 != lVar12) {
      if (lVar1 != 0) {
        LOCK();
        *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
        UNLOCK();
        lVar12 = plVar8[1];
      }
      plVar8[1] = lVar1;
      FUN_00439f40(lVar12);
      plVar8 = *(long **)this;
    }
    lVar1 = *(long *)(param_3 + 8);
    plVar8[2] = *(long *)param_3;
    plVar8[3] = lVar1;
    pKVar9 = (KisPaintDevice *)operator_new(0x28);
    local_38 = PTR_shared_null_008377d0;
                    /* try { // try from 00442510 to 00442514 has its CatchHandler @ 00442585 */
    pQVar10 = (QString *)KoColorSpaceRegistry::instance();
    local_40 = PTR_shared_null_008377d0;
                    /* try { // try from 00442531 to 00442543 has its CatchHandler @ 00442579 */
    pKVar11 = (KoColorSpace *)KoColorSpaceRegistry::rgb8(pQVar10);
    KisPaintDevice::KisPaintDevice(pKVar9,pKVar11,(QString *)&local_38);
    lVar1 = *(long *)this;
    if (pKVar9 != *(KisPaintDevice **)(lVar1 + 0x30)) {
      LOCK();
      *(int *)(pKVar9 + 0x10) = *(int *)(pKVar9 + 0x10) + 1;
      UNLOCK();
      uVar2 = *(undefined8 *)(lVar1 + 0x30);
      *(KisPaintDevice **)(lVar1 + 0x30) = pKVar9;
      FUN_00439f40(uVar2);
    }
    FUN_002dd9a0(&local_40);
    FUN_002dd9a0(&local_38);
  }
  else {
    kis_safe_assert_recoverable
              ("heightMap->colorSpace()->pixelSize() == 1",
               "/builds/graphics/krita/libs/image/lazybrush/KisWatershedWorker.cpp",0xfd);
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



