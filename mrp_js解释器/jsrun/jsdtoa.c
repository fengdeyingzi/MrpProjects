/* Locale-independent implementations of string <-> double conversions. */

#include "jsi.h"
#include <mrc_base.h>
#include <math.h>

#if defined(_MSC_VER) && (_MSC_VER < 1700) /* VS2012 has stdint.h */
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

#include <errno.h>
#include <assert.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*
 * format exponent like sprintf(p, "e%+d", e)
 */
void
js_fmtexp(char *p, int e)
{
	char se[9];
	int i;

	*p++ = 'e';
	if(e < 0) {
		*p++ = '-';
		e = -e;
	} else
		*p++ = '+';
	i = 0;
	while(e) {
		se[i++] = e % 10 + '0';
		e /= 10;
	}
	while(i < 1)
		se[i++] = '0';
	while(i > 0)
		*p++ = se[--i];
	*p++ = '\0';
}

/*
 * grisu2_59_56.c
 *
 * Grisu prints the optimal decimal representation of floating-point numbers.
 *
 * Copyright (c) 2009 Florian Loitsch
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

typedef struct diy_fp_t {
	uint64_t f;
	int e;
} diy_fp_t;

#define DIY_SIGNIFICAND_SIZE 64
#define D_1_LOG2_10 0.30102999566398114 /* 1 / lg(10) */

