using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stepanov.QuickSort
{
    /// <summary>
    /// C# implementation of quick sort implementation from Stepanov conversations course.
    /// Original code and data can be found at <https://github.com/psoberoi/stepanov-conversations-course>.
    /// Conversations can be watched on <https://www.youtube.com/playlist?list=PLHxtyCq_WDLXFAEA-lYoRNQIezL_vaSX->.
    /// </summary>
    class Program
    {
        const int minSize = 8;
        const int maxSize = 16 * 1024 * 1024;

        readonly long[] _data;
        readonly long[] _buffer;

        Program() {
            _data = new long[maxSize];
            _buffer = new long[maxSize];
            _data.Iota<long>().Suffle();
        }

         long TimeSort(int size) {
             var stopwatch = Stopwatch.StartNew();

             int first = 0;
             while (first <= maxSize - size) {
                 Array.Copy(_data, _buffer, size);
                 _buffer.QuickSort(0, size);
                 //Array.Sort(_buffer, 0, size);
                 first += size;
             }

             return (long)(stopwatch.Elapsed.TotalMilliseconds * 1000000L);
        }

         void Run() {
             Console.WriteLine("{0,-12} {1,6} {2,6}", "size", "time", "log2");

             int lg = 3;
             for (int arraySize = minSize; arraySize <= maxSize; arraySize *= 2) {
                 long time = TimeSort(arraySize);
                 Console.WriteLine("{0,-12} {1,6} {2,6:N2}", arraySize, time / maxSize, (double)time / (maxSize * lg));

                 ++lg;
             }
         }

        static void Main(string[] args) {
            new Program().Run();
        }
    }

    public static class ArrayExtensions
    {
        public static T[] Suffle<T>(this T[] array, int index, int count, Random random) {
            for (int i = 1; i < count; ++i) {
                int randomPlace = Math.Abs(random.Next()) % (i + 1);
                T tmp = array[randomPlace];
                array[randomPlace] = array[i + index];
                array[i + index] = tmp;
            }
            return array;
        }

        public static T[] Suffle<T>(this T[] array) {
            return Suffle(array, 0, array.Length, new Random());
        }

        public static long[] Iota<T>(this long[] array) {
            for (int i = 0; i < array.Length; ++i) {
                array[i] = (long)i;
            }
            return array;
        }
    }
}
