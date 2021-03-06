// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <string>

#include "core/fxcrt/fx_string.h"
#include "public/fpdf_doc.h"
#include "public/fpdf_edit.h"
#include "public/fpdfview.h"
#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

class FPDFDocEmbeddertest : public EmbedderTest {};

TEST_F(FPDFDocEmbeddertest, DestGetPageIndex) {
  EXPECT_TRUE(OpenDocument("named_dests.pdf"));

  // NULL FPDF_DEST case.
  EXPECT_EQ(0U, FPDFDest_GetPageIndex(document(), nullptr));

  // Page number directly in item from Dests NameTree.
  FPDF_DEST dest = FPDF_GetNamedDestByName(document(), "First");
  EXPECT_TRUE(dest);
  EXPECT_EQ(1U, FPDFDest_GetPageIndex(document(), dest));

  // Page number via object reference in item from Dests NameTree.
  dest = FPDF_GetNamedDestByName(document(), "Next");
  EXPECT_TRUE(dest);
  EXPECT_EQ(1U, FPDFDest_GetPageIndex(document(), dest));

  // Page number directly in item from Dests dictionary.
  dest = FPDF_GetNamedDestByName(document(), "FirstAlternate");
  EXPECT_TRUE(dest);
  EXPECT_EQ(11U, FPDFDest_GetPageIndex(document(), dest));

  // Invalid object reference in item from Dests NameTree.
  dest = FPDF_GetNamedDestByName(document(), "LastAlternate");
  EXPECT_TRUE(dest);
  EXPECT_EQ(0U, FPDFDest_GetPageIndex(document(), dest));
}

TEST_F(FPDFDocEmbeddertest, DestGetLocationInPage) {
  EXPECT_TRUE(OpenDocument("named_dests.pdf"));

  // NULL FPDF_DEST case.
  EXPECT_EQ(0U, FPDFDest_GetPageIndex(document(), nullptr));

  FPDF_DEST dest = FPDF_GetNamedDestByName(document(), "First");
  EXPECT_TRUE(dest);

  FPDF_BOOL hasX;
  FPDF_BOOL hasY;
  FPDF_BOOL hasZoom;
  FS_FLOAT x;
  FS_FLOAT y;
  FS_FLOAT zoom;
  EXPECT_TRUE(
      FPDFDest_GetLocationInPage(dest, &hasX, &hasY, &hasZoom, &x, &y, &zoom));
  EXPECT_TRUE(hasX);
  EXPECT_TRUE(hasY);
  EXPECT_TRUE(hasZoom);
  EXPECT_EQ(0, x);
  EXPECT_EQ(0, y);
  EXPECT_EQ(1, zoom);
}

TEST_F(FPDFDocEmbeddertest, BUG_680376) {
  EXPECT_TRUE(OpenDocument("bug_680376.pdf"));

  // Page number directly in item from Dests NameTree.
  FPDF_DEST dest = FPDF_GetNamedDestByName(document(), "First");
  EXPECT_TRUE(dest);
  EXPECT_EQ(static_cast<unsigned long>(-1),
            FPDFDest_GetPageIndex(document(), dest));
}

TEST_F(FPDFDocEmbeddertest, ActionGetFilePath) {
  EXPECT_TRUE(OpenDocument("launch_action.pdf"));

  FPDF_PAGE page = FPDF_LoadPage(document(), 0);
  ASSERT_TRUE(page);

  // The target action is nearly the size of the whole page.
  FPDF_LINK link = FPDFLink_GetLinkAtPoint(page, 100, 100);
  ASSERT_TRUE(link);

  FPDF_ACTION action = FPDFLink_GetAction(link);
  ASSERT_TRUE(action);

  const char kExpectedResult[] = "test.pdf";
  const unsigned long kExpectedLength = sizeof(kExpectedResult);
  unsigned long bufsize = FPDFAction_GetFilePath(action, nullptr, 0);
  ASSERT_EQ(kExpectedLength, bufsize);

  char buf[kExpectedLength];
  EXPECT_EQ(bufsize, FPDFAction_GetFilePath(action, buf, bufsize));
  EXPECT_EQ(std::string(kExpectedResult), std::string(buf));

  FPDF_ClosePage(page);
}

TEST_F(FPDFDocEmbeddertest, NoBookmarks) {
  // Open a file with no bookmarks.
  EXPECT_TRUE(OpenDocument("named_dests.pdf"));

  // The non-existent top-level bookmark has no title.
  unsigned short buf[128];
  EXPECT_EQ(0u, FPDFBookmark_GetTitle(nullptr, buf, sizeof(buf)));

  // The non-existent top-level bookmark has no children.
  EXPECT_EQ(nullptr, FPDFBookmark_GetFirstChild(document(), nullptr));
}

