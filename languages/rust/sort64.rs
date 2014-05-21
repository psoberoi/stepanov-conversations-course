extern crate rand;
extern crate time;
use rand::Rng;
use std::slice::MutableCloneableVector;

mod quicksort64;

fn time_sort(data: &[u64], buffer: &mut [u64], size: uint) -> u64 {
  let mut first = 0u;
  let start_time = time::precise_time_ns();
  while first <= data.len() - size {
    buffer.mut_slice(0, size).copy_from(data.slice(first, first + size));
    quicksort64::quicksort(buffer.as_mut_slice(), 0, size);
    first += size;
  }
  time::precise_time_ns() - start_time
}

fn main() {
  let min_size = 8u;
  let max_size = 16u * 1024u * 1024u;

  println!("           {:>12s} {:>6s} {:>6s}", "size", "time", "log2");

  let mut data = Vec::from_fn(max_size, |i| { i as u64 });
  let mut buffer = data.clone();

  let mut rng = rand::task_rng();
  rng.shuffle(data.as_mut_slice());

  let mut lg = 3;
  let mut array_size = min_size;
  while array_size <= max_size {
    let time = time_sort(data.as_slice(), buffer.as_mut_slice(), array_size);
    println!("    [Rust] {:12u} {:6u} {:6.1f}",
             array_size, time / max_size as u64, time as f64 / (max_size * lg) as f64);
    lg += 1;
    array_size *= 2;
  }
}
