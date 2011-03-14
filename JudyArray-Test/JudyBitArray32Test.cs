using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using JudyArray;

namespace JudyArray_Test
{
	/// <summary>
	/// Test fixture for <see cref="JudyBitArray32"/>.
	/// </summary>
	[TestFixture]
	public class JudyBitArray32Test
	{
		[Test]
		public void Set()
		{
			JudyBitArray32 bitArray = new JudyBitArray32();

			// Min
			Assert.IsFalse(bitArray.Get(0));
			bitArray.Set(0, true);
			Assert.IsTrue(bitArray.Get(0));
			bitArray.Set(0, false);
			Assert.IsFalse(bitArray.Get(0));

			// Max
			Assert.IsFalse(bitArray.Get(uint.MaxValue));
			bitArray.Set(uint.MaxValue, true);
			Assert.IsTrue(bitArray.Get(uint.MaxValue));
			bitArray.Set(uint.MaxValue, false);
			Assert.IsFalse(bitArray.Get(uint.MaxValue));
		}

		[Test]
		public void Count()
		{
			JudyBitArray32 bitArray = new JudyBitArray32();
			Assert.AreEqual(0, bitArray.Count);

			bitArray.Set(0, true);
			Assert.AreEqual(1, bitArray.Count);

			bitArray.Set(1, true);
			Assert.AreEqual(2, bitArray.Count);

			bitArray.Set(0, true);
			Assert.AreEqual(2, bitArray.Count);

			bitArray.Set(uint.MaxValue, true);
			Assert.AreEqual(3, bitArray.Count);

			bitArray.Set(1, false);
			Assert.AreEqual(2, bitArray.Count);

			bitArray.Set(0, false);
			Assert.AreEqual(1, bitArray.Count);

			bitArray.Set(uint.MaxValue, false);
			Assert.AreEqual(0, bitArray.Count);
		}

		[Test]
		public void Enumerator()
		{
			JudyBitArray32 bitArray = new JudyBitArray32();

			CollectionAssert.IsEmpty(bitArray.ToArray());

			bitArray.Set(0, true);
			bitArray.Set(UInt32.MaxValue, true);

			CollectionAssert.AreEqual(new[] { 0L, UInt32.MaxValue }, bitArray.ToArray());
		}

		[Test]
		public void Enumerator__Errors()
		{
			JudyBitArray32 bitArray = new JudyBitArray32();
			bitArray.Set(0, true);
			bitArray.Set(1, true);

			IEnumerator<long> bitArrayEnum = bitArray.GetEnumerator();

			// MoveNext must be called
			Assert.Throws<InvalidOperationException>(() => { var x = bitArrayEnum.Current; });

			Assert.IsTrue(bitArrayEnum.MoveNext());

			// Only one enumerator at a time allowed
			var bitArrayEnum2 = bitArray.GetEnumerator();
			Assert.Throws<InvalidOperationException>(() => bitArrayEnum.MoveNext());

			// Modification invalidates enumerator
			bitArrayEnum = bitArray.GetEnumerator();
			bitArray.Set(100, true);
			Assert.Throws<InvalidOperationException>(() => bitArrayEnum.MoveNext());
		}
	}
}
