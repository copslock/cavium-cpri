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

#include "cvmx.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <test-kasumi-api.h>
#include <openssl/f8-f9.h>
#include "test-crypto-common.h"

#ifdef TEST_CPU_CYCLES
	extern uint32_t numcores;
	extern CVMX_SHARED uint64_t total_cpucycles;
	extern CVMX_SHARED uint64_t total_mbps;		
#endif 

int kasumi () {
	int inlen, ret = 0;
	uint8_t encrypt[MAX_BUFF_SIZE];
	uint8_t decrypt[MAX_BUFF_SIZE];
	uint8_t plaintext[MAX_BUFF_SIZE] = "\xAD\x9C\x44\x1F\x89\x0B\x38\xC4\x57\xA4\x9D\x42\x14\x07\xE8";
	uint8_t key[16] = "\x5A\xCB\x1D\x64\x4C\x0D\x51\x20\x4E\xA5\xF1\x45\x10\x10\xD8\x52";
	uint32_t count = 0xFA556B26;
	uint8_t bearer = 0x03;
	uint8_t direction = 0x1;
	uint32_t fresh = 0xB8AEFDA9;
	uint32_t mac;
	uint32_t length;
	f8_ctx ctx1;
	f9_ctx ctx2;
 
	/* for performance tests perfomance taken for sizes ranging from 64
	 * bytes to 16 kB
	 */

	/* single call f8 */
	for (inlen = START_PACKET_SIZE; inlen <=MAX_BUFF_SIZE;) {	
		PRINT_HDR;
		length = inlen*8;
		memset (encrypt, 0, sizeof(encrypt));
		START_CYCLE;
		ret=f8_uea1 (count, bearer, direction, key, length, plaintext, encrypt);
		END_CYCLE("KASUMI ENCRYPTION OF");
		START_CYCLE;
		ret=f8_uea1 (count, bearer, direction, key, length, encrypt, decrypt);
		END_CYCLE("KASUMI DECRYPTION OF");
		if(memcmp (plaintext, decrypt, length/8)) {
			printf ("F8 perfomance test for single call Encrypt/Decrypt Failed\n");
			ret = -1;
			goto End;
		}	
		#ifdef TEST_CPU_CYCLES
			inlen+=inlen;
		#else	
			inlen+=INCR_STEPS;
		#endif
	}

	/* multi call f8 */
	for (inlen = START_PACKET_SIZE; inlen <=MAX_BUFF_SIZE;) {
		memset (encrypt, 0, sizeof(encrypt));
		memset (decrypt, 0, sizeof(decrypt));
		length = inlen*8;
		ctx1.uea_rev = UEA_REV_1;

		//Encrypt
		ret=f8_init (&ctx1, count, bearer, direction, key);
		if (ret != 0) {
			printf ("f8_init Failed\n");
			ret = -1;
			goto End;
		}
		ret=f8_enc (&ctx1, length, plaintext, encrypt);
		if (ret != 0) {
			printf ("f8_enc Encrypt Failed\n");
			ret = -1;
			goto End;
		}
		
		//Decrypt
		ret=f8_init (&ctx1, count, bearer, direction, key);
		if (ret != 0) {
			printf ("f8_init Failed\n");
			ret = -1;
			goto End;
		}
		ret=f8_enc (&ctx1, length, encrypt, decrypt);
		if (ret != 0) {
			printf ("f8_init Decrypt Failed\n");
			ret = -1;
			goto End;
		}

		if(memcmp (plaintext, decrypt, length/8)) {
			printf ("F8 perfomance test for multi call Encrypt/Decrypt Failed\n");
			ret = -1;
			goto End;
		}	
		#ifdef TEST_CPU_CYCLES
			inlen+=inlen;
		#else	
			inlen+=INCR_STEPS;
		#endif
	}

	/* single call f9 */
	for (inlen = START_PACKET_SIZE; inlen <=MAX_BUFF_SIZE;) {	
		PRINT_HDR;
		length = inlen*8;
		START_CYCLE;
		ret=f9_uia1 (count, fresh, direction, key, length, plaintext, &mac);
		END_CYCLE("KASUMI INTEGRITY TEST OF");	
		#ifdef TEST_CPU_CYCLES
			inlen+=inlen;
		#else	
			inlen+=INCR_STEPS;
		#endif
	}

	/* multi call f9 */
	for (inlen = START_PACKET_SIZE; inlen <=MAX_BUFF_SIZE;) {
		length = inlen*8;
		ctx2.uia_rev=UIA_REV_1;
		ret=f9_init (&ctx2, count, fresh, direction, key);
		if (ret != 0) {
			printf ("f9_init Failed\n");
			ret = -1;
			goto End;
		}
		ret=f9_update (&ctx2, length, plaintext);
		if (ret != 0) {
			printf ("f9_update Failed\n");
			ret = -1;
			goto End;
		}
		ret=f9_final (&ctx2, &mac);
		if (ret != 0) {
			printf ("f9_final Failed\n");
			ret = -1;
			goto End;
		}	
		#ifdef TEST_CPU_CYCLES
			inlen+=inlen;
		#else	
			inlen+=INCR_STEPS;
		#endif
	}
End:
	if (ret == -1 ) {
		printf("kasumi test Failed at Line:%d\n",__LINE__);
	} 
	if (cvmx_is_init_core()) {
		printf ("Tested %-20s : Packet Size from %d to %d : %s\n","Kasumi",
					START_PACKET_SIZE,MAX_BUFF_SIZE,(ret==0)?"Passed":"Failed");
	}

	return 0;
}

