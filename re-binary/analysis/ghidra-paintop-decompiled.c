/* Ghidra-decompiled pseudocode of Krita paintop (libkritalibpaintop) functions
 * Source binary: libkritalibpaintop.so.20.0.0 (krita-5.3.3 AppImage)
 * SPDX-License-Identifier: GPL-2.0-or-later
 * NOTE: binary is stripped; class names recovered from dynamic symbols.
 */

// ====== KisOpacityOption @ 00197380 ======

void __thiscall
KisOpacityOption::KisOpacityOption
          (KisOpacityOption *this,KisPropertiesConfiguration *param_1,KisSharedPtr param_2)

{
  (*(code *)PTR_KisOpacityOption_004eb028)();
  return;
}



// ====== KisDabCache @ 00197840 ======

void __thiscall KisDabCache::KisDabCache(KisDabCache *this,QSharedPointer param_1)

{
  (*(code *)PTR_KisDabCache_004eb288)();
  return;
}



// ====== KisSensorData @ 00197850 ======

void __thiscall KisSensorData::KisSensorData(KisSensorData *this,KoID *param_1)

{
  (*(code *)PTR_KisSensorData_004eb290)();
  return;
}



// ====== KisSpacingOption @ 001979c0 ======

void __thiscall
KisSpacingOption::KisSpacingOption(KisSpacingOption *this,KisSpacingOptionData *param_1)

{
  (*(code *)PTR_KisSpacingOption_004eb348)();
  return;
}



// ====== KisScatterOption @ 00197cb0 ======

void __thiscall
KisScatterOption::KisScatterOption(KisScatterOption *this,KisScatterOptionData *param_1)

{
  (*(code *)PTR_KisScatterOption_004eb4c0)();
  return;
}



// ====== KisMirrorOption @ 00197e30 ======

void __thiscall KisMirrorOption::KisMirrorOption(KisMirrorOption *this,KisMirrorOptionData *param_1)

{
  (*(code *)PTR_KisMirrorOption_004eb580)();
  return;
}



// ====== KisRotationOption @ 00198320 ======

void __thiscall
KisRotationOption::KisRotationOption(KisRotationOption *this,KisRotationOptionData *param_1)

{
  (*(code *)PTR_KisRotationOption_004eb7f8)();
  return;
}



// ====== KisSensorData @ 00198ac0 ======

void __thiscall KisSensorData::KisSensorData(KisSensorData *this,KoID *param_1)

{
  (*(code *)PTR_KisSensorData_004ebbc8)();
  return;
}



// ====== KisCurveOption @ 00198f10 ======

void __thiscall KisCurveOption::KisCurveOption(KisCurveOption *this,KisCurveOptionData *param_1)

{
  (*(code *)PTR_KisCurveOption_004ebdf0)();
  return;
}



// ====== KisRotationOption @ 001992d0 ======

void __thiscall
KisRotationOption::KisRotationOption(KisRotationOption *this,KisPropertiesConfiguration *param_1)

{
  (*(code *)PTR_KisRotationOption_004ebfd0)();
  return;
}



// ====== KisTextureOption @ 0019a5a0 ======

void __thiscall
KisTextureOption::KisTextureOption
          (KisTextureOption *this,KisPropertiesConfiguration *param_1,QSharedPointer param_2,
          QSharedPointer param_3,int param_4,QFlags param_5)

{
  (*(code *)PTR_KisTextureOption_004ec938)();
  return;
}



// ====== KisMirrorOption @ 0019a6f0 ======

void __thiscall
KisMirrorOption::KisMirrorOption(KisMirrorOption *this,KisPropertiesConfiguration *param_1)

{
  (*(code *)PTR_KisMirrorOption_004ec9e0)();
  return;
}



// ====== KisBrushBasedPaintOp @ 001fe520 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisBrushBasedPaintOp::KisBrushBasedPaintOp(KisPinnedSharedPtr<KisPaintOpSettings>, KisPainter*,
   QFlags<KisBrushTextureFlag>) */

void __thiscall
KisBrushBasedPaintOp::KisBrushBasedPaintOp
          (KisBrushBasedPaintOp *this,KisPinnedSharedPtr param_1,KisPainter *param_2,QFlags param_3)

