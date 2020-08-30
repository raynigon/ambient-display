#!/bin/bash

log() {
    echo "[$(date)] [$1]: $2"
}

checkForBrew() {
    if ! hash brew 2>/dev/null; then
        log "ERROR" "Expected brew to be installed"
        exit 1
    fi
}

############################################################
###################### Execute Script ######################
############################################################
checkForBrew