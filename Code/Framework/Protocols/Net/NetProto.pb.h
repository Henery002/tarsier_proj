// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Net/NetProto.proto

#ifndef PROTOBUF_Net_2fNetProto_2eproto__INCLUDED
#define PROTOBUF_Net_2fNetProto_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
#include "google/protobuf/descriptor.pb.h"
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_Net_2fNetProto_2eproto();
void protobuf_AssignDesc_Net_2fNetProto_2eproto();
void protobuf_ShutdownFile_Net_2fNetProto_2eproto();

class Packet;
class PacketSize;

enum PACKET_TYPE {
  PACKET_INVALID = -1,
  PACKET_REQUEST = 0,
  PACKET_RESPONSE = 1,
  PACKET_NOTIFY = 2,
  NUM_PACKET_TYPE = 3
};
bool PACKET_TYPE_IsValid(int value);
const PACKET_TYPE PACKET_TYPE_MIN = PACKET_INVALID;
const PACKET_TYPE PACKET_TYPE_MAX = NUM_PACKET_TYPE;
const int PACKET_TYPE_ARRAYSIZE = PACKET_TYPE_MAX + 1;

const ::google::protobuf::EnumDescriptor* PACKET_TYPE_descriptor();
inline const ::std::string& PACKET_TYPE_Name(PACKET_TYPE value) {
  return ::google::protobuf::internal::NameOfEnum(
    PACKET_TYPE_descriptor(), value);
}
inline bool PACKET_TYPE_Parse(
    const ::std::string& name, PACKET_TYPE* value) {
  return ::google::protobuf::internal::ParseNamedEnum<PACKET_TYPE>(
    PACKET_TYPE_descriptor(), name, value);
}
// ===================================================================

class Packet : public ::google::protobuf::Message {
 public:
  Packet();
  virtual ~Packet();

  Packet(const Packet& from);

  inline Packet& operator=(const Packet& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Packet& default_instance();

  void Swap(Packet* other);

  // implements Message ----------------------------------------------

  Packet* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Packet& from);
  void MergeFrom(const Packet& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional fixed32 size = 1;
  inline bool has_size() const;
  inline void clear_size();
  static const int kSizeFieldNumber = 1;
  inline ::google::protobuf::uint32 size() const;
  inline void set_size(::google::protobuf::uint32 value);

  // optional .PACKET_TYPE type = 2 [default = PACKET_INVALID];
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 2;
  inline ::PACKET_TYPE type() const;
  inline void set_type(::PACKET_TYPE value);

  // optional uint64 seq = 3;
  inline bool has_seq() const;
  inline void clear_seq();
  static const int kSeqFieldNumber = 3;
  inline ::google::protobuf::uint64 seq() const;
  inline void set_seq(::google::protobuf::uint64 value);

  // optional int32 cmd = 4;
  inline bool has_cmd() const;
  inline void clear_cmd();
  static const int kCmdFieldNumber = 4;
  inline ::google::protobuf::int32 cmd() const;
  inline void set_cmd(::google::protobuf::int32 value);

  // optional int32 err = 5;
  inline bool has_err() const;
  inline void clear_err();
  static const int kErrFieldNumber = 5;
  inline ::google::protobuf::int32 err() const;
  inline void set_err(::google::protobuf::int32 value);

  // optional string body = 6;
  inline bool has_body() const;
  inline void clear_body();
  static const int kBodyFieldNumber = 6;
  inline const ::std::string& body() const;
  inline void set_body(const ::std::string& value);
  inline void set_body(const char* value);
  inline void set_body(const char* value, size_t size);
  inline ::std::string* mutable_body();
  inline ::std::string* release_body();
  inline void set_allocated_body(::std::string* body);

  // optional string extra = 7;
  inline bool has_extra() const;
  inline void clear_extra();
  static const int kExtraFieldNumber = 7;
  inline const ::std::string& extra() const;
  inline void set_extra(const ::std::string& value);
  inline void set_extra(const char* value);
  inline void set_extra(const char* value, size_t size);
  inline ::std::string* mutable_extra();
  inline ::std::string* release_extra();
  inline void set_allocated_extra(::std::string* extra);

  // @@protoc_insertion_point(class_scope:Packet)
 private:
  inline void set_has_size();
  inline void clear_has_size();
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_seq();
  inline void clear_has_seq();
  inline void set_has_cmd();
  inline void clear_has_cmd();
  inline void set_has_err();
  inline void clear_has_err();
  inline void set_has_body();
  inline void clear_has_body();
  inline void set_has_extra();
  inline void clear_has_extra();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 size_;
  int type_;
  ::google::protobuf::uint64 seq_;
  ::google::protobuf::int32 cmd_;
  ::google::protobuf::int32 err_;
  ::std::string* body_;
  ::std::string* extra_;
  friend void  protobuf_AddDesc_Net_2fNetProto_2eproto();
  friend void protobuf_AssignDesc_Net_2fNetProto_2eproto();
  friend void protobuf_ShutdownFile_Net_2fNetProto_2eproto();

  void InitAsDefaultInstance();
  static Packet* default_instance_;
};
// -------------------------------------------------------------------

class PacketSize : public ::google::protobuf::Message {
 public:
  PacketSize();
  virtual ~PacketSize();

