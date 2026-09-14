/* Class KisOptimizedByteArray - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisOptimizedByteArray @ 0020d7e0 ======

void __thiscall
KisOptimizedByteArray::KisOptimizedByteArray(KisOptimizedByteArray *this,QSharedPointer param_1)

{
  (*(code *)PTR_KisOptimizedByteArray_0083e6c0)();
  return;
}



// ====== KisOptimizedByteArray @ 005d8e80 ======

/* KisOptimizedByteArray::KisOptimizedByteArray(QSharedPointer<KisOptimizedByteArray::MemoryAllocator>)
    */

void __thiscall
KisOptimizedByteArray::KisOptimizedByteArray(KisOptimizedByteArray *this,QSharedPointer param_1)

{
  int *piVar1;
  long lVar2;
  int *piVar3;
  code *pcVar4;
  int iVar5;
  int *piVar6;
  undefined8 *puVar7;
  undefined4 in_register_00000034;
  int *piVar8;
  
  piVar6 = (int *)operator_new(0x38);
  lVar2 = *(long *)CONCAT44(in_register_00000034,param_1);
  piVar3 = (int *)((long *)CONCAT44(in_register_00000034,param_1))[1];
  if (piVar3 == (int *)0x0) {
    *piVar6 = 0;
    piVar6[8] = 0;
    piVar6[9] = 0;
    piVar6[10] = 0;
    piVar6[0xc] = 0;
    *(undefined (*) [16])(piVar6 + 4) = (undefined  [16])0x0;
    if (lVar2 != 0) {
      piVar6[6] = 0;
      piVar6[7] = 0;
      *(long *)(piVar6 + 4) = lVar2;
      *(long *)(piVar6 + 2) = lVar2;
      *(int **)this = piVar6;
      LOCK();
      *piVar6 = *piVar6 + 1;
      UNLOCK();
      return;
    }
LAB_005d8f58:
    if (DAT_00853670 < -1) {
                    /* WARNING: Does not return */
      pcVar4 = (code *)invalidInstructionException();
      (*pcVar4)();
    }
    if ((DAT_00853650 == '\0') && (iVar5 = __cxa_guard_acquire(&DAT_00853650), iVar5 != 0)) {
                    /* try { // try from 005d904c to 005d906e has its CatchHandler @ 005d90d9 */
      puVar7 = (undefined8 *)operator_new(8);
      *puVar7 = &PTR_FUN_008213c0;
      DAT_00853660 = puVar7;
      DAT_00853668 = (int *)operator_new(0x18);
      *(undefined8 **)(DAT_00853668 + 4) = puVar7;
      *(code **)(DAT_00853668 + 2) = FUN_005d8b30;
      DAT_00853668[1] = 1;
      *DAT_00853668 = 1;
      DAT_00853670 = -1;
      __cxa_atexit(FUN_005d8dd0,&DAT_00853660,&PTR_LOOP_0083ea60);
      __cxa_guard_release(&DAT_00853650);
    }
    piVar1 = DAT_00853668;
    puVar7 = DAT_00853660;
    if (DAT_00853668 != (int *)0x0) {
      LOCK();
      *DAT_00853668 = *DAT_00853668 + 1;
      UNLOCK();
      LOCK();
      piVar8 = piVar1 + 1;
      *piVar8 = *piVar8 + 1;
      UNLOCK();
    }
    piVar8 = *(int **)(piVar6 + 6);
    *(undefined8 **)(piVar6 + 4) = puVar7;
    *(int **)(piVar6 + 6) = piVar1;
    if (piVar8 != (int *)0x0) goto LAB_005d8f02;
  }
  else {
    LOCK();
    *piVar3 = *piVar3 + 1;
    UNLOCK();
    LOCK();
    piVar3[1] = piVar3[1] + 1;
    UNLOCK();
    *piVar6 = 0;
    piVar6[8] = 0;
    piVar6[9] = 0;
    piVar6[10] = 0;
    piVar6[0xc] = 0;
    *(undefined (*) [16])(piVar6 + 4) = (undefined  [16])0x0;
    if (lVar2 == 0) goto LAB_005d8f58;
    LOCK();
    *piVar3 = *piVar3 + 1;
    UNLOCK();
    LOCK();
    piVar3[1] = piVar3[1] + 1;
    UNLOCK();
    piVar8 = *(int **)(piVar6 + 6);
    *(long *)(piVar6 + 4) = lVar2;
    *(int **)(piVar6 + 6) = piVar3;
    if (piVar8 == (int *)0x0) {
      *(long *)(piVar6 + 2) = lVar2;
      *(int **)this = piVar6;
      LOCK();
      *piVar6 = *piVar6 + 1;
      UNLOCK();
      goto LAB_005d8f34;
    }
LAB_005d8f02:
    LOCK();
    piVar1 = piVar8 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(piVar8 + 2))(piVar8);
    }
    LOCK();
    *piVar8 = *piVar8 + -1;
    UNLOCK();
    if (*piVar8 == 0) {
      operator_delete(piVar8,0x10);
    }
    puVar7 = *(undefined8 **)(piVar6 + 4);
  }
  *(undefined8 **)(piVar6 + 2) = puVar7;
  *(int **)this = piVar6;
  LOCK();
  *piVar6 = *piVar6 + 1;
  UNLOCK();
  if (piVar3 == (int *)0x0) {
    return;
  }
LAB_005d8f34:
  LOCK();
  piVar6 = piVar3 + 1;
  *piVar6 = *piVar6 + -1;
  UNLOCK();
  if (*piVar6 == 0) {
    (**(code **)(piVar3 + 2))(piVar3);
  }
  LOCK();
  *piVar3 = *piVar3 + -1;
  UNLOCK();
  if (*piVar3 != 0) {
    return;
  }
  operator_delete(piVar3,0x10);
  return;
}



// ====== KisOptimizedByteArray @ 005d90f0 ======

/* KisOptimizedByteArray::KisOptimizedByteArray(KisOptimizedByteArray const&) */

void __thiscall
KisOptimizedByteArray::KisOptimizedByteArray
          (KisOptimizedByteArray *this,KisOptimizedByteArray *param_1)

{
  int *piVar1;
  
  piVar1 = *(int **)param_1;
  *(int **)this = piVar1;
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  return;
}