static const uint64_t powers_ten[] = {
	0xbf29dcaba82fdeaeull, 0xeef453d6923bd65aull, 0x9558b4661b6565f8ull,
	0xbaaee17fa23ebf76ull, 0xe95a99df8ace6f54ull, 0x91d8a02bb6c10594ull,
	0xb64ec836a47146faull, 0xe3e27a444d8d98b8ull, 0x8e6d8c6ab0787f73ull,
	0xb208ef855c969f50ull, 0xde8b2b66b3bc4724ull, 0x8b16fb203055ac76ull,
	0xaddcb9e83c6b1794ull, 0xd953e8624b85dd79ull, 0x87d4713d6f33aa6cull,
	0xa9c98d8ccb009506ull, 0xd43bf0effdc0ba48ull, 0x84a57695fe98746dull,
	0xa5ced43b7e3e9188ull, 0xcf42894a5dce35eaull, 0x818995ce7aa0e1b2ull,
	0xa1ebfb4219491a1full, 0xca66fa129f9b60a7ull, 0xfd00b897478238d1ull,
	0x9e20735e8cb16382ull, 0xc5a890362fddbc63ull, 0xf712b443bbd52b7cull,
	0x9a6bb0aa55653b2dull, 0xc1069cd4eabe89f9ull, 0xf148440a256e2c77ull,
	0x96cd2a865764dbcaull, 0xbc807527ed3e12bdull, 0xeba09271e88d976cull,
	0x93445b8731587ea3ull, 0xb8157268fdae9e4cull, 0xe61acf033d1a45dfull,
	0x8fd0c16206306bacull, 0xb3c4f1ba87bc8697ull, 0xe0b62e2929aba83cull,
	0x8c71dcd9ba0b4926ull, 0xaf8e5410288e1b6full, 0xdb71e91432b1a24bull,
	0x892731ac9faf056full, 0xab70fe17c79ac6caull, 0xd64d3d9db981787dull,
	0x85f0468293f0eb4eull, 0xa76c582338ed2622ull, 0xd1476e2c07286faaull,
	0x82cca4db847945caull, 0xa37fce126597973dull, 0xcc5fc196fefd7d0cull,
	0xff77b1fcbebcdc4full, 0x9faacf3df73609b1ull, 0xc795830d75038c1eull,
	0xf97ae3d0d2446f25ull, 0x9becce62836ac577ull, 0xc2e801fb244576d5ull,
	0xf3a20279ed56d48aull, 0x9845418c345644d7ull, 0xbe5691ef416bd60cull,
	0xedec366b11c6cb8full, 0x94b3a202eb1c3f39ull, 0xb9e08a83a5e34f08ull,
	0xe858ad248f5c22caull, 0x91376c36d99995beull, 0xb58547448ffffb2eull,
	0xe2e69915b3fff9f9ull, 0x8dd01fad907ffc3cull, 0xb1442798f49ffb4bull,
	0xdd95317f31c7fa1dull, 0x8a7d3eef7f1cfc52ull, 0xad1c8eab5ee43b67ull,
	0xd863b256369d4a41ull, 0x873e4f75e2224e68ull, 0xa90de3535aaae202ull,
	0xd3515c2831559a83ull, 0x8412d9991ed58092ull, 0xa5178fff668ae0b6ull,
	0xce5d73ff402d98e4ull, 0x80fa687f881c7f8eull, 0xa139029f6a239f72ull,
	0xc987434744ac874full, 0xfbe9141915d7a922ull, 0x9d71ac8fada6c9b5ull,
	0xc4ce17b399107c23ull, 0xf6019da07f549b2bull, 0x99c102844f94e0fbull,
	0xc0314325637a193aull, 0xf03d93eebc589f88ull, 0x96267c7535b763b5ull,
	0xbbb01b9283253ca3ull, 0xea9c227723ee8bcbull, 0x92a1958a7675175full,
	0xb749faed14125d37ull, 0xe51c79a85916f485ull, 0x8f31cc0937ae58d3ull,
	0xb2fe3f0b8599ef08ull, 0xdfbdcece67006ac9ull, 0x8bd6a141006042beull,
	0xaecc49914078536dull, 0xda7f5bf590966849ull, 0x888f99797a5e012dull,
	0xaab37fd7d8f58179ull, 0xd5605fcdcf32e1d7ull, 0x855c3be0a17fcd26ull,
	0xa6b34ad8c9dfc070ull, 0xd0601d8efc57b08cull, 0x823c12795db6ce57ull,
	0xa2cb1717b52481edull, 0xcb7ddcdda26da269ull, 0xfe5d54150b090b03ull,
	0x9efa548d26e5a6e2ull, 0xc6b8e9b0709f109aull, 0xf867241c8cc6d4c1ull,
	0x9b407691d7fc44f8ull, 0xc21094364dfb5637ull, 0xf294b943e17a2bc4ull,
	0x979cf3ca6cec5b5bull, 0xbd8430bd08277231ull, 0xece53cec4a314ebeull,
	0x940f4613ae5ed137ull, 0xb913179899f68584ull, 0xe757dd7ec07426e5ull,
	0x9096ea6f3848984full, 0xb4bca50b065abe63ull, 0xe1ebce4dc7f16dfcull,
	0x8d3360f09cf6e4bdull, 0xb080392cc4349dedull, 0xdca04777f541c568ull,
	0x89e42caaf9491b61ull, 0xac5d37d5b79b6239ull, 0xd77485cb25823ac7ull,
	0x86a8d39ef77164bdull, 0xa8530886b54dbdecull, 0xd267caa862a12d67ull,
	0x8380dea93da4bc60ull, 0xa46116538d0deb78ull, 0xcd795be870516656ull,
	0x806bd9714632dff6ull, 0xa086cfcd97bf97f4ull, 0xc8a883c0fdaf7df0ull,
	0xfad2a4b13d1b5d6cull, 0x9cc3a6eec6311a64ull, 0xc3f490aa77bd60fdull,
	0xf4f1b4d515acb93cull, 0x991711052d8bf3c5ull, 0xbf5cd54678eef0b7ull,
	0xef340a98172aace5ull, 0x9580869f0e7aac0full, 0xbae0a846d2195713ull,
	0xe998d258869facd7ull, 0x91ff83775423cc06ull, 0xb67f6455292cbf08ull,
	0xe41f3d6a7377eecaull, 0x8e938662882af53eull, 0xb23867fb2a35b28eull,
	0xdec681f9f4c31f31ull, 0x8b3c113c38f9f37full, 0xae0b158b4738705full,
	0xd98ddaee19068c76ull, 0x87f8a8d4cfa417caull, 0xa9f6d30a038d1dbcull,
	0xd47487cc8470652bull, 0x84c8d4dfd2c63f3bull, 0xa5fb0a17c777cf0aull,
	0xcf79cc9db955c2ccull, 0x81ac1fe293d599c0ull, 0xa21727db38cb0030ull,
	0xca9cf1d206fdc03cull, 0xfd442e4688bd304bull, 0x9e4a9cec15763e2full,
	0xc5dd44271ad3cdbaull, 0xf7549530e188c129ull, 0x9a94dd3e8cf578baull,
	0xc13a148e3032d6e8ull, 0xf18899b1bc3f8ca2ull, 0x96f5600f15a7b7e5ull,
	0xbcb2b812db11a5deull, 0xebdf661791d60f56ull, 0x936b9fcebb25c996ull,
	0xb84687c269ef3bfbull, 0xe65829b3046b0afaull, 0x8ff71a0fe2c2e6dcull,
	0xb3f4e093db73a093ull, 0xe0f218b8d25088b8ull, 0x8c974f7383725573ull,
	0xafbd2350644eead0ull, 0xdbac6c247d62a584ull, 0x894bc396ce5da772ull,
	0xab9eb47c81f5114full, 0xd686619ba27255a3ull, 0x8613fd0145877586ull,
	0xa798fc4196e952e7ull, 0xd17f3b51fca3a7a1ull, 0x82ef85133de648c5ull,
	0xa3ab66580d5fdaf6ull, 0xcc963fee10b7d1b3ull, 0xffbbcfe994e5c620ull,
	0x9fd561f1fd0f9bd4ull, 0xc7caba6e7c5382c9ull, 0xf9bd690a1b68637bull,
	0x9c1661a651213e2dull, 0xc31bfa0fe5698db8ull, 0xf3e2f893dec3f126ull,
	0x986ddb5c6b3a76b8ull, 0xbe89523386091466ull, 0xee2ba6c0678b597full,
	0x94db483840b717f0ull, 0xba121a4650e4ddecull, 0xe896a0d7e51e1566ull,
	0x915e2486ef32cd60ull, 0xb5b5ada8aaff80b8ull, 0xe3231912d5bf60e6ull,
	0x8df5efabc5979c90ull, 0xb1736b96b6fd83b4ull, 0xddd0467c64bce4a1ull,
	0x8aa22c0dbef60ee4ull, 0xad4ab7112eb3929eull, 0xd89d64d57a607745ull,
	0x87625f056c7c4a8bull, 0xa93af6c6c79b5d2eull, 0xd389b47879823479ull,
	0x843610cb4bf160ccull, 0xa54394fe1eedb8ffull, 0xce947a3da6a9273eull,
	0x811ccc668829b887ull, 0xa163ff802a3426a9ull, 0xc9bcff6034c13053ull,
	0xfc2c3f3841f17c68ull, 0x9d9ba7832936edc1ull, 0xc5029163f384a931ull,
	0xf64335bcf065d37dull, 0x99ea0196163fa42eull, 0xc06481fb9bcf8d3aull,
	0xf07da27a82c37088ull, 0x964e858c91ba2655ull, 0xbbe226efb628afebull,
	0xeadab0aba3b2dbe5ull, 0x92c8ae6b464fc96full, 0xb77ada0617e3bbcbull,
	0xe55990879ddcaabeull, 0x8f57fa54c2a9eab7ull, 0xb32df8e9f3546564ull,
	0xdff9772470297ebdull, 0x8bfbea76c619ef36ull, 0xaefae51477a06b04ull,
	0xdab99e59958885c5ull, 0x88b402f7fd75539bull, 0xaae103b5fcd2a882ull,
	0xd59944a37c0752a2ull, 0x857fcae62d8493a5ull, 0xa6dfbd9fb8e5b88full,
	0xd097ad07a71f26b2ull, 0x825ecc24c8737830ull, 0xa2f67f2dfa90563bull,
	0xcbb41ef979346bcaull, 0xfea126b7d78186bdull, 0x9f24b832e6b0f436ull,
	0xc6ede63fa05d3144ull, 0xf8a95fcf88747d94ull, 0x9b69dbe1b548ce7dull,
	0xc24452da229b021cull, 0xf2d56790ab41c2a3ull, 0x97c560ba6b0919a6ull,
	0xbdb6b8e905cb600full, 0xed246723473e3813ull, 0x9436c0760c86e30cull,
	0xb94470938fa89bcfull, 0xe7958cb87392c2c3ull, 0x90bd77f3483bb9baull,
	0xb4ecd5f01a4aa828ull, 0xe2280b6c20dd5232ull, 0x8d590723948a535full,
	0xb0af48ec79ace837ull, 0xdcdb1b2798182245ull, 0x8a08f0f8bf0f156bull,
	0xac8b2d36eed2dac6ull, 0xd7adf884aa879177ull, 0x86ccbb52ea94baebull,
	0xa87fea27a539e9a5ull, 0xd29fe4b18e88640full, 0x83a3eeeef9153e89ull,
	0xa48ceaaab75a8e2bull, 0xcdb02555653131b6ull, 0x808e17555f3ebf12ull,
	0xa0b19d2ab70e6ed6ull, 0xc8de047564d20a8cull, 0xfb158592be068d2full,
	0x9ced737bb6c4183dull, 0xc428d05aa4751e4dull, 0xf53304714d9265e0ull,
	0x993fe2c6d07b7facull, 0xbf8fdb78849a5f97ull, 0xef73d256a5c0f77dull,
	0x95a8637627989aaeull, 0xbb127c53b17ec159ull, 0xe9d71b689dde71b0ull,
	0x9226712162ab070eull, 0xb6b00d69bb55c8d1ull, 0xe45c10c42a2b3b06ull,
	0x8eb98a7a9a5b04e3ull, 0xb267ed1940f1c61cull, 0xdf01e85f912e37a3ull,
	0x8b61313bbabce2c6ull, 0xae397d8aa96c1b78ull, 0xd9c7dced53c72256ull,
	0x881cea14545c7575ull, 0xaa242499697392d3ull, 0xd4ad2dbfc3d07788ull,
	0x84ec3c97da624ab5ull, 0xa6274bbdd0fadd62ull, 0xcfb11ead453994baull,
	0x81ceb32c4b43fcf5ull, 0xa2425ff75e14fc32ull, 0xcad2f7f5359a3b3eull,
	0xfd87b5f28300ca0eull, 0x9e74d1b791e07e48ull, 0xc612062576589ddbull,
	0xf79687aed3eec551ull, 0x9abe14cd44753b53ull, 0xc16d9a0095928a27ull,
	0xf1c90080baf72cb1ull, 0x971da05074da7befull, 0xbce5086492111aebull,
	0xec1e4a7db69561a5ull, 0x9392ee8e921d5d07ull, 0xb877aa3236a4b449ull,
	0xe69594bec44de15bull, 0x901d7cf73ab0acd9ull, 0xb424dc35095cd80full,
	0xe12e13424bb40e13ull, 0x8cbccc096f5088ccull, 0xafebff0bcb24aaffull,
	0xdbe6fecebdedd5bfull, 0x89705f4136b4a597ull, 0xabcc77118461cefdull,
	0xd6bf94d5e57a42bcull, 0x8637bd05af6c69b6ull, 0xa7c5ac471b478423ull,
	0xd1b71758e219652cull, 0x83126e978d4fdf3bull, 0xa3d70a3d70a3d70aull,
	0xcccccccccccccccdull, 0x8000000000000000ull, 0xa000000000000000ull,
	0xc800000000000000ull, 0xfa00000000000000ull, 0x9c40000000000000ull,
	0xc350000000000000ull, 0xf424000000000000ull, 0x9896800000000000ull,
	0xbebc200000000000ull, 0xee6b280000000000ull, 0x9502f90000000000ull,
	0xba43b74000000000ull, 0xe8d4a51000000000ull, 0x9184e72a00000000ull,
	0xb5e620f480000000ull, 0xe35fa931a0000000ull, 0x8e1bc9bf04000000ull,
	0xb1a2bc2ec5000000ull, 0xde0b6b3a76400000ull, 0x8ac7230489e80000ull,
	0xad78ebc5ac620000ull, 0xd8d726b7177a8000ull, 0x878678326eac9000ull,
	0xa968163f0a57b400ull, 0xd3c21bcecceda100ull, 0x84595161401484a0ull,
	0xa56fa5b99019a5c8ull, 0xcecb8f27f4200f3aull, 0x813f3978f8940984ull,
	0xa18f07d736b90be5ull, 0xc9f2c9cd04674edfull, 0xfc6f7c4045812296ull,
	0x9dc5ada82b70b59eull, 0xc5371912364ce305ull, 0xf684df56c3e01bc7ull,
	0x9a130b963a6c115cull, 0xc097ce7bc90715b3ull, 0xf0bdc21abb48db20ull,
	0x96769950b50d88f4ull, 0xbc143fa4e250eb31ull, 0xeb194f8e1ae525fdull,
	0x92efd1b8d0cf37beull, 0xb7abc627050305aeull, 0xe596b7b0c643c719ull,
	0x8f7e32ce7bea5c70ull, 0xb35dbf821ae4f38cull, 0xe0352f62a19e306full,
	0x8c213d9da502de45ull, 0xaf298d050e4395d7ull, 0xdaf3f04651d47b4cull,
	0x88d8762bf324cd10ull, 0xab0e93b6efee0054ull, 0xd5d238a4abe98068ull,
	0x85a36366eb71f041ull, 0xa70c3c40a64e6c52ull, 0xd0cf4b50cfe20766ull,
	0x82818f1281ed44a0ull, 0xa321f2d7226895c8ull, 0xcbea6f8ceb02bb3aull,
	0xfee50b7025c36a08ull, 0x9f4f2726179a2245ull, 0xc722f0ef9d80aad6ull,
	0xf8ebad2b84e0d58cull, 0x9b934c3b330c8577ull, 0xc2781f49ffcfa6d5ull,
	0xf316271c7fc3908bull, 0x97edd871cfda3a57ull, 0xbde94e8e43d0c8ecull,
	0xed63a231d4c4fb27ull, 0x945e455f24fb1cf9ull, 0xb975d6b6ee39e437ull,
	0xe7d34c64a9c85d44ull, 0x90e40fbeea1d3a4bull, 0xb51d13aea4a488ddull,
	0xe264589a4dcdab15ull, 0x8d7eb76070a08aedull, 0xb0de65388cc8ada8ull,
	0xdd15fe86affad912ull, 0x8a2dbf142dfcc7abull, 0xacb92ed9397bf996ull,
	0xd7e77a8f87daf7fcull, 0x86f0ac99b4e8dafdull, 0xa8acd7c0222311bdull,
	0xd2d80db02aabd62cull, 0x83c7088e1aab65dbull, 0xa4b8cab1a1563f52ull,
	0xcde6fd5e09abcf27ull, 0x80b05e5ac60b6178ull, 0xa0dc75f1778e39d6ull,
	0xc913936dd571c84cull, 0xfb5878494ace3a5full, 0x9d174b2dcec0e47bull,
	0xc45d1df942711d9aull, 0xf5746577930d6501ull, 0x9968bf6abbe85f20ull,
	0xbfc2ef456ae276e9ull, 0xefb3ab16c59b14a3ull, 0x95d04aee3b80ece6ull,
	0xbb445da9ca61281full, 0xea1575143cf97227ull, 0x924d692ca61be758ull,
	0xb6e0c377cfa2e12eull, 0xe498f455c38b997aull, 0x8edf98b59a373fecull,
	0xb2977ee300c50fe7ull, 0xdf3d5e9bc0f653e1ull, 0x8b865b215899f46dull,
	0xae67f1e9aec07188ull, 0xda01ee641a708deaull, 0x884134fe908658b2ull,
	0xaa51823e34a7eedfull, 0xd4e5e2cdc1d1ea96ull, 0x850fadc09923329eull,
	0xa6539930bf6bff46ull, 0xcfe87f7cef46ff17ull, 0x81f14fae158c5f6eull,
	0xa26da3999aef774aull, 0xcb090c8001ab551cull, 0xfdcb4fa002162a63ull,
	0x9e9f11c4014dda7eull, 0xc646d63501a1511eull, 0xf7d88bc24209a565ull,
	0x9ae757596946075full, 0xc1a12d2fc3978937ull, 0xf209787bb47d6b85ull,
	0x9745eb4d50ce6333ull, 0xbd176620a501fc00ull, 0xec5d3fa8ce427b00ull,
	0x93ba47c980e98ce0ull, 0xb8a8d9bbe123f018ull, 0xe6d3102ad96cec1eull,
	0x9043ea1ac7e41393ull, 0xb454e4a179dd1877ull, 0xe16a1dc9d8545e95ull,
	0x8ce2529e2734bb1dull, 0xb01ae745b101e9e4ull, 0xdc21a1171d42645dull,
	0x899504ae72497ebaull, 0xabfa45da0edbde69ull, 0xd6f8d7509292d603ull,
	0x865b86925b9bc5c2ull, 0xa7f26836f282b733ull, 0xd1ef0244af2364ffull,
	0x8335616aed761f1full, 0xa402b9c5a8d3a6e7ull, 0xcd036837130890a1ull,
	0x802221226be55a65ull, 0xa02aa96b06deb0feull, 0xc83553c5c8965d3dull,
	0xfa42a8b73abbf48dull, 0x9c69a97284b578d8ull, 0xc38413cf25e2d70eull,
	0xf46518c2ef5b8cd1ull, 0x98bf2f79d5993803ull, 0xbeeefb584aff8604ull,
	0xeeaaba2e5dbf6785ull, 0x952ab45cfa97a0b3ull, 0xba756174393d88e0ull,
	0xe912b9d1478ceb17ull, 0x91abb422ccb812efull, 0xb616a12b7fe617aaull,
	0xe39c49765fdf9d95ull, 0x8e41ade9fbebc27dull, 0xb1d219647ae6b31cull,
	0xde469fbd99a05fe3ull, 0x8aec23d680043beeull, 0xada72ccc20054aeaull,
	0xd910f7ff28069da4ull, 0x87aa9aff79042287ull, 0xa99541bf57452b28ull,
	0xd3fa922f2d1675f2ull, 0x847c9b5d7c2e09b7ull, 0xa59bc234db398c25ull,
	0xcf02b2c21207ef2full, 0x8161afb94b44f57dull, 0xa1ba1ba79e1632dcull,
	0xca28a291859bbf93ull, 0xfcb2cb35e702af78ull, 0x9defbf01b061adabull,
	0xc56baec21c7a1916ull, 0xf6c69a72a3989f5cull, 0x9a3c2087a63f6399ull,
	0xc0cb28a98fcf3c80ull, 0xf0fdf2d3f3c30b9full, 0x969eb7c47859e744ull,
	0xbc4665b596706115ull, 0xeb57ff22fc0c795aull, 0x9316ff75dd87cbd8ull,
	0xb7dcbf5354e9beceull, 0xe5d3ef282a242e82ull, 0x8fa475791a569d11ull,
	0xb38d92d760ec4455ull, 0xe070f78d3927556bull, 0x8c469ab843b89563ull,
	0xaf58416654a6babbull, 0xdb2e51bfe9d0696aull, 0x88fcf317f22241e2ull,
	0xab3c2fddeeaad25bull, 0xd60b3bd56a5586f2ull, 0x85c7056562757457ull,
	0xa738c6bebb12d16dull, 0xd106f86e69d785c8ull, 0x82a45b450226b39dull,
	0xa34d721642b06084ull, 0xcc20ce9bd35c78a5ull, 0xff290242c83396ceull,
	0x9f79a169bd203e41ull, 0xc75809c42c684dd1ull, 0xf92e0c3537826146ull,
	0x9bbcc7a142b17cccull, 0xc2abf989935ddbfeull, 0xf356f7ebf83552feull,
	0x98165af37b2153dfull, 0xbe1bf1b059e9a8d6ull, 0xeda2ee1c7064130cull,
	0x9485d4d1c63e8be8ull, 0xb9a74a0637ce2ee1ull, 0xe8111c87c5c1ba9aull,
	0x910ab1d4db9914a0ull, 0xb54d5e4a127f59c8ull, 0xe2a0b5dc971f303aull,
	0x8da471a9de737e24ull, 0xb10d8e1456105dadull, 0xdd50f1996b947519ull,
	0x8a5296ffe33cc930ull, 0xace73cbfdc0bfb7bull, 0xd8210befd30efa5aull,
	0x8714a775e3e95c78ull, 0xa8d9d1535ce3b396ull, 0xd31045a8341ca07cull,
	0x83ea2b892091e44eull, 0xa4e4b66b68b65d61ull, 0xce1de40642e3f4b9ull,
	0x80d2ae83e9ce78f4ull, 0xa1075a24e4421731ull, 0xc94930ae1d529cfdull,
	0xfb9b7cd9a4a7443cull, 0x9d412e0806e88aa6ull, 0xc491798a08a2ad4full,
	0xf5b5d7ec8acb58a3ull, 0x9991a6f3d6bf1766ull, 0xbff610b0cc6edd3full,
	0xeff394dcff8a948full, 0x95f83d0a1fb69cd9ull, 0xbb764c4ca7a44410ull,
	0xea53df5fd18d5514ull, 0x92746b9be2f8552cull, 0xb7118682dbb66a77ull,
	0xe4d5e82392a40515ull, 0x8f05b1163ba6832dull, 0xb2c71d5bca9023f8ull,
	0xdf78e4b2bd342cf7ull, 0x8bab8eefb6409c1aull, 0xae9672aba3d0c321ull,
	0xda3c0f568cc4f3e9ull, 0x8865899617fb1871ull, 0xaa7eebfb9df9de8eull,
	0xd51ea6fa85785631ull, 0x8533285c936b35dfull, 0xa67ff273b8460357ull,
	0xd01fef10a657842cull, 0x8213f56a67f6b29cull, 0xa298f2c501f45f43ull,
	0xcb3f2f7642717713ull, 0xfe0efb53d30dd4d8ull, 0x9ec95d1463e8a507ull,
	0xc67bb4597ce2ce49ull, 0xf81aa16fdc1b81dbull, 0x9b10a4e5e9913129ull,
	0xc1d4ce1f63f57d73ull, 0xf24a01a73cf2dcd0ull, 0x976e41088617ca02ull,
	0xbd49d14aa79dbc82ull, 0xec9c459d51852ba3ull, 0x93e1ab8252f33b46ull,
	0xb8da1662e7b00a17ull, 0xe7109bfba19c0c9dull, 0x906a617d450187e2ull,
	0xb484f9dc9641e9dbull, 0xe1a63853bbd26451ull, 0x8d07e33455637eb3ull,
	0xb049dc016abc5e60ull, 0xdc5c5301c56b75f7ull, 0x89b9b3e11b6329bbull,
	0xac2820d9623bf429ull, 0xd732290fbacaf134ull, 0x867f59a9d4bed6c0ull,
	0xa81f301449ee8c70ull, 0xd226fc195c6a2f8cull, 0x83585d8fd9c25db8ull,
	0xa42e74f3d032f526ull, 0xcd3a1230c43fb26full, 0x80444b5e7aa7cf85ull,
	0xa0555e361951c367ull, 0xc86ab5c39fa63441ull, 0xfa856334878fc151ull,
	0x9c935e00d4b9d8d2ull, 0xc3b8358109e84f07ull, 0xf4a642e14c6262c9ull,
	0x98e7e9cccfbd7dbeull, 0xbf21e44003acdd2dull, 0xeeea5d5004981478ull,
	0x95527a5202df0ccbull, 0xbaa718e68396cffeull, 0xe950df20247c83fdull,
	0x91d28b7416cdd27eull, 0xb6472e511c81471eull, 0xe3d8f9e563a198e5ull,
	0x8e679c2f5e44ff8full, 0xb201833b35d63f73ull, 0xde81e40a034bcf50ull,
	0x8b112e86420f6192ull, 0xadd57a27d29339f6ull, 0xd94ad8b1c7380874ull,
	0x87cec76f1c830549ull, 0xa9c2794ae3a3c69bull, 0xd433179d9c8cb841ull,
	0x849feec281d7f329ull, 0xa5c7ea73224deff3ull, 0xcf39e50feae16bf0ull,
	0x81842f29f2cce376ull, 0xa1e53af46f801c53ull, 0xca5e89b18b602368ull,
	0xfcf62c1dee382c42ull, 0x9e19db92b4e31ba9ull, 0xc5a05277621be294ull,
	0xf70867153aa2db39ull, 0x9a65406d44a5c903ull, 0xc0fe908895cf3b44ull,
	0xf13e34aabb430a15ull, 0x96c6e0eab509e64dull, 0xbc789925624c5fe1ull,
	0xeb96bf6ebadf77d9ull, 0x933e37a534cbaae8ull, 0xb80dc58e81fe95a1ull,
	0xe61136f2227e3b0aull, 0x8fcac257558ee4e6ull, 0xb3bd72ed2af29e20ull,
	0xe0accfa875af45a8ull, 0x8c6c01c9498d8b89ull, 0xaf87023b9bf0ee6bull,
	0xdb68c2ca82ed2a06ull, 0x892179be91d43a44ull, 0xab69d82e364948d4
};

