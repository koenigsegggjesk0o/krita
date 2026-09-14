/* Class KisGapMap - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisGapMap @ 00207f70 ======

void __thiscall KisGapMap::KisGapMap(KisGapMap *this,int param_1,QRect *param_2,function *param_3)

{
  (*(code *)PTR_KisGapMap_0083ba88)();
  return;
}



// ====== KisGapMap @ 0041c550 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisGapMap::KisGapMap(int, QRect const&, std::function<bool (KisPaintDevice*, QRect const&)>
   const&) */

void __thiscall KisGapMap::KisGapMap(KisGapMap *this,int param_1,QRect *param_2,function *param_3)

{
  long lVar1;
  undefined8 uVar2;
  undefined *puVar3;
  int iVar4;
  undefined4 uVar5;
  KisPaintDevice *this_00;
  QString *pQVar6;
  KoColorSpace *pKVar7;
  undefined4 *puVar8;
  int iVar9;
  QArrayData *pQVar10;
  long in_FS_OFFSET;
  float fVar11;
  float fVar12;
  QArrayData *local_98;
  QArrayData *local_90;
  KoColor local_88 [56];
  QMapNodeBase *local_50;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisShared::KisShared((KisShared *)this);
  iVar4 = (*(int *)(param_2 + 0xc) - *(int *)(param_2 + 4)) + 1;
  iVar9 = (*(int *)(param_2 + 8) - *(int *)param_2) + 1;
  fVar11 = (float)iVar4 * DAT_0072c888;
  if ((float)((uint)fVar11 & DAT_00722bb0) < DAT_00722b90) {
    fVar11 = (float)((uint)((float)(int)fVar11 +
                           (float)(-(uint)((float)(int)fVar11 < fVar11) & _DAT_0072c88c)) |
                    ~DAT_00722bb0 & (uint)fVar11);
  }
  fVar12 = (float)iVar9 * DAT_0072c888;
  if ((float)((uint)fVar12 & DAT_00722bb0) < DAT_00722b90) {
    fVar12 = (float)((uint)((float)(int)fVar12 +
                           (float)(-(uint)((float)(int)fVar12 < fVar12) & _DAT_0072c88c)) |
                    ~DAT_00722bb0 & (uint)fVar12);
  }
  *(undefined8 *)(this + 0x38) = 0;
  *(undefined8 *)(this + 0x40) = 0;
  *(undefined (*) [16])(this + 0x28) = (undefined  [16])0x0;
  *(int *)(this + 0x20) = (int)fVar11;
  *(ulong *)(this + 0x10) = CONCAT44(iVar9,param_1);
  *(ulong *)(this + 0x18) = CONCAT44((int)fVar12,iVar4);
  if (*(code **)(param_3 + 4) != (code *)0x0) {
                    /* try { // try from 0041c692 to 0041c693 has its CatchHandler @ 0041c936 */
    (**(code **)(param_3 + 4))(this + 0x28,param_3,2);
    uVar2 = *(undefined8 *)(param_3 + 6);
    *(undefined8 *)(this + 0x38) = *(undefined8 *)(param_3 + 4);
    *(undefined8 *)(this + 0x40) = uVar2;
  }
  *(undefined8 *)(this + 0x48) = 0;
                    /* try { // try from 0041c6ab to 0041c6af has its CatchHandler @ 0041c977 */
  this_00 = (KisPaintDevice *)operator_new(0x28);
  puVar3 = PTR_shared_null_008377d0;
  local_90 = (QArrayData *)PTR_shared_null_008377d0;
                    /* try { // try from 0041c6bf to 0041c6c3 has its CatchHandler @ 0041c942 */
  pQVar6 = (QString *)KoColorSpaceRegistry::instance();
  local_98 = (QArrayData *)puVar3;
                    /* try { // try from 0041c6d6 to 0041c6e8 has its CatchHandler @ 0041c95f */
  pKVar7 = (KoColorSpace *)KoColorSpaceRegistry::rgb8(pQVar6);
  KisPaintDevice::KisPaintDevice(this_00,pKVar7,(QString *)&local_90);
  *(KisPaintDevice **)(this + 0x58) = this_00;
  LOCK();
  *(int *)(this_00 + 0x10) = *(int *)(this_00 + 0x10) + 1;
  UNLOCK();
  if (*(int *)local_98 == 0) {
LAB_0041c860:
    QArrayData::deallocate(local_98,2,8);
    iVar4 = *(int *)local_90;
    if (iVar4 != 0) goto LAB_0041c724;
LAB_0041c87e:
    QArrayData::deallocate(local_90,2,8);
  }
  else {
    if (*(int *)local_98 != -1) {
      LOCK();
      *(int *)local_98 = *(int *)local_98 + -1;
      UNLOCK();
      if (*(int *)local_98 == 0) goto LAB_0041c860;
    }
    iVar4 = *(int *)local_90;
    if (iVar4 == 0) goto LAB_0041c87e;
LAB_0041c724:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_90 = *(int *)local_90 + -1;
      UNLOCK();
      if (*(int *)local_90 == 0) goto LAB_0041c87e;
    }
  }
                    /* try { // try from 0041c73d to 0041c741 has its CatchHandler @ 0041c983 */
  puVar8 = (undefined4 *)operator_new(0x20);
                    /* try { // try from 0041c749 to 0041c75f has its CatchHandler @ 0041c99b */
  uVar5 = KisPaintDevice::pixelSize(*(KisPaintDevice **)(this + 0x58));
  *puVar8 = uVar5;
  KisPaintDevice::createRandomAccessorNG();
  iVar4 = *(int *)param_2;
  *(undefined4 **)(this + 0x60) = puVar8;
  *(undefined8 *)(puVar8 + 4) = 0xffffffffffffffff;
  *(undefined8 *)(puVar8 + 6) = 0;
  if ((iVar4 != 0) || (*(int *)(param_2 + 4) != 0)) {
                    /* try { // try from 0041c853 to 0041c857 has its CatchHandler @ 0041c96b */
    kis_assert_exception
              ("(mapBounds.x() == 0) && (mapBounds.y() == 0) && \"Gap closing fill assumes x and y start at coordinate (0, 0)\""
               ,"/builds/graphics/krita/libs/image/floodfill/kis_gap_map.cpp",0x65);
  }
  local_98 = (QArrayData *)CONCAT44(local_98._4_4_,0xffffff);
                    /* try { // try from 0041c793 to 0041c797 has its CatchHandler @ 0041c96b */
  pQVar6 = (QString *)KoColorSpaceRegistry::instance();
  local_90 = (QArrayData *)puVar3;
                    /* try { // try from 0041c7a3 to 0041c7ba has its CatchHandler @ 0041c953 */
  pKVar7 = (KoColorSpace *)KoColorSpaceRegistry::rgb8(pQVar6);
  KoColor::KoColor(local_88,(uchar *)&local_98,pKVar7);
  if (*(int *)local_90 == 0) {
LAB_0041c900:
    QArrayData::deallocate(local_90,2,8);
  }
  else if (*(int *)local_90 != -1) {
    LOCK();
    *(int *)local_90 = *(int *)local_90 + -1;
    UNLOCK();
    if (*(int *)local_90 == 0) goto LAB_0041c900;
  }
                    /* try { // try from 0041c7e5 to 0041c7f8 has its CatchHandler @ 0041c98f */
  KisPaintDevice::setDefaultPixel(*(KisPaintDevice **)(this + 0x58),local_88);
  KisPaintDevice::fill(*(KisPaintDevice **)(this + 0x58),param_2,local_88);
  if (*(int *)local_50 != 0) {
    if (*(int *)local_50 == -1) goto LAB_0041c81c;
    LOCK();
    *(int *)local_50 = *(int *)local_50 + -1;
    UNLOCK();
    if (*(int *)local_50 != 0) goto LAB_0041c81c;
  }
  lVar1 = *(long *)(local_50 + 0x10);
  if (lVar1 != 0) {
    pQVar10 = *(QArrayData **)(lVar1 + 0x18);
    if (*(int *)pQVar10 == 0) {
LAB_0041c920:
      QArrayData::deallocate(pQVar10,2,8);
    }
    else if (*(int *)pQVar10 != -1) {
      LOCK();
      *(int *)pQVar10 = *(int *)pQVar10 + -1;
      UNLOCK();
      if (*(int *)pQVar10 == 0) {
        pQVar10 = *(QArrayData **)(lVar1 + 0x18);
        goto LAB_0041c920;
      }
    }
    QVariant::~QVariant((QVariant *)(lVar1 + 0x20));
    if (*(long *)(lVar1 + 8) != 0) {
      FUN_00323e20();
    }
    if (*(long *)(lVar1 + 0x10) != 0) {
      FUN_00323e20();
    }
    QMapDataBase::freeTree(local_50,(int)*(undefined8 *)(local_50 + 0x10));
  }
  QMapDataBase::freeData((QMapDataBase *)local_50);
LAB_0041c81c:
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



