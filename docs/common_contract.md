# Contrat commun

Ce fichier est l'accord commun entre la Personne A et la Personne B.

## Vocabulaire

- `Config` : description complete du probleme.
- `Process` : une action possible.
- `SimulationState` : cycle actuel, stocks actuels, evenements en cours.
- `Event` : process deja lance, mais pas encore termine.
- `TraceAction` : lancement d'un process dans la trace de sortie.
- `Trace` : liste ordonnee des lancements de process.

## Regles centrales

Quand un process demarre :

1. Toutes les ressources dans `needs` sont retirees immediatement des stocks.
2. Rien de `results` n'est ajoute immediatement.
3. Un `Event` est cree a `state.cycle + process.duration`.
4. Le solver enregistre une action dans la trace : `<cycle>:<processName>`.

Quand un evenement se termine :

1. Toutes les ressources dans `results` sont ajoutees aux stocks.
2. L'evenement est retire de `runningEvents`.

Les outils reutilisables comme `four`, `poele`, `moi`, `clock` ou `dream` ne sont pas
des cas speciaux dans le code. Ils sont consommes dans `needs` et rendus dans
`results`.

## Conventions importantes

- Un stock absent signifie une quantite de `0`.
- Les quantites et les cycles utilisent `long long`.
- `duration` doit etre superieure ou egale a `0`.
- Un `results` vide est valide.
- Plusieurs process peuvent demarrer au meme cycle.
- Plusieurs evenements peuvent se terminer au meme cycle.
- Les ressources produites au cycle `N` peuvent etre utilisees par des process qui
  demarrent aussi au cycle `N`.

## Repartition initiale

La Personne A s'occupe d'abord du simulateur et du verifier :

- `makeInitialState`
- `canStartProcess`
- `startProcess`
- `completeEventsAtCycle`
- `nextEventCycle`
- `verifyTrace`

La Personne B s'occupe d'abord du solver et des scenarios :

- `getStartableProcesses`
- `solveNaive`
- strategies de solver
- tests sur scenarios mockes
- plus tard, parser de trace et integration CLI
