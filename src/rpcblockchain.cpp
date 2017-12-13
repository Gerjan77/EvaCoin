// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The EvaCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "main.h"
#include "bitcoinrpc.h"

using namespace json_spirit;
using namespace std;

void ScriptPubKeyToJSON(const CScript& scriptPubKey, Object& out);

double GetDifficulty(const CBlockIndex* blockindex)
{
    // Floating point number that is a multiple of the minimum difficulty,
    // minimum difficulty = 1.0.
    if (blockindex == NULL)
    {
        if (pindexBest == NULL)
            return 1.0;
        else
            blockindex = pindexBest;
    }

    int nShift = (blockindex->nBits >> 24) & 0xff;

    double dDiff =
        (double)0x0000ffff / (double)(blockindex->nBits & 0x00ffffff);

    while (nShift < 29)
    {
        dDiff *= 256.0;
        nShift++;
    }
    while (nShift > 29)
    {
        dDiff /= 256.0;
        nShift--;
    }

    return dDiff;
}

Object blockToJSON(const CBlock& block, const CBlockIndex* blockindex)
{
    Object result;
    result.push_back(Pair("hash", block.GetHash().GetHex()));
    CMerkleTx txGen(block.vtx[0]);
    txGen.SetMerkleBranch(&block);
    result.push_back(Pair("confirmations", (int)txGen.GetDepthInMainChain()));
    result.push_back(Pair("size", (int)::GetSerializeSize(block, SER_NETWORK, PROTOCOL_VERSION)));
    result.push_back(Pair("height", blockindex->nHeight));
    result.push_back(Pair("version", block.nVersion));
    result.push_back(Pair("merkleroot", block.hashMerkleRoot.GetHex()));
    Array txs;
    BOOST_FOREACH(const CTransaction&tx, block.vtx)
        txs.push_back(tx.GetHash().GetHex());
    result.push_back(Pair("tx", txs));
    result.push_back(Pair("time", (boost::int64_t)block.GetBlockTime()));
    result.push_back(Pair("nonce", (boost::uint64_t)block.nNonce));
    result.push_back(Pair("bits", HexBits(block.nBits)));
    result.push_back(Pair("difficulty", GetDifficulty(blockindex)));
    if (blockindex->pprev)
        result.push_back(Pair("previousblockhash", blockindex->pprev->GetBlockHash().GetHex()));
    CBlockIndex *pnext = blockindex->GetNextInMainChain();
    if (pnext)
        result.push_back(Pair("nextblockhash", pnext->GetBlockHash().GetHex()));
    return result;
}

Object blockInfo(const CBlock& block, const CBlockIndex* blockindex)
{
    Object result;
    CMerkleTx txGen(block.vtx[0]);
    txGen.SetMerkleBranch(&block);
    boost::uint64_t tBlock = block.GetBlockTime();
    string sTimb = rfcTime(tBlock);
    int iHeight = blockindex->nHeight;
    result.push_back(Pair("height", iHeight));
    result.push_back(Pair("time", sTimb));
    result.push_back(Pair("difficulty", GetDifficulty(blockindex)));
    return result;
}


Value getblockcount(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getblockcount\n"
            "Returns the number of blocks in the longest block chain.");

    return nBestHeight;
}

Value getbestblockhash(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getbestblockhash\n"
            "Returns the hash of the best (tip) block in the longest block chain.");

    return hashBestChain.GetHex();
}

Value getdifficulty(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getdifficulty\n"
            "Returns the proof-of-work difficulty as a multiple of the minimum difficulty.");

    return GetDifficulty();
}

Value settxfee(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 1)
        throw runtime_error(
            "settxfee <amount>\n"
            "<amount> is a real and is rounded to the nearest 0.00000001");

    // Amount
    int64 nAmount = 0;
    if (params[0].get_real() != 0.0)
        nAmount = AmountFromValue(params[0]);        // rejects 0.0 amounts

    nTransactionFee = nAmount;
    return true;
}

Value getrawmempool(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getrawmempool\n"
            "Returns all transaction ids in memory pool.");

    vector<uint256> vtxid;
    mempool.queryHashes(vtxid);

    Array a;
    BOOST_FOREACH(const uint256& hash, vtxid)
        a.push_back(hash.ToString());

    return a;
}

