/* Class KisSimpleStrokeStrategy - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSimpleStrokeStrategy @ 00200d30 ======

void __thiscall
KisSimpleStrokeStrategy::KisSimpleStrokeStrategy
          (KisSimpleStrokeStrategy *this,QLatin1String *param_1,KUndo2MagicString *param_2)

{
  (*(code *)PTR_KisSimpleStrokeStrategy_00838168)();
  return;
}



// ====== KisSimpleStrokeStrategy @ 002057a0 ======

void __thiscall
KisSimpleStrokeStrategy::KisSimpleStrokeStrategy
          (KisSimpleStrokeStrategy *this,QLatin1String *param_1,KUndo2MagicString *param_2)

{
  (*(code *)PTR_KisSimpleStrokeStrategy_0083a6a0)();
  return;
}



// ====== KisSimpleStrokeStrategy @ 0020c4b0 ======

void __thiscall
KisSimpleStrokeStrategy::KisSimpleStrokeStrategy
          (KisSimpleStrokeStrategy *this,KisSimpleStrokeStrategy *param_1)

{
  (*(code *)PTR_KisSimpleStrokeStrategy_0083dd28)();
  return;
}



// ====== KisSimpleStrokeStrategy @ 004e9810 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisSimpleStrokeStrategy::KisSimpleStrokeStrategy(QLatin1String const&, KUndo2MagicString const&)
    */

void __thiscall
KisSimpleStrokeStrategy::KisSimpleStrokeStrategy
          (KisSimpleStrokeStrategy *this,QLatin1String *param_1,KUndo2MagicString *param_2)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined8 uVar3;
  long lVar4;
  undefined4 *puVar5;
  undefined8 *puVar6;
  undefined (*pauVar7) [16];
  
  KisStrokeStrategy::KisStrokeStrategy((KisStrokeStrategy *)this,param_1,param_2);
  *(undefined **)this = PTR_vtable_008375e0 + 0x10;
  lVar4 = QArrayData::allocate(1,8,6,0);
  *(long *)(this + 0x48) = lVar4;
  if (lVar4 == 0) {
    qBadAlloc();
    lVar4 = *(long *)(this + 0x48);
  }
  *(undefined4 *)(lVar4 + 4) = 6;
  puVar5 = (undefined4 *)(lVar4 + *(long *)(lVar4 + 0x10));
  *(undefined2 *)(puVar5 + 1) = 0;
  *puVar5 = 0;
  lVar4 = QArrayData::allocate(4,8,6,0);
  *(long *)(this + 0x50) = lVar4;
  if (lVar4 == 0) {
    qBadAlloc();
    lVar4 = *(long *)(this + 0x50);
  }
  uVar1 = DAT_0072f880;
  *(undefined4 *)(lVar4 + 4) = 6;
  uVar3 = _UNK_0072f888;
  uVar2 = DAT_0072f880;
  puVar6 = (undefined8 *)(lVar4 + *(long *)(lVar4 + 0x10));
  *puVar6 = uVar1;
  puVar6[1] = uVar2;
  puVar6[2] = uVar3;
  lVar4 = QArrayData::allocate(4,8,6,0);
  *(long *)(this + 0x58) = lVar4;
  if (lVar4 == 0) {
    qBadAlloc();
    lVar4 = *(long *)(this + 0x58);
  }
  *(undefined4 *)(lVar4 + 4) = 6;
  pauVar7 = (undefined (*) [16])(lVar4 + *(long *)(lVar4 + 0x10));
  *(undefined8 *)pauVar7[1] = 0;
  *pauVar7 = (undefined  [16])0x0;
  return;
}



// ====== KisSimpleStrokeStrategy @ 004e9900 ======

/* KisSimpleStrokeStrategy::KisSimpleStrokeStrategy(KisSimpleStrokeStrategy const&) */

void __thiscall
KisSimpleStrokeStrategy::KisSimpleStrokeStrategy
          (KisSimpleStrokeStrategy *this,KisSimpleStrokeStrategy *param_1)

