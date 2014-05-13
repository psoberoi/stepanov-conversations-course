// compile using ldc from https://github.com/ldc-developers/ldc/releases/tag/v0.13.0-alpha2
// ldmd2 bigram.d -O -release -inline -noboundscheck

import std.stdio;
import std.algorithm;
import std.range;
import std.file : read;
import std.typecons;

alias prefix_t = ulong;

bool myisspace(char x) {
  if (x > ' ') return false;
  return (x == ' ' || x == '\r' || x == '\t' || x == '\n');
}

ubyte high_byte(N)(N x) {
  return cast(ubyte)(x >> ((N.sizeof - 1) * 8));
}

void print_prefix(prefix_t prefix) {
  auto p = prefix;
  while (p) {
    char c = high_byte(p);
    writef("%c", c);
    p <<= 8;
  }
}

struct PrefixSplitRange
{
  const(char)[] input;
  prefix_t prefix;

  this(const(char)[] input)
  {
    this.input = input;
    popFront();
  }

  bool empty()
  {
    return prefix == 0;
  }

  prefix_t front()
  {
    return prefix;
  }

  void popFront()
  {
    size_t startIndex = 0;
    prefix = 0;
    foreach(size_t i, char c; input)
    {
      if(!myisspace(c))
      {
        startIndex = i;
        break;
      }
    }
    if(startIndex >= input.length)
      return;
    input = input[startIndex..$];
    
    auto offset = prefix_t.sizeof;

    size_t len;
    foreach(char c; input)
    {
      len++;
      if(myisspace(c)) 
      {
        break;
      }
      if (offset) {
        prefix <<= 8;
        prefix |= cast(ubyte)(c);
        --offset;
      }
    }
    input = input[len..$];
  }  
}

auto prefixSplit(const(char)[] input)
{
  return PrefixSplitRange(input);
}

struct BigramRange(T)
{
  alias ET = ElementType!T;
  T input;
  ET last;
  
  this(T input)
  {
    this.input = input;
    this.input.empty;
    last = this.input.front;
    this.input.popFront();
  }
  
  auto front()
  {
    return tuple(last, input.front);
  }
  
  bool empty()
  {
    return input.empty;
  }
  
  void popFront()
  {
    last = input.front;
    input.popFront();
  }
}

auto bigram(T)(T input)
{
  return BigramRange!T(input);
}

struct CountReduceRange(T)
{
  T input;
  
  this(T input)
  {
    this.input = input;
  }
  
  bool empty()
  {
    return input.empty;
  }
  
  // unsafe assumption that front may only be called once per call to popFront()
  auto front()
  {
    auto first = input.front;
    input.popFront();
    uint count = 1;
    while(!input.empty && input.front == first)
    {
      count++;
      input.popFront();
    }
    return tuple(count, first);
  }
  
  auto popFront()
  {
    
  }
}

auto countReduce(T)(T input)
{
  return CountReduceRange!T(input);
}

int main(string[] args)
{
  if(args.length != 2)
  {
    writefln("one argument expected");
    return 1;
  }
  auto fileContents = cast(const(char)[])read(args[1]);
  auto result = 
  fileContents.prefixSplit // convert to prefixes and split
  .bigram // create bigrams
  .array // store in array
  .sort!"a[0] < b[0] || (a[0] == b[0] && a[1] < b[1])" // sort bigram array
  .countReduce // count and merge equal bigrams
  .array // store in array
  .sort!"a[0] > b[0]" // sort array
  .take(250); // use first 250 entries from array
  foreach(entry; result)
  {
    writef("%d ", entry[0]);
    print_prefix(entry[1][0]);
    writef(" ");
    print_prefix(entry[1][1]);
    writef("\n");
  }
  return 0;
}