import java.util.Random;
import java.lang.Math;

public class Sort64 {

  public static
  void random_shuffle(long[] array, int first, int last) {
    Random rng = new Random();
    for (int i = first + 1; i < last; ++i) {
      int randomPlace =  Math.abs(rng.nextInt()) % ((i - first) + 1);
      long tmp = array[randomPlace];
      array[randomPlace] = array[i];
      array[i] = tmp;
    }
  }

  public static
  void iota(long[] array) {
    for (int i = 0; i < array.length; ++i) {
      array[i] = (long)i;
    }
  }

  public static
  void main(String[] args) {
    int min_size = 8;
    int max_size = 16 * 1024 * 1024;

    System.out.printf("%12s %6s %6s\n", "size", "time", "log2");

    long[] data = new long[max_size];
    long[] buffer = new long[max_size];
    iota(data);
    random_shuffle(data, 0, data.length);

    int lg = 3;
    for (int array_size = min_size; array_size <= max_size; array_size *= 2) {
      long time = timeSort(data, buffer, array_size);
      System.out.printf("%12d %6d %6.1f\n", array_size, time / max_size, (double) time / (max_size * lg));
      ++lg;
    }
  }

  static
  long timeSort(long[] data, long[] buffer, int size) {
    int first = 0;
    long startTime = System.nanoTime();
    while (first <= data.length - size) {
      System.arraycopy(data, first, buffer, 0, size);
      Quicksort64.quicksort(buffer, 0, size);
      first += size;
    }
    return System.nanoTime() - startTime;
  }
}
