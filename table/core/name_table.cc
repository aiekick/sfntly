/*
 * Copyright 2011 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sfntly/table/core/name_table.h"

#include <stdio.h>
#include <string>
#include <codecvt>
//#include <unicode/unistr.h>

#include "sfntly/font.h"
#include "sfntly/port/exception_type.h"

namespace sfntly {
/******************************************************************************
 * NameTable::NameEntryId class
 ******************************************************************************/
NameTable::NameEntryId::NameEntryId()
    : platform_id_(0),
      encoding_id_(0),
      language_id_(0),
      name_id_(0) {
}

NameTable::NameEntryId::NameEntryId(int32_t platform_id,
                                    int32_t encoding_id,
                                    int32_t language_id,
                                    int32_t name_id)
    : platform_id_(platform_id),
      encoding_id_(encoding_id),
      language_id_(language_id),
      name_id_(name_id) {
}

NameTable::NameEntryId::NameEntryId(const NameTable::NameEntryId& rhs) {
  *this = rhs;
}

const NameTable::NameEntryId&
    NameTable::NameEntryId::operator=(const NameTable::NameEntryId& rhs) const {
  platform_id_ = rhs.platform_id_;
  encoding_id_ = rhs.encoding_id_;
  language_id_ = rhs.language_id_;
  name_id_ = rhs.name_id_;
  return *this;
}

bool NameTable::NameEntryId::operator==(const NameEntryId& rhs) const {
  return platform_id_ == rhs.platform_id_ &&
         encoding_id_ == rhs.encoding_id_ &&
         language_id_ == rhs.language_id_ &&
         name_id_ == rhs.name_id_;
}

bool NameTable::NameEntryId::operator<(const NameEntryId& rhs) const {
  if (platform_id_ != rhs.platform_id_) return platform_id_ < rhs.platform_id_;
  if (encoding_id_ != rhs.encoding_id_) return encoding_id_ < rhs.encoding_id_;
  if (language_id_ != rhs.language_id_) return language_id_ < rhs.language_id_;
  return name_id_ < rhs.name_id_;
}

/******************************************************************************
 * NameTable::NameEntry class
 ******************************************************************************/
NameTable::NameEntry::NameEntry() {
  Init(0, 0, 0, 0, NULL);
}

NameTable::NameEntry::NameEntry(const NameEntryId& name_entry_id,
                                const std::vector<uint8_t>& name_bytes) {
  Init(name_entry_id.platform_id(),
       name_entry_id.encoding_id(),
       name_entry_id.language_id(),
       name_entry_id.name_id(),
       &name_bytes);
}

NameTable::NameEntry::NameEntry(int32_t platform_id,
                                int32_t encoding_id,
                                int32_t language_id,
                                int32_t name_id,
                                const std::vector<uint8_t>& name_bytes) {
  Init(platform_id, encoding_id, language_id, name_id, &name_bytes);
}

NameTable::NameEntry::~NameEntry() {}

std::vector<uint8_t>* NameTable::NameEntry::NameAsBytes() {
  return &name_bytes_;
}

int32_t NameTable::NameEntry::NameBytesLength() {
  return (int32_t)name_bytes_.size();
}

wchar_t* NameTable::NameEntry::Name() {
  return NameTable::ConvertFromNameBytes(&name_bytes_,
                                         platform_id(),
                                         encoding_id());
}

bool NameTable::NameEntry::operator==(const NameEntry& rhs) const {
  return (name_entry_id_ == rhs.name_entry_id_ &&
          name_bytes_ == rhs.name_bytes_);
}

void NameTable::NameEntry::Init(int32_t platform_id,
                                int32_t encoding_id,
                                int32_t language_id,
                                int32_t name_id,
                                const std::vector<uint8_t>* name_bytes) {
  name_entry_id_ = NameEntryId(platform_id, encoding_id, language_id, name_id);
  if (name_bytes) {
    name_bytes_ = *name_bytes;
  } else {
    name_bytes_.clear();
  }
}

/******************************************************************************
 * NameTable::NameEntryBuilder class
 ******************************************************************************/
NameTable::NameEntryBuilder::NameEntryBuilder() {
  Init(0, 0, 0, 0, NULL);
}

