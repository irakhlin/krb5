#include "k5-int.h"
#include "rsa-md4.h"
#include "arcfour-int.h"

static void asctouni(unsigned char *unicode, unsigned char *ascii, size_t len)
{
	int counter;
	for (counter=0;counter<len;counter++) {
		unicode[2*counter]=ascii[counter];
		unicode[2*counter + 1]=0x00;
	}
}

krb5_error_code
krb5_arcfour_string_to_key(enc, string, salt, key)
	krb5_const struct krb5_enc_provider *enc;
	krb5_const krb5_data *string;
	krb5_const krb5_data *salt;
	krb5_keyblock *key;
{
  size_t len,slen;
  unsigned char *copystr;
  krb5_MD4_CTX md4_context;
  
  if (key->length != 16)
    return (KRB5_BAD_MSIZE);

  /* We ignore salt per the Microsoft spec*/

  /* compute the space needed for the new string.
     Since the password must be stored in unicode, we need to increase
     that number by 2x.

     This should be re-evauated in the future, it makes the assumption that
     thes user's password is in ascii.
  */
  slen = ((string->length)>128)?128:string->length;
  len=(slen)*2;

  copystr = malloc(len);
  if (copystr == NULL)
    return ENOMEM;

  /* make the string.  start by creating the unicode version of the password*/
  asctouni(copystr, string->data, slen );

  /* the actual MD4 hash of the data */
  krb5_MD4Init(&md4_context);
  krb5_MD4Update(&md4_context, (unsigned char *)copystr, len);
  krb5_MD4Final(&md4_context);
  memcpy(key->contents, md4_context.digest, 16);

#if 0  
  /* test the string_to_key function */
  printf("Hash=");
  {
    int counter;
    for(counter=0;counter<16;counter++)
      printf("%02x", md4_context.digest[counter]);
    printf("\n");
  }
#endif /* 0 */

  /* Zero out the data behind us */
  memset (copystr, 0, len);
  memset(&md4_context, 0, sizeof(md4_context));
  return 0;
}