{
  long *plVar1;
  KisMirrorOption *this_00;
  KisPrecisionOption *this_01;
  int iVar2;
  QString *pQVar3;
  long lVar4;
  undefined8 uVar5;
  int *piVar6;
  QTextStream *pQVar7;
  char cVar8;
  int iVar9;
  QSharedPointer QVar10;
  undefined *puVar11;
  KisDabCacheBase *this_02;
  long lVar12;
  QSharedPointer QVar13;
  undefined8 *puVar14;
  int *piVar15;
  KisBrushBasedPaintOp KVar16;
  undefined4 in_register_00000034;
  long *plVar17;
  long *plVar18;
  long in_FS_OFFSET;
  QTextStream *local_98;
  long local_90;
  long *local_88;
  long *local_80;
  QString local_78 [8];
  int *local_70;
  long *local_68;
  int *piStack_60;
  undefined8 uStack_58;
  char *local_50;
  long local_40;
  
  plVar17 = (long *)CONCAT44(in_register_00000034,param_1);
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  KisPaintOp::KisPaintOp((KisPaintOp *)this,param_2);
  puVar11 = PTR_vtable_004eac58 + 0x10;
  *(undefined (*) [16])(this + 0x28) = (undefined  [16])0x0;
  *(undefined **)this = puVar11;
                    /* try { // try from 001fe58a to 001fe58e has its CatchHandler @ 001fedfb */
  KisPainter::device();
                    /* try { // try from 001fe5a1 to 001fe5a5 has its CatchHandler @ 001fedd7 */
  KisPaintDevice::defaultBounds();
                    /* try { // try from 001fe5ae to 001fe5c4 has its CatchHandler @ 001fedef */
  iVar9 = (**(code **)(*local_88 + 0x30))();
  KisPaintOpSettings::canvasResourcesInterface();
                    /* try { // try from 001fe5d1 to 001fe5d5 has its CatchHandler @ 001fede3 */
  KisPaintOpSettings::resourcesInterface();
  QVar13 = (QSharedPointer)&local_68;
                    /* try { // try from 001fe5ea to 001fe5ee has its CatchHandler @ 001fee4f */
  KisTextureOption::KisTextureOption
            ((KisTextureOption *)(this + 0x38),(KisPropertiesConfiguration *)*plVar17,
             (QSharedPointer)local_78,QVar13,iVar9,param_3);
  piVar15 = local_70;
  if (local_70 != (int *)0x0) {
    LOCK();
    piVar6 = local_70 + 1;
    *piVar6 = *piVar6 + -1;
    UNLOCK();
    if (*piVar6 == 0) {
      (**(code **)(local_70 + 2))(local_70);
    }
    LOCK();
    *piVar15 = *piVar15 + -1;
    UNLOCK();
    if (*piVar15 == 0) {
      operator_delete(piVar15,0x10);
    }
  }
  piVar15 = piStack_60;
  if (piStack_60 != (int *)0x0) {
    LOCK();
    piVar6 = piStack_60 + 1;
    *piVar6 = *piVar6 + -1;
    UNLOCK();
    if (*piVar6 == 0) {
      (**(code **)(piStack_60 + 2))(piStack_60);
    }
    LOCK();
    *piVar15 = *piVar15 + -1;
    UNLOCK();
    if (*piVar15 == 0) {
      operator_delete(piVar15,0x10);
    }
  }
  if (local_88 != (long *)0x0) {
    LOCK();
    plVar18 = local_88 + 1;
    *(int *)plVar18 = *(int *)plVar18 + -1;
    UNLOCK();
    if (*(int *)plVar18 == 0) {
      (**(code **)(*local_88 + 8))();
    }
  }
  if (local_80 != (long *)0x0) {
    LOCK();
    plVar18 = local_80 + 2;
    *(int *)plVar18 = *(int *)plVar18 + -1;
    UNLOCK();
    if (*(int *)plVar18 == 0) {
      (**(code **)(*local_80 + 0x20))();
    }
  }
  this_00 = (KisMirrorOption *)(this + 0x150);
                    /* try { // try from 001fe675 to 001fe679 has its CatchHandler @ 001fee43 */
  KisMirrorOption::KisMirrorOption(this_00,(KisPropertiesConfiguration *)*plVar17);
  this_01 = (KisPrecisionOption *)(this + 400);
                    /* try { // try from 001fe688 to 001fe68c has its CatchHandler @ 001fee07 */
  KisPrecisionOption::KisPrecisionOption(this_01,(KisPropertiesConfiguration *)*plVar17);
  puVar14 = (undefined8 *)0x0;
  if (-2 < DAT_004ed238) {
    if ((DAT_004ed210 == '\0') && (iVar9 = __cxa_guard_acquire(&DAT_004ed210), iVar9 != 0)) {
      DAT_004ed230 = 0;
      _DAT_004ed220 = (undefined  [16])0x0;
      DAT_004ed238 = -1;
      __cxa_atexit(FUN_001fd010,&DAT_004ed220,&PTR_LOOP_004ecfb8);
      __cxa_guard_release(&DAT_004ed210);
    }
    puVar14 = (undefined8 *)&DAT_004ed220;
  }
  plVar18 = (long *)*plVar17;
  if (plVar18 == (long *)0x0) {
    piVar15 = *(int **)(this + 0x30);
    *(undefined (*) [16])(this + 0x28) = (undefined  [16])0x0;
    if (piVar15 != (int *)0x0) goto LAB_001fe6dd;
  }
  else {
    LOCK();
    *(int *)(plVar18 + 1) = *(int *)(plVar18 + 1) + 1;
    UNLOCK();
    if (plVar18 == (long *)puVar14[2]) {
      uVar5 = *puVar14;
      piVar6 = (int *)puVar14[1];
      if (piVar6 == (int *)0x0) {
        piVar15 = *(int **)(this + 0x30);
        *(undefined8 *)(this + 0x28) = uVar5;
        *(undefined8 *)(this + 0x30) = 0;
      }
      else {
        LOCK();
        *piVar6 = *piVar6 + 1;
        UNLOCK();
        LOCK();
        piVar6[1] = piVar6[1] + 1;
        UNLOCK();
        piVar15 = *(int **)(this + 0x30);
        *(undefined8 *)(this + 0x28) = uVar5;
        *(int **)(this + 0x30) = piVar6;
      }
    }
    else {
      piVar15 = *(int **)(this + 0x30);
      *(undefined (*) [16])(this + 0x28) = (undefined  [16])0x0;
    }
    if (piVar15 == (int *)0x0) {
LAB_001fe70a:
      LOCK();
      plVar1 = plVar18 + 1;
      *(int *)plVar1 = *(int *)plVar1 + -1;
      UNLOCK();
      if (*(int *)plVar1 == 0) {
        (**(code **)(*plVar18 + 8))(plVar18);
      }
    }
    else {
LAB_001fe6dd:
      LOCK();
      piVar6 = piVar15 + 1;
      *piVar6 = *piVar6 + -1;
      UNLOCK();
      if (*piVar6 == 0) {
        (**(code **)(piVar15 + 2))();
      }
      LOCK();
      *piVar15 = *piVar15 + -1;
      UNLOCK();
      if (*piVar15 == 0) {
        operator_delete(piVar15,0x10);
      }
      if (plVar18 != (long *)0x0) goto LAB_001fe70a;
    }
    plVar18 = *(long **)(this + 0x28);
    if (plVar18 != (long *)0x0) goto LAB_001fe722;
    if ((*plVar17 != 0) &&
       (lVar12 = __dynamic_cast(*plVar17,PTR_typeinfo_004eaa00,PTR_typeinfo_004eaa90,0), lVar12 != 0
       )) {
      KisBrushBasedPaintOpSettings::brush();
      piVar15 = piStack_60;
      plVar18 = local_68;
      lVar12 = *(long *)(this + 0x30);
      local_68 = (long *)0x0;
      piStack_60 = (int *)0x0;
      *(long **)(this + 0x28) = plVar18;
      *(int **)(this + 0x30) = piVar15;
      if (lVar12 != 0) {
        FUN_001fcf80();
        if (piStack_60 != (int *)0x0) {
          FUN_001fcf80();
        }
        plVar18 = *(long **)(this + 0x28);
      }
      if (plVar18 != (long *)0x0) goto LAB_001fe722;
    }
  }
  local_68 = (long *)0x2;
  local_50 = "default";
  piStack_60 = (int *)0x0;
  uStack_58 = 0;
  QMessageLogger::warning();
  pQVar7 = local_98;
                    /* try { // try from 001fe8ce to 001fe8d2 has its CatchHandler @ 001feda7 */
  QString::fromUtf8_helper((char *)local_78,0x3ef520);
                    /* try { // try from 001fe8db to 001fe8df has its CatchHandler @ 001fed9b */
  QTextStream::operator<<(pQVar7,local_78);
  FUN_001ea4c0(local_78);
  if (local_98[0x20] != (QTextStream)0x0) {
                    /* try { // try from 001fed15 to 001fed19 has its CatchHandler @ 001feda7 */
    QTextStream::operator<<(local_98,' ');
  }
  pQVar3 = (QString *)*plVar17;
  local_80 = (long *)PTR_shared_null_004ead38;
                    /* try { // try from 001fe918 to 001fe91c has its CatchHandler @ 001fedbf */
  local_88 = (long *)QString::fromAscii_helper("brush_definition",0x10);
                    /* try { // try from 001fe93e to 001fe942 has its CatchHandler @ 001fedb3 */
  KisPropertiesConfiguration::getString((QString *)&local_90,pQVar3);
                    /* try { // try from 001fe954 to 001fe980 has its CatchHandler @ 001fed8f */
  QDebug::putString((QChar *)&local_98,local_90 + *(long *)(local_90 + 0x10));
  if (local_98[0x20] != (QTextStream)0x0) {
    QTextStream::operator<<(local_98,' ');
  }
  pQVar7 = local_98;
  QString::fromUtf8_helper((char *)local_78,0x3ef550);
                    /* try { // try from 001fe989 to 001fe98d has its CatchHandler @ 001fed83 */
  QTextStream::operator<<(pQVar7,local_78);
  FUN_001ea4c0(local_78);
  if (local_98[0x20] != (QTextStream)0x0) {
                    /* try { // try from 001fece5 to 001fecfc has its CatchHandler @ 001fed8f */
    QTextStream::operator<<(local_98,' ');
  }
  FUN_001ea4c0((QString *)&local_90);
  FUN_001ea4c0((QString *)&local_88);
  FUN_001ea4c0((QDomElement *)&local_80);
  QDebug::~QDebug((QDebug *)&local_98);
                    /* try { // try from 001fe9d9 to 001fe9dd has its CatchHandler @ 001fee07 */
  local_98 = (QTextStream *)
             QString::fromAscii_helper
                       ("<Brush useAutoSpacing=\"1\" angle=\"0\" spacing=\"0.1\" density=\"1\" BrushVersion=\"2\" type=\"auto_brush\" randomness=\"0\" autoSpacingCoeff=\"0.8\"> <MaskGenerator spikes=\"2\" hfade=\"1\" ratio=\"1\" diameter=\"40\" id=\"default\" type=\"circle\" antialiasEdges=\"1\" vfade=\"1\"/> </Brush> "
                        ,0x107);
                    /* try { // try from 001fe9e8 to 001fe9ec has its CatchHandler @ 001fee37 */
  QDomDocument::QDomDocument((QDomDocument *)&local_90);
                    /* try { // try from 001fe9fe to 001fea13 has its CatchHandler @ 001fee2b */
  QDomDocument::setContent((QString *)&local_90,(QString *)&local_98,(int *)0x0,(int *)0x0);
  local_68 = (long *)QString::fromAscii_helper("Brush",5);
                    /* try { // try from 001fea26 to 001fea2a has its CatchHandler @ 001fee1f */
  QDomNode::firstChildElement((QString *)&local_88);
  FUN_001ea4c0(&local_68);
                    /* try { // try from 001fea33 to 001fea48 has its CatchHandler @ 001fee13 */
  QVar10 = KisBrushRegistry::instance();
  KisPaintOpSettings::resourcesInterface();
                    /* try { // try from 001fea5e to 001fea62 has its CatchHandler @ 001fed77 */
  KisBrushRegistry::createBrush((QDomElement *)&local_80,QVar10);
  KoResourceLoadResult::resource();
  if (local_68 == (long *)0x0) {
    lVar12 = 0;
    piVar15 = (int *)0x0;
LAB_001feae8:
    if (piStack_60 != (int *)0x0) {
      FUN_001fcf80();
    }
  }
  else {
    lVar12 = __dynamic_cast(local_68,PTR_typeinfo_004eaaa8,PTR_typeinfo_004eabe8,0);
    if (lVar12 == 0) {
      piVar15 = (int *)0x0;
      goto LAB_001feae8;
    }
    piVar15 = piStack_60;
    if (piStack_60 != (int *)0x0) {
      piVar6 = piStack_60 + 1;
      iVar9 = piStack_60[1];
      while (0 < iVar9) {
        LOCK();
        iVar2 = *piVar6;
        if (iVar9 == iVar2) {
          *piVar6 = iVar9 + 1;
        }
        UNLOCK();
        if (iVar9 == iVar2) {
          LOCK();
          *piStack_60 = *piStack_60 + 1;
          UNLOCK();
          if (*piVar6 == 0) goto LAB_001feae1;
          goto LAB_001feae8;
        }
        iVar9 = *piVar6;
      }
      piVar15 = (int *)0x0;
LAB_001feae1:
      lVar12 = 0;
      goto LAB_001feae8;
    }
    lVar12 = 0;
  }
  lVar4 = *(long *)(this + 0x30);
  *(long *)(this + 0x28) = lVar12;
  *(int **)(this + 0x30) = piVar15;
  if (lVar4 != 0) {
    FUN_001fcf80(lVar4);
  }
  KoResourceLoadResult::~KoResourceLoadResult((KoResourceLoadResult *)&local_80);
  if (local_70 != (int *)0x0) {
    FUN_001fcf80();
  }
  QDomNode::~QDomNode((QDomNode *)&local_88);
  QDomDocument::~QDomDocument((QDomDocument *)&local_90);
  FUN_001ea4c0((QChar *)&local_98);
  plVar18 = *(long **)(this + 0x28);
LAB_001fe722:
                    /* try { // try from 001fe725 to 001fe734 has its CatchHandler @ 001fee07 */
  (**(code **)(*plVar18 + 0xe8))();
  this_02 = (KisDabCacheBase *)operator_new(0x10);
  local_68 = *(long **)(this + 0x28);
  piStack_60 = *(int **)(this + 0x30);
  piVar15 = *(int **)(this + 0x30);
  if (piVar15 != (int *)0x0) {
    LOCK();
    *piVar15 = *piVar15 + 1;
    UNLOCK();
    LOCK();
    piStack_60[1] = piStack_60[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 001fe75f to 001fe763 has its CatchHandler @ 001fedcb */
  KisDabCache::KisDabCache((KisDabCache *)this_02,QVar13);
  piVar15 = piStack_60;
  *(KisDabCacheBase **)(this + 0x20) = this_02;
  if (piStack_60 != (int *)0x0) {
    LOCK();
    piVar6 = piStack_60 + 1;
    *piVar6 = *piVar6 + -1;
    UNLOCK();
    if (*piVar6 == 0) {
      (**(code **)(piStack_60 + 2))(piStack_60);
    }
    LOCK();
    *piVar15 = *piVar15 + -1;
    UNLOCK();
    if (*piVar15 == 0) {
      operator_delete(piVar15,0x10);
    }
    this_02 = *(KisDabCacheBase **)(this + 0x20);
  }
                    /* try { // try from 001fe797 to 001fe8b4 has its CatchHandler @ 001fee07 */
  KisDabCacheBase::setPrecisionOption(this_02,this_01);
  KisDabCacheBase::setMirrorPostprocessing(*(KisDabCacheBase **)(this + 0x20),this_00);
  KisDabCache::setTexturePostprocessing
            (*(KisDabCache **)(this + 0x20),(KisTextureOption *)(this + 0x38));
  cVar8 = KisPrecisionOption::hasImprecisePositionOptions(this_01);
                    /* try { // try from 001feb53 to 001fec45 has its CatchHandler @ 001fee07 */
  if ((cVar8 == '\0') &&
     (cVar8 = KisCurveOption::isChecked((KisCurveOption *)this_00), cVar8 == '\0')) {
    KVar16 = this[0x38];
  }
  else {
    KVar16 = (KisBrushBasedPaintOp)0x1;
  }
  KisPrecisionOption::setHasImprecisePositionOptions(this_01,(bool)KVar16);
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSensorData @ 002687e0 ======

/* KisSensorData::KisSensorData(KoID const&) */

void __thiscall KisSensorData::KisSensorData(KisSensorData *this,KoID *param_1)

{
  int *piVar1;
  
  *(undefined **)this = PTR_vtable_004eaec8 + 0x10;
  KoID::KoID((KoID *)(this + 8),param_1);
  piVar1 = DAT_004ed890;
  *(int **)(this + 0x18) = DAT_004ed890;
  if (*piVar1 + 1U < 2) {
    this[0x20] = (KisSensorData)0x0;
    return;
  }
  LOCK();
  *piVar1 = *piVar1 + 1;
  UNLOCK();
  this[0x20] = (KisSensorData)0x0;
  return;
}



// ====== KisCurveOption @ 002a28c0 ======

/* KisCurveOption::KisCurveOption(KisCurveOptionData const&) */

void __thiscall KisCurveOption::KisCurveOption(KisCurveOption *this,KisCurveOptionData *param_1)

{
  undefined8 uVar1;
  undefined8 uVar2;
  undefined4 uVar3;
  long lVar4;
  long in_FS_OFFSET;
  
  uVar1 = *(undefined8 *)(param_1 + 0x40);
  lVar4 = *(long *)(in_FS_OFFSET + 0x28);
  uVar2 = *(undefined8 *)(param_1 + 0x20);
  *(undefined2 *)this = *(undefined2 *)(param_1 + 0x30);
  uVar3 = *(undefined4 *)(param_1 + 0x34);
  *(undefined8 *)(this + 8) = uVar1;
  *(undefined8 *)(this + 0x10) = uVar2;
  uVar1 = *(undefined8 *)(param_1 + 0x28);
  *(undefined4 *)(this + 4) = uVar3;
  *(undefined8 *)(this + 0x18) = uVar1;
  FUN_002a1110();
  if (lVar4 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisSpacingOption @ 002bff50 ======

/* KisSpacingOption::KisSpacingOption(KisSpacingOptionData const&) */

void __thiscall
KisSpacingOption::KisSpacingOption(KisSpacingOption *this,KisSpacingOptionData *param_1)

{
  KisCurveOption::KisCurveOption((KisCurveOption *)this,(KisCurveOptionData *)param_1);
  *(ushort *)(this + 0x38) = CONCAT11(param_1[0x90],param_1[0x91]);
  return;
}



// ====== KisSpacingOption @ 002bfff0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisSpacingOption::KisSpacingOption(KisPropertiesConfiguration const*) */

void __thiscall
KisSpacingOption::KisSpacingOption(KisSpacingOption *this,KisPropertiesConfiguration *param_1)

{
  int *piVar1;
  long *plVar2;
  char cVar3;
  int iVar4;
  long in_FS_OFFSET;
  QArrayData *local_130;
  QArrayData *local_128;
  QArrayData *local_120;
  KoID local_118 [8];
  int *local_110;
  undefined8 local_108;
  undefined8 uStack_100;
  KisCurveOptionData local_e8 [8];
  int *local_e0;
  QArrayData *local_d8;
  QArrayData *local_b0;
  long *local_a0;
  undefined local_98 [16];
  code *local_88;
  undefined local_78 [16];
  code *local_68;
  undefined2 local_58 [4];
  QArrayData *local_50 [2];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_130 = (QArrayData *)QString::fromAscii_helper("",0);
                    /* try { // try from 002c004b to 002c004f has its CatchHandler @ 002c0488 */
  ki18nd((char *)&local_108,"krita");
                    /* try { // try from 002c005b to 002c005f has its CatchHandler @ 002c047c */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_108);
                    /* try { // try from 002c0070 to 002c0074 has its CatchHandler @ 002c046b */
  local_128 = (QArrayData *)QString::fromAscii_helper("Spacing",7);
                    /* try { // try from 002c0092 to 002c0096 has its CatchHandler @ 002c045a */
  KoID::KoID(local_118,(QString *)&local_128,(QString *)&local_120);
  local_108 = _DAT_003ef500;
  uStack_100 = DAT_003ef508;
                    /* try { // try from 002c00c2 to 002c00c6 has its CatchHandler @ 002c04ad */
  KisCurveOptionData::KisCurveOptionData
            (local_e8,(QString *)&local_130,local_118,1,0,(pair *)&local_108);
  local_58[0] = 0;
  local_50[0] = local_130;
  if (1 < *(int *)local_130 + 1U) {
    LOCK();
    *(int *)local_130 = *(int *)local_130 + 1;
    UNLOCK();
  }
  if (local_110 == (int *)0x0) {
LAB_002c0111:
    iVar4 = *(int *)local_128;
    if (iVar4 == 0) goto LAB_002c0338;
LAB_002c0120:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_128 = *(int *)local_128 + -1;
      UNLOCK();
      if (*(int *)local_128 == 0) goto LAB_002c0338;
    }
    iVar4 = *(int *)local_120;
    if (iVar4 != 0) goto LAB_002c0143;
LAB_002c0356:
    QArrayData::deallocate(local_120,2,8);
    iVar4 = *(int *)local_130;
    if (iVar4 == 0) goto LAB_002c0374;
LAB_002c0166:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_130 = *(int *)local_130 + -1;
      UNLOCK();
      if (*(int *)local_130 == 0) goto LAB_002c0374;
    }
  }
  else {
    LOCK();
    piVar1 = local_110 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_110 + 2))(local_110);
    }
    LOCK();
    *local_110 = *local_110 + -1;
    UNLOCK();
    if (*local_110 != 0) goto LAB_002c0111;
    operator_delete(local_110,0x10);
    iVar4 = *(int *)local_128;
    if (iVar4 != 0) goto LAB_002c0120;
LAB_002c0338:
    QArrayData::deallocate(local_128,2,8);
    iVar4 = *(int *)local_120;
    if (iVar4 == 0) goto LAB_002c0356;
LAB_002c0143:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_120 = *(int *)local_120 + -1;
      UNLOCK();
      if (*(int *)local_120 == 0) goto LAB_002c0356;
    }
    iVar4 = *(int *)local_130;
    if (iVar4 != 0) goto LAB_002c0166;