static const int powers_ten_e[] = {
	-1203, -1200, -1196, -1193, -1190, -1186, -1183, -1180, -1176, -1173,
	-1170, -1166, -1163, -1160, -1156, -1153, -1150, -1146, -1143, -1140,
	-1136, -1133, -1130, -1127, -1123, -1120, -1117, -1113, -1110, -1107,
	-1103, -1100, -1097, -1093, -1090, -1087, -1083, -1080, -1077, -1073,
	-1070, -1067, -1063, -1060, -1057, -1053, -1050, -1047, -1043, -1040,
	-1037, -1034, -1030, -1027, -1024, -1020, -1017, -1014, -1010, -1007,
	-1004, -1000, -997, -994, -990, -987, -984, -980, -977, -974, -970,
	-967, -964, -960, -957, -954, -950, -947, -944, -940, -937, -934, -931,
	-927, -924, -921, -917, -914, -911, -907, -904, -901, -897, -894, -891,
	-887, -884, -881, -877, -874, -871, -867, -864, -861, -857, -854, -851,
	-847, -844, -841, -838, -834, -831, -828, -824, -821, -818, -814, -811,
	-808, -804, -801, -798, -794, -791, -788, -784, -781, -778, -774, -771,
	-768, -764, -761, -758, -754, -751, -748, -744, -741, -738, -735, -731,
	-728, -725, -721, -718, -715, -711, -708, -705, -701, -698, -695, -691,
	-688, -685, -681, -678, -675, -671, -668, -665, -661, -658, -655, -651,
	-648, -645, -642, -638, -635, -632, -628, -625, -622, -618, -615, -612,
	-608, -605, -602, -598, -595, -592, -588, -585, -582, -578, -575, -572,
	-568, -565, -562, -558, -555, -552, -549, -545, -542, -539, -535, -532,
	-529, -525, -522, -519, -515, -512, -509, -505, -502, -499, -495, -492,
	-489, -485, -482, -479, -475, -472, -469, -465, -462, -459, -455, -452,
	-449, -446, -442, -439, -436, -432, -429, -426, -422, -419, -416, -412,
	-409, -406, -402, -399, -396, -392, -389, -386, -382, -379, -376, -372,
	-369, -366, -362, -359, -356, -353, -349, -346, -343, -339, -336, -333,
	-329, -326, -323, -319, -316, -313, -309, -306, -303, -299, -296, -293,
	-289, -286, -283, -279, -276, -273, -269, -266, -263, -259, -256, -253,
	-250, -246, -243, -240, -236, -233, -230, -226, -223, -220, -216, -213,
	-210, -206, -203, -200, -196, -193, -190, -186, -183, -180, -176, -173,
	-170, -166, -163, -160, -157, -153, -150, -147, -143, -140, -137, -133,
	-130, -127, -123, -120, -117, -113, -110, -107, -103, -100, -97, -93,
	-90, -87, -83, -80, -77, -73, -70, -67, -63, -60, -57, -54, -50, -47,
	-44, -40, -37, -34, -30, -27, -24, -20, -17, -14, -10, -7, -4, 0, 3, 6,
	10, 13, 16, 20, 23, 26, 30, 33, 36, 39, 43, 46, 49, 53, 56, 59, 63, 66,
	69, 73, 76, 79, 83, 86, 89, 93, 96, 99, 103, 106, 109, 113, 116, 119,
	123, 126, 129, 132, 136, 139, 142, 146, 149, 152, 156, 159, 162, 166,
	169, 172, 176, 179, 182, 186, 189, 192, 196, 199, 202, 206, 209, 212,
	216, 219, 222, 226, 229, 232, 235, 239, 242, 245, 249, 252, 255, 259,
	262, 265, 269, 272, 275, 279, 282, 285, 289, 292, 295, 299, 302, 305,
	309, 312, 315, 319, 322, 325, 328, 332, 335, 338, 342, 345, 348, 352,
	355, 358, 362, 365, 368, 372, 375, 378, 382, 385, 388, 392, 395, 398,
	402, 405, 408, 412, 415, 418, 422, 425, 428, 431, 435, 438, 441, 445,
	448, 451, 455, 458, 461, 465, 468, 471, 475, 478, 481, 485, 488, 491,
	495, 498, 501, 505, 508, 511, 515, 518, 521, 524, 528, 531, 534, 538,
	541, 544, 548, 551, 554, 558, 561, 564, 568, 571, 574, 578, 581, 584,
	588, 591, 594, 598, 601, 604, 608, 611, 614, 617, 621, 624, 627, 631,
	634, 637, 641, 644, 647, 651, 654, 657, 661, 664, 667, 671, 674, 677,
	681, 684, 687, 691, 694, 697, 701, 704, 707, 711, 714, 717, 720, 724,
	727, 730, 734, 737, 740, 744, 747, 750, 754, 757, 760, 764, 767, 770,
	774, 777, 780, 784, 787, 790, 794, 797, 800, 804, 807, 810, 813, 817,
	820, 823, 827, 830, 833, 837, 840, 843, 847, 850, 853, 857, 860, 863,
	867, 870, 873, 877, 880, 883, 887, 890, 893, 897, 900, 903, 907, 910,
	913, 916, 920, 923, 926, 930, 933, 936, 940, 943, 946, 950, 953, 956,
	960, 963, 966, 970, 973, 976, 980, 983, 986, 990, 993, 996, 1000, 1003,
	1006, 1009, 1013, 1016, 1019, 1023, 1026, 1029, 1033, 1036, 1039, 1043,
	1046, 1049, 1053, 1056, 1059, 1063, 1066, 1069, 1073, 1076
};

