/* Class KisMergeLabeledLayersCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisMergeLabeledLayersCommand @ 00372e80 ======

/* KisMergeLabeledLayersCommand::KisMergeLabeledLayersCommand(KisSharedPtr<KisImage>,
   KisSharedPtr<KisPaintDevice>, QList<int>, KisMergeLabeledLayersCommand::GroupSelectionPolicy) */

void __thiscall
KisMergeLabeledLayersCommand::KisMergeLabeledLayersCommand
          (KisMergeLabeledLayersCommand *this,undefined8 *param_2,long *param_3,undefined8 param_4,
          undefined4 param_5)

{
  long lVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  KisImage *this_00;
  KoColorSpace *pKVar5;
  KisSurrogateUndoStore *this_01;
  KisImageAnimationInterface *this_02;
  KisImageAnimationInterface *this_03;
  long in_FS_OFFSET;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_48 = (QArrayData *)QString::fromAscii_helper("MERGE_LABELED_LAYERS",0x14);
                    /* try { // try from 00372edb to 00372edf has its CatchHandler @ 0037316e */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_50,(QString *)&local_48);
                    /* try { // try from 00372ee8 to 00372eec has its CatchHandler @ 0037313e */
  KUndo2Command::KUndo2Command
            ((KUndo2Command *)this,(KUndo2MagicString *)&local_50,(KUndo2Command *)0x0);
  if (*(int *)local_50 == 0) {
LAB_00373088:
    QArrayData::deallocate(local_50,2,8);
    iVar3 = *(int *)local_48;
    if (iVar3 != 0) goto LAB_00372f1f;
LAB_003730a6:
    QArrayData::deallocate(local_48,2,8);
  }
  else {
    if (*(int *)local_50 != -1) {
      LOCK();
      *(int *)local_50 = *(int *)local_50 + -1;
      UNLOCK();
      if (*(int *)local_50 == 0) goto LAB_00373088;
    }
    iVar3 = *(int *)local_48;
    if (iVar3 == 0) goto LAB_003730a6;
LAB_00372f1f:
    if (iVar3 != -1) {
      LOCK();
      *(int *)local_48 = *(int *)local_48 + -1;
      UNLOCK();
      if (*(int *)local_48 == 0) goto LAB_003730a6;
    }
  }
  *(undefined **)this = PTR_vtable_00837bd8 + 0x10;
                    /* try { // try from 00372f46 to 00372f4a has its CatchHandler @ 00373192 */
  this_00 = (KisImage *)operator_new(0x68);
                    /* try { // try from 00372f5a to 00372f5e has its CatchHandler @ 00373162 */
  local_48 = (QArrayData *)QString::fromAscii_helper("Merge Labeled Layers Reference Image",0x24);
                    /* try { // try from 00372f68 to 00372f94 has its CatchHandler @ 0037317a */
  pKVar5 = (KoColorSpace *)KisImage::colorSpace((KisImage *)*param_2);
  iVar3 = KisImage::height((KisImage *)*param_2);
  iVar4 = KisImage::width((KisImage *)*param_2);
  this_01 = (KisSurrogateUndoStore *)operator_new(0x18);
                    /* try { // try from 00372f9b to 00372f9f has its CatchHandler @ 0037314a */
  KisSurrogateUndoStore::KisSurrogateUndoStore(this_01);
                    /* try { // try from 00372fb5 to 00372fb9 has its CatchHandler @ 0037317a */
  KisImage::KisImage(this_00,(KisUndoStore *)this_01,iVar4,iVar3,pKVar5,(QString *)&local_48);
  *(KisImage **)(this + 0x28) = this_00;
  LOCK();
  *(int *)(this_00 + 0x50) = *(int *)(this_00 + 0x50) + 1;
  UNLOCK();
  if (*(int *)local_48 != 0) {
    if (*(int *)local_48 == -1) goto LAB_00372fe8;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_00372fe8;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_00372fe8:
  *(undefined8 *)(this + 0x50) = 0;
  *(undefined (*) [16])(this + 0x30) = (undefined  [16])0x0;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
  lVar1 = *param_3;
  *(long *)(this + 0x58) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 0x10) = *(int *)(lVar1 + 0x10) + 1;
    UNLOCK();
  }
                    /* try { // try from 00373019 to 0037301d has its CatchHandler @ 00373186 */
  KisNodeFacade::root();
                    /* try { // try from 0037302a to 0037302e has its CatchHandler @ 00373156 */
  FUN_003743c0(this + 0x68,param_4);
  lVar1 = *param_3;
  this[0x74] = (KisMergeLabeledLayersCommand)0x1;
  *(undefined8 *)(this + 0x78) = 0;
  *(undefined4 *)(this + 0x70) = param_5;
  if (lVar1 == 0) {
    kis_assert_exception
              ("newRefPaintDevice",
               "/builds/graphics/krita/libs/image/commands_new/KisMergeLabeledLayersCommand.cpp",
               0x27);
  }
                    /* try { // try from 00373050 to 00373127 has its CatchHandler @ 00373132 */
  KisImage::animationInterface((KisImage *)*param_2);
  cVar2 = KisImageAnimationInterface::hasAnimation();
  if (cVar2 != '\0') {
    this_02 = (KisImageAnimationInterface *)
              KisImage::animationInterface(*(KisImage **)(this + 0x28));
    this_03 = (KisImageAnimationInterface *)KisImage::animationInterface((KisImage *)*param_2);
    iVar3 = KisImageAnimationInterface::currentTime(this_03);
    KisImageAnimationInterface::switchCurrentTimeAsync(this_02,iVar3,0);
    KisImage::waitForDone(*(KisImage **)(this + 0x28));
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisMergeLabeledLayersCommand @ 003731a0 ======

/* KisMergeLabeledLayersCommand::KisMergeLabeledLayersCommand(KisSharedPtr<KisImage>,
   QSharedPointer<QList<KisMergeLabeledLayersCommand::ReferenceNodeInfo> >,
   QSharedPointer<QList<KisMergeLabeledLayersCommand::ReferenceNodeInfo> >,
   KisSharedPtr<KisPaintDevice>, KisSharedPtr<KisPaintDevice>, QList<int>,
   KisMergeLabeledLayersCommand::GroupSelectionPolicy, bool, KisSharedPtr<KisNode>) */

void __thiscall
KisMergeLabeledLayersCommand::KisMergeLabeledLayersCommand
          (void *this,undefined8 *param_2,long *param_3,long *param_4,long *param_5,long *param_6,
          undefined8 param_11,undefined4 param_12,undefined param_13,long *param_14)

{
  int *piVar1;
  long lVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  KisImage *this_00;
  KoColorSpace *pKVar6;
  KisSurrogateUndoStore *this_01;
  KisImageAnimationInterface *this_02;
  KisImageAnimationInterface *this_03;
  long in_FS_OFFSET;
  QArrayData *local_50;
  QArrayData *local_48;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_48 = (QArrayData *)QString::fromAscii_helper("MERGE_LABELED_LAYERS",0x14);
                    /* try { // try from 00373225 to 00373229 has its CatchHandler @ 003735ca */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_50,(QString *)&local_48);
                    /* try { // try from 00373232 to 00373236 has its CatchHandler @ 003735a6 */
  KUndo2Command::KUndo2Command
            ((KUndo2Command *)this,(KUndo2MagicString *)&local_50,(KUndo2Command *)0x0);
  if (*(int *)local_50 == 0) {
LAB_00373450:
    QArrayData::deallocate(local_50,2,8);
    iVar4 = *(int *)local_48;
    if (iVar4 != 0) goto LAB_00373269;
LAB_0037346e:
    QArrayData::deallocate(local_48,2,8);
  }
  else {
    if (*(int *)local_50 != -1) {
      LOCK();
      *(int *)local_50 = *(int *)local_50 + -1;
      UNLOCK();
      if (*(int *)local_50 == 0) goto LAB_00373450;
    }
    iVar4 = *(int *)local_48;
    if (iVar4 == 0) goto LAB_0037346e;
LAB_00373269:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_48 = *(int *)local_48 + -1;
      UNLOCK();
      if (*(int *)local_48 == 0) goto LAB_0037346e;
    }
  }
  *(undefined **)this = PTR_vtable_00837bd8 + 0x10;
                    /* try { // try from 00373290 to 00373294 has its CatchHandler @ 0037358e */
  this_00 = (KisImage *)operator_new(0x68);
                    /* try { // try from 003732a4 to 003732a8 has its CatchHandler @ 00373582 */
  local_48 = (QArrayData *)QString::fromAscii_helper("Merge Labeled Layers Reference Image",0x24);
                    /* try { // try from 003732b2 to 003732df has its CatchHandler @ 003735be */
  pKVar6 = (KoColorSpace *)KisImage::colorSpace((KisImage *)*param_2);
  iVar4 = KisImage::height((KisImage *)*param_2);
  iVar5 = KisImage::width((KisImage *)*param_2);
  this_01 = (KisSurrogateUndoStore *)operator_new(0x18);
                    /* try { // try from 003732e8 to 003732ec has its CatchHandler @ 003735b2 */
  KisSurrogateUndoStore::KisSurrogateUndoStore(this_01);
                    /* try { // try from 00373307 to 0037330b has its CatchHandler @ 003735be */
  KisImage::KisImage(this_00,(KisUndoStore *)this_01,iVar5,iVar4,pKVar6,(QString *)&local_48);
  *(KisImage **)((long)this + 0x28) = this_00;
  LOCK();
  *(int *)(this_00 + 0x50) = *(int *)(this_00 + 0x50) + 1;
  UNLOCK();
  if (*(int *)local_48 != 0) {
    if (*(int *)local_48 == -1) goto LAB_0037333a;
    LOCK();
    *(int *)local_48 = *(int *)local_48 + -1;
    UNLOCK();
    if (*(int *)local_48 != 0) goto LAB_0037333a;
  }
  QArrayData::deallocate(local_48,2,8);
