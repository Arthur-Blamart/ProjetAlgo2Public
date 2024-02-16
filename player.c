// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand
#include <stdio.h> // printf

// program header file
#include "bomberman.h"

// global declarations
extern const char BOMBERMAN; // ascii used for bomberman
extern const char WALL; // ascii used for the walls
extern const char BREAKABLE_WALL; // ascii used for the breakable walls
extern const char PATH; // ascii used for the pathes
extern const char EXIT; // ascii used for the EXIT
extern const char BOMB; // ascii used for a bomb
extern const char BOMB_BONUS; // ascii used for the bombs
extern const char FLAME_BONUS; // ascii used for the flames
extern const char FLAME_ENEMY; // ascii used for the flame ennemy
extern const char GHOST_ENEMY; // ascii used for the ghost ennemy

extern const int BOMB_DELAY; // time before a bomb explode
extern const int BREAKABLE_WALL_SCORE; // reward for bombing a breakable wall
extern const int FLAME_ENEMY_SCORE; // reward for bombing a flame enemy
extern const int GHOST_ENEMY_SCORE; // reward for bombing a ghost enemy 
extern const int BOMB_BONUS_SCORE; // reward for a bomb bonus, which increases by 1 the number of bombs that can be dropped at a time
extern const int FLAME_BONUS_SCORE; // reward for a flame bonus, which increses by 1 the explosion range of bombs
extern const int VIEW_DISTANCE; // bomberman view distance 

extern bool DEBUG; // indicates whether the game runs in DEBUG mode

const char * binome="Arthur"; // student name goes here

// prototypes of the local functions/procedures
void printAction(action);
void printBoolean(bool);

//maillonAction pour lsite chainée d'ACTIONS
typedef struct maillonAction{
  action valeur;
  struct maillonAction* next;
}maillonAction;

//Sert à décrire un objet par on caractère, quel fils prendre à partir de la racine pour le rejoindre et sa distance au joueur.
typedef struct triplet{
  char quoi;
  maillonAction* chemin;
  int distance;
}triplet;

//maillonTriplet pour liste chainée DE TRIPLETS
typedef struct maillonTriplet{
  triplet valeur;
  struct maillonTriplet* next;
}maillonTriplet;

/*Fonctions d'action*/

//Renvoit une liste chainée de tous les triplets objets de l'arbre
maillonTriplet* getObject(tree arbre);
//Permet de "mettre" à jour une liste
void misAjourGetObject(maillonTriplet* liste, action direction);
//permet de choisir quel action faire en fonction de la liste d'ojet
action chooseAction(tree map, maillonTriplet* listeObjet, int explosion_range, int remaining_bombs, action last_action);
//Permet de déterminer si une bombe posé maintenant toucherais l'objet donné
bool isBombingUsefulOnObject(triplet objet, int explosion_range);
//permet de determiner si une position est hors de danger, que ca soit de bombe ou d'enemies
bool isPosSafe(tree map, bool isJoueur, int explosion_range, action last_action);
//Permet de determiner si le joueur est dans le radius d'une bombe
bool isPosInBombRadius(tree arbre, bool isJoueur, int explosion_range, action last_action);
//Es ce qu'un enemie est trop proche du joueur
bool isEnemyTooclose(tree map);
//Es ce qu'en posant une bombe à cet instant on pourrait toucher un enemie
bool isAnEnemyKillable(tree map, int explosion_range, int remaining_bombs);
//Es ce qu'en explorant la map à partir de map jusqu'à distance on va trouver au moins un enemie
bool isEnemyInRange(tree map, int distance);

