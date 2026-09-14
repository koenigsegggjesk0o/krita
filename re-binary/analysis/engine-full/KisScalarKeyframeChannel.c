/* Class KisScalarKeyframeChannel - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisScalarKeyframeChannel @ 00206410 ======

void __thiscall
KisScalarKeyframeChannel::KisScalarKeyframeChannel
          (KisScalarKeyframeChannel *this,KoID *param_1,KisSharedPtr param_2)

{
  (*(code *)PTR_KisScalarKeyframeChannel_0083acd8)();
  return;
}



// ====== KisScalarKeyframeChannel @ 0020a450 ======

void __thiscall
KisScalarKeyframeChannel::KisScalarKeyframeChannel
          (KisScalarKeyframeChannel *this,KisScalarKeyframeChannel *param_1)

{
  (*(code *)PTR_KisScalarKeyframeChannel_0083ccf8)();
  return;
}



// ====== KisScalarKeyframeChannel @ 00653c70 ======

/* KisScalarKeyframeChannel::KisScalarKeyframeChannel(KoID const&,
   KisSharedPtr<KisDefaultBoundsBase>) */

void __thiscall
KisScalarKeyframeChannel::KisScalarKeyframeChannel
          (KisScalarKeyframeChannel *this,KoID *param_1,KisSharedPtr param_2)