TEST_F(FPDFDocEmbeddertest, Bookmarks) {
  // Open a file with two bookmarks.
  EXPECT_TRUE(OpenDocument("bookmarks.pdf"));

  // The existent top-level bookmark has no title.
  unsigned short buf[128];
  EXPECT_EQ(0u, FPDFBookmark_GetTitle(nullptr, buf, sizeof(buf)));

  FPDF_BOOKMARK child = FPDFBookmark_GetFirstChild(document(), nullptr);
  EXPECT_TRUE(child);
  EXPECT_EQ(34u, FPDFBookmark_GetTitle(child, buf, sizeof(buf)));
  EXPECT_EQ(WideString(L"A Good Beginning"), WideString::FromUTF16LE(buf, 16));

  EXPECT_EQ(nullptr, FPDFBookmark_GetFirstChild(document(), child));

  FPDF_BOOKMARK sibling = FPDFBookmark_GetNextSibling(document(), child);
  EXPECT_TRUE(sibling);
  EXPECT_EQ(28u, FPDFBookmark_GetTitle(sibling, buf, sizeof(buf)));
  EXPECT_EQ(WideString(L"A Good Ending"), WideString::FromUTF16LE(buf, 13));

  EXPECT_EQ(nullptr, FPDFBookmark_GetNextSibling(document(), sibling));
}

TEST_F(FPDFDocEmbeddertest, FindBookmarks) {
  // Open a file with two bookmarks.
  EXPECT_TRUE(OpenDocument("bookmarks.pdf"));

  // Find the first one, based on its known title.
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> title =
      GetFPDFWideString(L"A Good Beginning");
  FPDF_BOOKMARK child = FPDFBookmark_Find(document(), title.get());
  EXPECT_TRUE(child);

  // Check that the string matches.
  unsigned short buf[128];
  EXPECT_EQ(34u, FPDFBookmark_GetTitle(child, buf, sizeof(buf)));
  EXPECT_EQ(WideString(L"A Good Beginning"), WideString::FromUTF16LE(buf, 16));

  // Check that it is them same as the one returned by GetFirstChild.
  EXPECT_EQ(child, FPDFBookmark_GetFirstChild(document(), nullptr));

  // Try to find one using a non-existent title.
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> bad_title =
      GetFPDFWideString(L"A BAD Beginning");
  EXPECT_EQ(nullptr, FPDFBookmark_Find(document(), bad_title.get()));
}

// Check circular bookmarks will not cause infinite loop.
TEST_F(FPDFDocEmbeddertest, FindBookmarks_bug420) {
  // Open a file with circular bookmarks.
  EXPECT_TRUE(OpenDocument("bookmarks_circular.pdf"));

  // Try to find a title.
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> title =
      GetFPDFWideString(L"anything");
  EXPECT_EQ(nullptr, FPDFBookmark_Find(document(), title.get()));
}

TEST_F(FPDFDocEmbeddertest, DeletePage) {
  EXPECT_TRUE(OpenDocument("hello_world.pdf"));
  EXPECT_EQ(1, FPDF_GetPageCount(document()));
  FPDFPage_Delete(document(), 0);
  EXPECT_EQ(0, FPDF_GetPageCount(document()));
}

TEST_F(FPDFDocEmbeddertest, GetMetaText) {
  ASSERT_TRUE(OpenDocument("bug_601362.pdf"));

  // Invalid document / tag results in 0.
  unsigned short buf[128];
  EXPECT_EQ(0u, FPDF_GetMetaText(document(), nullptr, buf, sizeof(buf)));
  EXPECT_EQ(0u, FPDF_GetMetaText(nullptr, "", buf, sizeof(buf)));

  // Tags that do not eixst results in an empty wide string.
  EXPECT_EQ(2u, FPDF_GetMetaText(document(), "", buf, sizeof(buf)));
  EXPECT_EQ(2u, FPDF_GetMetaText(document(), "foo", buf, sizeof(buf)));
  ASSERT_EQ(2u, FPDF_GetMetaText(document(), "Title", buf, sizeof(buf)));
  ASSERT_EQ(2u, FPDF_GetMetaText(document(), "Author", buf, sizeof(buf)));
  ASSERT_EQ(2u, FPDF_GetMetaText(document(), "Subject", buf, sizeof(buf)));
  ASSERT_EQ(2u, FPDF_GetMetaText(document(), "Keywords", buf, sizeof(buf)));
  ASSERT_EQ(2u, FPDF_GetMetaText(document(), "Producer", buf, sizeof(buf)));

  constexpr wchar_t kExpectedCreator[] = L"Microsoft Word";
  ASSERT_EQ(30u, FPDF_GetMetaText(document(), "Creator", buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedCreator),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedCreator)));

  constexpr wchar_t kExpectedCreationDate[] = L"D:20160411190039+00'00'";
  ASSERT_EQ(48u,
            FPDF_GetMetaText(document(), "CreationDate", buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedCreationDate),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedCreationDate)));

  constexpr wchar_t kExpectedModDate[] = L"D:20160411190039+00'00'";
  ASSERT_EQ(48u, FPDF_GetMetaText(document(), "ModDate", buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedModDate),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedModDate)));
}

