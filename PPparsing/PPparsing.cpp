#include <vector>



static_assert(sizeof(int) == 4);



// supports 2-byte characters only for now
static_assert(sizeof(short) == 2);
using Char = short;



#pragma pack(push, 8)
template <typename T>
struct Span
{
	T* begin;
	T* end;

	constexpr operator bool() noexcept
	{
		return begin != end;
	}
};

// wraps Span with C style structs,
// because C linkage doesn't allow templated classes in signatures
struct RangeSpan { Span<Span<Char>> x; };
struct CharSpan { Span<Char> x; };
#pragma pack(pop)



// ugly mess to get an r-value of pointer type from Span
template <typename Range>
constexpr std::add_rvalue_reference_t<decltype(*std::declval<Range>().begin)> declval_range() noexcept;



// skips elements from the start while `predicate` is true
template <typename Range, typename Predicate>
constexpr Range skip_while(Range input, Predicate predicate) noexcept(noexcept(predicate(declval_range<Range>())))
{
	return { std::find_if(input.begin, input.end, negate(predicate)), input.end };
}

// removes elements for which `predicate` is true
template <typename Range, typename Predicate>
constexpr Range remove(Range input, Predicate predicate) noexcept(noexcept(predicate(declval_range<Range>())))
{
	return { input.begin, std::remove_if(input.begin, input.end, predicate) };
}



// negates a predicate - `negate(predicate)(x)` returns false iff `predicate(x)` returns true
template <typename F>
constexpr auto negate(F&& predicate) noexcept
{
	return[&predicate]<typename... T>(T &&... args)
	{
		return !std::forward<F>(predicate)(std::forward<T>(args)...);
	};
}



template <Char c>
constexpr inline auto is_char = [](auto x) noexcept { return x == c; };

// used for parsing characters inside quotes
// not safe to have it global, but it is the simplest solution in this case
static bool in_quotes;

constexpr inline auto is_nonspace_or_in_quotes =
[](auto c) noexcept
{
	if (c == '"')
	{
		in_quotes = true;
		return true;
	}
	else
		return c != ' ' || in_quotes;
};



// `input` must begin with a non-space character
// returns a pair (token, rest), where:
//		token is the first token in `input`,
//		rest is the rest of `input` without leading spaces
// modifies the underlying data in order to remove quotes from the token
std::pair<Span<Char>, Span<Char>> split_one_token(Span<Char> input) noexcept
{
	auto begin = input.begin;

	in_quotes = false;

	input = skip_while(input, is_nonspace_or_in_quotes);

	Span<Char> token{ begin, input.begin };

	return { remove(token, is_char<'"'>), skip_while(input, is_char<' '>) };
}

// the container holding the ranges returned by `GetTokensRangeImplementation`
static std::vector<Span<Char>> ranges;

// returns a range containing all tokens in `input`
// token is represented as a range of characters
Span<Span<Char>> get_tokens(Span<Char> input)
{
	ranges.clear();

	input = skip_while(input, is_char<' '>);

	while (input)
	{
		auto [x, y] = split_one_token(input);
		input = std::move(y);
		ranges.push_back(x);
	}

	return { ranges.data(), ranges.data() + ranges.size() };
}



#define EXPORT extern "C" __declspec(dllexport)

// just a wrapper for get_tokens with C structs in signature
EXPORT RangeSpan GetTokensRangeImplementation(CharSpan input)
{
	return { get_tokens(input.x) };
}

EXPORT void ReleaseResources() noexcept
{
	ranges.clear();
}
