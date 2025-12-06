//
// Created by vogje01 on 12/19/24.
//

#ifndef AWS_MOCK_CORE_SRP_UTILS_H
#define AWS_MOCK_CORE_SRP_UTILS_H

// C++ includes
#include <cassert>
#include <string>

// OpenSSL includes
#define OPENSSL_SUPPRESS_DEPRECATED
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/srp.h>

// AwsMock includes

namespace AwsMock::Core {

    enum { M_FROM_K,
           M_FROM_S,
    };
    enum { RANDOM_SIZE = 32,
    };

    class SrpUtils {

      public:

        explicit SrpUtils(int nMFrom = M_FROM_K);
        ~SrpUtils();

        const BIGNUM *CalcB();
        bool CalcV(BIGNUM **ppSalt, const char *pszPassword);
        void SetSaltAndV(BIGNUM *pSalt, BIGNUM *pV);
        bool VerifyA(const BIGNUM *pA);
        bool VerifyM1(const BIGNUM *pM1);

        /**
         * @brief Verify the initial SPR_A parameter of a InitialAuth flow.
         *
         * @param srpAStr SPA_A auth parameter
         * @return true if it is verified
         */
        bool VerifyA(const std::string &srpAStr);

      private:

        // Common
        // non-const method
        const BIGNUM *CalcM1();
        const BIGNUM *CalcM2();
        void SetA(BIGNUM *pA);
        void SetB(BIGNUM *pB);
        void SetI(const char *pszUserName);
        void SetMFrom(int nMFrom);
        void SetS(BIGNUM *pS);
        void SetSalt(BIGNUM *pSalt);

        // non-const method
        const BIGNUM *CalcM1FromK();
        const BIGNUM *CalcM1FromS();

        // const method
        [[nodiscard]] const BIGNUM *GetA() const;
        [[nodiscard]] const BIGNUM *GetB() const;
        [[nodiscard]] const BIGNUM *Getg() const;
        [[nodiscard]] const char *GetI() const;
        [[nodiscard]] const BIGNUM *GetK() const;
        [[nodiscard]] int GetMFrom() const;
        [[nodiscard]] const BIGNUM *GetN() const;
        [[nodiscard]] const BIGNUM *GetS() const;
        [[nodiscard]] const BIGNUM *GetSalt() const;

        int m_nMFrom;
        BIGNUM *m_pA;
        BIGNUM *m_pB;
        BIGNUM *m_pg;
        BIGNUM *m_pK;
        BIGNUM *m_pM1;
        BIGNUM *m_pM2;
        BIGNUM *m_pN;
        BIGNUM *m_pS;
        BIGNUM *m_pSalt;
        std::string m_strUserName;

        // Server
        BIGNUM *m_pb;
        BIGNUM *m_pV;
    };

    inline const BIGNUM *SrpUtils::GetA() const {
        return m_pA;
    }

    inline const BIGNUM *SrpUtils::GetB() const {
        return m_pB;
    }

    inline const BIGNUM *SrpUtils::Getg() const {
        return m_pg;
    }

    inline const char *SrpUtils::GetI() const {
        return m_strUserName.c_str();
    }

    inline const BIGNUM *SrpUtils::GetK() const {
        return m_pK;
    }

    inline const BIGNUM *SrpUtils::GetN() const {
        return m_pN;
    }

    inline const BIGNUM *SrpUtils::GetS() const {
        return m_pS;
    }

    inline const BIGNUM *SrpUtils::GetSalt() const {
        return m_pSalt;
    }

    inline void SrpUtils::SetMFrom(const int nMFrom) {
        m_nMFrom = nMFrom;
    }
}// namespace AwsMock::Core

#endif// AWS_MOCK_CORE_SRP_UTILS_H
