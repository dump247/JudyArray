// JudyStringDictionary.h

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
	public ref class JudyStringDictionary
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
		bool RemoveKey(System::String^ key);

		/// <summary>
		/// Try and get a value from the dictionary.
		/// </summary>
		/// <param name="key">Key to get the value for.</param>
		/// <param name="value">Value for the specified key.</param>
		/// <returns>True if the key was found.</returns>
		/// <exception cref="System::ArgumentNullException">If <paramref name="key"/> is a null reference.</exception>
		bool TryGetValue(System::String^ key, [System::Runtime::InteropServices::Out] TValue% value);

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
	};
}
