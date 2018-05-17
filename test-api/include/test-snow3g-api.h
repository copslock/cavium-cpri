
/* ====================================================================
 * Copyright (c) 1998-2008 The OpenSSL Project.  All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#ifdef TEST_CPU_CYCLES
	static uint64_t start_cycle ;
	static uint64_t end_cycle ;
	static uint64_t cpucycles = 0;
	static uint64_t mbps = 0;
	static int iter;
	unsigned int core;
	cvmx_sysinfo_t *sysinfo;
#endif

#ifdef TEST_CPU_CYCLES
	#define PRINT_HDR \
		if(cvmx_is_init_core()) {\
			printf ("\n\n###################################################\n"); \
        	printf ("Printing CPU cycles for packet length :%u bytes\n", inlen ); \
        	printf ("###################################################\n");\
		}
		
#else
    #define PRINT_HDR
#endif

#ifdef TEST_CPU_CYCLES
	#define START_CYCLE  \
 		start_cycle = end_cycle = 0;  \
 		sysinfo = cvmx_sysinfo_get();\
		for (iter = 0; iter < MAX_ITERATIONS; iter++) {  \
        	start_cycle += cvmx_clock_get_count (CVMX_CLOCK_CORE);

	#define END_CYCLE(str) \
    	end_cycle += cvmx_clock_get_count (CVMX_CLOCK_CORE); \
 		} \
    	cpucycles = (long)(end_cycle - start_cycle)/MAX_ITERATIONS; \
    	mbps = inlen * cpufreq * 8 / cpucycles; \
		for (core = 0; core < CVMX_MAX_CORES; core++) {\
			if (cvmx_coremask_is_core_set(&sysinfo->core_mask, core) && core == cvmx_get_core_num()){ \
				total_cpucycles+=cpucycles;\
				total_mbps +=mbps;\
			}\
			cvmx_coremask_barrier_sync(&sysinfo->core_mask);\
		}\
		cvmx_coremask_barrier_sync(&sysinfo->core_mask);\
		if(cvmx_is_init_core()){\
 			printf ("API :%-20s total mbps :%-10lu average values per core    cpucycles :%lu mbps :%lu\n", str, total_mbps, (total_cpucycles/numcores), (total_mbps/numcores));\
			total_cpucycles = 0;\
			total_mbps = 0;\
		}
#else
	#define START_CYCLE
	#define END_CYCLE(str)
#endif
/* 
  These test vectors are taken from 
 "3GPP TS 35.203 V3.1.1 (2001-07)" document 
*/

typedef struct __F8TestVectors {
	uint8_t key[16];
	uint32_t count;
	uint8_t bearer;
	uint8_t direction;
	uint32_t length;
	uint8_t plaintext[2*1024];
	uint8_t ciphertext[2*1024];
} F8TestVectors;

/* Test Vectors from Implementor's Test Data */
F8TestVectors f8test_impl[] = {
	// Test Vector #1
	{
	"\x2B\xD6\x45\x9F\x82\xC5\xB3\x00\x95\x2C\x49\x10\x48\x81\xFF\x48",
	0x72A4F20F,
	0x0C,
	0x1,
	798,
	"\x7E\xC6\x12\x72\x74\x3B\xF1\x61\x47\x26\x44\x6A\x6C\x38\xCE\xD1\x66\xF6\xCA\x76\xEB\x54\x30\x04\x42\x86\x34\x6C\xEF\x13\x0F\x92\x92\x2B\x03\x45\x0D\x3A\x99\x75\xE5\xBD\x2E\xA0\xEB\x55\xAD\x8E\x1B\x19\x9E\x3E\xC4\x31\x60\x20\xE9\xA1\xB2\x85\xE7\x62\x79\x53\x59\xB7\xBD\xFD\x39\xBE\xF4\xB2\x48\x45\x83\xD5\xAF\xE0\x82\xAE\xE6\x38\xBF\x5F\xD5\xA6\x06\x19\x39\x01\xA0\x8F\x4A\xB4\x1A\xAB\x9B\x13\x48\x80",
	"\x8C\xEB\xA6\x29\x43\xDC\xED\x3A\x09\x90\xB0\x6E\xA1\xB0\xA2\xC4\xFB\x3C\xED\xC7\x1B\x36\x9F\x42\xBA\x64\xC1\xEB\x66\x65\xE7\x2A\xA1\xC9\xBB\x0D\xEA\xA2\x0F\xE8\x60\x58\xB8\xBA\xEE\x2C\x2E\x7F\x0B\xEC\xCE\x48\xB5\x29\x32\xA5\x3C\x9D\x5F\x93\x1A\x3A\x7C\x53\x22\x59\xAF\x43\x25\xE2\xA6\x5E\x30\x84\xAD\x5F\x6A\x51\x3B\x7B\xDD\xC1\xB6\x5F\x0A\xA0\xD9\x7A\x05\x3D\xB5\x5A\x88\xC4\xC4\xF9\x60\x5E\x41\x40"
	},

	// Test Vector #2
	{
	"\xEF\xA8\xB2\x22\x9E\x72\x0C\x2A\x7C\x36\xEA\x55\xE9\x60\x56\x95",
	0xE28BCF7B,
	0x18,
	0x0,
	510,
	"\x10\x11\x12\x31\xE0\x60\x25\x3A\x43\xFD\x3F\x57\xE3\x76\x07\xAB\x28\x27\xB5\x99\xB6\xB1\xBB\xDA\x37\xA8\xAB\xCC\x5A\x8C\x55\x0D\x1B\xFB\x2F\x49\x46\x24\xFB\x50\x36\x7F\xA3\x6C\xE3\xBC\x68\xF1\x1C\xF9\x3B\x15\x10\x37\x6B\x02\x13\x0F\x81\x2A\x9F\xA1\x69\xD8",
	"\xE0\xDA\x15\xCA\x8E\x25\x54\xF5\xE5\x6C\x94\x68\xDC\x6C\x7C\x12\x9C\x56\x8A\xA5\x03\x23\x17\xE0\x4E\x07\x29\x64\x6C\xAB\xEF\xA6\x89\x86\x4C\x41\x0F\x24\xF9\x19\xE6\x1E\x3D\xFD\xFA\xD7\x7E\x56\x0D\xB0\xA9\xCD\x36\xC3\x4A\xE4\x18\x14\x90\xB2\x9F\x5F\xA2\xFC"
	},

	// Test Vector #3
	{
	"\x5A\xCB\x1D\x64\x4C\x0D\x51\x20\x4E\xA5\xF1\x45\x10\x10\xD8\x52",
	0xFA556B26,
	0x03,
	0x1,
	120,
	"\xAD\x9C\x44\x1F\x89\x0B\x38\xC4\x57\xA4\x9D\x42\x14\x07\xE8",
	"\xBA\x0F\x31\x30\x03\x34\xC5\x6B\x52\xA7\x49\x7C\xBA\xC0\x46"
	},

	// Test Vector #4
	{
	"\xD3\xC5\xD5\x92\x32\x7F\xB1\x1C\x40\x35\xC6\x68\x0A\xF8\xC6\xD1",
	0x398A59B4,
	0x05,
	0x1,
	253,
	"\x98\x1B\xA6\x82\x4C\x1B\xFB\x1A\xB4\x85\x47\x20\x29\xB7\x1D\x80\x8C\xE3\x3E\x2C\xC3\xC0\xB5\xFC\x1F\x3D\xE8\xA6\xDC\x66\xB1\xF0",
	"\x98\x9B\x71\x9C\xDC\x33\xCE\xB7\xCF\x27\x6A\x52\x82\x7C\xEF\x94\xA5\x6C\x40\xC0\xAB\x9D\x81\xF7\xA2\xA9\xBA\xC6\x0E\x11\xC4\xB0"
	},

	// Test Vector #5
	{
	"\x60\x90\xEA\xE0\x4C\x83\x70\x6E\xEC\xBF\x65\x2B\xE8\xE3\x65\x66",
	0x72A4F20F,
	0x09,
	0x0,
	837,
	"\x40\x98\x1B\xA6\x82\x4C\x1B\xFB\x42\x86\xB2\x99\x78\x3D\xAF\x44\x2C\x09\x9F\x7A\xB0\xF5\x8D\x5C\x8E\x46\xB1\x04\xF0\x8F\x01\xB4\x1A\xB4\x85\x47\x20\x29\xB7\x1D\x36\xBD\x1A\x3D\x90\xDC\x3A\x41\xB4\x6D\x51\x67\x2A\xC4\xC9\x66\x3A\x2B\xE0\x63\xDA\x4B\xC8\xD2\x80\x8C\xE3\x3E\x2C\xCC\xBF\xC6\x34\xE1\xB2\x59\x06\x08\x76\xA0\xFB\xB5\xA4\x37\xEB\xCC\x8D\x31\xC1\x9E\x44\x54\x31\x87\x45\xE3\x98\x76\x45\x98\x7A\x98\x6F\x2C\xB0",
	"\x58\x92\xBB\xA8\x8B\xBB\xCA\xAE\xAE\x76\x9A\xA0\x6B\x68\x3D\x3A\x17\xCC\x04\xA3\x69\x88\x16\x97\x43\x5E\x44\xFE\xD5\xFF\x9A\xF5\x7B\x9E\x89\x0D\x4D\x5C\x64\x70\x98\x85\xD4\x8A\xE4\x06\x90\xEC\x04\x3B\xAA\xE9\x70\x57\x96\xE4\xA9\xFF\x5A\x4B\x8D\x8B\x36\xD7\xF3\xFE\x57\xCC\x6C\xFD\x6C\xD0\x05\xCD\x38\x52\xA8\x5E\x94\xCE\x6B\xCD\x90\xD0\xD0\x78\x39\xCE\x09\x73\x35\x44\xCA\x8E\x35\x08\x43\x24\x85\x50\x92\x2A\xC1\x28\x18"
	}
};