//Permet d'obtenir une liste avec toutes les bombes de la listeObj
maillonTriplet* getBombList(maillonTriplet* listeObj);
//Permet d'obtenir une liste avec tous les enemies de la listeObj
maillonTriplet* getEnemyList(maillonTriplet* listeObj);
//On coupe tout les objets qui sont mit en danger par des bombes ou des enemies de la liste pour avoir une nouvelle liste
maillonTriplet* cutListOfDanger(maillonTriplet* listeBomb, maillonTriplet* listeEnemy, maillonTriplet* listeObj, int explosion_range);
//Fonction qui permet de savoir si un objet a besoin d'être coupé de la liste d'objet car mit par en danger par une des bombes de la liste bombe
bool doWeNeedToCutItCauseBomb(triplet objet, maillonTriplet* listBomb, int explosion_range);
//même que précédente mais pour les enemies
bool doWeNeedToCutItCauseEnemy(triplet objet, maillonTriplet* listeEnemy);
//Permet de déterminer si liste1 est préfixe de liste 2
bool isListeActionPrefixe(maillonAction* liste1, maillonAction* liste2);
//Permet de raccourcir **à** size la liste en entrée, (renvoit un nouveau pointeur)
maillonAction* shortenList(maillonAction* liste, int size);
//Permet de fuir le danger et potentiellement aller vers un objet
action fleeDanger(tree map, int explosion_range, maillonTriplet* listeObj);
//Permet d'aller vers l'objet safe le plus proche
action getToSafestObject(tree map, maillonTriplet* listeObj, int explosion_range);
//donne une action qui permetteras d'aller vers au moins un endroit safe
action findSafePlace(tree map, int explosion_range);
//Quand on détecte une bombe à nos pieds on veut savoir lequel de N,S,W,E nous amène au plus de cases (sans passer par des monstres ou bombe)
action wherePlusDeCases(tree map, int explosion_range, action last_action);
//Nous donne le nb de cases "safe" dans un arbre, heuristique intéressante
int howManyCase(tree map);

/*Fonctions de gestion de liste chainée DE TRIPLET*/
//Renvoit une liste vide
maillonTriplet* listeTripletVide();
//Ajout en tête d'un triplet dans une liste chainée
maillonTriplet* ajoutListeTriplet(maillonTriplet* liste, triplet val);
//Concatène 2 listes chainée
maillonTriplet* concatenationListeTriplet(maillonTriplet* liste1, maillonTriplet* liste2);
//Libere récursivement la mémoire allouée pour une liste chainée
void libereListeTriplet(maillonTriplet* liste);

/*Fonctions de gestion de liste chainée DE TRIPLET*/
//Renvoit une liste chainée d'ation vide
maillonAction* listeActionVide();
//Ajoute (en tête) val dans liste et renvoit la nouvelle liste
maillonAction* ajoutListeAction(maillonAction* liste, action val);
//Permet de renvoyer une copie d'un maillon en entrée
maillonAction* copieMaillonAction(maillonAction* m);
//permet d'obtenir la taille d'une liste d'action
int tailleListeAction(maillonAction* liste);
//libère récursivement une lsite d'action
void libereListeAction(maillonAction* liste);

/*Fonctions d'identification*/
//Compte le nombre de fois où la derniere action d'un chemin est répétée
int countLastAction(maillonAction* liste);
//obtenir le fils de a dans une direction
tree getSonInDirection(tree a, action direction);
//Renvoit le maximum ente deux entiers
int max(int a, int b);
//Renvoit le min entre 2 entiers
int min(int a, int b);
//Es ce qu'un objet est considéré pertinent
bool IsObjetInteressant(char objet);
//Es ce que le caractere représente un enemie
bool isEnemy(char objet);
//Es ce que le caractere représente un bonus
bool isBonus(char objet);
//Es ce que le caractere représente un mur
bool isWall(char objet);
//Es ce que le caractere représente un danger (enemie+bomb)
bool isDanger(char objet);
//Es ce qu'on peut marcher sur un objet sans que ca soit une action qui nous tue directement
bool isWalkable(char objet);

/*Fonctions d'affichages*/
//Affiche les triplets contenue dans une liste
void AfficheListeObjet(maillonTriplet* liste);
//Affiche les actions, sert pour AfficheListeObjet
void afficheListeAction(maillonAction* liste);
//Affiche toutes les distances d'une liste
void afficheDistances(maillonTriplet* liste);
//Permet de demander quel action faire
action controlable(int remaining_bombs);

/*
  bomberman function:
  This function randomly select a valid move for BOMBERMAN based on its current position on the game map.
 */