Value getblockhash(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getblockhash <index>\n"
            "Returns hash of block in best-block-chain at <index>.");

    int nHeight = params[0].get_int();
    if (nHeight < 0 || nHeight > nBestHeight)
        throw runtime_error("Block number out of range.");

    CBlockIndex* pblockindex = FindBlockByHeight(nHeight);
    return pblockindex->phashBlock->GetHex();
}

Value getblock(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "getblock <hash> [verbose=true]\n"
            "If verbose is false, returns a string that is serialized, hex-encoded data for block <hash>.\n"
            "If verbose is true, returns an Object with information about block <hash>."
        );

    std::string strHash = params[0].get_str();
    uint256 hash(strHash);

    bool fVerbose = true;
    if (params.size() > 1)
        fVerbose = params[1].get_bool();

    if (mapBlockIndex.count(hash) == 0)
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Block not found");

    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hash];
    ReadBlockFromDisk(block, pblockindex);

    if (!fVerbose)
    {
        CDataStream ssBlock(SER_NETWORK, PROTOCOL_VERSION);
        ssBlock << block;
        std::string strHex = HexStr(ssBlock.begin(), ssBlock.end());
        return strHex;
    }

    return blockToJSON(block, pblockindex);
}

Value getblockinfo(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "getblockinfo <height> [verbose=true]\n"
            "If verbose is false, returns a string that is serialized, hex-encoded data for block <height>.\n"
            "If verbose is true, returns an Object with information about block <height>."
        );
    std::string strHash = hashBestChain.GetHex();
    std::string sHeight = params[0].get_str();
    int mult = 1;
    int re = 0;
    int len = sHeight.length();
    for(int i = len -1 ; i >= 0 ; i--)
    {
         re = re + ((int)sHeight[i] -48)*mult;
         mult = mult*10;
    }
    int iHeight = re;

    uint256 hash(strHash);
    bool fVerbose = true;
    if (params.size() > 1)
        fVerbose = params[1].get_bool();
    if (mapBlockIndex.count(hash) == 0)
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Block not found");
    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hash];
    ReadBlockFromDisk(block, pblockindex);
    for (int c = nBestHeight; c > iHeight; c--)
    {
        if (pblockindex->pprev) strHash = pblockindex->pprev->GetBlockHash().GetHex();
        uint256 hash(strHash);
        pblockindex = mapBlockIndex[hash];
        ReadBlockFromDisk(block, pblockindex);
    }
    if (!fVerbose)
    {
        CDataStream ssBlock(SER_NETWORK, PROTOCOL_VERSION);
        ssBlock << block;
        std::string strHex = HexStr(ssBlock.begin(), ssBlock.end());
        return strHex;
    }
    return blockInfo(block, pblockindex);
}

Value getnextworkinfo(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "getnextworkinfo <height> [verbose=true]\n"
            "If verbose is false, returns a string that is serialized, hex-encoded data for block <height>.\n"
            "If verbose is true, returns an Object with information about block <height>."
        );
    std::string strHash = hashBestChain.GetHex();
    std::string sHeight = params[0].get_str();
    int mult = 1;
    int re = 0;
    int len = sHeight.length();
    for(int i = len -1 ; i >= 0 ; i--)
    {
         re = re + ((int)sHeight[i] -48)*mult;
         mult = mult*10;
    }
    int iHeight = re;
    uint256 hash(strHash);
    bool fVerbose = true;
    if (params.size() > 1)
        fVerbose = params[1].get_bool();
    if (mapBlockIndex.count(hash) == 0)
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Block not found");
    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hash];
    ReadBlockFromDisk(block, pblockindex);
    for (int c = nBestHeight; c > iHeight; c--)
    {
        if (pblockindex->pprev) strHash = pblockindex->pprev->GetBlockHash().GetHex();
        uint256 hash(strHash);
        pblockindex = mapBlockIndex[hash];
        ReadBlockFromDisk(block, pblockindex);
    }
    if (!fVerbose)
    {
        CDataStream ssBlock(SER_NETWORK, PROTOCOL_VERSION);
        ssBlock << block;
        std::string strHex = HexStr(ssBlock.begin(), ssBlock.end());
        return strHex;
    }
    const CBlockHeader hblock = block.GetBlockHeader();
    unsigned int iNextwork = GetNextWorkRequired(pblockindex, &hblock);
    CBigNum bnDif = CBigNum().SetCompact(iNextwork);
    double nextDif = Difficulty(bnDif.GetCompact());
    return nextDif;
}