NameTable::NameEntryBuilder::NameEntryBuilder(const NameEntryId& name_entry_id,
                                              const std::vector<uint8_t>& name_bytes) {
  Init(name_entry_id.platform_id(),
       name_entry_id.encoding_id(),
       name_entry_id.language_id(),
       name_entry_id.name_id(),
       &name_bytes);
}

NameTable::NameEntryBuilder::NameEntryBuilder(
    const NameEntryId& name_entry_id) {
  Init(name_entry_id.platform_id(),
       name_entry_id.encoding_id(),
       name_entry_id.language_id(),
       name_entry_id.name_id(),
       NULL);
}

NameTable::NameEntryBuilder::NameEntryBuilder(NameEntry* b) {
  Init(b->platform_id(),
       b->encoding_id(),
       b->language_id(),
       b->name_id(),
       b->NameAsBytes());
}

NameTable::NameEntryBuilder::~NameEntryBuilder() {}

void NameTable::NameEntryBuilder::SetName(const wchar_t* name) {
  if (name == NULL) {
    name_entry_->name_bytes_.clear();
    return;
  }
  NameTable::ConvertToNameBytes(name,
                                name_entry_->platform_id(),
                                name_entry_->encoding_id(),
                                &name_entry_->name_bytes_);
}

void NameTable::NameEntryBuilder::SetName(const std::vector<uint8_t>& name_bytes) {
  name_entry_->name_bytes_.clear();
  std::copy(name_bytes.begin(),
            name_bytes.end(),
            name_entry_->name_bytes_.begin());
}

void NameTable::NameEntryBuilder::SetName(const std::vector<uint8_t>& name_bytes,
                                          int32_t offset,
                                          int32_t length) {
  name_entry_->name_bytes_.clear();
  std::copy(name_bytes.begin() + offset,
            name_bytes.begin() + offset + length,
            name_entry_->name_bytes_.begin());
}

void NameTable::NameEntryBuilder::Init(int32_t platform_id,
                                       int32_t encoding_id,
                                       int32_t language_id,
                                       int32_t name_id,
                                       const std::vector<uint8_t>* name_bytes) {
  name_entry_ = new NameEntry();
  name_entry_->Init(platform_id, encoding_id, language_id, name_id, name_bytes);
}

/******************************************************************************
 * NameTable::NameEntryFilterInPlace class (C++ port only)
 ******************************************************************************/
NameTable::NameEntryFilterInPlace::NameEntryFilterInPlace(int32_t platform_id,
                                                          int32_t encoding_id,
                                                          int32_t language_id,
                                                          int32_t name_id)
    : platform_id_(platform_id),
      encoding_id_(encoding_id),
      language_id_(language_id),
      name_id_(name_id) {
}

bool NameTable::NameEntryFilterInPlace::Accept(int32_t platform_id,
                                               int32_t encoding_id,
                                               int32_t language_id,
                                               int32_t name_id) {
  return (platform_id_ == platform_id &&
          encoding_id_ == encoding_id &&
          language_id_ == language_id &&
          name_id_ == name_id);
}

/******************************************************************************
 * NameTable::NameEntryIterator class
 ******************************************************************************/
NameTable::NameEntryIterator::NameEntryIterator(NameTable* table)
    : RefIterator<NameEntry, NameTable>(table),
      name_index_(0),
      filter_(NULL) {
}

NameTable::NameEntryIterator::NameEntryIterator(NameTable* table,
                                                NameEntryFilter* filter)
    : RefIterator<NameEntry, NameTable>(table),
      name_index_(0),
      filter_(filter) {
}

bool NameTable::NameEntryIterator::HasNext() {
  if (!filter_) {
    if (name_index_ < container()->NameCount()) {
      return true;
    }
    return false;
  }
  for (; name_index_ < container()->NameCount(); ++name_index_) {
    if (filter_->Accept(container()->PlatformId(name_index_),
                        container()->EncodingId(name_index_),
                        container()->LanguageId(name_index_),
                        container()->NameId(name_index_))) {
      return true;
    }
  }
  return false;
}