action bomberman(
		 tree map, // 4-ary tree of char modeling a subset of the game map
		 action last_action, // last action made, -1 in the be beginning 
		 int remaining_bombs, // number of bombs
		 int explosion_range // explosion range for the bombs 
		 ) {
  action a; // action to choose and return


bool amIInDanger = !isPosSafe(map, true, explosion_range, last_action);

if(!amIInDanger){//Attention le fait de ne pas être dans le radius est nécessaire à l'execution de shortenList utilisé nottament dans cutList
  maillonTriplet* listeObj = getObject(map);
  maillonTriplet* listeBomb = getBombList(listeObj);
  maillonTriplet* listeEnemy = getEnemyList(listeObj);
  //bool amIinDanger = isPosInBombRadius(map, explosion_range, last_action);
  bool areThereBombInSight = (listeBomb != NULL);//permet de savoir si il y a une bombe visible
  bool areThereEnemyInSight = (listeEnemy != NULL);
  if(areThereBombInSight || areThereEnemyInSight){
  //Si il y a des bombes on enlève les objets mits en dangers par celle ci
    listeObj = cutListOfDanger(listeBomb, listeEnemy, listeObj, explosion_range);
  }
  //Si il reste des objets après les différentes coupure (ou aucune) ont choisit quoi faire à partir de la liste objet
  if(listeObj != NULL)
    a = chooseAction(map, listeObj, explosion_range, remaining_bombs, last_action);
  //Si il ne reste plus d'objet pertinent à aller voir on utilise l'heuristique
  else
    a = wherePlusDeCases(map, explosion_range, last_action);

  libereListeTriplet(listeObj);
}
//Si on est en danger on utilise l'heuristique pour fuir
else{
  a = wherePlusDeCases(map, explosion_range, last_action);
}

  return a; // answer to the game engine
}

/*
  printAction procedure:
  This procedure prints the input action name on screen.
 */
void printAction(action a) {
  switch(a) {
  case BOMBING:
    printf("BOMBING");
    break;
  case NORTH:
    printf("NORTH");
    break;
  case EAST:
    printf("EAST");
    break;
  case SOUTH:
    printf("SOUTH");
    break;
  case WEST:
    printf("WEST");
    break;
  }
}


/*
  printBoolean procedure:
  This procedure prints the input boolan value on screen.
 */
void printBoolean(bool b) {
  if(b==true) {
    printf("true");
  }
  else {
    printf("false");
  }
}

maillonTriplet* getObject(tree arbre){
  maillonTriplet* newListe = listeTripletVide();

  //Si l'arbre n'est pas vide on pourras peut etre trouver des trucs intéressant
  if(arbre != NULL){
    maillonTriplet* filsNord = getObject(arbre->n);
    maillonTriplet* filsSud = getObject(arbre->s);
    maillonTriplet* filsOuest = getObject(arbre->w);
    maillonTriplet* filsEst = getObject(arbre->e);
    misAjourGetObject(filsNord, NORTH);
    misAjourGetObject(filsSud, SOUTH);
    misAjourGetObject(filsOuest, WEST);
    misAjourGetObject(filsEst, EAST);

    if(IsObjetInteressant(arbre->c)){
      triplet newItem = (triplet){
        .quoi = arbre->c,
        .chemin = listeActionVide(),
        .distance = 0
      };
      newListe = ajoutListeTriplet(newListe, newItem);
    }
    newListe = concatenationListeTriplet(newListe, filsNord);
    newListe = concatenationListeTriplet(newListe, filsSud);
    newListe = concatenationListeTriplet(newListe, filsOuest);
    newListe = concatenationListeTriplet(newListe, filsEst);
  }

  return newListe;
}

void misAjourGetObject(maillonTriplet* liste, action direction){
  //Le but est d'incrémenter toutes les distances de la liste de 1 et de mettre a jour leurs chemins
  if(liste != NULL){
    liste->valeur.chemin = ajoutListeAction(liste->valeur.chemin, direction);
    liste->valeur.distance++;
    misAjourGetObject(liste->next, direction);
  }
}