LAB_002c0374:
    QArrayData::deallocate(local_130,2,8);
  }
                    /* try { // try from 002c0180 to 002c0184 has its CatchHandler @ 002c0499 */
  cVar3 = KisCurveOptionDataCommon::read((KisCurveOptionDataCommon *)local_e8,param_1);
  if ((cVar3 != '\0') && (param_1 != (KisPropertiesConfiguration *)0x0)) {
    if (*(int *)(local_50[0] + 4) == 0) {
                    /* try { // try from 002c0433 to 002c0437 has its CatchHandler @ 002c0499 */
      KisSpacingOptionMixInImpl::read((KisSpacingOptionMixInImpl *)local_58,param_1);
    }
    else {
                    /* try { // try from 002c03db to 002c03df has its CatchHandler @ 002c0499 */
      KisPropertiesConfiguration::KisPropertiesConfiguration
                ((KisPropertiesConfiguration *)&local_108);
                    /* try { // try from 002c03f1 to 002c0405 has its CatchHandler @ 002c0442 */
      KisPropertiesConfiguration::getPrefixedProperties
                ((QString *)param_1,(KisPropertiesConfiguration *)local_50);
      KisSpacingOptionMixInImpl::read
                ((KisSpacingOptionMixInImpl *)local_58,(KisPropertiesConfiguration *)&local_108);
      KisPropertiesConfiguration::~KisPropertiesConfiguration
                ((KisPropertiesConfiguration *)&local_108);
    }
  }
                    /* try { // try from 002c0194 to 002c0198 has its CatchHandler @ 002c044e */
  KisSpacingOption(this,(KisSpacingOptionData *)local_e8);
  if (*(int *)local_50[0] == 0) {
LAB_002c03a8:
    QArrayData::deallocate(local_50[0],2,8);
  }
  else if (*(int *)local_50[0] != -1) {
    LOCK();
    *(int *)local_50[0] = *(int *)local_50[0] + -1;
    UNLOCK();
    if (*(int *)local_50[0] == 0) goto LAB_002c03a8;
  }
  if (local_68 != (code *)0x0) {
    (*local_68)(local_78,local_78,3);
  }
  if (local_88 != (code *)0x0) {
    (*local_88)(local_98,local_98,3);
  }
  if (local_a0 == (long *)0x0) {
LAB_002c0218:
    iVar4 = *(int *)local_b0;
    if (iVar4 == 0) goto LAB_002c02e0;
LAB_002c022a:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_b0 = *(int *)local_b0 + -1;
      UNLOCK();
      if (*(int *)local_b0 == 0) goto LAB_002c02e0;
    }
    iVar4 = *(int *)local_d8;
  }
  else {
    LOCK();
    plVar2 = local_a0 + 1;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if ((*(int *)plVar2 != 0) || (local_a0 == (long *)0x0)) goto LAB_002c0218;
    (**(code **)(*local_a0 + 8))();
    iVar4 = *(int *)local_b0;
    if (iVar4 != 0) goto LAB_002c022a;
LAB_002c02e0:
    QArrayData::deallocate(local_b0,2,8);
    iVar4 = *(int *)local_d8;
  }
  if (iVar4 != 0) {
    if (iVar4 == -1) goto LAB_002c0264;
    LOCK();
    *(int *)local_d8 = *(int *)local_d8 + -1;
    UNLOCK();
    if (*(int *)local_d8 != 0) goto LAB_002c0264;
  }
  QArrayData::deallocate(local_d8,2,8);
