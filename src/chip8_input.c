#include <GLFW/glfw3.h>

#include "chip8.h"

/*	 Chip-8 Keypad			Keyboard
 *	   +-+-+-+-+			+-+-+-+-+
 *	   |1|2|3|C|			|1|2|3|4|
 *	   +-+-+-+-+			+-+-+-+-+
 *	   |4|5|6|D|			|Q|W|E|R|
 *	   +-+-+-+-+	 =>		+-+-+-+-+
 *	   |7|8|9|E|			|A|S|D|F|
 *	   +-+-+-+-+			+-+-+-+-+
 *	   |A|0|B|F|			|Z|X|C|V|
 *	   +-+-+-+-+			+-+-+-+-+
 */

void chip8_process_input (chip8_vm* chip8, GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_GET_KEY) == GLFW_PRESS) {}
	if (glfwGetKey(window, GLFW_GET_KEY) == GLFW_PRESS) {}
	if (glfwGetKey(window, GLFW_GET_KEY) == GLFW_PRESS) {}
	if (glfwGetKey(window, GLFW_GET_KEY) == GLFW_PRESS) {}
	if (glfwGetKey(window, GLFW_GET_KEY) == GLFW_PRESS) {}
	if (glfwGetKey(window, GLFW_GET_KEY) == GLFW_PRESS) {}
	if (glfwGetKey(window, GLFW_GET_KEY) == GLFW_PRESS) {}
	if (glfwGetKey(window, GLFW_GET_KEY) == GLFW_PRESS) {}
	if (glfwGetKey(window, GLFW_GET_KEY) == GLFW_PRESS) {}
}