action chooseAction(tree map, maillonTriplet* listeObjet, int explosion_range, int remaining_bombs, action last_action){
  /*
  Les critères pour déterminer l'objet le plus important sont :
    -Qu'es ce que c'est Bonus > enemie tuable > Mur_cassable (cad BOMBING)> Sortie > Mur
    -La distance 
    */
  action res;//Sauvegarde l'action choisit, sert pour différencier un choix de mur cassable (cad à notre porté) ou non
  maillonTriplet* currentInObjet = listeObjet;//Par utilisation n'est pas null
  triplet currentObjet;//Le triplet de l'objet qu'on regarderas à chaque tours
  triplet candidat = currentInObjet->valeur;//initialisation candidat pour être l'objet qu'on va vouloir chercher

  //On initialise l'action résultante
  if(candidat.quoi == BREAKABLE_WALL && (remaining_bombs>0) && isBombingUsefulOnObject(candidat, explosion_range))
    res = BOMBING;
  //Permet d'éviter le cas où c'est un mur auquel on est déja collé, (sinon on fonce dedans)
  else if(candidat.quoi == BREAKABLE_WALL && candidat.distance<=1)
    res = wherePlusDeCases(map, explosion_range, last_action);
  else
    res = candidat.chemin->valeur;

  currentInObjet = currentInObjet->next;
  while(currentInObjet != NULL){
    currentObjet = currentInObjet->valeur;
    //On vérifie si les deux objets à comparer sont de même nature
    if(candidat.quoi == currentObjet.quoi){
      //Si les deux objets sont de même nature on va préférer le plus proche, !!!! DANS LE CAS où les deux objets ne sont pas des murs
      if(candidat.quoi!=BREAKABLE_WALL && (candidat.distance > currentObjet.distance)){
        candidat = currentObjet;
        res = candidat.chemin->valeur;
      }
      else if(currentObjet.quoi == BREAKABLE_WALL){
        //Si on as un nouvel objet cassable, qu'on peut le casser d'ici et que le candidat n'étais pas cassable (cad res!=BOMBING) alors on le prend
        if(res!=BOMBING && (remaining_bombs>0) && isBombingUsefulOnObject(currentObjet, explosion_range)){
          candidat = currentObjet;
          res = BOMBING;
        }
        //Sinon on prend le plus court avec dst>1 pour pas foncer desn un mur
        else if(!isBombingUsefulOnObject(currentObjet, explosion_range) && currentObjet.distance>1){
          if(candidat.distance>currentObjet.distance && currentObjet.distance>1){
            candidat = currentObjet;
            res = candidat.chemin->valeur;
          }
        }
      }
    }
    //Si le meilleur précédent n'est pas un bonus alors on peut trouver mieux (on sait de plus que les 2 obj sont !=)
    else if(!isBonus(candidat.quoi)){
      if(isBonus(currentObjet.quoi)){
        candidat = currentObjet;
        res = candidat.chemin->valeur;
      }
      //On regarde si on as pas trouvé un mur cassable
      else if(currentObjet.quoi == BREAKABLE_WALL && (remaining_bombs>0) &&isBombingUsefulOnObject(currentObjet, explosion_range)){
        candidat = currentObjet;
        res = BOMBING;
      }
      //Attention on préfère exploser un mur que prendre une sortie
      else if(currentObjet.quoi==EXIT && res!=BOMBING){
        candidat = currentObjet;
        res = candidat.chemin->valeur;
      }
    }
    currentInObjet = currentInObjet->next;
  }

  if(!isBonus(candidat.quoi) && isAnEnemyKillable(map, explosion_range, remaining_bombs))
    res = BOMBING;
  return res;
}

bool isBombingUsefulOnObject(triplet objet, int explosion_range){
  /*Pour savoir si ca sera utile il faut regarder le chemin*/
  bool res = true;//On l'initialise a true parce que plus simple pour arreter le while suivant
  //D'abord on regarde si l'objet n'est pas trop loin
  if(objet.distance > explosion_range){
    res = false;
  }
  //Si l'objet est a porté on regarde si son chemin est une ligne droite
  else{
    maillonAction* currentInChemin = objet.chemin;
    action firstSeen = currentInChemin->valeur;
    currentInChemin = currentInChemin->next;
    while(currentInChemin!=NULL && res==true){//Si res!=true c'est parce qu'on as trouvé un virage dans le chemin
      if(currentInChemin->valeur!=firstSeen){
        res = false;
      }
      currentInChemin = currentInChemin->next;
    }
  }
  return res;
}

bool isPosSafe(tree map, bool isJoueur, int explosion_range, action last_action){
  bool res = !isPosInBombRadius(map, isJoueur, explosion_range, last_action)&&(!isEnemyTooclose(map));
  return res;
}

