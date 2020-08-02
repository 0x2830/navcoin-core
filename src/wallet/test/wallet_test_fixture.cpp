#include <wallet/test/wallet_test_fixture.h>

#include <rpc/server.h>
#include <wallet/db.h>
#include <wallet/wallet.h>

WalletTestingSetup::WalletTestingSetup(const std::string& chainName):
    TestingSetup(chainName)
{
    bitdb.MakeMock();

    bool fFirstRun, fBLSCTFirstRun;
    pwalletMain = new CWallet("wallet_test.dat");
    pwalletMain->LoadWallet(fFirstRun, fBLSCTFirstRun);
    RegisterValidationInterface(pwalletMain);

    RegisterWalletRPCCommands(tableRPC);
}

WalletTestingSetup::~WalletTestingSetup()
{
    UnregisterValidationInterface(pwalletMain);
    delete pwalletMain;
    pwalletMain = nullptr;

    bitdb.Flush(true);
    bitdb.Reset();
}
