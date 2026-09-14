/* Class KisSyncLodCacheStrokeStrategy - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisSyncLodCacheStrokeStrategy @ 0020d340 ======

void __thiscall
KisSyncLodCacheStrokeStrategy::KisSyncLodCacheStrokeStrategy
          (KisSyncLodCacheStrokeStrategy *this,KisWeakSharedPtr param_1,bool param_2)

{
  (*(code *)PTR_KisSyncLodCacheStrokeStrategy_0083e470)();
  return;
}



// ====== KisSyncLodCacheStrokeStrategy @ 004fef10 ======

/* KisSyncLodCacheStrokeStrategy::KisSyncLodCacheStrokeStrategy(KisWeakSharedPtr<KisImage>, bool) */

void __thiscall
KisSyncLodCacheStrokeStrategy::KisSyncLodCacheStrokeStrategy
          (KisSyncLodCacheStrokeStrategy *this,KisWeakSharedPtr param_1,bool param_2)

{
  long lVar1;
  undefined (*pauVar2) [16];
  int *piVar3;
  undefined4 in_register_00000034;
  long *plVar4;
  long in_FS_OFFSET;
  QArrayData *local_68;
  QArrayData *local_60;
  undefined4 local_58 [2];
  char *local_50;
  long local_40;
  
  plVar4 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  ki18ndc((char *)local_58,"krita","(qtundo-format)");
                    /* try { // try from 004fef68 to 004fef6c has its CatchHandler @ 004ff143 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)local_58);
                    /* try { // try from 004fef7e to 004fef82 has its CatchHandler @ 004ff173 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_68,(QString *)&local_60);
  if (*(int *)local_60 == 0) {
LAB_004ff0d8:
    QArrayData::deallocate(local_60,2,8);
  }
  else if (*(int *)local_60 != -1) {
    LOCK();
    *(int *)local_60 = *(int *)local_60 + -1;
    UNLOCK();
    if (*(int *)local_60 == 0) goto LAB_004ff0d8;
  }
  local_58[0] = 0x12;
  local_50 = "SyncLodCacheStroke";
                    /* try { // try from 004fefc3 to 004fefc7 has its CatchHandler @ 004ff167 */
  KisRunnableBasedStrokeStrategy::KisRunnableBasedStrokeStrategy
            ((KisRunnableBasedStrokeStrategy *)this,(QLatin1String *)local_58,
             (KUndo2MagicString *)&local_68);
  if (*(int *)local_68 == 0) {
LAB_004ff0f0:
    QArrayData::deallocate(local_68,2,8);
  }
  else if (*(int *)local_68 != -1) {
    LOCK();
    *(int *)local_68 = *(int *)local_68 + -1;
    UNLOCK();
    if (*(int *)local_68 == 0) goto LAB_004ff0f0;
  }
  *(undefined **)this = PTR_vtable_00836dd0 + 0x10;
                    /* try { // try from 004feffc to 004ff000 has its CatchHandler @ 004ff15b */
  pauVar2 = (undefined (*) [16])operator_new(0x10);
  lVar1 = *plVar4;
  *(undefined (**) [16])(this + 0x68) = pauVar2;
  *pauVar2 = (undefined  [16])0x0;
  if (lVar1 == 0) {
    *(undefined8 *)*pauVar2 = 0;
  }
  else {
    if (((uint *)plVar4[1] == (uint *)0x0) || ((*(uint *)plVar4[1] & 1) == 0)) {
      *pauVar2 = (undefined  [16])0x0;
      goto LAB_004ff035;
    }
    lVar1 = *plVar4;
    *(long *)*pauVar2 = lVar1;
    if (lVar1 != 0) {
      piVar3 = *(int **)(lVar1 + 0x58);
      if (piVar3 == (int *)0x0) {
        piVar3 = (int *)operator_new(4);
        *piVar3 = 0;
        *(int **)(lVar1 + 0x58) = piVar3;
        LOCK();
        *piVar3 = *piVar3 + 1;
        UNLOCK();
        piVar3 = *(int **)(lVar1 + 0x58);
      }
      *(int **)(*pauVar2 + 8) = piVar3;
      LOCK();
      *piVar3 = *piVar3 + 2;
      UNLOCK();
      goto LAB_004ff035;
    }
  }
  *(undefined8 *)(*pauVar2 + 8) = 0;
LAB_004ff035:
                    /* try { // try from 004ff04a to 004ff129 has its CatchHandler @ 004ff14f */
  KisSimpleStrokeStrategy::enableJob((KisSimpleStrokeStrategy *)this,0,true,2,1);
  KisSimpleStrokeStrategy::enableJob((KisSimpleStrokeStrategy *)this,3,true,1,0);
  KisStrokeStrategy::setRequestsOtherStrokesToEnd((KisStrokeStrategy *)this,false);
  KisStrokeStrategy::setClearsRedoOnStart((KisStrokeStrategy *)this,false);
  KisStrokeStrategy::setCanForgetAboutMe((KisStrokeStrategy *)this,param_2);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