TEST_F(FPDFDocEmbeddertest, GetMetaTextSameObjectNumber) {
  ASSERT_TRUE(OpenDocument("annotation_highlight_square_with_ap.pdf"));

  // The PDF has been edited. It has two %%EOF markers, and 2 objects numbered
  // (1 0). Both objects are /Info dictionaries, but contain different data.
  // Make sure ModDate is the date of the last modification.
  unsigned short buf[128];
  constexpr wchar_t kExpectedModDate[] = L"D:20170612232940-04'00'";
  ASSERT_EQ(48u, FPDF_GetMetaText(document(), "ModDate", buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedModDate),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedModDate)));
}

TEST_F(FPDFDocEmbeddertest, GetMetaTextInAttachmentFile) {
  ASSERT_TRUE(OpenDocument("embedded_attachments.pdf"));

  // Make sure this is the date from the PDF itself and not the attached PDF.
  unsigned short buf[128];
  constexpr wchar_t kExpectedModDate[] = L"D:20170712214448-07'00'";
  ASSERT_EQ(48u, FPDF_GetMetaText(document(), "ModDate", buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedModDate),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedModDate)));
}

TEST_F(FPDFDocEmbeddertest, NoPageLabels) {
  EXPECT_TRUE(OpenDocument("about_blank.pdf"));
  EXPECT_EQ(1, FPDF_GetPageCount(document()));

  ASSERT_EQ(0u, FPDF_GetPageLabel(document(), 0, nullptr, 0));
}

TEST_F(FPDFDocEmbeddertest, GetPageLabels) {
  EXPECT_TRUE(OpenDocument("page_labels.pdf"));
  EXPECT_EQ(7, FPDF_GetPageCount(document()));

  // We do not request labels, when use FPDFAvail_IsXXXAvail.
  // Flush all data, to allow read labels.
  SetWholeFileAvailable();

  unsigned short buf[128];
  EXPECT_EQ(0u, FPDF_GetPageLabel(document(), -2, buf, sizeof(buf)));
  EXPECT_EQ(0u, FPDF_GetPageLabel(document(), -1, buf, sizeof(buf)));

  const wchar_t kExpectedPageLabel0[] = L"i";
  ASSERT_EQ(4u, FPDF_GetPageLabel(document(), 0, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel0),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel0)));

  const wchar_t kExpectedPageLabel1[] = L"ii";
  ASSERT_EQ(6u, FPDF_GetPageLabel(document(), 1, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel1),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel1)));

  const wchar_t kExpectedPageLabel2[] = L"1";
  ASSERT_EQ(4u, FPDF_GetPageLabel(document(), 2, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel2),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel2)));

  const wchar_t kExpectedPageLabel3[] = L"2";
  ASSERT_EQ(4u, FPDF_GetPageLabel(document(), 3, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel3),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel3)));

  const wchar_t kExpectedPageLabel4[] = L"zzA";
  ASSERT_EQ(8u, FPDF_GetPageLabel(document(), 4, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel4),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel4)));

  const wchar_t kExpectedPageLabel5[] = L"zzB";
  ASSERT_EQ(8u, FPDF_GetPageLabel(document(), 5, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel5),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel5)));

  const wchar_t kExpectedPageLabel6[] = L"";
  ASSERT_EQ(2u, FPDF_GetPageLabel(document(), 6, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel6),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel6)));

  ASSERT_EQ(0u, FPDF_GetPageLabel(document(), 7, buf, sizeof(buf)));
  ASSERT_EQ(0u, FPDF_GetPageLabel(document(), 8, buf, sizeof(buf)));
}
