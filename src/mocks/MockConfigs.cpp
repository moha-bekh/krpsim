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

Config ikeaDemo() {
  return {
    {{"planche", 7}},
    {
        {"do_montant", {{"planche", 1}}, {{"montant", 1}}, 15},
        {"do_fond", {{"planche", 2}}, {{"fond", 1}}, 20},
        {"do_etagere", {{"planche", 1}}, {{"etagere", 1}}, 10},
        {"do_armoire_ikea",
         {{"montant", 2}, {"fond", 1}, {"etagere", 3}},
         {{"armoire", 1}},
         30},
    },
    true,
    {"armoire"}};
}

Config steakDemo() {
  return {
    {{"steak_cru", 3}, {"poele", 1}},
    {
        {"cuisson_1",
         {{"steak_cru", 2}, {"poele", 1}},
         {{"steak_mi_cuit", 2}, {"poele", 1}},
         10},
        {"cuisson_2",
         {{"steak_mi_cuit", 2}, {"poele", 1}},
         {{"steak_cuit", 2}, {"poele", 1}},
         10},
        {"cuisson_3",
         {{"steak_cru", 1}, {"steak_mi_cuit", 1}, {"poele", 1}},
         {{"steak_mi_cuit", 1}, {"steak_cuit", 1}, {"poele", 1}},
         10},
        {"cuisson_4",
         {{"steak_cru", 1}, {"poele", 1}},
         {{"steak_mi_cuit", 1}, {"poele", 1}},
         10},
        {"cuisson_5",
         {{"steak_mi_cuit", 1}, {"poele", 1}},
         {{"steak_cuit", 1}, {"poele", 1}},
         10},
    },
    true,
    {"steak_cuit"}};
}

Config recreDemo() {
  return {
    {{"bonbon", 10}, {"moi", 1}},
    {
        {"manger", {{"bonbon", 1}}, {}, 10},
        {"jouer_a_la_marelle",
         {{"bonbon", 5}, {"moi", 1}},
         {{"moi", 1}, {"marelle", 1}},
         20},
        {"parier_avec_un_copain",
         {{"bonbon", 2}, {"moi", 1}},
         {{"moi", 1}, {"bonbon", 3}},
         10},
        {"parier_avec_un_autre_copain",
         {{"moi", 1}, {"bonbon", 2}},
         {{"moi", 1}, {"bonbon", 1}},
         10},
        {"se_battre_dans_la_cours",
         {{"moi", 1}},
         {{"moi", 1}, {"bonbon", 1}},
         50},
    },
    false,
    {"marelle"}};
}

Config pommeDemo() {
  return {
    {{"four", 10}, {"euro", 10000}},
    {
        {"buy_pomme", {{"euro", 100}}, {{"pomme", 700}}, 200},
        {"buy_citron", {{"euro", 100}}, {{"citron", 400}}, 200},
        {"buy_oeuf", {{"euro", 100}}, {{"oeuf", 100}}, 200},
        {"buy_farine", {{"euro", 100}}, {{"farine", 800}}, 200},
        {"buy_beurre", {{"euro", 100}}, {{"beurre", 2000}}, 200},
        {"buy_lait", {{"euro", 100}}, {{"lait", 2000}}, 200},
        {"separation_oeuf",
         {{"oeuf", 1}},
         {{"jaune_oeuf", 1}, {"blanc_oeuf", 1}},
         2},
        {"reunion_oeuf",
         {{"jaune_oeuf", 1}, {"blanc_oeuf", 1}},
         {{"oeuf", 1}},
         1},
        {"do_pate_sablee",
         {{"oeuf", 5}, {"farine", 100}, {"beurre", 4}, {"lait", 5}},
         {{"pate_sablee", 300}, {"blanc_oeuf", 3}},
         300},
        {"do_pate_feuilletee",
         {{"oeuf", 3}, {"farine", 200}, {"beurre", 10}, {"lait", 2}},
         {{"pate_feuilletee", 100}},
         800},
        {"do_tarte_citron",
         {{"pate_feuilletee", 100}, {"citron", 50}, {"blanc_oeuf", 5}, {"four", 1}},
         {{"tarte_citron", 5}, {"four", 1}},
         60},
        {"do_tarte_pomme",
         {{"pate_sablee", 100}, {"pomme", 30}, {"four", 1}},
         {{"tarte_pomme", 8}, {"four", 1}},
         50},
        {"do_flan",
         {{"jaune_oeuf", 10}, {"lait", 4}, {"four", 1}},
         {{"flan", 5}, {"four", 1}},
         300},
        {"do_boite",
         {{"tarte_citron", 3}, {"tarte_pomme", 7}, {"flan", 1}, {"euro", 30}},
         {{"boite", 1}},
         1},
        {"vente_boite", {{"boite", 100}}, {{"euro", 55000}}, 30},
        {"vente_tarte_pomme", {{"tarte_pomme", 10}}, {{"euro", 100}}, 30},
        {"vente_tarte_citron", {{"tarte_citron", 10}}, {{"euro", 200}}, 30},
        {"vente_flan", {{"flan", 10}}, {{"euro", 300}}, 30},
    },
    false,
    {"euro"}};
}

Config inceptionDemo() {
  return {
    {{"clock", 1}},
    {
        {"make_sec", {{"clock", 1}}, {{"clock", 1}, {"second", 1}}, 1},
        {"make_minute", {{"second", 60}}, {{"minute", 1}}, 6},
        {"make_hour", {{"minute", 60}}, {{"hour", 1}}, 36},
        {"make_day", {{"hour", 24}}, {{"day", 1}}, 86},
        {"make_year", {{"day", 365}}, {{"year", 1}}, 365},
        {"start_dream", {{"minute", 1}, {"clock", 1}}, {{"dream", 1}}, 60},
        {"start_dream_2", {{"minute", 1}, {"dream", 1}}, {{"dream", 2}}, 60},
        {"dream_minute",
         {{"second", 1}, {"dream", 1}},
         {{"minute", 1}, {"dream", 1}},
         1},
        {"dream_hour",
         {{"second", 1}, {"dream", 2}},
         {{"hour", 1}, {"dream", 2}},
         1},
        {"dream_day",
         {{"second", 1}, {"dream", 3}},
         {{"day", 1}, {"dream", 3}},
         1},
        {"end_dream", {{"dream", 3}}, {{"clock", 1}}, 60},
    },
    false,
    {"year"}};
}

} // krpsim::mock
