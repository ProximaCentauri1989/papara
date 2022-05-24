// Copyright (c) 2014 The Bitcoin Core developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2017 The PIVX developers
// Copyright (c) 2017-2019 The papara developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/transaction.h"
#include "main.h"
#include "spork.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(main_tests)

BOOST_AUTO_TEST_CASE(subsidy_limit_test)
{
    CAmount nSum = 0;
    CAmount nSubsidy = 0;

    nSubsidy = GetBlockValue(0);
    BOOST_CHECK(nSubsidy == 0.1 * COIN);
    nSum += nSubsidy;

    /* premine in block 1 (3,000,000 papara) */
    nSubsidy = GetBlockValue(1);
    BOOST_CHECK(nSubsidy == 3000000 * COIN);
    nSum += nSubsidy;

    nSubsidy = GetBlockValue(0);
    BOOST_CHECK(nSubsidy == 0.1 * COIN);
    nSum += nSubsidy;

    for (int nHeight = 3; nHeight <= 291; nHeight += 1) {
        /* PoW Phase */
        CAmount nSubsidy = GetBlockValue(nHeight);
        BOOST_CHECK(nSubsidy == 3 * COIN);
        nSum += nSubsidy;
    }

    for (int nHeight = 292; nHeight <= 1728; nHeight += 1) {
        CAmount nSubsidy = GetBlockValue(nHeight);
        BOOST_CHECK(nSubsidy == 2 * COIN);
        nSum += nSubsidy;
    }

    for (int nHeight = 1729; nHeight <= 4608; nHeight += 1) {
        CAmount nSubsidy = GetBlockValue(nHeight);
        BOOST_CHECK(nSubsidy == 5 * COIN);
        nSum += nSubsidy;
    }

    for (int nHeight = 4609; nHeight <= 8928; nHeight += 1) {
        CAmount nSubsidy = GetBlockValue(nHeight);
        BOOST_CHECK(nSubsidy == 10 * COIN);
        nSum += nSubsidy;
    }

    for (int nHeight = 8929; nHeight <= 11808; nHeight += 1) {
        CAmount nSubsidy = GetBlockValue(nHeight);
        BOOST_CHECK(nSubsidy == 25 * COIN);
        nSum += nSubsidy;
    }

    for (int nHeight = 11809; nHeight <= 16128; nHeight += 1) {
        CAmount nSubsidy = GetBlockValue(nHeight);
        BOOST_CHECK(nSubsidy == 70 * COIN);
        nSum += nSubsidy;
    }

    for (int nHeight = 16129; nHeight <= 19008; nHeight += 1) {
        CAmount nSubsidy = GetBlockValue(nHeight);
        BOOST_CHECK(nSubsidy == 100 * COIN);
        nSum += nSubsidy;
    }
    //Until TIER_BLOCK_HEIGHT
    for (int nHeight = 19009; nHeight <= 524500; nHeight += 1) {
        CAmount nSubsidy = GetBlockValue(nHeight);
        BOOST_CHECK(nSubsidy == 200 * COIN);
        nSum += nSubsidy;
    }

    for (int nHeight = 524501; nHeight <= 524500 + 100; nHeight += 1) {
        /* Masternode tiers system implemented */
        CAmount nSubsidy = GetBlockValue(nHeight);
        BOOST_CHECK(nSubsidy == 100 * COIN);
        nSum += nSubsidy;
    }

    for (int nHeight = 1578902; nHeight <= 1578902 + 100; nHeight += 1) {
        /* Masternode tiers system implemented */
        CAmount nSubsidy = GetBlockValue(nHeight);
        BOOST_CHECK(nSubsidy == 25 * COIN);
        nSum += nSubsidy;
    }

    for (int nHeight = 1578902; nHeight < SPORK_21_SUPERBLOCK_START_DEFAULT; nHeight += 1) {
        /* Standard reward of 25 coin until 21th spork and halving */
        CAmount nSubsidy = GetBlockValue(nHeight);
        BOOST_CHECK(nSubsidy == 25 * COIN);
        nSum += nSubsidy;
    }

    for (int nHeight = SPORK_21_SUPERBLOCK_START_DEFAULT; nHeight < SPORK_20_REWARD_HALVING_START_DEFAULT+SPORK_20_REWARD_HALVING_PERIOD_DEFAULT; nHeight += 1) {
        /* Standard reward of 1000 or 300000 coin after 21th spork and halving */
        CAmount nSubsidy = GetBlockValue(nHeight);
        if (nHeight % SPORK_21_SUPERBLOCK_PERIOD_DEFAULT == 0) {
            BOOST_CHECK(nSubsidy == 300000 * COIN);
        }else{
            BOOST_CHECK(nSubsidy == 1000 * COIN);
        }

        nSum += nSubsidy;
    }

    for (int nHeight = SPORK_22_SUPERBLOCK_START_DEFAULT; nHeight < SPORK_22_SUPERBLOCK_START_DEFAULT+SPORK_20_REWARD_HALVING_PERIOD_DEFAULT; nHeight += 1) {
        /* Standard reward of 1000000 or 30000000 coin after 22th spork and halving */
        CAmount nSubsidy = GetBlockValue(nHeight);
        if (nHeight % SPORK_21_SUPERBLOCK_PERIOD_DEFAULT == 0) {
            BOOST_CHECK(nSubsidy == 30000000 * COIN);
        }else{
            BOOST_CHECK(nSubsidy == 1000000 * COIN);
        }

        nSum += nSubsidy;
    }
}