static diy_fp_t cached_power(int k)
{
	diy_fp_t res;
	int index = 343 + k;
	res.f = powers_ten[index];
	res.e = powers_ten_e[index];
	return res;
}

static int k_comp(int e, int alpha, int gamma) {
	return ceil((alpha-e+63) * D_1_LOG2_10);
}

static diy_fp_t minus(diy_fp_t x, diy_fp_t y)
{
	diy_fp_t r;
	assert(x.e == y.e);
	assert(x.f >= y.f);
	r.f = x.f - y.f;
	r.e = x.e;
	return r;
}

static diy_fp_t multiply(diy_fp_t x, diy_fp_t y)
{
	uint64_t a,b,c,d,ac,bc,ad,bd,tmp;
	diy_fp_t r;
	uint64_t M32 = 0xFFFFFFFF;
	a = x.f >> 32; b = x.f & M32;
	c = y.f >> 32; d = y.f & M32;
	ac = a*c; bc = b*c; ad = a*d; bd = b*d;
	tmp = (bd>>32) + (ad&M32) + (bc&M32);
	tmp += 1U << 31;
	r.f = ac+(ad>>32)+(bc>>32)+(tmp >>32);
	r.e = x.e + y.e + 64;
	return r;
}

static uint64_t double_to_uint64(double d)
{
	uint64_t n;
	memcpy(&n, &d, 8);
	return n;
}

