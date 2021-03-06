// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_thisproxy.h"

#include "xfa/fxfa/parser/cxfa_node.h"

CXFA_ThisProxy::CXFA_ThisProxy(CXFA_Node* pThisNode, CXFA_Node* pScriptNode)
    : CXFA_Object(pThisNode->GetDocument(),
                  XFA_ObjectType::VariablesThis,
                  XFA_Element::Unknown,
                  WideStringView()),
      m_pThisNode(nullptr),
      m_pScriptNode(nullptr) {
  m_pThisNode = pThisNode;
  m_pScriptNode = pScriptNode;
}

CXFA_ThisProxy::~CXFA_ThisProxy() {}
