J'aime beaucoup cette nouvelle direction. Elle transforme le projet d'un simple "faire un solver" en un laboratoire d'expérimentation algorithmique :

vous construisez ensemble un moteur fiable (simulation + verifier),
puis chacun explore des stratégies différentes,
vous benchmarkez les résultats,
vous comparez les performances et fusionnez les meilleures idées.

C'est beaucoup plus proche de la manière dont on travaille sur des moteurs d'optimisation ou des algorithmes de recherche en entreprise ou en recherche.

J'ai préparé une première version du document mise à jour : krpsim.md

Ce que j'aimerais encore ajouter dans une V2

Je pense qu'on peut pousser le document encore plus loin (20-30 pages) et en faire un vrai document de conception. J'ajouterais notamment :

Architecture UML (Config, State, Event, Solver, Verifier, Parser)
Diagrammes de séquence (lancement d'un process, fin d'un process, boucle de simulation)
Liste des DSA utilisées (priority queue, graphes, maps, event queue, etc.)
Catalogue des heuristiques (Greedy, Priority, A*, Beam Search, Monte-Carlo, Branch & Bound...) avec leurs avantages/inconvénients
Jeux de données de benchmark (simple → ikea → steak → pomme → inception)
Métriques de comparaison (temps, quantité produite, nombre de cycles, complexité)
Roadmap Git (branches, PR, merge strategy)
Planning sur 2 semaines avec les objectifs de chaque journée
Idées de bonus (profiling, visualisation du graphe, statistiques des solveurs)

À mon avis, cette V2 deviendrait un véritable design document. Vous pourriez pratiquement coder tout le projet en suivant uniquement ce document, et ce serait un excellent support pour la soutenance également.