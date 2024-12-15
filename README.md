# Software Engineering Project - Wizard
Welcome to **Wizard**, a C++ implementation of the classic card game. Compete with your friends in exciting multiplayer 
gameplay!

This project is part of the *Software Engineering* lecture at *ETH Zurich* in AS24. By Marie Becker, Janne Berger, Martina
Lehman, Aidan Meara, Michel Tarnow, and Hannah Winter.

<div style="display: flex; align-items: center;">
  <img src="./assets/wizard_logo.png" alt="Wizard Logo" style="height: 150px; margin-right: 20px;">
  <img src="./assets/cards.png" alt="Cards" style="height: 75px;">
</div>

## 🚀 Features
- 🌐 **Multiplayer Gameplay:** Play with 3 to 6 players.
- 🔄 **Synchronized Game State:** Real-time updates for all players.
- 🖼 **Elegant User Interface:** A visually appealing UI powered by wxWidgets.

---

You can read the game's rules [here](https://blog.amigo-spiele.de/content/ap/rule/06900-GB-AmigoRule.pdf). 
The implementation features a client/server architecture for multiplayer scenarios. It uses [wxWidgets](https://www.wxwidgets.org/) 
for the GUI, [sockpp](https://github.com/fpagliughi/sockpp) for the network interface, [rapidjson](https://rapidjson.org/md_doc_tutorial.html) 
for object serialization, and [googletest](https://github.com/google/googletest) for the unit tests. 

---

## 1 Overview

The game and source files are available on GitLab on the main branch. The game was developed based on the provided 
[LAMA example project](https://gitlab.ethz.ch/hraymond/cse-lama-example-project) game. This project consists of a 
**server** and a **client**, each with their own main.cpp file. Each player can run their own client and connect to 
a server in the same local network.

---

## 2 Compile Instructions

This project only works on UNIX systems (Linux / macOS). We thus only explain how to compile and run the game on these 
systems. The following description was tested on Ubuntu 24.04 and on macOS Sequoia.

### 2.1 Prepare OS Environment

If your OS does not yet have git installed, install git by running `sudo apt-get install git` on Ubuntu or by 
running `xcode-select --install` on macOS (this installs git as well). The Wizard repository can then be cloned by 
running `git clone https://gitlab.ethz.ch/beckermar/wizard.git` (clone with HTTPS) or by running
`git clone git@gitlab.ethz.ch:beckermar/wizard.git` (clone with SSH) inside the directory that should contain the game.
Cloning the game as a first step also makes provided scripts for preparing the OS environment and for compiling the code
available.

#### 2.1.1 Ubuntu 24.04

The necessary packages and software can either be installed manually or by running the provided script. 

To use the provided script, run `bash scripts/prepare_ubuntu.sh` inside the **wizard** directory.

To manually prepare Ubuntu, execute the following commands:
1. `sudo apt-get update` (update package list)
2. `sudo apt-get install build-essential` (install software to build from source)
3. `sudo apt-get install libwxgtk3.2-dev` (install wxwidgets, use libwxgtk3.0-gtk3-dev on Ubuntu 20.04 / 22.04)
4. `sudo apt-get install cmake` (install cmake)
5. `sudo reboot` (reboot system)

#### 2.1.2 macOS Sequoia

The necessary packages and software can either be installed manually or by running the provided script.

To use the provided script, run `zsh scripts/prepare_macos.sh` inside the **wizard** directory.

To manually prepare macOS, execute the following commands: 
1. `/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"` (install homebrew)
2. `brew install cmake` (install cmake)
3. `brew install wxwidgets` (install wxwidgets)

### 2.2 Compile Code

Compiling the code creates executables for the client (Wizard-client) and for the server (Wizard-server).

#### 2.2.1 Ubuntu 24.04

Compiling the code can be done by running the provided script.

To use the provided script, run `bash scripts/compile_ubuntu.sh` inside the **wizard** directory.

#### 2.2.1 macOS Sequoia

Compiling the code can be done by running the provided script.

To use the provided script, run `zsh scripts/compile_macos.sh` inside the **wizard** directory.

---

## 3 Run the Game

After compiling the code, navigate into the **cmake-build-debug** directory inside the **wizard** directory. To start a 
server, run `./Wizard-server`. In new consoles, you can now start as many clients as you wish by running `./Wizard-client`.

---

## 4 Play the Game

To play the game, at least 3 players have to connect to the server. The maximum number of players per game is 6. Once at
least 3 players are connected to the server, the game can start.

## 5 Special Features
