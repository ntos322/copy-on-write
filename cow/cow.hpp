#pragma once
#include <Windows.h>
#include <iostream>

#include <vector>

/* used in is_page_private */
struct cow_result {
	bool patched;
	std::uintptr_t page;
};

class cow {
private:
	std::vector<std::uintptr_t>m_protected_modules;
public:

	auto add_module(std::uintptr_t image_base) -> bool;
	auto get_protected_modules() -> std::vector<std::uintptr_t>;

public:

	auto get_module_section(std::string name, std::uintptr_t image_base) -> std::pair<std::uintptr_t, std::uintptr_t>;
	auto is_section_private(std::uintptr_t section_range_start, std::uintptr_t section_range_end) -> cow_result;

};

inline cow g_cow;
