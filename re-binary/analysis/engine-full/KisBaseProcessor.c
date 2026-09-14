/* Class KisBaseProcessor - decompiled from libkritaimage.so
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// ====== KisBaseProcessor @ 00202010 ======

void __thiscall
KisBaseProcessor::KisBaseProcessor
          (KisBaseProcessor *this,KoID *param_1,KoID *param_2,QString *param_3)

{
  (*(code *)PTR_KisBaseProcessor_00838ad8)();
  return;
}



// ====== KisBaseProcessor @ 004666f0 ======

/* KisBaseProcessor::KisBaseProcessor(KoID const&, KoID const&, QString const&) */

void __thiscall
KisBaseProcessor::KisBaseProcessor
          (KisBaseProcessor *this,KoID *param_1,KoID *param_2,QString *param_3)

{
  undefined8 *puVar1;
  
  KisShared::KisShared((KisShared *)(this + 8));
  *(undefined **)this = PTR_vtable_00837150 + 0x10;
                    /* try { // try from 00466730 to 00466734 has its CatchHandler @ 004667af */
  puVar1 = (undefined8 *)operator_new(0x40);
  *puVar1 = 0;
                    /* try { // try from 00466746 to 0046674a has its CatchHandler @ 004667d3 */
  KoID::KoID((KoID *)(puVar1 + 1));
                    /* try { // try from 0046674f to 00466753 has its CatchHandler @ 004667c7 */
  KoID::KoID((KoID *)(puVar1 + 3));
  puVar1[5] = PTR_shared_null_008377d0;
                    /* try { // try from 00466763 to 00466767 has its CatchHandler @ 004667bb */
  QKeySequence::QKeySequence((QKeySequence *)(puVar1 + 6));
  puVar1[7] = 0x1010100;
  *(undefined8 **)(this + 0x18) = puVar1;
                    /* try { // try from 0046677a to 0046678e has its CatchHandler @ 004667af */
  KoID::operator=((KoID *)(puVar1 + 1),param_1);
  KoID::operator=((KoID *)(*(long *)(this + 0x18) + 0x18),param_2);
  QString::operator=((QString *)(*(long *)(this + 0x18) + 0x28),(QString *)param_3);
  return;
}



