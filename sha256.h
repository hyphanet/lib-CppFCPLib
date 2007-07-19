#if !defined( _sha256_h )
#define _sha256_h

#define word32 unsigned long
#define byte unsigned char

typedef struct {
      word32 H[ 8 ];
      word32 hbits, lbits;
      byte M[ 64 ];
      byte mlen;
} SHA256_ctx;

void SHA256_init ( SHA256_ctx* );
void SHA256_update( SHA256_ctx*, const byte *, word32 );
void SHA256_final ( SHA256_ctx* );
void SHA256_digest( SHA256_ctx*, byte* );

class SHA256 {

    SHA256_ctx ctx;
    byte digest[32];
 public:

    SHA256 () { init (); }

    enum { DIGEST_LEN = 32 };

    void init ( ) { SHA256_init (&ctx); }
    void reset () { SHA256_init (&ctx); }
    void write(byte *a, unsigned int b) { SHA256_update (&ctx, a, b); }
    void final () { SHA256_final (&ctx); SHA256_digest (&ctx, digest); }
    byte *read () { return digest; }
};

#endif
