/* Class KisColorTransformationConfiguration - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisColorTransformationConfiguration @ 00205180 ======

void __thiscall
KisColorTransformationConfiguration::KisColorTransformationConfiguration
          (KisColorTransformationConfiguration *this,KisColorTransformationConfiguration *param_1)

{
  (*(code *)PTR_KisColorTransformationConfiguration_0083a390)();
  return;
}



// ====== KisColorTransformationConfiguration @ 00209470 ======

void __thiscall
KisColorTransformationConfiguration::KisColorTransformationConfiguration
          (KisColorTransformationConfiguration *this,QString *param_1,int param_2,
          QSharedPointer param_3)

{
  (*(code *)PTR_KisColorTransformationConfiguration_0083c508)();
  return;
}



// ====== KisColorTransformationConfiguration @ 00381af0 ======

/* KisColorTransformationConfiguration::KisColorTransformationConfiguration(QString const&, int,
   QSharedPointer<KisResourcesInterface>) */

void __thiscall
KisColorTransformationConfiguration::KisColorTransformationConfiguration
          (KisColorTransformationConfiguration *this,QString *param_1,int param_2,
          QSharedPointer param_3)

{
  int *piVar1;
  undefined *puVar2;
  int *piVar3;
  undefined8 *puVar4;
  undefined4 in_register_0000000c;
  long in_FS_OFFSET;
  undefined8 local_38;
  int *piStack_30;
  long local_20;
  
  local_38 = *(undefined8 *)CONCAT44(in_register_0000000c,param_3);
  piStack_30 = (int *)((undefined8 *)CONCAT44(in_register_0000000c,param_3))[1];
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  if (piStack_30 != (int *)0x0) {
    LOCK();
    *piStack_30 = *piStack_30 + 1;
    UNLOCK();
    LOCK();
    piStack_30[1] = piStack_30[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 00381b33 to 00381b37 has its CatchHandler @ 00381bc0 */
  KisFilterConfiguration::KisFilterConfiguration
            ((KisFilterConfiguration *)this,param_1,param_2,(QSharedPointer)&local_38);
  piVar3 = piStack_30;
  if (piStack_30 != (int *)0x0) {
    LOCK();
    piVar1 = piStack_30 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piStack_30 + 2))(piStack_30);
    }
    LOCK();
    *piVar3 = *piVar3 + -1;
    UNLOCK();
    if (*piVar3 == 0) {
      operator_delete(piVar3,0x10);
    }
  }
  *(undefined **)this = PTR_vtable_00837ec0 + 0x10;
                    /* try { // try from 00381b69 to 00381b6d has its CatchHandler @ 00381bb4 */
  puVar4 = (undefined8 *)operator_new(0x10);
  puVar2 = PTR_shared_null_008372c0;
  puVar4[1] = 0;
  *(undefined8 **)(this + 0x28) = puVar4;
  *puVar4 = puVar2;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisColorTransformationConfiguration @ 00381bd0 ======

/* KisColorTransformationConfiguration::KisColorTransformationConfiguration(KisColorTransformationConfiguration
   const&) */

void __thiscall
KisColorTransformationConfiguration::KisColorTransformationConfiguration
          (KisColorTransformationConfiguration *this,KisColorTransformationConfiguration *param_1)

{
  undefined *puVar1;
  undefined8 *puVar2;
  
  KisFilterConfiguration::KisFilterConfiguration
            ((KisFilterConfiguration *)this,(KisFilterConfiguration *)param_1);
  *(undefined **)this = PTR_vtable_00837ec0 + 0x10;
                    /* try { // try from 00381bf5 to 00381bf9 has its CatchHandler @ 00381c17 */
  puVar2 = (undefined8 *)operator_new(0x10);
  puVar1 = PTR_shared_null_008372c0;
  puVar2[1] = 0;
  *(undefined8 **)(this + 0x28) = puVar2;
  *puVar2 = puVar1;
  return;
}



