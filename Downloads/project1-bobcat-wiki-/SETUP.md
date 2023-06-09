# Setting Up Linux Environment

This assignment will be evaluated with the following operating systems/tools.

- Ubuntu 22.04
- Clang 14
- Clang-Format 14

It is your responsibility that your code runs on the environment that we will use for grading. We provide the [development container](https://containers.dev/) configuration to easily set up the environment.

If you encounter any trouble while setting up the environment, feel free to reach out to any of the course staff.

## Using VS Code and DevContainer

The easiest way to work on the assignment is to use VS Code with the provided devcontainer configuration (the `.devcontainer` directory on this repo).

First, you need to install [Visual Studio Code](https://code.visualstudio.com/) and [Docker Desktop](https://www.docker.com/products/docker-desktop/).

Then, open VS Code and install [the devcontainer extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers).

Once the extension is installed, open this repository. Press `F1` to open the command palette and select `Dev Containers: Reopen in Container`. This might take a while for the first time as it downloads the dependencies.

When it is done, VS Code opens the project in a new window that is attached to the Linux container.

## Using Docker

If you prefer using editors other than VS Code, you can use the docker image (`.devcontainer/Dockerfile`).

## Using Virtual Machine

You can also set up a virtual machine. You'll need to create a virtual machine from the Ubuntu 22.04 base image and running commands listed in `.devcontainer/Dockerfile`.