#define DP_SIGNIFICAND_SIZE 52
#define DP_EXPONENT_BIAS (0x3FF + DP_SIGNIFICAND_SIZE)
#define DP_MIN_EXPONENT (-DP_EXPONENT_BIAS)
#define DP_EXPONENT_MASK 0x7FF0000000000000ll
#define DP_SIGNIFICAND_MASK 0x000FFFFFFFFFFFFFll
#define DP_HIDDEN_BIT 0x0010000000000000ll

static diy_fp_t double2diy_fp(double d)
{
	uint64_t d64 = double_to_uint64(d);
	int biased_e = (d64 & DP_EXPONENT_MASK) >> DP_SIGNIFICAND_SIZE;
	uint64_t significand = (d64 & DP_SIGNIFICAND_MASK);
	diy_fp_t res;
	if (biased_e != 0) {
		res.f = significand + DP_HIDDEN_BIT;
		res.e = biased_e - DP_EXPONENT_BIAS;
	} else {
		res.f = significand;
		res.e = DP_MIN_EXPONENT + 1;
	}
	return res;
}

static diy_fp_t normalize_boundary(diy_fp_t in)
{
	diy_fp_t res = in;
	/* Normalize now */
	/* the original number could have been a denormal. */
	while (! (res.f & (DP_HIDDEN_BIT << 1))) {
		res.f <<= 1;
		res.e--;
	}
	/* do the final shifts in one go. Don't forget the hidden bit (the '-1') */
	res.f <<= (DIY_SIGNIFICAND_SIZE - DP_SIGNIFICAND_SIZE - 2);
	res.e = res.e - (DIY_SIGNIFICAND_SIZE - DP_SIGNIFICAND_SIZE - 2);
	return res;
}

