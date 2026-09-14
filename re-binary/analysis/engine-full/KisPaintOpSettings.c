/* Class KisPaintOpSettings - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisPaintOpSettings @ 0020da90 ======

void __thiscall
KisPaintOpSettings::KisPaintOpSettings(KisPaintOpSettings *this,QSharedPointer param_1)

{
  (*(code *)PTR_KisPaintOpSettings_0083e818)();
  return;
}



// ====== KisPaintOpSettings @ 00349af0 ======

/* KisPaintOpSettings::KisPaintOpSettings(QSharedPointer<KisResourcesInterface>) */

void __thiscall
KisPaintOpSettings::KisPaintOpSettings(KisPaintOpSettings *this,QSharedPointer param_1)

{
  int *piVar1;
  undefined8 uVar2;
  int *piVar3;
  undefined *puVar4;
  undefined (*pauVar5) [16];
  undefined8 uVar6;
  undefined4 in_register_00000034;
  
  KisPropertiesConfiguration::KisPropertiesConfiguration((KisPropertiesConfiguration *)this);
  *(undefined **)this = PTR_vtable_00837878 + 0x10;
                    /* try { // try from 00349b1d to 00349b21 has its CatchHandler @ 00349c07 */
  pauVar5 = (undefined (*) [16])operator_new(0x80);
  *pauVar5 = (undefined  [16])0x0;
  puVar4 = PTR_shared_null_008377d0;
  *(undefined8 *)(pauVar5[1] + 8) = 0;
  *(undefined **)pauVar5[1] = puVar4;
  puVar4 = PTR_shared_null_00837830;
  *(undefined8 *)pauVar5[2] = 0;
  *(undefined **)(pauVar5[2] + 8) = puVar4;
  *(undefined8 *)pauVar5[3] = 0;
  *(undefined8 *)(pauVar5[3] + 8) = 0;
  *(undefined8 *)pauVar5[4] = 0;
  *(undefined8 *)(pauVar5[4] + 8) = 0;
  *(undefined8 *)pauVar5[5] = 0;
  *(undefined8 *)(pauVar5[5] + 8) = 0;
                    /* try { // try from 00349b8b to 00349b8f has its CatchHandler @ 00349c1f */
  KisRandomSource::KisRandomSource((KisRandomSource *)(pauVar5 + 6),(int)pauVar5);
                    /* try { // try from 00349b93 to 00349b97 has its CatchHandler @ 00349c13 */
  uVar6 = KisRandomSource::generate();
  *(undefined (**) [16])(this + 0x20) = pauVar5;
  uVar2 = *(undefined8 *)CONCAT44(in_register_00000034,param_1);
  piVar3 = (int *)((undefined8 *)CONCAT44(in_register_00000034,param_1))[1];
  *(undefined8 *)(pauVar5[7] + 8) = uVar6;
  if (piVar3 != (int *)0x0) {
    LOCK();
    *piVar3 = *piVar3 + 1;
    UNLOCK();
    LOCK();
    piVar3[1] = piVar3[1] + 1;
    UNLOCK();
  }
  piVar1 = *(int **)(pauVar5[3] + 8);
  *(undefined8 *)pauVar5[3] = uVar2;
  *(int **)(pauVar5[3] + 8) = piVar3;
  if (piVar1 != (int *)0x0) {
    LOCK();
    piVar3 = piVar1 + 1;
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      (**(code **)(piVar1 + 2))(piVar1);
    }
    LOCK();
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      operator_delete(piVar1,0x10);
      return;
    }
  }
  return;
}



// ====== KisPaintOpSettings @ 00349df0 ======

/* KisPaintOpSettings::KisPaintOpSettings(KisPaintOpSettings const&) */

void __thiscall
KisPaintOpSettings::KisPaintOpSettings(KisPaintOpSettings *this,KisPaintOpSettings *param_1)

{
  long lVar1;
  int *piVar2;
  undefined8 uVar3;
  undefined *puVar4;
  undefined (*pauVar5) [16];
  
  KisPropertiesConfiguration::KisPropertiesConfiguration
            ((KisPropertiesConfiguration *)this,(KisPropertiesConfiguration *)param_1);
  *(undefined **)this = PTR_vtable_00837878 + 0x10;
                    /* try { // try from 00349e17 to 00349e1b has its CatchHandler @ 00349ed9 */
  pauVar5 = (undefined (*) [16])operator_new(0x80);
  lVar1 = *(long *)(param_1 + 0x20);
  *pauVar5 = (undefined  [16])0x0;
  piVar2 = *(int **)(lVar1 + 0x10);
  *(int **)pauVar5[1] = piVar2;
  if (1 < *piVar2 + 1U) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  puVar4 = PTR_shared_null_00837830;
  *(undefined (*) [16])(pauVar5[1] + 8) = (undefined  [16])0x0;
  *(undefined **)(pauVar5[2] + 8) = puVar4;
  *(undefined8 *)pauVar5[3] = *(undefined8 *)(lVar1 + 0x30);
  piVar2 = *(int **)(lVar1 + 0x38);
  *(int **)(pauVar5[3] + 8) = piVar2;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    LOCK();
    piVar2 = (int *)(*(long *)(pauVar5[3] + 8) + 4);
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  uVar3 = *(undefined8 *)(lVar1 + 0x48);
  piVar2 = *(int **)(lVar1 + 0x48);
  *(undefined8 *)pauVar5[4] = *(undefined8 *)(lVar1 + 0x40);
  *(undefined8 *)(pauVar5[4] + 8) = uVar3;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    LOCK();
    piVar2 = (int *)(*(long *)(pauVar5[4] + 8) + 4);
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  uVar3 = *(undefined8 *)(lVar1 + 0x58);
  piVar2 = *(int **)(lVar1 + 0x58);
  *(undefined8 *)pauVar5[5] = *(undefined8 *)(lVar1 + 0x50);
  *(undefined8 *)(pauVar5[5] + 8) = uVar3;
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    LOCK();
    piVar2 = (int *)(*(long *)(pauVar5[5] + 8) + 4);
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
                    /* try { // try from 00349eb9 to 00349ebd has its CatchHandler @ 00349ee5 */
  KisRandomSource::KisRandomSource((KisRandomSource *)(pauVar5 + 6),(int)pauVar5);
  uVar3 = *(undefined8 *)(lVar1 + 0x78);
  *(undefined (**) [16])(this + 0x20) = pauVar5;
  *(undefined8 *)(pauVar5[7] + 8) = uVar3;
  return;
}



