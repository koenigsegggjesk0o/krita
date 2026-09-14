/* Class KisOverlayPaintDeviceWrapper - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisOverlayPaintDeviceWrapper @ 0032b550 ======

/* KisOverlayPaintDeviceWrapper::KisOverlayPaintDeviceWrapper(KisSharedPtr<KisPaintDevice>, int,
   KisOverlayPaintDeviceWrapper::OverlayMode, KoColorSpace const*) */

void __thiscall
KisOverlayPaintDeviceWrapper::KisOverlayPaintDeviceWrapper
          (KisOverlayPaintDeviceWrapper *this,KisSharedPtr param_1,int param_2,OverlayMode param_3,
          KoColorSpace *param_4)

{
  int *piVar1;
  int iVar2;
  code *pcVar3;
  long lVar4;
  long lVar5;
  long lVar6;
  long lVar7;
  undefined *puVar8;
  int *piVar9;
  QString *this_00;
  QMapNodeBase *pQVar10;
  byte bVar11;
  char cVar12;
  bool bVar13;
  long *plVar14;
  KoColorSpace *pKVar15;
  undefined8 *puVar16;
  long *plVar17;
  KisPaintDevice *pKVar18;
  undefined8 uVar19;
  QString *pQVar20;
  long lVar21;
  undefined4 in_register_00000034;
  long *plVar22;
  long *plVar23;
  QArrayData *pQVar24;
  long in_FS_OFFSET;
  KoColorSpace *local_138;
  QString *local_108;
  int *local_100;
  QString *local_f8;
  long local_f0;
  QString *local_e8;
  long local_e0;
  QString *local_d8;
  long local_d0;
  KoColorSpace local_c8 [56];
  QMapNodeBase *local_90;
  QMapNodeBase *local_50;
  long local_40;
  
  plVar22 = (long *)CONCAT44(in_register_00000034,param_1);
  bVar11 = 0;
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  plVar14 = (long *)operator_new(0x80);
  plVar17 = plVar14;
  for (lVar21 = 0x10; lVar21 != 0; lVar21 = lVar21 + -1) {
    *plVar17 = 0;
    plVar17 = plVar17 + (ulong)bVar11 * -2 + 1;
  }
  plVar14[1] = (long)PTR_shared_null_008377d0;
                    /* try { // try from 0032b5b9 to 0032b5bd has its CatchHandler @ 0032dd77 */
  KisRectsGrid::KisRectsGrid((KisRectsGrid *)(plVar14 + 2),0x40);
  plVar17 = (long *)*plVar22;
  plVar23 = (long *)*plVar14;
  *(long **)this = plVar14;
  *(undefined *)(plVar14 + 6) = 0;
  plVar14[9] = 0;
  plVar14[0xf] = 0;
  *(undefined (*) [16])(plVar14 + 7) = (undefined  [16])0x0;
  *(undefined (*) [16])(plVar14 + 0xb) = (undefined  [16])0x0;
  *(undefined (*) [16])(plVar14 + 0xd) = (undefined  [16])0x0;
  if (plVar17 != plVar23) {
    if (plVar17 != (long *)0x0) {
      LOCK();
      *(int *)(plVar17 + 2) = *(int *)(plVar17 + 2) + 1;
      UNLOCK();
      plVar23 = (long *)*plVar14;
    }
    *plVar14 = (long)plVar17;
    if (plVar23 != (long *)0x0) {
      LOCK();
      plVar17 = plVar23 + 2;
      *(int *)plVar17 = *(int *)plVar17 + -1;
      UNLOCK();
      if (*(int *)plVar17 == 0) {
        (**(code **)(*plVar23 + 0x20))();
      }
    }
    plVar23 = (long *)*plVar22;
  }
                    /* try { // try from 0032b625 to 0032b64c has its CatchHandler @ 0032dd5f */
  pKVar15 = (KoColorSpace *)(**(code **)(*plVar23 + 0x70))();
  local_138 = param_4;
  if ((param_4 == (KoColorSpace *)0x0) && (local_138 = pKVar15, param_3 - 1 < 2)) {
    (**(code **)(*(long *)pKVar15 + 0x70))(&local_d8,pKVar15);
    if ((*(QString **)PTR_Integer8BitsColorDepthID_008374b0 == local_d8) ||
       (cVar12 = operator==(local_d8,*(QString **)PTR_Integer8BitsColorDepthID_008374b0),
       cVar12 != '\0')) {
      if (local_d0 != 0) {
        FUN_00328630();
      }
                    /* try { // try from 0032d8cf to 0032d8fe has its CatchHandler @ 0032dd5f */
      pQVar20 = (QString *)KoColorSpaceRegistry::instance();
      (**(code **)(*(long *)pKVar15 + 0xa8))(pKVar15);
      KoID::id();
                    /* try { // try from 0032d909 to 0032d90b has its CatchHandler @ 0032dc93 */
      (**(code **)(*(long *)pKVar15 + 0x68))(&local_d8,pKVar15);
                    /* try { // try from 0032d917 to 0032d91b has its CatchHandler @ 0032dc87 */
      KoID::id();
                    /* try { // try from 0032d92c to 0032d930 has its CatchHandler @ 0032dc7b */
      local_138 = (KoColorSpace *)
                  KoColorSpaceRegistry::colorSpace
                            (pQVar20,(QString *)&local_f8,(KoColorProfile *)&local_e8);
      if (*(int *)local_f8 == 0) {
LAB_0032dc30:
        QArrayData::deallocate((QArrayData *)local_f8,2,8);
      }
      else if (*(int *)local_f8 != -1) {
        LOCK();
        *(int *)local_f8 = *(int *)local_f8 + -1;
        UNLOCK();
        if (*(int *)local_f8 == 0) goto LAB_0032dc30;
      }
      if (local_d0 != 0) {
        FUN_00328630();
      }
      if (*(int *)local_e8 == 0) {
LAB_0032d98b:
        QArrayData::deallocate((QArrayData *)local_e8,2,8);
      }
      else if (*(int *)local_e8 != -1) {
        LOCK();
        *(int *)local_e8 = *(int *)local_e8 + -1;
        UNLOCK();
        if (*(int *)local_e8 == 0) goto LAB_0032d98b;
      }
    }
    else if (local_d0 != 0) {
      FUN_00328630();
    }
  }
  puVar16 = (undefined8 *)KisPaintDevice::colorSpace((KisPaintDevice *)*plVar22);
  bVar11 = (**(code **)*puVar16)(puVar16,local_138);
  pKVar18 = (KisPaintDevice *)*plVar22;
  *(byte *)(*(long *)this + 0x30) = bVar11 ^ 1;
                    /* try { // try from 0032b65a to 0032b65e has its CatchHandler @ 0032dd0b */
  plVar17 = (long *)KisPaintDevice::colorSpace(pKVar18);
                    /* try { // try from 0032b66d to 0032b66f has its CatchHandler @ 0032ddcb */
  (**(code **)(*plVar17 + 0x68))(&local_108,plVar17);
  puVar8 = PTR_RGBAColorModelID_008371f0;
  if ((*(QString **)PTR_RGBAColorModelID_008371f0 == local_108) ||
     (cVar12 = operator==(local_108,*(QString **)PTR_RGBAColorModelID_008371f0), cVar12 != '\0')) {
                    /* try { // try from 0032b744 to 0032b748 has its CatchHandler @ 0032ddbf */
    plVar17 = (long *)KisPaintDevice::colorSpace((KisPaintDevice *)*plVar22);
                    /* try { // try from 0032b754 to 0032b756 has its CatchHandler @ 0032dd6b */
    (**(code **)(*plVar17 + 0x70))(&local_f8,plVar17);
    if ((local_f8 != *(QString **)PTR_Integer8BitsColorDepthID_008374b0) &&
       (cVar12 = operator==(local_f8,*(QString **)PTR_Integer8BitsColorDepthID_008374b0),
       cVar12 == '\0')) {
      cVar12 = '\0';
      if (local_f0 != 0) {
        FUN_00328630();
      }
      goto LAB_0032b699;
    }
                    /* try { // try from 0032b788 to 0032b78a has its CatchHandler @ 0032dd83 */
    (**(code **)(*(long *)local_138 + 0x68))(&local_e8);
    if ((local_e8 == *(QString **)puVar8) ||
       (cVar12 = operator==(local_e8,*(QString **)puVar8), cVar12 != '\0')) {
                    /* try { // try from 0032b89d to 0032b89f has its CatchHandler @ 0032dd05 */
      (**(code **)(*(long *)local_138 + 0x70))(&local_d8);
      if ((local_d8 == *(QString **)PTR_Integer16BitsColorDepthID_008377f8) ||
         (cVar12 = operator==(local_d8,*(QString **)PTR_Integer16BitsColorDepthID_008377f8),
         cVar12 != '\0')) {
                    /* try { // try from 0032b8c4 to 0032b8f8 has its CatchHandler @ 0032dccf */
        plVar17 = (long *)KisPaintDevice::colorSpace((KisPaintDevice *)*plVar22);
        plVar17 = (long *)(**(code **)(*plVar17 + 0xa8))(plVar17);
        pcVar3 = *(code **)(*plVar17 + 0x100);
        uVar19 = (**(code **)(*(long *)local_138 + 0xa8))();
        cVar12 = (*pcVar3)(plVar17,uVar19);
      }
      if (local_d0 != 0) {
        FUN_00328630();
      }
    }
    if (local_e0 != 0) {
      FUN_00328630();
    }
    if (local_f0 != 0) {
      FUN_00328630();
    }
    if (local_100 != (int *)0x0) goto LAB_0032b6a7;
    if (cVar12 == '\0') goto LAB_0032b7e8;
LAB_0032b6cb:
    lVar21 = *(long *)this;
                    /* try { // try from 0032b6ce to 0032b6d2 has its CatchHandler @ 0032dd5f */
    plVar17 = (long *)KoOptimizedPixelDataScalerU8ToU16Factory::createRgbaScaler();
LAB_0032b6d3:
    plVar14 = *(long **)(lVar21 + 0x38);
    if ((plVar17 != plVar14) && (*(long **)(lVar21 + 0x38) = plVar17, plVar14 != (long *)0x0)) {
      (**(code **)(*plVar14 + 8))();
    }
  }
  else {
LAB_0032b699:
    if (local_100 != (int *)0x0) {
LAB_0032b6a7:
      piVar9 = local_100;
      LOCK();
      piVar1 = local_100 + 1;
      *piVar1 = *piVar1 + -1;
      UNLOCK();
      if (*piVar1 == 0) {
        (**(code **)(local_100 + 2))(local_100);
      }
      LOCK();
      *piVar9 = *piVar9 + -1;
      UNLOCK();
      if (*piVar9 == 0) {
        operator_delete(piVar9,0x10);
      }
      if (cVar12 != '\0') goto LAB_0032b6cb;
    }
LAB_0032b7e8:
                    /* try { // try from 0032b7ec to 0032b7f0 has its CatchHandler @ 0032dcf9 */
    plVar17 = (long *)KisPaintDevice::colorSpace((KisPaintDevice *)*plVar22);
                    /* try { // try from 0032b7fa to 0032b7fc has its CatchHandler @ 0032dda7 */
    (**(code **)(*plVar17 + 0x68))(&local_108,plVar17);
    puVar8 = PTR_CMYKAColorModelID_00837838;
    if ((local_108 == *(QString **)PTR_CMYKAColorModelID_00837838) ||
       (cVar12 = operator==(local_108,*(QString **)PTR_CMYKAColorModelID_00837838), cVar12 != '\0'))
    {
                    /* try { // try from 0032d3b4 to 0032d3b8 has its CatchHandler @ 0032dd8f */
      plVar17 = (long *)KisPaintDevice::colorSpace((KisPaintDevice *)*plVar22);
                    /* try { // try from 0032d3c4 to 0032d3c6 has its CatchHandler @ 0032dd9b */
      (**(code **)(*plVar17 + 0x70))(&local_f8,plVar17);
      if ((local_f8 != *(QString **)PTR_Integer8BitsColorDepthID_008374b0) &&
         (cVar12 = operator==(local_f8,*(QString **)PTR_Integer8BitsColorDepthID_008374b0),
         cVar12 == '\0')) {
        if (local_f0 == 0) goto LAB_0032b823;
        FUN_00328630();
        if (local_100 == (int *)0x0) goto LAB_0032b832;
        goto LAB_0032b82d;
      }
                    /* try { // try from 0032d41d to 0032d41f has its CatchHandler @ 0032ddb3 */
      (**(code **)(*(long *)local_138 + 0x68))(&local_e8);
      if ((local_e8 == *(QString **)puVar8) ||
         (cVar12 = operator==(local_e8,*(QString **)puVar8), cVar12 != '\0')) {
                    /* try { // try from 0032d447 to 0032d449 has its CatchHandler @ 0032dce7 */
        (**(code **)(*(long *)local_138 + 0x70))(&local_d8);
        if ((local_d8 == *(QString **)PTR_Integer16BitsColorDepthID_008377f8) ||
           (cVar12 = operator==(local_d8,*(QString **)PTR_Integer16BitsColorDepthID_008377f8),
           cVar12 != '\0')) {
                    /* try { // try from 0032d46e to 0032d4a2 has its CatchHandler @ 0032dcdb */
          plVar17 = (long *)KisPaintDevice::colorSpace((KisPaintDevice *)*plVar22);
          plVar17 = (long *)(**(code **)(*plVar17 + 0xa8))(plVar17);
          pcVar3 = *(code **)(*plVar17 + 0x100);
          uVar19 = (**(code **)(*(long *)local_138 + 0xa8))();
          cVar12 = (*pcVar3)(plVar17,uVar19);
        }
        if (local_d0 != 0) {
          FUN_00328630();
        }
      }
      if (local_e0 != 0) {
        FUN_00328630();
      }
      if (local_f0 != 0) {
        FUN_00328630();
      }
      if (local_100 != (int *)0x0) {
        FUN_00328630();
      }
      if (cVar12 != '\0') {
        lVar21 = *(long *)this;
                    /* try { // try from 0032d4ee to 0032d631 has its CatchHandler @ 0032dd5f */
        plVar17 = (long *)KoOptimizedPixelDataScalerU8ToU16Factory::createCmykaScaler();
        goto LAB_0032b6d3;
      }
    }
    else {
LAB_0032b823:
      if (local_100 != (int *)0x0) {
LAB_0032b82d:
        FUN_00328630();
      }
    }
LAB_0032b832:
                    /* try { // try from 0032b836 to 0032b83a has its CatchHandler @ 0032dced */
    plVar17 = (long *)KisPaintDevice::colorSpace((KisPaintDevice *)*plVar22);
                    /* try { // try from 0032b844 to 0032b846 has its CatchHandler @ 0032dcc3 */
    (**(code **)(*plVar17 + 0x68))(&local_108,plVar17);
    puVar8 = PTR_YCbCrAColorModelID_00837898;
    if ((local_108 == *(QString **)PTR_YCbCrAColorModelID_00837898) ||
       (cVar12 = operator==(local_108,*(QString **)PTR_YCbCrAColorModelID_00837898), cVar12 != '\0')
       ) {
                    /* try { // try from 0032d674 to 0032d678 has its CatchHandler @ 0032dd17 */
      plVar17 = (long *)KisPaintDevice::colorSpace((KisPaintDevice *)*plVar22);
                    /* try { // try from 0032d684 to 0032d686 has its CatchHandler @ 0032dd23 */
      (**(code **)(*plVar17 + 0x70))(&local_f8,plVar17);
      if ((local_f8 == *(QString **)PTR_Integer8BitsColorDepthID_008374b0) ||
         (cVar12 = operator==(local_f8,*(QString **)PTR_Integer8BitsColorDepthID_008374b0),
         cVar12 != '\0')) {
                    /* try { // try from 0032d6b5 to 0032d6b7 has its CatchHandler @ 0032dcb7 */
        (**(code **)(*(long *)local_138 + 0x68))(&local_e8);
        if ((local_e8 == *(QString **)puVar8) ||
           (cVar12 = operator==(local_e8,*(QString **)puVar8), cVar12 != '\0')) {
                    /* try { // try from 0032d725 to 0032d727 has its CatchHandler @ 0032dcab */
          (**(code **)(*(long *)local_138 + 0x70))(&local_d8);
          if ((local_d8 == *(QString **)PTR_Integer16BitsColorDepthID_008377f8) ||
             (cVar12 = operator==(local_d8,*(QString **)PTR_Integer16BitsColorDepthID_008377f8),
             cVar12 != '\0')) {
                    /* try { // try from 0032d74c to 0032d780 has its CatchHandler @ 0032dc9f */
            plVar17 = (long *)KisPaintDevice::colorSpace((KisPaintDevice *)*plVar22);
            plVar17 = (long *)(**(code **)(*plVar17 + 0xa8))(plVar17);
            pcVar3 = *(code **)(*plVar17 + 0x100);
            uVar19 = (**(code **)(*(long *)local_138 + 0xa8))();
            cVar12 = (*pcVar3)(plVar17,uVar19);
          }
          if (local_d0 != 0) {
            FUN_00328630();
          }
        }
        if (local_e0 != 0) {
          FUN_00328630();
        }
        if (local_f0 != 0) {
          FUN_00328630();
        }
        if (local_100 != (int *)0x0) {
          FUN_00328630();
        }
        lVar21 = *(long *)this;
        if (cVar12 == '\0') goto LAB_0032b6f0;
                    /* try { // try from 0032d70b to 0032d70f has its CatchHandler @ 0032dd5f */
        plVar17 = (long *)KoOptimizedPixelDataScalerU8ToU16Factory::createRgbaScaler();
        goto LAB_0032b6d3;
      }
      if (local_f0 != 0) {
        FUN_00328630();
      }
    }
    if (local_100 != (int *)0x0) {
      FUN_00328630();
      lVar21 = *(long *)this;
      goto LAB_0032b6f0;
    }
  }
  lVar21 = *(long *)this;
LAB_0032b6f0:
  if (((*(char *)(lVar21 + 0x30) != '\0') || (param_3 != 2)) || (param_2 != 1)) {
    iVar2 = 0;
    if (0 < param_2) {
LAB_0032b938:
      do {
                    /* try { // try from 0032b93d to 0032b941 has its CatchHandler @ 0032dd5f */
        pKVar18 = (KisPaintDevice *)operator_new(0x28);
        local_d8 = (QString *)PTR_shared_null_008377d0;
                    /* try { // try from 0032b95c to 0032b960 has its CatchHandler @ 0032dc6f */
        KisPaintDevice::KisPaintDevice(pKVar18,local_138,(QString *)&local_d8);
        LOCK();
        *(int *)(pKVar18 + 0x10) = *(int *)(pKVar18 + 0x10) + 1;
        UNLOCK();
        local_e8 = (QString *)pKVar18;
        if (*(int *)local_d8 == 0) {
LAB_0032d370:
          QArrayData::deallocate((QArrayData *)local_d8,2,8);
        }
        else if (*(int *)local_d8 != -1) {
          LOCK();
          *(int *)local_d8 = *(int *)local_d8 + -1;
          UNLOCK();
          if (*(int *)local_d8 == 0) goto LAB_0032d370;
        }
        this_00 = local_e8;
                    /* try { // try from 0032b9a2 to 0032b9a6 has its CatchHandler @ 0032dd2f */
        KisPaintDevice::defaultPixel();
                    /* try { // try from 0032b9ba to 0032b9be has its CatchHandler @ 0032dd3b */
        KoColor::convertedTo(local_c8);
                    /* try { // try from 0032b9c5 to 0032b9c9 has its CatchHandler @ 0032dd47 */
        KisPaintDevice::setDefaultPixel((KisPaintDevice *)this_00,(KoColor *)local_c8);
        pQVar10 = local_90;
        if (*(int *)local_90 == 0) {
LAB_0032c750:
          lVar21 = *(long *)(local_90 + 0x10);
          if (lVar21 != 0) {
            pQVar24 = *(QArrayData **)(lVar21 + 0x18);
            if (*(int *)pQVar24 == 0) {
LAB_0032d508:
              QArrayData::deallocate(pQVar24,2,8);
            }
            else if (*(int *)pQVar24 != -1) {
              LOCK();
              *(int *)pQVar24 = *(int *)pQVar24 + -1;
              UNLOCK();
              if (*(int *)pQVar24 == 0) {
                pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                goto LAB_0032d508;
              }
            }
            QVariant::~QVariant((QVariant *)(lVar21 + 0x20));
            lVar4 = *(long *)(lVar21 + 8);
            if (lVar4 != 0) {
              pQVar24 = *(QArrayData **)(lVar4 + 0x18);
              if (*(int *)pQVar24 == 0) {
LAB_0032d560:
                QArrayData::deallocate(pQVar24,2,8);
              }
              else if (*(int *)pQVar24 != -1) {
                LOCK();
                *(int *)pQVar24 = *(int *)pQVar24 + -1;
                UNLOCK();
                if (*(int *)pQVar24 == 0) {
                  pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                  goto LAB_0032d560;
                }
              }
              QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
              lVar5 = *(long *)(lVar4 + 8);
              if (lVar5 != 0) {
                pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                if (*(int *)pQVar24 == 0) {
LAB_0032d7f0:
                  QArrayData::deallocate(pQVar24,2,8);
                }
                else if (*(int *)pQVar24 != -1) {
                  LOCK();
                  *(int *)pQVar24 = *(int *)pQVar24 + -1;
                  UNLOCK();
                  if (*(int *)pQVar24 == 0) {
                    pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                    goto LAB_0032d7f0;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                lVar6 = *(long *)(lVar5 + 8);
                if (lVar6 != 0) {
                  pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032d9d0:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                      goto LAB_0032d9d0;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                  lVar7 = *(long *)(lVar6 + 8);
                  if (lVar7 != 0) {
                    pQVar24 = *(QArrayData **)(lVar7 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032c86f:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar7 + 0x18);
                        goto LAB_0032c86f;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar7 + 0x20));
                    if (*(long *)(lVar7 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar7 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar6 = *(long *)(lVar6 + 0x10);
                  if (lVar6 != 0) {
                    pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032c937:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                        goto LAB_0032c937;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                    if (*(long *)(lVar6 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar6 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
                lVar5 = *(long *)(lVar5 + 0x10);
                if (lVar5 != 0) {
                  pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032db08:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                      goto LAB_0032db08;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                  lVar6 = *(long *)(lVar5 + 8);
                  if (lVar6 != 0) {
                    pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032ca1b:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                        goto LAB_0032ca1b;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                    if (*(long *)(lVar6 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar6 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar5 = *(long *)(lVar5 + 0x10);
                  if (lVar5 != 0) {
                    pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032cab7:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                        goto LAB_0032cab7;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
              }
              lVar4 = *(long *)(lVar4 + 0x10);
              if (lVar4 != 0) {
                pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                if (*(int *)pQVar24 == 0) {
LAB_0032d880:
                  QArrayData::deallocate(pQVar24,2,8);
                }
                else if (*(int *)pQVar24 != -1) {
                  LOCK();
                  *(int *)pQVar24 = *(int *)pQVar24 + -1;
                  UNLOCK();
                  if (*(int *)pQVar24 == 0) {
                    pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                    goto LAB_0032d880;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                lVar5 = *(long *)(lVar4 + 8);
                if (lVar5 != 0) {
                  pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032da00:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                      goto LAB_0032da00;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                  lVar6 = *(long *)(lVar5 + 8);
                  if (lVar6 != 0) {
                    FUN_002dd9a0(lVar6 + 0x18);
                    QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                    if (*(long *)(lVar6 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar6 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar5 = *(long *)(lVar5 + 0x10);
                  if (lVar5 != 0) {
                    FUN_002dd9a0(lVar5 + 0x18);
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
                lVar4 = *(long *)(lVar4 + 0x10);
                if (lVar4 != 0) {
                  pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032db58:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                      goto LAB_0032db58;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                  lVar5 = *(long *)(lVar4 + 8);
                  if (lVar5 != 0) {
                    pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032ccf3:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                        goto LAB_0032ccf3;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar4 = *(long *)(lVar4 + 0x10);
                  if (lVar4 != 0) {
                    pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032cd8f:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                        goto LAB_0032cd8f;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                    if (*(long *)(lVar4 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar4 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
              }
            }
            lVar21 = *(long *)(lVar21 + 0x10);
            if (lVar21 != 0) {
              pQVar24 = *(QArrayData **)(lVar21 + 0x18);
              if (*(int *)pQVar24 == 0) {
LAB_0032d5a0:
                QArrayData::deallocate(pQVar24,2,8);
              }
              else if (*(int *)pQVar24 != -1) {
                LOCK();
                *(int *)pQVar24 = *(int *)pQVar24 + -1;
                UNLOCK();
                if (*(int *)pQVar24 == 0) {
                  pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                  goto LAB_0032d5a0;
                }
              }
              QVariant::~QVariant((QVariant *)(lVar21 + 0x20));
              lVar4 = *(long *)(lVar21 + 8);
              if (lVar4 != 0) {
                pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                if (*(int *)pQVar24 == 0) {
LAB_0032d7c8:
                  QArrayData::deallocate(pQVar24,2,8);
                }
                else if (*(int *)pQVar24 != -1) {
                  LOCK();
                  *(int *)pQVar24 = *(int *)pQVar24 + -1;
                  UNLOCK();
                  if (*(int *)pQVar24 == 0) {
                    pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                    goto LAB_0032d7c8;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                lVar5 = *(long *)(lVar4 + 8);
                if (lVar5 != 0) {
                  pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032dbd8:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                      goto LAB_0032dbd8;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                  lVar6 = *(long *)(lVar5 + 8);
                  if (lVar6 != 0) {
                    pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032cebd:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                        goto LAB_0032cebd;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                    if (*(long *)(lVar6 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar6 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar5 = *(long *)(lVar5 + 0x10);
                  if (lVar5 != 0) {
                    FUN_002dd9a0(lVar5 + 0x18);
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
                lVar4 = *(long *)(lVar4 + 0x10);
                if (lVar4 != 0) {
                  pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032da30:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                      goto LAB_0032da30;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                  lVar5 = *(long *)(lVar4 + 8);
                  if (lVar5 != 0) {
                    FUN_002dd9a0(lVar5 + 0x18);
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar4 = *(long *)(lVar4 + 0x10);
                  if (lVar4 != 0) {
                    pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032d09d:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                        goto LAB_0032d09d;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                    if (*(long *)(lVar4 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar4 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
              }
              lVar21 = *(long *)(lVar21 + 0x10);
              if (lVar21 != 0) {
                pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                if (*(int *)pQVar24 == 0) {
LAB_0032d8a8:
                  QArrayData::deallocate(pQVar24,2,8);
                }
                else if (*(int *)pQVar24 != -1) {
                  LOCK();
                  *(int *)pQVar24 = *(int *)pQVar24 + -1;
                  UNLOCK();
                  if (*(int *)pQVar24 == 0) {
                    pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                    goto LAB_0032d8a8;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar21 + 0x20));
                lVar4 = *(long *)(lVar21 + 8);
                if (lVar4 != 0) {
                  pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032dbb0:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                      goto LAB_0032dbb0;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                  lVar5 = *(long *)(lVar4 + 8);
                  if (lVar5 != 0) {
                    pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032d186:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                        goto LAB_0032d186;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar4 = *(long *)(lVar4 + 0x10);
                  if (lVar4 != 0) {
                    FUN_002dd9a0(lVar4 + 0x18);
                    QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                    if (*(long *)(lVar4 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar4 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
                lVar21 = *(long *)(lVar21 + 0x10);
                if (lVar21 != 0) {
                  pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032da58:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                      goto LAB_0032da58;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar21 + 0x20));
                  lVar4 = *(long *)(lVar21 + 8);
                  if (lVar4 != 0) {
                    pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032d2a5:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                        goto LAB_0032d2a5;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                    if (*(long *)(lVar4 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar4 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar21 = *(long *)(lVar21 + 0x10);
                  if (lVar21 != 0) {
                    pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032d314:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                        goto LAB_0032d314;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar21 + 0x20));
                    if (*(long *)(lVar21 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar21 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
              }
            }
            QMapDataBase::freeTree(pQVar10,(int)*(undefined8 *)(pQVar10 + 0x10));
          }
          QMapDataBase::freeData((QMapDataBase *)pQVar10);
        }
        else if (*(int *)local_90 != -1) {
          LOCK();
          *(int *)local_90 = *(int *)local_90 + -1;
          UNLOCK();
          if (*(int *)local_90 == 0) goto LAB_0032c750;
        }
        pQVar10 = local_50;
        if (*(int *)local_50 == 0) {
LAB_0032bac0:
          lVar21 = *(long *)(local_50 + 0x10);
          if (lVar21 != 0) {
            pQVar24 = *(QArrayData **)(lVar21 + 0x18);
            if (*(int *)pQVar24 == 0) {
LAB_0032d528:
              QArrayData::deallocate(pQVar24,2,8);
            }
            else if (*(int *)pQVar24 != -1) {
              LOCK();
              *(int *)pQVar24 = *(int *)pQVar24 + -1;
              UNLOCK();
              if (*(int *)pQVar24 == 0) {
                pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                goto LAB_0032d528;
              }
            }
            QVariant::~QVariant((QVariant *)(lVar21 + 0x20));
            lVar4 = *(long *)(lVar21 + 8);
            if (lVar4 != 0) {
              pQVar24 = *(QArrayData **)(lVar4 + 0x18);
              if (*(int *)pQVar24 == 0) {
LAB_0032d580:
                QArrayData::deallocate(pQVar24,2,8);
              }
              else if (*(int *)pQVar24 != -1) {
                LOCK();
                *(int *)pQVar24 = *(int *)pQVar24 + -1;
                UNLOCK();
                if (*(int *)pQVar24 == 0) {
                  pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                  goto LAB_0032d580;
                }
              }
              QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
              lVar5 = *(long *)(lVar4 + 8);
              if (lVar5 != 0) {
                pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                if (*(int *)pQVar24 == 0) {
LAB_0032d830:
                  QArrayData::deallocate(pQVar24,2,8);
                }
                else if (*(int *)pQVar24 != -1) {
                  LOCK();
                  *(int *)pQVar24 = *(int *)pQVar24 + -1;
                  UNLOCK();
                  if (*(int *)pQVar24 == 0) {
                    pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                    goto LAB_0032d830;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                lVar6 = *(long *)(lVar5 + 8);
                if (lVar6 != 0) {
                  pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032db80:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                      goto LAB_0032db80;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                  lVar7 = *(long *)(lVar6 + 8);
                  if (lVar7 != 0) {
                    pQVar24 = *(QArrayData **)(lVar7 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032bbdf:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar7 + 0x18);
                        goto LAB_0032bbdf;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar7 + 0x20));
                    if (*(long *)(lVar7 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar7 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar6 = *(long *)(lVar6 + 0x10);
                  if (lVar6 != 0) {
                    FUN_002dd9a0(lVar6 + 0x18);
                    QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                    if (*(long *)(lVar6 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar6 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
                lVar5 = *(long *)(lVar5 + 0x10);
                if (lVar5 != 0) {
                  pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032dae0:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                      goto LAB_0032dae0;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                  lVar6 = *(long *)(lVar5 + 8);
                  if (lVar6 != 0) {
                    pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032bd53:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                        goto LAB_0032bd53;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                    if (*(long *)(lVar6 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar6 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar5 = *(long *)(lVar5 + 0x10);
                  if (lVar5 != 0) {
                    pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032bdef:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                        goto LAB_0032bdef;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
              }
              lVar4 = *(long *)(lVar4 + 0x10);
              if (lVar4 != 0) {
                pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                if (*(int *)pQVar24 == 0) {
LAB_0032d858:
                  QArrayData::deallocate(pQVar24,2,8);
                }
                else if (*(int *)pQVar24 != -1) {
                  LOCK();
                  *(int *)pQVar24 = *(int *)pQVar24 + -1;
                  UNLOCK();
                  if (*(int *)pQVar24 == 0) {
                    pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                    goto LAB_0032d858;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                lVar5 = *(long *)(lVar4 + 8);
                if (lVar5 != 0) {
                  pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032dab0:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                      goto LAB_0032dab0;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                  lVar6 = *(long *)(lVar5 + 8);
                  if (lVar6 != 0) {
                    pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032beef:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                        goto LAB_0032beef;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                    if (*(long *)(lVar6 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar6 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar5 = *(long *)(lVar5 + 0x10);
                  if (lVar5 != 0) {
                    pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032bfb7:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                        goto LAB_0032bfb7;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
                lVar4 = *(long *)(lVar4 + 0x10);
                if (lVar4 != 0) {
                  pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032db30:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                      goto LAB_0032db30;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                  lVar5 = *(long *)(lVar4 + 8);
                  if (lVar5 != 0) {
                    FUN_002dd9a0(lVar5 + 0x18);
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar4 = *(long *)(lVar4 + 0x10);
                  if (lVar4 != 0) {
                    pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032c105:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                        goto LAB_0032c105;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                    if (*(long *)(lVar4 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar4 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
              }
            }
            lVar21 = *(long *)(lVar21 + 0x10);
            if (lVar21 != 0) {
              pQVar24 = *(QArrayData **)(lVar21 + 0x18);
              if (*(int *)pQVar24 == 0) {
LAB_0032d5c0:
                QArrayData::deallocate(pQVar24,2,8);
              }
              else if (*(int *)pQVar24 != -1) {
                LOCK();
                *(int *)pQVar24 = *(int *)pQVar24 + -1;
                UNLOCK();
                if (*(int *)pQVar24 == 0) {
                  pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                  goto LAB_0032d5c0;
                }
              }
              QVariant::~QVariant((QVariant *)(lVar21 + 0x20));
              lVar4 = *(long *)(lVar21 + 8);
              if (lVar4 != 0) {
                pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                if (*(int *)pQVar24 == 0) {
LAB_0032d7a0:
                  QArrayData::deallocate(pQVar24,2,8);
                }
                else if (*(int *)pQVar24 != -1) {
                  LOCK();
                  *(int *)pQVar24 = *(int *)pQVar24 + -1;
                  UNLOCK();
                  if (*(int *)pQVar24 == 0) {
                    pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                    goto LAB_0032d7a0;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                lVar5 = *(long *)(lVar4 + 8);
                if (lVar5 != 0) {
                  pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032dc08:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                      goto LAB_0032dc08;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                  lVar6 = *(long *)(lVar5 + 8);
                  if (lVar6 != 0) {
                    pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032c235:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar6 + 0x18);
                        goto LAB_0032c235;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar6 + 0x20));
                    if (*(long *)(lVar6 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar6 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar5 = *(long *)(lVar5 + 0x10);
                  if (lVar5 != 0) {
                    pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032c2fd:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                        goto LAB_0032c2fd;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
                lVar4 = *(long *)(lVar4 + 0x10);
                if (lVar4 != 0) {
                  pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032d9a8:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                      goto LAB_0032d9a8;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                  lVar5 = *(long *)(lVar4 + 8);
                  if (lVar5 != 0) {
                    pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032c3e3:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                        goto LAB_0032c3e3;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar4 = *(long *)(lVar4 + 0x10);
                  if (lVar4 != 0) {
                    pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032c47f:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                        goto LAB_0032c47f;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                    if (*(long *)(lVar4 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar4 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
              }
              lVar21 = *(long *)(lVar21 + 0x10);
              if (lVar21 != 0) {
                pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                if (*(int *)pQVar24 == 0) {
LAB_0032d818:
                  QArrayData::deallocate(pQVar24,2,8);
                }
                else if (*(int *)pQVar24 != -1) {
                  LOCK();
                  *(int *)pQVar24 = *(int *)pQVar24 + -1;
                  UNLOCK();
                  if (*(int *)pQVar24 == 0) {
                    pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                    goto LAB_0032d818;
                  }
                }
                QVariant::~QVariant((QVariant *)(lVar21 + 0x20));
                lVar4 = *(long *)(lVar21 + 8);
                if (lVar4 != 0) {
                  pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032da70:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                      goto LAB_0032da70;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                  lVar5 = *(long *)(lVar4 + 8);
                  if (lVar5 != 0) {
                    pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032c566:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar5 + 0x18);
                        goto LAB_0032c566;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar5 + 0x20));
                    if (*(long *)(lVar5 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar5 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar4 = *(long *)(lVar4 + 0x10);
                  if (lVar4 != 0) {
                    pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032c602:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                        goto LAB_0032c602;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                    if (*(long *)(lVar4 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar4 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
                lVar21 = *(long *)(lVar21 + 0x10);
                if (lVar21 != 0) {
                  pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                  if (*(int *)pQVar24 == 0) {
LAB_0032da98:
                    QArrayData::deallocate(pQVar24,2,8);
                  }
                  else if (*(int *)pQVar24 != -1) {
                    LOCK();
                    *(int *)pQVar24 = *(int *)pQVar24 + -1;
                    UNLOCK();
                    if (*(int *)pQVar24 == 0) {
                      pQVar24 = *(QArrayData **)(lVar21 + 0x18);
                      goto LAB_0032da98;
                    }
                  }
                  QVariant::~QVariant((QVariant *)(lVar21 + 0x20));
                  lVar4 = *(long *)(lVar21 + 8);
                  if (lVar4 != 0) {
                    pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                    if (*(int *)pQVar24 == 0) {
LAB_0032c6a5:
                      QArrayData::deallocate(pQVar24,2,8);
                    }
                    else if (*(int *)pQVar24 != -1) {
                      LOCK();
                      *(int *)pQVar24 = *(int *)pQVar24 + -1;
                      UNLOCK();
                      if (*(int *)pQVar24 == 0) {
                        pQVar24 = *(QArrayData **)(lVar4 + 0x18);
                        goto LAB_0032c6a5;
                      }
                    }
                    QVariant::~QVariant((QVariant *)(lVar4 + 0x20));
                    if (*(long *)(lVar4 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar4 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                  lVar21 = *(long *)(lVar21 + 0x10);
                  if (lVar21 != 0) {
                    FUN_002dd9a0(lVar21 + 0x18);
                    QVariant::~QVariant((QVariant *)(lVar21 + 0x20));
                    if (*(long *)(lVar21 + 8) != 0) {
                      FUN_00323e20();
                    }
                    if (*(long *)(lVar21 + 0x10) != 0) {
                      FUN_00323e20();
                    }
                  }
                }
              }
            }
            QMapDataBase::freeTree(pQVar10,(int)*(undefined8 *)(pQVar10 + 0x10));
          }
          QMapDataBase::freeData((QMapDataBase *)pQVar10);
        }
        else if (*(int *)local_50 != -1) {
          LOCK();
          *(int *)local_50 = *(int *)local_50 + -1;
          UNLOCK();
          if (*(int *)local_50 == 0) goto LAB_0032bac0;
        }
                    /* try { // try from 0032ba27 to 0032ba2b has its CatchHandler @ 0032dd2f */
        KisPaintDevice::defaultBounds();
                    /* try { // try from 0032ba32 to 0032ba36 has its CatchHandler @ 0032dd53 */
        KisPaintDevice::setDefaultBounds((KisPaintDevice *)this_00,(KisSharedPtr)&local_d8);
        if (local_d8 != (QString *)0x0) {
          LOCK();
          pQVar24 = (QArrayData *)(local_d8 + 8);
          *(int *)pQVar24 = *(int *)pQVar24 + -1;
          UNLOCK();
          if (*(int *)pQVar24 == 0) {
            (**(code **)(*(long *)local_d8 + 8))();
          }
        }
                    /* try { // try from 0032ba54 to 0032ba92 has its CatchHandler @ 0032dd2f */
        bVar13 = (bool)KisPaintDevice::supportsWraproundMode((KisPaintDevice *)*plVar22);
        KisPaintDevice::setSupportsWraparoundMode((KisPaintDevice *)this_00,bVar13);
        pcVar3 = *(code **)(*(long *)this_00 + 0x60);
        local_d8 = (QString *)KisPaintDevice::offset((KisPaintDevice *)*plVar22);
        (*pcVar3)(this_00,&local_d8);
        FUN_0032e9a0(*(long *)this + 8,&local_e8);
        LOCK();
        pKVar18 = (KisPaintDevice *)(this_00 + 0x10);
        *(int *)pKVar18 = *(int *)pKVar18 + -1;
        UNLOCK();
        if (*(int *)pKVar18 == 0) {
          (**(code **)(*(long *)this_00 + 0x20))(this_00);
          iVar2 = iVar2 + 1;
          if (param_2 == iVar2) break;
          goto LAB_0032b938;
        }
        iVar2 = iVar2 + 1;
      } while (param_2 != iVar2);
    }
  }
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



