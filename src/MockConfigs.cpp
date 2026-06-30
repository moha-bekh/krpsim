#include "krpsim/MockConfigs.hpp"

namespace krpsim::mock {

Config simpleDemo()
{
    Config config;

    config.initialStocks["euro"] = 10;
    config.optimizeTime = true;
    config.optimizeResources.push_back("client_content");

    config.processes.push_back(Process{
        "achat_materiel",
        {{"euro", 8}},
        {{"materiel", 1}},
        10
    });

    config.processes.push_back(Process{
        "realisation_produit",
        {{"materiel", 1}},
        {{"produit", 1}},
        30
    });

    config.processes.push_back(Process{
        "livraison",
        {{"produit", 1}},
        {{"client_content", 1}},
        20
    });

    return config;
}

Config ikeaDemo()
{
    Config config;

    config.initialStocks["planche"] = 7;
    config.optimizeTime = true;
    config.optimizeResources.push_back("armoire");

    config.processes.push_back(Process{
        "do_montant",
        {{"planche", 1}},
        {{"montant", 1}},
        15
    });

    config.processes.push_back(Process{
        "do_fond",
        {{"planche", 2}},
        {{"fond", 1}},
        20
    });

    config.processes.push_back(Process{
        "do_etagere",
        {{"planche", 1}},
        {{"etagere", 1}},
        10
    });

    config.processes.push_back(Process{
        "do_armoire_ikea",
        {{"montant", 2}, {"fond", 1}, {"etagere", 3}},
        {{"armoire", 1}},
        30
    });

    return config;
}

} // namespace krpsim::mock