static void normalized_boundaries(double d, diy_fp_t* out_m_minus, diy_fp_t* out_m_plus)
{
	diy_fp_t v = double2diy_fp(d);
	diy_fp_t pl, mi;
	int significand_is_zero = v.f == DP_HIDDEN_BIT;
	pl.f = (v.f << 1) + 1; pl.e = v.e - 1;
	pl = normalize_boundary(pl);
	if (significand_is_zero) {
		mi.f = (v.f << 2) - 1;
		mi.e = v.e - 2;
	} else {
		mi.f = (v.f << 1) - 1;
		mi.e = v.e - 1;
	}
	mi.f <<= mi.e - pl.e;
	mi.e = pl.e;
	*out_m_plus = pl;
	*out_m_minus = mi;
}

#define TEN2 100
static void digit_gen(diy_fp_t Mp, diy_fp_t delta, char* buffer, int* len, int* K)
{
	uint32_t div, p1;
	uint64_t p2;
	int d,kappa;
	diy_fp_t one;
	one.f = ((uint64_t) 1) << -Mp.e; one.e = Mp.e;
	p1 = Mp.f >> -one.e;
	p2 = Mp.f & (one.f - 1);
	*len = 0; kappa = 3; div = TEN2;
	while (kappa > 0) {
		d = p1 / div;
		if (d || *len) buffer[(*len)++] = '0' + d;
		p1 %= div; kappa--; div /= 10;
		if ((((uint64_t)p1)<<-one.e)+p2 <= delta.f) {
			*K += kappa; return;
		}
	}
	do {
		p2 *= 10;
		d = p2 >> -one.e;
		if (d || *len) buffer[(*len)++] = '0' + d;
		p2 &= one.f - 1; kappa--; delta.f *= 10;
	} while (p2 > delta.f);
	*K += kappa;
}

