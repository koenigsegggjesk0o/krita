/* Class KisAnimatedOpacityProperty - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisAnimatedOpacityProperty @ 002085f0 ======

void __thiscall
KisAnimatedOpacityProperty::KisAnimatedOpacityProperty
          (KisAnimatedOpacityProperty *this,KisSharedPtr param_1,KoProperties *param_2,uchar param_3
          ,QObject *param_4)

{
  (*(code *)PTR_KisAnimatedOpacityProperty_0083bdc8)();
  return;
}



// ====== KisAnimatedOpacityProperty @ 0032f070 ======

/* KisAnimatedOpacityProperty::KisAnimatedOpacityProperty(KisSharedPtr<KisDefaultBoundsBase>,
   KoProperties*, unsigned char, QObject*) */

void __thiscall
KisAnimatedOpacityProperty::KisAnimatedOpacityProperty
          (KisAnimatedOpacityProperty *this,KisSharedPtr param_1,KoProperties *param_2,uchar param_3
          ,QObject *param_4)

{
  long lVar1;
  undefined4 in_register_00000034;
  
  QObject::QObject((QObject *)this,param_4);
  *(undefined **)this = PTR_vtable_00837990 + 0x10;
  lVar1 = *(long *)CONCAT44(in_register_00000034,param_1);
  *(long *)(this + 0x10) = lVar1;
  if (lVar1 != 0) {
    LOCK();
    *(int *)(lVar1 + 8) = *(int *)(lVar1 + 8) + 1;
    UNLOCK();
  }
  *(KoProperties **)(this + 0x18) = param_2;
  *(undefined8 *)(this + 0x20) = 0;
  this[0x28] = (KisAnimatedOpacityProperty)param_3;
  return;
}



