# Notes de conception du simulateur / solver

## Ce qu'il faut absolument remarquer

### 1. Un process peut ne rien produire

Exemple (`recre`) :

```txt
manger:(bonbon:1)::10
```

Le champ `results` peut donc être vide.

La structure doit accepter :

```cpp
results = {};
```

---

### 2. Une ressource peut être consommée puis rendue

Exemples :

```txt
four:1 -> four:1
poele:1 -> poele:1
moi:1 -> moi:1
clock:1 -> clock:1
dream:1 -> dream:1
```

Ce sont des ressources représentant des **machines**, **outils** ou **acteurs**.

Elles sont :

- consommées au lancement du process ;
- indisponibles pendant toute sa durée ;
- rendues à la fin.

Exemple :

```txt
do_tarte_pomme:(pate_sablee:100;pomme:30;four:1):(tarte_pomme:8;four:1):50
```

Le four est bloqué pendant **50 cycles**.

---

### 3. Plusieurs process peuvent démarrer au même cycle

Si les stocks le permettent, plusieurs process peuvent être lancés simultanément.

Exemple (`ikea`) :

```txt
planche:7
```

Au cycle `0`, on peut lancer :

```txt
do_montant
do_fond
do_etagere
do_etagere
do_etagere
```

Attention à ne jamais consommer plus que les ressources disponibles.

---

### 4. Les ressources sont produites uniquement à la fin

Exemple :

```txt
0  : achat_materiel
10 : realisation_produit
40 : livraison
```

`realisation_produit` ne peut pas commencer au cycle `0` car `materiel` n'existe qu'au cycle `10`.

---

### 5. La durée d'un process est un délai, pas une date

Exemple :

```txt
achat_materiel:(euro:8):(materiel:1):10
```

- lancé au cycle `0` → termine au cycle `10`
- lancé au cycle `4` → termine au cycle `14`

---

### 6. `optimize:(time;resource)` a deux significations

Exemple :

```txt
optimize:(time;client_content)
```

Cela signifie :

- minimiser le temps d'exécution ;
- maximiser la quantité de `client_content`.

Une représentation possible :

```cpp
bool optimizeTime;
std::vector<std::string> optimizeResources;
```

---

# Analyse des fichiers

## simple

Le meilleur fichier pour commencer.

Pattern :

```txt
euro -> materiel -> produit -> client_content
```

Chaîne totalement linéaire.

À tester :

- dépendances simples
- fin normale
- optimisation du temps
- trace facile à vérifier

➡️ Premier mock recommandé.

---

## ikea

Pattern :

```txt
planche -> montant
planche -> fond
planche -> etagere

montant + fond + etagere -> armoire
```

Très bon pour tester :

- plusieurs productions possibles
- ressources limitées
- bonnes quantités à fabriquer
- assemblage final

### Piège

Une armoire nécessite :

- 2 montants
- 1 fond
- 3 étagères

Avec :

```txt
planche:7
```

Il faut exactement :

```txt
2 planches -> montants
2 planches -> fond
3 planches -> étagères

Total = 7
```

Si le solver produit trop de fonds ou de montants, il ne pourra plus fabriquer l'armoire.

---

## steak

Très bon fichier pour tester l'optimisation.

Pattern :

```txt
steak_cru
    ↓
steak_mi_cuit
    ↓
steak_cuit
```

Mais plusieurs chemins existent.

### Pièges

- `poele` est une ressource bloquante.
- Certains process cuisent **2 steaks**.
- D'autres n'en cuisent qu'un.

Avec :

```txt
steak_cru:3
poele:1
```

Une seule cuisson peut être effectuée à la fois.

Ce fichier permet de tester :

- choix optimal ;
- comparaison avec une stratégie naïve.

---

## recre

Très important pour les cas particuliers.

Patterns :

- `manger` consomme sans produire ;
- `parier` peut gagner ou perdre des bonbons ;
- `se_battre` produit des bonbons gratuitement mais prend du temps ;
- `moi` est une ressource bloquante rendue à la fin.

### Pièges

Résultat vide :

```txt
manger:(bonbon:1)::10
```

Boucle potentiellement infinie :

```txt
se_battre_dans_la_cours:(moi:1):(moi:1;bonbon:1):50
```

Ce process peut produire des bonbons à l'infini si on le répète.

➡️ Prévoir une condition d'arrêt.

---

## pomme

Le premier gros test réaliste.

Patterns :

- achat de ressources ;
- transformations intermédiaires ;
- recettes multiples ;
- ventes ;
- boucle économique ;
- outils bloquants (`four`) ;
- optimisation d'argent (`euro`).

Schéma :

```txt
euro
   ↓
ingrédients
   ↓
tartes / flans
   ↓
vente
   ↓
euro
```

Le système est potentiellement cyclique.

### Pièges

