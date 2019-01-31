#ifndef CHIP8_DBG_H
#define CHIP8_DBG_H

#include <stdio.h>
#include <stdbool.h>

#define NDEBUG

#ifdef NDEBUG
	#define CHIP8_DBG_ON 0
#else
	#define CHIP8_DBG_ON 1
#endif

/*
 * @brief Serves as a precursor function-like macro to the debug macro.
 */
#define CHIP8_DBG_PRE(MSG, ...)									    	\
do {															    	\
	if (CHIP8_DBG_ON) {											    	\
		fprintf(stderr, "great_chip-8::DEBUG::%s::%d::%s: " MSG "\n",	\
		__FILE__, __LINE__+0UL, __func__, __VA_ARGS__);                 \
	}															    	\
} while (false)

/*
 * @brief Extracts the first argument from the list of arguments.
 */
#define CHIP8_DBG_FIRST0(_0, ...) _0
#define CHIP8_DBG_FIRST(...) CHIP8_DBG_FIRST0(__VA_ARGS__, 0)

/*
 * @brief Removes the first argument from the list of arguments.
 */
#define CHIP8_DBG_LAST0(_0, ...) __VA_ARGS__
#define CHIP8_DBG_LAST(...) CHIP8_DBG_LAST0(__VA_ARGS__, 0)

/*
 * @brief Prints debug message whilst containing contextual program info.
 */
#define CHIP8_DBG(...)							    \
CHIP8_DBG_PRE(CHIP8_DBG_FIRST(__VA_ARGS__) "%.0d",  \
		CHIP8_DBG_LAST(__VA_ARGS__))

/*
 * @brief Serves as a precursor function-like macro to the instruction macro.
 */
#define CHIP8_ISTR_LOG_PRE(MSG, ...)									\
do {																    \
	if (CHIP8_DBG_ON) {												    \
		printf("great_chip-8::ISTR: " MSG "\n", __VA_ARGS__);	        \
	}																    \
} while (false)

/*
 * @brief Extracts the first argument from the list of arguments.
 */
#define CHIP8_ISTR_LOG_FIRST0(_0, ...) _0
#define CHIP8_ISTR_LOG_FIRST(...) CHIP8_ISTR_LOG_FIRST0(__VA_ARGS__, 0)

/*
 * @brief Removes the first argument from the list of arguments.
 */
#define CHIP8_ISTR_LOG_LAST0(_0, ...) __VA_ARGS__
#define CHIP8_ISTR_LOG_LAST(...) CHIP8_ISTR_LOG_LAST0(__VA_ARGS__, 0)

/*
 * @brief Prints instruction operation for debugging.
 */
#define CHIP8_ISTR_LOG(...)								        \
	CHIP8_ISTR_LOG_PRE(CHIP8_ISTR_LOG_FIRST(__VA_ARGS__) "%.0d",\
			CHIP8_ISTR_LOG_LAST(__VA_ARGS__))

/*
 * @brief Dumps loaded memory contents to standard output.
 */
#define CHIP8_MEM_DUMP(CHIP8_MEM)                                           \
do {                                                                        \
	if (CHIP8_DBG_ON) {                                                     \
		if ((chip8_byte*){ 0 } = CHIP8_MEM) {                               \
			for (chip8_word I_MEM = 0; I_MEM < CHIP8_MEM_SIZE; I_MEM++) {   \
				printf("%04u: chip8->mem[0x%04X] = 0x%04X\n", I_MEM, I_MEM, \
						CHIP8_MEM[I_MEM]);                                  \
			}                                                               \
		}                                                                   \
	}                                                                       \
} while (false)

#endif /* CHIP8_DBG_H */