/* Test Vectors from Design Conformance Test Data */
F8TestVectors f8test_conf[] = {
	// Test Vector #1
	{
		"\xD3\xC5\xD5\x92\x32\x7F\xB1\x1C\x40\x35\xC6\x68\x0A\xF8\xC6\xD1",
		0x398A59B4,
		0x15,
		1,
		253,
		"\x98\x1B\xA6\x82\x4C\x1B\xFB\x1A\xB4\x85\x47\x20\x29\xB7\x1D\x80\x8C\xE3\x3E\x2C\xC3\xC0\xB5\xFC\x1F\x3D\xE8\xA6\xDC\x66\xB1\xF0",
		"\x5D\x5B\xFE\x75\xEB\x04\xF6\x8C\xE0\xA1\x23\x77\xEA\x00\xB3\x7D\x47\xC6\xA0\xBA\x06\x30\x91\x55\x08\x6A\x85\x9C\x43\x41\xB3\x78"
	},
	// Test Vector #2
	{
		"\x2B\xD6\x45\x9F\x82\xC4\x40\xE0\x95\x2C\x49\x10\x48\x05\xFF\x48",
		0xC675A64B,
		0x0C,
		1,
		798,
		"\x7E\xC6\x12\x72\x74\x3B\xF1\x61\x47\x26\x44\x6A\x6C\x38\xCE\xD1\x66\xF6\xCA\x76\xEB\x54\x30\x04\x42\x86\x34\x6C\xEF\x13\x0F\x92\x92\x2B\x03\x45\x0D\x3A\x99\x75\xE5\xBD\x2E\xA0\xEB\x55\xAD\x8E\x1B\x19\x9E\x3E\xC4\x31\x60\x20\xE9\xA1\xB2\x85\xE7\x62\x79\x53\x59\xB7\xBD\xFD\x39\xBE\xF4\xB2\x48\x45\x83\xD5\xAF\xE0\x82\xAE\xE6\x38\xBF\x5F\xD5\xA6\x06\x19\x39\x01\xA0\x8F\x4A\xB4\x1A\xAB\x9B\x13\x48\x80",
		"\x3F\x67\x85\x07\x14\xB8\xDA\x69\xEF\xB7\x27\xED\x7A\x6C\x0C\x50\x71\x4A\xD7\x36\xC4\xF5\x60\x00\x06\xE3\x52\x5B\xE8\x07\xC4\x67\xC6\x77\xFF\x86\x4A\xF4\x5F\xBA\x09\xC2\x7C\xDE\x38\xF8\x7A\x1F\x84\xD5\x9A\xB2\x55\x40\x8F\x2C\x7B\x82\xF9\xEA\xD4\x1A\x1F\xE6\x5E\xAB\xEB\xFB\xC1\xF3\xA4\xC5\x6C\x9A\x26\xFC\xF7\xB3\xD6\x6D\x02\x20\xEE\x47\x75\xBC\x58\x17\x0A\x2B\x12\xF3\x43\x1D\x11\xB3\x44\xD6\xE3\x6C"
	},
	// Test Vector #3
	{
		"\x0A\x8B\x6B\xD8\xD9\xB0\x8B\x08\xD6\x4E\x32\xD1\x81\x77\x77\xFB",
		0x544D49CD,
		0x04,
		0,
		310,
		"\xFD\x40\xA4\x1D\x37\x0A\x1F\x65\x74\x50\x95\x68\x7D\x47\xBA\x1D\x36\xD2\x34\x9E\x23\xF6\x44\x39\x2C\x8E\xA9\xC4\x9D\x40\xC1\x32\x71\xAF\xF2\x64\xD0\xF2\x48",
		"\x48\x14\x8E\x54\x52\xA2\x10\xC0\x5F\x46\xBC\x80\xDC\x6F\x73\x49\x5B\x02\x04\x8C\x1B\x95\x8B\x02\x61\x02\xCA\x97\x28\x02\x79\xA4\xC1\x8D\x2E\xE3\x08\x92\x1C"
	},
	// Test Vector #4
	{
		"\xAA\x1F\x95\xAE\xA5\x33\xBC\xB3\x2E\xB6\x3B\xF5\x2D\x8F\x83\x1A",
		0x72D8C671,
		0x10,
		1,
		1022,
		"\xFB\x1B\x96\xC5\xC8\xBA\xDF\xB2\xE8\xE8\xED\xFD\xE7\x8E\x57\xF2\xAD\x81\xE7\x41\x03\xFC\x43\x0A\x53\x4D\xCC\x37\xAF\xCE\xC7\x0E\x15\x17\xBB\x06\xF2\x72\x19\xDA\xE4\x90\x22\xDD\xC4\x7A\x06\x8D\xE4\xC9\x49\x6A\x95\x1A\x6B\x09\xED\xBD\xC8\x64\xC7\xAD\xBD\x74\x0A\xC5\x0C\x02\x2F\x30\x82\xBA\xFD\x22\xD7\x81\x97\xC5\xD5\x08\xB9\x77\xBC\xA1\x3F\x32\xE6\x52\xE7\x4B\xA7\x28\x57\x60\x77\xCE\x62\x8C\x53\x5E\x87\xDC\x60\x77\xBA\x07\xD2\x90\x68\x59\x0C\x8C\xB5\xF1\x08\x8E\x08\x2C\xFA\x0E\xC9\x61\x30\x2D\x69\xCF\x3D\x44",
		"\xFF\xCF\xC2\xFE\xAD\x6C\x09\x4E\x96\xC5\x89\xD0\xF6\x77\x9B\x67\x84\x24\x6C\x3C\x4D\x1C\xEA\x20\x3D\xB3\x90\x1F\x40\xAD\x4F\xD7\x13\x8B\xC6\xD7\x7E\x83\x20\xCB\x10\x2F\x49\x7F\xDD\x44\xA2\x69\xA9\x6E\xCB\x28\x61\x77\x00\xE3\x32\xEB\x2F\x73\x6B\x34\xF4\xF2\x69\x30\x94\xE2\x2F\xF9\x4F\x9B\xE4\x72\x3D\xA4\x0C\x40\xDF\xD3\x93\x1C\xC1\xAC\x97\x23\xF6\xB4\xA9\x91\x3E\x96\xB6\xDB\x7A\xBC\xAC\xE4\x15\x17\x7C\x1D\x01\x15\xC5\xF0\x9B\x5F\xDE\xA0\xB3\xAD\xB8\xF9\xDA\x6E\x9F\x9A\x04\xC5\x43\x39\x7B\x9D\x43\xF8\x73\x30"
	},
	// Test Vector #5
	{
		"\x96\x18\xAE\x46\x89\x1F\x86\x57\x8E\xEB\xE9\x0E\xF7\xA1\x20\x2E",
		0xC675A64B,
		0x0c,
		1,
		1245,
		"\x8D\xAA\x17\xB1\xAE\x05\x05\x29\xC6\x82\x7F\x28\xC0\xEF\x6A\x12\x42\xE9\x3F\x8B\x31\x4F\xB1\x8A\x77\xF7\x90\xAE\x04\x9F\xED\xD6\x12\x26\x7F\xEC\xAE\xFC\x45\x01\x74\xD7\x6D\x9F\x9A\xA7\x75\x5A\x30\xCD\x90\xA9\xA5\x87\x4B\xF4\x8E\xAF\x70\xEE\xA3\xA6\x2A\x25\x0A\x8B\x6B\xD8\xD9\xB0\x8B\x08\xD6\x4E\x32\xD1\x81\x77\x77\xFB\x54\x4D\x49\xCD\x49\x72\x0E\x21\x9D\xBF\x8B\xBE\xD3\x39\x04\xE1\xFD\x40\xA4\x1D\x37\x0A\x1F\x65\x74\x50\x95\x68\x7D\x47\xBA\x1D\x36\xD2\x34\x9E\x23\xF6\x44\x39\x2C\x8E\xA9\xC4\x9D\x40\xC1\x32\x71\xAF\xF2\x64\xD0\xF2\x48\x41\xD6\x46\x5F\x09\x96\xFF\x84\xE6\x5F\xC5\x17\xC5\x3E\xFC\x33\x63\xC3\x84\x92\xA8",
		"\x6C\xDB\x18\xA7\xCA\x82\x18\xE8\x6E\x4B\x4B\x71\x6A\x4D\x04\x37\x1F\xBE\xC2\x62\xFC\x5A\xD0\xB3\x81\x9B\x18\x7B\x97\xE5\x5B\x1A\x4D\x7C\x19\xEE\x24\xC8\xB4\xD7\x72\x3C\xFE\xDF\x04\x5B\x8A\xCA\xE4\x86\x95\x17\xD8\x0E\x50\x61\x5D\x90\x35\xD5\xD9\xC5\xA4\x0A\xF6\x02\x28\x0B\x54\x25\x97\xB0\xCB\x18\x61\x9E\xEB\x35\x92\x57\x59\xD1\x95\xE1\x00\xE8\xE4\xAA\x0C\x38\xA3\xC2\xAB\xE0\xF3\xD8\xFF\x04\xF3\xC3\x3C\x29\x50\x69\xC2\x36\x94\xB5\xBB\xEA\xCD\xD5\x42\xE2\x8E\x8A\x94\xED\xB9\x11\x9F\x41\x2D\x05\x4B\xE1\xFA\x72\x72\xB5\xFF\xB2\xB2\x57\x0F\x4F\x7C\xEA\xF3\x83\xA8\xA9\xD9\x35\x72\xF0\x4D\x6E\x3A\x6E\x29\x37\x26\xEC\x62\xC8"
	},
	// Test Vector #6
	{
		"\x54\xF4\xE2\xE0\x4C\x83\x78\x6E\xEC\x8F\xB5\xAB\xE8\xE3\x65\x66",
		0xACA4F50F,
		0x0b,
		0,
		3861,
		"\x40\x98\x1B\xA6\x82\x4C\x1B\xFB\x42\x86\xB2\x99\x78\x3D\xAF\x44\x2C\x09\x9F\x7A\xB0\xF5\x8D\x5C\x8E\x46\xB1\x04\xF0\x8F\x01\xB4\x1A\xB4\x85\x47\x20\x29\xB7\x1D\x36\xBD\x1A\x3D\x90\xDC\x3A\x41\xB4\x6D\x51\x67\x2A\xC4\xC9\x66\x3A\x2B\xE0\x63\xDA\x4B\xC8\xD2\x80\x8C\xE3\x3E\x2C\xCC\xBF\xC6\x34\xE1\xB2\x59\x06\x08\x76\xA0\xFB\xB5\xA4\x37\xEB\xCC\x8D\x31\xC1\x9E\x44\x54\x31\x87\x45\xE3\xFA\x16\xBB\x11\xAD\xAE\x24\x88\x79\xFE\x52\xDB\x25\x43\xE5\x3C\xF4\x45\xD3\xD8\x28\xCE\x0B\xF5\xC5\x60\x59\x3D\x97\x27\x8A\x59\x76\x2D\xD0\xC2\xC9\xCD\x68\xD4\x49\x6A\x79\x25\x08\x61\x40\x14\xB1\x3B\x6A\xA5\x11\x28\xC1\x8C\xD6\xA9\x0B\x87\x97\x8C\x2F\xF1\xCA\xBE\x7D\x9F\x89\x8A\x41\x1B\xFD\xB8\x4F\x68\xF6\x72\x7B\x14\x99\xCD\xD3\x0D\xF0\x44\x3A\xB4\xA6\x66\x53\x33\x0B\xCB\xA1\x10\x5E\x4C\xEC\x03\x4C\x73\xE6\x05\xB4\x31\x0E\xAA\xAD\xCF\xD5\xB0\xCA\x27\xFF\xD8\x9D\x14\x4D\xF4\x79\x27\x59\x42\x7C\x9C\xC1\xF8\xCD\x8C\x87\x20\x23\x64\xB8\xA6\x87\x95\x4C\xB0\x5A\x8D\x4E\x2D\x99\xE7\x3D\xB1\x60\xDE\xB1\x80\xAD\x08\x41\xE9\x67\x41\xA5\xD5\x9F\xE4\x18\x9F\x15\x42\x00\x26\xFE\x4C\xD1\x21\x04\x93\x2F\xB3\x8F\x73\x53\x40\x43\x8A\xAF\x7E\xCA\x6F\xD5\xCF\xD3\xA1\x95\xCE\x5A\xBE\x65\x27\x2A\xF6\x07\xAD\xA1\xBE\x65\xA6\xB4\xC9\xC0\x69\x32\x34\x09\x2C\x4D\x01\x8F\x17\x56\xC6\xDB\x9D\xC8\xA6\xD8\x0B\x88\x81\x38\x61\x6B\x68\x12\x62\xF9\x54\xD0\xE7\x71\x17\x48\x78\x0D\x92\x29\x1D\x86\x29\x99\x72\xDB\x74\x1C\xFA\x4F\x37\xB8\xB5\x6C\xDB\x18\xA7\xCA\x82\x18\xE8\x6E\x4B\x4B\x71\x6A\x4D\x04\x37\x1F\xBE\xC2\x62\xFC\x5A\xD0\xB3\x81\x9B\x18\x7B\x97\xE5\x5B\x1A\x4D\x7C\x19\xEE\x24\xC8\xB4\xD7\x72\x3C\xFE\xDF\x04\x5B\x8A\xCA\xE4\x86\x95\x17\xD8\x0E\x50\x61\x5D\x90\x35\xD5\xD9\xC5\xA4\x0A\xF6\x02\x28\x0B\x54\x25\x97\xB0\xCB\x18\x61\x9E\xEB\x35\x92\x57\x59\xD1\x95\xE1\x00\xE8\xE4\xAA\x0C\x38\xA3\xC2\xAB\xE0\xF3\xD8\xFF\x04\xF3\xC3\x3C\x29\x50\x69\xC2\x36\x94\xB5\xBB\xEA\xCD\xD5\x42\xE2\x8E\x8A\x94\xED\xB9\x11\x9F\x41\x2D\x05\x4B\xE1\xFA\x72\xB0\x95\x50",
		"\x35\x1E\x30\xD4\xD9\x10\xC5\xDD\x5A\xD7\x83\x4C\x42\x6E\x6C\x0C\xAB\x64\x86\xDA\x7B\x0F\xDA\x4C\xD8\x3A\xF1\xB9\x64\x71\x37\xF1\xAC\x43\xB4\x34\x22\x3B\x19\xBE\x07\xBD\x89\xD1\xCC\x30\x69\x44\xD3\x36\x1E\xA1\xA2\xF8\xCD\xBD\x32\x16\x55\x97\x63\x50\xD0\x0B\x80\xDD\x83\x81\x20\xA7\x75\x5C\x6D\xEA\x2A\xB2\xB0\xC9\x9A\x91\x3F\x47\xDA\xE2\xB8\xDE\xB9\xA8\x29\xE5\x46\x9F\xF2\xE1\x87\x77\x6F\x6F\xD0\x81\xE3\x87\x1D\x11\x9A\x76\xE2\x4C\x91\x7E\xA6\x26\x48\xE0\x2E\x90\x36\x75\x64\xDE\x72\xAE\x7E\x4F\x0A\x42\x49\xA9\xA5\xB0\xE4\x65\xA2\xD6\xD9\xDC\x87\x84\x3B\x1B\x87\x5C\xC9\xA3\xBE\x93\xD8\xDA\x8F\x56\xEC\xAF\x59\x81\xFE\x93\xC2\x84\x31\x8B\x0D\xEC\x7A\x3B\xA1\x08\xE2\xCB\x1A\x61\xE9\x66\xFA\x7A\xFA\x7A\xC7\xF6\x7F\x65\xBC\x4A\x2D\xF0\x70\xD4\xE4\x34\x84\x5F\x10\x9A\xB2\xB6\x8A\xDE\x3D\xC3\x16\xCA\x63\x32\xA6\x28\x93\xE0\xA7\xEC\x0B\x4F\xC2\x51\x91\xBF\x2F\xF1\xB9\xF9\x81\x5E\x4B\xA8\xA9\x9C\x64\x3B\x52\x18\x04\xF7\xD5\x85\x0D\xDE\x39\x52\x20\x6E\xC6\xCC\xF3\x40\xF9\xB3\x22\x0B\x30\x23\xBD\xD0\x63\x95\x6E\xA8\x33\x39\x20\xFD\xE9\x9E\x06\x75\x41\x0E\x49\xEF\x3B\x4D\x3F\xB3\xDF\x51\x92\xF9\x9C\xA8\x3D\x3B\x00\x32\xDE\x08\xC2\x20\x77\x6A\x58\x65\xB0\xE4\xB3\xB0\xC7\x5D\xEF\xE7\x76\x2D\xFF\x01\x8E\xA7\xF5\xBE\x2B\x2F\x97\x2B\x2A\x8B\xA5\x97\x0E\x43\xBD\x6F\xDD\x63\xDA\xE6\x29\x78\x4E\xC4\x8D\x61\x00\x54\xEE\x4E\x4B\x5D\xBB\xF1\xFC\x2F\xA0\xB8\x30\xE9\x4D\xCB\xB7\x01\x4E\x8A\xB4\x29\xAB\x10\x0F\xC4\x8F\x83\x17\x1D\x99\xFC\x25\x8B\x7C\x2B\xA7\xC1\x76\xEA\xEA\xAD\x37\xF8\x60\xD5\x97\xA3\x1C\xE7\x9B\x59\x47\x33\xC7\x14\x1D\xF7\x91\x51\xFC\xA9\x0C\x08\x47\x8A\x5C\x6C\x2C\xC4\x81\xD5\x1F\xFE\xCE\x3C\xD7\xD2\x58\x13\x48\x82\x7A\x71\xF0\x91\x42\x8E\xBE\x38\xC9\x5A\x3F\x5C\x63\xE0\x56\xDF\xB7\xCC\x45\xA9\xB7\xC0\x7D\x83\x4E\x7B\x20\xB9\x9E\xD2\x02\x42\x9C\x14\xBB\x85\xFF\xA4\x3B\x7C\xB6\x84\x95\xCD\x75\xAB\x66\xD9\x64\xD4\xCA\xFE\x64\xDD\x94\x04\xDA\xE2\xDC\x51\x10\x61\x7F\x19\x4F\xC3\xC1\x84\xF5\x83\xCD\x0D\xEF\x6D\x00"
	},
};

