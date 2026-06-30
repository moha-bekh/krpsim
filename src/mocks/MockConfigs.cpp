#include "krpsim/MockConfigs.hpp"

// struct Config {
//     StockMap initialStocks;
//     std::vector<Process> processes;
//     bool optimizeTime = false;
//     std::vector<std::string> optimizeResources;
// };

namespace krpsim::mock {

Config simpleDemo() {
  return {
    {{"euro", 10}},
    {
        {"achat_materiel", {{"euro", 8}}, {{"materiel", 1}}, 10},
        {"realisation_produit", {{"materiel", 1}}, {{"produit", 1}}, 30},
        {"livraison", {{"produit", 1}}, {{"client_content", 1}}, 20},
    },
    true,
    {"client_content"}};
}

} // krpsim::mock
