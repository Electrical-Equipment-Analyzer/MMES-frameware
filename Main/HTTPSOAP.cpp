/* HTTPText.cpp */
/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "HTTPSOAP.h"

#include <cstring>
#include <stdio.h>

#define OK 0

using std::memcpy;
using std::strncpy;
using std::strlen;

#define MIN(x,y) (((x)<(y))?(x):(y))

HTTPSOAP::HTTPSOAP(char* id, FILE* data) :
		m_id(id), m_data(data), m_state(0), m_pos(0), m_args(0) {
	m_size = strlen(id) + 1;
}

HTTPSOAP::HTTPSOAP(char* str, size_t size) :
		m_id(str), m_size(size), m_pos(0) {

}

//IHTTPDataIn
/*virtual*/void HTTPSOAP::readReset() {
	m_state = 0;
	m_pos = 0;
	m_args = 0;
}

const char * TXT_XML_H = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
const char * TXT_SOAP_H =
		"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">";
const char * TXT_SOAP_T = "</soap:Envelope>";
const char * TXT_SOAP_BODY_H = "<soap:Body>";
const char * TXT_SOAP_BODY_T = "</soap:Body>";
const char * TXT_WM_FU_H =
		"<ns0:fileUpload xmlns:ns0=\"http://motorweb.cloud.ee305.ntust.edu.tw/\">";
const char * TXT_WM_FU_T = "</ns0:fileUpload>";
const char * TXT_DATA_H = "<arg%d>";
const char * TXT_DATA_T = "</arg%d>";

#define state_hxml_head 0
#define state_soap_head 1
#define state_body_head 2
#define state_wmfu_head 3
#define state_args_head 4
#define state_args_body 5
#define state_args_tail 6
#define state_wmfu_tail 7
#define state_body_tail 8
#define state_soap_tail 9

#define status if (m_pos > 0 && *pReadLen > 0) {break;}

/*virtual*/int HTTPSOAP::read(char* buf, size_t len, size_t* pReadLen) {
	while (true) {
		switch (m_state) {
		case state_hxml_head:
			readFrom(TXT_XML_H, buf, len, pReadLen);
			status
		case state_soap_head:
			readFrom(TXT_SOAP_H, buf, len, pReadLen);
			status
		case state_body_head:
			readFrom(TXT_SOAP_BODY_H, buf, len, pReadLen);
			status
		case state_wmfu_head:
			readFrom(TXT_WM_FU_H, buf, len, pReadLen);
			status
		case state_args_head:
			sprintf(m_buffer, TXT_DATA_H, m_args);
			readFrom(m_buffer, buf, len, pReadLen);
			status
		case state_args_body:
			switch (m_args) {
			case 0:
				readFrom(m_id, buf, len, pReadLen);
				break;
			case 1:
				fseek(m_data, 0, SEEK_END);
				m_size = ftell(m_data);
				fseek(m_data, 0, SEEK_SET);
				if (m_pos < m_size) {
					*pReadLen = MIN(len, m_size - m_pos);
					fread(buf, 1, *pReadLen, m_data);
					m_pos += *pReadLen;
				} else {
					m_state++;
					m_pos = 0;
					*pReadLen = 0;
				}
				break;
			}
			status
		case state_args_tail:
			sprintf(m_buffer, TXT_DATA_T, m_args);
			readFrom(m_buffer, buf, len, pReadLen);
			status
			if (++m_args < 2) {
				m_state = state_args_head;
				continue;
			}
		case state_wmfu_tail:
			readFrom(TXT_WM_FU_T, buf, len, pReadLen);
			status
		case state_body_tail:
			readFrom(TXT_SOAP_BODY_T, buf, len, pReadLen);
			status
		case state_soap_tail:
			readFrom(TXT_SOAP_T, buf, len, pReadLen);
			status
		}
		break;
	}
	return OK;
}

void HTTPSOAP::readFrom(const char* txt, char* buf, size_t len,
		size_t* pReadLen) {
	m_size = strlen(txt);
	if (m_pos < m_size) {
		*pReadLen = MIN(len, m_size - m_pos);
		memcpy(buf, txt + m_pos, *pReadLen);
		m_pos += *pReadLen;
	} else {
		m_state++;
		m_pos = 0;
		*pReadLen = 0;
	}
}

//void HTTPSOAP::textData(const char* txt, char* buf, size_t num, char* data) {
//	sprintf(buf, txt, num, data, num);
//}

/*virtual*/int HTTPSOAP::getDataType(char* type, size_t maxTypeLen) //Internet media type for Content-Type header
		{
	strncpy(type, "text/xml", maxTypeLen - 1);
	type[maxTypeLen - 1] = '\0';
	return OK;
}

/*virtual*/bool HTTPSOAP::getIsChunked() //For Transfer-Encoding header
{
	return false;
}

/*virtual*/size_t HTTPSOAP::getDataLen()
//For Content-Length header
{

	fseek(m_data, 0, SEEK_END);
	m_size = ftell(m_data);
	fseek(m_data, 0, SEEK_SET);
	return strlen(TXT_XML_H) + strlen(TXT_SOAP_H) + strlen(TXT_SOAP_T)
			+ strlen(TXT_SOAP_BODY_H) + strlen(TXT_SOAP_BODY_T)
			+ strlen(TXT_WM_FU_H) + strlen(TXT_WM_FU_T) + 13 + strlen(m_id) + 13
			+ m_size;
}

//IHTTPDataOut
/*virtual*/void HTTPSOAP::writeReset() {
	m_pos = 0;
}

/*virtual*/int HTTPSOAP::write(const char* buf, size_t len) {
	size_t writeLen = MIN(len, m_size - 1 - m_pos);
	memcpy(m_id + m_pos, buf, writeLen);
	m_pos += writeLen;
	m_id[m_pos] = '\0';
	return OK;
}

/*virtual*/void HTTPSOAP::setDataType(const char* type) //Internet media type from Content-Type header
		{

}

/*virtual*/void HTTPSOAP::setIsChunked(bool chunked) //From Transfer-Encoding header
		{

}

/*virtual*/void HTTPSOAP::setDataLen(size_t len) //From Content-Length header, or if the transfer is chunked, next chunk length
		{

}

