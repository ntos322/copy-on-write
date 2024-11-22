#include "cow/cow.hpp"
#include <thread>

int main()
{

    auto image_base = (std::uintptr_t)GetModuleHandleA(0);

    /* insert the image base to the protection list */
    g_cow.add_module(image_base);

    std::thread t([&]() {

        while (true) {

            std::printf("cow: checking protected images...\n");

            for (auto image : g_cow.get_protected_modules()) {

                /* get the text section of the image */
                auto [section_start, section_end] = g_cow.get_module_section(".text", image);

                auto result = g_cow.is_page_private(section_start, section_end);
                if (result.patched)
                    std::printf("page at 0x%lX in 0x%lX is private, which means the .text section has been patched/written to\n", result.page, image);
                
            }

            std::this_thread::sleep_for(std::chrono::seconds(5));

        }

    });


    t.join();

}

