// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_IFWL_THEMEPROVIDER_H_
#define XFA_FWL_IFWL_THEMEPROVIDER_H_

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/retain_ptr.h"
#include "xfa/fgas/font/cfgas_gefont.h"

class CFWL_ThemeBackground;
class CFWL_ThemePart;
class CFWL_ThemeText;
class CFWL_Widget;

class IFWL_ThemeProvider {
 public:
  virtual ~IFWL_ThemeProvider() {}

  virtual void DrawBackground(CFWL_ThemeBackground* pParams) = 0;
  virtual void DrawText(CFWL_ThemeText* pParams) = 0;
  virtual void CalcTextRect(CFWL_ThemeText* pParams, CFX_RectF& rect) = 0;
  virtual float GetCXBorderSize() const = 0;
  virtual float GetCYBorderSize() const = 0;
  virtual CFX_RectF GetUIMargin(CFWL_ThemePart* pThemePart) const = 0;
  virtual float GetFontSize(CFWL_ThemePart* pThemePart) const = 0;
  virtual RetainPtr<CFGAS_GEFont> GetFont(CFWL_ThemePart* pThemePart) const = 0;
  virtual float GetLineHeight(CFWL_ThemePart* pThemePart) const = 0;
  virtual float GetScrollBarWidth() const = 0;
  virtual FX_COLORREF GetTextColor(CFWL_ThemePart* pThemePart) const = 0;
  virtual CFX_SizeF GetSpaceAboveBelow(CFWL_ThemePart* pThemePart) const = 0;
};

#endif  // XFA_FWL_IFWL_THEMEPROVIDER_H_