CALLER_ATTACH NameTable::NameEntry* NameTable::NameEntryIterator::Next() {
  if (!HasNext())
    return NULL;
  return container()->GetNameEntry(name_index_++);
}

/******************************************************************************
 * NameTable::Builder class
 ******************************************************************************/
NameTable::Builder::Builder(Header* header, WritableFontData* data)
    : SubTableContainerTable::Builder(header, data) {
}

NameTable::Builder::Builder(Header* header, ReadableFontData* data)
    : SubTableContainerTable::Builder(header, data) {
}

CALLER_ATTACH NameTable::Builder*
    NameTable::Builder::CreateBuilder(Header* header,
                                      WritableFontData* data) {
  Ptr<NameTable::Builder> builder;
  builder = new NameTable::Builder(header, data);
  return builder.Detach();
}

void NameTable::Builder::RevertNames() {
  name_entry_map_.clear();
  set_model_changed(false);
}

int32_t NameTable::Builder::BuilderCount() {
  GetNameBuilders();  // Ensure name_entry_map_ is built.
  return (int32_t)name_entry_map_.size();
}

bool NameTable::Builder::Has(int32_t platform_id,
                             int32_t encoding_id,
                             int32_t language_id,
                             int32_t name_id) {
  NameEntryId probe(platform_id, encoding_id, language_id, name_id);
  GetNameBuilders();  // Ensure name_entry_map_ is built.
  return (name_entry_map_.find(probe) != name_entry_map_.end());
}

CALLER_ATTACH NameTable::NameEntryBuilder*
    NameTable::Builder::NameBuilder(int32_t platform_id,
                                    int32_t encoding_id,
                                    int32_t language_id,
                                    int32_t name_id) {
  NameEntryId probe(platform_id, encoding_id, language_id, name_id);
  NameEntryBuilderMap builders;
  GetNameBuilders();  // Ensure name_entry_map_ is built.
  if (name_entry_map_.find(probe) != name_entry_map_.end()) {
    return name_entry_map_[probe];
  }
  NameEntryBuilderPtr builder = new NameEntryBuilder(probe);
  name_entry_map_[probe] = builder;
  return builder.Detach();
}

bool NameTable::Builder::Remove(int32_t platform_id,
                                int32_t encoding_id,
                                int32_t language_id,
                                int32_t name_id) {
  NameEntryId probe(platform_id, encoding_id, language_id, name_id);
  GetNameBuilders();  // Ensure name_entry_map_ is built.
  NameEntryBuilderMap::iterator position = name_entry_map_.find(probe);
  if (position != name_entry_map_.end()) {
    name_entry_map_.erase(position);
    return true;
  }
  return false;
}

CALLER_ATTACH FontDataTable*
    NameTable::Builder::SubBuildTable(ReadableFontData* data) {
  FontDataTablePtr table = new NameTable(header(), data);
  return table.Detach();
}

void NameTable::Builder::SubDataSet() {
  name_entry_map_.clear();
  set_model_changed(false);
}

int32_t NameTable::Builder::SubDataSizeToSerialize() {
  if (name_entry_map_.empty()) {
    return 0;
  }

  int32_t size = NameTable::Offset::kNameRecordStart +
      (int32_t)(name_entry_map_.size() * NameTable::Offset::kNameRecordSize);
  for (NameEntryBuilderMap::iterator b = name_entry_map_.begin(),
                                     end = name_entry_map_.end();
                                     b != end; ++b) {
    NameEntryBuilderPtr p = b->second;
    NameEntry* entry = p->name_entry();
    size += entry->NameBytesLength();
  }
  return size;
}

bool NameTable::Builder::SubReadyToSerialize() {
  return !name_entry_map_.empty();
}

