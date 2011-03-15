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
#include "JudyBitArray32.h"
#include <Judy.h>

using namespace JudyArray;
using namespace System;
using namespace System::Collections::Generic;

JudyBitArray32::JudyBitArray32()
  : _judyArrayPtr(NULL), _enumeratorVersion(0)
{
}

JudyBitArray32::~JudyBitArray32()
{
  if (_judyArrayPtr != NULL)
  {
    Word_t bytes;
    Pvoid_t judyArrayPtr = _judyArrayPtr;
    
    J1FA(bytes, judyArrayPtr);
    
    _enumeratorVersion += 1;
    _judyArrayPtr = NULL;
  }
}

void JudyBitArray32::Set(unsigned int index, bool value)
{
  int rc;
  Pvoid_t judyArrayPtr = _judyArrayPtr;

  if (value)
  {
    J1S(rc, judyArrayPtr, (Word_t)index);
  }
  else
  {
    J1U(rc, judyArrayPtr, (Word_t)index);
  }

  if (rc == 1) _enumeratorVersion += 1;

  _judyArrayPtr = judyArrayPtr;
}

void JudyBitArray32::Set(int index, bool value)
{
  if (index < 0) throw gcnew ArgumentOutOfRangeException("index", index, "Value must be greater than or equal to 0.");
  Set((unsigned int)index, value);
}

bool JudyBitArray32::Get(unsigned int index)
{
  int rc;
  Pvoid_t judyArrayPtr = _judyArrayPtr;

  J1T(rc, judyArrayPtr, (Word_t)index);

  return rc == 1;
}

bool JudyBitArray32::Get(int index)
{
  if (index < 0) throw gcnew ArgumentOutOfRangeException("index", index, "Value must be greater than or equal to 0.");
  return Get((unsigned int)index);
}

__int64 JudyBitArray32::Count::get()
{
  int rc;
  Pvoid_t judyArrayPtr = _judyArrayPtr;
  J1C(rc, judyArrayPtr, 0, -1);
  return (unsigned int)rc;
}

IEnumerator<__int64>^ JudyBitArray32::GetEnumerator()
{
  // Only one enumerator is allowed at a time
  _enumeratorVersion += 1;
  return gcnew JudyBitArray32Enumerator(this);
}

JudyBitArray32::JudyBitArray32Enumerator::JudyBitArray32Enumerator(JudyBitArray32^ bitArray)
  : _bitArray(bitArray), _version(bitArray->_enumeratorVersion), _state(0), _currentIndex(0)
{
}

JudyBitArray32::JudyBitArray32Enumerator::~JudyBitArray32Enumerator()
{
  _state = 3;
}

bool JudyBitArray32::JudyBitArray32Enumerator::MoveNext()
{
  if (_state == 3) throw gcnew ObjectDisposedException(this->ToString());
  if (_version != _bitArray->_enumeratorVersion) throw gcnew InvalidOperationException("Collection has been modified.");

  if (_state == 2) return false;

  int rc;
  Pvoid_t judyArrayPtr = _bitArray->_judyArrayPtr;
  Word_t index = _currentIndex;

  if (_state == 0)
  {
    J1F(rc, judyArrayPtr, index);
  }
  else
  {
    J1N(rc, judyArrayPtr, index);
  }

  if (rc == 0)
  { 
    _state = 2;
  }
  else
  {
    _state = 1;
    _currentIndex = index;
  }

  return _state == 1;
}

void JudyBitArray32::JudyBitArray32Enumerator::Reset()
{
  if (_state == 3) throw gcnew ObjectDisposedException(this->ToString());
  if (_version != _bitArray->_enumeratorVersion) throw gcnew InvalidOperationException("Collection has been modified.");

  _state = 0;
  _currentIndex = 0;
}

__int64 JudyBitArray32::JudyBitArray32Enumerator::Current::get()
{
  if (_state == 3) throw gcnew ObjectDisposedException(this->ToString());
  if (_state != 1) throw gcnew InvalidOperationException("Enumerator is not positioned.");
  return _currentIndex;
}
