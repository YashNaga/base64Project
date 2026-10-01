#include <stdint.h>
#include <stdio.h>
#include <string.h>

char *encoding(const char *input, int inputLength, char *output, int *outputIndex);
char *decoding(const char *input, int inputLength, char *output, int *outputIndex);
int decoderLookup(const char inputChar);

const char kBase64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

typedef uint32_t u32;

int main() {
	char opts[100];
	char inputBuffer[1000];
	printf("BASE64 Encoder/Decoder\n\nWhat would you like to do:\n> Encode\n> Decode\n\n> ");
	if(fgets(opts, sizeof(opts), stdin) != NULL) {
		opts[strcspn(opts, "\n")] = '\0';

		if(strcmp(opts, "encode") == 0 || strcmp(opts, "Encode") == 0 || strcmp(opts, "ENCODE") == 0) {
			printf("Enter a string to be encoded: ");
			if(fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
				inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
			}
			int expectedOutputSize = ((strlen(inputBuffer) + 2) / 3) * 4 +
									 1;	 // Rounds up to the nearest multiple of 3, * 4 to account for 3 bytes turning
										 // into 4 base64 char including padding, and + 1 for null terminator
			char outputBuffer[expectedOutputSize];

			// Maybe refactor stream code to a func
			// Refactor stream code for decoding [4]
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
		}

		if(strcmp(opts, "decode") == 0 || strcmp(opts, "Decode") == 0 || strcmp(opts, "DECODE") == 0) {
			// printf("Enter a string to be decoded: ");
			// if(fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
			// 	inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
			// }

			// int expectedOutputSize = ((strlen(inputBuffer) + 2) / 3) * 4 + 1;	 // Rounds up to the nearest
			// multiple of 3, * 4 to account for 3 bytes turning into 4 base64 char including padding, and + 1 for null
			// terminator
			char outputBuffer[1000];

			char stream[4];
			const char *streamCursor = inputBuffer;
			int remainingBytes = strlen(inputBuffer);
			int outIndex = 0;

			while(remainingBytes > 0) {
				int bytesToProcess = (remainingBytes >= 4) ? 4 : remainingBytes;
				decoding(stream, bytesToProcess, outputBuffer, &outIndex);
			}
		}
	}

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

int decoderLookup(const char input) {
	int i;
	for(i = 0; i < strlen(kBase64Chars); i++) {
		if(kBase64Chars[i] == input) {
			printf("%c is in index: %d", input, i);
			return i;
		}
	}
	if(input == '=') {
		return 0;
	}
	return -1;
}

char *decoding(const char *input, int inputLength, char *output, int *outputIndex) {
	// Take a char in BASE64 lets say 'C' and find its position in kBase64Chars which would be 2,
	// Then we take the binary value of 2 and we | pack it into a u32 & mask it with 255
	if(inputLength == 4) {
		printf("Index: %d", decoderLookup(input[0]));
		u32 packed = (decoderLookup(input[0]) << 16) | (decoderLookup(input[0]) << 8) | (decoderLookup(input[0]) << 16);
	}

	return output;
}
// decoding user input
// How do effeciently look up a characters index backwards
// Think about how your gonna build the bits in reverse