LAB_0037333a:
  lVar2 = param_3[1];
  piVar1 = (int *)param_3[1];
  *(long *)((long)this + 0x30) = *param_3;
  *(long *)((long)this + 0x38) = lVar2;
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
    LOCK();
    piVar1 = (int *)(*(long *)((long)this + 0x38) + 4);
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  lVar2 = param_4[1];
  piVar1 = (int *)param_4[1];
  *(long *)((long)this + 0x40) = *param_4;
  *(long *)((long)this + 0x48) = lVar2;
  if (piVar1 != (int *)0x0) {
    LOCK();
    *piVar1 = *piVar1 + 1;
    UNLOCK();
    LOCK();
    piVar1 = (int *)(*(long *)((long)this + 0x48) + 4);
    *piVar1 = *piVar1 + 1;
    UNLOCK();
  }
  lVar2 = *param_5;
  *(long *)((long)this + 0x50) = lVar2;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
  }
  lVar2 = *param_6;
  *(long *)((long)this + 0x58) = lVar2;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
  }
                    /* try { // try from 003733ab to 003733af has its CatchHandler @ 00373576 */
  KisNodeFacade::root();
                    /* try { // try from 003733bc to 003733c0 has its CatchHandler @ 0037356a */
  FUN_003743c0((long)this + 0x68,param_11);
  *(undefined4 *)((long)this + 0x70) = param_12;
  *(undefined *)((long)this + 0x74) = param_13;
  lVar2 = *param_14;
  *(long *)((long)this + 0x78) = lVar2;
  if (lVar2 != 0) {
    LOCK();
    *(int *)(lVar2 + 0x10) = *(int *)(lVar2 + 0x10) + 1;
    UNLOCK();
  }
  if (*param_3 == 0) {
    kis_safe_assert_recoverable
              ("prevRefNodeInfoList",
               "/builds/graphics/krita/libs/image/commands_new/KisMergeLabeledLayersCommand.cpp",
               0x43);
    lVar2 = *param_4;
  }
  else {
    lVar2 = *param_4;
  }
  if (lVar2 == 0) {
    kis_safe_assert_recoverable
              ("newRefNodeInfoList",
               "/builds/graphics/krita/libs/image/commands_new/KisMergeLabeledLayersCommand.cpp",
               0x44);
  }
  if (*param_5 == 0) {
    kis_safe_assert_recoverable
              ("prevRefPaintDevice",
               "/builds/graphics/krita/libs/image/commands_new/KisMergeLabeledLayersCommand.cpp",
               0x45);
  }
  if (*param_6 == 0) {
    kis_assert_exception
              ("newRefPaintDevice",
               "/builds/graphics/krita/libs/image/commands_new/KisMergeLabeledLayersCommand.cpp",
               0x46);
  }
                    /* try { // try from 0037341a to 0037355f has its CatchHandler @ 0037359a */
  KisImage::animationInterface((KisImage *)*param_2);
  cVar3 = KisImageAnimationInterface::hasAnimation();
  if (cVar3 != '\0') {
    this_02 = (KisImageAnimationInterface *)
              KisImage::animationInterface(*(KisImage **)((long)this + 0x28));
    this_03 = (KisImageAnimationInterface *)KisImage::animationInterface((KisImage *)*param_2);
    iVar4 = KisImageAnimationInterface::currentTime(this_03);
    KisImageAnimationInterface::switchCurrentTimeAsync(this_02,iVar4,0);
    KisImage::waitForDone(*(KisImage **)((long)this + 0x28));
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