int32_t NameTable::Builder::SubSerialize(WritableFontData* new_data) {
  int32_t string_table_start_offset =
      NameTable::Offset::kNameRecordStart +
      (int32_t)(name_entry_map_.size() * NameTable::Offset::kNameRecordSize);

  // Header
  new_data->WriteUShort(NameTable::Offset::kFormat, 0);
  new_data->WriteUShort(NameTable::Offset::kCount, (int32_t)name_entry_map_.size());
  new_data->WriteUShort(NameTable::Offset::kStringOffset,
                        string_table_start_offset);
  int32_t name_record_offset = NameTable::Offset::kNameRecordStart;
  int32_t string_offset = 0;
  // Note: we offered operator< in NameEntryId, which will be used by std::less,
  //       and therefore our map will act like TreeMap in Java to provide
  //       sorted key set.
  for (NameEntryBuilderMap::iterator b = name_entry_map_.begin(),
                                     end = name_entry_map_.end();
                                     b != end; ++b) {
    new_data->WriteUShort(
        name_record_offset + NameTable::Offset::kNameRecordPlatformId,
        b->first.platform_id());
    new_data->WriteUShort(
        name_record_offset + NameTable::Offset::kNameRecordEncodingId,
        b->first.encoding_id());
    new_data->WriteUShort(
        name_record_offset + NameTable::Offset::kNameRecordLanguageId,
        b->first.language_id());
    new_data->WriteUShort(
        name_record_offset + NameTable::Offset::kNameRecordNameId,
        b->first.name_id());
    NameEntry* builder_entry = b->second->name_entry();
    new_data->WriteUShort(
        name_record_offset + NameTable::Offset::kNameRecordStringLength,
        builder_entry->NameBytesLength());
    new_data->WriteUShort(
        name_record_offset + NameTable::Offset::kNameRecordStringOffset,
        string_offset);
    name_record_offset += NameTable::Offset::kNameRecordSize;
    string_offset += new_data->WriteBytes(
        string_offset + string_table_start_offset,
        builder_entry->NameAsBytes());
  }

  return string_offset + string_table_start_offset;
}

void NameTable::Builder::Initialize(ReadableFontData* data) {
  if (data) {
    NameTablePtr table = new NameTable(header(), data);
    Ptr<NameEntryIterator> name_iter;
    name_iter.Attach(table->Iterator());
    while (name_iter->HasNext()) {
      NameEntryPtr name_entry;
      name_entry.Attach(name_iter->Next());
      NameEntryBuilderPtr name_entry_builder = new NameEntryBuilder(name_entry);
      NameEntry* builder_entry = name_entry_builder->name_entry();
      NameEntryId probe = builder_entry->name_entry_id();
      name_entry_map_[probe] = name_entry_builder;
    }
  }
}

NameTable::NameEntryBuilderMap* NameTable::Builder::GetNameBuilders() {
  if (name_entry_map_.empty()) {
    Initialize(InternalReadData());
  }
  set_model_changed();
  return &name_entry_map_;
}

/******************************************************************************
 * NameTable class
 ******************************************************************************/
NameTable::~NameTable() {}

int32_t NameTable::Format() {
  return data_->ReadUShort(Offset::kFormat);
}

int32_t NameTable::NameCount() {
  return data_->ReadUShort(Offset::kCount);
}

int32_t NameTable::PlatformId(int32_t index) {
  return data_->ReadUShort(Offset::kNameRecordPlatformId +
                           OffsetForNameRecord(index));
}

int32_t NameTable::EncodingId(int32_t index) {
  return data_->ReadUShort(Offset::kNameRecordEncodingId +
                           OffsetForNameRecord(index));
}

int32_t NameTable::LanguageId(int32_t index) {
  return data_->ReadUShort(Offset::kNameRecordLanguageId +
                           OffsetForNameRecord(index));
}

int32_t NameTable::NameId(int32_t index) {
  return data_->ReadUShort(Offset::kNameRecordNameId +
                           OffsetForNameRecord(index));
}

void NameTable::NameAsBytes(int32_t index, std::vector<uint8_t>* b) {
  assert(b);
  b->clear();

  int32_t length = NameLength(index);
  if (length <= 0)
    return;

  int32_t offset = NameOffset(index);
  if (offset < 0)
    return;

  b->resize(length);
  data_->ReadBytes(offset, &((*b)[0]), 0, length);
}

