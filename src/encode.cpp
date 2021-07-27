#include <zlib.h>
#include <stdlib.h>

#include "encode.hpp"

namespace yah {
  char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                        'w', 'x', 'y', 'z', '0', '1', '2', '3',
                        '4', '5', '6', '7', '8', '9', '+', '/'};
  int mod_table[] = {0, 2, 1};

  char *
  base64_encode(const unsigned char *data, size_t input_length) {
    int output_length;
    output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = (char*)malloc(output_length + 1);
    memset(encoded_data, 0, sizeof(char) * output_length + 1);
    if (encoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[output_length - 1 - i] = '=';

    return encoded_data;
}

  Encode::Encode(const std::string& text) {
    this->text = text;
  }

  std::string Encode::encode() const {
    // 首先使用gzip
    size_t bufSize = 2 * this->text.size();

    auto str = this->text.c_str();
    unsigned char dest[bufSize];
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = this->text.size();
    strm.avail_out = bufSize;
    strm.next_in = (Bytef*) str;
    strm.next_out = (Bytef*) dest;
    deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    deflate(&strm, Z_FINISH);
    bufSize = bufSize - strm.avail_out;
    deflateEnd(&strm);

    char* final = base64_encode(dest, bufSize);
    std::string ret(final);
    free(final);

    return ret;
  }
}