LAB_002c0264:
  if (local_e0 != (int *)0x0) {
    LOCK();
    piVar1 = local_e0 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_e0 + 2))(local_e0);
    }
    LOCK();
    *local_e0 = *local_e0 + -1;
    UNLOCK();
    if (*local_e0 == 0) {
      operator_delete(local_e0,0x10);
    }
  }
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



// ====== KisMirrorOption @ 002cd950 ======

/* KisMirrorOption::KisMirrorOption(KisMirrorOptionData const&) */

void __thiscall KisMirrorOption::KisMirrorOption(KisMirrorOption *this,KisMirrorOptionData *param_1)

{
  KisCurveOption::KisCurveOption((KisCurveOption *)this,(KisCurveOptionData *)param_1);
  *(ushort *)(this + 0x38) = CONCAT11(param_1[0x90],param_1[0x91]);
  return;
}



// ====== KisMirrorOption @ 002cda30 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisMirrorOption::KisMirrorOption(KisPropertiesConfiguration const*) */

void __thiscall
KisMirrorOption::KisMirrorOption(KisMirrorOption *this,KisPropertiesConfiguration *param_1)

{
  int *piVar1;
  long *plVar2;
  char cVar3;
  int iVar4;
  long in_FS_OFFSET;
  QArrayData *local_130;
  QArrayData *local_128;
  QArrayData *local_120;
  KoID local_118 [8];
  int *local_110;
  undefined8 local_108;
  undefined8 uStack_100;
  KisCurveOptionData local_e8 [8];
  int *local_e0;
  QArrayData *local_d8;
  QArrayData *local_b0;
  long *local_a0;
  undefined local_98 [16];
  code *local_88;
  undefined local_78 [16];
  code *local_68;
  undefined2 local_58 [4];
  QArrayData *local_50 [2];
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_130 = (QArrayData *)QString::fromAscii_helper("",0);
                    /* try { // try from 002cda8b to 002cda8f has its CatchHandler @ 002cdec8 */
  ki18nd((char *)&local_108,"krita");
                    /* try { // try from 002cda9b to 002cda9f has its CatchHandler @ 002cdebc */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_108);
                    /* try { // try from 002cdab0 to 002cdab4 has its CatchHandler @ 002cdeab */
  local_128 = (QArrayData *)QString::fromAscii_helper("Mirror",6);
                    /* try { // try from 002cdad2 to 002cdad6 has its CatchHandler @ 002cde9a */
  KoID::KoID(local_118,(QString *)&local_128,(QString *)&local_120);
  local_108 = _DAT_003ef500;
  uStack_100 = DAT_003ef508;
                    /* try { // try from 002cdb02 to 002cdb06 has its CatchHandler @ 002cdeed */
  KisCurveOptionData::KisCurveOptionData
            (local_e8,(QString *)&local_130,local_118,1,0,(pair *)&local_108);
  local_58[0] = 0;
  local_50[0] = local_130;
  if (1 < *(int *)local_130 + 1U) {
    LOCK();
    *(int *)local_130 = *(int *)local_130 + 1;
    UNLOCK();
  }
  if (local_110 == (int *)0x0) {
LAB_002cdb51:
    iVar4 = *(int *)local_128;
    if (iVar4 == 0) goto LAB_002cdd78;
LAB_002cdb60:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_128 = *(int *)local_128 + -1;
      UNLOCK();
      if (*(int *)local_128 == 0) goto LAB_002cdd78;
    }
    iVar4 = *(int *)local_120;
    if (iVar4 != 0) goto LAB_002cdb83;
LAB_002cdd96:
    QArrayData::deallocate(local_120,2,8);
    iVar4 = *(int *)local_130;
    if (iVar4 == 0) goto LAB_002cddb4;
LAB_002cdba6:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_130 = *(int *)local_130 + -1;
      UNLOCK();
      if (*(int *)local_130 == 0) goto LAB_002cddb4;
    }
  }
  else {
    LOCK();
    piVar1 = local_110 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_110 + 2))(local_110);
    }
    LOCK();
    *local_110 = *local_110 + -1;
    UNLOCK();
    if (*local_110 != 0) goto LAB_002cdb51;
    operator_delete(local_110,0x10);
    iVar4 = *(int *)local_128;
    if (iVar4 != 0) goto LAB_002cdb60;
LAB_002cdd78:
    QArrayData::deallocate(local_128,2,8);
    iVar4 = *(int *)local_120;
    if (iVar4 == 0) goto LAB_002cdd96;
LAB_002cdb83:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_120 = *(int *)local_120 + -1;
      UNLOCK();
      if (*(int *)local_120 == 0) goto LAB_002cdd96;
    }
    iVar4 = *(int *)local_130;
    if (iVar4 != 0) goto LAB_002cdba6;
