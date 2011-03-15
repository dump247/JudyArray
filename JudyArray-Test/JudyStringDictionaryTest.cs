using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using JudyArray;

namespace JudyArray_Test
{
    [TestFixture]
    public class JudyStringDictionaryTest
    {
        [Test]
        public void Constructor__Int32()
        {
            JudyStringDictionary<int> dic = new JudyStringDictionary<int>();
            dic.Dispose();
        }

        [Test]
        public void Enumerator()
        {
            JudyStringDictionary<int> dic = new JudyStringDictionary<int>();

            Assert.AreEqual(0, dic.Count());

            dic["key"] = 17;

            CollectionAssert.AreEqual(new[] {
                new KeyValuePair<string,int>("key", 17)
            }, dic.ToArray());

            dic["key2"] = 11;

            CollectionAssert.AreEqual(new[] {
                new KeyValuePair<string,int>("key", 17),
                new KeyValuePair<string,int>("key2", 11)
            }, dic.ToArray());

            dic["akey"] = 22;

            CollectionAssert.AreEqual(new[] {
                new KeyValuePair<string,int>("akey", 22),
                new KeyValuePair<string,int>("key", 17),
                new KeyValuePair<string,int>("key2", 11)
            }, dic.ToArray());

            dic.Dispose();
        }

        [Test]
        public void Enumerator__Errors()
        {
            JudyStringDictionary<int> dic = new JudyStringDictionary<int>();
			dic["key"] = 17;
			dic["key2"] = 18;

            IEnumerator<KeyValuePair<string, int>> dicEnum = dic.GetEnumerator();

            // MoveNext must be called
            Assert.Throws<InvalidOperationException>(() => { var x = dicEnum.Current; });

			Assert.IsTrue(dicEnum.MoveNext());

            // Only one enumerator at a time allowed
            var dicEnum2 = dic.GetEnumerator();
            Assert.Throws<InvalidOperationException>(() => dicEnum.MoveNext());

            // Modification invalidates enumerator
            dicEnum = dic.GetEnumerator();
            dic["key"] = 10;
            Assert.Throws<InvalidOperationException>(() => dicEnum.MoveNext());
        }

        [Test]
        public void ContainsKey()
        {
            JudyStringDictionary<int> dic = new JudyStringDictionary<int>();

            Assert.IsFalse(dic.ContainsKey("key"));
            dic["key"] = 17;
            Assert.IsTrue(dic.ContainsKey("key"));

            Assert.IsFalse(dic.ContainsKey("key1"));

            dic.Dispose();
        }

        [Test]
        public void Clear()
        {
            JudyStringDictionary<int> dic = new JudyStringDictionary<int>();

            dic.Clear();

            dic["key"] = 17;
            Assert.AreEqual(17, dic["key"]);

            dic.Clear();
            Assert.IsFalse(dic.ContainsKey("key"));

            dic.Dispose();
        }

        [Test]
        public void Int32()
        {
            JudyStringDictionary<int> dic = new JudyStringDictionary<int>();
            dic["key1"] = 17;
            dic["key3"] = int.MinValue;
            dic["key4"] = int.MaxValue;
            dic["key5"] = 0;

            Assert.AreEqual(17, dic["key1"]);

            int value;
            Assert.IsTrue(dic.TryGetValue("key3", out value));
            Assert.AreEqual(int.MinValue, value);

            Assert.IsTrue(dic.TryGetValue("key4", out value));
            Assert.AreEqual(int.MaxValue, value);

            Assert.IsTrue(dic.TryGetValue("key5", out value));
            Assert.AreEqual(0, value);

            Assert.IsFalse(dic.TryGetValue("key2", out value));
            Assert.AreEqual(0, value);

            Assert.IsTrue(dic.Remove("key1"));

            Assert.IsFalse(dic.TryGetValue("key1", out value));
            Assert.AreEqual(0, value);

            Assert.IsFalse(dic.Remove("key1"));

            dic.Dispose();
        }

        [Test]
        public void Int64()
        {
            JudyStringDictionary<long> dic = new JudyStringDictionary<long>();
            dic["key1"] = 17;
            dic["key3"] = long.MinValue;
            dic["key4"] = long.MaxValue;
            dic["key5"] = 0;

            Assert.AreEqual(17, dic["key1"]);

            long value;
            Assert.IsTrue(dic.TryGetValue("key3", out value));
            Assert.AreEqual(long.MinValue, value);

            Assert.IsTrue(dic.TryGetValue("key4", out value));
            Assert.AreEqual(long.MaxValue, value);

            Assert.IsTrue(dic.TryGetValue("key5", out value));
            Assert.AreEqual(0, value);

            Assert.IsFalse(dic.TryGetValue("key2", out value));
            Assert.AreEqual(0, value);

            Assert.IsTrue(dic.Remove("key1"));

            Assert.IsFalse(dic.TryGetValue("key1", out value));
            Assert.AreEqual(0, value);

            Assert.IsFalse(dic.Remove("key1"));

            dic.Dispose();
        }

        [Test]
        public void Double()
        {
            JudyStringDictionary<double> dic = new JudyStringDictionary<double>();
            dic["key1"] = 17.235;
            dic["key3"] = double.MinValue;
            dic["key4"] = double.MaxValue;
            dic["key5"] = 0;

            Assert.AreEqual(17.235, dic["key1"]);

            double value;
            Assert.IsTrue(dic.TryGetValue("key3", out value));
            Assert.AreEqual(double.MinValue, value);

            Assert.IsTrue(dic.TryGetValue("key4", out value));
            Assert.AreEqual(double.MaxValue, value);

            Assert.IsTrue(dic.TryGetValue("key5", out value));
            Assert.AreEqual(0, value);

            Assert.IsFalse(dic.TryGetValue("key2", out value));
            Assert.AreEqual(0, value);

            Assert.IsTrue(dic.Remove("key1"));

            Assert.IsFalse(dic.TryGetValue("key1", out value));
            Assert.AreEqual(0, value);

            Assert.IsFalse(dic.Remove("key1"));

            dic.Dispose();
        }

        [Test]
        public void Reference()
        {
            Version v1 = new Version();
            Version v2 = new Version(1, 2);
            JudyStringDictionary<Version> dic = new JudyStringDictionary<Version>();
            dic["key1"] = v1;
            dic["key3"] = v2;
            dic["key4"] = null;

            Assert.AreSame(v1, dic["key1"]);

            Version value;
            Assert.IsTrue(dic.TryGetValue("key3", out value));
            Assert.AreSame(v2, value);

            Assert.IsTrue(dic.TryGetValue("key4", out value));
            Assert.IsNull(value);

            Assert.IsFalse(dic.TryGetValue("key2", out value));
            Assert.IsNull(value);

            Assert.IsTrue(dic.Remove("key1"));

            Assert.IsFalse(dic.TryGetValue("key1", out value));
            Assert.IsNull(value);

            Assert.IsFalse(dic.Remove("key1"));

            dic.Dispose();
        }

        [Test]
        public void Reference__Release()
        {
            Version value = new Version();
            JudyStringDictionary<Version> dic = new JudyStringDictionary<Version>();
            dic["key1"] = value;

            WeakReference weakValue = new WeakReference(value);
            value = null;
            GC.Collect();
            Assert.IsNotNull(weakValue.Target);

            dic.TryGetValue("key1", out value);
            Assert.AreSame(weakValue.Target, value);
            value = null;

            dic.Remove("key1");
            GC.Collect();
            Assert.IsNull(weakValue.Target);

            dic.Dispose();
        }
    }
}
