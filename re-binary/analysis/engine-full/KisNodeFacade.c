/* Class KisNodeFacade - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNodeFacade @ 00209c10 ======

void __thiscall KisNodeFacade::KisNodeFacade(KisNodeFacade *this)

{
  (*(code *)PTR_KisNodeFacade_0083c8d8)();
  return;
}



// ====== KisNodeFacade @ 005b8af0 ======

/* KisNodeFacade::KisNodeFacade() */

void __thiscall KisNodeFacade::KisNodeFacade(KisNodeFacade *this)

{
  undefined (*pauVar1) [16];
  
  *(undefined **)this = PTR_vtable_008371b0 + 0x10;
  pauVar1 = (undefined (*) [16])operator_new(0x10);
  *(undefined (**) [16])(this + 8) = pauVar1;
  *pauVar1 = (undefined  [16])0x0;
  return;
}



// ====== KisNodeFacade @ 005ba840 ======

/* KisNodeFacade::KisNodeFacade(KisSharedPtr<KisNode>) */

void __thiscall KisNodeFacade::KisNodeFacade(KisNodeFacade *this,KisSharedPtr param_1)

{
  int iVar1;
  uint uVar2;
  long lVar3;
  long lVar4;
  undefined (*pauVar5) [16];
  int *piVar6;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  uint *local_30;
  
  lVar3 = *(long *)(in_FS_OFFSET + 0x28);
  *(undefined **)this = PTR_vtable_008371b0 + 0x10;
  pauVar5 = (undefined (*) [16])operator_new(0x10);
  lVar4 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(undefined (**) [16])(this + 8) = pauVar5;
  *pauVar5 = (undefined  [16])0x0;
  if (lVar4 == 0) {
    local_30 = (uint *)0x0;
    *(undefined8 *)*pauVar5 = 0;
LAB_005ba9d5:
    *(undefined8 *)*pauVar5 = 0;
LAB_005ba9dc:
    *(undefined8 *)(*pauVar5 + 8) = 0;
LAB_005ba91f:
    if (local_30 != (uint *)0x0) {
      LOCK();
      uVar2 = *local_30;
      *local_30 = *local_30 - 2;
      UNLOCK();
      if (((int)uVar2 < 3) && (local_30 != (uint *)0x0)) {
        if (lVar3 == *(long *)(in_FS_OFFSET + 0x28)) {
          operator_delete(local_30,4);
          return;
        }
        goto LAB_005baa53;
      }
    }
  }
  else {
    local_30 = *(uint **)(lVar4 + 0x18);
    if (local_30 == (uint *)0x0) {
                    /* try { // try from 005baa0d to 005baa11 has its CatchHandler @ 005baa62 */
      piVar6 = (int *)operator_new(4);
      *piVar6 = 0;
      *(int **)(lVar4 + 0x18) = piVar6;
      LOCK();
      *piVar6 = *piVar6 + 1;
      UNLOCK();
      local_30 = *(uint **)(lVar4 + 0x18);
    }
    LOCK();
    *local_30 = *local_30 + 2;
    UNLOCK();
    pauVar5 = *(undefined (**) [16])(this + 8);
    piVar6 = *(int **)(*pauVar5 + 8);
    *(undefined8 *)*pauVar5 = 0;
    if (piVar6 != (int *)0x0) {
      LOCK();
      iVar1 = *piVar6;
      *piVar6 = *piVar6 + -2;
      UNLOCK();
      if (iVar1 < 3) {
        if (*(void **)(*pauVar5 + 8) != (void *)0x0) {
          operator_delete(*(void **)(*pauVar5 + 8),4);
        }
        *(undefined8 *)(*pauVar5 + 8) = 0;
      }
    }
    if (lVar4 == 0) goto LAB_005ba9d5;
    if (local_30 != (uint *)0x0) {
      if ((*local_30 & 1) != 0) {
        *(long *)*pauVar5 = lVar4;
        if (lVar4 != 0) {
          piVar6 = *(int **)(lVar4 + 0x18);
          if (piVar6 == (int *)0x0) {
                    /* try { // try from 005baa35 to 005baa39 has its CatchHandler @ 005baa6e */
            piVar6 = (int *)operator_new(4);
            *piVar6 = 0;
            *(int **)(lVar4 + 0x18) = piVar6;
            LOCK();
            *piVar6 = *piVar6 + 1;
            UNLOCK();
            piVar6 = *(int **)(lVar4 + 0x18);
          }
          *(int **)(*pauVar5 + 8) = piVar6;
          LOCK();
          *piVar6 = *piVar6 + 2;
          UNLOCK();
          goto LAB_005ba91f;
        }
        goto LAB_005ba9dc;
      }
      *pauVar5 = (undefined  [16])0x0;
      goto LAB_005ba91f;
    }
    *pauVar5 = (undefined  [16])0x0;
  }
  if (lVar3 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
LAB_005baa53:
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



