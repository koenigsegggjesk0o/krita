/* Class KisProofingConfiguration - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisProofingConfiguration @ 00207140 ======

void __thiscall KisProofingConfiguration::KisProofingConfiguration(KisProofingConfiguration *this)

{
  (*(code *)PTR_KisProofingConfiguration_0083b370)();
  return;
}



// ====== KisProofingConfiguration @ 00696190 ======

/* KisProofingConfiguration::KisProofingConfiguration() */

void __thiscall KisProofingConfiguration::KisProofingConfiguration(KisProofingConfiguration *this)

{
  undefined *puVar1;
  QString *pQVar2;
  KoColorSpace *pKVar3;
  undefined8 uVar4;
  long in_FS_OFFSET;
  QColor local_48 [24];
  long local_30;
  
  uVar4 = DAT_0074c0c0;
  local_30 = *(long *)(in_FS_OFFSET + 0x28);
  this[8] = (KisProofingConfiguration)0x1;
  *(undefined4 *)(this + 0xc) = 0x400;
  *(undefined8 *)this = uVar4;
  pQVar2 = (QString *)KoColorSpaceRegistry::instance();
  puVar1 = PTR_shared_null_008377d0;
                    /* try { // try from 006961e4 to 0069620f has its CatchHandler @ 006962a6 */
  pKVar3 = (KoColorSpace *)KoColorSpaceRegistry::rgb8(pQVar2);
  QColor::QColor(local_48,8);
  KoColor::KoColor((KoColor *)(this + 0x10),local_48,pKVar3);
  if (*(int *)puVar1 != 0) {
    if (*(int *)puVar1 == -1) goto LAB_0069622b;
    LOCK();
    *(int *)puVar1 = *(int *)puVar1 + -1;
    UNLOCK();
    if (*(int *)puVar1 != 0) goto LAB_0069622b;
  }
  QArrayData::deallocate((QArrayData *)puVar1,2,8);
LAB_0069622b:
                    /* try { // try from 00696237 to 0069623b has its CatchHandler @ 006962ca */
  uVar4 = QString::fromAscii_helper("Chemical proof",0xe);
  *(undefined8 *)(this + 0x50) = uVar4;
                    /* try { // try from 0069624c to 00696250 has its CatchHandler @ 006962be */
  uVar4 = QString::fromAscii_helper("CMYKA",5);
  *(undefined8 *)(this + 0x58) = uVar4;
                    /* try { // try from 00696261 to 00696265 has its CatchHandler @ 006962b2 */
  uVar4 = QString::fromAscii_helper("U8",2);
  *(undefined8 *)(this + 0x60) = uVar4;
  *(undefined4 *)(this + 0x68) = 1;
  if (local_30 == *(long *)(in_FS_OFFSET + 0x28)) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  __stack_chk_fail();
}



