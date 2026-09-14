/* Class KisNodeQueryPath - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNodeQueryPath @ 0020dea0 ======

void __thiscall KisNodeQueryPath::KisNodeQueryPath(KisNodeQueryPath *this)

{
  (*(code *)PTR_KisNodeQueryPath_0083ea20)();
  return;
}



// ====== KisNodeQueryPath @ 00696660 ======

/* KisNodeQueryPath::KisNodeQueryPath() */

void __thiscall KisNodeQueryPath::KisNodeQueryPath(KisNodeQueryPath *this)

{
  undefined *puVar1;
  undefined8 *puVar2;
  
  puVar2 = (undefined8 *)operator_new(0x10);
  puVar1 = PTR_shared_null_00837830;
  *(undefined8 **)this = puVar2;
  *puVar2 = puVar1;
  return;
}



// ====== KisNodeQueryPath @ 00696750 ======

/* KisNodeQueryPath::KisNodeQueryPath(KisNodeQueryPath const&) */

void __thiscall KisNodeQueryPath::KisNodeQueryPath(KisNodeQueryPath *this,KisNodeQueryPath *param_1)

{
  undefined uVar1;
  long *plVar2;
  int *piVar3;
  long lVar4;
  undefined8 *puVar5;
  long *plVar6;
  long lVar7;
  undefined8 *puVar8;
  long lVar9;
  undefined8 *puVar10;
  undefined8 *puVar11;
  undefined8 *puVar12;
  
  plVar6 = (long *)operator_new(0x10);
  plVar2 = *(long **)param_1;
  piVar3 = (int *)*plVar2;
  *plVar6 = (long)piVar3;
  if (*piVar3 == 0) {
                    /* try { // try from 006967c6 to 006967ca has its CatchHandler @ 00696832 */
    QListData::detach((int)plVar6);
    puVar12 = (undefined8 *)(*plVar2 + 0x10 + (long)*(int *)(*plVar2 + 8) * 8);
    lVar4 = *plVar6;
    lVar7 = (long)*(int *)(lVar4 + 8) * 8;
    lVar9 = (long)*(int *)(lVar4 + 0xc) * 8;
    puVar10 = (undefined8 *)(lVar4 + 0x10 + lVar7);
    if (lVar9 != lVar7) {
      do {
                    /* try { // try from 0069680d to 00696811 has its CatchHandler @ 0069683e */
        puVar8 = (undefined8 *)operator_new(8);
        puVar5 = (undefined8 *)*puVar12;
        puVar11 = puVar10 + 1;
        puVar12 = puVar12 + 1;
        *puVar8 = *puVar5;
        *puVar10 = puVar8;
        puVar10 = puVar11;
      } while ((undefined8 *)(lVar4 + 0x10 + lVar9) != puVar11);
    }
  }
  else if (*piVar3 != -1) {
    LOCK();
    *piVar3 = *piVar3 + 1;
    UNLOCK();
  }
  uVar1 = *(undefined *)(plVar2 + 1);
  *(long **)this = plVar6;
  *(undefined *)(plVar6 + 1) = uVar1;
  return;
}



