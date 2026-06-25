# Roadmap krpsim — Répartition groupe de 2

## Stratégie générale

L'idée est de ne pas commencer par le parsing.

On part d'abord sur du **full mocking** : les configurations sont créées directement dans le code avec des structs/classes.  
Cela permet de se concentrer à deux sur le vrai cœur du projet :

- simulation
- scheduling
- optimisation
- vérification
- edge cases

Le parsing sera ajouté à la fin, une fois que le moteur fonctionne correctement.

---

## Objectif de l'architecture

Le projet doit être organisé pour que le parsing soit juste une couche d'entrée.

```text
mock config
     │
     ▼
Config ──► Solver ──► Trace ──► Verifier
     ▲                         │
     │                         ▼
parser config            result / stocks
```

À terme :

```text
file.krp ──► Parser ──► Config ──► Solver ──► Trace
```

Le solver et le verifier ne doivent pas dépendre du parser.

---

## Structures communes à définir ensemble

À faire au tout début, en binôme.

```cpp
struct Process {
    std::string name;
    std::map<std::string, int> needs;
    std::map<std::string, int> results;
    int duration;
};

struct Config {
    std::map<std::string, int> initialStocks;
    std::vector<Process> processes;
    std::vector<std::string> optimizeTargets;
};

struct Event {
    int endCycle;
    std::string processName;
    std::map<std::string, int> results;
};

struct TraceAction {
    int cycle;
    std::string processName;
};

struct SimulationState {
    int cycle;
    std::map<std::string, int> stocks;
    std::vector<Event> runningEvents;
};
```

---

# Répartition en mode full mocking

## Phase 1 — Core commun

### À faire ensemble

- définir les structs
- créer 3 ou 4 configs mockées dans le code
- définir le format de trace
- décider comment afficher les stocks
- décider comment comparer deux solutions

Exemples de configs mockées :

```cpp
Config simpleDemo();
Config finiteFactory();
Config infiniteLoopFactory();
Config bottleneckFactory();
```

---

# Phase 2 — MVP à deux sans parsing

## Personne A — Simulation engine v1

Responsabilités :

- gérer les stocks
- gérer les événements en cours
- consommer les ressources au lancement d'un process
- produire les ressources à la fin d'un process
- avancer le temps au prochain événement
- savoir si un process est lançable

Fonctions typiques :

```cpp
bool canStartProcess(const SimulationState& state, const Process& process);
void startProcess(SimulationState& state, const Process& process);
void completeEventsAtCycle(SimulationState& state, int cycle);
int nextEventCycle(const SimulationState& state);
```

Objectif final de la phase :

```text
Pouvoir exécuter manuellement une trace mockée.
```

---

## Personne B — Solver naïf v1

Responsabilités :

- utiliser une Config mockée
- trouver les processus disponibles
- choisir un processus lançable
- générer une trace simple
- arrêter quand plus rien n'est possible
- produire une sortie au format attendu

Fonctions typiques :

```cpp
std::vector<const Process*> getStartableProcesses(
    const Config& config,
    const SimulationState& state
);

Trace solveNaive(const Config& config, int maxDelay);
void printTrace(const Trace& trace);
```

Objectif final de la phase :

```text
Avoir un krpsim mocké qui produit une trace valide.
```

---

# Phase 3 — Verifier sans parsing

## Personne A — Verifier v1

Responsabilités :

- prendre une Config mockée
- prendre une Trace générée ou écrite à la main
- rejouer la trace
- détecter les erreurs de ressources
- détecter les cycles invalides
- afficher les stocks finaux

Fonctions typiques :

```cpp
VerificationResult verifyTrace(const Config& config, const Trace& trace);
```

---

## Personne B — Tests de scénarios

Responsabilités :

- créer des traces valides
- créer des traces invalides
- tester les cas limites
- comparer solver et verifier
- créer plusieurs configs mockées

Cas à tester :

```text
process lancé sans ressource
process lancé avant la fin d'un autre
plusieurs process au même cycle
stock consommé plusieurs fois au même cycle
process avec durée 0 ou invalide
système fini
système infini
ressource jamais produite
ressource produite mais jamais consommée
```

---

# Phase 4 — Optimisation à deux

À partir de là, vous travaillez tous les deux sur l'algo.

## Personne A — Heuristique de score

Responsabilités :

