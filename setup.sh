#!/bin/sh
set -e # Fail at the first error
BASEDIR_ABS=$(cd "$(dirname "$0")" && pwd)
SIMULATORS_DIR="$BASEDIR_ABS/simulators"
echo "Fetching simulators in $SIMULATORS_DIR"

echo "Fetching cadmium"
CADMIUM_DIR="$SIMULATORS_DIR/cadmium"
if [ -d "$CADMIUM_DIR" ]; then
    cd "$CADMIUM_DIR"
    git pull
else
    cd $SIMULATORS_DIR
    git clone https://github.com/SimulationEverywhere/cadmium.git
fi

echo "Fetching cdboost"
CDBOOST_DIR="$SIMULATORS_DIR/cdboost"
if [ -d "$CDBOOST_DIR" ]; then
    cd "$CDBOOST_DIR"
    git pull
else
    cd $SIMULATORS_DIR
    git clone https://scm.gforge.inria.fr/anonscm/git/cdboost/cdboost.git
fi

#adevs does not have a git repository available
echo "Fetching adevs"
ADEVS_DIR="$SIMULATORS_DIR/adevs"
if [ -d "$ADEVS_DIR" ]; then
    cd "$ADEVS_DIR"
    git svn fetch
    git svn rebase
else
    cd $SIMULATORS_DIR
    git svn clone https://svn.code.sf.net/p/adevs/code/trunk adevs
fi
