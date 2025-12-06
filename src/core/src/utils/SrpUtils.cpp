//
// Created by vogje01 on 12/19/24.
//

#include <awsmock/core/SrpUtils.h>

namespace AwsMock::Core {

    SrpUtils::SrpUtils(const int nMFrom) : m_nMFrom(nMFrom) {

        m_pA = nullptr;
        m_pB = nullptr;
        m_pg = nullptr;
        m_pK = nullptr;
        m_pM1 = nullptr;
        m_pM2 = nullptr;
        m_pN = nullptr;
        m_pS = nullptr;
        m_pSalt = nullptr;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        SRP_gN *pGN = SRP_get_default_gN("1024");
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        assert(pGN);
        m_pg = BN_dup(pGN->g);
        m_pN = BN_dup(pGN->N);

        m_pb = nullptr;
        m_pV = nullptr;
    }

    SrpUtils::~SrpUtils() {
        BN_free(m_pA);
        BN_free(m_pB);
        BN_clear_free(m_pg);
        BN_clear_free(m_pK);
        BN_free(m_pM1);
        BN_free(m_pM2);
        BN_clear_free(m_pN);
        BN_clear_free(m_pS);
        BN_free(m_pSalt);
    }

    bool SrpUtils::VerifyA(const std::string &srpAStr) {
        BIGNUM *sprA = BN_new();
        if (const int ret = BN_hex2bn(&sprA, srpAStr.c_str()); !ret) {
            BN_print_fp(stderr, sprA);
        }
        const bool ret = VerifyA(sprA);
        BN_clear_free(sprA);
        return ret;
    }

    const BIGNUM *SrpUtils::CalcM1() {
        assert(GetS());
        switch (m_nMFrom) {
            case M_FROM_K:
                CalcM1FromK();
                break;
            case M_FROM_S:
                CalcM1FromS();
                break;
            default:
                assert(0);
                break;
        }

        assert(m_pM1);
        return m_pM1;
    }
    const BIGNUM *SrpUtils::CalcB() {
        assert(GetN());
        assert(Getg());
        assert(m_pV);

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        unsigned char rand_tmp[RANDOM_SIZE];
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        RAND_bytes(rand_tmp, sizeof(rand_tmp));
        BN_free(m_pb);
        m_pb = BN_bin2bn(rand_tmp, sizeof(rand_tmp), nullptr);
        assert(m_pb);
        SetB(SRP_Calc_B(m_pb, GetN(), Getg(), m_pV));
        assert(GetB());
        return GetB();
    }

    bool SrpUtils::CalcV(BIGNUM **ppSalt, const char *pszPassword) {
        assert(ppSalt);
        assert(pszPassword);
        assert(GetN());
        assert(Getg());
        BN_clear_free(m_pV);
        m_pV = nullptr;
        if (SRP_create_verifier_BN(GetI(), pszPassword, ppSalt, &m_pV, GetN(), Getg())) {
            assert(*ppSalt);
            assert(m_pV);
            SetSalt(BN_dup(*ppSalt));
            assert(GetSalt());
            return true;
        }

        assert(0);
        return false;
    }

    // ============================================================================
    // ==============================================================================
    void SrpUtils::SetSaltAndV(BIGNUM *pSalt, BIGNUM *pV) {
        assert(pSalt);
        assert(pV);
        SetSalt(pSalt);
        BN_clear_free(m_pV);
        m_pV = pV;
        assert(GetSalt());
    }

    // ============================================================================
    // ==============================================================================
    bool SrpUtils::VerifyA(const BIGNUM *pA) {
        assert(pA);
        assert(GetN());
        if (SRP_Verify_A_mod_N(pA, GetN())) {
            SetA(BN_dup(pA));
            assert(GetA());
            return true;
        }

        assert(0);
        return false;
    }

