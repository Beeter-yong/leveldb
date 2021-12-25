// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_
#define STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_

#include <cstdint>
#include <vector>

#include "leveldb/slice.h"

namespace leveldb {

struct Options;

class BlockBuilder {
 public:
  explicit BlockBuilder(const Options* options);

  BlockBuilder(const BlockBuilder&) = delete;
  BlockBuilder& operator=(const BlockBuilder&) = delete;

  // Reset the contents as if the BlockBuilder was just constructed.
  void Reset();  // Block 构建完毕，状态重置

  // REQUIRES: Finish() has not been called since the last call to Reset().
  // REQUIRES: key is larger than any previously added key
  // 向 Block 添加数据 key
  void Add(const Slice& key, const Slice& value);

  // Finish building the block and return a slice that refers to the
  // block contents.  The returned slice will remain valid for the
  // lifetime of this builder or until Reset() is called.
  // 最后一个 Block 构建完成后触发 Reset 等操作
  Slice Finish();

  // Returns an estimate of the current (uncompressed) size of the block
  // we are building.  
  // 当前 Block 大小，超过阈值要开启新的 Block
  size_t CurrentSizeEstimate() const;

  // Return true iff no entries have been added since the last Reset()
  bool empty() const { return buffer_.empty(); }

 private:
  const Options* options_;    // 设置的信息，如重启点等
  std::string buffer_;              // Destination buffer 序列化后的数据
  std::vector<uint32_t> restarts_;  // Restart points  内部重启点偏移量
  int counter_;                     // Number of entries emitted since restart  重启点计数
  bool finished_;                   // Has Finish() been called? 结束
  std::string last_key_;    // 记录上一个 key
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_
