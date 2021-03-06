// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_binditems.h"

#include "xfa/fxfa/parser/cxfa_node.h"

CXFA_BindItems::CXFA_BindItems(CXFA_Node* pNode) : CXFA_Data(pNode) {}

void CXFA_BindItems::GetLabelRef(WideStringView& wsLabelRef) {
  m_pNode->TryCData(XFA_ATTRIBUTE_LabelRef, wsLabelRef);
}

void CXFA_BindItems::GetValueRef(WideStringView& wsValueRef) {
  m_pNode->TryCData(XFA_ATTRIBUTE_ValueRef, wsValueRef);
}

void CXFA_BindItems::GetRef(WideStringView& wsRef) {
  m_pNode->TryCData(XFA_ATTRIBUTE_Ref, wsRef);
}

bool CXFA_BindItems::SetConnection(const WideString& wsConnection) {
  return m_pNode->SetCData(XFA_ATTRIBUTE_Connection, wsConnection);
}