    // ============================================================================
    // ==============================================================================
    bool SrpUtils::VerifyM1(const BIGNUM *pM1) {
        assert(GetN());
        assert(GetA());
        assert(GetB());
        assert(m_pV);
        assert(m_pb);

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        bool bRet = false;
        BIGNUM *u = SRP_Calc_u(GetA(), GetB(), GetN());
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        if (!BN_is_zero(u)) {
            SetS(SRP_Calc_server_key(GetA(), m_pV, u, m_pb, GetN()));
            assert(GetS());
            BN_clear_free(u);
            return !BN_cmp(pM1, CalcM1());
        }

        assert(0);
        BN_clear_free(u);
        return bRet;
    }

    // ==============================================================================
    // Common
    // ==============================================================================

    const BIGNUM *SrpUtils::CalcM1FromK() {
        // K = H(S) and M1 = H[H(N) XOR H(g) | H(I) | s | A | B | K]
        assert(GetN());
        assert(Getg());
        assert(GetSalt());
        assert(GetA());
        assert(GetB());
        assert(GetS());
        BN_free(m_pK);
        BN_free(m_pM1);
        m_pK = nullptr;
        m_pM1 = nullptr;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        EVP_MD_CTX *ctxt = EVP_MD_CTX_new();
        auto *cs = static_cast<unsigned char *>(OPENSSL_malloc(BN_num_bytes(GetN())));
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        assert(ctxt);
        assert(cs);
        if (EVP_DigestInit_ex(ctxt, EVP_sha1(), nullptr)) {

            // H(N)
            BN_bn2bin(GetN(), cs);
            if (unsigned char dig[SHA_DIGEST_LENGTH]; EVP_DigestUpdate(ctxt, cs, BN_num_bytes(GetN())) && EVP_DigestFinal_ex(ctxt, dig, nullptr)) {

                // H(g) -> dig2
                BN_bn2bin(Getg(), cs);
                if (unsigned char dig2[SHA_DIGEST_LENGTH]; EVP_DigestInit_ex(ctxt, EVP_sha1(), nullptr) && EVP_DigestUpdate(ctxt, cs, BN_num_bytes(Getg()) && EVP_DigestFinal_ex(ctxt, dig2, nullptr))) {

                    // H(N) ^ H(g)
                    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
                        dig[i] ^= dig2[i];
                    }

                    // H(I) -> dig2
                    if (EVP_DigestInit_ex(ctxt, EVP_sha1(), nullptr) && EVP_DigestUpdate(ctxt, GetI(), strlen(GetI())) && EVP_DigestFinal_ex(ctxt, dig2, nullptr)) {

                        // H(S) -> dig3
                        BN_bn2bin(GetS(), cs);
                        if (unsigned char dig3[SHA_DIGEST_LENGTH]; EVP_DigestInit_ex(ctxt, EVP_sha1(), nullptr) && EVP_DigestUpdate(ctxt, cs, BN_num_bytes(GetS())) && EVP_DigestFinal_ex(ctxt, dig3, nullptr)) {

                            // H(N) ^ H(g) | H(I)
                            if (EVP_DigestInit_ex(ctxt, EVP_sha1(), nullptr) && EVP_DigestUpdate(ctxt, dig, sizeof(dig)) && EVP_DigestUpdate(ctxt, dig2, sizeof(dig2))) {

                                // | s
                                BN_bn2bin(GetSalt(), cs);
                                if (EVP_DigestUpdate(ctxt, cs, BN_num_bytes(GetSalt()))) {

                                    // | A
                                    BN_bn2bin(GetA(), cs);
                                    if (EVP_DigestUpdate(ctxt, cs, BN_num_bytes(GetA()))) {

                                        // | B | H(S)
                                        BN_bn2bin(GetB(), cs);
                                        if (EVP_DigestUpdate(ctxt, cs, BN_num_bytes(GetB())) && EVP_DigestUpdate(ctxt, dig3, sizeof(dig3)) && EVP_DigestFinal_ex(ctxt, dig, nullptr)) {
                                            m_pM1 = BN_bin2bn(dig, sizeof(dig), nullptr);
                                            assert(m_pM1);
                                            m_pK = BN_bin2bn(dig3, sizeof(dig3), nullptr);
                                            assert(m_pK);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        OPENSSL_free(cs);
        EVP_MD_CTX_free(ctxt);
        assert(m_pK);
        assert(m_pM1);
        return m_pM1;
    }

    const BIGNUM *SrpUtils::CalcM1FromS() {
        // M1 = H(A | B | S)
        assert(GetN());
        assert(GetA());
        assert(GetB());
        assert(GetS());
        BN_free(m_pM1);
        m_pM1 = nullptr;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        EVP_MD_CTX *ctxt = EVP_MD_CTX_new();
        auto *cs = static_cast<unsigned char *>(OPENSSL_malloc(BN_num_bytes(GetN())));
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        assert(ctxt);
        assert(cs);
        if (EVP_DigestInit_ex(ctxt, EVP_sha1(), nullptr)) {
            BN_bn2bin(GetA(), cs);
            if (EVP_DigestUpdate(ctxt, cs, BN_num_bytes(GetA()))) {
                BN_bn2bin(GetB(), cs);
                if (EVP_DigestUpdate(ctxt, cs, BN_num_bytes(GetB()))) {
                    BN_bn2bin(GetS(), cs);
                    if (unsigned char dig[SHA_DIGEST_LENGTH]; EVP_DigestUpdate(ctxt, cs, BN_num_bytes(GetS())) && EVP_DigestFinal_ex(ctxt, dig, nullptr)) {
                        m_pM1 = BN_bin2bn(dig, sizeof(dig), nullptr);
                        assert(m_pM1);
                    }
                }
            }
        }

        OPENSSL_free(cs);
        EVP_MD_CTX_free(ctxt);
        assert(m_pM1);
        return m_pM1;
    }

    const BIGNUM *SrpUtils::CalcM2() {
        // M2 = H(A | M1 | K) or H(A | M1 | S)
        assert(GetN());
        assert(GetA());
        assert(m_pM1);
        BN_free(m_pM2);
        m_pM2 = nullptr;

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        EVP_MD_CTX *ctxt = EVP_MD_CTX_new();
        auto *cs = static_cast<unsigned char *>(OPENSSL_malloc(BN_num_bytes(GetN())));
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        assert(ctxt);
        assert(cs);
        if (EVP_DigestInit_ex(ctxt, EVP_sha1(), nullptr)) {
            BN_bn2bin(GetA(), cs);
            if (EVP_DigestUpdate(ctxt, cs, BN_num_bytes(GetA()))) {
                BN_bn2bin(m_pM1, cs);
                if (EVP_DigestUpdate(ctxt, cs, BN_num_bytes(m_pM1))) {
                    switch (m_nMFrom) {
                        case M_FROM_K:
                            assert(m_pK);
                            BN_bn2bin(m_pK, cs);
                            EVP_DigestUpdate(ctxt, cs, BN_num_bytes(m_pK));
                            break;
                        case M_FROM_S:
                            assert(GetS());
                            BN_bn2bin(GetS(), cs);
                            EVP_DigestUpdate(ctxt, cs, BN_num_bytes(GetS()));
                            break;
                        default:
                            assert(0);
                            break;
                    }

                    if (unsigned char dig[SHA_DIGEST_LENGTH]; EVP_DigestFinal_ex(ctxt, dig, nullptr)) {
                        m_pM2 = BN_bin2bn(dig, sizeof(dig), nullptr);
                        assert(m_pM2);
                    }
                }
            }
        }

        OPENSSL_free(cs);
        EVP_MD_CTX_free(ctxt);
        assert(m_pM2);
        return m_pM2;
    }
    void SrpUtils::SetA(BIGNUM *pA) {
        assert(pA);
        BN_free(m_pA);
        m_pA = pA;
    }

    void SrpUtils::SetB(BIGNUM *pB) {
        assert(pB);
        BN_free(m_pB);
        m_pB = pB;
    }

    void SrpUtils::SetI(const char *pszUserName) {
        assert(pszUserName);
        m_strUserName = pszUserName;
    }

    void SrpUtils::SetS(BIGNUM *pS) {
        assert(pS);
        BN_free(m_pS);
        m_pS = pS;
    }

    void SrpUtils::SetSalt(BIGNUM *pSalt) {
        assert(pSalt);
        BN_free(m_pSalt);
        m_pSalt = pSalt;
    }
}// namespace AwsMock::Core