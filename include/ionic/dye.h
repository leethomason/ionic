#pragma once

namespace ionic {
namespace internal {

struct dye
{
	static constexpr char reset[] = "\033[0m";
	static constexpr char black[] = "\x1B[30m";
	static constexpr char dark_red[] = "\x1B[31m";
	static constexpr char dark_green[] = "\x1B[32m";
	static constexpr char dark_yellow[] = "\x1B[33m";
	static constexpr char dark_blue[] = "\x1B[34m";
	static constexpr char dark_purple[] = "\x1B[35m";
	static constexpr char dark_cyan[] = "\x1B[36m";
	static constexpr char light_gray[] = "\x1B[37m";
	static constexpr char dark_gray[] = "\x1B[90m";
	static constexpr char red[] = "\x1B[91m";
	static constexpr char green[] = "\x1B[92m";
	static constexpr char yellow[] = "\x1B[93m";
	static constexpr char blue[] = "\x1B[94m";
	static constexpr char purple[] = "\x1B[95m";
	static constexpr char cyan[] = "\x1B[96m";
	static constexpr char white[] = "\x1B[97m";
};
} // namespace internal
} // namespace ionic