bool isPosInBombRadius(tree arbre, bool isJoueur, int explosion_range, action last_action){
  bool res;

  //la valeur par défaut doit changer si on regarde un joueur ou un objet
  if(isJoueur)
    res = last_action==BOMBING;
  else{
    res = !isWalkable(arbre->c);
  }

  tree currentNord = arbre;
  tree currentSouth = arbre;
  tree currentEast = arbre;
  tree currentWest = arbre;
  int i = 1;
  //On s'arrête quand on a trouvé la bombe ou quand on à été voir assez loin
  while(i<=explosion_range+1){
    if(currentNord!=NULL){
      res |= (currentNord->c==BOMB);
      currentNord = currentNord->n;
    }
    if(currentSouth!=NULL){
      res |= currentSouth->c==BOMB;
      currentSouth = currentSouth->s;
    }
    if(currentEast!=NULL){
      res |= currentEast->c==BOMB;
      currentEast = currentEast->e;
    }
    if(currentWest!=NULL){
      res |= currentWest->c==BOMB;
      currentWest =currentWest->w;
    }

    i++;
  }
  return res;
}

bool isEnemyTooclose(tree map){
  //distanceMax représente la distance max à laquelle on veut se tenir des enemies, par exemples si c'est 1 on ne veut juste pas qu'il soit collés
  int distanceMax = 3;
  bool res = isEnemyInRange(map, distanceMax);
  return res;
}

bool isAnEnemyKillable(tree map, int explosion_range, int remaining_bombs){
  int distanceToSearch = explosion_range+BOMB_DELAY-2;
  bool res = (remaining_bombs>0)&&isEnemyInRange(map, distanceToSearch);
  return res;
}

bool isEnemyInRange(tree map, int distance){
  bool res;
  //Cas de base, on as pas trouvé d'enemie
  if(distance==0 || map==NULL)
    res = false;
  else{
    //on regarde à la position actuelle et si on as pas trouvé on regarde dans les fils
    res = isEnemy(map->c);
    if(!res){
      res |= isEnemyInRange(getSonInDirection(map, NORTH), distance-1);
      res |= isEnemyInRange(getSonInDirection(map, SOUTH), distance-1);
      res |= isEnemyInRange(getSonInDirection(map, WEST), distance-1);
      res |= isEnemyInRange(getSonInDirection(map, EAST), distance-1);
    }
  }
  return res;
}

maillonTriplet* getBombList(maillonTriplet* listeObj){
  maillonTriplet* new;
  if(listeObj != NULL){
    if(listeObj->valeur.quoi == BOMB){
      new = (maillonTriplet*)malloc(sizeof(maillonTriplet));
      new->valeur = listeObj->valeur;
      new->next = getBombList(listeObj->next);
    }
    else{
      new = getBombList(listeObj->next);
    }
  }
  else{
    new = listeTripletVide();
  }
  return new;
}

maillonTriplet* getEnemyList(maillonTriplet* listeObj){
  maillonTriplet* new;
  if(listeObj != NULL){
    if(isEnemy(listeObj->valeur.quoi)){
      new = (maillonTriplet*)malloc(sizeof(maillonTriplet));
      new->valeur = listeObj->valeur;
      new->next = getEnemyList(listeObj->next);
    }
    else{
      new = getEnemyList(listeObj->next);
    }
  }
  else{
    new = listeTripletVide();
  }
  return new;
}

maillonTriplet* cutListOfDanger(maillonTriplet* listeBomb, maillonTriplet* listeEnemy, maillonTriplet* listeObj, int explosion_range){
  maillonTriplet* lastGood = listeTripletVide();
  maillonTriplet* currentInObjet = listeObj;
  maillonTriplet* temp;
  maillonTriplet* temp2;
  triplet currentObjet;
  while(currentInObjet != NULL){
    currentObjet = currentInObjet->valeur;
    if(doWeNeedToCutItCauseBomb(currentObjet, listeBomb, explosion_range) || doWeNeedToCutItCauseEnemy(currentObjet, listeEnemy)){
      //On coupe le maillon en faisant un free mais attention de pas perdre ke reste
      temp = currentInObjet->next;
      free(currentInObjet);
      currentInObjet = temp;
    }
    //On veut garder ce maillon
    else{
      //On, il est le nouveau lastGood, on lui met les précédent bon en queu tout et on ne perd pas le maillon suivant
      temp = lastGood;
      temp2 = currentInObjet->next;

      lastGood = currentInObjet;
      lastGood->next = temp;

      currentInObjet = temp2;
    }
  }


  return lastGood;
}