static const int64 nLowerBound = 250;
static const int64 nUpperBound = 950;

Value nextwork(const Array& params, bool fHelp)
{
    if (fHelp || params.size() > 0)
        throw runtime_error(
            "nextwork \n"
            "Call info from GetNextWorkRequired from main.cpp\n"
            "\n"
        );
    std::string strHash = hashBestChain.GetHex();
    uint256 hash(strHash);
    if (mapBlockIndex.count(hash) == 0)
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Block not found");
    CBlock block;
    CBlockIndex* pblockindex = mapBlockIndex[hash];
    ReadBlockFromDisk(block, pblockindex);
    const CBlockHeader hblock = block.GetBlockHeader();
    const CBlockHeader* pblock = &hblock;
    const CBlockIndex* pindexLast = pblockindex;
    Object result;

    ///
    /// start a copy from GetNextWorkRequired main.cpp
    ///
    ///
    ///
    ///
    result.push_back(Pair("Starting", "GetNextWorkRequired main.cpp"));

    unsigned int nProofOfWorkLimit = Params().ProofOfWorkLimit().GetCompact();

    // Genesis block
    if (pindexLast == NULL)
    {
        result.push_back(Pair("nProofOfWorkLimit", "Difficulty(nProofOfWorkLimit)"));
        return result;
    }
    boost::uint64_t tBlock = pindexLast->nTime;
    time_t now;
    time(&now);
    boost::uint64_t tNow = now;
    if (tNow-tBlock > 3600)
    {
        result.push_back(Pair("nProofOfWorkLimit", "Difficulty(nProofOfWorkLimit)"));
        //return result;
    }
    result.push_back(Pair("Starting", "Go back by 144 blocks"));
    // Go back by 144 blocks if possible
    const CBlockIndex* pindexFirst = pindexLast;
    const CBlockIndex* pindex = pindexLast;
    int64 iHeight = pindexFirst->nHeight;
    int64 iAdjustTimeSpan = 0;
    double dHrate=0;
    int j=0;
    int k=0;
    double dNetworkDiff=0;
    if (iHeight > 144)
    {
        for (int i = 0; pindex && i < 144; i++)
        {
            int64 time2 = pindex->nTime;
            double dDi = Difficulty(pindex->nBits);
            pindex = pindex->pprev;
            assert(pindex);
            int64 time1 = pindex->nTime;
            int64 time3 = time2 - time1;
            if (time3 > 3600) k += time3 / 600;
            if ((time3 > 250) && (time3 < 950))
            {
                dHrate += HashRate(dDi,time3);
                j++;
            }
        }
        if (j > 0)
        {
            dHrate/=(j+k);
            int64 iMulti = 0x100000000;
            dNetworkDiff = dHrate*600/iMulti;
            iAdjustTimeSpan = (int64)(((600 * Difficulty(pindexLast->nBits)) / dNetworkDiff) + 0.5);
        }
    }

    pindexFirst = pindexFirst->pprev;
    assert(pindexFirst);
    // Limit adjustment step
    int64 nActualTimespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();
    int64 iTimespan = nActualTimespan;
    if (nActualTimespan < nLowerBound)
        nActualTimespan = nLowerBound;
    if (nActualTimespan > nUpperBound)
        nActualTimespan = nUpperBound;
    // Retarget
    if (iAdjustTimeSpan != 0 ) nActualTimespan = iAdjustTimeSpan;

    CBigNum bnNew;
    bnNew.SetCompact(pindexLast->nBits);
    bnNew *= nActualTimespan;
    bnNew /= 600;
    if (bnNew > Params().ProofOfWorkLimit())
        bnNew = Params().ProofOfWorkLimit();

    /// debug print
    int64 nCurrTime = pblock->nTime;
    int64 nPrevTime = pindexFirst->nTime;
    int64 nCurrentTimespan = nCurrTime - nPrevTime;
    result.push_back(Pair("Network Difficulty ", dNetworkDiff));
    result.push_back(Pair("Network Hashrate ", dHrate));
    result.push_back(Pair("Based on blocks /144 ", j));
    result.push_back(Pair("blocks reset ", k));
    result.push_back(Pair("previous difficulty", Difficulty(pindexLast->nBits)));
    result.push_back(Pair("next difficulty", Difficulty(bnNew.GetCompact())));
    result.push_back(Pair("height", iHeight));

    string sTim = rfcTime(tBlock);
    char buffer[64];
    struct tm* now_gmt = gmtime(&now);
    string locale(setlocale(LC_TIME, NULL));
    setlocale(LC_TIME, "C"); // we want POSIX (aka "C") weekday/month strings
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S +0000", now_gmt);
    setlocale(LC_TIME, locale.c_str());
    string sNow = string(buffer);
    result.push_back(Pair("block", sTim));
    result.push_back(Pair("now", sNow));
    result.push_back(Pair("block age hrs ", (tNow-tBlock)/3600));

    return result;
    //return bnNew.GetCompact();
}