- beaucoup de ressources ;
- plusieurs produits vendables ;
- certains process rendent le four ;
- choix entre vendre directement ou fabriquer des boîtes ;
- risque de boucle infinie si l'objectif est de maximiser les euros.

À utiliser après :

- `simple`
- `ikea`
- `steak`

---

## inception

Le fichier le plus complexe.

Patterns :

```txt
clock
 ↓
second
 ↓
minute
 ↓
hour
 ↓
day
 ↓
year
```

Mais aussi :

```txt
dream
```

qui accélère la production du temps.

### Pièges majeurs

- `clock` est consommée puis rendue ;
- `dream` peut se multiplier ;
- certains process conservent `dream` ;
- optimisation de `year` ;
- nombreuses boucles et accélérations.

Ce fichier teste principalement :

- scheduler intelligent ;
- gestion des cycles ;
- ressources constantes ;
- ressources qui amplifient la production.

➡️ À garder pour la fin.

---

# Mocks recommandés

## 1. Mock linéaire

Basé sur `simple`.

```txt
A -> B -> C
```

Objectif :

- vérifier que la simulation fonctionne.

---

## 2. Mock assemblage

Basé sur `ikea`.

```txt
raw -> part1
raw -> part2

part1 + part2 -> final
```

Objectif :

- tester les choix de production.

---

## 3. Mock outil bloquant

Basé sur `steak`.

```txt
raw + machine -> cooked + machine
```

Objectif :

- vérifier qu'une machine ne peut pas être utilisée en parallèle.

---

## 4. Mock résultat vide

Basé sur `recre`.

```txt
food -> nothing
```

Objectif :

- vérifier que `results` peut être vide.

---

## 5. Mock boucle rentable

Basé sur `pomme`.

```txt
money
  ↓
material
  ↓
product
  ↓
money
```

Objectif :

- tester l'optimisation à long terme.

---

## 6. Mock système infini

Basé sur `recre` ou `inception`.

```txt
worker -> worker + resource
```

Objectif :

- vérifier les conditions d'arrêt.

---

# Structures recommandées

```cpp
using StockMap = std::unordered_map<std::string, int>;

struct Process {
    std::string name;
    StockMap needs;
    StockMap results;
    int duration;
};

struct Config {
    StockMap initialStocks;
    std::vector<Process> processes;
    bool optimizeTime;
    std::vector<std::string> optimizeResources;
};

struct RunningProcess {
    std::string name;
    int startCycle;
    int endCycle;
    StockMap results;
};

struct TraceAction {
    int cycle;
    std::string processName;
};

struct State {
    int cycle;
    StockMap stocks;
    std::vector<RunningProcess> running;
};
```

> **Conseil :** utiliser `long long` plutôt que `int` pour éviter les débordements sur les gros nombres de cycles ou de ressources.

---

# Règles de simulation

À chaque cycle :

1. terminer les process arrivant à échéance ;
2. ajouter leurs résultats au stock ;
3. lancer les nouveaux process ;
4. consommer immédiatement leurs besoins ;
5. enregistrer les process en cours.

Cela permet par exemple de lancer :

```txt
process_B
```

au cycle `10` si `process_A` termine également au cycle `10`.

---

# Pièges classiques

## Ne pas supprimer les stocks à 0

Même lorsqu'une ressource tombe à zéro, il peut être utile de la conserver dans la map pour l'affichage.

---

## Ne jamais produire au lancement

Les résultats sont ajoutés uniquement à :

```txt
startCycle + duration
```

---

## Ne pas libérer trop tôt les ressources rendues

Exemple :

```txt
four:1 -> four:1
```

Le four est indisponible pendant toute la durée du process.

---

## Ne pas supposer qu'un process produit toujours quelque chose

Exemple :

```txt
manger
```

Produit :

```cpp
results = {};
```

---

## Ne pas supposer qu'un process est toujours intéressant

Exemple :

```txt
parier_avec_un_autre_copain
```

consomme :

```txt
2 bonbons
```

et ne rend que :

```txt
1 bonbon
```

Selon l'objectif, il peut être contre-productif.

---

## Ne pas lancer naïvement tous les process possibles

Dans `ikea`, produire trop de mauvaises pièces peut empêcher la fabrication de l'armoire finale.

---

# Plan de tests recommandé

Créer les mocks suivants :

```cpp
Config mockSimple();
Config mockIkea();
Config mockSteak();
Config mockRecre();
Config mockPommeSmall();
Config mockInfinite();
```

Puis les tester dans cet ordre :

```txt
mockSimple   -> doit terminer correctement
mockIkea     -> doit produire une armoire
mockSteak    -> doit optimiser steak_cuit
mockRecre    -> doit gérer résultat vide + boucle
mockPomme    -> gros test réaliste
inception    -> boss final
```

---

# Règle la plus importante

> **Le solver peut être imparfait, mais le simulateur / vérificateur doit être strict, déterministe et fiable.**