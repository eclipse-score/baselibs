/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
/*!        \file
 *        \brief  Representation of JSON data.
 *
 *      \details  Provides handling methods for JSON data.
 *
 *********************************************************************************************************************/

#ifndef LIB_VAJSON_INCLUDE_AMSR_JSON_READER_JSON_DATA_H_
#define LIB_VAJSON_INCLUDE_AMSR_JSON_READER_JSON_DATA_H_

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
#include <cstdint>
#include <functional>
#include <memory>
#include "amsr/json/reader/internal/config/json_reader_cfg.h"
#include "amsr/json/reader/internal/depth_counter.h"
#include "amsr/json/reader/parser_state.h"
#include "amsr/json/reader_fwd.h"
#include "amsr/json/util/types.h"

namespace amsr {
namespace json {

/*!
 * \brief           A JSON data representation
 *
 * \details         Handles the state of the data, such as the current position in the file and nesting of JSON tree.
 * \vpublic
 *
 * \trace           DSGN-JSON-Reader-Deserialization
 */
class JsonData final {
  /*!
   * \brief           Stream buffer
   */
  std::reference_wrapper<std::istream> stream_;
  /*!
   * \brief           The potentially owned stream
   */
  std::unique_ptr<std::istream> owned_stream_{nullptr};
  /*!
   * \brief           JSON structure state
   */
  internal::DepthCounter depth_counter_{};
  /*!
   * \brief           Current key
   */
  String current_key_{};
  /*!
   * \brief           Current buffer
   */
  String current_buffer_{};
  /*!
   * \brief           Flag to indicate if the document has a UTF-8 BOM
   */
  EncodingType encoding_{EncodingType::kNone};
  /*!
   * \brief           Backup of the structure state
   */
  internal::DepthCounter depth_counter_backup_{};
  /*!
   * \brief           Backup of the document position
   */
  std::uint64_t pos_backup_{0U};
  /*!
   * \brief           Flag to indicate if a backup state is available
   */
  bool has_backup_{false};

 public:
  /*!
   * \brief           Class JsonOps must have access to the InputStream but no other classes
   */
  friend class internal::JsonOps;  // VECTOR SL AutosarC++17_10-A11.3.1: MD_JSON_AutosarC++17_10-A11.3.1_json_ops

  /*!
   * \brief           Initializes a JSON data object using a constructed reader
   * \vprivate        Vector product internal API
   * \param[in]       input_stream
   *                  to operate on.
   *
   * \context         ANY
   * \pre             input_stream has to be open.
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   *    requires true;
   * \endspec
   */
  explicit JsonData(std::istream& input_stream) noexcept;

  /*!
   * \brief           Initializes a JSON data object using a constructed reader
   * \vprivate        Vector component internal API
   * \param[in]       input_stream
   *                  to operate on.
   *
   * \context         ANY
   * \pre             input_stream has to be open.
   * \threadsafe      FALSE
   * \reentrant       FALSE
   */
  explicit JsonData(std::unique_ptr<std::istream> input_stream) noexcept;

  /*!
   * \brief           Initializes a JSON data object from a file
   * \vpublic
   * \param[in]       path
   *                  The path to the JSON file.
   * \return          A constructed JSON data object.
   * \error           amsr::json::JsonErrc::kStreamFailure
   *                  if the file could not be opened.
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   */
  static auto FromFile(std::string_view path) noexcept -> Result<JsonData>;

  /*!
   * \brief           Initializes a JSON data object from a buffer
   * \vpublic
   * \param[in]       buffer
   *                  The buffer containing the JSON value.
   * \return          A constructed JSON data object.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   */
  static auto FromBuffer(std::string_view buffer) noexcept -> Result<JsonData>;

  /*!
   * \brief           Initializes a JSON data object from a buffer
   * \vpublic
   * \param[in]       buffer
   *                  The buffer containing the JSON value.
   * \return          A constructed JSON data object.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   */
  static auto FromBuffer(score::safecpp::zstring_view buffer) noexcept -> Result<JsonData>;