Value gettxoutsetinfo(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "gettxoutsetinfo\n"
            "Returns statistics about the unspent transaction output set.");

    Object ret;

    CCoinsStats stats;
    if (pcoinsTip->GetStats(stats)) {
        ret.push_back(Pair("height", (boost::int64_t)stats.nHeight));
        ret.push_back(Pair("bestblock", stats.hashBlock.GetHex()));
        ret.push_back(Pair("transactions", (boost::int64_t)stats.nTransactions));
        ret.push_back(Pair("txouts", (boost::int64_t)stats.nTransactionOutputs));
        ret.push_back(Pair("bytes_serialized", (boost::int64_t)stats.nSerializedSize));
        ret.push_back(Pair("hash_serialized", stats.hashSerialized.GetHex()));
        ret.push_back(Pair("total_amount", ValueFromAmount(stats.nTotalAmount)));
    }
    return ret;
}

Value gettxout(const Array& params, bool fHelp)
{
    if (fHelp || params.size() < 2 || params.size() > 3)
        throw runtime_error(
            "gettxout <txid> <n> [includemempool=true]\n"
            "Returns details about an unspent transaction output.");

    Object ret;

    std::string strHash = params[0].get_str();
    uint256 hash(strHash);
    int n = params[1].get_int();
    bool fMempool = true;
    if (params.size() > 2)
        fMempool = params[2].get_bool();

    CCoins coins;
    if (fMempool) {
        LOCK(mempool.cs);
        CCoinsViewMemPool view(*pcoinsTip, mempool);
        if (!view.GetCoins(hash, coins))
            return Value::null;
        mempool.pruneSpent(hash, coins); // TODO: this should be done by the CCoinsViewMemPool
    } else {
        if (!pcoinsTip->GetCoins(hash, coins))
            return Value::null;
    }
    if (n<0 || (unsigned int)n>=coins.vout.size() || coins.vout[n].IsNull())
        return Value::null;

    ret.push_back(Pair("bestblock", pcoinsTip->GetBestBlock()->GetBlockHash().GetHex()));
    if ((unsigned int)coins.nHeight == MEMPOOL_HEIGHT)
        ret.push_back(Pair("confirmations", 0));
    else
        ret.push_back(Pair("confirmations", pcoinsTip->GetBestBlock()->nHeight - coins.nHeight + 1));
    ret.push_back(Pair("value", ValueFromAmount(coins.vout[n].nValue)));
    Object o;
    ScriptPubKeyToJSON(coins.vout[n].scriptPubKey, o);
    ret.push_back(Pair("scriptPubKey", o));
    ret.push_back(Pair("version", coins.nVersion));
    ret.push_back(Pair("coinbase", coins.fCoinBase));

    return ret;
}

Value verifychain(const Array& params, bool fHelp)
{
    if (fHelp || params.size() > 2)
        throw runtime_error(
            "verifychain [check level] [num blocks]\n"
            "Verifies blockchain database.");

    int nCheckLevel = GetArg("-checklevel", 3);
    int nCheckDepth = GetArg("-checkblocks", 288);
    if (params.size() > 0)
        nCheckLevel = params[0].get_int();
    if (params.size() > 1)
        nCheckDepth = params[1].get_int();

    return VerifyDB(nCheckLevel, nCheckDepth);
}

