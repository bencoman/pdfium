// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FFDRAW_H_
#define XFA_FXFA_CXFA_FFDRAW_H_

#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"

class CXFA_FFDraw : public CXFA_FFWidget {
 public:
  explicit CXFA_FFDraw(CXFA_WidgetAcc* pDataAcc);
  ~CXFA_FFDraw() override;
};

#endif  // XFA_FXFA_CXFA_FFDRAW_H_