LAB_002cddb4:
    QArrayData::deallocate(local_130,2,8);
  }
                    /* try { // try from 002cdbc0 to 002cdbc4 has its CatchHandler @ 002cded9 */
  cVar3 = KisCurveOptionDataCommon::read((KisCurveOptionDataCommon *)local_e8,param_1);
  if ((cVar3 != '\0') && (param_1 != (KisPropertiesConfiguration *)0x0)) {
    if (*(int *)(local_50[0] + 4) == 0) {
                    /* try { // try from 002cde73 to 002cde77 has its CatchHandler @ 002cded9 */
      KisMirrorOptionMixInImpl::read((KisMirrorOptionMixInImpl *)local_58,param_1);
    }
    else {
                    /* try { // try from 002cde1b to 002cde1f has its CatchHandler @ 002cded9 */
      KisPropertiesConfiguration::KisPropertiesConfiguration
                ((KisPropertiesConfiguration *)&local_108);
                    /* try { // try from 002cde31 to 002cde45 has its CatchHandler @ 002cde82 */
      KisPropertiesConfiguration::getPrefixedProperties
                ((QString *)param_1,(KisPropertiesConfiguration *)local_50);
      KisMirrorOptionMixInImpl::read
                ((KisMirrorOptionMixInImpl *)local_58,(KisPropertiesConfiguration *)&local_108);
      KisPropertiesConfiguration::~KisPropertiesConfiguration
                ((KisPropertiesConfiguration *)&local_108);
    }
  }
                    /* try { // try from 002cdbd4 to 002cdbd8 has its CatchHandler @ 002cde8e */
  KisMirrorOption(this,(KisMirrorOptionData *)local_e8);
  if (*(int *)local_50[0] == 0) {
LAB_002cdde8:
    QArrayData::deallocate(local_50[0],2,8);
  }
  else if (*(int *)local_50[0] != -1) {
    LOCK();
    *(int *)local_50[0] = *(int *)local_50[0] + -1;
    UNLOCK();
    if (*(int *)local_50[0] == 0) goto LAB_002cdde8;
  }
  if (local_68 != (code *)0x0) {
    (*local_68)(local_78,local_78,3);
  }
  if (local_88 != (code *)0x0) {
    (*local_88)(local_98,local_98,3);
  }
  if (local_a0 == (long *)0x0) {
LAB_002cdc58:
    iVar4 = *(int *)local_b0;
    if (iVar4 == 0) goto LAB_002cdd20;
LAB_002cdc6a:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_b0 = *(int *)local_b0 + -1;
      UNLOCK();
      if (*(int *)local_b0 == 0) goto LAB_002cdd20;
    }
    iVar4 = *(int *)local_d8;
  }
  else {
    LOCK();
    plVar2 = local_a0 + 1;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if ((*(int *)plVar2 != 0) || (local_a0 == (long *)0x0)) goto LAB_002cdc58;
    (**(code **)(*local_a0 + 8))();
    iVar4 = *(int *)local_b0;
    if (iVar4 != 0) goto LAB_002cdc6a;
LAB_002cdd20:
    QArrayData::deallocate(local_b0,2,8);
    iVar4 = *(int *)local_d8;
  }
  if (iVar4 != 0) {
    if (iVar4 == -1) goto LAB_002cdca4;
    LOCK();
    *(int *)local_d8 = *(int *)local_d8 + -1;
    UNLOCK();
    if (*(int *)local_d8 != 0) goto LAB_002cdca4;
  }
  QArrayData::deallocate(local_d8,2,8);
LAB_002cdca4:
  if (local_e0 != (int *)0x0) {
    LOCK();
    piVar1 = local_e0 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_e0 + 2))(local_e0);
    }
    LOCK();
    *local_e0 = *local_e0 + -1;
    UNLOCK();
    if (*local_e0 == 0) {
      operator_delete(local_e0,0x10);
    }
  }
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



// ====== KisScatterOption @ 002ec4c0 ======

/* KisScatterOption::KisScatterOption(KisScatterOptionData const&) */

void __thiscall
KisScatterOption::KisScatterOption(KisScatterOption *this,KisScatterOptionData *param_1)

{
  KisCurveOption::KisCurveOption((KisCurveOption *)this,(KisCurveOptionData *)param_1);
  *(undefined2 *)(this + 0x38) = *(undefined2 *)(param_1 + 0x90);
  return;
}



// ====== KisScatterOption @ 002ec770 ======

/* KisScatterOption::KisScatterOption(KisPropertiesConfiguration const*) */

void __thiscall
KisScatterOption::KisScatterOption(KisScatterOption *this,KisPropertiesConfiguration *param_1)

{
  long *plVar1;
  int *piVar2;
  char cVar3;
  int iVar4;
  long in_FS_OFFSET;
  QArrayData *local_f8 [4];
  KisScatterOptionData local_d8 [8];
  int *local_d0;
  QArrayData *local_c8;
  QArrayData *local_a0;
  long *local_90;
  undefined local_88 [16];
  code *local_78;
  undefined local_68 [16];
  code *local_58;
  KisScatterOptionMixInImpl local_48 [8];
  QArrayData *local_40 [2];
  long local_30;
  
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  local_f8[0] = (QArrayData *)QString::fromAscii_helper("",0);
                    /* try { // try from 002ec7bc to 002ec7c0 has its CatchHandler @ 002eca56 */
  KisScatterOptionData::KisScatterOptionData(local_d8,(QString *)local_f8);
  if (*(int *)local_f8[0] == 0) {
LAB_002ec988:
    QArrayData::deallocate(local_f8[0],2,8);
  }
  else if (*(int *)local_f8[0] != -1) {
    LOCK();
    *(int *)local_f8[0] = *(int *)local_f8[0] + -1;
    UNLOCK();
    if (*(int *)local_f8[0] == 0) goto LAB_002ec988;
  }
                    /* try { // try from 002ec7e8 to 002ec7ec has its CatchHandler @ 002eca42 */
  cVar3 = KisCurveOptionDataCommon::read((KisCurveOptionDataCommon *)local_d8,param_1);
  if ((cVar3 != '\0') && (param_1 != (KisPropertiesConfiguration *)0x0)) {
    if (*(int *)(local_40[0] + 4) == 0) {
                    /* try { // try from 002eca1b to 002eca1f has its CatchHandler @ 002eca42 */
      KisScatterOptionMixInImpl::read(local_48,param_1);
    }
    else {
                    /* try { // try from 002ec9d3 to 002ec9d7 has its CatchHandler @ 002eca42 */
      KisPropertiesConfiguration::KisPropertiesConfiguration((KisPropertiesConfiguration *)local_f8)
      ;
                    /* try { // try from 002ec9e9 to 002ec9fd has its CatchHandler @ 002eca2a */
      KisPropertiesConfiguration::getPrefixedProperties
                ((QString *)param_1,(KisPropertiesConfiguration *)local_40);
      KisScatterOptionMixInImpl::read(local_48,(KisPropertiesConfiguration *)local_f8);
      KisPropertiesConfiguration::~KisPropertiesConfiguration
                ((KisPropertiesConfiguration *)local_f8);
    }
  }
                    /* try { // try from 002ec7fb to 002ec7ff has its CatchHandler @ 002eca36 */
  KisScatterOption(this,local_d8);
  if (*(int *)local_40[0] == 0) {
LAB_002ec9a0:
    QArrayData::deallocate(local_40[0],2,8);
  }
  else if (*(int *)local_40[0] != -1) {
    LOCK();
    *(int *)local_40[0] = *(int *)local_40[0] + -1;
    UNLOCK();
    if (*(int *)local_40[0] == 0) goto LAB_002ec9a0;
  }
  if (local_58 != (code *)0x0) {
    (*local_58)(local_68,local_68,3);
  }
  if (local_78 != (code *)0x0) {
    (*local_78)(local_88,local_88,3);
  }
  if (local_90 == (long *)0x0) {
LAB_002ec879:
    iVar4 = *(int *)local_a0;
    if (iVar4 == 0) goto LAB_002ec938;
LAB_002ec888:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_a0 = *(int *)local_a0 + -1;
      UNLOCK();
      if (*(int *)local_a0 == 0) goto LAB_002ec938;
    }
    iVar4 = *(int *)local_c8;
  }
  else {
    LOCK();
    plVar1 = local_90 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if ((*(int *)plVar1 != 0) || (local_90 == (long *)0x0)) goto LAB_002ec879;
    (**(code **)(*local_90 + 8))();
    iVar4 = *(int *)local_a0;
    if (iVar4 != 0) goto LAB_002ec888;
LAB_002ec938:
    QArrayData::deallocate(local_a0,2,8);
    iVar4 = *(int *)local_c8;
  }
  if (iVar4 != 0) {
    if (iVar4 == -1) goto LAB_002ec8bf;
    LOCK();
    *(int *)local_c8 = *(int *)local_c8 + -1;
    UNLOCK();
    if (*(int *)local_c8 != 0) goto LAB_002ec8bf;
  }
  QArrayData::deallocate(local_c8,2,8);
