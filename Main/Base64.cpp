/*
 * Base64.cpp
 *
 *  Created on: 2016/2/24
 *      Author: D10307009
 */

//
// Base64 Encoding and Decoding
//
// @note Copyright &copy; 2013 by Smartware Computing, all rights reserved.
//     Individuals may use this application for evaluation or non-commercial
//     purposes. Within this restriction, changes may be made to this application
//     as long as this copyright notice is retained. The user shall make
//     clear that their work is a derived work, and not the original.
//     Users of this application and sources accept this application "as is" and
//     shall hold harmless Smartware Computing, for any undesired results while
//     using this application - whether real or imagined.
//
// author David Smart, Smartware Computing
//
#ifndef WIN32
#include "mbed.h"
#else
#include "windows.h"
typedef unsigned int uint32_t;
#endif
#include "Base64.h"
#include "FATFileSystem.h"

static const char encoding_table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
		'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
		'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

static const int mod_table[] = { 0, 2, 1 };

Base64::Base64() {
	decoding_table = NULL;
}

Base64::~Base64() {
	if (decoding_table)
		free(decoding_table);
}

char * Base64::Encode(const char *data, size_t input_length,
		size_t *output_length) {
	*output_length = 4 * ((input_length + 2) / 3);

	char *encoded_data = (char *) malloc(*output_length + 1); // often used for text, so add room for NULL
	if (encoded_data == NULL)
		return NULL;

	for (unsigned int i = 0, j = 0; i < input_length;) {

		uint32_t octet_a = i < input_length ? data[i++] : 0;
		uint32_t octet_b = i < input_length ? data[i++] : 0;
		uint32_t octet_c = i < input_length ? data[i++] : 0;

		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (int i = 0; i < mod_table[input_length % 3]; i++)
		encoded_data[*output_length - 1 - i] = '=';

	encoded_data[*output_length] = '\0';    // as a courtesy to text users
	return encoded_data;
}

int Base64::Encode(FILE *base, FILE *source) {

	fseek(source, 0, SEEK_END);
	size_t input_length = ftell(source);
	fseek(source, 0, SEEK_SET);
	size_t output_length = 4 * ((input_length + 2) / 3);

	char buff_in[3];
	char buff_out[4];
	memset(buff_in, 0, 3);

	size_t ret;
	while ((ret = fread(buff_in, 1, 3, source)) > 0) {

		uint32_t triple = (buff_in[0] << 0x10) + (buff_in[1] << 0x08)
				+ buff_in[2];

		buff_out[0] = encoding_table[(triple >> 3 * 6) & 0x3F];
		buff_out[1] = encoding_table[(triple >> 2 * 6) & 0x3F];
		buff_out[2] = encoding_table[(triple >> 1 * 6) & 0x3F];
		buff_out[3] = encoding_table[(triple >> 0 * 6) & 0x3F];

		fwrite(buff_out, 1, 4, base);
		memset(buff_in, 0, 3);
	}

	memcpy(buff_out, "===", 3);
	long int mt = mod_table[input_length % 3];
	fseek(base, mt*-1, SEEK_CUR);
	fwrite(buff_out, 1, mt, base);

//	encoded_data[*output_length] = '\0';    // as a courtesy to text users

	return 0;
}

char * Base64::Decode(const char *data, size_t input_length,
		size_t *output_length) {
	if (decoding_table == NULL)
		build_decoding_table();

	if (input_length % 4 != 0)
		return NULL;

	*output_length = input_length / 4 * 3;
	if (data[input_length - 1] == '=')
		(*output_length)--;
	if (data[input_length - 2] == '=')
		(*output_length)--;

	char *decoded_data = (char *) malloc(*output_length + 1); // often used for text, so add room for NULL
	if (decoded_data == NULL)
		return NULL;

	for (unsigned int i = 0, j = 0; i < input_length;) {

		uint32_t sextet_a =
				data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_b =
				data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_c =
				data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_d =
				data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

		uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6)
				+ (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

		if (j < *output_length)
			decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < *output_length)
			decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < *output_length)
			decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}
	decoded_data[*output_length] = '\0';    // as a courtesy to text users
	return decoded_data;
}

void Base64::build_decoding_table() {
	decoding_table = (char *) malloc(256);

	for (int i = 0; i < 64; i++)
		decoding_table[(unsigned char) encoding_table[i]] = i;
}