typedef struct __F9TestVectors {
	uint8_t   key[16];
	uint32_t  count;
	uint32_t  fresh;
	uint8_t   direction;
	uint32_t  length;
	uint8_t   message[2100];
	uint32_t  expected_mac; 
} F9TestVectors;

/* Test Vectors from Implementor's Test Data */
F9TestVectors f9test_impl[] = {
	// Test Vector #1
	{
	"\x2B\xD6\x45\x9F\x82\xC5\xB3\x00\x95\x2C\x49\x10\x48\x81\xFF\x48",
	0x38A6F056,
	0x05D2EC49,
	0x0,
	189,
	"\x6B\x22\x77\x37\x29\x6F\x39\x3C\x80\x79\x35\x3E\xDC\x87\xE2\xE8\x05\xD2\xEC\x49\xA4\xF2\xD8\xE0",
	0x2BCE1820
	},

	// Test Vector #2
	{
	"\xD4\x2F\x68\x24\x28\x20\x1C\xAF\xCD\x9F\x97\x94\x5E\x6D\xE7\xB7",
	0x3EDC87E2,
	0xA4F2D8E2,
	0x1,
	254,
	"\xB5\x92\x43\x84\x32\x8A\x4A\xE0\x0B\x73\x71\x09\xF8\xB6\xC8\xDD\x2B\x4D\xB6\x3D\xD5\x33\x98\x1C\xEB\x19\xAA\xD5\x2A\x5B\x2B\xC0",
	0xFC7B18BD
	},

	// Test Vector #3
	{
	"\xFD\xB9\xCF\xDF\x28\x93\x6C\xC4\x83\xA3\x18\x69\xD8\x1B\x8F\xAB",
	0x36AF6144,
	0x9838F03A,
	0x01,
	319,
	"\x59\x32\xBC\x0A\xCE\x2B\x0A\xBA\x33\xD8\xAC\x18\x8A\xC5\x4F\x34\x6F\xAD\x10\xBF\x9D\xEE\x29\x20\xB4\x3B\xD0\xC5\x3A\x91\x5C\xB7\xDF\x6C\xAA\x72\x05\x3A\xBF\xF2",
	0x02F1FAAF
	},

	// Test Vector #4
	{
	"\xC7\x36\xC6\xAA\xB2\x2B\xFF\xF9\x1E\x26\x98\xD2\xE2\x2A\xD5\x7E",
	0x14793E41,
	0x0397E8FD,
	0x01,
	384,
	"\xD0\xA7\xD4\x63\xDF\x9F\xB2\xB2\x78\x83\x3F\xA0\x2E\x23\x5A\xA1\x72\xBD\x97\x0C\x14\x73\xE1\x29\x07\xFB\x64\x8B\x65\x99\xAA\xA0\xB2\x4A\x03\x86\x65\x42\x2B\x20\xA4\x99\x27\x6A\x50\x42\x70\x09",
	0x38B554C0
	},

	// Test Vector #5
	{
	"\xF4\xEB\xEC\x69\xE7\x3E\xAF\x2E\xB2\xCF\x6A\xF4\xB3\x12\x0F\xFD",
	0x296F393C,
	0x6B227737,
	0x01,
	1000,
	"\x10\xBF\xFF\x83\x9E\x0C\x71\x65\x8D\xBB\x2D\x17\x07\xE1\x45\x72\x4F\x41\xC1\x6F\x48\xBF\x40\x3C\x3B\x18\xE3\x8F\xD5\xD1\x66\x3B\x6F\x6D\x90\x01\x93\xE3\xCE\xA8\xBB\x4F\x1B\x4F\x5B\xE8\x22\x03\x22\x32\xA7\x8D\x7D\x75\x23\x8D\x5E\x6D\xAE\xCD\x3B\x43\x22\xCF\x59\xBC\x7E\xA8\x4A\xB1\x88\x11\xB5\xBF\xB7\xBC\x55\x3F\x4F\xE4\x44\x78\xCE\x28\x7A\x14\x87\x99\x90\xD1\x8D\x12\xCA\x79\xD2\xC8\x55\x14\x90\x21\xCD\x5C\xE8\xCA\x03\x71\xCA\x04\xFC\xCE\x14\x3E\x3D\x7C\xFE\xE9\x45\x85\xB5\x88\x5C\xAC\x46\x06\x8B",
	0x061745AE
	},
};

