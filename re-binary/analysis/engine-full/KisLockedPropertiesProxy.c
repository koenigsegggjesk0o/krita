/* Class KisLockedPropertiesProxy - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisLockedPropertiesProxy @ 0020da60 ======

void __thiscall
KisLockedPropertiesProxy::KisLockedPropertiesProxy
          (KisLockedPropertiesProxy *this,KisPropertiesConfiguration *param_1,KisSharedPtr param_2)

{
  (*(code *)PTR_KisLockedPropertiesProxy_0083e800)();
  return;
}



// ====== KisLockedPropertiesProxy @ 003502e0 ======

/* KisLockedPropertiesProxy::KisLockedPropertiesProxy(KisPropertiesConfiguration*,
   KisSharedPtr<KisLockedProperties>) */

void __thiscall
KisLockedPropertiesProxy::KisLockedPropertiesProxy
          (KisLockedPropertiesProxy *this,KisPropertiesConfiguration *param_1,KisSharedPtr param_2)

{
  int *piVar1;
  int *piVar2;
  undefined *puVar3;
  undefined4 in_register_00000014;
  
  KisPropertiesConfiguration::KisPropertiesConfiguration((KisPropertiesConfiguration *)this);
  puVar3 = PTR_vtable_00837220;
  *(KisPropertiesConfiguration **)(this + 0x28) = param_1;
  *(undefined8 *)(this + 0x20) = 0;
  *(undefined **)this = puVar3 + 0x10;
  piVar1 = *(int **)CONCAT44(in_register_00000014,param_2);
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
    piVar2 = *(int **)(this + 0x20);
    *(int **)(this + 0x20) = piVar1;
    if (piVar2 != (int *)0x0) {
      LOCK();
      *piVar2 = *piVar2 + -1;
      UNLOCK();
      if (*piVar2 == 0) {
        FUN_0034ef40(piVar2);
        operator_delete(piVar2,0x18);
        return;
      }
    }
  }
  return;
}