int
js_grisu2(double v, char *buffer, int *K)
{
	int length, mk;
	diy_fp_t w_m, w_p, c_mk, Wp, Wm, delta;
	int q = 64, alpha = -59, gamma = -56;
	normalized_boundaries(v, &w_m, &w_p);
	mk = k_comp(w_p.e + q, alpha, gamma);
	c_mk = cached_power(mk);
	Wp = multiply(w_p, c_mk);
	Wm = multiply(w_m, c_mk);
	Wm.f++; Wp.f--;
	delta = minus(Wp, Wm);
	*K = -mk;
	digit_gen(Wp, delta, buffer, &length, K);
	return length;
}

/*
 * strtod.c
 *
 * Copyright (c) 1988-1993 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies. The University of
 * California makes no representations about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 */

/* Largest possible base 10 exponent. Any exponent larger than this will
 * already produce underflow or overflow, so there's no need to worry about
 * additional digits.
 */
static int maxExponent = 511;

/* Table giving binary powers of 10. Entry
 * is 10^2^i. Used to convert decimal
 * exponents into floating-point numbers.
 */
static double powersOf10[] = {
	10.,
	100.,
	1.0e4,
	1.0e8,
	1.0e16,
	1.0e32,
	1.0e64,
	1.0e128,
	1.0e256
};

