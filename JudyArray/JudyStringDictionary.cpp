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
#include <vector>

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
ref class JudyHybridArrayEnumerator : IEnumerator<KeyValuePair<String^, TValue>>
{
private:
	JudyHybridArray<TValue>^ _judyArray;
	int _version;
	int _state;
	KeyValuePair<String^, TValue> _current;
	cli::array<unsigned char>^ _buffer;

public:
	JudyHybridArrayEnumerator(JudyHybridArray<TValue>^ judyArray);
	~JudyHybridArrayEnumerator() {}

	virtual bool MoveNext();
	virtual void Reset();

	virtual property KeyValuePair<String^, TValue> Current
	{
		KeyValuePair<String^, TValue> get();
	}

	virtual property Object^ CurrentObject
	{
		Object^ get() = System::Collections::IEnumerator::Current::get { return Current; }
	}
};

generic<typename TValue>
ref class JudyHybridArray : IEnumerable<KeyValuePair<String^, TValue>>
{
public:
	Pvoid_t _judyArrayPtr;
	int _enumeratorVersion;
	int _maxValueLength;

	JudyHybridArray() : _judyArrayPtr(NULL), _enumeratorVersion(0), _maxValueLength(0)
	{ 
	}

	virtual ~JudyHybridArray()
	{
		Clear();
	}

	bool ContainsKey(String^ key)
	{
		KeyBytes keyBytes(key);
		Pvoid_t judyArrayPtr = _judyArrayPtr;
		Pvoid_t valuePtr = NULL;

		JSLG(valuePtr, judyArrayPtr, keyBytes.Value);

		return valuePtr != NULL;
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
			_enumeratorVersion += 1;
			_maxValueLength = 0;
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

		_enumeratorVersion += 1;
		_maxValueLength = Math::Max(_maxValueLength, keyBytes.Length);
		Set((PWord_t)valuePtr, value);
	}

	virtual bool Remove(String^ key)
	{
		KeyBytes keyBytes(key);
		Pvoid_t judyArrayPtr = _judyArrayPtr;

		int rc;
		JSLD(rc, judyArrayPtr, keyBytes.Value);
		_judyArrayPtr = judyArrayPtr;

		if (rc != 0)
		{
			_enumeratorVersion += 1;

			if (_judyArrayPtr == NULL)
				_maxValueLength = 0;
		}

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

	virtual System::Collections::Generic::IEnumerator<System::Collections::Generic::KeyValuePair<System::String^, TValue>>^ GetEnumerator()
	{
		// Invalidate any other enumerators
		_enumeratorVersion += 1;
		return gcnew JudyHybridArrayEnumerator<TValue>(this);
	}

	virtual System::Collections::IEnumerator^ EnumerableGetEnumerator() = System::Collections::IEnumerable::GetEnumerator
	{
		return GetEnumerator();
	}

	virtual TValue GetValue(PWord_t entry) { return TValue(); }

protected:
	virtual void Dispose(Pvoid_t judyArrayPtr) {}
	virtual void Set(PWord_t entry, TValue value) {}
};

generic<typename TValue>
JudyHybridArrayEnumerator<TValue>::JudyHybridArrayEnumerator(JudyHybridArray<TValue>^ judyArray)
	: _judyArray(judyArray), _version(judyArray->_enumeratorVersion), _state(0), _buffer(gcnew cli::array<unsigned char>(judyArray->_maxValueLength + 1))
{
}

generic<typename TValue>
KeyValuePair<String^, TValue> JudyHybridArrayEnumerator<TValue>::Current::get()
{
	if (_judyArray->_enumeratorVersion != _version) throw gcnew InvalidOperationException("Enumerator version.");
	if (_state != 1) throw gcnew InvalidOperationException("Enumerator is not positioned.");

	return _current;
}

generic<typename TValue>
bool JudyHybridArrayEnumerator<TValue>::MoveNext()
{
	if (_judyArray->_enumeratorVersion != _version) throw gcnew InvalidOperationException("Enumerator version.");
	if (_state == 2) return false;

	Pvoid_t valuePtr = NULL;
	Pvoid_t judyArrayPtr = _judyArray->_judyArrayPtr;
	pin_ptr<unsigned char> bufferPin = &_buffer[0];

	if (_state == 0)
	{
		_buffer[0] = 0;
		JSLF(valuePtr, judyArrayPtr, (unsigned char*)bufferPin);

		if (valuePtr == NULL)
		{
			_state = 2;
		}
		else
		{
			_state = 1;
			
			String^ key = Encoding::UTF8->GetString(_buffer, 0, strlen((const char*)bufferPin));
			TValue value = _judyArray->GetValue((PWord_t)valuePtr);
			_current = KeyValuePair<String^, TValue>(key, value);
		}
	}
	else if (_state == 1)
	{
		JSLN(valuePtr, judyArrayPtr, (unsigned char*)bufferPin);

		if (valuePtr == NULL)
		{
			_state = 2;
		}
		else
		{
			String^ key = Encoding::UTF8->GetString(_buffer, 0, strlen((const char*)bufferPin));
			TValue value = _judyArray->GetValue((PWord_t)valuePtr);
			_current = KeyValuePair<String^, TValue>(key, value);
		}
	}

	return _state == 1;
}

generic<typename TValue>
void JudyHybridArrayEnumerator<TValue>::Reset()
{
	if (_judyArray->_enumeratorVersion != _version) throw gcnew InvalidOperationException("Enumerator version.");
	_state = 0;
}

generic<typename TValue>
ref class PrimitiveJudyHybridArray : public JudyHybridArray<TValue>
{
public:
	virtual TValue GetValue(PWord_t entry) override
	{
		TValue value;
		memcpy(&value, entry, sizeof(TValue));
		return value;
	}

protected:
	virtual void Set(PWord_t entry, TValue value) override
	{
		memcpy(entry, &value, sizeof(TValue));
	}
};

generic<typename TValue>
ref class ReferenceJudyHybridArray : public JudyHybridArray<TValue>
{
public:
	virtual bool Remove(String^ key) override
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

protected:
	virtual void Dispose(Pvoid_t judyArrayPtr) override
	{
		std::vector<uint8_t> index(_maxValueLength + 1);
		Pvoid_t valuePtr = NULL;

		index[0] = '\0';
		JSLF(valuePtr, judyArrayPtr, &index[0]);

		while (valuePtr != NULL)
		{
			Free((PWord_t)valuePtr);
			JSLN(valuePtr, judyArrayPtr, &index[0]);
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
#ifdef _WIN64
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
bool JudyStringDictionary<TValue>::Remove(String^ key)
{
	if (key == nullptr) throw gcnew ArgumentNullException("key");
	return _judyArray->Remove(key);
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
bool JudyStringDictionary<TValue>::ContainsKey(String^ key)
{
	return _judyArray->ContainsKey(key);
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

generic<typename TValue>
IEnumerator<KeyValuePair<String^, TValue>>^ JudyStringDictionary<TValue>::GetEnumerator()
{
	return _judyArray->GetEnumerator();
}
