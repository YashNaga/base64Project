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
			printf("%s\n", outputBuffer);
		}

		if(strcmp(opts, "decode") == 0 || strcmp(opts, "Decode") == 0 || strcmp(opts, "DECODE") == 0) {
			printf("Enter a string to be decoded: ");
			if(fgets(inputBuffer, sizeof(inputBuffer), stdin) != NULL) {
				inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
			}

			int expectedOutputSize = ((strlen(inputBuffer) + 2) / 3) * 4 + 1;  // Rounds up to the nearest
			// multiple of 3, * 4 to account for 3 bytes turning into 4 base64 char including padding, and + 1 for null
			// terminator
			// char testStr[32] = "aGVsbG8=";
			char outputBuffer[1000];

			char stream[4];
			const char *streamCursor = inputBuffer;
			int remainingBytes = strlen(inputBuffer);
			int outIndex = 0;

			while(remainingBytes > 0) {
				int bytesToProcess = (remainingBytes >= 4) ? 4 : remainingBytes;
				memcpy(stream, streamCursor, bytesToProcess);

				decoding(stream, bytesToProcess, outputBuffer, &outIndex);

				streamCursor += bytesToProcess;
				remainingBytes -= bytesToProcess;
			}
			outputBuffer[outIndex] = '\0';
			printf("%s\n", outputBuffer);
		}
	}

	return 0;
}

char *encoding(const char *input, int inputLength, char *output, int *outputIndex) {
	if(inputLength == 3) {
		u32 packed = (input[0] << 16) | (input[1] << 8) | input[2];	 // 00000000 AAAAAAAA BBBBBBBB CCCCCCCC

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
			return i;
		}
	}
	if(input == '=') {
		return 0;
	}
	return -1;
}

char *decoding(const char *input, int inputLength, char *output, int *outputIndex) {
	if(input[2] == '=' && input[3] == '=') {
		u32 packed = (decoderLookup(input[0]) << 18) | (decoderLookup(input[1]) << 12);
		u32 chunk1 = (packed >> 16) & 255;
		output[(*outputIndex)++] = chunk1;
	} else if(input[3] == '=') {
		u32 packed = (decoderLookup(input[0]) << 18) | (decoderLookup(input[1]) << 12) | (decoderLookup(input[2]) << 6);
		u32 chunk1 = (packed >> 16) & 255;
		u32 chunk2 = (packed >> 8) & 255;
		output[(*outputIndex)++] = chunk1;
		output[(*outputIndex)++] = chunk2;
	} else if(inputLength == 4) {
		u32 packed = (decoderLookup(input[0]) << 18) | (decoderLookup(input[1]) << 12) |
					 (decoderLookup(input[2]) << 6) | decoderLookup(input[3]);

		u32 chunk1 = (packed >> 16) & 255;
		u32 chunk2 = (packed >> 8) & 255;
		u32 chunk3 = packed & 255;

		output[(*outputIndex)++] = chunk1;
		output[(*outputIndex)++] = chunk2;
		output[(*outputIndex)++] = chunk3;
	}

	return output;
}
