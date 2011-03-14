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

using namespace System;

namespace JudyArray {

	generic<typename TValue>
	ref class JudyHybridArray;

	/// <summary>
	/// JudySL array.
	/// </summary>
	/// <typeparam name="TValue">Type of the dictionary value.</typeparam>
	generic<typename TValue>
	public ref class JudyStringDictionary : System::Collections::Generic::IEnumerable<System::Collections::Generic::KeyValuePair<System::String^, TValue>>
	{
	private:

		JudyHybridArray<TValue>^ _judyArray;

	public:

		/// <summary>
		/// Initialize a new instance.
		/// </summary>
		JudyStringDictionary(void);

		virtual ~JudyStringDictionary(void);

		/// <summary>
		/// Remove a key from the dictionary.
		/// </summary>
		/// <param name="key">Key to remove.</param>
		/// <returns>True if the key was found and removed.</returns>
		/// <exception cref="System::ArgumentNullException">If <paramref name="key"/> is a null reference.</exception>
		bool Remove(System::String^ key);

		/// <summary>
		/// Try and get a value from the dictionary.
		/// </summary>
		/// <param name="key">Key to get the value for.</param>
		/// <param name="value">Value for the specified key.</param>
		/// <returns>True if the key was found.</returns>
		/// <exception cref="System::ArgumentNullException">If <paramref name="key"/> is a null reference.</exception>
		bool TryGetValue(System::String^ key, [System::Runtime::InteropServices::Out] TValue% value);

		/// <summary>
		/// Test if the dictionary contains the specified key.
		/// </summary>
		/// <returns>True if the dictionary contains the key.</returns>
		/// <exception cref="System::ArgumentNullException">If <paramref name="key"/> is a null reference.</exception>
		bool ContainsKey(System::String^ key);

		/// <summary>
		/// Get or set the value for a given key.
		/// </summary>
		/// <exception cref="System::Collections::Generic::KeyNotFoundException">If the specified key is not found.</exception>
		/// <exception cref="System::ArgumentNullException">If <paramref name="key"/> is a null reference.</exception>
		property TValue default [System::String^]
		{
			TValue get(System::String^ key);
			void set(System::String^ key, TValue value);
		}

		/// <summary>
		/// Remove all items from the dictionary.
		/// </summary>
		void Clear();

		virtual System::Collections::Generic::IEnumerator<System::Collections::Generic::KeyValuePair<System::String^, TValue>>^ GetEnumerator();

		virtual System::Collections::IEnumerator^ EnumerableGetEnumerator() = System::Collections::IEnumerable::GetEnumerator
		{
			return GetEnumerator();
		}
	};
}
