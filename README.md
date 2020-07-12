# PPparsing

C++ token parsing library.

## Build info

It is intended to be built as a dynamic library.

## Exported functions

Provides two exported functions:

[`RangeSpan GetTokensRangeImplementation(CharSpan input);`](#RangeSpan-GetTokensRangeImplementationCharSpan-input)

and

[`void ReleaseResources();`](#void-ReleaseResources)

## Memory layout

#### Char

A 2-byte integer.

#### CharSpan

```cpp
CharSpan
{
	Char*;
	Char*;
};
```

#### RangeSpan

```cpp
RangeSpan
{
	CharSpan*;
	CharSpan*;
};
```

## Definitions

#### range

A pair of pointers (begin, end) denoting a contiguous "range" of elements in memory.

#### raw token

A substring of characters which are either non-space or a space character enclosed in quotes.

#### token

A [raw token](#raw-token) with removed quotes.

## Usage

#### RangeSpan GetTokensRangeImplementation(CharSpan input)

Returns a [range](#range) of tokens in input.

[Token](#token) is represented as a range of characters.

Allocates memory for the ranges.

#### void ReleaseResources()

Releases memory allocated by `RangeSpan GetTokensRangeImplementation(CharSpan input)`
