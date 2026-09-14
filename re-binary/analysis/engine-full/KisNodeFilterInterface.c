/* Class KisNodeFilterInterface - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNodeFilterInterface @ 00204e00 ======

void __thiscall
KisNodeFilterInterface::KisNodeFilterInterface
          (KisNodeFilterInterface *this,KisPinnedSharedPtr param_1)

{
  (*(code *)PTR_KisNodeFilterInterface_0083a1d0)();
  return;
}



// ====== KisNodeFilterInterface @ 00205980 ======

void __thiscall
KisNodeFilterInterface::KisNodeFilterInterface
          (KisNodeFilterInterface *this,KisNodeFilterInterface *param_1)

{
  (*(code *)PTR_KisNodeFilterInterface_0083a790)();
  return;
}



// ====== KisNodeFilterInterface @ 0045e2b0 ======

/* KisNodeFilterInterface::KisNodeFilterInterface(KisPinnedSharedPtr<KisFilterConfiguration>) */

void __thiscall
KisNodeFilterInterface::KisNodeFilterInterface
          (KisNodeFilterInterface *this,KisPinnedSharedPtr param_1)

{
  long lVar1;
  char cVar2;
  undefined4 in_register_00000034;
  
  *(undefined **)this = PTR_vtable_008379c8 + 0x10;
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 8) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 8) = *(int *)(lVar1 + 8) + 1;
    UNLOCK();
    if (*(KisFilterConfiguration **)(this + 8) != (KisFilterConfiguration *)0x0) {
                    /* try { // try from 0045e2e8 to 0045e327 has its CatchHandler @ 0045e32f */
      KisFilterConfiguration::sanityRefUsageCounter(*(KisFilterConfiguration **)(this + 8));
    }
  }
  if (*(long *)CONCAT44(in_register_00000034,param_1) != 0) {
    cVar2 = KisFilterConfiguration::hasLocalResourcesSnapshot();
    if (cVar2 == '\0') {
      kis_safe_assert_recoverable
                ("!filterConfig || filterConfig->hasLocalResourcesSnapshot()",
                 "/builds/graphics/krita/libs/image/kis_node_filter_interface.cpp",0x2f);
      return;
    }
  }
  return;
}



// ====== KisNodeFilterInterface @ 0045e340 ======

/* KisNodeFilterInterface::KisNodeFilterInterface(KisNodeFilterInterface const&) */

void __thiscall
KisNodeFilterInterface::KisNodeFilterInterface
          (KisNodeFilterInterface *this,KisNodeFilterInterface *param_1)

{
  long *plVar1;
  long lVar2;
  long in_FS_OFFSET;
  
  plVar1 = *(long **)(param_1 + 8);
  lVar2 = *(long *)(in_FS_OFFSET + 0x28);
  *(undefined **)this = PTR_vtable_008379c8 + 0x10;
  (**(code **)(*plVar1 + 0x70))();
  if (*(KisFilterConfiguration **)(this + 8) != (KisFilterConfiguration *)0x0) {
                    /* try { // try from 0045e381 to 0045e385 has its CatchHandler @ 0045e3a2 */
    KisFilterConfiguration::sanityRefUsageCounter(*(KisFilterConfiguration **)(this + 8));
  }
  if (lVar2 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



