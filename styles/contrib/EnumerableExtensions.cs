using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stepanov.Bigram
{
    public static partial class EnumerableExtensions
    {
        public static IEnumerable<T> MergeWith<T>(this IEnumerable<T> self, IEnumerable<T> other, IComparer<T> comparer) {
            var enumerator1 = self.GetEnumerator();
            var hasValue1 = enumerator1.MoveNext();
            var enumerator2 = other.GetEnumerator();
            var hasValue2 = enumerator2.MoveNext();

            if (!hasValue1) {
                while (hasValue2) {
                    yield return enumerator2.Current;
                    hasValue2 = enumerator2.MoveNext();
                }
                yield break;
            }
            if (!hasValue2) {
                while (hasValue1) {
                    yield return enumerator1.Current;
                    hasValue1 = enumerator1.MoveNext();
                }
                yield break;
            }
            while (true) {
                if (comparer.Compare(enumerator2.Current, enumerator1.Current) < 0) {
                    yield return enumerator2.Current;
                    hasValue2 = enumerator2.MoveNext();
                    if (!hasValue2) {
                        do {
                            yield return enumerator1.Current;
                        } while (enumerator1.MoveNext());
                        yield break;
                    }
                } else {
                    yield return enumerator1.Current;
                    hasValue1 = enumerator1.MoveNext();
                    if (!hasValue1) {
                        do {
                            yield return enumerator2.Current;
                        } while (enumerator2.MoveNext());
                        yield break;
                    }
                }
            }
        }
    }
}
