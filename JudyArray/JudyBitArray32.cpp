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

JudyBitArray32::JudyBitArray32()
  : _judyArrayPtr(NULL)
{
}

JudyBitArray32::~JudyBitArray32()
{
  if (_judyArrayPtr != NULL)
  {
    Word_t bytes;
    Pvoid_t judyArrayPtr = _judyArrayPtr;
    
    J1FA(bytes, judyArrayPtr);
    
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