void NameTable::NameAsBytes(int32_t platform_id,
                            int32_t encoding_id,
                            int32_t language_id,
                            int32_t name_id,
                            std::vector<uint8_t>* b) {
  assert(b);
  NameEntryPtr entry;
  entry.Attach(GetNameEntry(platform_id, encoding_id, language_id, name_id));
  if (entry) {
    std::vector<uint8_t>* name = entry->NameAsBytes();
    std::copy(name->begin(), name->end(), b->begin());
  }
}

wchar_t* NameTable::Name(int32_t index) {
  std::vector<uint8_t> b;
  NameAsBytes(index, &b);
  return ConvertFromNameBytes(&b, PlatformId(index), EncodingId(index));
}

wchar_t* NameTable::Name(int32_t platform_id,
                       int32_t encoding_id,
                       int32_t language_id,
                       int32_t name_id) {
  NameEntryPtr entry;
  entry.Attach(GetNameEntry(platform_id, encoding_id, language_id, name_id));
  if (entry) {
    return entry->Name();
  }
  return NULL;
}

CALLER_ATTACH NameTable::NameEntry* NameTable::GetNameEntry(int32_t index) {
  std::vector<uint8_t> b;
  NameAsBytes(index, &b);
  NameEntryPtr instance = new NameEntry(PlatformId(index),
                                        EncodingId(index),
                                        LanguageId(index),
                                        NameId(index), b);
  return instance.Detach();
}

CALLER_ATTACH NameTable::NameEntry* NameTable::GetNameEntry(int32_t platform_id,
                                                            int32_t encoding_id,
                                                            int32_t language_id,
                                                            int32_t name_id) {
  NameTable::NameEntryFilterInPlace
      filter(platform_id, encoding_id, language_id, name_id);
  Ptr<NameTable::NameEntryIterator> name_entry_iter;
  name_entry_iter.Attach(Iterator(&filter));
  NameEntryPtr result;
  if (name_entry_iter->HasNext()) {
    result = name_entry_iter->Next();
  }
  return result;
}

CALLER_ATTACH NameTable::NameEntryIterator* NameTable::Iterator() {
  Ptr<NameEntryIterator> output = new NameTable::NameEntryIterator(this);
  return output.Detach();
}

CALLER_ATTACH
NameTable::NameEntryIterator* NameTable::Iterator(NameEntryFilter* filter) {
  Ptr<NameEntryIterator> output =
      new NameTable::NameEntryIterator(this, filter);
  return output.Detach();
}

NameTable::NameTable(Header* header, ReadableFontData* data)
    : SubTableContainerTable(header, data) {}

int32_t NameTable::StringOffset() {
  return data_->ReadUShort(Offset::kStringOffset);
}

int32_t NameTable::OffsetForNameRecord(int32_t index) {
  return Offset::kNameRecordStart + index * Offset::kNameRecordSize;
}

int32_t NameTable::NameLength(int32_t index) {
  return data_->ReadUShort(Offset::kNameRecordStringLength +
                           OffsetForNameRecord(index));
}

int32_t NameTable::NameOffset(int32_t index) {
  return data_->ReadUShort(Offset::kNameRecordStringOffset +
                           OffsetForNameRecord(index)) + StringOffset();
}

const char* NameTable::GetEncodingName(int32_t platform_id,
                                       int32_t encoding_id) {
  switch (platform_id) {
    case PlatformId::kUnicode:
      return "UTF-16BE";
    case PlatformId::kMacintosh:
      switch (encoding_id) {
        case MacintoshEncodingId::kRoman:
          return "MacRoman";
        case MacintoshEncodingId::kJapanese:
          return "Shift-JIS";
        case MacintoshEncodingId::kChineseTraditional:
          return "Big5";
        case MacintoshEncodingId::kKorean:
          return "EUC-KR";
        case MacintoshEncodingId::kArabic:
          return "MacArabic";
        case MacintoshEncodingId::kHebrew:
          return "MacHebrew";
        case MacintoshEncodingId::kGreek:
          return "MacGreek";
        case MacintoshEncodingId::kRussian:
          return "MacCyrillic";
        case MacintoshEncodingId::kRSymbol:
          return "MacSymbol";
        case MacintoshEncodingId::kThai:
          return "MacThai";
        case MacintoshEncodingId::kChineseSimplified:
          return "EUC-CN";
        default:  // Note: unknown/unconfirmed cases are not ported.
          break;
      }
      break;
    case PlatformId::kISO:
      break;
    case PlatformId::kWindows:
      switch (encoding_id) {
        case WindowsEncodingId::kSymbol:
        case WindowsEncodingId::kUnicodeUCS2:
          return "UTF-16BE";
        case WindowsEncodingId::kShiftJIS:
          return "windows-933";
        case WindowsEncodingId::kPRC:
          return "windows-936";
        case WindowsEncodingId::kBig5:
          return "windows-950";
        case WindowsEncodingId::kWansung:
          return "windows-949";
        case WindowsEncodingId::kJohab:
          return "ms1361";
        case WindowsEncodingId::kUnicodeUCS4:
          return "UCS-4";
      }
      break;
    case PlatformId::kCustom:
      break;
    default:
      break;
  }
  return NULL;
}

