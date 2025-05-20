#!/bin/bash
set -e

USER_UID=${USER_UID:-1000}
USER_GID=${USER_GID:-1000}
USER_NAME=${USER_NAME:-developer}

DOCKER_GID=${DOCKER_GID:?Variable not set or empty}

create_user() {
    # Remove default user with UID 1000
    userdel -r ubuntu

    # Create group
    groupadd -g ${USER_GID} ${USER_NAME}

    # Create user
    useradd -u ${USER_UID} -g ${USER_GID} -m ${USER_NAME}

    # Replicate docker group
    groupdel docker
    groupadd -o -g ${DOCKER_GID} docker

    usermod -aG docker ${USER_NAME}
}

create_user
sudo -HEu ${USER_NAME} "$@"