bool doWeNeedToCutItCauseBomb(triplet objet, maillonTriplet* listBomb, int explosion_range){
  bool res = false;

  maillonTriplet* currentInBomb = listBomb;
  triplet currentBomb;
  maillonAction* prefixeToCheck;
  //On regarde sur toutes les bombes si cet objet n'est pas mit en danger
  while(currentInBomb != NULL && !res){
    currentBomb = currentInBomb->valeur;
    prefixeToCheck = shortenList(currentBomb.chemin, currentBomb.distance-explosion_range);//dst-exp_range>0 par construction
    //On regarde si cet objet est mit en danger par la bombe courante.
    res = isListeActionPrefixe(prefixeToCheck, objet.chemin);
    currentInBomb = currentInBomb->next;
  }

  return res;
}

bool doWeNeedToCutItCauseEnemy(triplet objet, maillonTriplet* listeEnemy){
  bool res = false;

  maillonTriplet* currentInEnemy = listeEnemy;
  triplet currentEnemy;
  maillonAction* prefixeToCheck;
  while(currentInEnemy != NULL && !res){
    currentEnemy = currentInEnemy->valeur;
    prefixeToCheck = shortenList(currentEnemy.chemin, currentEnemy.distance-1);
    res = isListeActionPrefixe(prefixeToCheck, objet.chemin);
    currentInEnemy = currentInEnemy->next;
  }

  return res;
}


bool isListeActionPrefixe(maillonAction* liste1, maillonAction* liste2){
  bool res;
  if(liste1 != NULL && liste2 != NULL){
    if(liste1->valeur == liste2->valeur){
      res = isListeActionPrefixe(liste1->next, liste2->next);
    }
    else{
      res = false;
    }
  }
  //Si une des deux listes se termine, on a faux uniquement si liste2 s'est terminé et pas liste1
  else{
    res = !(liste1!=NULL);
  }
  return res;
}

maillonAction* shortenList(maillonAction* liste, int size){
  maillonAction* res;
  if(size<=0 || liste==NULL){
    res = listeActionVide();
  }
  else{
    res = copieMaillonAction(liste);
    res->next = shortenList(liste->next, size-1);
  }
  return res;
}

action fleeDanger(tree map, int explosion_range, maillonTriplet* listeObj){
  action res = BOMBING;
  if(listeObj != NULL){
    //On va vouloir choisir un endroit soit assez loin en ligne droite soit qui tourne
    res = getToSafestObject(map, listeObj, explosion_range);
  }
  //Si on as pas pu trouver un chemin avec un objet on doit décider autre chose
  if(res == BOMBING)
    res = findSafePlace(map, explosion_range+1);//+1 pour la sécurité d'avoir 2 cases pour psa se bloquer
  
  return res;
}

action getToSafestObject(tree map, maillonTriplet* listeObj, int explosion_range){
  action res = BOMBING;
  
  int i;//Va servir a compter la profondeur des chemins en ligne droite
  action firstSeen;
  maillonTriplet* currentInObjet = listeObj;
  maillonAction* currentInPath;
  while(currentInObjet != NULL && res==BOMBING){
    currentInPath = currentInObjet->valeur.chemin;
    firstSeen = currentInPath->valeur;
    do{
    /*!!!!!!!! peut poser des soucie ???????,,,*/
      currentInPath = currentInPath->next;
      //Si la condition suivante est vraie, alors c'est qu'il y a un virage qui permet de se mettre hors de porté de la bombe
      if(currentInPath->valeur != firstSeen){
        int taille = tailleListeAction(currentInPath);
        //Si il y a au moins deux cases sans danger avant l'objet ou une case et que l'objet n'est pas un mur (on peut donc marcher dessus)
        if(taille >3 || (taille>2 && !isWall(currentInObjet->valeur.quoi))){
          res = firstSeen;
        }
      }
      i++;
    }
    while(res!=BOMBING && currentInPath != NULL);
    if(res!=BOMBING && i>explosion_range)//permet de savoir si on est aller assez loin tout droit
      res = firstSeen;
    
    currentInObjet = currentInObjet->next;
  }
  return res;
}

