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
	}
}