- donner un score à chaque processus
- favoriser les processus qui produisent une ressource optimisée
- favoriser les processus utiles à d'autres processus
- pénaliser les processus longs
- pénaliser la consommation de ressources rares

Exemple :

```cpp
int scoreProcess(const Config& config, const SimulationState& state, const Process& process);
```

---

## Personne B — Stratégies multiples

Responsabilités :

- implémenter plusieurs stratégies de résolution
- tester plusieurs traces possibles
- garder la meilleure
- ajouter un peu de randomisation si utile

Exemples :

```cpp
Trace solveGreedyByScore(const Config& config, int maxDelay);
Trace solveShortestProcessFirst(const Config& config, int maxDelay);
Trace solveTargetResourceFirst(const Config& config, int maxDelay);
Trace solveRandomizedGreedy(const Config& config, int maxDelay);
Trace chooseBestTrace(const std::vector<Trace>& traces);
```

---

# Phase 5 — Swap de responsabilités

À ce stade, chacun doit relire et améliorer le code de l'autre.

## Personne A reprend

- stratégies de solver
- comparaison de solutions
- cas infinis
- choix de stop condition

## Personne B reprend

- simulation engine
- verifier
- edge cases
- cohérence des stocks

But du swap :

```text
Les deux membres comprennent tout le moteur.
Aucun bloc critique n'appartient à une seule personne.
```

---

# Phase 6 — Ajout du parsing à la fin

Une fois que le moteur fonctionne avec des Config mockées, on ajoute le parser.

## Personne A — Parser config

Responsabilités :

- lire les stocks initiaux
- lire les processus
- lire optimize
- ignorer les commentaires
- construire une Config

Fonction :

```cpp
Config parseConfigFile(const std::string& path);
```

---

## Personne B — Parser trace + intégration CLI

Responsabilités :

- lire une trace `<cycle>:<process_name>`
- brancher les arguments CLI
- connecter parser, solver et verifier
- gérer les erreurs d'utilisation

Fonctions :

```cpp
Trace parseTraceFile(const std::string& path);
int mainKrpsim(int argc, char** argv);
int mainKrpsimVerif(int argc, char** argv);
```

---

# Phase 7 — Robustesse finale

À faire ensemble.

Checklist :

- fichier inexistant
- ligne mal formatée
- quantité négative
- durée négative
- process sans nom
- ressource inconnue
- optimize vide
- trace avec process inconnu
- trace non triée
- cycle négatif
- plusieurs actions au même cycle
- système qui se termine
- système qui peut tourner longtemps
- output propre pour le correcteur

---

# Planning recommandé

## Jour 1

Ensemble :

- architecture
- structs
- mocks
- conventions

## Jours 2-3

Personne A :

- simulation engine

Personne B :

- solver naïf

## Jour 4

Ensemble :

- intégration solver + simulation
- première trace valide

## Jours 5-6

Personne A :

- verifier

Personne B :

- tests mockés + cas limites

## Jours 7-9

Les deux :

- optimisation
- heuristiques
- stratégies multiples
- comparaison de solutions

## Jour 10

Swap :

- chacun relit et améliore la partie de l'autre

## Jours 11-12

Parsing :

- config parser
- trace parser
- CLI

## Jours 13+

Finalisation :

- tests
- README
- fichiers de config perso
- nettoyage
- défense

---

# Répartition finale résumée

| Phase | Personne A | Personne B |
|---|---|---|
| Design | Structs communes | Structs communes |
| MVP | Simulation engine | Solver naïf |
| Vérification | Verifier | Tests et scénarios |
| Optimisation | Scoring | Stratégies multiples |
| Swap | Solver / stop condition | Simulation / verifier |
| Parsing | Parser config | Parser trace + CLI |
| Final | Tests + défense | Tests + défense |

---

# Avantage de cette stratégie

Cette approche est meilleure que commencer par le parsing parce que :

- vous travaillez directement sur le cœur algorithmique
- vous pouvez tester sans attendre le parser
- vous comprenez tous les deux le moteur
- le parser devient une simple couche d'entrée
- vous limitez le risque d'avoir une personne bloquée sur une partie peu intéressante
- vous obtenez rapidement une version fonctionnelle

---

# Priorité absolue

Avant d'optimiser, il faut garantir :

```text
Toute trace générée par krpsim doit être acceptée par krpsim_verif.
```

L'optimisation vient après la validité.
