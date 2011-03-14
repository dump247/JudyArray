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

#pragma once

namespace JudyArray {

  /// <summary>
	/// Judy bit array with 32-bit index.
	/// </summary>
  /// <remarks>
  /// The bit array is unbounded, so any index between 0 and
  /// <see cref="System::UInt32::MaxValue"/> is valid.
  /// </remarks>
  public ref class JudyBitArray32
  {
  private:

    void* _judyArrayPtr;

  public:

    /// <summary>
		/// Initialize a new instance with all bits clear.
		/// </summary>
    JudyBitArray32();

    virtual ~JudyBitArray32();

    /// <summary>
		/// Set the bit at a specific position in the bit array.
		/// </summary>
    /// <param name="index">Index of the bit to set.</param>
    /// <param name="value">The value to assign to the bit.</param>
    void Set(unsigned int index, bool value);

    /// <summary>
		/// Set the bit at a specific position in the bit array.
		/// </summary>
    /// <param name="index">Index of the bit to set.</param>
    /// <param name="value">The value to assign to the bit.</param>
    /// <exception cref="System::ArgumentOutOfRangeException">If <paramref name="index"/> is less than 0.</exception>
    void Set(int index, bool value);

    /// <summary>
		/// Get the value of the bit at a specific position in the bit array.
		/// </summary>
    /// <param name="index">Index of the bit to get.</param>
    /// <returns>True if the bit is set.</returns>
    bool Get(unsigned int index);

    /// <summary>
		/// Get the value of the bit at a specific position in the bit array.
		/// </summary>
    /// <param name="index">Index of the bit to get.</param>
    /// <returns>True if the bit is set.</returns>
    /// <exception cref="System::ArgumentOutOfRangeException">If <paramref name="index"/> is less than 0.</exception>
    bool Get(int index);

    /// <summary>
    /// Get the number of set bits in the bit array.
    /// </summary>
    virtual property __int64 Count { __int64 get(); };
  };

}