{
  long lVar1;
  int *piVar2;
  long lVar3;
  
  KisStrokeStrategy::KisStrokeStrategy((KisStrokeStrategy *)this,(KisStrokeStrategy *)param_1);
  *(undefined **)this = PTR_vtable_008375e0 + 0x10;
  piVar2 = *(int **)(param_1 + 0x48);
  if (*piVar2 == 0) {
    if (*(char *)((long)piVar2 + 0xb) < '\0') {
      lVar3 = QArrayData::allocate(1,8,(ulong)(piVar2[2] & 0x7fffffff),0);
      *(long *)(this + 0x48) = lVar3;
      if (lVar3 == 0) {
        qBadAlloc();
        lVar3 = *(long *)(this + 0x48);
      }
      *(byte *)(lVar3 + 0xb) = *(byte *)(lVar3 + 0xb) | 0x80;
    }
    else {
      lVar3 = QArrayData::allocate(1,8,(long)piVar2[1],0);
      *(long *)(this + 0x48) = lVar3;
      if (lVar3 == 0) {
        qBadAlloc();
        lVar3 = *(long *)(this + 0x48);
      }
    }
    if ((*(uint *)(lVar3 + 8) & 0x7fffffff) != 0) {
      lVar1 = *(long *)(param_1 + 0x48);
      memcpy((void *)(lVar3 + *(long *)(lVar3 + 0x10)),(void *)(lVar1 + *(long *)(lVar1 + 0x10)),
             (long)*(int *)(lVar1 + 4));
      *(undefined4 *)(*(long *)(this + 0x48) + 4) = *(undefined4 *)(*(long *)(param_1 + 0x48) + 4);
    }
  }
  else {
    if (*piVar2 != -1) {
      LOCK();
      *piVar2 = *piVar2 + 1;
      UNLOCK();
      piVar2 = *(int **)(param_1 + 0x48);
    }
    *(int **)(this + 0x48) = piVar2;
  }
  piVar2 = *(int **)(param_1 + 0x50);
  if (*piVar2 == 0) {
    if (*(char *)((long)piVar2 + 0xb) < '\0') {
      lVar3 = QArrayData::allocate(4,8,(ulong)(piVar2[2] & 0x7fffffff),0);
      *(long *)(this + 0x50) = lVar3;
      if (lVar3 == 0) {
        qBadAlloc();
        lVar3 = *(long *)(this + 0x50);
      }
      *(byte *)(lVar3 + 0xb) = *(byte *)(lVar3 + 0xb) | 0x80;
    }
    else {
      lVar3 = QArrayData::allocate(4,8,(long)piVar2[1],0);
      *(long *)(this + 0x50) = lVar3;
      if (lVar3 == 0) {
        qBadAlloc();
        lVar3 = *(long *)(this + 0x50);
      }
    }
    if ((*(uint *)(lVar3 + 8) & 0x7fffffff) != 0) {
      lVar1 = *(long *)(param_1 + 0x50);
      memcpy((void *)(lVar3 + *(long *)(lVar3 + 0x10)),(void *)(lVar1 + *(long *)(lVar1 + 0x10)),
             (long)*(int *)(lVar1 + 4) << 2);
      *(undefined4 *)(*(long *)(this + 0x50) + 4) = *(undefined4 *)(*(long *)(param_1 + 0x50) + 4);
    }
  }
  else {
    if (*piVar2 != -1) {
      LOCK();
      *piVar2 = *piVar2 + 1;
      UNLOCK();
      piVar2 = *(int **)(param_1 + 0x50);
    }
    *(int **)(this + 0x50) = piVar2;
  }
  piVar2 = *(int **)(param_1 + 0x58);
  if (*piVar2 == 0) {
    if (*(char *)((long)piVar2 + 0xb) < '\0') {
      lVar3 = QArrayData::allocate(4,8,(ulong)(piVar2[2] & 0x7fffffff),0);
      *(long *)(this + 0x58) = lVar3;
      if (lVar3 == 0) {
        qBadAlloc();
        lVar3 = *(long *)(this + 0x58);
      }
      *(byte *)(lVar3 + 0xb) = *(byte *)(lVar3 + 0xb) | 0x80;
    }
    else {
      lVar3 = QArrayData::allocate(4,8,(long)piVar2[1],0);
      *(long *)(this + 0x58) = lVar3;
      if (lVar3 == 0) {
        qBadAlloc();
        lVar3 = *(long *)(this + 0x58);
      }
    }
    if ((*(uint *)(lVar3 + 8) & 0x7fffffff) != 0) {
      lVar1 = *(long *)(param_1 + 0x58);
      memcpy((void *)(lVar3 + *(long *)(lVar3 + 0x10)),(void *)(lVar1 + *(long *)(lVar1 + 0x10)),
             (long)*(int *)(lVar1 + 4) << 2);
      *(undefined4 *)(*(long *)(this + 0x58) + 4) = *(undefined4 *)(*(long *)(param_1 + 0x58) + 4);
      return;
    }
  }
  else {
    if (*piVar2 != -1) {
      LOCK();
      *piVar2 = *piVar2 + 1;
      UNLOCK();
      piVar2 = *(int **)(param_1 + 0x58);
    }
    *(int **)(this + 0x58) = piVar2;
  }
  return;
}