action findSafePlace(tree map, int explosion_range){
  /*On oublie pas qu'on pars du principe qu'on est en danger, le but c'est pas d'aller sur une bombe
  on essaye de trouver le plus simple pour fuir
  utilisation de parcours backtracking ?*/
  action res;
  if(map == NULL || isDanger(map->c)){
    if(explosion_range == 0)
      res = NORTH;
    else
      res = BOMBING;
  }
  else{
    if(findSafePlace(map->n, explosion_range-1) != BOMBING)
      res = NORTH;
    else if(findSafePlace(map->s, explosion_range-1) != BOMBING)
      res = SOUTH;
    else if(findSafePlace(map->w, explosion_range-1) != BOMBING)
      res = WEST;
    else if(findSafePlace(map->e, explosion_range-1) != BOMBING)
      res = EAST;
  }
  return res;
}

action wherePlusDeCases(tree map, int explosion_range, action last_action){
  /*
    Le but est de trouver la direction qui amènera au plus de cases,
    on priorise d'abord les case qui ne sont pas mise en danger, cad on regarde d'abord si il existe des cases "safe"
    sinon on prend juste la direction aumène au plus de case  
  */

  action res = BOMBING;//Valeur pardéfaut qui nous permetteras de vérifier que la cas où on veut aller est safe
  
  bool isNordSafe = isPosSafe(getSonInDirection(map, NORTH), false, explosion_range, last_action);
  bool isSouthSafe = isPosSafe(getSonInDirection(map, SOUTH), false, explosion_range, last_action);
  bool isWestSafe = isPosSafe(getSonInDirection(map, WEST), false, explosion_range, last_action);
  bool isEastSafe = isPosSafe(getSonInDirection(map, EAST), false, explosion_range, last_action);

  //Nous serviras a savoir si on doit chercher une direction safe ou non
  bool isThereAnyPosSafe = isNordSafe||isSouthSafe||isWestSafe||isEastSafe;


  int nbNorth = howManyCase(map->n);
  int nbSouth = howManyCase(map->s);
  int nbWest = howManyCase(map->w);
  int nbEast = howManyCase(map->e);

  int nbMaxDeCases;

  if(!isThereAnyPosSafe){
    nbMaxDeCases = max(nbNorth, max(nbSouth, max(nbWest, nbEast)));
    if(nbNorth==nbMaxDeCases)
      res = NORTH;
    else if(nbSouth==nbMaxDeCases)
      res = SOUTH;
    else if(nbWest==nbMaxDeCases)
      res = WEST;
    else
      res = EAST;
  }
  else{
    int seuilRandom = 45;
    //déplacement random si rien d'intéressant
    if(min(nbNorth, min(nbSouth, min(nbEast, min(nbWest, seuilRandom))))==seuilRandom){
      res = (rand()%4)+1;
    }
    else{
      nbMaxDeCases = 0;
      if(isNordSafe && nbMaxDeCases<nbNorth){
        nbMaxDeCases = nbNorth;
        res = NORTH;
      }

      if(isSouthSafe && nbMaxDeCases<nbSouth){
        nbMaxDeCases = nbSouth;
        res = SOUTH;
      }
      
      if(isWestSafe && nbMaxDeCases<nbWest){
        nbMaxDeCases = nbWest;
        res = WEST;
      }
      
      if(isEastSafe && nbMaxDeCases<nbEast){
        nbMaxDeCases = nbEast;
        res = EAST;
      }
    }
  }

  return res;
}

int howManyCase(tree map){
  int res = 0;
  if(map != NULL){
    if(!isDanger(map->c)){
      res = 1 + howManyCase(map->n)+howManyCase(map->s)+howManyCase(map->w)+howManyCase(map->e);
    }
  }
  return res;
}

maillonTriplet* listeTripletVide(){
  return NULL;
}
maillonTriplet* ajoutListeTriplet(maillonTriplet* liste, triplet val){
  //Attention il s'agit bien d'un ajout en tête il renvoit donc un nouveau pointeur
  maillonTriplet* new = (maillonTriplet*)malloc(sizeof(maillonTriplet));
  new->next = liste;
  new->valeur = val;
  return new;
}
maillonTriplet* concatenationListeTriplet(maillonTriplet* liste1, maillonTriplet* liste2){
  maillonTriplet* listeResultante;
  maillonTriplet* tmp = liste1;

  if(liste1 == NULL){
    listeResultante = liste2;
  }
  else{
    //Permet d'obtenir le dernier maillonTriplet de liste1 (on as deja verifier que liste1 non null)
    while(tmp->next != NULL){
      tmp = tmp->next;
    }
    tmp->next = liste2;
    listeResultante = liste1;
  }

  //Le retour du pointeur n'étais pas absolument nécessaire met permet d'éviter de bêtes erreurs
  return listeResultante;
}
void libereListeTriplet(maillonTriplet* liste){
  if(liste != NULL){
    libereListeTriplet(liste->next);
    libereListeAction(liste->valeur.chemin);
    free(liste);
  }
}


