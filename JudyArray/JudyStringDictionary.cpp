// _________________
//
// Copyright (C) 2011 Cory Thomas
//
// This program is free software; you can redistribute it and/or modify it
// under the term of the GNU Lesser General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// _________________

#include "StdAfx.h"
#include "JudyStringDictionary.h"
#include <Judy.h>
#include <string.h>
#include <vcclr.h>

using namespace JudyArray;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::Runtime::InteropServices;

struct KeyBytes
{
	unsigned char* Value;
	int Length;

	KeyBytes(String^ key)
	{
		Length = Encoding::UTF8->GetByteCount(key);
		Value = new unsigned char[Length + 1];
		pin_ptr<const wchar_t> keyChars = PtrToStringChars(key);
		Encoding::UTF8->GetBytes((wchar_t*)keyChars, key->Length, Value, Length);
		Value[Length] = 0;
	}

	~KeyBytes()
	{
		Release();
	}

	void Release()
	{
		delete [] Value;
		Value = NULL;
		Length = 0;
	}
};

namespace JudyArray
{

generic<typename TValue>
ref class JudyHybridArray
{
protected:
	Pvoid_t _judyArrayPtr;

public:
	JudyHybridArray() : _judyArrayPtr(NULL)
	{ 
	}

	virtual ~JudyHybridArray()
	{
		Clear();
	}

	void Clear()
	{
		if (_judyArrayPtr != NULL)
		{
			Word_t Bytes;
			Pvoid_t PJArray = _judyArrayPtr;
			Dispose(PJArray);
			JSLFA(Bytes, PJArray);
			_judyArrayPtr = NULL;
		}
	}

	void Set(String^ key, TValue value)
	{
		KeyBytes keyBytes(key);
		Pvoid_t valuePtr = NULL;
		Pvoid_t judyArrayPtr = _judyArrayPtr;

		JSLI(valuePtr, judyArrayPtr, keyBytes.Value);

		_judyArrayPtr = judyArrayPtr;

		if (valuePtr == PJERR)
		{
			throw gcnew Exception("Error adding key.");
		}

		Set((PWord_t)valuePtr, value);
	}

	virtual bool RemoveKey(String^ key)
	{
		KeyBytes keyBytes(key);
		Pvoid_t judyArrayPtr = _judyArrayPtr;

		int rc;
		JSLD(rc, judyArrayPtr, keyBytes.Value);
		_judyArrayPtr = judyArrayPtr;

		return rc != 0;
	}

	virtual bool TryGetValue(String^ key, [Out] TValue% value)
	{
		KeyBytes keyBytes(key);
		Pvoid_t judyArrayPtr = _judyArrayPtr;
		Pvoid_t valuePtr = NULL;

		JSLG(valuePtr, judyArrayPtr, keyBytes.Value);

		if (valuePtr == NULL)
		{
			value = TValue();
			return false;
		}

		value = GetValue((PWord_t)valuePtr);
		return true;
	}

protected:
	virtual void Dispose(Pvoid_t judyArrayPtr) {}
	virtual void Set(PWord_t entry, TValue value) {}
	virtual TValue GetValue(PWord_t entry) { return (TValue)0; }
};

generic<typename TValue>
ref class PrimitiveJudyHybridArray : public JudyHybridArray<TValue>
{
protected:
	virtual void Set(PWord_t entry, TValue value) override
	{
		memcpy(entry, &value, sizeof(TValue));
	}

	virtual TValue GetValue(PWord_t entry) override
	{
		TValue value;
		memcpy(&value, entry, sizeof(TValue));
		return value;
	}
};

generic<typename TValue>
ref class ReferenceJudyHybridArray : public JudyHybridArray<TValue>
{
public:
	virtual bool RemoveKey(String^ key) override
	{
		KeyBytes keyBytes(key);
		Pvoid_t judyArrayPtr = _judyArrayPtr;
		Pvoid_t valuePtr = NULL;

		JSLG(valuePtr, judyArrayPtr, keyBytes.Value);

		if (valuePtr != NULL)
		{
			Free((PWord_t)valuePtr);

			int rc;
			JSLD(rc, judyArrayPtr, keyBytes.Value);
			_judyArrayPtr = judyArrayPtr;
		}

		return valuePtr != NULL;
	}

protected:
	virtual void Dispose(Pvoid_t judyArrayPtr) override
	{
		uint8_t index[1024];
		Pvoid_t valuePtr = NULL;

		index[0] = '\0';
		JSLF(valuePtr, judyArrayPtr, index);

		while (valuePtr != NULL)
		{
			Free((PWord_t)valuePtr);
			JSLN(valuePtr, judyArrayPtr, index);
		}
	}
	
	virtual TValue GetValue(PWord_t entry) override
	{
		Word_t value = *entry;

		if (value == 0)
		{
			return TValue(); // default
		}
		else
		{
			IntPtr objPtr = IntPtr((void*)value);
			GCHandle objHandle = GCHandle::FromIntPtr(objPtr);
			return (TValue)objHandle.Target;
		}
	}

	static void Free(PWord_t entry)
	{
		Word_t value = *entry;

		if (value != 0)
		{
			IntPtr objPtr = IntPtr((void*)value);
			GCHandle objHandle = GCHandle::FromIntPtr(objPtr);
			objHandle.Free();
		}
	}

	virtual void Set(PWord_t entry, TValue value) override
	{
		Free(entry);

		if (value == nullptr)
		{
			*entry = 0;
		}
		else
		{
			Object^ objRef = value;
			GCHandle objHandle = GCHandle::Alloc(objRef);
			IntPtr objPtr = GCHandle::ToIntPtr(objHandle);
			*entry = (Word_t)objPtr.ToPointer();
		}
	}
};

}

