//
//  ClockReceiver.hpp
//  Clock Signal
//
//  Created by Thomas Harte on 22/07/2017.
//  Copyright 2017 Thomas Harte. All rights reserved.
//

#ifndef ClockReceiver_hpp
#define ClockReceiver_hpp

/*
	Informal pattern for all classes that run from a clock cycle:

		Each will implement either or both of run_for(Cycles) and run_for(HalfCycles), as
		is appropriate.

		Callers that are accumulating HalfCycles but want to talk to receivers that implement
		only run_for(Cycles) can use HalfCycle.flush_cycles if they have appropriate storage, or
		can wrap the receiver in HalfClockReceiver in order automatically to bind half-cycle
		storage to it.

	Alignment rule:

		run_for(Cycles) may be called only after an even number of half cycles. E.g. the following
		sequence will have undefined results:

			run_for(HalfCycles(1))
			run_for(Cycles(1))

		An easy way to ensure this as a caller is to pick only one of run_for(Cycles) and
		run_for(HalfCycles) to use.

	Reasoning:

		Users of this template may with to implement run_for(Cycles) and run_for(HalfCycles)
		where there is a need to implement at half-cycle precision but a faster execution
		path can be offered for full-cycle precision. Those users are permitted to assume
		phase in run_for(Cycles) and should do so to be compatible with callers that use
		only run_for(Cycles).

	Corollary:

		Starting from nothing, the first run_for(HalfCycles(1)) will do the **first** half
		of a full cycle. The second will do the second half. Etc.

*/

/*!
	Provides a class that wraps a plain int, providing most of the basic arithmetic and
	Boolean operators, but forcing callers and receivers to be explicit as to usage.
*/
template <class T> class WrappedInt {
	public:
		inline WrappedInt(int l) : length_(l) {}
		inline WrappedInt() : length_(0) {}

		inline T &operator =(const T &rhs) {
			length_ = rhs.length_;
			return *this;
		}

		inline T &operator +=(const T &rhs) {
			length_ += rhs.length_;
			return *static_cast<T *>(this);
		}

		inline T &operator -=(const T &rhs) {
			length_ -= rhs.length_;
			return *static_cast<T *>(this);
		}

		inline T &operator ++() {
			++ length_;
			return *static_cast<T *>(this);
		}

		inline T &operator ++(int) {
			length_ ++;
			return *static_cast<T *>(this);
		}

		inline T &operator --() {
			-- length_;
			return *static_cast<T *>(this);
		}

		inline T &operator --(int) {
			length_ --;
			return *static_cast<T *>(this);
		}

		inline T &operator %=(const T &rhs) {
			length_ %= rhs.length_;
			return *static_cast<T *>(this);
		}

		inline T &operator &=(const T &rhs) {
			length_ &= rhs.length_;
			return *static_cast<T *>(this);
		}

		inline T operator +(const T &rhs) const			{	return T(length_ + rhs.length_);	}
		inline T operator -(const T &rhs) const			{	return T(length_ - rhs.length_);	}

		inline T operator %(const T &rhs) const			{	return T(length_ % rhs.length_);	}
		inline T operator &(const T &rhs) const			{	return T(length_ & rhs.length_);	}

		inline T operator -() const						{	return T(- length_);				}

		inline bool operator <(const T &rhs) const		{	return length_ < rhs.length_;		}
		inline bool operator >(const T &rhs) const		{	return length_ > rhs.length_;		}
		inline bool operator <=(const T &rhs) const		{	return length_ <= rhs.length_;		}
		inline bool operator >=(const T &rhs) const		{	return length_ >= rhs.length_;		}
		inline bool operator ==(const T &rhs) const		{	return length_ == rhs.length_;		}
		inline bool operator !=(const T &rhs) const		{	return length_ != rhs.length_;		}

		inline bool operator !() const					{	return !length_;					}
		// bool operator () is not supported because it offers an implicit cast to int, which is prone silently to permit misuse

		inline int as_int() const { return length_; }

		/*!
			Severs from @c this the effect of dividing by @c divisor; @c this will end up with
			the value of @c this modulo @c divisor and @c divided by @c divisor is returned.
		*/
		inline T divide(const T &divisor) {
			T result(length_ / divisor.length_);
			length_ %= divisor.length_;
			return result;
		}

		/*!
			Flushes the value in @c this. The current value is returned, and the internal value
			is reset to zero.
		*/
		inline T flush() {
			T result(length_);
			length_ = 0;
			return result;
		}

		// operator int() is deliberately not provided, to avoid accidental subtitution of
		// classes that use this template.

	protected:
		int length_;
};

/// Describes an integer number of whole cycles: pairs of clock signal transitions.
class Cycles: public WrappedInt<Cycles> {
	public:
		inline Cycles(int l) : WrappedInt<Cycles>(l) {}
		inline Cycles() : WrappedInt<Cycles>() {}
		inline Cycles(const Cycles &cycles) : WrappedInt<Cycles>(cycles.length_) {}
};

/// Describes an integer number of half cycles: single clock signal transitions.
class HalfCycles: public WrappedInt<HalfCycles> {
	public:
		inline HalfCycles(int l) : WrappedInt<HalfCycles>(l) {}
		inline HalfCycles() : WrappedInt<HalfCycles>() {}

		inline HalfCycles(const Cycles cycles) : WrappedInt<HalfCycles>(cycles.as_int() * 2) {}
		inline HalfCycles(const HalfCycles &half_cycles) : WrappedInt<HalfCycles>(half_cycles.length_) {}

		/// @returns The number of whole cycles completely covered by this span of half cycles.
		inline Cycles cycles() {
			return Cycles(length_ >> 1);
		}

		/// Flushes the whole cycles in @c this, subtracting that many from the total stored here.
		inline Cycles flush_cycles() {
			Cycles result(length_ >> 1);
			length_ &= 1;
			return result;
		}

		/// Flushes the half cycles in @c this, returning the number stored and setting this total to zero.
		inline HalfCycles flush() {
			HalfCycles result(length_);
			length_ = 0;
			return result;
		}

		/*!
			Severs from @c this the effect of dividing by @c divisor; @c this will end up with
			the value of @c this modulo @c divisor and @c divided by @c divisor is returned.
		*/
		inline Cycles divide_cycles(const Cycles &divisor) {
			HalfCycles half_divisor = HalfCycles(divisor);
			Cycles result(length_ / half_divisor.length_);
			length_ %= half_divisor.length_;
			return result;
		}
};

/*!
	If a component implements only run_for(Cycles), an owner can wrap it in HalfClockReceiver
	automatically to gain run_for(HalfCycles).
*/
template <class T> class HalfClockReceiver: public T {
	public:
		using T::T;

		using T::run_for;
		inline void run_for(const HalfCycles half_cycles) {
			half_cycles_ += half_cycles;
			T::run_for(half_cycles_.flush_cycles());
		}

	private:
		HalfCycles half_cycles_;
};

#endif /* ClockReceiver_hpp */
