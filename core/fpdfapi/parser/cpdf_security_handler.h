// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_SECURITY_HANDLER_H_
#define CORE_FPDFAPI_PARSER_CPDF_SECURITY_HANDLER_H_

#include <memory>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"

#define FXCIPHER_NONE 0
#define FXCIPHER_RC4 1
#define FXCIPHER_AES 2
#define FXCIPHER_AES2 3

#define PDF_ENCRYPT_CONTENT 0

class CPDF_Array;
class CPDF_CryptoHandler;
class CPDF_Dictionary;
class CPDF_Parser;

class CPDF_SecurityHandler {
 public:
  CPDF_SecurityHandler();
  ~CPDF_SecurityHandler();

  bool OnInit(const CPDF_Dictionary* pEncryptDict,
              const CPDF_Array* pIdArray,
              const ByteString& password);
  uint32_t GetPermissions();
  bool IsMetadataEncrypted() const;

  void OnCreate(CPDF_Dictionary* pEncryptDict,
                CPDF_Array* pIdArray,
                const ByteString& user_password,
                const ByteString& owner_password,
                uint32_t type = PDF_ENCRYPT_CONTENT);

  void OnCreate(CPDF_Dictionary* pEncryptDict,
                CPDF_Array* pIdArray,
                const ByteString& user_password,
                uint32_t type = PDF_ENCRYPT_CONTENT);

  ByteString GetUserPassword(const ByteString& owner_password, int32_t key_len);
  bool CheckPassword(const ByteString& user_password,
                     bool bOwner,
                     uint8_t* key,
                     int key_len);

  CPDF_CryptoHandler* GetCryptoHandler() const {
    return m_pCryptoHandler.get();
  }

 private:
  bool LoadDict(const CPDF_Dictionary* pEncryptDict);
  bool LoadDict(const CPDF_Dictionary* pEncryptDict,
                uint32_t type,
                int& cipher,
                int& key_len);

  bool CheckUserPassword(const ByteString& password,
                         bool bIgnoreEncryptMeta,
                         uint8_t* key,
                         int32_t key_len);

  bool CheckOwnerPassword(const ByteString& password,
                          uint8_t* key,
                          int32_t key_len);
  bool AES256_CheckPassword(const ByteString& password,
                            bool bOwner,
                            uint8_t* key);
  void AES256_SetPassword(CPDF_Dictionary* pEncryptDict,
                          const ByteString& password,
                          bool bOwner,
                          const uint8_t* key);
  void AES256_SetPerms(CPDF_Dictionary* pEncryptDict,
                       uint32_t permission,
                       bool bEncryptMetadata,
                       const uint8_t* key);
  void OnCreateInternal(CPDF_Dictionary* pEncryptDict,
                        CPDF_Array* pIdArray,
                        const ByteString& user_password,
                        const ByteString& owner_password,
                        bool bDefault,
                        uint32_t type);
  bool CheckSecurity(const ByteString& password);

  void InitCryptoHandler();

  int m_Version;
  int m_Revision;
  UnownedPtr<const CPDF_Dictionary> m_pEncryptDict;
  ByteString m_FileId;
  uint32_t m_Permissions;
  int m_Cipher;
  uint8_t m_EncryptKey[32];
  int m_KeyLen;
  bool m_bOwnerUnlocked;
  std::unique_ptr<CPDF_CryptoHandler> m_pCryptoHandler;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_SECURITY_HANDLER_H_
