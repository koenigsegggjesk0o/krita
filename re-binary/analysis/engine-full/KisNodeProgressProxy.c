/* Class KisNodeProgressProxy - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNodeProgressProxy @ 002077b0 ======

void __thiscall
KisNodeProgressProxy::KisNodeProgressProxy(KisNodeProgressProxy *this,KisNode *param_1)

{
  (*(code *)PTR_KisNodeProgressProxy_0083b6a8)();
  return;
}



// ====== KisNodeProgressProxy @ 005baca0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisNodeProgressProxy::KisNodeProgressProxy(KisNode*) */

void __thiscall
KisNodeProgressProxy::KisNodeProgressProxy(KisNodeProgressProxy *this,KisNode *param_1)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined *puVar3;
  undefined8 *puVar4;
  int *piVar5;
  
  QObject::QObject((QObject *)this,(QObject *)0x0);
  puVar3 = PTR_vtable_00836f80 + 0xa0;
  *(undefined **)this = PTR_vtable_00836f80 + 0x10;
  *(undefined **)(this + 0x10) = puVar3;
                    /* try { // try from 005bacd3 to 005bad21 has its CatchHandler @ 005bad38 */
  puVar4 = (undefined8 *)operator_new(0x20);
  uVar2 = _UNK_007381c8;
  uVar1 = _DAT_007381c0;
  puVar4[1] = 0;
  *(undefined8 **)(this + 0x18) = puVar4;
  *puVar4 = param_1;
  puVar4[2] = uVar1;
  puVar4[3] = uVar2;
  if (param_1 != (KisNode *)0x0) {
    piVar5 = *(int **)(param_1 + 0x18);
    if (piVar5 == (int *)0x0) {
      piVar5 = (int *)operator_new(4);
      *piVar5 = 0;
      *(int **)(param_1 + 0x18) = piVar5;
      LOCK();
      *piVar5 = *piVar5 + 1;
      UNLOCK();
      piVar5 = *(int **)(param_1 + 0x18);
    }
    puVar4[1] = piVar5;
    LOCK();
    *piVar5 = *piVar5 + 2;
    UNLOCK();
  }
  return;
}



