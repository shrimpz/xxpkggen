#pragma once
namespace xxlib
{

class Stopwatch
{
public:
	Stopwatch()
		: beginTime( std::chrono::high_resolution_clock::now() )
	{
	}

	inline void Reset()
	{
		beginTime = std::chrono::high_resolution_clock::now();
	}

	inline int64 ElapsedMilliseconds() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - beginTime ).count();
	}

	inline int64 ElapsedMicroseconds() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>( std::chrono::high_resolution_clock::now() - beginTime ).count();
	}

	inline int64 ElapsedNanoseconds() const
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>( std::chrono::high_resolution_clock::now() - beginTime ).count();
	}

	inline int64 ElapsedSeconds() const
	{
		return std::chrono::duration_cast<std::chrono::seconds>( std::chrono::high_resolution_clock::now() - beginTime ).count();
	}

	inline int64 ElapsedMinutes() const
	{
		return std::chrono::duration_cast<std::chrono::minutes>( std::chrono::high_resolution_clock::now() - beginTime ).count();
	}

	inline int64 ElapsedHours() const
	{
		return std::chrono::duration_cast<std::chrono::hours>( std::chrono::high_resolution_clock::now() - beginTime ).count();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> beginTime;
};

}
