using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stepanov.QuickSort
{
    public static class QuickSort64
    {
        static void UnguardedLinearInsert(long[] array, int last, long value) {
            int previous = last;
            while (value < array[--previous])
                array[last--] = array[previous];
            array[last] = value;
        }

        static void UnguardedInsertionSort(long[] array, int first, int last) {
            for (int i = first; i != last; ++i)
                UnguardedLinearInsert(array, i, array[i]);
        }

        static void LinearInsert(long[] array, int first, int last, long value) {
            if (value < array[first]) {
                while (first != last--)
                    array[last + 1] = array[last];
                array[first] = value;
            } else {
                UnguardedLinearInsert(array, last, value);
            }
        }

        static void InsertionSort(long[] array, int first, int last) {
            if (first == last)
                return;
            for (int i = first + 1; i != last; ++i)
                LinearInsert(array, first, i, array[i]);
        }

        public static long MedianOf3(long a, long b, long c) {
            if (a < b) {
                if (b < c)
                    return b;
                else if (a < c)
                    return c;
                else
                    return a;
            } else if (a < c)
                return a;
            else if (b < c)
                return c;
            else
                return b;
        }

        static int UnguardedPartition(long[] array, int first, int last, long pivot) {
            --last;
            while (array[first] < pivot)
                ++first;
            while (pivot < array[last])
                --last;
            while (first < last) {
                long tmp = array[first];
                array[first] = array[last];
                array[last] = tmp;
                ++first;
                --last;
                while (array[first] < pivot)
                    ++first;
                while (pivot < array[last])
                    --last;
            }
            return first;
        }

        static void QuickSortLoop(long[] array, int first, int last, int threshold) {
            int len = last - first;
            while (len > threshold) {
                int middle = first + (last - first) / 2;
                long pivot = MedianOf3(array[first], array[middle], array[last - 1]);
                int cut = UnguardedPartition(array, first, last, pivot);
                if (last - cut < cut - first) {
                    QuickSortLoop(array, cut, last, threshold);
                    last = cut;
                } else {
                    QuickSortLoop(array, first, cut, threshold);
                    first = cut;
                }
                len = last - first;
            }
        }

        public static void QuickSort(this long[] array, int first, int last) {
            int len = last - first;
            int threshold = 16;
            if (len <= threshold) {
                InsertionSort(array, first, last);
            } else {
                QuickSortLoop(array, first, last, threshold);
                int middle = first + threshold;
                InsertionSort(array, first, middle);
                UnguardedInsertionSort(array, middle, last);
            }
        }
    }
}
