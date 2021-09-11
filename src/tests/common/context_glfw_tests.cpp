#include <doctest/doctest.h>
#include <context_glfw.h>

TEST_CASE("GLFW should initialize") {
	common::context_glfw c;

	SUBCASE("and obtain getProcAddressPtr") {
		CHECK(c.getProcAddressPtr() != nullptr);
	}

	SUBCASE("and create window") {
		c.create();

		SUBCASE("and poll events") {
			c.pollEvents();

			SUBCASE("and swap buffers") {
				c.swapBuffers();
			}

			SUBCASE("and check if window should close") {
				c.shouldClose();
			}
		}
	}
}
