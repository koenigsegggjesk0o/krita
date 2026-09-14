/* Class KisNodeOpacityCommand - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisNodeOpacityCommand @ 00362850 ======

/* KisNodeOpacityCommand::KisNodeOpacityCommand(KisSharedPtr<KisNode>, unsigned char) */

void __thiscall
KisNodeOpacityCommand::KisNodeOpacityCommand
          (KisNodeOpacityCommand *this,KisSharedPtr param_1,uchar param_2)

{
  QTextStream *pQVar1;
  int *piVar2;
  uint uVar3;
  KisBaseNode *this_00;
  SkipFirstRedoWrapper *pSVar4;
  undefined *puVar5;
  QArrayData *pQVar6;
  undefined8 uVar7;
  int iVar8;
  long lVar9;
  KisImageAnimationInterface *this_01;
  KisScalarKeyframeChannel *this_02;
  KUndo2Command *this_03;
  undefined4 in_register_00000034;
  long in_FS_OFFSET;
  QArrayData *local_88;
  QTextStream *local_80;
  QArrayData *local_78;
  uint *local_70;
  QArrayData *local_68;
  undefined local_60 [16];
  undefined8 local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_80 = *(QTextStream **)CONCAT44(in_register_00000034,param_1);
  if (local_80 != (QTextStream *)0x0) {
    LOCK();
    *(int *)(local_80 + 0x10) = *(int *)(local_80 + 0x10) + 1;
    UNLOCK();
  }
                    /* try { // try from 003628a8 to 003628ac has its CatchHandler @ 00362d95 */
  ki18ndc((char *)&local_68,"krita","(qtundo-format)");
                    /* try { // try from 003628b8 to 003628bc has its CatchHandler @ 00362d89 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_68);
                    /* try { // try from 003628cb to 003628cf has its CatchHandler @ 00362d41 */
  KUndo2MagicString::KUndo2MagicString((KUndo2MagicString *)&local_68,(QString *)&local_78);
  if (*(int *)local_78 == 0) {
LAB_00362b50:
    QArrayData::deallocate(local_78,2,8);
  }
  else if (*(int *)local_78 != -1) {
    LOCK();
    *(int *)local_78 = *(int *)local_78 + -1;
    UNLOCK();
    if (*(int *)local_78 == 0) goto LAB_00362b50;
  }
                    /* try { // try from 00362901 to 00362905 has its CatchHandler @ 00362dad */
  KisNodeCommand::KisNodeCommand
            ((KisNodeCommand *)this,(KUndo2MagicString *)&local_68,(KisSharedPtr)(QDebug *)&local_80
            );
  if (*(int *)local_68 == 0) {
LAB_00362be8:
    QArrayData::deallocate(local_68,2,8);
  }
  else if (*(int *)local_68 != -1) {
    LOCK();
    *(int *)local_68 = *(int *)local_68 + -1;
    UNLOCK();
    if (*(int *)local_68 == 0) goto LAB_00362be8;
  }
  if (local_80 != (QTextStream *)0x0) {
    LOCK();
    pQVar1 = local_80 + 0x10;
    *(int *)pQVar1 = *(int *)pQVar1 + -1;
    UNLOCK();
    if (*(int *)pQVar1 == 0) {
      (**(code **)(*(long *)local_80 + 0x20))();
    }
  }
  puVar5 = PTR_vtable_00837a18;
  *(undefined8 *)(this + 0x40) = 0;
  this[0x48] = (KisNodeOpacityCommand)param_2;
  *(undefined **)(this + 0x30) = puVar5 + 0xb8;
  *(undefined2 *)(this + 0x38) = 0;
  *(undefined **)this = puVar5 + 0x10;
                    /* try { // try from 0036296f to 003629d4 has its CatchHandler @ 00362db9 */
  KisBaseNode::image();
  if ((((local_68 == (QArrayData *)0x0) || ((uint *)local_60._0_8_ == (uint *)0x0)) ||
      ((*(uint *)local_60._0_8_ & 1) == 0)) || (local_68 == (QArrayData *)0x0)) {
                    /* try { // try from 00362b0b to 00362b0f has its CatchHandler @ 00362d7d */
    kis_safe_assert_recoverable
              ("node->image()",
               "/builds/graphics/krita/libs/image/commands/kis_node_opacity_command.cpp",0x1a);
    local_68 = (QArrayData *)0x0;
    if ((int *)local_60._0_8_ != (int *)0x0) {
      LOCK();
      iVar8 = *(int *)local_60._0_8_;
      *(int *)local_60._0_8_ = *(int *)local_60._0_8_ + -2;
      UNLOCK();
      if ((iVar8 < 3) && ((int *)local_60._0_8_ != (int *)0x0)) {
        operator_delete((void *)local_60._0_8_,4);
      }
    }
    goto LAB_00362ad0;
  }
  local_68 = (QArrayData *)0x0;
  if ((uint *)local_60._0_8_ != (uint *)0x0) {
    LOCK();
    uVar3 = *(uint *)local_60._0_8_;
    *(uint *)local_60._0_8_ = *(uint *)local_60._0_8_ - 2;
    UNLOCK();
    if (((int)uVar3 < 3) && ((uint *)local_60._0_8_ != (uint *)0x0)) {
      operator_delete((void *)local_60._0_8_,4);
    }
  }
  KisBaseNode::image();
                    /* try { // try from 003629ed to 00362a0f has its CatchHandler @ 00362d4d */
  if ((((local_78 == (QArrayData *)0x0) || (local_70 == (uint *)0x0)) || ((*local_70 & 1) == 0)) &&
     (lVar9 = _41000(), *(char *)(lVar9 + 0x11) != '\0')) {
                    /* try { // try from 00362c88 to 00362cb2 has its CatchHandler @ 00362d4d */
    lVar9 = _41000();
    local_50 = *(undefined8 *)(lVar9 + 8);
    local_68 = (QArrayData *)0x2;
    local_60 = (undefined  [16])0x0;
    QMessageLogger::warning();
    if (1 < *(int *)(local_80 + 0x28)) {
      *(uint *)(local_80 + 0x48) = *(uint *)(local_80 + 0x48) | 1;
    }
                    /* try { // try from 00362cc8 to 00362ccc has its CatchHandler @ 00362d65 */
    kisBacktrace();
                    /* try { // try from 00362cdb to 00362d39 has its CatchHandler @ 00362d71 */
    QDebug::putString((QChar *)&local_80,(ulong)(local_88 + *(long *)(local_88 + 0x10)));
    if (local_80[0x20] != (QTextStream)0x0) {
      QTextStream::operator<<(local_80,' ');
    }
    if (*(int *)local_88 == 0) {
LAB_00362d18:
      QArrayData::deallocate(local_88,2,8);
    }
    else if (*(int *)local_88 != -1) {
      LOCK();
      *(int *)local_88 = *(int *)local_88 + -1;
      UNLOCK();
      if (*(int *)local_88 == 0) goto LAB_00362d18;
    }
    QDebug::~QDebug((QDebug *)&local_80);
  }
  this_01 = (KisImageAnimationInterface *)KisImage::animationInterface((KisImage *)local_78);
  iVar8 = KisImageAnimationInterface::currentTime(this_01);
  local_78 = (QArrayData *)0x0;
  if (local_70 != (uint *)0x0) {
    LOCK();
    uVar3 = *local_70;
    *local_70 = *local_70 - 2;
    UNLOCK();
    if (((int)uVar3 < 3) && (local_70 != (uint *)0x0)) {
      operator_delete(local_70,4);
    }
  }
  this_00 = *(KisBaseNode **)(this + 0x28);
                    /* try { // try from 00362a46 to 00362a4a has its CatchHandler @ 00362db9 */
  KoID::id();
                    /* try { // try from 00362a51 to 00362a55 has its CatchHandler @ 00362d59 */
  lVar9 = KisBaseNode::getKeyframeChannel(this_00,(QString *)&local_68);
  if (*(int *)local_68 == 0) {
LAB_00362c00:
    QArrayData::deallocate(local_68,2,8);
  }
  else if (*(int *)local_68 != -1) {
    LOCK();
    *(int *)local_68 = *(int *)local_68 + -1;
    UNLOCK();
    if (*(int *)local_68 == 0) goto LAB_00362c00;
  }
  if (lVar9 != 0) {
                    /* try { // try from 00362a8a to 00362a8e has its CatchHandler @ 00362db9 */
    KisKeyframeChannel::keyframeAt((int)(KLocalizedString *)&local_68);
    pQVar6 = local_68;
    uVar7 = local_60._0_8_;
    if ((int *)local_60._0_8_ != (int *)0x0) {
      LOCK();
      piVar2 = (int *)(local_60._0_8_ + 4);
      *piVar2 = *piVar2 + -1;
      UNLOCK();
      if (*piVar2 == 0) {
        (**(code **)(local_60._0_8_ + 8))(local_60._0_8_);
      }
      LOCK();
      *(int *)uVar7 = *(int *)uVar7 + -1;
      UNLOCK();
      if (*(int *)uVar7 == 0) {
        operator_delete((void *)uVar7,0x10);
      }
    }
    if (pQVar6 == (QArrayData *)0x0) {
      this_02 = (KisScalarKeyframeChannel *)
                __dynamic_cast(lVar9,PTR_typeinfo_00837fe0,PTR_typeinfo_00837e10,0);
      if (this_02 == (KisScalarKeyframeChannel *)0x0) {
        kis_assert_exception
                  ("scalarChannel",
                   "/builds/graphics/krita/libs/image/commands/kis_node_opacity_command.cpp",0x21);
      }
                    /* try { // try from 00362b91 to 00362b95 has its CatchHandler @ 00362db9 */
      this_03 = (KUndo2Command *)operator_new(0x38);
                    /* try { // try from 00362ba0 to 00362ba4 has its CatchHandler @ 00362da1 */
      KisCommandUtils::SkipFirstRedoWrapper::SkipFirstRedoWrapper
                ((SkipFirstRedoWrapper *)this_03,(KUndo2Command *)0x0,(KUndo2Command *)0x0);
      pSVar4 = *(SkipFirstRedoWrapper **)(this + 0x40);
      if ((this_03 != (KUndo2Command *)pSVar4) &&
         (*(KUndo2Command **)(this + 0x40) = this_03, pSVar4 != (SkipFirstRedoWrapper *)0x0)) {
        (**(code **)(*(long *)pSVar4 + 8))();
        this_03 = *(KUndo2Command **)(this + 0x40);
      }
                    /* try { // try from 00362bd7 to 00362c7f has its CatchHandler @ 00362db9 */
      KisScalarKeyframeChannel::addScalarKeyframe(this_02,iVar8,(double)param_2,this_03);
    }
  }
LAB_00362ad0:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



