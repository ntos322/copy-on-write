#include "cow.hpp"
#include <psapi.h>

auto cow::add_module(std::uintptr_t image_base) -> bool {

	/* just make sure that the module base isnt already added to the list of protected modules */
	if (std::find(m_protected_modules.begin(), m_protected_modules.end(), image_base) != m_protected_modules.end())
		return false;

	m_protected_modules.push_back(image_base);

}

auto cow::get_protected_modules() -> std::vector<std::uintptr_t> {
	return m_protected_modules;
}

auto cow::get_module_section(std::string name, std::uintptr_t image_base) -> std::pair<std::uintptr_t, std::uintptr_t> {

    auto nt_headers = (IMAGE_NT_HEADERS*)((std::uintptr_t)((IMAGE_DOS_HEADER*)image_base)->e_lfanew + image_base);
    auto section = (IMAGE_SECTION_HEADER*)((std::uintptr_t)nt_headers + sizeof(IMAGE_NT_HEADERS));

    for (auto i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i, ++section) {

        if (std::strcmp((const char*)section->Name, name.c_str()) == 0) {

            auto section_range_start = image_base + section->VirtualAddress;
            auto section_range_end = section_range_start + section->Misc.VirtualSize;

            return { section_range_start, section_range_end };

        }
    }

    return { 0, 0 };

}

auto cow::is_page_private(std::uintptr_t section_range_start, std::uintptr_t section_range_end) -> cow_result {

    for (std::uintptr_t page = section_range_start; page < section_range_end; page += 0x1000) {

        PSAPI_WORKING_SET_EX_INFORMATION page_information = {};
        page_information.VirtualAddress = reinterpret_cast<LPVOID>(page);

        if (K32QueryWorkingSetEx(GetCurrentProcess(), &page_information, sizeof(page_information))) {

            /* check if the page is private */
            if (page_information.VirtualAttributes.Shared == 0) {

                return { true, page };

            }

        }
        else {
            return { false, 0 };
        }


    }

    return { false, 0 };

}