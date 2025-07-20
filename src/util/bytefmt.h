#ifndef BYTEFMT_H
#define BYTEFMT_H

#include<string>
#include<regex>
#include<iomanip>
#include<sstream>

namespace blooDot {
	// implementation inspired by original source from https://github.com/eudoxos/bytesize
	class bytesize {
		// number of bytes
		size_t bytes;

	public:
		// construct from number
		explicit bytesize(const size_t bytes_): bytes(bytes_) {}

		// parse from string
		static bytesize parse(const std::string &str) {
			const static std::regex rx(R"(\s*(\d+|\d+[.]|\d?[.]\d+)\s*((|ki|Mi|Gi|Ti|Pi|k|M|G|T|P)[Bb]?)\s*)");
			std::smatch m;

			if (!std::regex_match(str, m, rx)) {
				throw std::runtime_error("Unable to parse '" + str + "' as size.");
			}

			const unsigned long long d = std::stoull(m[1].str());
			size_t mult;

			if (m[3] == "") {
				mult = 1;
			} else if (m[3] == "ki") {
				mult = 1LL << 10;
			} else if (m[3] == "Mi") {
				mult = 1LL << 20;
			} else if (m[3] == "Gi") {
				mult = 1LL << 30;
			} else if (m[3] == "Ti") {
				mult = 1LL << 40;
			} else if (m[3] == "Pi") {
				mult = 1LL << 50;
			} else if (m[3] == "k") {
				mult = 1'000LL;
			} else if (m[3] == "M") {
				mult = 1'000'000LL;
			} else if (m[3] == "G") {
				mult = 1'000'000'000LL;
			} else if (m[3] == "T") {
				mult = 1'000'000'000'000LL;
			} else if (m[3] == "P") {
				mult = 1'000'000'000'000'000LL;
			} else {
				throw std::logic_error("Unhandled prefix \"" + m[2].str() + "\".");
			}

			return bytesize(d * mult);
		}

		// represent as string
		[[nodiscard]] std::string format() const {
			std::ostringstream oss;

			oss << std::setprecision(3);
			if (bytes < 1'000LL) {
				oss << bytes << " B";
			} else if (bytes < 1'000'000LL) {
				oss << (static_cast<double>(bytes) * 1. / 1000LL) << " kB";
			} else if (bytes < 1'000'000'000LL) {
				oss << (static_cast<double>(bytes) * 1. / 1000'000LL) << " MB";
			} else if (bytes < 1'000'000'000'000LL) {
				oss << (static_cast<double>(bytes) * 1. / 1000'000'000LL) << " GB";
			} else if (bytes < 1'000'000'000'000'000LL) {
				oss << (static_cast<double>(bytes) * 1. / 1000'000'000'000LL) << " TB";
			} else {
				oss << (static_cast<double>(bytes) * 1. / 1000'000'000'000'000LL) << " PB";
			}

			return oss.str();
		}

		// conversion to size_t
		explicit operator size_t() const { return bytes; }

		// conversion to std::string
		explicit operator std::string() const { return this->format(); }
	};

	// stream concatenation operator
	inline std::ostream &operator<<(std::ostream &os, const bytesize &bs) {
		os << bs.format();

		return os;
	}

	// separate namespace for literals
	namespace literals {
		// bytes only with integer
		inline bytesize operator"" _B(const unsigned long long int num) { return bytesize(num); }

		// floating-point numbers, like 5.5_kB
		inline bytesize operator"" _kiB(const long double num) { return bytesize(static_cast<size_t>((1LL << 10) * num)); }
		inline bytesize operator"" _MiB(const long double num) { return bytesize(static_cast<size_t>((1LL << 20) * num)); }
		inline bytesize operator"" _GiB(const long double num) { return bytesize(static_cast<size_t>((1LL << 30) * num)); }
		inline bytesize operator"" _TiB(const long double num) { return bytesize(static_cast<size_t>((1LL << 40) * num)); }
		inline bytesize operator"" _PiB(const long double num) { return bytesize(static_cast<size_t>((1LL << 50) * num)); }
		inline bytesize operator"" _kB(const long double num) { return bytesize(static_cast<size_t>(1'000LL * num)); }
		inline bytesize operator"" _MB(const long double num) { return bytesize(static_cast<size_t>(1'000'000LL * num)); }
		inline bytesize operator"" _GB(const long double num) { return bytesize(static_cast<size_t>(1'000'000'000LL * num)); }
		inline bytesize operator"" _TB(const long double num) { return bytesize(static_cast<size_t>(1'000'000'000'000LL * num)); }
		inline bytesize operator"" _PB(const long double num) { return bytesize(static_cast<size_t>(1'000'000'000'000'000LL * num)); }

		// repeated for integer literals so that e.g. 5_kB works
		inline bytesize operator"" _kiB(const unsigned long long int num) { return bytesize((1LL << 10) * num); }
		inline bytesize operator"" _MiB(const unsigned long long int num) { return bytesize((1LL << 20) * num); }
		inline bytesize operator"" _GiB(const unsigned long long int num) { return bytesize((1LL << 30) * num); }
		inline bytesize operator"" _TiB(const unsigned long long int num) { return bytesize((1LL << 40) * num); }
		inline bytesize operator"" _PiB(const unsigned long long int num) { return bytesize((1LL << 50) * num); }
		inline bytesize operator"" _kB(const unsigned long long int num) { return bytesize(1'000LL * num); }
		inline bytesize operator"" _MB(const unsigned long long int num) { return bytesize(1'000'000LL * num); }

		inline bytesize operator"" _GB(const unsigned long long int num) {
			return bytesize(1'000'000'000LL * num);
		}

		inline bytesize operator"" _TB(const unsigned long long int num) {
			return bytesize(1'000'000'000'000LL * num);
		}

		inline bytesize operator"" _PB(const unsigned long long int num) {
			return bytesize(1'000'000'000'000'000LL * num);
		}
	}
}

#define BYTESIZE_FMTLIB_FORMATTER \
	/* make bytesize::bytesize known to fmt::format */ \
	template<> struct fmt::formatter<bytesize::bytesize> { \
	  template<typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); } \
	  template<typename FormatContext> auto format(const bytesize::bytesize &bs, FormatContext &ctx) { return format_to(ctx.out(), "{}", bs.format()); } \
	};
#endif //BYTEFMT_H
