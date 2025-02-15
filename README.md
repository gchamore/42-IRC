# 🏗️ ft_irc

## 🚀 Présentation

ft_irc est un projet visant à implémenter un **serveur IRC** en **C++ 98**.  
L’objectif est de comprendre le fonctionnement des protocoles de communication sur Internet et de mettre en place un serveur capable de gérer plusieurs connexions clients.  

Le serveur devra respecter les standards du protocole IRC et fonctionner avec un client IRC standard.

---

## 🛠️ Fonctionnalités

- **Gestion des connexions clients** en TCP/IP (v4 ou v6).
- **Authentification** des utilisateurs avec un mot de passe.
- **Gestion des canaux** :
  - Création et suppression de canaux.
  - Envoi et réception de messages publics et privés.
  - Attribution des rôles (utilisateur, opérateur).
- **Implémentation des commandes IRC obligatoires** :
  - `NICK`, `USER`, `JOIN`, `PART`, `QUIT`
  - `PRIVMSG` pour les messages privés.
  - `KICK`, `INVITE`, `TOPIC`, `MODE` pour la gestion des canaux.
- **Utilisation d’un seul `poll()` (ou équivalent) pour gérer toutes les opérations non bloquantes**.

---

## 📌 Technologies Utilisées

- **C++ 98**
- **Sockets TCP/IP**
- **poll() ou équivalent (select, epoll, kqueue)**
- **Gestion des descripteurs de fichiers non bloquants**

---

## 🏗️ Structure du Projet

📂 includes  
 ┣ 📜 Channel.hpp  
 ┣ 📜 Client.hpp  
 ┣ 📜 CommandParser.hpp  
 ┣ 📜 Constants.hpp  
 ┣ 📜 Debug.hpp  
 ┣ 📜 Server.hpp  
 ┣ 📜 ServerMessages.hpp  

📂 srcs  
 ┣ 📜 Channel.cpp  
 ┣ 📜 Client.cpp  
 ┣ 📜 CommandParser.cpp  
 ┣ 📜 Debug.cpp  
 ┣ 📜 OperatorCommands.cpp  
 ┣ 📜 Server.cpp  
 ┣ 📜 ServerCommands.cpp  
 ┣ 📜 main.cpp  

📂 tests  
 ┣ 📂 Errors  
 ┃ ┣ 📜 JOIN_errors.sh  
 ┃ ┣ 📜 MODE_errors.sh  
 ┃ ┣ 📜 NICK_errors.sh  
 ┃ ┣ 📜 PART_errors.sh  
 ┃ ┣ 📜 PASS_errors.sh  
 ┃ ┣ 📜 PRIVMSG_errors.sh  
 ┃ ┣ 📜 QUIT_errors.sh  
 ┃ ┣ 📜 USER_errors.sh  
 ┃ ┗ 📜 WHO_errors.sh  
 ┣ 📜 IncompleteCmd.sh  
 ┣ 📜 LowBandwidth.sh  
 ┣ 📜 ManyClients.sh  
 ┣ 📜 PartialDataReception.sh  
 ┣ 📜 run_all_error_tests.sh  

📜 .gitignore  
📜 Makefile  

---

## 🔥 Difficultés Rencontrées

- **Gestion des connexions simultanées** : Assurer que plusieurs clients peuvent interagir sans bloquer le serveur.  
- **Utilisation correcte de `poll()`** : Un seul appel pour gérer toutes les interactions (lecture, écriture, écoute).  
- **Respect des standards IRC** : Implémentation stricte des commandes et du format des messages.  
- **Interopérabilité** : Tester le serveur avec des clients IRC existants pour garantir son bon fonctionnement.  

---

## 🏗️ Mise en place

1. **Cloner le dépôt** :
   ```bash
   git clone https://github.com/ton-repo/ft_irc.git
   cd ft_irc
2. **Compiler le projet** :
   ```bash
   make
3. **Lancer le serveur** :
   ```bash
   ./ircserv <port> <password>
4. **Se connecter avec un client IRC (exemple avec netcat)** :
   ```bash
   nc 127.0.0.1 <port>
5. **Arrêter et nettoyer le projet** :
   ```bash
   make fclean

---

## 👨‍💻 Équipe  

👤 Antonin Ferre (Anferre) 
👤 Grégoire Chamorel (Gchamore)  

---

## 📜 Projet réalisé dans le cadre du cursus 42.  

Tu peux bien sûr modifier ce README selon tes besoins, ajouter plus de détails sur ton approche et des instructions d’installation précises. 🚀  