  /*!
   * \brief           Initializes a JSON data object from a buffer
   * \vpublic
   * \param[in]       buffer
   *                  The buffer containing the JSON value.
   * \return          A constructed JSON data object.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \synchronous     -
   * \spec
   * requires true;
   * \endspec
   */
  static auto FromBuffer(score::cpp::span<char const> buffer) noexcept -> Result<JsonData>;

  /*!
   * \brief           Move constructor
   * \vpublic
   * \param[in]       other
   *                  The moved from object.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  JsonData(JsonData&& other) noexcept = default;

  /*!
   * \brief           Default move assignment
   * \vpublic
   * \return          A reference to the moved into object.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */

  auto operator=(JsonData&&) & noexcept -> JsonData& = default;

  /*!
   * \brief           Deleted copy constructor
   */
  JsonData(JsonData const&) = delete;

  /*!
   * \brief           Deleted copy assignment
   */
  auto operator=(JsonData const&) -> JsonData& = delete;

  /*!
   * \brief           Default destructor
   * \vpublic
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */

  ~JsonData() noexcept = default;

  /*!
   * \brief           Returns the JSON structure state
   * \vprivate        Vector component internal API
   * \return          The current state.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetState() & noexcept -> internal::DepthCounter& { return this->depth_counter_; }

  /*!
   * \brief           Sets the current key
   * \vprivate        Vector component internal API
   * \param[in]       key
   *                  to store.
   * \return          The current key.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  void StoreCurrentKey(StringView key) noexcept {

    this->current_key_ = std::string(key);
  }

  /*!
   * \brief           Returns the current key
   * \vprivate        Vector component internal API
   * \return          The current key.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetCurrentKey() const noexcept -> CStringView { return CStringView{this->current_key_}; }

  /*!
   * \brief           Returns the current string
   * \vprivate        Vector component internal API
   * \return          The current string.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetCurrentString() const noexcept -> CStringView { return CStringView{this->current_buffer_}; }

  /*!
   * \brief           Gets the emptied string buffer
   * \vprivate        Vector component internal API
   * \return          The emptied string buffer.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetClearedStringBuffer() & noexcept -> String& {

    this->current_buffer_.clear();
    return this->current_buffer_;
  }

  /*!
   * \brief           Gets the string buffer
   * \vprivate        Vector component internal API
   * \return          The string buffer.
   *
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetStringBuffer() & noexcept -> String& { return this->current_buffer_; }

  /*!
   * \brief           Returns the detected encoding type
   * \vpublic
   * \return          The encoding type.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \reentrant       TRUE, for different this pointer
   * \synchronous     -
   * \trace           DSGN-JSON-Reader-Encoding
   * \spec
   * requires true;
   * \endspec
   */
  auto GetEncoding() & noexcept -> EncodingType& { return this->encoding_; }

  /*!
   * \brief           Stores a snapshot of the parser's current state
   * \vprivate        Vector component internal API
   * \return          A Result being empty, or the error that occurred.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto Snap() noexcept -> Result<score::Blank>;

  /*!
   * \brief           Restores the parser's state from the previous snapshot
   * \vprivate        Vector component internal API
   * \return          A Result being empty, or the error that occurred.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto Restore() noexcept -> Result<score::Blank>;

 private:
  /*!
   * \brief           Returns the stream
   * \return          The stream.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetStream() noexcept -> std::istream& { return this->stream_.get(); }

  /*!
   * \brief           Returns the stream
   * \return          The stream.
   * \context         ANY
   * \pre             -
   * \threadsafe      TRUE, for different this pointer
   * \spec
   * requires true;
   * \endspec
   */
  auto GetStream() const noexcept -> std::istream const& { return this->stream_.get(); }

  /*!
   * \brief           Inspects the document's BOM
   *
   * \context         ANY
   * \pre             The read pointer must be at the beginning of the document.
   * \threadsafe      FALSE
   * \reentrant       FALSE
   * \spec
   * requires true;
   * \endspec
   */
  void ParseBom() noexcept;
};

}  // namespace json
}  // namespace amsr

#endif  // LIB_VAJSON_INCLUDE_AMSR_JSON_READER_JSON_DATA_H_