LAB_002ec8bf:
  if (local_d0 != (int *)0x0) {
    LOCK();
    piVar2 = local_d0 + 1;
    *piVar2 = *piVar2 + -1;
    UNLOCK();
    if (*piVar2 == 0) {
      (**(code **)(local_d0 + 2))(local_d0);
    }
    LOCK();
    *local_d0 = *local_d0 + -1;
    UNLOCK();
    if (*local_d0 == 0) {
      operator_delete(local_d0,0x10);
    }
  }
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



// ====== KisRotationOption @ 0035ea40 ======

/* KisRotationOption::KisRotationOption(KisRotationOptionData const&) */

void __thiscall
KisRotationOption::KisRotationOption(KisRotationOption *this,KisRotationOptionData *param_1)

{
  undefined8 uVar1;
  KisRotationOption KVar2;
  long lVar3;
  
  KisCurveOption::KisCurveOption((KisCurveOption *)this,(KisCurveOptionData *)param_1);
  uVar1 = DAT_004442c0;
  this[0x38] = (KisRotationOption)0x0;
  *(undefined8 *)(this + 0x40) = uVar1;
                    /* try { // try from 0035ea6b to 0035eacf has its CatchHandler @ 0035eadc */
  lVar3 = KisCurveOptionData::sensorStruct((KisCurveOptionData *)param_1);
  if (*(char *)(lVar3 + 0x138) == '\0') {
    return;
  }
  lVar3 = KisCurveOptionData::sensorStruct((KisCurveOptionData *)param_1);
  KVar2 = *(KisRotationOption *)(lVar3 + 0x139);
  if (KVar2 != (KisRotationOption)0x0) {
    lVar3 = KisCurveOptionData::sensorStruct((KisCurveOptionData *)param_1);
    KVar2 = (KisRotationOption)(*(byte *)(lVar3 + 0x148) ^ 1);
  }
  this[0x38] = KVar2;
  lVar3 = KisCurveOptionData::sensorStruct((KisCurveOptionData *)param_1);
  *(double *)(this + 0x40) = (double)*(int *)(lVar3 + 0x13c);
  return;
}



// ====== KisRotationOption @ 0035eaf0 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisRotationOption::KisRotationOption(KisPropertiesConfiguration const*) */

void __thiscall
KisRotationOption::KisRotationOption(KisRotationOption *this,KisPropertiesConfiguration *param_1)

{
  int *piVar1;
  long *plVar2;
  int iVar3;
  long in_FS_OFFSET;
  QArrayData *local_110;
  QArrayData *local_108;
  QArrayData *local_100;
  KLocalizedString local_f8 [8];
  int *local_f0;
  undefined8 local_e8;
  undefined8 uStack_e0;
  KisCurveOptionData local_d8 [8];
  int *local_d0;
  QArrayData *local_c8;
  QArrayData *local_a0;
  long *local_90;
  undefined local_88 [16];
  code *local_78;
  undefined local_68 [16];
  code *local_58;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_e8 = _DAT_003ef500;
  uStack_e0 = DAT_003ef508;
  local_110 = (QArrayData *)PTR_shared_null_004ead38;
                    /* try { // try from 0035eb56 to 0035eb5a has its CatchHandler @ 0035ee99 */
  ki18nd((char *)local_f8,"krita");
                    /* try { // try from 0035eb66 to 0035eb6a has its CatchHandler @ 0035ee75 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString(local_f8);
                    /* try { // try from 0035eb7b to 0035eb7f has its CatchHandler @ 0035ee64 */
  local_108 = (QArrayData *)QString::fromAscii_helper("Rotation",8);
                    /* try { // try from 0035eb93 to 0035eb97 has its CatchHandler @ 0035ee53 */
  KoID::KoID((KoID *)local_f8,(QString *)&local_108,(QString *)&local_100);
                    /* try { // try from 0035ebb8 to 0035ebbc has its CatchHandler @ 0035ee47 */
  KisCurveOptionData::KisCurveOptionData
            (local_d8,(QString *)&local_110,(KoID *)local_f8,1,0,(pair *)&local_e8);
  if (local_f0 == (int *)0x0) {
LAB_0035ebde:
    iVar3 = *(int *)local_108;
    if (iVar3 == 0) goto LAB_0035edd8;
LAB_0035ebed:
    if (iVar3 != -1) {
      LOCK();
      *(int *)local_108 = *(int *)local_108 + -1;
      UNLOCK();
      if (*(int *)local_108 == 0) goto LAB_0035edd8;
    }
    iVar3 = *(int *)local_100;
    if (iVar3 != 0) goto LAB_0035ec10;
LAB_0035edf6:
    QArrayData::deallocate(local_100,2,8);
    iVar3 = *(int *)local_110;
    if (iVar3 == 0) goto LAB_0035ee14;
LAB_0035ec33:
    if (iVar3 != -1) {
      LOCK();
      *(int *)local_110 = *(int *)local_110 + -1;
      UNLOCK();
      if (*(int *)local_110 == 0) goto LAB_0035ee14;
    }
  }
  else {
    LOCK();
    piVar1 = local_f0 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_f0 + 2))(local_f0);
    }
    LOCK();
    *local_f0 = *local_f0 + -1;
    UNLOCK();
    if (*local_f0 != 0) goto LAB_0035ebde;
    operator_delete(local_f0,0x10);
    iVar3 = *(int *)local_108;
    if (iVar3 != 0) goto LAB_0035ebed;
LAB_0035edd8:
    QArrayData::deallocate(local_108,2,8);
    iVar3 = *(int *)local_100;
    if (iVar3 == 0) goto LAB_0035edf6;
LAB_0035ec10:
    if (iVar3 != -1) {
      LOCK();
      *(int *)local_100 = *(int *)local_100 + -1;
      UNLOCK();
      if (*(int *)local_100 == 0) goto LAB_0035edf6;
    }
    iVar3 = *(int *)local_110;
    if (iVar3 != 0) goto LAB_0035ec33;
LAB_0035ee14:
    QArrayData::deallocate(local_110,2,8);
  }
                    /* try { // try from 0035ec4f to 0035ec53 has its CatchHandler @ 0035ee8d */
  KisCurveOptionDataCommon::read((KisCurveOptionDataCommon *)local_d8,param_1);
                    /* try { // try from 0035ec5b to 0035ec5f has its CatchHandler @ 0035ee81 */
  KisRotationOption(this,(KisRotationOptionData *)local_d8);
  if (local_58 != (code *)0x0) {
    (*local_58)(local_68,local_68,3);
  }
  if (local_78 != (code *)0x0) {
    (*local_78)(local_88,local_88,3);
  }
  if (local_90 == (long *)0x0) {
LAB_0035ecb6:
    iVar3 = *(int *)local_a0;
    if (iVar3 == 0) goto LAB_0035ed80;
LAB_0035ecc8:
    if (iVar3 != -1) {
      LOCK();
      *(int *)local_a0 = *(int *)local_a0 + -1;
      UNLOCK();
      if (*(int *)local_a0 == 0) goto LAB_0035ed80;
    }
    iVar3 = *(int *)local_c8;
  }
  else {
    LOCK();
    plVar2 = local_90 + 1;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if ((*(int *)plVar2 != 0) || (local_90 == (long *)0x0)) goto LAB_0035ecb6;
    (**(code **)(*local_90 + 8))();
    iVar3 = *(int *)local_a0;
    if (iVar3 != 0) goto LAB_0035ecc8;
LAB_0035ed80:
    QArrayData::deallocate(local_a0,2,8);
    iVar3 = *(int *)local_c8;
  }
  if (iVar3 != 0) {
    if (iVar3 == -1) goto LAB_0035ed02;
    LOCK();
    *(int *)local_c8 = *(int *)local_c8 + -1;
    UNLOCK();
    if (*(int *)local_c8 != 0) goto LAB_0035ed02;
  }
  QArrayData::deallocate(local_c8,2,8);
LAB_0035ed02:
  if (local_d0 != (int *)0x0) {
    LOCK();
    piVar1 = local_d0 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_d0 + 2))(local_d0);
    }
    LOCK();
    *local_d0 = *local_d0 + -1;
    UNLOCK();
    if (*local_d0 == 0) {
      operator_delete(local_d0,0x10);
    }
  }
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



// ====== KisOpacityOption @ 0035f080 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisOpacityOption::KisOpacityOption(KisPropertiesConfiguration const*, KisSharedPtr<KisNode>) */

void __thiscall
KisOpacityOption::KisOpacityOption
          (KisOpacityOption *this,KisPropertiesConfiguration *param_1,KisSharedPtr param_2)