maillonAction* listeActionVide(){
  return NULL;
}
maillonAction* ajoutListeAction(maillonAction* liste, action val){
  //Attention il s'agit bien d'un ajout en tête il renvoit donc un nouveau pointeur
  maillonAction* new = (maillonAction*)malloc(sizeof(maillonAction));
  new->next = liste;
  new->valeur = val;
  return new;
}
maillonAction* copieMaillonAction(maillonAction* m){
  maillonAction* new = (maillonAction*)malloc(sizeof(maillonAction));
  new->valeur = m->valeur;
  new->next = NULL;
  return new;
}
int tailleListeAction(maillonAction* liste){
  if(liste != NULL)
    return tailleListeAction(liste->next)+1;
  else
    return 0;
}

void libereListeAction(maillonAction* liste){
  if(liste != NULL){
    libereListeAction(liste->next);
    free(liste);
  }
}

int countLastAction(maillonAction* liste){
  int total = 1;
  action last_found = liste->valeur;
  liste = liste->next;
  while(liste != NULL){
    if(liste->valeur == last_found){
      total++;
    }
    else{
      last_found = liste->valeur;
      total = 1;
    }
    liste = liste->next;
  }

  return total;
}

tree getSonInDirection(tree a, action direction){
  tree res;
  switch(direction){
    case NORTH:
      res = a->n;
      break;
    case SOUTH:
      res = a->s;
      break;
    case EAST:
      res = a->e;
      break;
    case WEST:
      res = a->w;
      break;
    default:
      res = NULL;
      break;
  }
  return res;
}

int max(int a, int b){
  if(a>b)
    return a;
  else
    return b;
}
int min(int a, int b){
  if(a<b)
    return a;
  else
    return b;
}

bool IsObjetInteressant(char objet){
  bool res = isEnemy(objet)||isBonus(objet)||(objet==EXIT)||(objet==BREAKABLE_WALL)||(objet==BOMB);
  return res;
}
bool isEnemy(char objet){
  bool res = (objet==FLAME_ENEMY)||(objet==GHOST_ENEMY);
  return res;
}
bool isBonus(char objet){
  bool res = (objet==BOMB_BONUS)||(objet==FLAME_BONUS);
  return res;
}
bool isWall(char objet){
  bool res = (objet==WALL)||(objet==BREAKABLE_WALL);
  return res;
}
bool isDanger(char objet){
  bool res = (objet==BOMB)||isEnemy(objet);
  return res;
}
bool isWalkable(char objet){
  bool res = (objet==PATH)||isBonus(objet)||(objet==EXIT);
  return res;
}
void AfficheListeObjet(maillonTriplet* liste){
  if(liste != NULL){
    triplet objet = liste->valeur;
    afficheListeAction(objet.chemin);
    printf("\n");
    AfficheListeObjet(liste->next);
  }
}
void afficheListeAction(maillonAction* liste){
  if(liste != NULL){
    printf("%d ", liste->valeur);
    afficheListeAction(liste->next);
  }
}
void afficheDistances(maillonTriplet* liste){
  if(liste!=NULL){
    printf("%d ",liste->valeur.distance);
    afficheDistances(liste->next);
  }
}
action controlable(int remaining_bombs){
  char e;
  action a;
  printf("Que veut tu faire : ");
  scanf(" %c", &e);

    switch(e) { // check whether the randomly selected action is valid, i.e., if its preconditions are satisfied 
    case 'b':
      a = BOMBING;
      break;
    case 'z':
      a = NORTH;
      break;
    case 'd':
      a = EAST;
      break;
    case 's':
      a = SOUTH;
      break;
    case 'q':
      a = WEST;
      break;
    }

  return a; // answer to the game engine
}