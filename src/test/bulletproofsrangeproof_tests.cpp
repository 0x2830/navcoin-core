// Copyright (c) 2020 The NavCoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "blsct/bulletproofs.h"
#include "test/test_navcoin.h"

#include <map>

#include <boost/test/unit_test.hpp>
#include "boost/assign.hpp"

BOOST_FIXTURE_TEST_SUITE(bulletproofsrangeproof, BasicTestingSetup)

bool TestRange(std::vector<Scalar> values, Point nonce)
{
    std::vector<Scalar> gamma;
    std::vector<Point> nonces;

    for (unsigned int i = 0; i < values.size(); i++)
    {
        gamma.push_back(Scalar::Rand());
        nonces.push_back(nonce);
    }

    BulletproofsRangeproof bprp;
    bprp.Prove(values, gamma, nonce, {1, 2, 3, 4});

    std::vector<std::pair<int, BulletproofsRangeproof>> proofs;
    proofs.push_back(std::make_pair(0,bprp));

    std::vector<uint64_t> amounts;
    std::vector<Scalar> gammas;
    std::vector<std::vector<uint8_t>> messages;

    std::vector<RangeproofEncodedData> data;

    bool ret = VerifyBulletproof(proofs, data, nonces);

    if (!ret)
        return ret;

    if (values.size() == 1)
    {
        for (unsigned int i = 0; i < values.size(); i++)
        {
            if(data[i].amount != values[i].GetUint64())
                return false;
            if(!(data[i].gamma == gamma[i]))
                return false;
            if(data[i].message[0] != 1)
                return false;
            if(data[i].message[1] != 2)
                return false;
            if(data[i].message[2] != 3)
                return false;
            if(data[i].message[3] != 4)
                return false;
        }
    }

    return true;
}

bool TestRangeBatch(std::vector<Scalar> values, Point nonce)
{
    std::vector<std::pair<int, BulletproofsRangeproof>> proofs;
    std::vector<Point> nonces;
    std::vector<Scalar> gamma;

    for (unsigned int i = 0; i < values.size(); i++)
    {
        std::vector<Scalar> v;
        v.push_back(values[i]);

        std::vector<Scalar> g;
        Scalar mask = Scalar::Rand();
        g.push_back(mask);
        gamma.push_back(mask);

        BulletproofsRangeproof bprp;
        bprp.Prove(v, g, nonce, {1,2,3,4});

        nonces.push_back(nonce);

        proofs.push_back(std::make_pair(i, bprp));
    }

    std::vector<uint64_t> amounts;
    std::vector<Scalar> gammas;
    std::vector<std::vector<uint8_t>> messages;

    std::vector<RangeproofEncodedData> data;

    bool ret = VerifyBulletproof(proofs, data, nonces);

    if (!ret)
        return ret;

    for (unsigned int i = 0; i < values.size(); i++)
    {
        if(data[i].amount != values[i].GetUint64())
            return false;
        if(!(data[i].gamma == gamma[i]))
            return false;
        if(data[i].message[0] != 1)
            return false;
        if(data[i].message[1] != 2)
            return false;
        if(data[i].message[2] != 3)
            return false;
        if(data[i].message[3] != 4)
            return false;
    }

    return true;
}

BOOST_AUTO_TEST_CASE(RangeProofTest)
{
    std::vector<Scalar> vInRange;
    std::vector<Scalar> vOutOfRange;

    Point nonce = Point::Rand();

    // Admited range is (0, 2**64)
    Scalar one;
    one = 1;
    Scalar two;
    two = 2;
    Scalar twoPow64;
    twoPow64.SetPow2(64);
    Scalar bnLowerBound = 0;
    Scalar bnUpperBound = twoPow64-one;

    vInRange.push_back(bnLowerBound);
    vInRange.push_back(bnLowerBound+one);
    vInRange.push_back(bnUpperBound-one);
    vInRange.push_back(bnUpperBound);
    vOutOfRange.push_back(bnUpperBound+one);
    vOutOfRange.push_back(bnUpperBound+one+one);
    vOutOfRange.push_back(bnUpperBound*two);
    vOutOfRange.push_back(one.Negate());

    for (Scalar v: vInRange)
    {
        std::vector<Scalar> values;
        bn_print(v.bn);
        values.push_back(v);
        BOOST_CHECK(TestRange(values, nonce));
    }

    for (Scalar v: vOutOfRange)
    {
        std::vector<Scalar> values;
        values.push_back(v);
        BOOST_CHECK(!TestRange(values, nonce));
    }

    BOOST_CHECK(TestRangeBatch(vInRange, nonce));
    BOOST_CHECK(TestRange(vInRange, nonce));
    BOOST_CHECK(!TestRangeBatch(vOutOfRange, nonce));
    BOOST_CHECK(!TestRange(vOutOfRange, nonce));
}

BOOST_AUTO_TEST_SUITE_END()