{
  int *piVar1;
  long *plVar2;
  KisOpacityOption KVar3;
  int iVar4;
  int iVar5;
  long lVar6;
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  QArrayData *local_110;
  QArrayData *local_108;
  QArrayData *local_100;
  QArrayData *local_f8;
  int *local_f0;
  QArrayData *local_e8;
  undefined8 uStack_e0;
  KisCurveOptionData local_d8 [8];
  int *local_d0;
  QArrayData *local_c8;
  QArrayData *local_a0;
  long *local_90;
  undefined local_88 [16];
  code *local_78;
  undefined local_68 [16];
  code *local_58;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  local_e8 = _DAT_003ef500;
  uStack_e0 = DAT_003ef508;
  local_110 = (QArrayData *)PTR_shared_null_004ead38;
                    /* try { // try from 0035f0e7 to 0035f0eb has its CatchHandler @ 0035f59f */
  ki18nd((char *)&local_f8,"krita");
                    /* try { // try from 0035f0f7 to 0035f0fb has its CatchHandler @ 0035f571 */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_f8);
                    /* try { // try from 0035f10c to 0035f110 has its CatchHandler @ 0035f57d */
  local_108 = (QArrayData *)QString::fromAscii_helper("Opacity",7);
                    /* try { // try from 0035f129 to 0035f12d has its CatchHandler @ 0035f55b */
  KoID::KoID((KoID *)&local_f8,(QString *)&local_108,(QString *)&local_100);
                    /* try { // try from 0035f155 to 0035f159 has its CatchHandler @ 0035f543 */
  KisCurveOptionData::KisCurveOptionData
            (local_d8,(QString *)&local_110,(KoID *)&local_f8,0,0,(pair *)&local_e8);
  if (local_f0 == (int *)0x0) {
LAB_0035f17e:
    iVar4 = *(int *)local_108;
    if (iVar4 == 0) goto LAB_0035f470;
LAB_0035f18d:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_108 = *(int *)local_108 + -1;
      UNLOCK();
      if (*(int *)local_108 == 0) goto LAB_0035f470;
    }
    iVar4 = *(int *)local_100;
    if (iVar4 != 0) goto LAB_0035f1b0;
LAB_0035f48e:
    QArrayData::deallocate(local_100,2,8);
    iVar4 = *(int *)local_110;
    if (iVar4 == 0) goto LAB_0035f4ac;
LAB_0035f1d3:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_110 = *(int *)local_110 + -1;
      UNLOCK();
      if (*(int *)local_110 == 0) goto LAB_0035f4ac;
    }
  }
  else {
    LOCK();
    piVar1 = local_f0 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_f0 + 2))(local_f0);
    }
    LOCK();
    *local_f0 = *local_f0 + -1;
    UNLOCK();
    if (*local_f0 != 0) goto LAB_0035f17e;
    operator_delete(local_f0,0x10);
    iVar4 = *(int *)local_108;
    if (iVar4 != 0) goto LAB_0035f18d;
LAB_0035f470:
    QArrayData::deallocate(local_108,2,8);
    iVar4 = *(int *)local_100;
    if (iVar4 == 0) goto LAB_0035f48e;
LAB_0035f1b0:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_100 = *(int *)local_100 + -1;
      UNLOCK();
      if (*(int *)local_100 == 0) goto LAB_0035f48e;
    }
    iVar4 = *(int *)local_110;
    if (iVar4 != 0) goto LAB_0035f1d3;
LAB_0035f4ac:
    QArrayData::deallocate(local_110,2,8);
  }
                    /* try { // try from 0035f1ee to 0035f1f2 has its CatchHandler @ 0035f54f */
  KisCurveOptionDataCommon::read((KisCurveOptionDataCommon *)local_d8,param_1);
                    /* try { // try from 0035f1f9 to 0035f1fd has its CatchHandler @ 0035f593 */
  KisCurveOption::KisCurveOption((KisCurveOption *)this,local_d8);
  if (local_58 != (code *)0x0) {
    (*local_58)(local_68,local_68,3);
  }
  if (local_78 != (code *)0x0) {
    (*local_78)(local_88,local_88,3);
  }
  if (local_90 == (long *)0x0) {
LAB_0035f254:
    iVar4 = *(int *)local_a0;
    if (iVar4 == 0) goto LAB_0035f418;
LAB_0035f266:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_a0 = *(int *)local_a0 + -1;
      UNLOCK();
      if (*(int *)local_a0 == 0) goto LAB_0035f418;
    }
    iVar4 = *(int *)local_c8;
    if (iVar4 != 0) goto LAB_0035f28c;
LAB_0035f436:
    QArrayData::deallocate(local_c8,2,8);
  }
  else {
    LOCK();
    plVar2 = local_90 + 1;
    *(int *)plVar2 = *(int *)plVar2 + -1;
    UNLOCK();
    if ((*(int *)plVar2 != 0) || (local_90 == (long *)0x0)) goto LAB_0035f254;
    (**(code **)(*local_90 + 8))();
    iVar4 = *(int *)local_a0;
    if (iVar4 != 0) goto LAB_0035f266;
LAB_0035f418:
    QArrayData::deallocate(local_a0,2,8);
    iVar4 = *(int *)local_c8;
    if (iVar4 == 0) goto LAB_0035f436;
LAB_0035f28c:
    if (iVar4 != -1) {
      LOCK();
      *(int *)local_c8 = *(int *)local_c8 + -1;
      UNLOCK();
      if (*(int *)local_c8 == 0) goto LAB_0035f436;
    }
  }
  if (local_d0 != (int *)0x0) {
    LOCK();
    piVar1 = local_d0 + 1;
    *piVar1 = *piVar1 + -1;
    UNLOCK();
    if (*piVar1 == 0) {
      (**(code **)(local_d0 + 2))(local_d0);
    }
    LOCK();
    *local_d0 = *local_d0 + -1;
    UNLOCK();
    if (*local_d0 == 0) {
      operator_delete(local_d0,0x10);
    }
  }
  lVar6 = *(long *)CONCAT44(in_register_00000014,param_2);
  this[0x38] = (KisOpacityOption)0x0;
  if (lVar6 == 0) goto LAB_0035f3b8;
  local_e8 = (QArrayData *)PTR_shared_null_004ead38;
                    /* try { // try from 0035f2e3 to 0035f2e7 has its CatchHandler @ 0035f537 */
  KisPropertiesConfiguration::extractedPrefixKey();
                    /* try { // try from 0035f2f7 to 0035f2fb has its CatchHandler @ 0035f52b */
  KisPropertiesConfiguration::getString((QString *)&local_100,(QString *)param_1);
  iVar4 = *(int *)(local_100 + 4);
  if (*(int *)local_100 == 0) {
LAB_0035f320:
    QArrayData::deallocate(local_100,2,8);
  }
  else if (*(int *)local_100 != -1) {
    LOCK();
    *(int *)local_100 = *(int *)local_100 + -1;
    UNLOCK();
    if (*(int *)local_100 == 0) goto LAB_0035f320;
  }
  if (*(int *)local_f8 == 0) {
LAB_0035f4e8:
    QArrayData::deallocate(local_f8,2,8);
    iVar5 = *(int *)local_e8;
    if (iVar5 != 0) goto LAB_0035f361;
LAB_0035f506:
    QArrayData::deallocate(local_e8,2,8);
  }
  else {
    if (*(int *)local_f8 != -1) {
      LOCK();
      *(int *)local_f8 = *(int *)local_f8 + -1;
      UNLOCK();
      if (*(int *)local_f8 == 0) goto LAB_0035f4e8;
    }
    iVar5 = *(int *)local_e8;
    if (iVar5 == 0) goto LAB_0035f506;
LAB_0035f361:
    if (iVar5 != -1) {
      LOCK();
      *(int *)local_e8 = *(int *)local_e8 + -1;
      UNLOCK();
      if (*(int *)local_e8 == 0) goto LAB_0035f506;
    }
  }
  if (iVar4 == 0) {
    lVar6 = *(long *)CONCAT44(in_register_00000014,param_2);
    if ((lVar6 == 0) ||
       (lVar6 = __dynamic_cast(lVar6,PTR_typeinfo_004eaec0,PTR_typeinfo_004eafa0,0xfffffffffffffffe)
       , lVar6 == 0)) {
      this[0x38] = (KisOpacityOption)0x0;
    }
    else {
                    /* try { // try from 0035f3ac to 0035f3b0 has its CatchHandler @ 0035f51f */
      KVar3 = (KisOpacityOption)KisIndirectPaintingSupport::hasTemporaryTarget();
      this[0x38] = KVar3;
    }
  }
LAB_0035f3b8:
  if (local_40 != *(long *)(in_FS_OFFSET + 0x28)) {
                    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return;
}



// ====== KisDabCache @ 0037dc10 ======

/* KisDabCache::KisDabCache(QSharedPointer<KisBrush>) */

void __thiscall KisDabCache::KisDabCache(KisDabCache *this,QSharedPointer param_1)

