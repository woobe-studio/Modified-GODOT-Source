/**************************************************************************/
/*  compression.cpp                                                       */
/**************************************************************************/


#include "compression.h"

#include "core/io/zip_io.h"
#include "core/project_settings.h"

#include "thirdparty/misc/fastlz.h"

#include <zlib.h>

int Compression::compress(uint8_t *p_dst, const uint8_t *p_src, int p_src_size, Mode p_mode) {
	switch (p_mode) {
		case MODE_FASTLZ: {
			if (p_src_size < 16) {
				uint8_t src[16];
				memset(&src[p_src_size], 0, 16 - p_src_size);
				memcpy(src, p_src, p_src_size);
				return fastlz_compress(src, 16, p_dst);
			} else {
				return fastlz_compress(p_src, p_src_size, p_dst);
			}

		} break;
		case MODE_DEFLATE:
		case MODE_GZIP: {
			int window_bits = p_mode == MODE_DEFLATE ? 15 : 15 + 16;

			z_stream strm;
			strm.zalloc = zipio_alloc;
			strm.zfree = zipio_free;
			strm.opaque = Z_NULL;
			int level = p_mode == MODE_DEFLATE ? zlib_level : gzip_level;
			int err = deflateInit2(&strm, level, Z_DEFLATED, window_bits, 8, Z_DEFAULT_STRATEGY);
			if (err != Z_OK) {
				return -1;
			}

			strm.avail_in = p_src_size;
			int aout = deflateBound(&strm, p_src_size);
			strm.avail_out = aout;
			strm.next_in = (Bytef *)p_src;
			strm.next_out = p_dst;
			deflate(&strm, Z_FINISH);
			aout = aout - strm.avail_out;
			deflateEnd(&strm);
			return aout;

		} break;
	}

	ERR_FAIL_V(-1);
}

int Compression::get_max_compressed_buffer_size(int p_src_size, Mode p_mode) {
	switch (p_mode) {
		case MODE_FASTLZ: {
			int ss = p_src_size + p_src_size * 6 / 100;
			if (ss < 66) {
				ss = 66;
			}
			return ss;

		} break;
		case MODE_DEFLATE:
		case MODE_GZIP: {
			int window_bits = p_mode == MODE_DEFLATE ? 15 : 15 + 16;

			z_stream strm;
			strm.zalloc = zipio_alloc;
			strm.zfree = zipio_free;
			strm.opaque = Z_NULL;
			int err = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, window_bits, 8, Z_DEFAULT_STRATEGY);
			if (err != Z_OK) {
				return -1;
			}
			int aout = deflateBound(&strm, p_src_size);
			deflateEnd(&strm);
			return aout;
		} break;
	}

	ERR_FAIL_V(-1);
}

int Compression::decompress(uint8_t *p_dst, int p_dst_max_size, const uint8_t *p_src, int p_src_size, Mode p_mode) {
	switch (p_mode) {
		case MODE_FASTLZ: {
			int ret_size = 0;

			if (p_dst_max_size < 16) {
				uint8_t dst[16];
				fastlz_decompress(p_src, p_src_size, dst, 16);
				memcpy(p_dst, dst, p_dst_max_size);
				ret_size = p_dst_max_size;
			} else {
				ret_size = fastlz_decompress(p_src, p_src_size, p_dst, p_dst_max_size);
			}
			return ret_size;
		} break;
		case MODE_DEFLATE:
		case MODE_GZIP: {
			int window_bits = p_mode == MODE_DEFLATE ? 15 : 15 + 16;

			z_stream strm;
			strm.zalloc = zipio_alloc;
			strm.zfree = zipio_free;
			strm.opaque = Z_NULL;
			strm.avail_in = 0;
			strm.next_in = Z_NULL;
			int err = inflateInit2(&strm, window_bits);
			ERR_FAIL_COND_V(err != Z_OK, -1);

			strm.avail_in = p_src_size;
			strm.avail_out = p_dst_max_size;
			strm.next_in = (Bytef *)p_src;
			strm.next_out = p_dst;

			err = inflate(&strm, Z_FINISH);
			int total = strm.total_out;
			inflateEnd(&strm);
			ERR_FAIL_COND_V(err != Z_STREAM_END, -1);
			return total;
		} break;
	}

	ERR_FAIL_V(-1);
}

/**
	This will handle both Gzip and Deflat streams. It will automatically allocate the output buffer into the provided p_dst_vect Vector.
	This is required for compressed data who's final uncompressed size is unknown, as is the case for HTTP response bodies.
	This is much slower however than using Compression::decompress because it may result in multiple full copies of the output buffer.
*/
int Compression::decompress_dynamic(PoolVector<uint8_t> *p_dst, int p_max_dst_size, const uint8_t *p_src, int p_src_size, Mode p_mode) {
	int ret;
	uint8_t *dst = nullptr;
	int out_mark = 0;
	z_stream strm;

	ERR_FAIL_COND_V(p_src_size <= 0, Z_DATA_ERROR);

	// This function only supports GZip and Deflate
	int window_bits = p_mode == MODE_DEFLATE ? 15 : 15 + 16;
	ERR_FAIL_COND_V(p_mode != MODE_DEFLATE && p_mode != MODE_GZIP, Z_ERRNO);

	// Initialize the stream
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	int err = inflateInit2(&strm, window_bits);
	ERR_FAIL_COND_V(err != Z_OK, -1);

	// Setup the stream inputs
	strm.next_in = (Bytef *)p_src;
	strm.avail_in = p_src_size;

	// Ensure the destination buffer is empty
	p_dst->resize(0);

	// decompress until deflate stream ends or end of file
	do {
		// Add another chunk size to the output buffer
		// This forces a copy of the whole buffer
		p_dst->resize(p_dst->size() + gzip_chunk);
		// Get pointer to the actual output buffer
		dst = p_dst->write().ptr();

		// Set the stream to the new output stream
		// Since it was copied, we need to reset the stream to the new buffer
		strm.next_out = &(dst[out_mark]);
		strm.avail_out = gzip_chunk;

		// run inflate() on input until output buffer is full and needs to be resized
		// or input runs out
		do {
			ret = inflate(&strm, Z_SYNC_FLUSH);

			switch (ret) {
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;
					FALLTHROUGH;
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
				case Z_STREAM_ERROR:
				case Z_BUF_ERROR:
					if (strm.msg) {
						WARN_PRINT(strm.msg);
					}
					(void)inflateEnd(&strm);
					p_dst->resize(0);
					return ret;
			}
		} while (strm.avail_out > 0 && strm.avail_in > 0);

		out_mark += gzip_chunk;

		// Encorce max output size
		if (p_max_dst_size > -1 && strm.total_out > (uint64_t)p_max_dst_size) {
			(void)inflateEnd(&strm);
			p_dst->resize(0);
			return Z_BUF_ERROR;
		}
	} while (ret != Z_STREAM_END);

	// If all done successfully, resize the output if it's larger than the actual output
	if (ret == Z_STREAM_END && (unsigned long)p_dst->size() > strm.total_out) {
		p_dst->resize(strm.total_out);
	}

	// clean up and return
	(void)inflateEnd(&strm);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int Compression::zlib_level = Z_DEFAULT_COMPRESSION;
int Compression::gzip_level = Z_DEFAULT_COMPRESSION;
int Compression::zstd_level = 3;
bool Compression::zstd_long_distance_matching = false;
int Compression::zstd_window_log_size = 27; // ZSTD_WINDOWLOG_LIMIT_DEFAULT
int Compression::gzip_chunk = 16384;
