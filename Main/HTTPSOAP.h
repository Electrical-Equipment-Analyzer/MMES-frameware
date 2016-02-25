/* HTTPText.h */
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


#ifndef HTTPSOAP_H_
#define HTTPSOAP_H_

#include "IHTTPData.h"
#include "SDFileSystem.h"

/** A data endpoint to store text
*/
class HTTPSOAP : public IHTTPDataIn, public IHTTPDataOut
{
public:
  /** Create an HTTPText instance for output
   * @param str String to be transmitted
   */
	HTTPSOAP(char* id, FILE* data);

  /** Create an HTTPText instance for input
   * @param str Buffer to store the incoming string
   * @param size Size of the buffer
   */
	HTTPSOAP(char* str, size_t size);

protected:
  //IHTTPDataIn
  virtual void readReset();
  
  virtual int read(char* buf, size_t len, size_t* pReadLen);

  virtual int getDataType(char* type, size_t maxTypeLen); //Internet media type for Content-Type header

  virtual bool getIsChunked(); //For Transfer-Encoding header

  virtual size_t getDataLen(); //For Content-Length header

  //IHTTPDataOut
  virtual void writeReset();
  
  virtual int write(const char* buf, size_t len);

  virtual void setDataType(const char* type); //Internet media type from Content-Type header

  virtual void setIsChunked(bool chunked); //From Transfer-Encoding header

  virtual void setDataLen(size_t len); //From Content-Length header, or if the transfer is chunked, next chunk length

private:

  void readFrom(const char* txt, char* buf, size_t len, size_t* pReadLen);

  char* m_id;
  FILE* m_data;
  char m_buffer[10];
  size_t m_size;
  size_t m_state;
  size_t m_pos;
  size_t m_args;

};

#endif /* HTTPTEXT_H_ */
