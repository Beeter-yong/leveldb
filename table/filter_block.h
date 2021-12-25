// Copyright (c) 2012 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// A filter block is stored near the end of a Table file.  It contains
// filters (e.g., bloom filters) for all data blocks in the table combined
// into a single filter block.

#ifndef STORAGE_LEVELDB_TABLE_FILTER_BLOCK_H_
#define STORAGE_LEVELDB_TABLE_FILTER_BLOCK_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "leveldb/slice.h"
#include "util/hash.h"

namespace leveldb {

class FilterPolicy;

// A FilterBlockBuilder is used to construct all of the filters for a
// particular Table.  It generates a single string which is stored as
// a special block in the Table.
//
// The sequence of calls to FilterBlockBuilder must match the regexp:
//      (StartBlock AddKey*)* Finish
class FilterBlockBuilder {
 public:
  explicit FilterBlockBuilder(const FilterPolicy*);

  FilterBlockBuilder(const FilterBlockBuilder&) = delete;
  FilterBlockBuilder& operator=(const FilterBlockBuilder&) = delete;

  void StartBlock(uint64_t block_offset); // 根据 dataBlock 的偏移量构建新的 filterBlock
  void AddKey(const Slice& key);  // 添加 key 到 filterBlock 中
  Slice Finish(); // 保存每个 filter 的偏移量

 private:
  void GenerateFilter();  // 生成一个 filter

  const FilterPolicy* policy_;     // 使用的过滤器策略，当前是 bloom filter
  std::string keys_;             // Flattened key contents  所有的 key
  std::vector<size_t> start_;    // Starting index in keys_ of each key 每一个 key 在 key_ 中的偏移量
  std::string result_;           // Filter data computed so far 保存所有 key 的构建 filter 的二进制数据
  std::vector<Slice> tmp_keys_;  // policy_->CreateFilter() argument // 更具 keys_ 和 start_ 恢复具体的 key 保存地方
  std::vector<uint32_t> filter_offsets_; // 每 2KB 数据创建一个 filter，所以存在多个 fileter，这里存储filter 的偏移量
};

class FilterBlockReader {
 public:
  // REQUIRES: "contents" and *policy must stay live while *this is live.
  FilterBlockReader(const FilterPolicy* policy, const Slice& contents);
  bool KeyMayMatch(uint64_t block_offset, const Slice& key);

 private:
  const FilterPolicy* policy_;
  const char* data_;    // Pointer to filter data (at block-start)
  const char* offset_;  // Pointer to beginning of offset array (at block-end)
  size_t num_;          // Number of entries in offset array
  size_t base_lg_;      // Encoding parameter (see kFilterBaseLg in .cc file)
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_TABLE_FILTER_BLOCK_H_
