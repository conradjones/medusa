#ifndef __MEDUSA_CPU_HPP__
#define __MEDUSA_CPU_HPP__

#include "medusa/namespace.hpp"
#include "medusa/export.hpp"
#include "medusa/types.hpp"
#include "medusa/address.hpp"
#include "medusa/database.hpp"

#include <set>
#include <string>
#include <unordered_map>
#include <functional>

MEDUSA_NAMESPACE_BEGIN

class Medusa_EXPORT CpuInformation
{
public:
  enum Type
  {
    StackPointerRegister,
    StackFrameRegister,
    ProgramPointerRegister,
    FlagRegister,
    AccumulatorRegister,
    InvalidRegister
  };

  virtual char const* ConvertIdentifierToName(u32 Id)                   const = 0;
  virtual u32         ConvertNameToIdentifier(std::string const& rName) const = 0;
  virtual u32         GetRegisterByType(Type RegType)                   const = 0;
  virtual u32         GetSizeOfRegisterInBit(u32 Id)                    const = 0;
};

class Medusa_EXPORT CpuContext
{
public:
  CpuContext(CpuInformation const& rCpuInfo) : m_rCpuInfo(rCpuInfo) {}

  virtual bool ReadRegister (u32 Register, void*       pValue, u32 Size) const = 0;
  virtual bool WriteRegister(u32 Register, void const* pValue, u32 Size)       = 0;

  virtual bool Translate(Address const& rLogicalAddress, u64& rLinearAddress) const;
  virtual bool AddMapping(Address const& rLogicalAddress, u64 LinearAddress);
  virtual bool RemoveMapping(Address const& rLogicalAddress);

  virtual std::string ToString(void) const = 0;

  CpuInformation const& GetCpuInformation(void) const { return m_rCpuInfo; }

protected:
  CpuInformation const& m_rCpuInfo;
  typedef std::unordered_map<Address, u64> AddressMap;
  AddressMap m_AddressMap;
};

class Medusa_EXPORT MemoryContext
{
public:
  MemoryContext(CpuInformation const& rCpuInfo) : m_rCpuInfo(rCpuInfo) {}

  virtual bool ReadMemory    (u64 LinearAddress, void* pValue,       u32 ValueSize) const;
  virtual bool WriteMemory   (u64 LinearAddress, void const* pValue, u32 ValueSize);

  virtual bool AllocateMemory(u64 LinearAddress, u32 Size, void** ppRawMemory);
  virtual bool FreeMemory    (u64 LinearAddress);
  virtual void MapDatabase   (Database const& rDatabase, CpuContext const* pCpuCtxt);

  virtual std::string ToString(void) const;

protected:
  CpuInformation const& m_rCpuInfo;

  struct MemoryChunk
  {
    u64   m_Address;
    u32   m_Size;
    void* m_Buffer;

    MemoryChunk(u64 Address = 0, u32 Size = 0x0, void* Buffer = nullptr)
      : m_Address(Address), m_Size(Size), m_Buffer(Buffer) {}

    bool operator<(MemoryChunk const& rMemChunk) const
    { return m_Address < rMemChunk.m_Address; }
  };

  typedef std::set<MemoryChunk> MemoryChunkSet;
  MemoryChunkSet m_Memories;

  virtual bool FindMemoryChunk(Address const& rAddress, MemoryChunk& rMemChnk) const;
};

class Medusa_EXPORT VariableContext
{
public:
  virtual bool ReadVariable(std::string const& rVariableName, void* pValue, u32 ValueSize) const;
  virtual bool WriteVariable(std::string const& rVariableName, void const* pValue, u32 ValueSize);

  virtual bool AllocateVariable(std::string const& rVariableName, u32 Size, void** ppRawMemory);
  virtual bool FreeVariable(std::string const& rVariableName);

  virtual std::string ToString(void) const;

protected:
  struct VariableInformation
  {
    void *m_pBuffer;
    u32 m_Size;

    VariableInformation(void *pBuffer = nullptr, u32 Size = 0)
      : m_pBuffer(pBuffer), m_Size(Size) {}
  };
  typedef std::unordered_map<std::string, VariableInformation> VariableMap;
  VariableMap m_Variables;

  virtual bool FindVariable(std::string const& rVariableName, VariableInformation& rVariableInformation) const;
};

MEDUSA_NAMESPACE_END

#endif // !__MEDUSA_CPU_HPP__