/*UConverter* NameTable::GetCharset(int32_t platform_id, int32_t encoding_id) {
  UErrorCode error_code = U_ZERO_ERROR;
  UConverter* conv = ucnv_open(GetEncodingName(platform_id, encoding_id),
                               &error_code);
  if (U_SUCCESS(error_code)) {
    return conv;
  }

  if (conv) {
    ucnv_close(conv);
  }
  return NULL;
}*/

// https://github.com/aiekick/cTools
// std::wstring to std::string
// std::wstring(unicode/multibytes/char16/wchar_t) to std::string(char)
inline std::string wstring_to_string(const std::wstring wstr)
{
    std::mbstate_t state = std::mbstate_t();
    std::size_t len = wstr.size();
    std::vector<char> mbstr(len);
    const wchar_t* wptr = wstr.c_str();
#ifdef MSVC
    size_t res = 0;
    /*errno_t err = */wcsrtombs_s(&res, &mbstr[0], len, &wptr, mbstr.size(), &state);
#else
    std::wcsrtombs(&mbstr[0], &wptr, mbstr.size(), &state);
#endif
    return std::string(mbstr.data(), mbstr.size());
}

// https://github.com/aiekick/cTools
// std::string to std::wstring
// std::string(char) to std::wstring(unicode/multibytes/char16/wchar_t)
inline std::wstring string_to_wstring(const std::string mbstr)
{
    std::mbstate_t state = std::mbstate_t();
    std::size_t len = mbstr.size();
    std::vector<wchar_t> wstr(len);
    const char* ptr = mbstr.c_str();
#ifdef MSVC
    size_t res = 0;
    /*errno_t err = */mbsrtowcs_s(&res, &wstr[0], len, &ptr, wstr.size(), &state);
#else
    std::mbsrtowcs(&wstr[0], &ptr, wstr.size(), &state);
#endif
    return std::wstring(wstr.data(), wstr.size());
}

void NameTable::ConvertToNameBytes(const wchar_t* name,
	int32_t /*platform_id*/,
	int32_t /*encoding_id*/,
	std::vector<uint8_t>* b)
{
	assert(b);
	assert(name);
	b->clear();
	// UTF-16 to UTF-8
	//std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> myconv;
	//std::wstring ws = name;
	//std::string s = myconv.to_bytes(ws);
	std::string s = wstring_to_string(std::wstring(name));
	b->assign(s.begin(), s.end());
}

wchar_t* NameTable::ConvertFromNameBytes(std::vector<uint8_t>* name_bytes,
	int32_t /*platform_id*/,
	int32_t /*encoding_id*/)
{
	if (name_bytes == NULL || name_bytes->size() == 0)
	{
		return NULL;
	}

	// UTF-8 to UTF-16
	//std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> myconv;
	//std::string s = std::string(name_bytes->begin(), name_bytes->end());
	//std::wstring ws = myconv.from_bytes(s);
	std::wstring ws = string_to_wstring(std::string(name_bytes->begin(), name_bytes->end()));
	wchar_t *cstr = new wchar_t[ws.length() + 1];
#ifdef MSVC
	wcscpy_s(cstr, ws.length(), ws.c_str());
#else
    wcscpy(cstr, ws.c_str());
#endif
	return cstr;
}

}  // namespace sfntly