{
  long *plVar1;
  undefined8 *puVar2;
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  long *local_28;
  long local_20;
  
  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  local_28 = *(long **)CONCAT44(in_register_00000014,param_2);
  if (local_28 != (long *)0x0) {
    LOCK();
    *(int *)(local_28 + 1) = *(int *)(local_28 + 1) + 1;
    UNLOCK();
  }
                    /* try { // try from 00653ca2 to 00653ca6 has its CatchHandler @ 00653d0d */
  KisKeyframeChannel::KisKeyframeChannel((KisKeyframeChannel *)this,param_1,(KisSharedPtr)&local_28)
  ;
  if (local_28 != (long *)0x0) {
    LOCK();
    plVar1 = local_28 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if (*(int *)plVar1 == 0) {
      (**(code **)(*local_28 + 8))();
    }
  }
  *(undefined **)this = PTR_vtable_00837268 + 0x10;
                    /* try { // try from 00653cca to 00653cce has its CatchHandler @ 00653d19 */
  puVar2 = (undefined8 *)operator_new(0x20);
  *puVar2 = 0;
  *(undefined4 *)(puVar2 + 1) = 0;
  *(undefined8 **)(this + 0x18) = puVar2;
  *(undefined (*) [16])(puVar2 + 2) = (undefined  [16])0x0;
  if (local_20 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisScalarKeyframeChannel @ 006551b0 ======

/* KisScalarKeyframeChannel::KisScalarKeyframeChannel(KisScalarKeyframeChannel const&) */

void __thiscall
KisScalarKeyframeChannel::KisScalarKeyframeChannel
          (KisScalarKeyframeChannel *this,KisScalarKeyframeChannel *param_1)

{
  int *piVar1;
  Data *pDVar2;
  Data *pDVar3;
  undefined4 uVar4;
  undefined8 *puVar5;
  int *piVar6;
  long lVar7;
  undefined *puVar8;
  undefined8 *puVar9;
  undefined8 *puVar10;
  undefined4 *puVar11;
  undefined8 uVar12;
  long in_FS_OFFSET;
  Data *local_60;
  Data *local_58;
  Data *pDStack_50;
  Data *local_48;
  undefined4 local_40;
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  KisKeyframeChannel::KisKeyframeChannel((KisKeyframeChannel *)this,(KisKeyframeChannel *)param_1);
  puVar8 = PTR_vtable_00837268;
  *(undefined8 *)(this + 0x18) = 0;
  *(undefined **)this = puVar8 + 0x10;
                    /* try { // try from 006551f7 to 006551fb has its CatchHandler @ 0065540b */
  puVar9 = (undefined8 *)operator_new(0x20);
  puVar5 = *(undefined8 **)(param_1 + 0x18);
  *puVar9 = *puVar5;
  uVar4 = *(undefined4 *)(puVar5 + 1);
  *(undefined (*) [16])(puVar9 + 2) = (undefined  [16])0x0;
  lVar7 = puVar5[2];
  *(undefined4 *)(puVar9 + 1) = uVar4;
  if (lVar7 != 0) {
                    /* try { // try from 00655229 to 00655246 has its CatchHandler @ 006553ff */
    puVar10 = (undefined8 *)operator_new(0x10);
    uVar12 = ((undefined8 *)puVar5[2])[1];
    *puVar10 = *(undefined8 *)puVar5[2];
    puVar10[1] = uVar12;
    puVar11 = (undefined4 *)operator_new(0x18);
    *(undefined8 **)(puVar11 + 4) = puVar10;
    *(code **)(puVar11 + 2) = FUN_006586d0;
    puVar11[1] = 1;
    *puVar11 = 1;
    puVar9[2] = puVar10;
    piVar6 = (int *)puVar9[3];
    puVar9[3] = puVar11;
    if (piVar6 != (int *)0x0) {
      LOCK();
      piVar1 = piVar6 + 1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      if (*piVar1 == 0) {
        (**(code **)(piVar6 + 2))(piVar6);
      }
      LOCK();
      *piVar6 = *piVar6 + -1;
      UNLOCK();
      if (*piVar6 == 0) {
        operator_delete(piVar6,0x10);
      }
    }
  }
  puVar5 = *(undefined8 **)(this + 0x18);
  if ((puVar9 != puVar5) && (*(undefined8 **)(this + 0x18) = puVar9, puVar5 != (undefined8 *)0x0)) {
    piVar6 = (int *)puVar5[3];
    if (piVar6 != (int *)0x0) {
      LOCK();
      piVar1 = piVar6 + 1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      if (*piVar1 == 0) {
        (**(code **)(piVar6 + 2))(piVar6);
      }
      LOCK();
      *piVar6 = *piVar6 + -1;
      UNLOCK();
      if (*piVar6 == 0) {
        operator_delete(piVar6,0x10);
      }
    }
    operator_delete(puVar5,0x20);
  }
                    /* try { // try from 006552cf to 006552e0 has its CatchHandler @ 0065540b */
  uVar12 = KisKeyframeChannel::constKeys((KisKeyframeChannel *)param_1);
  FUN_00652d60(&local_60,uVar12);
  local_58 = local_60;
  puVar8 = PTR_shared_null_00837830;
  local_40 = 1;
  pDVar2 = local_60 + 8;
  pDVar3 = local_60 + 0xc;
  local_60 = (Data *)PTR_shared_null_00837830;
  pDStack_50 = local_58 + (long)*(int *)pDVar2 * 8 + 0x10;
  pDVar2 = local_58 + (long)*(int *)pDVar3 * 8 + 0x10;
  local_48 = pDVar2;
  if (*(int *)PTR_shared_null_00837830 == 0) {
LAB_006553a0:
    QListData::dispose((Data *)puVar8);
    pDVar3 = pDStack_50;
  }
  else {
    pDVar3 = pDStack_50;
    if (*(int *)PTR_shared_null_00837830 != -1) {
      LOCK();
      *(int *)PTR_shared_null_00837830 = *(int *)PTR_shared_null_00837830 + -1;
      UNLOCK();
      if (*(int *)puVar8 == 0) goto LAB_006553a0;
    }
  }
  for (; pDStack_50 = pDVar3, pDVar2 != pDVar3; pDVar3 = pDVar3 + 8) {
                    /* try { // try from 00655355 to 00655359 has its CatchHandler @ 00655417 */
    KisKeyframeChannel::copyKeyframe
              ((KisKeyframeChannel *)param_1,*(int *)pDVar3,(KisKeyframeChannel *)this,
               *(int *)pDVar3,(KUndo2Command *)0x0);
  }
  if (*(int *)local_58 != 0) {
    if (*(int *)local_58 == -1) goto LAB_00655383;
    LOCK();
    *(int *)local_58 = *(int *)local_58 + -1;
    UNLOCK();
    if (*(int *)local_58 != 0) goto LAB_00655383;
  }
  QListData::dispose(local_58);
LAB_00655383:
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