generic<typename TValue>
JudyStringDictionary<TValue>::JudyStringDictionary()
{
	 Type^ valueType = TValue::typeid;

	 if (valueType->Equals(Char::typeid) ||
		 valueType->Equals(Byte::typeid) ||
		 valueType->Equals(SByte::typeid) ||
		 valueType->Equals(UInt16::typeid) ||
		 valueType->Equals(Int16::typeid) ||
		 valueType->Equals(UInt32::typeid) ||
		 valueType->Equals(Int32::typeid) ||
		 valueType->Equals(Single::typeid)
#ifdef WIN64
		 || valueType->Equals(Double::typeid)
		 || valueType->Equals(Int64::typeid)
		 || valueType->Equals(UInt64::typeid)
#endif
		 )
	 {
		 _judyArray = gcnew PrimitiveJudyHybridArray<TValue>();
	 }
	 else
	 {
		 _judyArray = gcnew ReferenceJudyHybridArray<TValue>();
	 }
}

generic<typename TValue>
JudyStringDictionary<TValue>::~JudyStringDictionary()
{
	delete _judyArray;
	_judyArray = nullptr;
}

generic<typename TValue>
bool JudyStringDictionary<TValue>::RemoveKey(String^ key)
{
	if (key == nullptr) throw gcnew ArgumentNullException("key");
	return _judyArray->RemoveKey(key);
}

generic<typename TValue>
bool JudyStringDictionary<TValue>::TryGetValue(String^ key, [Out] TValue% value)
{
	if (key == nullptr) throw gcnew ArgumentNullException("key");
	return _judyArray->TryGetValue(key, value);
}

generic<typename TValue>
void JudyStringDictionary<TValue>::Clear()
{
	_judyArray->Clear();
}

generic<typename TValue>
TValue JudyStringDictionary<TValue>::default::get(String^ key)
{
	if (key == nullptr) throw gcnew ArgumentNullException("key");

	TValue value;

	if (!_judyArray->TryGetValue(key, value))
		throw gcnew System::Collections::Generic::KeyNotFoundException(key);

	return value;
}

generic<typename TValue>
void JudyStringDictionary<TValue>::default::set(String^ key, TValue value)
{
	if (key == nullptr) throw gcnew ArgumentNullException("key");
	_judyArray->Set(key, value);
}
