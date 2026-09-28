#include <stdint.h>
#include <stdio.h>
#include <string.h>

char *encoding(const char *input, int inputLength, char *output, int *outputIndex);

const char kBase64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

typedef uint32_t u32;

int main() {
	char inputBuffer[1000];

	printf("Enter a string: ");
	if(fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
		inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
	}

	int expectedOutputSize = ((strlen(inputBuffer) + 2) / 3) * 4 +
							 1;	 // Rounds up to the nearest multiple of 3, * 4 to account for 3 bytes turning into 4
								 // base64 char including padding, and + 1 for null terminator
	char outputBuffer[expectedOutputSize];

	char stream[3];
	const char *streamCursor = inputBuffer;
	int remainingBytes = strlen(inputBuffer);
	int outIndex = 0;

	while(remainingBytes > 0) {
		int bytesToProcess = (remainingBytes >= 3) ? 3 : remainingBytes;

		memcpy(stream, streamCursor, bytesToProcess);

		encoding(stream, bytesToProcess, outputBuffer, &outIndex);

		streamCursor += bytesToProcess;
		remainingBytes -= bytesToProcess;
	}

	outputBuffer[outIndex] = '\0';
	printf("%s", outputBuffer);

	return 0;
}

char *encoding(const char *input, int inputLength, char *output, int *outputIndex) {
	if(inputLength == 3) {
		u32 byte1 = input[0];
		u32 byte2 = input[1];
		u32 byte3 = input[2];

		u32 packed = (byte1 << 16) | (byte2 << 8) | byte3;	// 00000000 AAAAAAAA BBBBBBBB CCCCCCCC

		u32 chunk1 = (packed >> 18) & 63;  // 00000000 00000000 00000000 00AAAAAA
		u32 chunk2 = (packed >> 12) & 63;  // 00000000 00000000 00000000 00AABBBB
		u32 chunk3 = (packed >> 6) & 63;   // 00000000 00000000 00000000 00BBBBCC
		u32 chunk4 = packed & 63;		   // 00000000 00000000 00000000 00CCCCCC

		output[(*outputIndex)++] = kBase64Chars[chunk1];
		output[(*outputIndex)++] = kBase64Chars[chunk2];
		output[(*outputIndex)++] = kBase64Chars[chunk3];
		output[(*outputIndex)++] = kBase64Chars[chunk4];
	} else if(inputLength == 2) {
		u32 leftoverPacked = (input[0] << 16) | (input[1] << 8);

		u32 leftoverChunk1 = (leftoverPacked >> 18) & 63;
		u32 leftoverChunk2 = (leftoverPacked >> 12) & 63;
		u32 leftoverChunk3 = (leftoverPacked >> 6) & 63;

		output[(*outputIndex)++] = kBase64Chars[leftoverChunk1];
		output[(*outputIndex)++] = kBase64Chars[leftoverChunk2];
		output[(*outputIndex)++] = kBase64Chars[leftoverChunk3];
		output[(*outputIndex)++] = '=';

	} else if(inputLength == 1) {
		u32 leftoverPacked2 = (input[0] << 16);

		u32 leftoverChunk4 = (leftoverPacked2 >> 18) & 63;
		u32 leftoverChunk5 = (leftoverPacked2 >> 12) & 63;

		output[(*outputIndex)++] = kBase64Chars[leftoverChunk4];
		output[(*outputIndex)++] = kBase64Chars[leftoverChunk5];
		output[(*outputIndex)++] = '=';
		output[(*outputIndex)++] = '=';
	}
	return output;
}

// decoding user input
// How do effeciently look up a characters index backwards
// Think about how your gonna build the bits in reverse