int test_kasumi_kat () 
{
	unsigned int i;
	int ret = 0,fail = 0;
	uint8_t encrypt[16*1024];
	uint8_t decrypt[16*1024];
	uint32_t mac;
	int cnt = 0;
	f8_ctx ctx1;

	/*kasumi tests for various vectors */
	/* single call f8 */

	for (i = 0; i < sizeof(f8_test)/sizeof(f8_test[0]); i++) {
		memset (encrypt, 0, sizeof(encrypt));
		f8_uea1 (f8_test[i].count, f8_test[i].bearer, f8_test[i].direction,
			f8_test[i].key, f8_test[i].length, f8_test[i].plaintext, encrypt);
		if (memcmp (f8_test[i].ciphertext, encrypt, f8_test[i].length/8)) {
			printf ("F8 Test vector #%d Encrypt Failed\n", (i+1));
			ret = -1;
			fail++;
			goto End;
		}
		f8_uea1 (f8_test[i].count, f8_test[i].bearer, f8_test[i].direction,
			f8_test[i].key, f8_test[i].length, encrypt, decrypt);
		if (memcmp (f8_test[i].plaintext, decrypt, f8_test[i].length/8)) {
			printf ("F8 Test vector #%d Encrypt/Decrypt Failed\n", (i+1));
			ret = -1;
			fail++;
			goto End;
		}
		cnt++;
	}

	/* Multi call f8 */
	for (i = 0; i < sizeof(f8_test)/sizeof(f8_test[0]); i++) {
		memset (encrypt, 0, sizeof(encrypt));
		memset (decrypt, 0, sizeof(decrypt));

		ctx1.uea_rev = UEA_REV_1;

		//Encrypt
		ret=f8_init(&ctx1, f8_test[i].count, f8_test[i].bearer, f8_test[i].direction, f8_test[i].key);
		if (ret != 0) {
			printf ("f8_init Failed\n");
			ret = -1;
			goto End;
		}
		ret=f8_enc(&ctx1, f8_test[i].length, f8_test[i].plaintext, encrypt);
		if (ret != 0) {
			printf ("f8_enc Encrypt Failed\n");
			ret = -1;
			goto End;
		}
		if (memcmp (f8_test[i].ciphertext, encrypt, f8_test[i].length/8)) {
			printf ("F8 Test vector #%d Encrypt Failed\n", (i+1));
			ret = -1;
			fail++;
			goto End;
		}

		//Decrypt
		ret=f8_init(&ctx1, f8_test[i].count, f8_test[i].bearer, f8_test[i].direction, f8_test[i].key);
		if (ret != 0) {
			printf ("f8_init Failed\n");
			ret = -1;
			goto End;
		}
		ret=f8_enc(&ctx1, f8_test[i].length, encrypt, decrypt);
		if (ret != 0) {
			printf ("f8_enc Decrypt Failed\n");
			ret = -1;
			goto End;
		}
		if (memcmp (f8_test[i].plaintext, decrypt, (f8_test[i].length/8))) {
			printf ("F8 Test vector #%d Encrypt/Decrypt Failed\n", (i+1));
			ret = -1;
			fail++;
			goto End;
		}
		cnt++;
	}

	/* single call f9 */
	
	for (i = 0; i < sizeof(f9_test)/sizeof(f9_test[0]); i++) {
		f9_uia1 (f9_test[i].count, f9_test[i].fresh, f9_test[i].direction, 
		    f9_test[i].key, f9_test[i].length, f9_test[i].message, &mac);
		if (mac != f9_test[i].expected_mac)  {
			printf ("F9 Test vector # %d Failed\n", (i+1));
			ret = -1;
			fail++;
			goto End;
		}
		cnt++;
	}

	/* Multi call f9 */
	for (i = 0; i < sizeof(f9_test)/sizeof(f9_test[0]); i++) {
		f9_ctx ctx2;
		ctx2.uia_rev = UIA_REV_1;

		ret=f9_init (&ctx2, f9_test[i].count, f9_test[i].fresh, f9_test[i].direction, f9_test[i].key);
		if (ret != 0) { 
			printf ("f9_init Failed\n");
			ret = -1;
			goto End;
		}
		ret=f9_update (&ctx2, f9_test[i].length, f9_test[i].message);
		if (ret != 0) {
			printf ("f9_update Failed\n");
			ret = -1;
			goto End;
		}
		ret=f9_final (&ctx2, &mac);
		if (ret != 0) {
			printf ("f9_final Failed\n");
			ret = -1;
			goto End;
		}
		
		if (mac != f9_test[i].expected_mac)  {
			printf ("F9 Test vector # %d Failed\n", (i+1));
			ret = -1;
			fail++;
			goto End;
		}
		cnt++;
	}

End:
	if (ret == -1 ) {
		printf("kasumi test Failed at Line:%d\n",__LINE__);
	} 
	else { 
	if (fail)
		printf("***");
		if (cvmx_is_init_core()) 
			printf ("%-20s :Total Test vectors tested:  %d passed : %d failed : %d\n","Kasumi",cnt,(cnt-fail),fail);
	}
	return ret;
}