BOOST_AUTO_TEST_CASE(halving_test)
{
    int blockBeforeSporkAndHalving = (SPORK_20_REWARD_HALVING_START_DEFAULT + (SPORK_21_SUPERBLOCK_START_DEFAULT - SPORK_20_REWARD_HALVING_START_DEFAULT)) - 10;

    BOOST_CHECK(GetHalvingReward(0, SPORK_20_REWARD_VALUE) == SPORK_20_REWARD_VALUE);
    BOOST_CHECK(GetHalvingReward(SPORK_20_REWARD_HALVING_START_DEFAULT, SPORK_20_REWARD_VALUE) == SPORK_20_REWARD_VALUE);
    /* Before 21 spork block reward is still 25 and default because halving will have not reached */
    BOOST_CHECK(GetHalvingReward(blockBeforeSporkAndHalving, SPORK_20_REWARD_VALUE) == SPORK_20_REWARD_VALUE);
    /* On 21 spork block reward is new but default because halving will have not reached */
    BOOST_CHECK(GetHalvingReward(SPORK_21_SUPERBLOCK_START_DEFAULT, SPORK_21_REWARD_VALUE) == SPORK_21_REWARD_VALUE);
    /* Cases when 21 spork and halving are reached */
    BOOST_CHECK(GetHalvingReward(1578902 + 525600, SPORK_21_REWARD_VALUE) == (SPORK_21_REWARD_VALUE / 2)); // 
    BOOST_CHECK(GetHalvingReward(1578902 + 525600 + 525600 + 525600 + 525600, SPORK_21_REWARD_VALUE) == (SPORK_21_REWARD_VALUE / 5));
    /* Cases when 22 spork and halving are reached */
    BOOST_CHECK(GetHalvingReward(SPORK_22_SUPERBLOCK_START_DEFAULT, SPORK_22_REWARD_VALUE) == SPORK_22_REWARD_VALUE);
    BOOST_CHECK(GetHalvingReward(SPORK_22_SUPERBLOCK_START_DEFAULT + 525600, SPORK_22_REWARD_VALUE) == SPORK_22_REWARD_VALUE / 2);
}

BOOST_AUTO_TEST_CASE(superblock_halving_test)
{
    double standartReward = 300000;
    double extendedReward = 30000000;

    BOOST_CHECK(GetSuperblockHalvingReward(0, standartReward) == standartReward);
    BOOST_CHECK(GetSuperblockHalvingReward( (SPORK_21_SUPERBLOCK_START_DEFAULT / SPORK_21_SUPERBLOCK_PERIOD_DEFAULT + 1) * SPORK_21_SUPERBLOCK_PERIOD_DEFAULT, standartReward) == standartReward);
    BOOST_CHECK(GetSuperblockHalvingReward(((SPORK_21_SUPERBLOCK_START_DEFAULT + 525600) / SPORK_21_SUPERBLOCK_PERIOD_DEFAULT + 1) * SPORK_21_SUPERBLOCK_PERIOD_DEFAULT, standartReward) == (standartReward / 2));
    BOOST_CHECK(GetSuperblockHalvingReward(((SPORK_21_SUPERBLOCK_START_DEFAULT + 525600 * 4) / SPORK_21_SUPERBLOCK_PERIOD_DEFAULT + 1) * SPORK_21_SUPERBLOCK_PERIOD_DEFAULT, standartReward) == (standartReward / 5));
    BOOST_CHECK(GetSuperblockHalvingReward( (SPORK_22_SUPERBLOCK_START_DEFAULT / SPORK_21_SUPERBLOCK_PERIOD_DEFAULT + 1) * SPORK_21_SUPERBLOCK_PERIOD_DEFAULT, standartReward) == extendedReward);
    BOOST_CHECK(GetSuperblockHalvingReward(((SPORK_22_SUPERBLOCK_START_DEFAULT + 525600) / SPORK_21_SUPERBLOCK_PERIOD_DEFAULT + 1) * SPORK_21_SUPERBLOCK_PERIOD_DEFAULT, standartReward) == (extendedReward / 2));
}

BOOST_AUTO_TEST_CASE(charity_address_valid_test)
{
    CScript payee;
    BOOST_CHECK(GetCharityPayee(SPORK_21_SUPERBLOCK_START_DEFAULT, payee) == true);
}

BOOST_AUTO_TEST_CASE(charity_address_invalid_test)
{
    CScript payee;
    BOOST_CHECK(GetCharityPayee(SPORK_20_REWARD_HALVING_START_DEFAULT, payee) == false);
}

BOOST_AUTO_TEST_SUITE_END()