  PacketSize(const PacketSize& from);

  inline PacketSize& operator=(const PacketSize& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const PacketSize& default_instance();

  void Swap(PacketSize* other);

  // implements Message ----------------------------------------------

  PacketSize* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const PacketSize& from);
  void MergeFrom(const PacketSize& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional fixed32 size = 1;
  inline bool has_size() const;
  inline void clear_size();
  static const int kSizeFieldNumber = 1;
  inline ::google::protobuf::uint32 size() const;
  inline void set_size(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:PacketSize)
 private:
  inline void set_has_size();
  inline void clear_has_size();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 size_;
  friend void  protobuf_AddDesc_Net_2fNetProto_2eproto();
  friend void protobuf_AssignDesc_Net_2fNetProto_2eproto();
  friend void protobuf_ShutdownFile_Net_2fNetProto_2eproto();

  void InitAsDefaultInstance();
  static PacketSize* default_instance_;
};
// ===================================================================

static const int kMethodIdFieldNumber = 2000;
extern ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::MethodOptions,
    ::google::protobuf::internal::PrimitiveTypeTraits< ::google::protobuf::int32 >, 5, false >
  method_id;

// ===================================================================

// Packet

// optional fixed32 size = 1;
inline bool Packet::has_size() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Packet::set_has_size() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Packet::clear_has_size() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Packet::clear_size() {
  size_ = 0u;
  clear_has_size();
}
inline ::google::protobuf::uint32 Packet::size() const {
  // @@protoc_insertion_point(field_get:Packet.size)
  return size_;
}
inline void Packet::set_size(::google::protobuf::uint32 value) {
  set_has_size();
  size_ = value;
  // @@protoc_insertion_point(field_set:Packet.size)
}

// optional .PACKET_TYPE type = 2 [default = PACKET_INVALID];
inline bool Packet::has_type() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Packet::set_has_type() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Packet::clear_has_type() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Packet::clear_type() {
  type_ = -1;
  clear_has_type();
}
inline ::PACKET_TYPE Packet::type() const {
  // @@protoc_insertion_point(field_get:Packet.type)
  return static_cast< ::PACKET_TYPE >(type_);
}
inline void Packet::set_type(::PACKET_TYPE value) {
  assert(::PACKET_TYPE_IsValid(value));
  set_has_type();
  type_ = value;
  // @@protoc_insertion_point(field_set:Packet.type)
}

// optional uint64 seq = 3;
inline bool Packet::has_seq() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Packet::set_has_seq() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Packet::clear_has_seq() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Packet::clear_seq() {
  seq_ = GOOGLE_ULONGLONG(0);
  clear_has_seq();
}
inline ::google::protobuf::uint64 Packet::seq() const {
  // @@protoc_insertion_point(field_get:Packet.seq)
  return seq_;
}
inline void Packet::set_seq(::google::protobuf::uint64 value) {
  set_has_seq();
  seq_ = value;
  // @@protoc_insertion_point(field_set:Packet.seq)
}

// optional int32 cmd = 4;
inline bool Packet::has_cmd() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Packet::set_has_cmd() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Packet::clear_has_cmd() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Packet::clear_cmd() {
  cmd_ = 0;
  clear_has_cmd();
}
inline ::google::protobuf::int32 Packet::cmd() const {
  // @@protoc_insertion_point(field_get:Packet.cmd)
  return cmd_;
}
inline void Packet::set_cmd(::google::protobuf::int32 value) {
  set_has_cmd();
  cmd_ = value;
  // @@protoc_insertion_point(field_set:Packet.cmd)
}

// optional int32 err = 5;
inline bool Packet::has_err() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void Packet::set_has_err() {
  _has_bits_[0] |= 0x00000010u;
}
inline void Packet::clear_has_err() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void Packet::clear_err() {
  err_ = 0;
  clear_has_err();
}
inline ::google::protobuf::int32 Packet::err() const {
  // @@protoc_insertion_point(field_get:Packet.err)
  return err_;
}
inline void Packet::set_err(::google::protobuf::int32 value) {
  set_has_err();
  err_ = value;
  // @@protoc_insertion_point(field_set:Packet.err)
}

// optional string body = 6;
inline bool Packet::has_body() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void Packet::set_has_body() {
  _has_bits_[0] |= 0x00000020u;
}
inline void Packet::clear_has_body() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void Packet::clear_body() {
  if (body_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    body_->clear();
  }
  clear_has_body();
}
inline const ::std::string& Packet::body() const {
  // @@protoc_insertion_point(field_get:Packet.body)
  return *body_;
}
inline void Packet::set_body(const ::std::string& value) {
  set_has_body();
  if (body_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    body_ = new ::std::string;
  }
  body_->assign(value);
  // @@protoc_insertion_point(field_set:Packet.body)
}
inline void Packet::set_body(const char* value) {
  set_has_body();
  if (body_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    body_ = new ::std::string;
  }
  body_->assign(value);
  // @@protoc_insertion_point(field_set_char:Packet.body)
}
inline void Packet::set_body(const char* value, size_t size) {
  set_has_body();
  if (body_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    body_ = new ::std::string;
  }
  body_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Packet.body)
}
inline ::std::string* Packet::mutable_body() {
  set_has_body();
  if (body_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    body_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Packet.body)
  return body_;
}
inline ::std::string* Packet::release_body() {
  clear_has_body();
  if (body_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = body_;
    body_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Packet::set_allocated_body(::std::string* body) {
  if (body_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete body_;
  }
  if (body) {
    set_has_body();
    body_ = body;
  } else {
    clear_has_body();
    body_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:Packet.body)
}

// optional string extra = 7;
inline bool Packet::has_extra() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void Packet::set_has_extra() {
  _has_bits_[0] |= 0x00000040u;
}
inline void Packet::clear_has_extra() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void Packet::clear_extra() {
  if (extra_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    extra_->clear();
  }
  clear_has_extra();
}
inline const ::std::string& Packet::extra() const {
  // @@protoc_insertion_point(field_get:Packet.extra)
  return *extra_;
}
inline void Packet::set_extra(const ::std::string& value) {
  set_has_extra();
  if (extra_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    extra_ = new ::std::string;
  }
  extra_->assign(value);
  // @@protoc_insertion_point(field_set:Packet.extra)
}
inline void Packet::set_extra(const char* value) {
  set_has_extra();
  if (extra_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    extra_ = new ::std::string;
  }
  extra_->assign(value);
  // @@protoc_insertion_point(field_set_char:Packet.extra)
}
inline void Packet::set_extra(const char* value, size_t size) {
  set_has_extra();
  if (extra_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    extra_ = new ::std::string;
  }
  extra_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:Packet.extra)
}
inline ::std::string* Packet::mutable_extra() {
  set_has_extra();
  if (extra_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    extra_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:Packet.extra)
  return extra_;
}
inline ::std::string* Packet::release_extra() {
  clear_has_extra();
  if (extra_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = extra_;
    extra_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void Packet::set_allocated_extra(::std::string* extra) {
  if (extra_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete extra_;
  }
  if (extra) {
    set_has_extra();
    extra_ = extra;
  } else {
    clear_has_extra();
    extra_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:Packet.extra)
}

// -------------------------------------------------------------------

// PacketSize

// optional fixed32 size = 1;
inline bool PacketSize::has_size() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void PacketSize::set_has_size() {
  _has_bits_[0] |= 0x00000001u;
}
inline void PacketSize::clear_has_size() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void PacketSize::clear_size() {
  size_ = 0u;
  clear_has_size();
}
inline ::google::protobuf::uint32 PacketSize::size() const {
  // @@protoc_insertion_point(field_get:PacketSize.size)
  return size_;
}
inline void PacketSize::set_size(::google::protobuf::uint32 value) {
  set_has_size();
  size_ = value;
  // @@protoc_insertion_point(field_set:PacketSize.size)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::PACKET_TYPE> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::PACKET_TYPE>() {
  return ::PACKET_TYPE_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_Net_2fNetProto_2eproto__INCLUDED