/* Test Vectors from Design Conformance Test Data */
F9TestVectors f9test_conf[] = {
	// Test Vector #1
	{
		"\x2B\xD6\x45\x9F\x82\xC5\xB3\x00\x95\x2C\x49\x10\x48\x81\xFF\x48",
		0x38A6F056,
		0xB8AEFDA9,
		0,
		88,
		"\x33\x32\x34\x62\x63\x39\x38\x61\x37\x34\x79",
		0xEE419E0D
	},
	// Test Vector #2
	{
		"\x7E\x5E\x94\x43\x1E\x11\xD7\x38\x28\xD7\x39\xCC\x6C\xED\x45\x73",
		0x36AF6144,
		0x9838F03A,
		1,
		254,
		"\xB3\xD3\xC9\x17\x0A\x4E\x16\x32\xF6\x0F\x86\x10\x13\xD2\x2D\x84\xB7\x26\xB6\xA2\x78\xD8\x02\xD1\xEE\xAF\x13\x21\xBA\x59\x29\xDC",
		0x92F2A453
	},
	// Test Vector #3
	{
		"\xD3\x41\x9B\xE8\x21\x08\x7A\xCD\x02\x12\x3A\x92\x48\x03\x33\x59",
		0xC7590EA9,
		0x57D5DF7D,
		0,
		511,
		"\xBB\xB0\x57\x03\x88\x09\x49\x6B\xCF\xF8\x6D\x6F\xBC\x8C\xE5\xB1\x35\xA0\x6B\x16\x60\x54\xF2\xD5\x65\xBE\x8A\xCE\x75\xDC\x85\x1E\x0B\xCD\xD8\xF0\x71\x41\xC4\x95\x87\x2F\xB5\xD8\xC0\xC6\x6A\x8B\x6D\xA5\x56\x66\x3E\x4E\x46\x12\x05\xD8\x45\x80\xBE\xE5\xBC\x7E",
		0xAD8C69F9
	},
	// Test Vector #4
	{
		"\x83\xFD\x23\xA2\x44\xA7\x4C\xF3\x58\xDA\x30\x19\xF1\x72\x26\x35",
		0x36AF6144,
		0x4F302AD2,
		1,
		768,
		"\x35\xC6\x87\x16\x63\x3C\x66\xFB\x75\x0C\x26\x68\x65\xD5\x3C\x11\xEA\x05\xB1\xE9\xFA\x49\xC8\x39\x8D\x48\xE1\xEF\xA5\x90\x9D\x39\x47\x90\x28\x37\xF5\xAE\x96\xD5\xA0\x5B\xC8\xD6\x1C\xA8\xDB\xEF\x1B\x13\xA4\xB4\xAB\xFE\x4F\xB1\x00\x60\x45\xB6\x74\xBB\x54\x72\x93\x04\xC3\x82\xBE\x53\xA5\xAF\x05\x55\x61\x76\xF6\xEA\xA2\xEF\x1D\x05\xE4\xB0\x83\x18\x1E\xE6\x74\xCD\xA5\xA4\x85\xF7\x4D\x7A",
		0x7306D607
	},
	// Test Vector #5
	{
		"\x68\x32\xA6\x5C\xFF\x44\x73\x62\x1E\xBD\xD4\xBA\x26\xA9\x21\xFE",
		0x36AF6144,
		0x9838F03A,
		0,
		383,
		"\xD3\xC5\x38\x39\x62\x68\x20\x71\x77\x65\x66\x76\x20\x32\x38\x37\x63\x62\x40\x98\x1B\xA6\x82\x4C\x1B\xFB\x1A\xB4\x85\x47\x20\x29\xB7\x1D\x80\x8C\xE3\x3E\x2C\xC3\xC0\xB5\xFC\x1F\x3D\xE8\xA6\xDC",
		0xE3D36EF1
	},
	// Test Vector #6
	{
		"\x5D\x0A\x80\xD8\x13\x4A\xE1\x96\x77\x82\x4B\x67\x1E\x83\x8A\xF4",
		0x7827FAB2,
		0xA56C6CA2,
		1,
		2558,
		"\x70\xDE\xDF\x2D\xC4\x2C\x5C\xBD\x3A\x96\xF8\xA0\xB1\x14\x18\xB3\x60\x8D\x57\x33\x60\x4A\x2C\xD3\x6A\xAB\xC7\x0C\xE3\x19\x3B\xB5\x15\x3B\xE2\xD3\xC0\x6D\xFD\xB2\xD1\x6E\x9C\x35\x71\x58\xBE\x6A\x41\xD6\xB8\x61\xE4\x91\xDB\x3F\xBF\xEB\x51\x8E\xFC\xF0\x48\xD7\xD5\x89\x53\x73\x0F\xF3\x0C\x9E\xC4\x70\xFF\xCD\x66\x3D\xC3\x42\x01\xC3\x6A\xDD\xC0\x11\x1C\x35\xB3\x8A\xFE\xE7\xCF\xDB\x58\x2E\x37\x31\xF8\xB4\xBA\xA8\xD1\xA8\x9C\x06\xE8\x11\x99\xA9\x71\x62\x27\xBE\x34\x4E\xFC\xB4\x36\xDD\xD0\xF0\x96\xC0\x64\xC3\xB5\xE2\xC3\x99\x99\x3F\xC7\x73\x94\xF9\xE0\x97\x20\xA8\x11\x85\x0E\xF2\x3B\x2E\xE0\x5D\x9E\x61\x73\x60\x9D\x86\xE1\xC0\xC1\x8E\xA5\x1A\x01\x2A\x00\xBB\x41\x3B\x9C\xB8\x18\x8A\x70\x3C\xD6\xBA\xE3\x1C\xC6\x7B\x34\xB1\xB0\x00\x19\xE6\xA2\xB2\xA6\x90\xF0\x26\x71\xFE\x7C\x9E\xF8\xDE\xC0\x09\x4E\x53\x37\x63\x47\x8D\x58\xD2\xC5\xF5\xB8\x27\xA0\x14\x8C\x59\x48\xA9\x69\x31\xAC\xF8\x4F\x46\x5A\x64\xE6\x2C\xE7\x40\x07\xE9\x91\xE3\x7E\xA8\x23\xFA\x0F\xB2\x19\x23\xB7\x99\x05\xB7\x33\xB6\x31\xE6\xC7\xD6\x86\x0A\x38\x31\xAC\x35\x1A\x9C\x73\x0C\x52\xFF\x72\xD9\xD3\x08\xEE\xDB\xAB\x21\xFD\xE1\x43\xA0\xEA\x17\xE2\x3E\xDC\x1F\x74\xCB\xB3\x63\x8A\x20\x33\xAA\xA1\x54\x64\xEA\xA7\x33\x38\x5D\xBB\xEB\x6F\xD7\x35\x09\xB8\x57\xE6\xA4\x19\xDC\xA1\xD8\x90\x7A\xF9\x77\xFB\xAC\x4D\xFA\x35\xEC",
		0xC058D244
	},
	// Test Vector #7
	{
		"\xB3\x12\x0F\xFD\xB2\xCF\x6A\xF4\xE7\x3E\xAF\x2E\xF4\xEB\xEC\x69",
		0x296F393C,
		0x6B227737,
		1,
		16448,
		"\x00\x00\x00\x00\x00\x00\x00\x00\x01\x01\x01\x01\x01\x01\x01\x01\xE0\x95\x80\x45\xF3\xA0\xBB\xA4\xE3\x96\x83\x46\xF0\xA3\xB8\xA7\xC0\x2A\x01\x8A\xE6\x40\x76\x52\x26\xB9\x87\xC9\x13\xE6\xCB\xF0\x83\x57\x00\x16\xCF\x83\xEF\xBC\x61\xC0\x82\x51\x3E\x21\x56\x1A\x42\x7C\x00\x9D\x28\xC2\x98\xEF\xAC\xE7\x8E\xD6\xD5\x6C\x2D\x45\x05\xAD\x03\x2E\x9C\x04\xDC\x60\xE7\x3A\x81\x69\x6D\xA6\x65\xC6\xC4\x86\x03\xA5\x7B\x45\xAB\x33\x22\x15\x85\xE6\x8E\xE3\x16\x91\x87\xFB\x02\x39\x52\x86\x32\xDD\x65\x6C\x80\x7E\xA3\x24\x8B\x7B\x46\xD0\x02\xB2\xB5\xC7\x45\x8E\xB8\x5B\x9C\xE9\x58\x79\xE0\x34\x08\x59\x05\x5E\x3B\x0A\xBB\xC3\xEA\xCE\x87\x19\xCA\xA8\x02\x65\xC9\x72\x05\xD5\xDC\x4B\xCC\x90\x2F\xE1\x83\x96\x29\xED\x71\x32\x8A\x0F\x04\x49\xF5\x88\x55\x7E\x68\x98\x86\x0E\x04\x2A\xEC\xD8\x4B\x24\x04\xC2\x12\xC9\x22\x2D\xA5\xBF\x8A\x89\xEF\x67\x97\x87\x0C\xF5\x07\x71\xA6\x0F\x66\xA2\xEE\x62\x85\x36\x57\xAD\xDF\x04\xCD\xDE\x07\xFA\x41\x4E\x11\xF1\x2B\x4D\x81\xB9\xB4\xE8\xAC\x53\x8E\xA3\x06\x66\x68\x8D\x88\x1F\x6C\x34\x84\x21\x99\x2F\x31\xB9\x4F\x88\x06\xED\x8F\xCC\xFF\x4C\x91\x23\xB8\x96\x42\x52\x7A\xD6\x13\xB1\x09\xBF\x75\x16\x74\x85\xF1\x26\x8B\xF8\x84\xB4\xCD\x23\xD2\x9A\x09\x34\x92\x57\x03\xD6\x34\x09\x8F\x77\x67\xF1\xBE\x74\x91\xE7\x08\xA8\xBB\x94\x9A\x38\x73\x70\x8A\xEF\x4A\x36\x23\x9E\x50\xCC\x08\x23\x5C\xD5\xED\x6B\xBE\x57\x86\x68\xA1\x7B\x58\xC1\x17\x1D\x0B\x90\xE8\x13\xA9\xE4\xF5\x8A\x89\xD7\x19\xB1\x10\x42\xD6\x36\x0B\x1B\x0F\x52\xDE\xB7\x30\xA5\x8D\x58\xFA\xF4\x63\x15\x95\x4B\x0A\x87\x26\x91\x47\x59\x77\xDC\x88\xC0\xD7\x33\xFE\xFF\x54\x60\x0A\x0C\xC1\xD0\x30\x0A\xAA\xEB\x94\x57\x2C\x6E\x95\xB0\x1A\xE9\x0D\xE0\x4F\x1D\xCE\x47\xF8\x7E\x8F\xA7\xBE\xBF\x77\xE1\xDB\xC2\x0D\x6B\xA8\x5C\xB9\x14\x3D\x51\x8B\x28\x5D\xFA\x04\xB6\x98\xBF\x0C\xF7\x81\x9F\x20\xFA\x7A\x28\x8E\xB0\x70\x3D\x99\x5C\x59\x94\x0C\x7C\x66\xDE\x57\xA9\xB7\x0F\x82\x37\x9B\x70\xE2\x03\x1E\x45\x0F\xCF\xD2\x18\x13\x26\xFC\xD2\x8D\x88\x23\xBA\xAA\x80\xDF\x6E\x0F\x44\x35\x59\x64\x75\x39\xFD\x89\x07\xC0\xFF\xD9\xD7\x9C\x13\x0E\xD8\x1C\x9A\xFD\x9B\x7E\x84\x8C\x9F\xED\x38\x44\x3D\x5D\x38\x0E\x53\xFB\xDB\x8A\xC8\xC3\xD3\xF0\x68\x76\x05\x4F\x12\x24\x61\x10\x7D\xE9\x2F\xEA\x09\xC6\xF6\x92\x3A\x18\x8D\x53\xAF\xE5\x4A\x10\xF6\x0E\x6E\x9D\x5A\x03\xD9\x96\xB5\xFB\xC8\x20\xF8\xA6\x37\x11\x6A\x27\xAD\x04\xB4\x44\xA0\x93\x2D\xD6\x0F\xBD\x12\x67\x1C\x11\xE1\xC0\xEC\x73\xE7\x89\x87\x9F\xAA\x3D\x42\xC6\x4D\x20\xCD\x12\x52\x74\x2A\x37\x68\xC2\x5A\x90\x15\x85\x88\x8E\xCE\xE1\xE6\x12\xD9\x93\x6B\x40\x3B\x07\x75\x94\x9A\x66\xCD\xFD\x99\xA2\x9B\x13\x45\xBA\xA8\xD9\xD5\x40\x0C\x91\x02\x4B\x0A\x60\x73\x63\xB0\x13\xCE\x5D\xE9\xAE\x86\x9D\x3B\x8D\x95\xB0\x57\x0B\x3C\x2D\x39\x14\x22\xD3\x24\x50\xCB\xCF\xAE\x96\x65\x22\x86\xE9\x6D\xEC\x12\x14\xA9\x34\x65\x27\x98\x0A\x81\x92\xEA\xC1\xC3\x9A\x3A\xAF\x6F\x15\x35\x1D\xA6\xBE\x76\x4D\xF8\x97\x72\xEC\x04\x07\xD0\x6E\x44\x15\xBE\xFA\xE7\xC9\x25\x80\xDF\x9B\xF5\x07\x49\x7C\x8F\x29\x95\x16\x0D\x4E\x21\x8D\xAA\xCB\x02\x94\x4A\xBF\x83\x34\x0C\xE8\xBE\x16\x86\xA9\x60\xFA\xF9\x0E\x2D\x90\xC5\x5C\xC6\x47\x5B\xAB\xC3\x17\x1A\x80\xA3\x63\x17\x49\x54\x95\x5D\x71\x01\xDA\xB1\x6A\xE8\x17\x91\x67\xE2\x14\x44\xB4\x43\xA9\xEA\xAA\x7C\x91\xDE\x36\xD1\x18\xC3\x9D\x38\x9F\x8D\xD4\x46\x9A\x84\x6C\x9A\x26\x2B\xF7\xFA\x18\x48\x7A\x79\xE8\xDE\x11\x69\x9E\x0B\x8F\xDF\x55\x7C\xB4\x87\x19\xD4\x53\xBA\x71\x30\x56\x10\x9B\x93\xA2\x18\xC8\x96\x75\xAC\x19\x5F\xB4\xFB\x06\x63\x9B\x37\x97\x14\x49\x55\xB3\xC9\x32\x7D\x1A\xEC\x00\x3D\x42\xEC\xD0\xEA\x98\xAB\xF1\x9F\xFB\x4A\xF3\x56\x1A\x67\xE7\x7C\x35\xBF\x15\xC5\x9C\x24\x12\xDA\x88\x1D\xB0\x2B\x1B\xFB\xCE\xBF\xAC\x51\x52\xBC\x99\xBC\x3F\x1D\x15\xF7\x71\x00\x1B\x70\x29\xFE\xDB\x02\x8F\x8B\x85\x2B\xC4\x40\x7E\xB8\x3F\x89\x1C\x9C\xA7\x33\x25\x4F\xDD\x1E\x9E\xDB\x56\x91\x9C\xE9\xFE\xA2\x1C\x17\x40\x72\x52\x1C\x18\x31\x9A\x54\xB5\xD4\xEF\xBE\xBD\xDF\x1D\x8B\x69\xB1\xCB\xF2\x5F\x48\x9F\xCC\x98\x13\x72\x54\x7C\xF4\x1D\x00\x8E\xF0\xBC\xA1\x92\x6F\x93\x4B\x73\x5E\x09\x0B\x3B\x25\x1E\xB3\x3A\x36\xF8\x2E\xD9\xB2\x9C\xF4\xCB\x94\x41\x88\xFA\x0E\x1E\x38\xDD\x77\x8F\x7D\x1C\x9D\x98\x7B\x28\xD1\x32\xDF\xB9\x73\x1F\xA4\xF4\xB4\x16\x93\x5B\xE4\x9D\xE3\x05\x16\xAF\x35\x78\x58\x1F\x2F\x13\xF5\x61\xC0\x66\x33\x61\x94\x1E\xAB\x24\x9A\x4B\xC1\x23\xF8\xD1\x5C\xD7\x11\xA9\x56\xA1\xBF\x20\xFE\x6E\xB7\x8A\xEA\x23\x73\x36\x1D\xA0\x42\x6C\x79\xA5\x30\xC3\xBB\x1D\xE0\xC9\x97\x22\xEF\x1F\xDE\x39\xAC\x2B\x00\xA0\xA8\xEE\x7C\x80\x0A\x08\xBC\x22\x64\xF8\x9F\x4E\xFF\xE6\x27\xAC\x2F\x05\x31\xFB\x55\x4F\x6D\x21\xD7\x4C\x59\x0A\x70\xAD\xFA\xA3\x90\xBD\xFB\xB3\xD6\x8E\x46\x21\x5C\xAB\x18\x7D\x23\x68\xD5\xA7\x1F\x5E\xBE\xC0\x81\xCD\x3B\x20\xC0\x82\xDB\xE4\xCD\x2F\xAC\xA2\x87\x73\x79\x5D\x6B\x0C\x10\x20\x4B\x65\x9A\x93\x9E\xF2\x9B\xBE\x10\x88\x24\x36\x24\x42\x99\x27\xA7\xEB\x57\x6D\xD3\xA0\x0E\xA5\xE0\x1A\xF5\xD4\x75\x83\xB2\x27\x2C\x0C\x16\x1A\x80\x65\x21\xA1\x6F\xF9\xB0\xA7\x22\xC0\xCF\x26\xB0\x25\xD5\x83\x6E\x22\x58\xA4\xF7\xD4\x77\x3A\xC8\x01\xE4\x26\x3B\xC2\x94\xF4\x3D\xEF\x7F\xA8\x70\x3F\x3A\x41\x97\x46\x35\x25\x88\x76\x52\xB0\xB2\xA4\xA2\xA7\xCF\x87\xF0\x09\x14\x87\x1E\x25\x03\x91\x13\xC7\xE1\x61\x8D\xA3\x40\x64\xB5\x7A\x43\xC4\x63\x24\x9F\xB8\xD0\x5E\x0F\x26\xF4\xA6\xD8\x49\x72\xE7\xA9\x05\x48\x24\x14\x5F\x91\x29\x5C\xDB\xE3\x9A\x6F\x92\x0F\xAC\xC6\x59\x71\x2B\x46\xA5\x4B\xA2\x95\xBB\xE6\xA9\x01\x54\xE9\x1B\x33\x98\x5A\x2B\xCD\x42\x0A\xD5\xC6\x7E\xC9\xAD\x8E\xB7\xAC\x68\x64\xDB\x27\x2A\x51\x6B\xC9\x4C\x28\x39\xB0\xA8\x16\x9A\x6B\xF5\x8E\x1A\x0C\x2A\xDA\x8C\x88\x3B\x7B\xF4\x97\xA4\x91\x71\x26\x8E\xD1\x5D\xDD\x29\x69\x38\x4E\x7F\xF4\xBF\x4A\xAB\x2E\xC9\xEC\xC6\x52\x9C\xF6\x29\xE2\xDF\x0F\x08\xA7\x7A\x65\xAF\xA1\x2A\xA9\xB5\x05\xDF\x8B\x28\x7E\xF6\xCC\x91\x49\x3D\x1C\xAA\x39\x07\x6E\x28\xEF\x1E\xA0\x28\xF5\x11\x8D\xE6\x1A\xE0\x2B\xB6\xAE\xFC\x33\x43\xA0\x50\x29\x2F\x19\x9F\x40\x18\x57\xB2\xBE\xAD\x5E\x6E\xE2\xA1\xF1\x91\x02\x2F\x92\x78\x01\x6F\x04\x77\x91\xA9\xD1\x8D\xA7\xD2\xA6\xD2\x7F\x2E\x0E\x51\xC2\xF6\xEA\x30\xE8\xAC\x49\xA0\x60\x4F\x4C\x13\x54\x2E\x85\xB6\x83\x81\xB9\xFD\xCF\xA0\xCE\x4B\x2D\x34\x13\x54\x85\x2D\x36\x02\x45\xC5\x36\xB6\x12\xAF\x71\xF3\xE7\x7C\x90\x95\xAE\x2D\xBD\xE5\x04\xB2\x65\x73\x3D\xAB\xFE\x10\xA2\x0F\xC7\xD6\xD3\x2C\x21\xCC\xC7\x2B\x8B\x34\x44\xAE\x66\x3D\x65\x92\x2D\x17\xF8\x2C\xAA\x2B\x86\x5C\xD8\x89\x13\xD2\x91\xA6\x58\x99\x02\x6E\xA1\x32\x84\x39\x72\x3C\x19\x8C\x36\xB0\xC3\xC8\xD0\x85\xBF\xAF\x8A\x32\x0F\xDE\x33\x4B\x4A\x49\x19\xB4\x4C\x2B\x95\xF6\xE8\xEC\xF7\x33\x93\xF7\xF0\xD2\xA4\x0E\x60\xB1\xD4\x06\x52\x6B\x02\x2D\xDC\x33\x18\x10\xB1\xA5\xF7\xC3\x47\xBD\x53\xED\x1F\x10\x5D\x6A\x0D\x30\xAB\xA4\x77\xE1\x78\x88\x9A\xB2\xEC\x55\xD5\x58\xDE\xAB\x26\x30\x20\x43\x36\x96\x2B\x4D\xB5\xB6\x63\xB6\x90\x2B\x89\xE8\x5B\x31\xBC\x6A\xF5\x0F\xC5\x0A\xCC\xB3\xFB\x9B\x57\xB6\x63\x29\x70\x31\x37\x8D\xB4\x78\x96\xD7\xFB\xAF\x6C\x60\x0A\xDD\x2C\x67\xF9\x36\xDB\x03\x79\x86\xDB\x85\x6E\xB4\x9C\xF2\xDB\x3F\x7D\xA6\xD2\x36\x50\xE4\x38\xF1\x88\x40\x41\xB0\x13\x11\x9E\x4C\x2A\xE5\xAF\x37\xCC\xCD\xFB\x68\x66\x07\x38\xB5\x8B\x3C\x59\xD1\xC0\x24\x84\x37\x47\x2A\xBA\x1F\x35\xCA\x1F\xB9\x0C\xD7\x14\xAA\x9F\x63\x55\x34\xF4\x9E\x7C\x5B\xBA\x81\xC2\xB6\xB3\x6F\xDE\xE2\x1C\xA2\x7E\x34\x7F\x79\x3D\x2C\xE9\x44\xED\xB2\x3C\x8C\x9B\x91\x4B\xE1\x03\x35\xE3\x50\xFE\xB5\x07\x03\x94\xB7\xA4\xA1\x5C\x0C\xA1\x20\x28\x35\x68\xB7\xBF\xC2\x54\xFE\x83\x8B\x13\x7A\x21\x47\xCE\x7C\x11\x3A\x3A\x4D\x65\x49\x9D\x9E\x86\xB8\x7D\xBC\xC7\xF0\x3B\xBD\x3A\x3A\xB1\xAA\x24\x3E\xCE\x5B\xA9\xBC\xF2\x5F\x82\x83\x6C\xFE\x47\x3B\x2D\x83\xE7\xA7\x20\x1C\xD0\xB9\x6A\x72\x45\x1E\x86\x3F\x6C\x3B\xA6\x64\xA6\xD0\x73\xD1\xF7\xB5\xED\x99\x08\x65\xD9\x78\xBD\x38\x15\xD0\x60\x94\xFC\x9A\x2A\xBA\x52\x21\xC2\x2D\x5A\xB9\x96\x38\x9E\x37\x21\xE3\xAF\x5F\x05\xBE\xDD\xC2\x87\x5E\x0D\xFA\xEB\x39\x02\x1E\xE2\x7A\x41\x18\x7C\xBB\x45\xEF\x40\xC3\xE7\x3B\xC0\x39\x89\xF9\xA3\x0D\x12\xC5\x4B\xA7\xD2\x14\x1D\xA8\xA8\x75\x49\x3E\x65\x77\x6E\xF3\x5F\x97\xDE\xBC\x22\x86\xCC\x4A\xF9\xB4\x62\x3E\xEE\x90\x2F\x84\x0C\x52\xF1\xB8\xAD\x65\x89\x39\xAE\xF7\x1F\x3F\x72\xB9\xEC\x1D\xE2\x15\x88\xBD\x35\x48\x4E\xA4\x44\x36\x34\x3F\xF9\x5E\xAD\x6A\xB1\xD8\xAF\xB1\xB2\xA3\x03\xDF\x1B\x71\xE5\x3C\x4A\xEA\x6B\x2E\x3E\x93\x72\xBE\x0D\x1B\xC9\x97\x98\xB0\xCE\x3C\xC1\x0D\x2A\x59\x6D\x56\x5D\xBA\x82\xF8\x8C\xE4\xCF\xF3\xB3\x3D\x5D\x24\xE9\xC0\x83\x11\x24\xBF\x1A\xD5\x4B\x79\x25\x32\x98\x3D\xD6\xC3\xA8\xB7\xD0",
		0x179F2FA6
	},
};
/*
static void hex_print (uint8_t *buff, uint32_t len)
{
	uint32_t cnt = 0;
	for (cnt = 0; cnt < len; cnt++)  {
		if ((cnt % 16) == 0)  printf ("\n");
			printf ("%02x ", buff[cnt]);
	}
	printf ("\n");
}
*/