{
  int *piVar1;
  undefined8 uVar2;
  int *piVar3;
  undefined4 *puVar4;
  undefined4 in_register_00000034;
  
  KisDabCacheBase::KisDabCacheBase((KisDabCacheBase *)this);
                    /* try { // try from 0037dc2a to 0037dc2e has its CatchHandler @ 0037dcee */
  puVar4 = (undefined4 *)operator_new(0x40);
  uVar2 = *(undefined8 *)CONCAT44(in_register_00000034,param_1);
  piVar3 = (int *)((undefined8 *)CONCAT44(in_register_00000034,param_1))[1];
  if (piVar3 == (int *)0x0) {
    *puVar4 = 0;
    *(undefined8 *)(puVar4 + 8) = 0;
    *(undefined8 *)(puVar4 + 10) = 0;
    *(undefined8 *)(puVar4 + 0xc) = 0;
    *(undefined8 *)(puVar4 + 0xe) = 0;
    *(undefined4 **)(this + 8) = puVar4;
    *(undefined (*) [16])(puVar4 + 2) = (undefined  [16])0x0;
    *(undefined8 *)(puVar4 + 6) = uVar2;
    return;
  }
  LOCK();
  *piVar3 = *piVar3 + 1;
  UNLOCK();
  piVar1 = piVar3 + 1;
  LOCK();
  piVar3[1] = piVar3[1] + 1;
  UNLOCK();
  *puVar4 = 0;
  *(undefined (*) [16])(puVar4 + 2) = (undefined  [16])0x0;
  *(undefined8 *)(puVar4 + 6) = uVar2;
  *(int **)(puVar4 + 8) = piVar3;
  LOCK();
  *piVar3 = *piVar3 + 1;
  UNLOCK();
  LOCK();
  *(int *)(*(long *)(puVar4 + 8) + 4) = *(int *)(*(long *)(puVar4 + 8) + 4) + 1;
  UNLOCK();
  *(undefined8 *)(puVar4 + 0xe) = 0;
  *(undefined4 **)(this + 8) = puVar4;
  *(undefined (*) [16])(puVar4 + 10) = (undefined  [16])0x0;
  LOCK();
  *piVar1 = *piVar1 + -1;
  UNLOCK();
  if (*piVar1 == 0) {
    (**(code **)(piVar3 + 2))(piVar3);
  }
  LOCK();
  *piVar3 = *piVar3 + -1;
  UNLOCK();
  if (*piVar3 != 0) {
    return;
  }
  operator_delete(piVar3,0x10);
  return;
}



// ====== KisTextureOption @ 0038ee60 ======

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* KisTextureOption::KisTextureOption(KisPropertiesConfiguration const*,
   QSharedPointer<KisResourcesInterface>, QSharedPointer<KoCanvasResourcesInterface>, int,
   QFlags<KisBrushTextureFlag>) */

void __thiscall
KisTextureOption::KisTextureOption
          (KisTextureOption *this,KisPropertiesConfiguration *param_1,QSharedPointer param_2,
          QSharedPointer param_3,int param_4,QFlags param_5)

{
  long *plVar1;
  undefined *puVar2;
  undefined4 in_register_0000000c;
  undefined4 in_register_00000014;
  long in_FS_OFFSET;
  undefined8 local_108;
  QString local_100 [8];
  undefined8 local_f8;
  int *piStack_f0;
  undefined8 local_e8;
  int *piStack_e0;
  KisCurveOptionData local_d8 [8];
  long local_d0;
  undefined local_c8 [40];
  undefined local_a0 [16];
  long *local_90;
  undefined local_88 [16];
  code *local_78;
  undefined local_68 [16];
  code *local_58;
  long local_40;
  
  local_40 = *(long *)(in_FS_OFFSET + 0x28);
  *this = (KisTextureOption)0x0;
  *(undefined8 *)(this + 4) = 0;
  *(undefined2 *)(this + 0xc) = 0;
  *(undefined4 *)(this + 0x10) = 0;
  this[0x14] = (KisTextureOption)0x0;
  *(undefined (*) [16])(this + 0x18) = (undefined  [16])0x0;
                    /* try { // try from 0038eed8 to 0038eedc has its CatchHandler @ 0038f1dd */
  local_e8 = QString::fromAscii_helper("",0);
                    /* try { // try from 0038eeed to 0038eef1 has its CatchHandler @ 0038f1d1 */
  KoAbstractGradient::KoAbstractGradient((KoAbstractGradient *)(this + 0x28),(QString *)&local_e8);
  FUN_001ea4c0((QString *)&local_e8);
  *(undefined4 *)(this + 0x58) = 0;
  *(undefined ***)(this + 0x28) = &PTR_FUN_004d24c0;
  puVar2 = PTR_shared_null_004ead38;
  *(undefined8 *)(this + 0x50) = 0;
  *(undefined **)(this + 0x60) = puVar2;
  *(undefined (*) [16])(this + 0x40) = (undefined  [16])0x0;
                    /* try { // try from 0038ef2b to 0038ef2f has its CatchHandler @ 0038f1c5 */
  KoColor::KoColor((KoColor *)(this + 0x68));
  *(int *)(this + 0xa8) = param_4;
  local_e8 = _DAT_003ef500;
  piStack_e0 = (int *)DAT_003ef508;
                    /* try { // try from 0038ef6a to 0038ef6e has its CatchHandler @ 0038f1b9 */
  ki18nd((char *)&local_f8,"krita");
                    /* try { // try from 0038ef7a to 0038ef7e has its CatchHandler @ 0038f1ad */
  KLocalizedString::toString();
  KLocalizedString::~KLocalizedString((KLocalizedString *)&local_f8);
                    /* try { // try from 0038ef93 to 0038ef97 has its CatchHandler @ 0038f1a1 */
  local_108 = QString::fromAscii_helper("Texture/Strength/",0x11);
                    /* try { // try from 0038efad to 0038efb1 has its CatchHandler @ 0038f195 */
  KoID::KoID((KoID *)&local_f8,(QString *)&local_108,local_100);
                    /* try { // try from 0038efc8 to 0038efcc has its CatchHandler @ 0038f189 */
  KisCurveOptionData::KisCurveOptionData(local_d8,(KoID *)&local_f8,1,0,(pair *)&local_e8);
  if (piStack_f0 != (int *)0x0) {
    FUN_00389350();
  }
  FUN_001ea4c0((QString *)&local_108);
  FUN_001ea4c0(local_100);
                    /* try { // try from 0038eff6 to 0038effa has its CatchHandler @ 0038f17d */
  KisCurveOptionDataCommon::read((KisCurveOptionDataCommon *)local_d8,param_1);
                    /* try { // try from 0038f003 to 0038f007 has its CatchHandler @ 0038f171 */
  KisCurveOption::KisCurveOption((KisCurveOption *)(this + 0xb0),local_d8);
  if (local_58 != (code *)0x0) {
    (*local_58)(local_68,local_68,3);
  }
  if (local_78 != (code *)0x0) {
    (*local_78)(local_88,local_88,3);
  }
  if (local_90 != (long *)0x0) {
    LOCK();
    plVar1 = local_90 + 1;
    *(int *)plVar1 = *(int *)plVar1 + -1;
    UNLOCK();
    if ((*(int *)plVar1 == 0) && (local_90 != (long *)0x0)) {
      (**(code **)(*local_90 + 8))();
    }
  }
  FUN_001ea4c0(local_a0);
  FUN_001ea4c0(local_c8);
  if (local_d0 != 0) {
    FUN_00389350();
  }
  *(undefined8 *)(this + 0x110) = 0;
  *(undefined (*) [16])(this + 0xe8) = (undefined  [16])0x0;
  *(QFlags *)(this + 0xf8) = param_5;
  *(undefined (*) [16])(this + 0x100) = (undefined  [16])0x0;
  local_e8 = *(undefined8 *)CONCAT44(in_register_0000000c,param_3);
  piStack_e0 = (int *)((undefined8 *)CONCAT44(in_register_0000000c,param_3))[1];
  if (piStack_e0 != (int *)0x0) {
    LOCK();
    *piStack_e0 = *piStack_e0 + 1;
    UNLOCK();
    LOCK();
    piStack_e0[1] = piStack_e0[1] + 1;
    UNLOCK();
  }
  local_f8 = *(undefined8 *)CONCAT44(in_register_00000014,param_2);
  piStack_f0 = (int *)((undefined8 *)CONCAT44(in_register_00000014,param_2))[1];
  if (piStack_f0 != (int *)0x0) {
    LOCK();
    *piStack_f0 = *piStack_f0 + 1;
    UNLOCK();
    LOCK();
    piStack_f0[1] = piStack_f0[1] + 1;
    UNLOCK();
  }
                    /* try { // try from 0038f108 to 0038f10c has its CatchHandler @ 0038f1e9 */
  fillProperties(this,param_1,(QSharedPointer)(KLocalizedString *)&local_f8,
                 (QSharedPointer)(QString *)&local_e8);
  if (piStack_f0 != (int *)0x0) {
    FUN_00389350();
  }
  if (piStack_e0 != (int *)0x0) {
    FUN_00389350();
  }
  if (local_40 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