/* Parse a decimal ASCII floating-point number, optionally preceded by white
 * space. Must have form "-I.FE-X", where I is the integer part of the
 * mantissa, F is the fractional part of the mantissa, and X is the exponent.
 * Either of the signs may be "+", "-", or omitted. Either I or F may be
 * omitted, or both. The decimal point isn't necessary unless F is present.
 * The "E" may actually be an "e". E and X may both be omitted (but not just
 * one).
 */
double
js_strtod(const char *string, char **endPtr)
{
	int sign, expSign = FALSE;
	double fraction, dblExp, *d;
	register const char *p;
	register int c;

	/* Exponent read from "EX" field. */
	int exp = 0;

	/* Exponent that derives from the fractional part. Under normal
	 * circumstances, it is the negative of the number of digits in F.
	 * However, if I is very long, the last digits of I get dropped
	 * (otherwise a long I with a large negative exponent could cause an
	 * unnecessary overflow on I alone). In this case, fracExp is
	 * incremented one for each dropped digit.
	 */
	int fracExp = 0;

	/* Number of digits in mantissa. */
	int mantSize;

	/* Number of mantissa digits BEFORE decimal point. */
	int decPt;

	/* Temporarily holds location of exponent in string. */
	const char *pExp;

	/*
	 * Strip off leading blanks and check for a sign.
	 */

	p = string;
	while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
		p += 1;
	}
	if (*p == '-') {
		sign = TRUE;
		p += 1;
	} else {
		if (*p == '+') {
			p += 1;
		}
		sign = FALSE;
	}

	/*
	 * Count the number of digits in the mantissa (including the decimal
	 * point), and also locate the decimal point.
	 */

	decPt = -1;
	for (mantSize = 0; ; mantSize += 1)
	{
		c = *p;
		if (!(c>='0'&&c<='9')) {
			if ((c != '.') || (decPt >= 0)) {
				break;
			}
			decPt = mantSize;
		}
		p += 1;
	}

	/*
	 * Now suck up the digits in the mantissa. Use two integers to
	 * collect 9 digits each (this is faster than using floating-point).
	 * If the mantissa has more than 18 digits, ignore the extras, since
	 * they can't affect the value anyway.
	 */

	pExp = p;
	p -= mantSize;
	if (decPt < 0) {
		decPt = mantSize;
	} else {
		mantSize -= 1;			/* One of the digits was the point. */
	}
	if (mantSize > 18) {
		fracExp = decPt - 18;
		mantSize = 18;
	} else {
		fracExp = decPt - mantSize;
	}
	if (mantSize == 0) {
		fraction = 0.0;
		p = string;
		goto done;
	} else {
		int frac1, frac2;
		frac1 = 0;
		for ( ; mantSize > 9; mantSize -= 1)
		{
			c = *p;
			p += 1;
			if (c == '.') {
				c = *p;
				p += 1;
			}
			frac1 = 10*frac1 + (c - '0');
		}
		frac2 = 0;
		for (; mantSize > 0; mantSize -= 1)
		{
			c = *p;
			p += 1;
			if (c == '.') {
				c = *p;
				p += 1;
			}
			frac2 = 10*frac2 + (c - '0');
		}
		fraction = (1.0e9 * frac1) + frac2;
	}

	/*
	 * Skim off the exponent.
	 */

	p = pExp;
	if ((*p == 'E') || (*p == 'e')) {
		p += 1;
		if (*p == '-') {
			expSign = TRUE;
			p += 1;
		} else {
			if (*p == '+') {
				p += 1;
			}
			expSign = FALSE;
		}
		while ((*p >= '0') && (*p <= '9')) {
			exp = exp * 10 + (*p - '0');
			p += 1;
		}
	}
	if (expSign) {
		exp = fracExp - exp;
	} else {
		exp = fracExp + exp;
	}

	/*
	 * Generate a floating-point number that represents the exponent.
	 * Do this by processing the exponent one bit at a time to combine
	 * many powers of 2 of 10. Then combine the exponent with the
	 * fraction.
	 */

	if (exp < -maxExponent) {
		exp = maxExponent;
		expSign = TRUE;
		errno = ERANGE;
	} else if (exp > maxExponent) {
		exp = maxExponent;
		expSign = FALSE;
		errno = ERANGE;
	} else if (exp < 0) {
		expSign = TRUE;
		exp = -exp;
	} else {
		expSign = FALSE;
	}
	dblExp = 1.0;
	for (d = powersOf10; exp != 0; exp >>= 1, d += 1) {
		if (exp & 01) {
			dblExp *= *d;
		}
	}
	if (expSign) {
		fraction /= dblExp;
	} else {
		fraction *= dblExp;
	}

done:
	if (endPtr != NULL) {
		*endPtr = (char *) p;
	}

	if (sign) {
		return -fraction;
	}
	return fraction;
}
