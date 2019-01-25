#ifndef CHIP8_DBG_H
#define CHIP8_DBG_H

#include <stdio.h>

#ifdef NDEBUG
	#define CHIP8_DBG_ON 0
#else
	#define CHIP8_DBG_ON 1
#endif

/*
 * @brief Serves as a precursor function-like macro to the actual debug macro.
 */
#define CHIP8_DBG_PRE(MSG, ...)										\
do {																\
	if (CHIP8_DBG_ON) {												\
		fprintf(stderr, "great_chip-8::DEBUG:%s:%d:%s: " MSG "\n",	\
				__FILE__, __LINE__+0UL, __func__, __VA_ARGS__);		\
	}																\
} while (false)

/*
 * @brief Extracts the first argument from the list of arguments.
 */
#define CHIP8_DBG_FIRST0(_0, ...) _0
#define CHIP8_DBG_FIRST(...) CHIP8_DBG_FIRST0(__VA_ARGS__, 0)

/*
 * @brief Remove the first argument from the list of arguments.
 */
#define CHIP8_DBG_LAST0(_0, ...) __VA_ARGS__
#define CHIP8_DBG_LAST(...) CHIP8_DBG_LAST0(__VA_ARGS__, 0)

/*
 * @brief Prints debug message whilst containing contextual program info.
 */
#if CHIP8_DBG_ON == 0
	#define CHIP8_DBG(...) do {} while (false)
#else
	#define CHIP8_DBG(...)								\
	CHIP8_DBG_PRE(CHIP8_DBG_FIRST(__VA_ARGS__) "%.0d",	\
			CHIP8_DBG_LAST(__VA_ARGS__))
#endif
#endif
