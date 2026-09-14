/* Class KisFilterConfiguration - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisFilterConfiguration @ 00204430 ======

void __thiscall
KisFilterConfiguration::KisFilterConfiguration
          (KisFilterConfiguration *this,QString *param_1,int param_2,QSharedPointer param_3)

{
  (*(code *)PTR_KisFilterConfiguration_00839ce8)();
  return;
}



// ====== KisFilterConfiguration @ 002048f0 ======

void __thiscall
KisFilterConfiguration::KisFilterConfiguration
          (KisFilterConfiguration *this,KisFilterConfiguration *param_1)

{
  (*(code *)PTR_KisFilterConfiguration_00839f48)();
  return;
}



// ====== KisFilterConfiguration @ 0020b780 ======

void __thiscall
KisFilterConfiguration::KisFilterConfiguration
          (KisFilterConfiguration *this,QString *param_1,int param_2,QSharedPointer param_3)

{
  (*(code *)PTR_KisFilterConfiguration_0083d690)();
  return;
}



// ====== KisFilterConfiguration @ 0020dde0 ======

void __thiscall
KisFilterConfiguration::KisFilterConfiguration
          (KisFilterConfiguration *this,KisFilterConfiguration *param_1)

{
  (*(code *)PTR_KisFilterConfiguration_0083e9c0)();
  return;
}



// ====== KisFilterConfiguration @ 003804f0 ======

/* KisFilterConfiguration::KisFilterConfiguration(QString const&, int,
   QSharedPointer<KisResourcesInterface>) */

void __thiscall
KisFilterConfiguration::KisFilterConfiguration
          (KisFilterConfiguration *this,QString *param_1,int param_2,QSharedPointer param_3)

{
  undefined8 uVar1;
  int *piVar2;
  undefined *puVar3;
  undefined8 *puVar4;
  undefined4 in_register_0000000c;
  int *piVar5;
  
  KisPropertiesConfiguration::KisPropertiesConfiguration((KisPropertiesConfiguration *)this);
  *(undefined **)this = PTR_vtable_00836f60 + 0x10;
                    /* try { // try from 00380523 to 00380527 has its CatchHandler @ 00380637 */
  puVar4 = (undefined8 *)operator_new(0x30);
  uVar1 = *(undefined8 *)CONCAT44(in_register_0000000c,param_3);
  piVar2 = (int *)((undefined8 *)CONCAT44(in_register_0000000c,param_3))[1];
  if (piVar2 == (int *)0x0) {
    piVar5 = *(int **)param_1;
    *puVar4 = piVar5;
    puVar3 = PTR_shared_null_008377d0;
    if (*piVar5 + 1U < 2) {
      *(int *)(puVar4 + 1) = param_2;
      puVar4[3] = uVar1;
      puVar4[2] = puVar3;
      puVar4[4] = 0;
      *(undefined4 *)(puVar4 + 5) = 0;
      *(undefined8 **)(this + 0x20) = puVar4;
      return;
    }
  }
  else {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    LOCK();
    piVar2[1] = piVar2[1] + 1;
    UNLOCK();
    piVar5 = *(int **)param_1;
    *puVar4 = piVar5;
    puVar3 = PTR_shared_null_008377d0;
    if (*piVar5 + 1U < 2) {
      *(int *)(puVar4 + 1) = param_2;
      puVar4[3] = uVar1;
      puVar4[2] = puVar3;
      puVar4[4] = piVar2;
      goto LAB_00380569;
    }
  }
  LOCK();
  *piVar5 = *piVar5 + 1;
  puVar3 = PTR_shared_null_008377d0;
  UNLOCK();
  *(int *)(puVar4 + 1) = param_2;
  puVar4[2] = puVar3;
  puVar4[3] = uVar1;
  puVar4[4] = piVar2;
  if (piVar2 == (int *)0x0) {
    *(undefined4 *)(puVar4 + 5) = 0;
    *(undefined8 **)(this + 0x20) = puVar4;
    return;
  }
LAB_00380569:
  LOCK();
  *piVar2 = *piVar2 + 1;
  UNLOCK();
  LOCK();
  *(int *)(puVar4[4] + 4) = *(int *)(puVar4[4] + 4) + 1;
  UNLOCK();
  *(undefined4 *)(puVar4 + 5) = 0;
  *(undefined8 **)(this + 0x20) = puVar4;
  LOCK();
  piVar5 = piVar2 + 1;
  *piVar5 = *piVar5 + -1;
  UNLOCK();
  if (*piVar5 == 0) {
    (**(code **)(piVar2 + 2))(piVar2);
  }
  LOCK();
  *piVar2 = *piVar2 + -1;
  UNLOCK();
  if (*piVar2 == 0) {
    operator_delete(piVar2,0x10);
    return;
  }
  return;
}



// ====== KisFilterConfiguration @ 00380650 ======

/* KisFilterConfiguration::KisFilterConfiguration(KisFilterConfiguration const&) */

void __thiscall
KisFilterConfiguration::KisFilterConfiguration
          (KisFilterConfiguration *this,KisFilterConfiguration *param_1)

{
  undefined8 *puVar1;
  int *piVar2;
  undefined8 uVar3;
  undefined8 *puVar4;
  
  KisPropertiesConfiguration::KisPropertiesConfiguration
            ((KisPropertiesConfiguration *)this,(KisPropertiesConfiguration *)param_1);
  *(undefined **)this = PTR_vtable_00836f60 + 0x10;
                    /* try { // try from 00380678 to 0038067c has its CatchHandler @ 003806f6 */
  puVar4 = (undefined8 *)operator_new(0x30);
  puVar1 = *(undefined8 **)(param_1 + 0x20);
  piVar2 = (int *)*puVar1;
  *puVar4 = piVar2;
  if (1 < *piVar2 + 1U) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  piVar2 = (int *)puVar1[2];
  *(undefined4 *)(puVar4 + 1) = *(undefined4 *)(puVar1 + 1);
  puVar4[2] = piVar2;
  if (1 < *piVar2 + 1U) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
  }
  uVar3 = puVar1[4];
  puVar4[3] = puVar1[3];
  puVar4[4] = uVar3;
  piVar2 = (int *)puVar4[4];
  if (piVar2 != (int *)0x0) {
    LOCK();
    *piVar2 = *piVar2 + 1;
    UNLOCK();
    LOCK();
    *(int *)(puVar4[4] + 4) = *(int *)(puVar4[4] + 4) + 1;
    UNLOCK();
  }
  *(undefined4 *)(puVar4 + 5) = 0;
  *(undefined8 **)(this + 0x20) = puVar4;
  return;
}



